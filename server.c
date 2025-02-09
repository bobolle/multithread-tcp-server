#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void *output(void *arg);

int main() {
    int server_fd;
    int client_fd;

    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;

    socklen_t client_len = sizeof(clientaddr);

    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("could not create socket\n");
    }

    // setup
    memset((char *)&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8080);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // wierd magic trick
    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

    // bind 
    if (bind(server_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        printf("could not bind socket\n");
    }

    // listen
    if (listen(server_fd, 16) < 0) {
        printf("could not listen to socket\n");
    }

    printf("server is running\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&clientaddr, &client_len);
        if (client_fd < 0) {
            printf("could not accept new connection\n");
        }

        // create 
        pthread_t child_tid;
        if (pthread_create(&child_tid, NULL, output, &client_fd) == 0) {
            pthread_detach(child_tid);
        } else {
            printf("thread creation failed\n");
        }

    }

    return 0;
}

void *output(void *arg) {
    int client_fd = *(int*)arg;

    char buffer[BUFFER_SIZE];

    while (1) {
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        printf("from client: %d: %s\n", client_fd, buffer);

        memset(buffer, 0, sizeof(buffer));
    }

    close(client_fd);
    return arg;
}
