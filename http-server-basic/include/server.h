#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

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


int talk_to_client(int connect_d) ;
void * serve() ;
