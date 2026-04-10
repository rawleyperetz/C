// program to list files and subdirectories of a directory

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define ARGCOUNT 2

int main(int argc, char **argv){

	if(argc > ARGCOUNT){
		fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	DIR *directory;
	struct dirent *entry;

	
	if(argc == 1){
		directory = opendir(".");
	}else{
		directory = opendir(argv[1]);
	}
	
	if(directory == NULL){
		fprintf(stderr, "Directory not found \n");
		exit(EXIT_FAILURE);
	}

	while((entry = readdir(directory)) != NULL){
		printf("%s\n", entry->d_name);
	}
	
	exit(EXIT_SUCCESS);
}
