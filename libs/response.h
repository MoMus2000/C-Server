#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct{
  char* buffer;
  int buffer_size;
  int capacity;
} http_response_t ;

void http_response_init(http_response_t* response);
void http_response_write(http_response_t* response, char* message);

#endif
