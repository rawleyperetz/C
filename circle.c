// this code computes the area or circumference of a circle

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float area_of_circle(float r){
    return M_PI*r*r;
}

float perimeter_of_circle(float r){
    return 2*M_PI*r;
}

int main(){
    float radius;
    char choice;

    printf("Enter the radius of the circle: ");
    /*while (scanf("%f", &radius) != 1) {
        printf("Invalid input! Enter a number: ");
        while (getchar() != '\n');  // Clear input buffer
    }*/
    scanf("%f", &radius);


    printf("(c)ircumference or (a)rea: ");
    /*while (scanf("%c", &choice) != 1) {
        printf("Invalid input! Enter a char: ");
        while (getchar() != '\n');  // Clear input buffer
    };*/
    scanf(" %c", &choice); // space before the character format specifier because the newline char is still in the buffer

    if(choice == 'a' || choice == 'A'){
        printf("Area is: %f\n", area_of_circle(radius));
    } else{
        printf("Circumference is: %f\n", perimeter_of_circle(radius));
    }

    //system("pause");
    return 0;
}
