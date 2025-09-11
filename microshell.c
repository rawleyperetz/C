// this code runs a tiny mini shell like bash


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define BUFFER_SIZE 1024
//#include <sys/wait.h>

int main(int argc, char *argv[]){

    //int ch;
    while(true){
        char theCommand[BUFFER_SIZE] = { NULL } ;
        printf("μ-sh $ ");
        if (fgets(theCommand, BUFFER_SIZE, stdin) == NULL) {
            break; // Handle Ctrl+D
        }

        // Remove trailing newline
        theCommand[strcspn(theCommand, "\n")] = 0;

        char *args[64]; // collects all arguments here
        int i = 0;
        char *tokenize_string = strtok(theCommand, " ");
        while(tokenize_string != NULL && i < 63){
            i += 1
            //if(strstr(tokenize_string, ">") || strstr(tokenize_string, "<")){
                //char redirection_symbol = tokenize_string;
              //  char *filename =
            //}
            args[i] = tokenize_string;
            tokenize_string = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (i == 0) continue; // Empty input

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        int rc = fork();
        if (rc < 0){
            fprintf(stderr, "fork failed \n");
            exit(1);
        } else if(rc == 0){
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else{
            wait(NULL);
        }

    }

    return 0;
}
