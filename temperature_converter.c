#include <stdio.h>
#include <stdlib.h>

int main(){
    int temp_celsius;
    char choice;

    printf("Enter the temperature in Celsius: ");
    scanf("%d", &temp_celsius);

    printf("Convert to (F)ahrenheit or (K)elvin?: ");
    scanf(" %c", &choice);

    if (choice == 'F' || choice == 'f')
    {
        float conversion = temp_celsius *  (9.0/5) + 32;
        printf("Temperature in Fahrenheit: %.2f \n", conversion);
    } else if (choice == 'K' || choice == 'k')
    {
        float conversion = temp_celsius + 273.15;
        printf("Temperature in Kelvin: %.2f \n", conversion);
    } else {
        printf("Wrong Choice \n");
    }
    
    system("pause");
    return 0;
}