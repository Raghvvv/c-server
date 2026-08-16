#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "response.h"

void sendFileResponse(int socket, const char *filepath, const char *content)
{
    char html[8192];
    char header[512];
    const char *status[] = {"200 OK", "404 NOT FOUND"};
    const char *code;
    FILE *fptr = fopen(filepath, "r");

    if (fptr == NULL) {
        code = status[1];
        fptr = fopen("static/error404.html", "r");
    } else {
        code = status[0];
    }

    struct stat file_stat;
    int content_len = 0;
    if (stat(filepath, &file_stat) == 0) {
        content_len = file_stat.st_size;
    } else {
        perror("error reading file length\n");
        stat("static/error404.html", &file_stat);
        content_len = file_stat.st_size;
    }

    sprintf(
        header,
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lld\r\n"
        "\r\n",
        code,
        content,
        (long long)content_len
    );
    send(socket, header, strlen(header), 0);

    ssize_t byte_read;
    while ((byte_read = fread(html, 1, sizeof(html), fptr)) > 0) {
        send(socket, html, byte_read, 0);
    }

    fclose(fptr);
}
