#include "../include/server.h"
#include "../include/debug.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

int main(void){
	pthread_t thread1;
	int id1 = 1;
	pthread_create(&thread1, NULL, serve, &id1);

	int sock;
	const char *hostname = "localhost";
	const char * port = "3333";
    struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname, port, &hints, &res) != 0) {
		perror("getaddrinfo failed.");
	}

	char ip[INET_ADDRSTRLEN];
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	if (inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip)) == NULL) {
		perror("inet_ntop failed;");
	}

    printf("Hostname: %s\n", hostname);
	d_print("IP :", GREEN);
	d_print(ip, GREEN);
	putc('\n', stdout);

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        perror("Socket creation failed");
        freeaddrinfo(res);
        return 1;
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Connection failed");
        close(sock);
        freeaddrinfo(res);
        return 1;
    }
    freeaddrinfo(res);

    const char *message = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (send(sock, message, strlen(message), 0) < 0) {
        perror("Send failed");
        close(sock);
        return 1;
    }
    char buffer[4096];
    int bytes_received;

    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        if (strcmp(buffer, "Hello from Server!\r\n") == 0) {
			d_print("[PASS]: read message from server.", GREEN);
			putc('\n', stdout);
		}
    }

    if (bytes_received < 0) {
        perror("Read failed");
    }

    close(sock);

	return 0;
}
