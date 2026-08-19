#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <signal.h>

#define PORT            8999
#define MAX_EVENTS      1024
#define BACKLOG         4096
#define READ_BUF_SIZE   8192
#define WRITE_BUF_SIZE  8192
#define KEEPALIVE_TIMEOUT_SEC 65
#define CLIENT_TIMEOUT_SEC    30

/* ======================================================================
 *  CONNECTION STATE MACHINE
 *
 *  Each connection goes through states driven by epoll events:
 *
 *      accept()
 *         │
 *         ▼
 *    ┌─────────┐  EPOLLIN   ┌────────────┐
 *    │ READING │ ─────────▶ │ PROCESSING │
 *    └─────────┘            └────────────┘
 *         │                       │
 *         │ EPOLLRDHUP/HUP        │ build response
 *         │ or read error         │
 *         ▼                       ▼
 *    ┌───────┐              ┌──────────┐
 *    │ CLOSE │              │ WRITING  │
 *    └───────┘              └──────────┘
 *                                │
 *                         write complete?
 *                          yes │      │ no
 *                              ▼      ▼
 *                        ┌──────────┐  (stay in WRITING,
 *                        │KEEPALIVE │   wait for EPOLLOUT)
 *                        └──────────┘
 *                              │
 *                    timeout or next request
 *                              ▼
 *                        ┌───────┐
 *                        │ CLOSE │
 *                        └───────┘
 *
 * ====================================================================== */

enum conn_state {
    STATE_READING,
    STATE_PROCESSING,
    STATE_WRITING,
    STATE_KEEPALIVE,
    STATE_CLOSE
};

static volatile sig_atomic_t running = 1;

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

/* ---- buffer chain (like nginx ngx_buf_t) ----
 *
 *  nginx uses a linked list of buffers so it can mix
 *  memory buffers, file buffers, and special "flush" markers.
 *  We simplify to memory-only but keep the chain pattern.
 */
typedef struct buffer {
    char           *pos;       /* current read position in data */
    char           *last;      /* one past last valid byte */
    int             in_file;   /* 1 = this is a file buffer (sendfile) */
    int             file_fd;   /* fd for sendfile */
    off_t           file_off;  /* offset into file */
    struct buffer  *next;      /* next buffer in chain */
} buffer_t;

/* ---- per-connection structure ---- */
typedef struct connection {
    int              fd;
    enum conn_state  state;

    /* input */
    char             read_buf[READ_BUF_SIZE];
    ssize_t          read_count;     /* bytes in read_buf */

    /* output chain: linked list of buffers to send */
    buffer_t        *out_chain;
    buffer_t        *out_chain_tail;
    ssize_t          out_total;      /* total bytes to send across chain */
    ssize_t          out_sent;       /* bytes already sent */

    /* timeout tracking */
    time_t           last_active;

    /* stats */
    unsigned long    requests_served;
} connection_t;

/* ---- worker context ---- */
typedef struct worker {
    int              id;
    int              server_fd;
    int              epfd;
    connection_t   **conns;          /* fd-indexed array */
    int              max_fd;
    time_t           now;            /* cached current time per event cycle */
} worker_t;

/* ======================================================================
 *  BUFFER CHAIN OPERATIONS
 * ====================================================================== */

static void chain_free(buffer_t *chain) {
    while (chain) {
        buffer_t *next = chain->next;
        free(chain);
        chain = next;
    }
}

/* Add a memory buffer to the tail of the chain */
static int chain_add_mem(worker_t *w, connection_t *c,
                         const char *data, size_t len) {
    buffer_t *b = malloc(sizeof(*b));
    if (!b) return -1;

    char *copy = malloc(len);
    if (!copy) { free(b); return -1; }

    memcpy(copy, data, len);

    b->pos     = copy;
    b->last    = copy + len;
    b->in_file = 0;
    b->file_fd = -1;
    b->file_off = 0;
    b->next    = NULL;

    if (c->out_chain_tail) {
        c->out_chain_tail->next = b;
    } else {
        c->out_chain = b;
    }
    c->out_chain_tail = b;
    c->out_total += (ssize_t)len;

    return 0;
}

/* Add a file buffer for sendfile (zero-copy) */
static int chain_add_file(worker_t *w, connection_t *c,
                           int file_fd, off_t offset, size_t len) {
    buffer_t *b = malloc(sizeof(*b));
    if (!b) return -1;

    b->pos      = NULL;
    b->last     = NULL;
    b->in_file  = 1;
    b->file_fd  = file_fd;
    b->file_off = offset;
    b->next     = NULL;

    if (c->out_chain_tail) {
        c->out_chain_tail->next = b;
    } else {
        c->out_chain = b;
    }
    c->out_chain_tail = b;
    c->out_total += (ssize_t)len;

    return 0;
}

/* ======================================================================
 *  CONNECTION LIFECYCLE
 * ====================================================================== */

static void conn_close(worker_t *w, connection_t *c) {
    epoll_ctl(w->epfd, EPOLL_CTL_DEL, c->fd, NULL);
    close(c->fd);
    chain_free(c->out_chain);
    w->conns[c->fd] = NULL;
    free(c);
}

static connection_t *conn_accept(worker_t *w) {
    int fd;
    while (1) {
        fd = accept(w->server_fd, NULL, NULL);
        if (fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return NULL;
            perror("accept");
            return NULL;
        }
        break;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(fd);
        return NULL;
    }

    if (fd >= w->max_fd) {
        int new_max = fd + 256;
        connection_t **tmp = realloc(w->conns, new_max * sizeof(connection_t *));
        if (!tmp) { close(fd); return NULL; }
        for (int i = w->max_fd; i < new_max; i++) tmp[i] = NULL;
        w->conns  = tmp;
        w->max_fd = new_max;
    }

    connection_t *c = calloc(1, sizeof(connection_t));
    if (!c) { close(fd); return NULL; }

    c->fd           = fd;
    c->state        = STATE_READING;
    c->last_active  = w->now;
    w->conns[fd]    = c;

    struct epoll_event ev = {
        .events  = EPOLLIN | EPOLLET | EPOLLRDHUP,
        .data.fd = fd
    };
    epoll_ctl(w->epfd, EPOLL_CTL_ADD, fd, &ev);

    return c;
}

/* ======================================================================
 *  STATE HANDLERS
 * ====================================================================== */

/*
 * READING: drain all available data from the socket into read_buf.
 *          On complete read (EAGAIN), transition to PROCESSING.
 *          On peer disconnect or error, close.
 */
static void handle_reading(worker_t *w, connection_t *c) {
    while (1) {
        ssize_t space = (ssize_t)sizeof(c->read_buf) - c->read_count;
        if (space <= 0) break;

        ssize_t n = read(c->fd, c->read_buf + c->read_count, space);
        if (n > 0) {
            c->read_count += n;
        } else if (n == 0) {
            /* peer closed */
            conn_close(w, c);
            return;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            /* real error */
            conn_close(w, c);
            return;
        }
    }

    /* all available data read — move to processing */
    c->state = STATE_PROCESSING;
}

/*
 * PROCESSING: the "business logic" phase.
 *             In a real server this would be HTTP parsing, file lookup, etc.
 *             Here we just echo back a fixed response.
 *             Build output chain, then transition to WRITING.
 */
static void handle_processing(worker_t *w, connection_t *c) {
    /* pretend we processed the input and built a response */
    const char *body = "Hello from state machine server!\n";
    char header[256];
    int hlen = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "Connection: keep-alive\r\n"
        "Keep-Alive: timeout=%d\r\n"
        "\r\n",
        (int)strlen(body), KEEPALIVE_TIMEOUT_SEC);

    /* reset output tracking */
    chain_free(c->out_chain);
    c->out_chain      = NULL;
    c->out_chain_tail = NULL;
    c->out_total      = 0;
    c->out_sent       = 0;

    /* build output chain: header buffer → body buffer */
    if (chain_add_mem(w, c, header, hlen) < 0 ||
        chain_add_mem(w, c, body, strlen(body)) < 0) {
        conn_close(w, c);
        return;
    }

    c->state = STATE_WRITING;
}

/*
 * WRITING: send as much of the output chain as the kernel accepts.
 *          Track partial writes across buffers.
 *          If all sent, transition to KEEPALIVE.
 *          If EAGAIN, register EPOLLOUT and stay in WRITING.
 *          On error, close.
 */
static void handle_writing(worker_t *w, connection_t *c) {
    while (c->out_chain && c->out_sent < c->out_total) {
        buffer_t *buf = c->out_chain;

        if (buf->in_file) {
            /* sendfile path — zero-copy file→socket */
            ssize_t remain = c->out_total - c->out_sent;
            ssize_t n = sendfile(c->fd, buf->file_fd, &buf->file_off, remain);
            if (n > 0) {
                c->out_sent += n;
            } else if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    /* register for EPOLLOUT, come back later */
                    struct epoll_event ev = {
                        .events  = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP,
                        .data.fd = c->fd
                    };
                    epoll_ctl(w->epfd, EPOLL_CTL_MOD, c->fd, &ev);
                    return;
                }
                conn_close(w, c);
                return;
            }
        } else {
            /* memory buffer path */
            ssize_t avail = buf->last - buf->pos;
            ssize_t n = write(c->fd, buf->pos, avail);
            if (n > 0) {
                c->out_sent += n;
                if (n < avail) {
                    /* partial write within this buffer — advance pos */
                    buf->pos += n;
                } else {
                    /* full buffer sent — move to next */
                    c->out_chain = buf->next;
                    if (!c->out_chain) c->out_chain_tail = NULL;
                    free(buf->pos);
                    free(buf);
                }
            } else if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    struct epoll_event ev = {
                        .events  = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP,
                        .data.fd = c->fd
                    };
                    epoll_ctl(w->epfd, EPOLL_CTL_MOD, c->fd, &ev);
                    return;
                }
                conn_close(w, c);
                return;
            }
        }
    }

    /* all sent — clear EPOLLOUT, go to keepalive */
    {
        struct epoll_event ev = {
            .events  = EPOLLIN | EPOLLET | EPOLLRDHUP,
            .data.fd = c->fd
        };
        epoll_ctl(w->epfd, EPOLL_CTL_MOD, c->fd, &ev);
    }

    c->state      = STATE_KEEPALIVE;
    c->last_active = w->now;
    c->requests_served++;
}

/*
 * KEEPALIVE: connection is idle, waiting for next request.
 *             If EPOLLIN fires → back to READING for next request.
 *             If timeout → close.
 */
static void handle_keepalive(worker_t *w, connection_t *c) {
    /* on EPOLLIN, a new request has arrived */
    c->read_count = 0;  /* reset read buffer for next request */
    c->state = STATE_READING;
}

/* ======================================================================
 *  TIMER: close connections that have been idle too long
 * ====================================================================== */

static void check_timeouts(worker_t *w) {
    for (int fd = 0; fd < w->max_fd; fd++) {
        connection_t *c = w->conns[fd];
        if (!c) continue;

        time_t timeout;
        if (c->state == STATE_KEEPALIVE)
            timeout = KEEPALIVE_TIMEOUT_SEC;
        else
            timeout = CLIENT_TIMEOUT_SEC;

        if (w->now - c->last_active >= timeout) {
            conn_close(w, c);
        }
    }
}

/* ======================================================================
 *  WORKER THREAD
 * ====================================================================== */

struct worker_args {
    int id;
    int server_fd;
};

static void *worker_main(void *arg) {
    struct worker_args *wa = arg;
    int server_fd = wa->server_fd;
    worker_t w = {0};
    w.id        = wa->id;
    w.server_fd = server_fd;
    free(wa);

    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1 || fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("worker fcntl");
        return NULL;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("worker listen");
        return NULL;
    }

    w.epfd = epoll_create1(0);
    if (w.epfd < 0) { perror("worker epoll_create1"); return NULL; }

    {
        struct epoll_event ev = {
            .events  = EPOLLIN | EPOLLET,
            .data.fd = server_fd
        };
        epoll_ctl(w.epfd, EPOLL_CTL_ADD, server_fd, &ev);
    }

    w.max_fd = 256;
    w.conns  = calloc(w.max_fd, sizeof(connection_t *));
    if (!w.conns) { close(w.epfd); return NULL; }

    struct epoll_event *events = calloc(MAX_EVENTS, sizeof(*events));
    if (!events) { free(w.conns); close(w.epfd); return NULL; }

    printf("[worker %d] ready\n", w.id);

    while (running) {
        w.now = time(NULL);

        int n = epoll_wait(w.epfd, events, MAX_EVENTS, 1000);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("worker epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            uint32_t ev_mask = events[i].events;

            if (fd == server_fd) {
                /* drain all pending accepts */
                while (conn_accept(&w) != NULL)
                    ;
                continue;
            }

            connection_t *c = w.conns[fd];
            if (!c) continue;

            /* error or hangup on any state → close */
            if (ev_mask & (EPOLLHUP | EPOLLERR)) {
                conn_close(&w, c);
                continue;
            }

            /* remote disconnected */
            if (ev_mask & EPOLLRDHUP) {
                conn_close(&w, c);
                continue;
            }

            switch (c->state) {

            case STATE_READING:
                if (ev_mask & EPOLLIN) {
                    handle_reading(&w, c);
                    if (!w.conns[fd]) continue; /* closed */
                }
                /* if we just moved to PROCESSING, fall through */
                if (c && c->state == STATE_PROCESSING)
                    handle_processing(&w, c);
                if (!w.conns[fd]) continue;
                if (c && c->state == STATE_WRITING)
                    handle_writing(&w, c);
                break;

            case STATE_PROCESSING:
                handle_processing(&w, c);
                if (!w.conns[fd]) continue;
                if (c->state == STATE_WRITING)
                    handle_writing(&w, c);
                break;

            case STATE_WRITING:
                if (ev_mask & EPOLLOUT) {
                    handle_writing(&w, c);
                }
                break;

            case STATE_KEEPALIVE:
                if (ev_mask & EPOLLIN) {
                    handle_keepalive(&w, c);
                    /* now in READING — handle it this same cycle */
                    handle_reading(&w, c);
                    if (!w.conns[fd]) continue;
                    if (c->state == STATE_PROCESSING)
                        handle_processing(&w, c);
                    if (!w.conns[fd]) continue;
                    if (c->state == STATE_WRITING)
                        handle_writing(&w, c);
                }
                break;

            default:
                break;
            }
        }

        /* check idle connections */
        check_timeouts(&w);
    }

    /* cleanup all remaining connections */
    for (int fd = 0; fd < w.max_fd; fd++) {
        if (w.conns[fd]) conn_close(&w, w.conns[fd]);
    }

    free(w.conns);
    free(events);
    close(w.epfd);
    printf("[worker %d] shut down\n", w.id);
    return NULL;
}

/* ======================================================================
 *  MAIN
 * ====================================================================== */

static int create_and_bind(int port) {
    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port        = htons(port),
    };

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return -1; }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(fd); return -1;
    }
    return fd;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGPIPE, SIG_IGN);

    long ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    if (ncpu < 1) ncpu = 4;
    int nworkers = (int)ncpu;

    printf("Starting %d workers on port %d\n", nworkers, PORT);

    pthread_t *threads = calloc(nworkers, sizeof(pthread_t));
    if (!threads) { perror("calloc"); exit(EXIT_FAILURE); }

    for (int i = 0; i < nworkers; i++) {
        int server_fd = create_and_bind(PORT);
        if (server_fd < 0) exit(EXIT_FAILURE);

        struct worker_args *wa = malloc(sizeof(*wa));
        if (!wa) { perror("malloc"); exit(EXIT_FAILURE); }
        wa->id = i;
        wa->server_fd = server_fd;

        if (pthread_create(&threads[i], NULL, worker_main, wa) != 0) {
            perror("pthread_create");
            free(wa); close(server_fd); exit(EXIT_FAILURE);
        }
    }

    printf("Server running. Press Ctrl+C to stop.\n");

    for (int i = 0; i < nworkers; i++)
        pthread_join(threads[i], NULL);

    free(threads);
    printf("Server stopped.\n");
    return 0;
}
