#ifndef RESPONSE_H
#define RESPONSE_H
#include "../http_request/http_request.h"
#include <stdlib.h>
#include <string.h>

struct Response {
  char *status;
  char *body;
  size_t size;
};

struct Response response_constructor(char *filename, struct Request request,
                                     char *status, int is_json);

#endif // RESPONSE_H
