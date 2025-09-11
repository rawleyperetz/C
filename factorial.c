// factorial computation in C

#include <stdlib.h>
#include <stdio.h>

#define EXPECTED_ARGC 2

int main(int argc, char **argv){

    if(argc != EXPECTED_ARGC){
        fprintf(stderr, "Only one command line argument needed\n");
        return 1;
    }

    int num = atoi(argv[1]);
    if(num <= 0){
        fprintf(stderr, "Number must be a positive integer\n");
        return 1;
    }

    unsigned long long factorial_val = 1;
    for(int i=1; i <= num; i++){
        factorial_val = factorial_val * i;
    }

    printf("The factorial of %d is %d\n", num, factorial_val);
    return 0;
}
