// tinyvm project created by Rawley Amankwah

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define STACK_SIZE 256

bool file_exists(const char *filename){
    FILE *fp = fopen(filename, "r");
    bool is_exist = false;
    if (fp != NULL)
    {
        is_exist = true;
        fclose(fp); // close the file
    }
    return is_exist;
}


//typedef struct Point{
//	int x, y;
//} Point;


int main(int argc, char **argv){
	
	if(argc != 2){
		fprintf(stderr, "Error: Only one command line argument should be given\n");
		exit(1);
	}
	
	char *filename = argv[1];
	
	if(!file_exists(filename)){
		fprintf(stderr, "Error: File Path does not exist\n");
		exit(1);
	}
	
	FILE *file = fopen(filename, "rb");
	int stack[STACK_SIZE];
	int sp = -1;
	
	unsigned char bytecode[256];
	//Point byte_line;
	size_t elem;

    // Read file into bytecode buffer
    while ((elem = fread(bytecode, 1, 256, file)) > 0) {
        int ip = 0; // Instruction pointer

        while (ip < elem) {
            unsigned char opcode = bytecode[ip++]; // Fetch instruction

            switch (opcode) {
                case 0x01: { // PUSH X
                    if (ip >= elem) {
                        fprintf(stderr, "Error: PUSH missing operand\n");
                        return 1;
                    }
                    stack[++sp] = bytecode[ip++]; // Push operand to stack
					
					
                    break;
                }
				
				case 0x02: { // POP
                    if (sp < 0) {
                        fprintf(stderr, "Error: POP missing operand\n");
                        return 1;
                    }
                    //stack[--sp] = bytecode[ip++]; // Push operand to stack
					--sp;
                    break;
                }

                case 0x03: { // ADD
                    if (sp < 1) {
                        fprintf(stderr, "Error: Not enough values for ADD\n");
                        return 1;
                    }
                    int b = stack[--sp]; // Pop value 1
                    int a = stack[--sp]; // Pop value 2
                    stack[sp++] = a + b; // Push result
                    break;
                }
				
				case 0x04: { // SUB
                    if (sp < 1) {
                        fprintf(stderr, "Error: Not enough values for SUB\n");
                        return 1;
                    }
                    int b = stack[--sp]; // Pop value 1
                    int a = stack[--sp]; // Pop value 2
                    stack[sp++] = a - b; // Push result
                    break;
                }
				
				case 0x05: { // MUL
                    if (sp < 1) {
                        fprintf(stderr, "Error: Not enough values for MUL\n");
                        return 1;
                    }
                    int b = stack[--sp]; // Pop value 1
                    int a = stack[--sp]; // Pop value 2
                    stack[sp++] = a * b; // Push result
                    break;
                }
				
				case 0x06: { // DIV
                    if (sp < 1) {
                        fprintf(stderr, "Error: Not enough values for DIV\n");
                        return 1;
                    }
                    int b = stack[--sp]; // Pop value 1
                    int a = stack[--sp]; // Pop value 2
					if (b == 0) {
						fprintf(stderr, "Error: Division by zero\n");
						return 1;
					}
                    stack[sp++] = a / b; // Push result
                    break;
                }
				
				case 0x08: { // PRINT
                    if (sp < 0) {
                        fprintf(stderr, "Error: Stack empty, cannot PRINT\n");
                        return 1;
                    }
                    printf("%d\n", stack[sp]); // Print top of stack
                    break;
                }

                case 0x00: { // HALT
                    fclose(file);
                    return 0;
                }

                default:
                    fprintf(stderr, "Error: Unknown opcode 0x%02X\n", opcode);
                    return 1;
            }
        }
    }

    //fclose(file);
	
	return 0;
}