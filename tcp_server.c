// tcp server

#include <stdio.h>
#include <stdlib.h>

// #include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFERSIZE 256
// #define USERSCAP 10

bool remove_client_fd(int *client_fd_arr, int fd, int *count) {
  int i;
  for (i = 0; i < (*count) + 1; i++) {
    if (client_fd_arr[i] == fd) {
      (*count)--;
      client_fd_arr[i] = client_fd_arr[*count];
      return true;
    }
  }
  return false;
}

int main() {

  // char server_message[BUFFERSIZE] = "Hello Client, you have reached me";

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    fprintf(stderr, "Error during creation of socket file descriptor\n");
    return EXIT_FAILURE;
  };

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9002);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int bind_status =
      bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
  if (bind_status == -1) {
    fprintf(stderr, "Error: binding unsuccessful\n");
    close(sockfd);
    return EXIT_FAILURE;
  }

  int listen_status = listen(sockfd, 0);
  if (listen_status == -1) {
    perror("listen");
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

  // client_fd's
  int cap = 10;
  int *client_fd_arr = malloc(cap * sizeof(int));
  if (!client_fd_arr) {
    perror("malloc");
    close(sockfd);
    return EXIT_FAILURE;
  }

  int count = 0;
  // client_fd_arr[count] = sockfd;

  // register sockets
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;

  int register_sockfd_epoll = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
  if (register_sockfd_epoll == -1) {
    perror("epoll_ctl sockfd");
    return EXIT_FAILURE;
  }

  // did anything happen?
  struct epoll_event events[10];
  while (1) {
    // sleep until something happens
    int n = epoll_wait(epfd, events, 10, -1);

    // loop over all triggered events
    for (int event_idx = 0; event_idx < n; event_idx++) {
      int fd = events[event_idx].data.fd;

      // case if a new client is trying to connect
      if (fd == sockfd) { // sockfd is the listening socket
        int clientfd = accept(sockfd, NULL, NULL);
        if (clientfd == -1) {
          perror("accept");
          continue;
        }

        // register new client into epoll
        ev.events = EPOLLIN;
        ev.data.fd = clientfd;
        int register_status = epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
        if (register_status == -1) {
          perror("epoll_ctl add");
          continue;
          // skip this file descriptor
        }

        // to do: check limit before adding
        if (count == cap) {
          cap *= 2;
          int *tmp = realloc(client_fd_arr, cap * sizeof(*client_fd_arr));
          if (!tmp) {
            perror("Memory reallocation failed:");
            close(sockfd);
            return EXIT_FAILURE;
          } else {
            client_fd_arr = tmp;
          }
        }
        client_fd_arr[count++] = clientfd; // add to client fd array
      } else {
        char buffer[BUFFERSIZE];
        ssize_t bytes = read(fd, buffer, sizeof(buffer));

        if (bytes <= 0) {
          int del_status = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
          if (del_status == -1) {
            perror("epoll_ctl del");
            continue;
          }
          // to do: remove fd from client_fd_arr;
          bool clientRemove = remove_client_fd(client_fd_arr, fd, &count);
          if (clientRemove == false) {
            fprintf(stderr, "Could not remove client from client Arr\n");
          }
          close(fd);
        } else {
          // handle overflow
          if (bytes >= BUFFERSIZE) {
            bytes = BUFFERSIZE - 1;
          }
          buffer[bytes] = '\0';
          // handle message (broadcast)
          for (int client_idx = 0; client_idx < count; client_idx++) {
            if (client_fd_arr[client_idx] != fd) {
              ssize_t sendBytes =
                  send(client_fd_arr[client_idx], buffer, bytes, 0);
              // sending failure
              if (sendBytes == -1) {
                fprintf(stderr,
                        "Could not reach fd: %d :", client_fd_arr[client_idx]);
                perror("sending bytes error");
                int dead_fd = client_fd_arr[client_idx];
                bool clientRemove = remove_client_fd(
                    client_fd_arr, client_fd_arr[client_idx], &count);
                if (clientRemove == false) {
                  fprintf(stderr, "Could not remove client from client Arr\n");
                }
                int del_fd_epoll = epoll_ctl(epfd, EPOLL_CTL_DEL,
                                             client_fd_arr[client_idx], NULL);
                if (del_fd_epoll == -1) {
                  perror("epoll_ctl del");
                }
                close(dead_fd);
                client_idx--;
              }
            }
          }
        }
      }
    }
  }

  close(sockfd);
  return 0;
}
