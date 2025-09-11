// ROT13

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *rot_13(char *msg){

    int msg_len = strlen(msg);

    char *cipher = malloc(msg_len + 1);
    if (cipher == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i < msg_len; i++){
        char letter = tolower(msg[i]);
        if (letter >= 'a' && letter <= 'z') {
            cipher[i] = ((letter - 'a' + 13) % 26) + 'a';
        }else{
            cipher[i] = letter;
        }
    }

    cipher[msg_len] = '\0'; // Null-terminate the string
    return cipher;
}


int main(){
    printf("Enter text here: ");

    char message[1024];

    fgets(message, 1024, stdin);//"%s\n", message);
    message[strcspn(message, "\n")] = '\0';

    char *encrypted = rot_13(message);
    printf("Cipher text is %s \n", encrypted);

    free(encrypted); // Always free what you malloc
    return 0;

}
