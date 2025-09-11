// inline assembly in C

#include <stdlib.h>
#include <stdio.h>

int sum4(int a, int b, int c, int d) {
    int result;
    __asm__ (
        "movl %1, %%eax;"
        "addl %2, %%eax;"
        "addl %3, %%eax;"
        "addl %4, %%eax;"
        "movl %%eax, %0;"
        : "=r" (result)
        : "r" (a), "r" (b), "r" (c), "r" (d)
        : "%eax"
    );
    return result;
}

int main(void){
    int s = sum4(1,2,3,4);
    printf("The sum is %d\n", s);

    return 0;
}
