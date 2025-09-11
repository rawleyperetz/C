#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX_LINE 256
struct userData
{
    char name[20];
    int acc_number;
    float init_deposit;
    int pin;
};

int generateAccountNumber(int minvalue, int maxvalue){
    return ((rand() % (maxvalue - minvalue + 1)) + minvalue);
}

void saveUser(struct userData user) {
    FILE *file = fopen("users.dat", "ab");  // "ab" = append binary
    if (file == NULL) {
        printf("Error opening file!\n");
        //return 1;
    }
    fwrite(&user, sizeof(struct userData), 1, file);
    //fclose(file);
    if (fclose(file) != 0) {
        perror("Error closing users.dat");
    }
    printf("User data saved successfully!\n");
}

int userExists(const char *username) {
    FILE *file = fopen("users.dat", "rb");  // "rb" = read binary
    if (file == NULL) {
        printf("No user data found.\n");
        return 0;
    }
    struct userData user;
    while (fread(&user, sizeof(struct userData), 1, file)){
        if (strcmp(user.name, username) == 0){
            fclose(file);
            return 1; // User found
        }
    }
    fclose(file);
    return 0;
}

int useraccExists(const int *useracc) {
    FILE *file = fopen("users.dat", "rb");  // "rb" = read binary
    if (file == NULL) {
        printf("No user data found.\n");
        return 0;
    }
    struct userData user;
    while (fread(&user, sizeof(struct userData), 1, file)){
        if (user.acc_number == *useracc){
            fclose(file);
            return 1; // User found
        }
    }
    fclose(file);
    return 0;
}

void printBalance(const int *accountnumber, const int *pin){
    FILE *file = fopen("users.dat", "rb");  // "rb" = read binary
    if (file == NULL) {
        printf("No user data found.\n");
        return;
    }
    struct userData user;
    int found = 0; // Flag to track if user is found 

    while (fread(&user, sizeof(struct userData), 1, file)){
        if (*accountnumber == user.acc_number && *pin == user.pin){
            printf("Your account balance is %.2f\n", (float) user.init_deposit);
            found = 1;
            break;
            //return 1; // User found
        }
    }
    //printf("Account under %s not found\n", username);
    fclose(file);

    if (!found){
        printf("Account number, %d not found\n", *accountnumber);
    }
    //return 0;
}

void transfer_money(const int *accNum, const int *pin, const float *new_balance, const int receiver_acc){
    FILE *file = fopen("users.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (!file || !temp) {
        perror("Error opening file");
        return;
    }

    struct userData user;
    int sender_found = 0, receiver_found = 0;
    float sender_balance = 0;

    // **First pass: Check sender's balance before modifying anything**
    while (fread(&user, sizeof(struct userData), 1, file)) {
        if (*accNum == user.acc_number && *pin == user.pin) {
            sender_found = 1;
            sender_balance = user.init_deposit;
        }
        if (receiver_acc == user.acc_number) {
            receiver_found = 1;
        }
    }

    // **Ensure sender has enough money before modifying files**
    if (sender_balance < *new_balance) {
        printf("Sorry, Insufficient Funds\n");
        fclose(file);
        fclose(temp);
        remove("temp.dat");
        return;
    }

    // Rewind file to process updates
    rewind(file);

    // **Second pass: Apply changes**
    while (fread(&user, sizeof(struct userData), 1, file)) {
        if (*accNum == user.acc_number && *pin == user.pin) {
            user.init_deposit -= *new_balance;
        }
        if (receiver_acc == user.acc_number) {
            user.init_deposit += *new_balance;
        }
        fwrite(&user, sizeof(struct userData), 1, temp);
    }

    // Close files
    fclose(file);
    fclose(temp);

    // Replace original file with updated file
    if (remove("users.dat") != 0) {
        perror("Error deleting old file");
        return;
    }

    if (rename("temp.dat", "users.dat") != 0) {
        perror("Error renaming temp file");
        return;
    }

    printf("Transfer successful!\n");
}



void update_balance(const int *accNum, const int *pin, const float *new_balance, const char ch) {
    FILE *file = fopen("users.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (!file || !temp) {
        perror("Error opening file");
        return;
    }

    // char line[MAX_LINE];
    // char name[50];
    // int id;
    // double balance;
    struct userData user;
    //int found = 0; // Flag to track if user is found 

    while (fread(&user, sizeof(struct userData), 1, file)) {
        if (*accNum == user.acc_number && *pin == user.pin) {
            //found = 1; // Mark that the user was found
            if (ch == 'a') {
                user.init_deposit += *new_balance;
            } else if (ch == 's') {
                if (user.init_deposit < *new_balance) {
                    printf("Sorry, Insufficient Funds\n");
                    fclose(file);
                    fclose(temp);
                    remove("temp.dat"); // Avoid deleting users.dat
                    return;
                }
                user.init_deposit -= *new_balance;
            }
        }
        fwrite(&user, sizeof(struct userData), 1, temp);
    }
    // while (fgets(line, sizeof(line), file)) {
    //     if (sscanf(line, "%[^,],%d,%lf", name, &id, &balance) == 3) {
    //         // If this is the target name, update balance
    //         //if (strcmp(name, target_name) == 0)
    //         if(accNum == user.acc_number) {
    //             balance = new_balance;
    //         }
    //         // Write to temp file
    //         fprintf(temp, "%s,%d,%.2f\n", name, id, balance);
    //     }
    // }

    // fclose(file);
    // fclose(temp);
    if (fclose(file) != 0) {
        perror("Error closing users.dat");
    }
    if (fclose(temp) != 0) {
        perror("Error closing temp.dat");
    }

    // Replace original file with the updated file
    // remove("users.dat");
    // rename("temp.dat", "users.dat");
    // if (!found) {
    //     remove("temp.dat"); // Avoid deleting the original file if no user was found
    //     printf("Account not found.\n");
    //     return;
    // }

    if (remove("users.dat") != 0) {
        perror("Error deleting old file");
        return;
    }

    if (rename("temp.dat", "users.dat") != 0) {
        perror("Error renaming temp file");
        return;
    }
    printf("Operation successful!\n");
}


void check_details(const int *accNum, const int *pin){
    FILE *file = fopen("users.dat", "rb");  // "rb" = read binary
    if (file == NULL) {
        printf("No user data found.\n");
        return;
    }
    struct userData user;
    int found = 0; // Flag to track if user is found 

    while (fread(&user, sizeof(struct userData), 1, file)){
        if (*accNum == user.acc_number && *pin == user.pin){
            found = 1;
            printf("Account Holder: %s\n", user.name);
            printf("Account Number: %d\n", user.acc_number);
            printf("Amount in Account: %.2f\n", user.init_deposit);
            return;
            //return 1; // User found
        }
    }
    //printf("Account under %s not found\n", username);
    fclose(file);

    if (!found){
        printf("Account number, %d not found\n", *accNum);
    }
}

int main(){
    int choice;
    float deposit;
    
    //FILE* fptr;
    // char name[20];
    // int pin;
    // int init_deposit;

    while (1)
    {
        printf("Welcome to Rawley\'s Bank \n");
        printf("[1] Create Account \n");
        printf("[2] Deposit Money \n");
        printf("[3] Withdraw Money \n");
        printf("[4] Transfer Money \n");
        printf("[5] Check Balance \n");
        printf("[6] View Account Details \n");
        printf("[7] Exit \n");

        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { 
            printf("Invalid input! Please enter a number.\n");
            return 1;
        }

        
        struct userData user;
        switch (choice)
        {
        case 1:
            

            printf("Enter your name: ");

            while(getchar() != '\n'); // flush input buffer (Important If previous 'scanf()' calls were used)
            fgets(user.name, sizeof(user.name), stdin);
            user.name[strcspn(user.name,"\n")] = '\0'; // removes newline character

            //while(getchar() != '\n');
            printf("Enter initial deposit: ");
            while (scanf("%f", &user.init_deposit) != 1) {
                printf("Invalid input! Enter a number: ");
                while (getchar() != '\n');  // Clear input buffer
            }

            printf("Enter 4-digit pin: ");
            while (scanf("%d", &user.pin) != 1 || user.pin < 1000 || user.pin > 9999) {
                printf("Invalid PIN! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }
            srand(time(NULL));
            user.acc_number = generateAccountNumber(1000,9999);
            
            if (userExists(user.name)) {
                printf("Account for username '%s' already exists!\n", user.name);
            } else {
                saveUser(user);
                printf("\nAccount successfully created!\n");
                printf("Account Number: %d\n", user.acc_number);
                printf("Account Holder: %s\n", user.name);
                printf("Initial Deposit: %.2f\n", user.init_deposit);
            }
            
            break;



        case 2:
            char ch='a';
            printf("Enter account number: ");
            while (scanf("%d", &user.acc_number) != 1 || user.acc_number < 1000 || user.acc_number > 9999) {
                printf("Invalid Account Number! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            
            printf("Enter 4-digit pin: ");
            while (scanf("%d", &user.pin) != 1 || user.pin < 1000 || user.pin > 9999) {
                printf("Invalid PIN! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }
            
            if (useraccExists(&user.acc_number)) {
                //printf("Account for username '%s' already exists!\n", user.name);
                printf("Enter deposit amount: ");
                while (scanf("%f", &deposit) != 1 || deposit <= 0) {
                    printf("Invalid Deposit Amount! Enter a positive number: \n");
                    //printf("Exiting...");
                    while (getchar() != '\n');
                }
                update_balance(&user.acc_number, &user.pin, &deposit, ch);
                //printf("Deposit successful\n");
                printBalance(&user.acc_number, &user.pin);
                break;
            } else {
                printf("User account does not exist %d\n", user.acc_number);
            }

            break;



        case 3:
            char chs = 's';
            printf("Enter account number: ");
            while (scanf("%d", &user.acc_number) != 1 || user.acc_number < 1000 || user.acc_number > 9999) {
                printf("Invalid Account Number! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            
            printf("Enter 4-digit pin: ");
            while (scanf("%d", &user.pin) != 1 || user.pin < 1000 || user.pin > 9999) {
                printf("Invalid PIN! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            if (useraccExists(&user.acc_number)) {
                //printf("Account for username '%s' already exists!\n", user.name);
                printf("Enter withdrawal amount: ");
                while (scanf("%f", &deposit) != 1 || deposit <= 0) {
                    printf("Invalid Withdrawal Amount! Enter a positive number: \n");
                    //printf("Exiting...");
                    while (getchar() != '\n');
                }
                update_balance(&user.acc_number, &user.pin, &deposit, chs);
                //printf("Withdrawal successful\n");
                printBalance(&user.acc_number, &user.pin);
                break;
            } else {
                printf("User account does not exist %d\n", user.acc_number);
            }

            break;




        case 4:
            int receiver_acc;
            
            printf("Enter account number: ");
            while (scanf("%d", &user.acc_number) != 1 || user.acc_number < 1000 || user.acc_number > 9999) {
                printf("Invalid Account Number! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            
            printf("Enter 4-digit pin: ");
            while (scanf("%d", &user.pin) != 1 || user.pin < 1000 || user.pin > 9999) {
                printf("Invalid PIN! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            printf("Enter receiver\'s account number: ");
            while (scanf("%d", &receiver_acc) != 1 || receiver_acc < 1000 || receiver_acc > 9999) {
                printf("Invalid Account Number! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            if (useraccExists(&user.acc_number) && useraccExists(&receiver_acc)) {
                //printf("Account for username '%s' already exists!\n", user.name);
                printf("Enter transfer amount: ");
                while (scanf("%f", &deposit) != 1 || deposit <= 0) {
                    printf("Invalid Transfer Amount! Enter a positive number: \n");
                    //printf("Exiting...");
                    while (getchar() != '\n');
                }
                transfer_money(&user.acc_number, &user.pin, &deposit, receiver_acc);
                //printf("Transfer successful\n");
                printBalance(&user.acc_number, &user.pin);
                break;
            } else {
                printf("One or both of user account(s) do(es) not exist\n");
            }



        case 5:
            //struct userData puser;

            // printf("Enter your name: ");

            //while(getchar() != '\n'); // flush input buffer (Important If previous 'scanf()' calls were used)
            // fgets(puser.name, sizeof(puser.name), stdin);
            // puser.name[strcspn(puser.name,"\n")] = '\0'; // removes newline character

            printf("Enter account number: ");
            while (scanf("%d", &user.acc_number) != 1 || user.acc_number < 1000 || user.acc_number > 9999) {
                printf("Invalid Account Number! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            
            printf("Enter 4-digit pin: ");
            while (scanf("%d", &user.pin) != 1 || user.pin < 1000 || user.pin > 9999) {
                printf("Invalid PIN! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            printBalance(&user.acc_number, &user.pin);
            break;




        case 6:
            printf("Enter account number: ");
            while (scanf("%d", &user.acc_number) != 1 || user.acc_number < 1000 || user.acc_number > 9999) {
                printf("Invalid Account Number! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            
            printf("Enter 4-digit pin: ");
            while (scanf("%d", &user.pin) != 1 || user.pin < 1000 || user.pin > 9999) {
                printf("Invalid PIN! Enter a 4-digit number: ");
                while (getchar() != '\n');
            }

            if (useraccExists(&user.acc_number)) {
                //printf("Account for username '%s' already exists!\n", user.name);
        
                check_details(&user.acc_number, &user.pin);
                //printf("Transfer successful\n");
                //printBalance(&user.acc_number, &user.pin);
                break;
            } else {
                printf("User account, %d does not exist\n", user.acc_number);
            }

            break;
        
        case 7:
            printf("Thank you for banking with us!\n");
            printf("Tschuss\n");
            return 0;
            break;
            
        default:
            break;
        }
        break;
    }
    
    
    getchar();

    return 0;
}