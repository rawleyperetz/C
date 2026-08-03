// #include <stdio.h>
#include <unistd.h>

int main(void) {
  // puts("Hello");
  write(STDOUT_FILENO, "Hello\n", 6);
  return 0;
}
