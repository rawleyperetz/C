#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARGCOUNT 2

int main(int argc, char **argv) {

  if (argc < ARGCOUNT) {
    fprintf(stderr, "Usage: %s <binary file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  pid_t rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed \n");
    exit(1);
  } else if (rc == 0) { // child process

    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
      perror("PTRACE_TRACEME");
      exit(EXIT_FAILURE);
    };

    if (execvp(argv[1], &argv[1]) == -1) {
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  }
  int status;
  pid_t childPID = waitpid(rc, &status, 0);
  if (childPID == -1) {
    fprintf(stderr, "waitpid failed\n");
    exit(EXIT_FAILURE);
  }

  bool entering = true; // bool value for checking whether we entering the
                        // kernel space i.e. before syscall and false otherwise
  struct user_regs_struct oldregs; // defined in the sys/user.h header file
  while (1) {
    if (WIFEXITED(status)) { // check whether tracee ends normally
      printf("Process ended with an exit code of %d\n", WEXITSTATUS(status));
      break;
    }

    if (WIFSIGNALED(status)) { // check whether tracee ends abnormally say
                               // SIGSTOP and its like
      printf("Process ended abnormally by signal: %d\n", WTERMSIG(status));
    }

    if (ptrace(PTRACE_SYSCALL, rc, NULL, NULL) == -1) { // find the syscalls
      perror("PTRACE_SYSCALL");
      exit(EXIT_FAILURE);
    }

    waitpid(rc, &status, 0);

    if (ptrace(PTRACE_GETREGS, rc, NULL, &oldregs) == -1) {
      perror("GETREGS");
      exit(EXIT_FAILURE);
    }
    if (entering) {
      printf("\nRIP = %#llx\n", oldregs.rip);
      printf("Orig_RAX = %#llx\n", oldregs.orig_rax);
      printf("RDI = %#llx\n", oldregs.rdi);
      printf("RSI = %#llx\n", oldregs.rsi);
      printf("RDX = %#llx\n", oldregs.rdx);
      printf("RCX = %#llx\n", oldregs.rcx);
      printf("R8 = %#llx\n", oldregs.r8);
      printf("R9 = %#llx \n\n", oldregs.r9);
    } else {
      printf("\nRAX = %#llx\n\n", oldregs.rax);
    }

    entering = !entering;
  }
  return 0;
}
