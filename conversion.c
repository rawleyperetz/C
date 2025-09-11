// this code does the decimal to other bases conversion

// to run this code, you need to link to the math library
// gcc conversion.c -o conversion -lm

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int other_to_decimal(int arr[], size_t len, double base){
    double final_result = 0;
    for(size_t i=0; i < len; i++){
        final_result = final_result + arr[i]*pow(base,i);
    }
    return (int) final_result;
}

int main(){
    int arr_values[3] = {1,1,1};
    size_t len = sizeof(arr_values)/sizeof(int);
    printf("The conversion of 11 base 2 is %d\n", other_to_decimal(arr_values, len, 2));
    return 0;
}
