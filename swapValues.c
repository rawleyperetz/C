#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b){
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

int main(){
    int a = 10;
    int b = 20;
    printf("Before swap: x = %d and y = %d\n", a, b);
    swap(&a, &b);
    printf("After swap: x = %d and y = %d\n", a, b);
    return 0;
}
