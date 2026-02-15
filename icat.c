// learning about signal interruptions. beginning with EINTR from errno.h library
// the project is the simple cat command which displays file content onto standard out/ screen
// difference is during the printing onto screen if a signal say, ctrl + c is pressed
// it should catch it and not stop the program, then it continues to print the file until EOF

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define ARGCOUNT 2
#define BUF_SIZE 1024


void sigint_handler(int sig){
	(void) sig;
	write(STDOUT_FILENO, "\n^C caught, continuing\n", 23);
}




int main(int argc, char **argv){
	if (argc != ARGCOUNT){
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	sigaction(SIGINT, &sa, NULL);

	if(sigaction(SIGINT, &sa, NULL) == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	
	char *filename = argv[1];

	int fd = open(filename, O_RDONLY, 0644);
	if (fd < 0){
		fprintf(stderr, "Could not open file: %s\n", filename);
		exit(EXIT_FAILURE);
	}

	char buf[BUF_SIZE];
	ssize_t n = 0;
	while(1){
		n = read(fd, buf, BUF_SIZE);

		if (n > 0){
			ssize_t off = 0;
			while (off < n){
				ssize_t w = write(STDOUT_FILENO, buf + off, n - off);
				if ( w < 0){
					if (errno == EINTR){
						continue;
					}
					perror("write");
					exit(EXIT_FAILURE);
				}
				off += w;
				usleep(100000);
			}
		} else if(n == 0){
			break; // EOF (End of File)
		} else {
			if (errno == EINTR){
				continue;
			}
			perror("read");
			exit(EXIT_FAILURE);
		}

	}

	if(n < 0){
		perror("Reading of buffer is weird");
		exit(EXIT_FAILURE);
	}
	
	if(close(fd) < 0){
		perror("Error closing file");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
