#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<unistd.h>


#define PORT 8080

int main(){

    int sock_fd,new_socket;
    struct sockaddr_in address;
    char *ptr="hello from the server\n";
    char buffer[4096]={0};
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
    while(1)
    {
        //accept:
        socklen_t adderlen=sizeof(address);
        
        if((new_socket=accept(sock_fd,(struct sockaddr*)&address,&adderlen))<0){
            perror("cant accept more");
            exit(EXIT_FAILURE);
            
        }

        
        ssize_t byte=read(new_socket,buffer,sizeof(buffer)-1);

        if(byte <= 0){
            perror("read");
            close(new_socket);  

        }
        buffer[byte]='\0';
        
        
        char method[6];
        char route[100];
        
        //extract method and route from buffer:

        if(sscanf(buffer,"%s %s",method,route)<=0){

            fprintf(stderr,"error in parsing!\n");
            exit(EXIT_FAILURE);
        }
        
        char filepath[100];
        char content[20];
        //get the filepath:
        char *ext=strrchr(route,'.');
        if(strcmp(route,"/")==0){
            strcpy(filepath,"static/index.html");
            strcpy(content,"html");
        }
        else if(ext&&strcmp(ext,".css")==0){
            sprintf(filepath,"static%s",route);
            strcpy(content,"text/css");
        }
         else if(ext&&strcmp(ext,".png")==0){
            sprintf(filepath,"static%s",route);
            strcpy(content,"img/png");
        }
         else if(ext&&strcmp(ext,".jpeg")==0){
            sprintf(filepath,"static%s",route);
            strcpy(content,"img/jpeg");
        }
        else{
            sprintf(filepath,"static%s.html",route);
            strcpy(content,"text/html");
        }
        printf("%s\n",filepath);
        char html[8192];
        char header[512];
        
        const char *status[]={"200 OK","404 NOT FOUND"};
        const char *code;

        //open the file:

        //read the html:

        FILE*fptr=fopen(filepath,"r");

        if(fptr==NULL){

            code = status[1];
            fptr=fopen("static/error404.html","r");
            
            
        }
        
        else{
            
            code=status[0];
        }
        struct stat file_stat;
        int content_len;
        if(stat(filepath,&file_stat)==0){
            content_len=file_stat.st_size;
        }
        else{
            perror("error reading file length\n");
            exit(EXIT_FAILURE);
        }
        sprintf(
            header,
            "HTTP/1.1 %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %zu\r\n"
            "\r\n",
            code,
            content,
            // strlen(html)
            content_len
        );
        send(new_socket,header,strlen(header),0);
        ssize_t byte_read;
        while((byte_read=fread(html,1,sizeof(html),fptr))>0)
        {      
            //create the header:
           
            send(new_socket,html,byte_read,0);
        }

        fclose(fptr);

        close(new_socket);
        printf("server still up");
    }

    printf("server down");
    close(sock_fd);

    return 0;
}