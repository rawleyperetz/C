
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
//#include <math.h>
#define TEXTSIZE 1024

int main(int argc, char **argv) {

    int b = 0; int p = 0; int option_val = 0;
    FILE *outputfile = NULL;

    while((option_val = getopt(argc, argv, "bo:p")) != -1){
		switch(option_val){
			case 'b':
				//if(optarg) hours = abs(atoi(optarg));
                //if(optarg) 
		b = 1;
				break;
			case 'p':
				//if(optarg) 
		p = 1;
				// work on this
				break;
			case 'o':
				//if(optarg) output = 1; //seconds = abs(atoi(optarg));
				if(b == 1){
                    outputfile = fopen(optarg, "w");
				} else{
                    outputfile = fopen(optarg, "r");
				}
				break;
			default:
                fprintf(stderr, "Usage: (-b, -o outputfile, -p playback)");
				exit(-1);
		}
	}

	if(b == 1 && outputfile != NULL && p == 0){
        initscr();              // Start ncurses mode
        cbreak();               // Disable line buffering
        noecho();               // Don't echo typed keys
    //keypad(stdscr, TRUE);   // Enable function keys and arrows

        int ch;
        while ((ch = getch()) != 27) { // ESC key to exit (ASCII 27)
            fputc(ch, outputfile);
            fflush(outputfile);          // Ensure it's written immediately

        // Optional: show on screen
            printw("%c", ch);
            refresh();
        }



	} else if(b == 0 && outputfile != NULL && p == 1){
        // get file characters
        initscr();              // Start ncurses mode
        cbreak();               // Disable line buffering
        noecho();               // Don't echo typed keys
    //keypad(stdscr, TRUE);   // Enable function keys and arrows
        int ch;
        char buffer[TEXTSIZE] = {0};
        while (fgets(buffer, TEXTSIZE, outputfile)) {
            printw("%s", buffer);
            refresh();
            napms(300);
        }
	napms(10000);
	} else if(b == 1 && p == 1){
        fprintf(stderr, "Cannot use both -b and -p at the same time.\n");
        exit(1);
	}else{
        fprintf(stderr, "Error! Usage: (-b, -o outputfile, -p playback)");
        exit(-1);
	}


    fclose(outputfile);
    endwin();               // End ncurses mode
    return 0;
}


