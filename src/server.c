#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>


#define PORT 8080

int main(){

    int sock_fd,new_socket;
    struct sockaddr_in address;
    char *ptr="hello from the server\n";
    int buffer[1024]={0};
    int valread;
    
    //configure the address struct:
    
    address.sin_family=AF_INET;
    address.sin_port=htons(PORT);
    address.sin_addr.s_addr=INADDR_ANY;
    
    //create a socket:

    if((sock_fd=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("coud not create socket");
        exit(EXIT_FAILURE);
    }
    
    //bind the socket:
    
    if(bind(sock_fd,(struct sockaddr*)&address,sizeof(address))<0){
        perror("problem in binding stage");
        exit(EXIT_FAILURE);
    }
    
    //listen:
    
    if((listen(sock_fd,4))<0){
        perror("error in listen");
        exit(EXIT_FAILURE);
    }
    
    //accept:
    socklen_t adderlen=sizeof(address);
    
    if((new_socket=accept(sock_fd,(struct sockaddr*)&address,&adderlen))<0){
        perror("cant accept more");
        
        exit(EXIT_FAILURE);
        
    }
    

    
    
    send(new_socket,ptr,strlen(ptr),0);
    
    while((valread=read(new_socket,buffer,sizeof(buffer)-1))>0){
        buffer[valread]='\0';
        printf("%s",buffer);
        send(new_socket,"messege recieved\n",strlen("messege recieved\n"),0);
        
    }
    


    


    close(new_socket);
    close(sock_fd);

    return 0;
}