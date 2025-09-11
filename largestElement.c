#include <stdio.h>
#include <stdlib.h>

int find_max(int *arr, int size){
    int *p = arr;
    int big = *p;

    for(int i=0; i < size; i++){
        //printf("Original %d\n", *(ptr + i));
        if(*(p + i) > big){
            big = *(p + i);
        }
    }
    return big;
}

int main(){
    int arr[] = {3,5,1,12,2};

    int sizeArr = sizeof(arr)/sizeof(int);
    int largest_val = find_max(arr, sizeArr);
    printf("Largest: %d\n", largest_val);
    return 0;
}
