// simulate the cat command in linux for printing contents of a file to standard output

#include <stdio.h>

int main(int argc, char **argv){

    if(argc < 2){
        fprintf(stderr, "Usage: %s file1 [file2 ... fileN]\n", argv[0]);
        return 1;
    }

    int i = 1;
    while(i < (argc)){
        FILE *fptr = fopen(argv[i], "r");

        if(fptr == NULL){
            fprintf(stderr, "Could not open file: %s\n", argv[i]);
            i += 1;
            continue;
        }

        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), fptr)){
            printf("%s", buffer);
        }

        fclose(fptr);
        printf("\n");
        i += 1;

    }


    return 0;
}
