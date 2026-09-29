#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>

#define BUFSIZE 512

void write_str_to_buf(char **buffer, char *input) {
  while (*input != '\0') {
    **buffer = *input;
    (*buffer)++;
    input++;
  }
  (*buffer)--;
}

void write_int_to_buf(char **buffer, int val) {
  char intString[18] = {'\0'};
  snprintf(intString, sizeof(intString), "%d", val);
  write_str_to_buf(buffer, intString);
}

void write_char_to_buf(char **buffer, char input) {
  **buffer = input;
  (*buffer)++;
}

void write_level(char **buffer, const char *level) {
  **buffer = '[';
  (*buffer)++;
  while (*level != '\0') {
    **buffer = *level;
    (*buffer)++;
    level++;
  }
  **buffer = ']';
  (*buffer)++;
  **buffer = ' ';
  (*buffer)++;
}
void log_message(const char *level, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char *buf = malloc(BUFSIZE * sizeof(char));
  if (!buf) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  char *start = buf;

  // write the level here first
  write_level(&buf, level);

  while (*fmt != '\0') {
    if (*fmt != '%') {
      *buf = *fmt;
    } else {
      fmt++;
      switch (*fmt) {
      case 's':;
        char *strArg = va_arg(args, char *);
        write_str_to_buf(&buf, strArg);
        break;
      case 'd':;
        int argVal = va_arg(args, int);
        write_int_to_buf(&buf, argVal);
        break;
      case 'c':;
        char c = (char)va_arg(args, int);
        write_char_to_buf(&buf, c);
        break;
      case '%':
        write_char_to_buf(&buf, '%');
        break;
      }
    }
    buf++;
    fmt++;
  }

  *buf = '\n';
  buf++;
  *buf = '\0';
  printf("%s", start);
  free(start);

  va_end(args);
}

int main() {

  log_message("INFO", "User %s connected from %s", "Rawley", "192.168.1.20");
  log_message("WARN", "Port %d is already in use", 8080);
  log_message("ERROR", "Failed to open file %s", "/etc/passwd");

  return 0;
}
