#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "http.h"
#include "response.h"
#include "router.h"
#include "server1.h"

void serve_client(int socket)
{
        sleep(3);
        printf("Thread %lu started\n",
        (unsigned long)pthread_self());
        // sleep(5);
        char buffer[4096]={0};
        int new_socket=socket;
        // free(_socket);
        ssize_t byte=read(new_socket,buffer,sizeof(buffer)-1);

        if(byte <= 0){
            perror("read");
            close(new_socket);
            return;

        }
        buffer[byte]='\0';

        HttpRequest req;

        //extract method and route from buffer:

        if(!parseRequest(buffer,&req)){

            fprintf(stderr,"error in parsing!\n");
            // exit(EXIT_FAILURE);
        }

        //get the filepath:
        char filepath[100];
        resolvePath(&req,filepath);
        printf("%s\n",filepath);

        //get content-type:
        const char *content = getContentType(filepath);

        sendFileResponse(new_socket, filepath, content);

        printf("Thread %lu finished\n",
       (unsigned long)pthread_self());

        close(new_socket);

        return;
}
