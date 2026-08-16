
#include <stdbool.h>
#include <stdio.h>
#include "http.h"

bool parseRequest(const char *buffer, HttpRequest *req)
{
    return sscanf(buffer, "%s %s", req->method, req->route) > 0;
}
