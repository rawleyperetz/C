// Shell History Searcher

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#define ARG_COUNT 2
#define BUFFER_SIZE 512

int main(int argc, char **argv){

    if(argc != ARG_COUNT){
        fprintf(stderr, "Only one command line argument needed \n Usage: ./histgrep <word> \n");
        return EXIT_FAILURE;
    }

    char *word = argv[1];

    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return EXIT_FAILURE;
    }

    char path[BUFFER_SIZE];
    snprintf(path, sizeof(path), "%s/.bash_history", home);

    FILE *fptr = fopen(path, "r");
    if(fptr == NULL){
        fprintf(stderr, "%s file path does not exist\n", path);
        return EXIT_FAILURE;
    }

    char line_buffer[BUFFER_SIZE];

    regex_t regex;
    int ret = regcomp(&regex, word, REG_ICASE | REG_NOSUB);
    if (ret){
        char errbuf[BUFFER_SIZE];
        regerror(ret, &regex, errbuf, sizeof(errbuf));
        fprintf(stderr, "Regex compilation failed: %s\n", errbuf);
        return EXIT_FAILURE;
    }

    while(fgets(line_buffer, sizeof(line_buffer),fptr)){
        line_buffer[strcspn(line_buffer, "\n")] = 0;
        if (regexec(&regex, line_buffer, 0, NULL, 0) == 0){
            printf("%s\n", line_buffer);
        }
    }

    regfree(&regex);
    fclose(fptr);
    return EXIT_SUCCESS;
}
