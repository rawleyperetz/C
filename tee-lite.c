#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define ARGCOUNT 2
#define BUF_SIZE 4096

int main(int argc, char **argv){
	if(argc != ARGCOUNT){
		fprintf(stderr, "One command line argument needed\n");
		return 1;
	}

	char *filename = argv[1];

	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if(fd == -1){
		perror("File could not open");
		return 1;
	}

	char buf[BUF_SIZE];
	ssize_t n;

	while((n = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
		//write(STDOUT_FILENO, buf, n);

		//int write_val = write(fd, buf, n);				   
 		ssize_t off = 0;
		while(off < n){
			ssize_t w = write(fd, buf + off, n - off);
			if (w < 0){
				 perror("Error writing to file");
				 return 1;
			}
			off += w;
		}

		off = 0;
		while(off < n){
			ssize_t wStd = write(STDOUT_FILENO, buf + off, n - off);
			if (wStd < 0){
				perror("Error writing to std out");
				return 1;
			}
			off += wStd;
		}
		//if(write_val == -1){
			//perror("Error occurred during writing\n");
			//return 1;
		//}
	}

	if (n < 0){
		perror("Error occurred");
		return 1;
	}

	if(close(fd) < 0){
		perror("Error closing file");
	}

	return 0;
}
