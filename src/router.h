#ifndef ROUTER_H
#define ROUTER_H

#include "http.h"

const char *getContentType(const char *filepath);
void resolvePath(const HttpRequest *req, char *filepath);

#endif
