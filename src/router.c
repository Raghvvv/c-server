#include <stdio.h>
#include <string.h>

#include "router.h"

const char *getContentType(const char *filepath)
{
    const char *ext = strrchr(filepath, '.');

    if (ext && strcmp(ext, ".css") == 0) {
        return "text/css";
    }
    if (ext && strcmp(ext, ".png") == 0) {
        return "img/png";
    }
    if (ext && strcmp(ext, ".jpeg") == 0) {
        return "img/jpeg";
    }
    return "text/html";
}

void resolvePath(const HttpRequest *req, char *filepath)
{
    const char *ext = strrchr(req->route, '.');

    if (strcmp(req->route, "/") == 0) {
        strcpy(filepath, "static/index.html");
    } else if (ext && (strcmp(ext, ".css") == 0 ||
                       strcmp(ext, ".png") == 0 ||
                       strcmp(ext, ".jpeg") == 0)) {
        sprintf(filepath, "static%s", req->route);
    } else {
        sprintf(filepath, "static%s.html", req->route);
    }
}
