// the following code prints the total seconds since the system booted and the total seconds all CPUs have spent idle

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 64

int main(){
    FILE *fptr = fopen("/proc/uptime", "r");

    if(fptr == NULL){
        fprintf(stderr, "The file /proc/uptime does not exist\n");
        return EXIT_FAILURE;
    }

    char buffer[BUFFERSIZE];

    if(fgets(buffer, BUFFERSIZE, fptr)){
        buffer[strcspn(buffer, "\n")] = 0;
        char *tokenize_string = strtok(buffer, " ");

        unsigned int i = 0;
        char preamble[BUFFERSIZE] = "Total seconds since system boot: ";
        while(tokenize_string != NULL){
            if(i != 0){
                strcpy(preamble, "Total seconds all CPUs have spent idle: ");
            }
            //preamble[strcspn(preamble, "\n")] = 0;
            printf("%s %s\n", preamble, tokenize_string);
            tokenize_string = strtok(NULL, " ");
            i++;
        }
    }
    fclose(fptr);
    return 0;
}
