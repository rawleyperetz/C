
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>

#define ARGCOUNT 4
#define PORT_MIN 1
#define PORT_MAX 65535
#define BUFFERSIZE 1024

int main(int argc, char **argv){

	if(argc != ARGCOUNT){
		fprintf(stderr, "Usage: %s <port_num> <remote ip> <dest file>\n",argv[0]);
		return 1;
	}

	char *port_number = argv[1];
	//char *shell = argv[2];

	// port_number cleansing
	char *endptr;
	
	errno = 0; //resetting before strtol
		
	long port_long = strtol(port_number, &endptr, 10);
		// checking for digits
	if(endptr == port_number){
		fprintf(stderr, "Port number is not numeric\n");
		return EXIT_FAILURE;
	}
	
	if(*endptr != '\0'){
		fprintf(stderr, "Port number contains invalid characters\n");
		return EXIT_FAILURE;
	}
	
	if(errno == ERANGE){
		fprintf(stderr, "Port number out of range\n");
		return EXIT_FAILURE;
	}
	
	if(port_long < PORT_MIN || port_long > PORT_MAX){
		fprintf(stderr, "Port must be between 1 and 65535\n");
    	return EXIT_FAILURE;
	}
	
	int port_num = (int) port_long;
		
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM,0);
	if(socket_fd < 0){
		//fprintf(stderr, "Error opening socket: %s", strerror(errno));
		perror("socket");
		return 1;
	}

	int opt = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port_num);
	server_address.sin_addr.s_addr = INADDR_ANY;


    int ip_check = inet_pton(AF_INET, argv[2], &server_address.sin_addr);
    if(ip_check == 0){
    	fprintf(stderr, "Ip Address is not valid\n");
    	close(socket_fd);
    	return EXIT_FAILURE;
    }

    int connection_status = connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if(connection_status == -1){
        perror("connect"); //fprintf(stderr, "connect failed: %s\n", strerror(errno));//Connection error to the remote socket'\n\n");
        close(socket_fd);
        return EXIT_FAILURE;
    }



	int file_fd = open(argv[3], O_WRONLY | O_CREAT, 0644);
	if(file_fd == -1){
		perror("File");
		close(socket_fd);
		//close(connection_fd);
		return EXIT_FAILURE;
	}

	
	char buffer[BUFFERSIZE];
	ssize_t bytes, ret;
	while ((bytes = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0) {
		ssize_t total_written = 0;
		while(total_written < bytes){
			ret = write(file_fd, buffer + total_written, bytes - total_written);
    		if(ret == -1){
    			if(errno == EINTR){
    				continue;
    			}
    			perror("write");
    			break;
    		}
    	total_written += ret;
    	}
	}

	if(bytes == 0){
		printf("File transfer was successful\n");
		if(fdatasync(file_fd) == -1){
			perror("File not written to disk");	
		}
		close(file_fd);
		close(socket_fd);
		// close(connection_fd);
		return EXIT_SUCCESS;
	}

	if(bytes < 0){
		perror("Error during file transfer");
		close(file_fd);
		close(socket_fd);
		//close(connection_fd);
		return EXIT_FAILURE;
	}

}


// 	pid_t child_fork = fork();
// 
// 	if(child_fork < 0){
// 		fprintf(stderr, "Child process could not be created\n");
// 		close(socket_fd);
// 		return 1;
// 	}
