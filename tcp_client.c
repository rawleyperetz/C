// a tcp client

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ARGCOUNT 3
#define BUFFERSIZE 256
#define PORT_MIN 1
#define PORT_MAX 65535

int main(int argc, char **argv) {

  if (argc != ARGCOUNT) {
    fprintf(stderr, "Usage: %s <remote ip addr> <port> ", argv[0]);
    return EXIT_FAILURE;
  }

  char *remote_ip = argv[1];

  char *port_number = argv[2];
  char *endptr;

  errno = 0; // resetting before strtol

  long port_long = strtol(port_number, &endptr, 10);
  // checking for digits
  if (endptr == port_number) {
    fprintf(stderr, "Port number is not numeric\n");
    return EXIT_FAILURE;
  }

  if (*endptr != '\0') {
    fprintf(stderr, "Port number contains invalid characters\n");
    return EXIT_FAILURE;
  }

  if (errno == ERANGE) {
    fprintf(stderr, "Port number out of range\n");
    return EXIT_FAILURE;
  }

  if (port_long < PORT_MIN || port_long > PORT_MAX) {
    fprintf(stderr, "Port must be between 1 and 65535\n");
    return EXIT_FAILURE;
  }

  int port_num = (int)port_long;

  // AF_INET = IPV4, SOCK_STREAM = TCP, 0 = let the OS choose the protocol (it
  // picks TCP)
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket"); // fprintf(stderr, "Error during creation of socket file
                      // descriptor\n");
    return EXIT_FAILURE;
  }

  // The following struct describes where you're connecting to
  struct sockaddr_in server_address;
  memset(&server_address, 0,
         sizeof(server_address)); // prevents garbage padding bytes

  server_address.sin_family = AF_INET;
  server_address.sin_port =
      htons(port_num); // host to network short (something about endianness)
  // server_address.sin_addr.s_addr = remote_ip; // INADDR_ANY;// Bind to any
  // local interface

  int ip_check = inet_pton(AF_INET, remote_ip, &server_address.sin_addr);
  if (ip_check == 0) {
    fprintf(stderr, "Ip Address is not valid\n");
    close(sockfd);
    return EXIT_FAILURE;
  }

  // performs the TCP three-way handshake (syn, syn-ack, ack)
  int connection_status = connect(sockfd, (struct sockaddr *)&server_address,
                                  sizeof(server_address));
  if (connection_status == -1) {
    perror("connect"); // fprintf(stderr, "connect failed: %s\n",
                       // strerror(errno));//Connection error to the remote
                       // socket'\n\n");
    close(sockfd);
    return EXIT_FAILURE;
  }

  // create epoll instance
  int epfd = epoll_create1(0);
  if (epfd == -1) {
    perror("epoll");
    close(sockfd);
    return EXIT_FAILURE;
  }

  // register stdin (0) and the socketfd
  struct epoll_event ev_sock;
  ev_sock.events = EPOLLIN;
  ev_sock.data.fd = sockfd;

  struct epoll_event ev_stdin;
  ev_stdin.events = EPOLLIN;
  ev_stdin.data.fd = STDIN_FILENO;

  int register_sock = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev_sock);
  if (register_sock == -1) {
    perror("sock register");
    close(sockfd);
    return EXIT_FAILURE;
  }

  int register_stdin = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev_stdin);
  if (register_stdin == -1) {
    perror("stdin register");
    close(sockfd);
    return EXIT_FAILURE;
  }

  struct epoll_event events[10];

  while (1) {
    int n = epoll_wait(epfd, events, 10, -1);

    for (int event_idx = 0; event_idx < n; event_idx++) {
      int fd = events[event_idx].data.fd;

      if (fd == sockfd) {
        char server_response[BUFFERSIZE];
        ssize_t responseBytes =
            recv(sockfd, server_response, BUFFERSIZE - 1, 0);
        if (responseBytes == -1) {
          perror("recv");
          // remove both fds from epoll.
          close(sockfd);
          return EXIT_FAILURE;
        }

        if (responseBytes == 0) {
          close(sockfd);
          return 0;
        }
        server_response[responseBytes] = '\0';
        printf("> \033[32m%s\033[0m\n", server_response);
      } else if (fd == STDIN_FILENO) {
        char msg[BUFFERSIZE];
        fgets(msg, BUFFERSIZE - 1, stdin);
        if (strcmp(msg, "/quit") == 0) {

          close(sockfd);
          return 0;
        }
        size_t lenMsg = strlen(msg);
        ssize_t msgBytes = send(sockfd, msg, lenMsg, 0);
        if (msgBytes == -1) {
          perror("server closed");
          close(sockfd);
          // remove both fds from epoll
          return EXIT_FAILURE;
        }
      }
    }
  }

  close(sockfd);

  return 0;
}
