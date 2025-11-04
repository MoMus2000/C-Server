#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <stdbool.h>
#include <string.h>
#include "libs/picohttpparser.h"

#define DEFAULT_BACKLOG 128

uv_loop_t* loop;

void after_write(uv_write_t* req, int status){
  free(req);
}

void parse_http_request(char* request_message, int nread){

  const char *method;
  const char *path;
  size_t method_len;

  size_t path_len;
  int minor_version;
  struct phr_header headers[100];  // or however many you want
  size_t num_headers = 100;

  int result = phr_parse_request(request_message, (size_t)nread,
                              &method, &method_len,
                              &path, &path_len,
                              &minor_version,
                              headers, &num_headers,
                              0);

  printf("HTTP Version %d\n", minor_version);

}

void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf){
    if (nread > 0){
      char* response = malloc(nread+1);
      memset(response, '\0', nread+1);
      memcpy(response, buf->base, nread);
      printf("%s", response);
      parse_http_request(response, nread);
      free(response);
    }
}

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void on_new_connection(uv_stream_t* stream, int status){
  if(status < 0) {
    fprintf(stderr, "Connection Error: %s", uv_strerror(status));
    return;
  }

  uv_tcp_t* client = malloc(sizeof(uv_tcp_t));

  uv_tcp_init(loop, client);

  int accept = uv_accept(stream, (uv_stream_t*) client);

  if(accept == 0){
    printf("Client Connected\n");

    uv_read_start((uv_stream_t*)client, alloc_buffer, on_read);

    const char* message = "Hello From the Server\n";
    uv_buf_t buffer = uv_buf_init((char*) message, strlen(message));

    uv_write_t *req = malloc(sizeof(uv_write_t));

    uv_write(req, (uv_stream_t*) client, &buffer, 1, after_write);

  } else {
    uv_close((uv_handle_t*) client, NULL);
  }

}

int main(){
  loop = uv_default_loop();

  uv_tcp_t conn;

  struct sockaddr_in addr;

  uv_tcp_init(loop, &conn);

  uv_ip4_addr("0.0.0.0", 8080, &addr);

  uv_tcp_bind(&conn, (const struct sockaddr*)&addr, 0);

  int r = uv_listen((uv_stream_t*) &conn, DEFAULT_BACKLOG, on_new_connection);

  printf("Listening on 0.0.0.0:8080\n");

  if (r){
    fprintf(stderr, "Listen Error %s\n", uv_strerror(r));
    return 1;
  }

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}
