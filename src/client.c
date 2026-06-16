#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#define PORT 8080

int main(){
    int client_fd,status;
    struct sockaddr_in address;
    char *hi="hi from client";
    char buffer[1024]={0};

    address.sin_family=AF_INET;
    address.sin_port=htons(PORT);
    // address.sin_addr.s_addr=INADDR_ANY;

    //create a client socket:
    if((client_fd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("coudnt create socket\n");
        exit(EXIT_FAILURE);
    }

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    //connection request:
    if((status=connect(client_fd,(struct sockaddr*)&address,sizeof(address)))<0){
        perror("error in connecting\n");
        exit(EXIT_FAILURE);
        return -1;
    }

    send(client_fd,hi,strlen(hi),0);
    printf("messege sent!\n");

    read(client_fd,&buffer,1024-1);
    printf("%s",buffer);
    close(client_fd);

    return 0;
}