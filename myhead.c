// simulate the head command in linux


#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char **argv){

    int option_val = 0;
    int n = 10;

    while((option_val = getopt(argc, argv, "n:")) != -1){
        switch(option_val){
            case 'n':
                if(optarg) n = atoi(optarg);
                break;
            default:
                return 0;
        }

    }


    int i = optind;
    while(i < (argc)){
        FILE *fptr = fopen(argv[i], "r");


        if(fptr == NULL){
            fprintf(stderr, "Could not open file: %s\n", argv[i]);
            i += 1;
            continue;
        }

        char buffer[1024];
        int num_lines = 1;
        while(fgets(buffer, sizeof(buffer), fptr) && num_lines <= n){
            printf("%d: %s",num_lines, buffer);
            num_lines += 1;
        }

        fclose(fptr);
        printf("\n");
        i += 1;

    }


    return 0;
}
