#include "../include/server.h"
#include "../include/debug.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

int main(void){
    char debug_msg[4096];

    pthread_t thread1;
    pthread_create(&thread1, NULL, serve, NULL);

    int sock;
    const char *hostname = "localhost";
    const char * port = "3333";
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int stat = getaddrinfo(hostname, port, &hints, &res);
    if (stat != 0) {
        snprintf(debug_msg, sizeof(debug_msg), "getaddrinfo : %s\n", gai_strerror(stat));
        print_red(debug_msg);
    }

    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    if (inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip)) == NULL) {
	snprintf(debug_msg, sizeof(debug_msg), "inet_ntop : %s\n", strerror(errno));
        print_red(debug_msg);
    }

    printf("Hostname: %s\n", hostname);
    snprintf(debug_msg, sizeof(debug_msg), "[PASS]: IP : %s\n", ip);
    print_green(debug_msg);

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
	snprintf(debug_msg, sizeof(debug_msg), "socket : %s\n", strerror(errno));
        print_red(debug_msg);
        return 1;
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
	snprintf(debug_msg, sizeof(debug_msg), "connect : %s\n", strerror(errno));
        print_red(debug_msg);
        close(sock);
        freeaddrinfo(res);
        return 1;
    }
    freeaddrinfo(res);

    const char *message = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (send(sock, message, strlen(message), 0) < 0) {
	snprintf(debug_msg, sizeof(debug_msg), "send : %s\n", strerror(errno));
        print_red(debug_msg);
        close(sock);
        return 1;
    }
    char buffer[4096];
    int bytes_received;

    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        if (strcmp(buffer, "Hello from Server!\r\n") == 0) {
			print_green("[PASS]: read message from server.\n");
		}
    }

    if (bytes_received < 0) {
	snprintf(debug_msg, sizeof(debug_msg), "recv : %s\n", strerror(errno));
        print_red(debug_msg);
    }

    close(sock);

	return 0;
}
