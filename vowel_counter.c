// vowel counter program

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TEXT_SIZE 64

int main()
{
    char str[TEXT_SIZE] = {0};

    printf("Enter the string: ");
    fgets(str,TEXT_SIZE-1,stdin);
    //printf("\nThe string is: %s",string);
    str[strcspn(str, "\n")] = '\0';

    // convert string to lowercase;
    int len = strlen(str);
    for(int i = 0; i < len; i++){
        str[i] = tolower(str[i]);
    }

    char vowels[] = "aeiou";
    int vowel_len = strlen(vowels);

    int vowel_count = 0;
    for(int i = 0; i < len; i++){
        for(int j=0; j < vowel_len; j++){
            if(str[i] == vowels[j]){
                vowel_count++;
            }
        }
    }
    printf("There are %d vowels in the string, %s\n", vowel_count, str);
    //int consonants = len - vowel_count;
    //printf("There are %d consonants in the string, %s", consonants, str);

    return 0;
}
