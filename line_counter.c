// line counter


#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv){

    if(argc != 2){
        fprintf(stderr, "Error: only One command line argument needed\n");
        exit(EXIT_FAILURE);
    }

    FILE *fptr = fopen(argv[1], "r");

    if(fptr == NULL){
        fprintf(stderr, "File path does not exist\n");
        exit(EXIT_FAILURE);
    }

    int line_count = 0;

    char buffer[1024];
    while(fgets(buffer, 1024, fptr)){
        line_count++;
    }

    fclose(fptr);

    printf("Line count: %d\n", line_count);
    return 0;
}
