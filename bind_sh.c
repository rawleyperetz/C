
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define ARGCOUNT 3
#define PORT_MIN 1
#define PORT_MAX 65535

int main(int argc, char **argv){

	if(argc != ARGCOUNT){
		fprintf(stderr, "Usage: %s <port_num> <shell>\n",argv[0]);
		return 1;
	}

	char *port_number = argv[1];
	char *shell = argv[2];

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
		fprintf(stderr, "Error opening socket: %s", strerror(errno));
		return 1;
	}

	int opt = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


	struct sockaddr_in target_address;
	target_address.sin_family = AF_INET;
	target_address.sin_port = htons(port_num);
	target_address.sin_addr.s_addr = INADDR_ANY;
	
	int bind_status = bind(socket_fd, (struct sockaddr*) &target_address, sizeof(target_address));
	if(bind_status < 0){
		fprintf(stderr, "Bind failed\n");
		close(socket_fd);
	    return 1;
	}
	
	int listen_val = listen(socket_fd, 1);
	if(listen_val < 0){
		fprintf(stderr, "Listening failed\n");
		close(socket_fd);
		return 1;
	}
	

	int accept_socket = accept(socket_fd, NULL, NULL);
	if(accept_socket < 0){
		perror("accept");
		close(socket_fd);
		return 1;
	}

	// add error checking of the dup2's
	dup2(accept_socket, STDIN_FILENO);
	dup2(accept_socket, STDOUT_FILENO);
	dup2(accept_socket, STDERR_FILENO);

	// we close socket_fd because the shell does not need the listening one
	// only the accepted one (accept_socket)
	close(socket_fd);
	
	int shell_program = execlp(shell, shell, NULL);
	if(shell_program < 0){
		fprintf(stderr, "Shell program failed to start: %s", shell);
		close(socket_fd);
		return 1;
	}
	

	if((close(socket_fd)) == -1){
		perror("Error closing socket file");
		return 1;
	}

	return 0;
}


// 	pid_t child_fork = fork();
// 
// 	if(child_fork < 0){
// 		fprintf(stderr, "Child process could not be created\n");
// 		close(socket_fd);
// 		return 1;
// 	}
