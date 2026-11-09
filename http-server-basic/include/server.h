#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PAGE_SIZE 4096

typedef enum {
  GET,
  PUT,
  POST,
  DELETE
} http_method;

typedef struct {
  int fd;
  http_method method;
  char header[PAGE_SIZE];
} connection;

typedef struct {
  char body[PAGE_SIZE*4];
  int length;
} payload;


void * handle_request(void * arg) ;
void * serve(void * ) ;
