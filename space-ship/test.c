#include <pthread.h>
#include <stdio.h>

int main() {
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}

