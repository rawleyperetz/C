#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
//#include <openssl/aes.h>

// char xor(const char* input, const char* output, const int key) 
void xor_encrypt(FILE *ifile, FILE *ofile, int key) {
    char fileString[100];
    char holder[100];

    while (fgets(fileString, sizeof(fileString), ifile)) {
        for (int i = 0; i < strlen(fileString); i++) {  // Use strlen to get actual content length
            holder[i] = fileString[i] ^ key;
        }
        holder[strlen(fileString)] = '\0'; // Null terminate
        fwrite(holder, strlen(holder), 1, ofile);
    }
}

void caesarcipher(FILE *ifile, FILE *ofile, int key, int encrypt, int decrypt) {
    char fileString[100];
    char holder[100];
    key = (encrypt == 0) ? -abs(key) : abs(key);
    
    
    // Read the file line by line
    while (fgets(fileString, sizeof(fileString), ifile)) {
        // Loop through each character of the line
        for (int i = 0; i < strlen(fileString); i++) {
            if (isalpha(fileString[i])) {  // Check if it's a letter
                // Encrypt/Decrypt using the Caesar cipher
                char base = 'a';
                if (isupper(fileString[i])) {
                    base = 'A';  // Handle uppercase letters separately
                }

                // Apply Caesar shift with wrap-around
                int temp = (fileString[i] - base + key);
                if (temp < 0)
                {
                    temp += 26;
                }
                
                holder[i] = temp % 26 + base; //(fileString[i] - base + key) % 26 + base;
            } else {
                // Non-alphabetic characters remain unchanged
                holder[i] = fileString[i];
            }
        }
        
        // Null-terminate the modified string
        holder[strlen(fileString)] = '\0'; 
        fwrite(holder, sizeof(char), strlen(holder), ofile);  // Write to output file
    }
}


// void aes_encrypt(FILE *ifile, FILE *ofile, unsigned char *key) {
//     AES_KEY encryptKey;
//     unsigned char iv[AES_BLOCK_SIZE] = {0};
//     AES_set_encrypt_key(key, 128, &encryptKey); // 128-bit key

//     unsigned char buffer[AES_BLOCK_SIZE];
//     while (fread(buffer, 1, AES_BLOCK_SIZE, ifile)) {
//         AES_cbc_encrypt(buffer, buffer, AES_BLOCK_SIZE, &encryptKey, iv, AES_ENCRYPT);
//         fwrite(buffer, 1, AES_BLOCK_SIZE, ofile);
//     }
// }

void transpose_cipher(FILE *ifile, FILE *ofile, int key) {
    char fileString[100];
    char holder[100];
    int length, blockSize = key;

    while (fgets(fileString, sizeof(fileString), ifile)) {
        length = strlen(fileString);
        // If we don't have full blocks, pad the last one
        if (length % blockSize != 0) {
            int padding = blockSize - (length % blockSize);
            for (int i = length; i < length + padding; i++) {
                fileString[i] = 'X'; // Padding with 'X'
            }
            fileString[length + padding] = '\0';
            length += padding;
        }

        // Rearranging characters in the fileString by key pattern
        for (int i = 0; i < length; i++) {
            holder[i] = fileString[(i / blockSize) + (i % blockSize) * (length / blockSize)];
        }

        fwrite(holder, sizeof(char), length, ofile);
    }
}

void vigenere_cipher(FILE *ifile, FILE *ofile, char *key, int encrypt) {
    char fileString[100];
    char holder[100];
    int keyIndex = 0;
    int keyLength = strlen(key);
    int keyShift;

    while (fgets(fileString, sizeof(fileString), ifile)) {
        for (int i = 0; i < strlen(fileString); i++) {
            if (isalpha(fileString[i])) {
                char base = isupper(fileString[i]) ? 'A' : 'a';
                keyShift = key[keyIndex % keyLength] - base;
                if (!encrypt) keyShift = -keyShift; // For decryption
                holder[i] = (fileString[i] - base + keyShift + 26) % 26 + base;
                keyIndex++;
            } else {
                holder[i] = fileString[i];
            }
        }
        holder[strlen(fileString)] = '\0';
        fwrite(holder, sizeof(char), strlen(holder), ofile);
    }
}

int main(int argc, char *argv[]) {
    // int option;
    int key = 0;
    // const char *input = NULL;
    // const char *output = NULL;
    int encrypt = 0;
    int decrypt = 0;
    int option;
    FILE *ifile = NULL;
    FILE *ofile = NULL;
    char method[20] = {0};


    // Define long options
    struct option long_options[] = {
        {"encrypt", no_argument, 0, 'e'},
        {"decrypt", no_argument, 0, 'd'},
        {"key", required_argument, 0, 'k'},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"method", required_argument, 0, 'm'},
        {0, 0, 0, 0}  // Terminating null element
    };

    while ((option = getopt_long(argc, argv, "edk:i:o:m:", long_options, NULL)) != -1) {
        switch (option) {
            case 'e':
                encrypt = 1;
                break;

            case 'd':
                decrypt = 1;
                break;

            case 'k':
                key = atoi(optarg);
                break;

            case 'i':
                
                ifile = fopen(optarg,"r");
                if (ifile == NULL) {
                    printf("Input File Path does not exist\n");
                    printf("Exiting Program...");
                    exit(1);
                } 
                break;

            case 'o':
                
                ofile = fopen(optarg,"w");
                if (!ofile) {
                    perror("Error opening output file"); 
                    exit(1);
                } 
                break;

            case 'm':
                strncpy(method, optarg, sizeof(method) - 1);
                
                break;

            default:
                fprintf(stderr, "Usage: %s -e (encrypt) -d (decrypt) -k (key) -i (input) -o (output)\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if((encrypt  && decrypt)  || (!encrypt && !decrypt)){
        fprintf(stderr, "Error: Choose either --encrypt or --decrypt, not both!\n");
        exit(1);
    }

    if (ifile == NULL || ofile == NULL) {
        fprintf(stderr, "Error: Input and Output files are required!\n");
        exit(1);
    }


    
    if (strcmp(method, "xor") == 0) {
        xor_encrypt(ifile, ofile, key);
        printf("Processing complete!\n");
    } else if (strcmp(method, "caesar") == 0) {
        caesarcipher(ifile, ofile, key, encrypt, decrypt);
    } else{
        fprintf(stderr, "Error: Unknown encryption method!\n");
    }
    printf("Processing complete!\n");
    fclose(ifile);
    fclose(ofile);
    // if(method == "xor"){
    //     xor(ifile, ofile, key, encrypt, decrypt);
    // }

    return 0;
}



// void caesarcipher(FILE *ifile, FILE *ofile, int key){
//     char fileString[100];
//     char holder[100];

//     while (fgets(fileString, sizeof(fileString), ifile)) {
//         for (int i = 0; i < strlen(fileString); i++) {  // Use strlen to get actual content length
//             if (tolower(holder[i]) < 97 || tolower(holder[i]) > 122){
//                 holder[i] = fileString[i];
//             } else{
//                 holder[i] = (fileString[i] + key) % 123 + 97;
//             }  
//         }
//         holder[strlen(fileString)] = '\0'; // Null terminate
//         fwrite(holder, strlen(holder), 1, ofile);
//     }
// }