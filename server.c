#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define _OE_SOCKETS
#define SERVER_PORT 8080
#define MAX_LINES 4096
#define BACK_LOG 20

int main() {
  printf("SERVER STARTING\n");
  printf("----------------\n");

  struct sockaddr_in socketaddr;
  int listen_desc;

  if ((listen_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
    // TODO: Handle errors
  }

  bzero(&socketaddr, sizeof(socketaddr));
  socketaddr.sin_family = AF_INET;
  socketaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  socketaddr.sin_port = htons(SERVER_PORT);

  if (bind(listen_desc, (struct sockaddr *)&socketaddr, sizeof(socketaddr)) <
      0) {
    return 1;
    // TODO: Handle errors
  }

  if (listen(listen_desc, BACK_LOG) < 0) {
    return -1;
    // TODO: Handel errors
  }

  int conn_fd;
  uint8_t recived_line[MAX_LINES + 1];
  uint8_t buff[MAX_LINES + 1];
  int n;
  while (1) {
    struct sockaddr_in conn_sock;
    socklen_t conn_sock_len;

    fflush(stdout);
    conn_fd = accept(listen_desc, (struct sockaddr *)NULL, NULL);

    while ((n = read(conn_fd, recived_line, MAX_LINES - 1)) > 0) {
      fprintf(stdout, "%s\n", recived_line);

      // TODO: Find end of request better
      if (recived_line[n - 1] == '\n') {
        printf("hit\n");
        break;
      }
      memset(recived_line, 0, MAX_LINES);
    }
    if (n < 0) {
      return -1;
      // TODO: Handle errors
    }

    snprintf((char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello World");

    write(conn_fd, (char *)buff, strlen((char *)buff));
    close(conn_fd);
  }
}
