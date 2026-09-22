#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EXPECTED_ARGC 2
#define BUFFERSIZE 4096

int main(int argc, char **argv) {
  // check command line args
  if (argc != EXPECTED_ARGC) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return 1;
  }

  // open file
  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  char buf[BUFFERSIZE];

  size_t cap = BUFFERSIZE;
  size_t len = 0;
  char *printBuf = malloc(cap * sizeof(char));
  if (!printBuf) {
    perror("malloc");
    close(fd);
    return 1;
  }
  char *start = printBuf;

  ssize_t bytesRead;

  int match = 0;
  while ((bytesRead = read(fd, buf, BUFFERSIZE)) > 0) {
    for (ssize_t i = 0; i < bytesRead; i++) {
      if (isprint((unsigned char)buf[i]) != 0) {
        match = 1;
        // here goes the capacity of dynamically allocated buf check
        goto ensure_space;
      } else if (match == 1) {
        goto ensure_space;
      }

      continue;

    ensure_space:
      if (len + 1 >= cap) {
        size_t offset = start - printBuf;
        cap *= 2;
        char *temp = realloc(printBuf, cap);
        if (!temp) {
          perror("Memory reallocation failed");
          free(printBuf);
          close(fd);
          return 1;
        }
        printBuf = temp;
        start = printBuf + offset;
      }

      if (isprint((unsigned char)buf[i]) != 0) {
        *start = buf[i];
        start++;
        len++;
      } else {
        *start = '\n';
        start++;
        len++;
        match = 0;
      }
    }
  }

  if (bytesRead < 0) {
    perror("read");
    free(printBuf);
    close(fd);
    return 1;
  }

  *start = '\0';
  printf("%s", printBuf);
  // print printBuf here
  free(printBuf);

  // close file
  if (close(fd) == -1) {
    perror("close");
    return 1;
  }

  return 0;
}
