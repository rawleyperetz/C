// check the file size of a file 

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


int main(int argc, char **argv){

	if(argc != 2){
		fprintf(stderr, "Error: only One command line argument needed\n");
		exit(EXIT_FAILURE);
	}

	char *fptr = argv[1]; //fopen(argv[1], "r");

	// if(fptr == NULL){
	// 	fprintf(stderr, "File path does not exist\n");
	// 	exit(EXIT_FAILURE);
	// }
	
	
	struct stat sb;
	if (stat(fptr, &sb) == -1) {
	    perror("stat");
	    exit(EXIT_FAILURE);
	}

	printf("File %s has size  %ld bytes\n",fptr, sb.st_size);
	


	//fclose(fptr);

	return 0;
}
