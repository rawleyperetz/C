// this code computes the area or perimeter of a rectangle

#include <stdlib.h>
#include <stdio.h>

float area_of_rectangle(float a, float b){
    return a*b;
}

float perimeter_of_rectangle(float a, float b ){
    return 2*(a + b);
}

int main(){
    float a;
    float b;
    char choice;

    printf("Enter the length of the rectangle: ");
    /*while (scanf("%f", &radius) != 1) {
        printf("Invalid input! Enter a number: ");
        while (getchar() != '\n');  // Clear input buffer
    }*/
    scanf("%f", &a);
    while (getchar() != '\n');

    printf("Enter the breadth of the rectangle: ");
    scanf("%f", &b);
    while (getchar() != '\n');
    /*while (scanf("%c", &choice) != 1) {
        printf("Invalid input! Enter a char: ");
        while (getchar() != '\n');  // Clear input buffer
    };*/
    printf("(a)rea or (p)erimeter: ");
    scanf(" %c", &choice); // space before the character format specifier because the newline char is still in the buffer

    if(choice == 'a' || choice == 'A'){
        printf("Area is: %f\n", area_of_rectangle(a,b));
    } else{
        printf("Circumference is: %f\n", perimeter_of_rectangle(a,b));
    }

    //system("pause");
    return 0;
}
