// this code copies file from one directory to another like the in-built cp

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <libgen.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#define ARGCOUNT 3

bool file_exists(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    bool is_exist = false;
    if (fp != NULL)
    {
        is_exist = true;
        fclose(fp); // close the file
    }
    return is_exist;
}

int isDirectoryExists(const char *path)
{
    struct stat stats;

    if(stat(path, &stats) != 0){
    	return 0;
    }

    return S_ISDIR(stats.st_mode);
}


int main(int argc, char *argv[]){
	if(argc != ARGCOUNT){
		fprintf(stderr, "Error!. Usage %s <filename> <dest directory> \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	// check whether first argument is a file path
    	char *filename = argv[1];

    	if (!file_exists(filename)){
        	fprintf(stderr, "File path %s does not exist\n", filename);
        	exit(EXIT_FAILURE);
    	}

    	// check whether second argument is a directory
    	//char path = argv[2];
    	if (!isDirectoryExists(argv[2])){
        	fprintf(stderr, "Directory does not exist: %s\n", argv[2]);
        	exit(EXIT_FAILURE);
     	} 

	FILE *fp = fopen(filename, "r");

	if(!fp){
		perror("Error opening the file\n");
		exit(EXIT_FAILURE);
	}

	size_t cap = 1024;
	size_t len = 0;
   	char *buf = malloc(cap); 

	if(!buf){
		perror("Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	char line[1024];

	while(fgets(line, sizeof(line), fp)){
		size_t line_len = strlen(line);

		if(line_len + len + 1 >= cap){
			cap *= 2;
			buf = realloc(buf, cap);
			if(!buf){
				perror("Memory Reallocation failed\n");
				exit(EXIT_FAILURE);
			}

		
		}
	
		memcpy(buf + len, line, line_len);
		len += line_len;	
	}

	buf[len] = '\0';

	fclose(fp);

	char dest_path[PATH_MAX];
	const char *base = strrchr(filename, '/');
	base = base ? base + 1 : filename;
	
	snprintf(dest_path, sizeof(dest_path), "%s/%s", argv[2], base);

	FILE *sfp = fopen(dest_path, "w");

	if(!sfp){
		fprintf(stderr, "Cannot open file: %s\n", dest_path);
		exit(EXIT_FAILURE);
	}

	fwrite(buf, 1, len, sfp);

	fclose(sfp);
	
    // adding filename to directory


	free(buf);
	return 0;
}
