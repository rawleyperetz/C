// show hex works similar to hexdump but as a simpler version

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define BINARY_READ "rb"
#define BUFFER_SIZE 16



int main(int argc, char **argv){

    if(argc != 2){
        printf("Error: Need only One command line argument\n");
        exit(-1);
    }

    char *filename = argv[1];

    FILE *bin_file = fopen(filename, BINARY_READ);

    if(bin_file == NULL){
        printf("Could not locate file\n");
        exit(-1);
    }

    int line_counter = 0;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, bin_file)) > 0){
        // print byte offset
        printf("%08X ", line_counter * BUFFER_SIZE);

        printf("\t");
        // convert each byte to hex
        for(int i=0; i < BUFFER_SIZE; i++){
            //printf("%c \t", to_hex(buffer[i]));
            if(i < bytes_read){
                printf("%02X ", (unsigned char)buffer[i]);
            } else{
                printf(" ");
            }

        }

        printf("\t\t");

        // actual ascii characters
        for(int i=0; i < BUFFER_SIZE; i++){
            if(isprint(buffer[i])){
                printf("%c ", buffer[i]);
            } else{
                printf(". ");
            }
        }
        printf("\n");

        line_counter += 1;
    }

    fclose(bin_file);

    return 0;
}
