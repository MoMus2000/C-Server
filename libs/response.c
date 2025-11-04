#include <stdlib.h>
#include <string.h>

#include "response.h"

void http_response_init(http_response_t* response){
  response->capacity = 1024;
  char* buffer = malloc(response->capacity);
  response->buffer_size = 0;
}

void http_response_write(http_response_t* response, char* message){
  response->buffer_size = strlen(message);
  response->buffer = message;
}

