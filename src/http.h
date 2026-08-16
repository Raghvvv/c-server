#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>

typedef struct{
    char method[6];
    char route[100];
} HttpRequest;


bool parseRequest(const char *buffer, HttpRequest *req);

#endif
