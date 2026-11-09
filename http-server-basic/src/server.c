#include "../include/server.h"
#include "../include/debug.h"

void * handle_request(void * arg) {

  int connect_d = (int)(intptr_t)arg;

  char * msg = "Hello from Server!\r\n";

  if (connect_d == -1) {
    perror("can't open connect socket!");
  }

  char chunk[PAGE_SIZE];
  ssize_t bytes = read(connect_d, chunk, sizeof(chunk) - 1);
  if(bytes <= 0) {
    perror("can't read message!");
  }
  chunk[bytes] = '\0';

  printf("%s\n", chunk);

  if (send(connect_d, msg, strlen(msg), 0) == -1) {
    perror("can't send message!");
    return NULL;
  }

  close(connect_d);

  return 0;
}

void *serve(void * arg) {
  pthread_t thread;

  int listener_d = socket(PF_INET, SOCK_STREAM, 0);

  if ( listener_d == -1) {
    perror("can't open socket!");
    exit(-1);
  }

  struct sockaddr_in name;
  memset(&name, 0, sizeof(name));
  name.sin_family = PF_INET;
  name.sin_port = (in_port_t)htons(3333);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_true = 1;

  if (setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, (char *)&opt_true, sizeof(int)) == -1) {
    perror("Can't set the reuse option on the socket");
    exit(-1);
  }

  int c = bind(listener_d, (struct sockaddr *) &name, sizeof(name));

  if ( c == -1) {
    perror("can't bind port to socket!");
    exit(-1);
  }

  if (listen(listener_d, 10) == -1) {
    perror("can't listen!");
    exit(-1);
  }

  struct sockaddr_storage client_addr;
  unsigned int address_size = sizeof(client_addr);

  while (1) {
    int connect_d = accept(listener_d, (struct sockaddr *) &client_addr, &address_size);
    if (connect_d == -1) {
      perror("accept");
      continue;
    }

    int thread_res = pthread_create(&thread, NULL, handle_request, (void *)(intptr_t)connect_d);
    if (thread_res != 0) {
        errno = thread_res;
        perror("pthread_create");
        close(connect_d);
        continue;
    }
    pthread_detach(thread);
  }

  return 0;
}
