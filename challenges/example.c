#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char queue_id[32];
    char span_id[32];
    char sender[128];

    char **recipients;
    int recipient_count;
    int recipient_capacity;

    char status[32];
    long long first_seen;
} Transaction;

typedef struct {
    Transaction *items;
    int count;
    int capacity;
} TransactionList;

long long parse_timestamp(char *iso) {
    struct tm tm = {0};
    sscanf(iso, "%4d-%2d-%2dT%2d:%2d:%2dZ",
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon  -= 1;
    return (long long)timegm(&tm);
}

char** tokenize(char* line, char delim) {
    char** tokens = calloc(32, sizeof(char*));
    char word[64];
    int t = 0, w = 0;
    for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        if (line[i] == delim) {
            if (w == 0) continue;
            word[w] = '\0';
            tokens[t++] = strdup(word);
            w = 0;
            continue;
        }
        word[w++] = line[i];
    }
    if (w > 0) {
        word[w] = '\0';
        tokens[t++] = strdup(word);
    }
    tokens[t] = NULL;
    return tokens;
}

void free_tokens(char** tokens) {
    for (int i = 0; tokens[i] != NULL; i++)
        free(tokens[i]);
    free(tokens);
}

Transaction* find_by_queue_id(TransactionList *tl, const char *qid) {
    for (int i = 0; i < tl->count; i++) {
        if (strcmp(tl->items[i].queue_id, qid) == 0)
            return &tl->items[i];
    }
    return NULL;
}

Transaction* find_by_span_id(TransactionList *tl, const char *sid) {
    for (int i = 0; i < tl->count; i++) {
        if (strcmp(tl->items[i].span_id, sid) == 0)
            return &tl->items[i];
    }
    return NULL;
}

void ensure_capacity(TransactionList *tl) {
    if (tl->count >= tl->capacity) {
        tl->capacity = tl->capacity ? tl->capacity * 2 : 16;
        tl->items = realloc(tl->items, tl->capacity * sizeof(Transaction));
    }
}

void add_recipient(Transaction *t, const char *rcpt) {
    if (t->recipient_count >= t->recipient_capacity) {
        t->recipient_capacity = t->recipient_capacity ? t->recipient_capacity * 2 : 4;
        t->recipients = realloc(t->recipients, t->recipient_capacity * sizeof(char*));
    }
    t->recipients[t->recipient_count++] = strdup(rcpt);
}

void init_transaction(Transaction *t, const char *qid, const char *sid,
                      const char *from, long long ts) {
    strncpy(t->queue_id, qid, sizeof(t->queue_id) - 1);
    t->queue_id[sizeof(t->queue_id) - 1] = '\0';
    strncpy(t->span_id, sid, sizeof(t->span_id) - 1);
    t->span_id[sizeof(t->span_id) - 1] = '\0';
    strncpy(t->sender, from, sizeof(t->sender) - 1);
    t->sender[sizeof(t->sender) - 1] = '\0';
    t->recipients = NULL;
    t->recipient_count = 0;
    t->recipient_capacity = 0;
    strcpy(t->status, "unknown");
    t->first_seen = ts;
}

void free_transaction(Transaction *t) {
    for (int i = 0; i < t->recipient_count; i++)
        free(t->recipients[i]);
    free(t->recipients);
}

void sort_by_timestamp(TransactionList *tl) {
    for (int i = 1; i < tl->count; i++) {
        Transaction key = tl->items[i];
        int j = i - 1;
        while (j >= 0 && tl->items[j].first_seen > key.first_seen) {
            tl->items[j + 1] = tl->items[j];
            j--;
        }
        tl->items[j + 1] = key;
    }
}

void print_report(TransactionList *tl) {
    sort_by_timestamp(tl);

    int total = tl->count;
    int success = 0, failed = 0, total_rcpt = 0;

    for (int i = 0; i < tl->count; i++) {
        Transaction *t = &tl->items[i];
        printf("Transaction %s\n", t->span_id);
        printf("  Sender: %s\n", t->sender);
        printf("  Recipients: %d\n", t->recipient_count);
        printf("  Status: %s\n\n", t->status);

        total_rcpt += t->recipient_count;
        if (strcmp(t->status, "success") == 0) success++;
        else if (strcmp(t->status, "failed") == 0) failed++;
    }

    printf("Total Transactions : %d\n", total);
    printf("Successful         : %d\n", success);
    printf("Failed             : %d\n", failed);
    printf("Recipients Total   : %d\n", total_rcpt);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;

    TransactionList tl = {0};
    tl.capacity = 16;
    tl.items = malloc(tl.capacity * sizeof(Transaction));

    while (getline(&line, &len, stdin) != -1) {
        if (line[0] == '\n') continue;

        char **tokens = tokenize(line, ' ');
        if (tokens[0] == NULL || tokens[0][0] == '\0') {
            free_tokens(tokens);
            continue;
        }

        long long ts = parse_timestamp(tokens[0]);

        if (strcmp(tokens[1], "QUEUE") == 0) {
            char **qid_kv  = tokenize(tokens[2], '=');
            char **span_kv = tokenize(tokens[3], '=');
            char **from_kv = tokenize(tokens[4], '=');

            ensure_capacity(&tl);
            init_transaction(&tl.items[tl.count],
                             qid_kv[1], span_kv[1], from_kv[1], ts);
            tl.count++;

            free_tokens(qid_kv);
            free_tokens(span_kv);
            free_tokens(from_kv);

        } else if (strcmp(tokens[1], "SMTP") == 0) {
            char **qid_kv = tokenize(tokens[2], '=');
            char **rcpt_kv = tokenize(tokens[3], '=');

            Transaction *t = find_by_queue_id(&tl, qid_kv[1]);
            if (t) {
                add_recipient(t, rcpt_kv[1]);
            } else {
                printf("WARNING: SMTP event references unknown queue id %s\n", qid_kv[1]);
            }

            free_tokens(qid_kv);
            free_tokens(rcpt_kv);

        } else if (strcmp(tokens[1], "DELIVERY") == 0) {
            char **span_kv  = tokenize(tokens[2], '=');
            char **stat_kv  = tokenize(tokens[3], '=');

            Transaction *t = find_by_span_id(&tl, span_kv[1]);
            if (t) {
                strncpy(t->status, stat_kv[1], sizeof(t->status) - 1);
                t->status[sizeof(t->status) - 1] = '\0';
            } else {
                printf("WARNING: DELIVERY event references unknown span %s\n", span_kv[1]);
            }

            free_tokens(span_kv);
            free_tokens(stat_kv);

        } else {
            printf("Invalid log line: %s", line);
        }

        free_tokens(tokens);
    }

    print_report(&tl);

    for (int i = 0; i < tl.count; i++)
        free_transaction(&tl.items[i]);
    free(tl.items);
    free(line);

    return 0;
}
