#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
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
  int serv_sock_fd;

  if ((serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
    // TODO: Handle errors
  }

  bzero(&socketaddr, sizeof(socketaddr));
  socketaddr.sin_family = AF_INET;
  socketaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  socketaddr.sin_port = htons(SERVER_PORT);

  if (bind(serv_sock_fd, (struct sockaddr *)&socketaddr, sizeof(socketaddr)) <
      0) {
    return 1;
    // TODO: Handle errors
  }

  if (listen(serv_sock_fd, BACK_LOG) < 0) {
    return -1;
    // TODO: Handel errors
  }

  FILE *fileptr;
  int conn_fd;
  uint8_t recived_line[MAX_LINES + 1];
  uint8_t buff[MAX_LINES + 1];
  int n;
  char address[40];

  printf("SERVER RUNNING\n");
  printf("----------------\n");
  while (1) {
    struct sockaddr_in conn_sock;
    socklen_t conn_sock_len;

    fflush(stdout);
    conn_fd = accept(serv_sock_fd, (struct sockaddr *)NULL, NULL);

    while ((n = read(conn_fd, recived_line, MAX_LINES - 1)) > 0) {
      fprintf(stdout, "%s\n", recived_line);

      int i = 0;
      int j = 0;
      bool reading_address = false;
      memset(address, 0, 40);
      while (recived_line[i] != '\n') {
        printf("%c", recived_line[i]);
        if (reading_address) {
          address[j] = recived_line[i];
          i++;
          j++;
          if (recived_line[i] == ' ') {
            printf("\n");
            break;
          }
        } else {
          if (recived_line[i] == ' ') {
            reading_address = true;
          }
          i++;
        }
      }
      address[j] = '\0';
      printf("Address: %s\n", address);

      // TODO: Find end of request better
      if (recived_line[n - 1] == '\n') {
        break;
      }
      memset(recived_line, 0, MAX_LINES);
    }
    if (n < 0) {
      return -1;
      // TODO: Handle errors
    }

    if (strcmp(address, "/") == 0) {
      fileptr = fopen("index.html", "r");
    } else if (strcmp(address, "/page2") == 0) {
      fileptr = fopen("page2.html", "r");
    } else {
      snprintf((char *)buff, sizeof(buff), "HTTP/1.0 404 NOT FOUND\r\n");
      write(conn_fd, (char *)buff, strlen((char *)buff));
      close(conn_fd);
      continue;
    }
    char html[MAX_LINES - 45];
    char line[100];
    while (true) {
      fgets(line, 100, fileptr);
      strcat(html, line);
      if (feof(fileptr))
        break;
    }
    fclose(fileptr);
    printf("Sent:\n%s\n", html);
    memset(buff, 0, MAX_LINES);
    snprintf((char *)buff, sizeof(buff),
             "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n%s\n", html);

    memset(html, 0, MAX_LINES - 45);
    memset(line, 0, 100);
    write(conn_fd, (char *)buff, strlen((char *)buff));
    close(conn_fd);
  }
}
