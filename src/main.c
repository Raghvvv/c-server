#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "queue.h"
#include "worker.h"

#define PORT 8080

int main(void)
{
    int sock_fd, new_socket;
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("coud not create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("problem in binding stage");
        exit(EXIT_FAILURE);
    }

    if (listen(sock_fd, 4) < 0) {
        perror("error in listen");
        exit(EXIT_FAILURE);
    }

    pthread_t tid;
    Myqueue *queue = queueInit(64);

    pthread_create(&tid, NULL, worker, queue);
    pthread_detach(tid);
    pthread_create(&tid, NULL, worker, queue);
    pthread_detach(tid);
    pthread_create(&tid, NULL, worker, queue);
    pthread_detach(tid);
    pthread_create(&tid, NULL, worker, queue);
    pthread_detach(tid);

    while (1) {
        socklen_t address_len = sizeof(address);

        if ((new_socket = accept(sock_fd, (struct sockaddr *)&address,
                                 &address_len)) < 0) {
            perror("cant accept more");
            exit(EXIT_FAILURE);
        }

        enqueue(queue, new_socket);
        printf("server still up\n");
    }

    close(sock_fd);
    return 0;
}
