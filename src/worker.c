#include "queue.h"
#include "server1.h"

void *worker(void *arg)
{
    Myqueue *queue = (Myqueue *)arg;

    while (1) {
        int socket = dequeue(queue);
        serve_client(socket);
    }

    return NULL;
}
