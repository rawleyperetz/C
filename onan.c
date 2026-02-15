// my editor similar to nano but reversed, onan

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
//#include <errno.h>

#define ARGCOUNT 2

bool file_exists(const char *filename){
	FILE *fp = fopen(filename, "r");
	bool is_exist = false;
	if(fp != NULL){
		is_exist = true;
		fclose(fp);
	}
	return is_exist;
}


size_t line_start(const char *buf, size_t cursor) {
    while (cursor > 0 && buf[cursor - 1] != '\n')
        cursor--;
    return cursor;
}

size_t prev_line_start(const char *buf, size_t cursor) {
    size_t start = line_start(buf, cursor);
    if (start == 0) return cursor;  // already top line

    cursor = start - 1;
    while (cursor > 0 && buf[cursor - 1] != '\n')
        cursor--;
    return cursor;
}

size_t next_line_start(const char *buf, size_t len, size_t cursor) {
    while (cursor < len && buf[cursor] != '\n')
        cursor++;

    if (cursor < len) cursor++; // skip '\n'
    return cursor;
}

void cursor_to_yx(const char *buf, size_t cursor, int *y, int *x) {
    *y = 0;
    *x = 0;
    for (size_t i = 0; i < cursor; i++) {
        if (buf[i] == '\n') {
            (*y)++;
            *x = 0;
        } else {
            (*x)++;
        }
    }
}

void render_buffer(const char *buf, size_t len){
	clear();

	for(size_t i = 0; i < len; i++){
		addch(buf[i]);
	}

	refresh();
}


void disable_flow_control(void) {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_iflag &= ~(IXON | IXOFF);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


int main(int argc, char **argv){

	const char *temp_path;
	bool file_does_exist = false;
	if(argc == ARGCOUNT){
		temp_path = argv[1];
		
		if(file_exists(temp_path)){
			file_does_exist = true;
		} else {
			perror("File path does not exist \n");
			exit(EXIT_FAILURE);
		}
		
	} else{
		static char default_path[512];
		snprintf(default_path, sizeof(default_path), "%s/temp.txt", getenv("HOME"));
		temp_path = default_path;
	}
    
    //FILE *tfptr = fopen(temp_path, "r+");
	//printf("I am somewhere here\n");

	short int saved = 0;


	disable_flow_control();
	initscr(); // initializing ncurses mode
	cbreak(); // disable line buffering
	keypad(stdscr, TRUE); // Enable function keys
	noecho(); // disable character echo

	//show_help();
	move(0,0);

	int ch, x, y;
	//char buffer[1024];


	FILE *tfptr = NULL;
	//struct stat sb;
	size_t cap = 1024;   // initial capacity
	size_t len = 0;      // actual text length
	size_t cursor = 0;
	char *buf = malloc(cap);

	if(!buf){
		endwin();
		perror("Memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	if(argc == ARGCOUNT && file_does_exist == true){

		tfptr = fopen(temp_path, "r+");

		if(!tfptr){
			endwin();
			fprintf(stderr, "File %s cannot be opened\n", temp_path);
			exit(EXIT_FAILURE);
		}

		//rewind(tfptr);

		char line[1024];
		//int row = 0;
		//int length_of_text = 0;

		while(fgets(line, sizeof(line), tfptr)){
			size_t line_len = strlen(line);

			if (line_len + len + 1 >= cap){
				cap *= 2;
				buf = realloc(buf, cap);
				if(!buf){
					endwin();
					perror("Memory Allocation Renew Error\n");
					exit(EXIT_FAILURE);
				}
			}

			// for(int i = 0; i < end_index; i++){
			// 	buf[i + length_of_text] = line[i];
			// }
			memcpy(buf + len, line, line_len);
			len += line_len;
		}

		buf[len] = '\0';

		for(size_t i = 0; i < len; i++){
			addch(buf[i]);
		}
		cursor = 0;
		// refresh();
		// if(row >= LINES - 1){
		// 	break;
		// }

		fclose(tfptr);
		refresh();
	 } //else{
	// 	tfptr = fopen(temp_path, "w+");
	// }

	//printf("I got here at least\n");
	
	while( (ch=getch()) != 24 ){ // Ctrl + X is ascii 24
		getyx(stdscr, y, x);
		if (ch == KEY_BACKSPACE || ch == 127){
		
			if (cursor > 0) {
				memmove(buf + cursor - 1, buf + cursor, len - cursor);
				cursor--;
				len--;
			}
		}//else if (ch == '\n'){
		// 	addch('\n');
		// 	//fputc('\n', tfptr);
		// }
		else if (ch == KEY_RIGHT){
			//move(y, x+1);
			if(cursor < len) cursor++;
		}else if (ch == KEY_LEFT){
			//move(y, x-1);
			if(cursor > 0) cursor--;
		}else if (ch == KEY_UP){
			//move(y-1, x);
			size_t col = cursor - line_start(buf, cursor);
    		size_t prev = prev_line_start(buf, cursor);

   			size_t i = prev;
    		while (i < len && buf[i] != '\n' && col--)
        		i++;

    		cursor = i;
		}else if (ch == KEY_DOWN){
			//move(y+1, x);
			size_t col = cursor - line_start(buf, cursor);
    		size_t next = next_line_start(buf, len, cursor);

    		size_t i = next;
    		while (i < len && buf[i] != '\n' && col--)
        		i++;

    		cursor = i;
		}else if (ch == 19 ){ // Ctrl + S is ascii 19 for save
			// get all characters and write to file
			FILE *fp = fopen(temp_path, "w");

			if(!fp){
				fprintf(stderr, "Cannot open the file %s", temp_path);
				exit(EXIT_FAILURE);
			}

			fwrite(buf, 1, len, fp);
			
			fclose(fp);
		

			saved = 1;
			//endwin();
			break;

		}else{
			//addch(ch);
			if(ch >= 32 && ch <= 126 || ch == '\n'){
			   	// fputc(ch, tfptr);
			   	// addch(ch);
			   	if (len + 1 >= cap){
			   		cap *= 2;
			   		buf = realloc(buf, cap);
			   		if(!buf){
						endwin();
						perror("Memory Allocation Renew 2 Error\n");
						exit(EXIT_FAILURE);
					}
			   	}

			   	memmove(buf + cursor + 1, buf + cursor, len - cursor);

			   	buf[cursor] = ch;
			   	cursor++;
			   	len++;
			} //else if (ch == '\n'){
			//    	fputc('\n', tfptr);
			//    	addch('\n');
			// }
		}
        
        //fflush(tfptr);
        render_buffer(buf, len);

        int y, x;
		cursor_to_yx(buf,cursor, &y, &x);
		move(y,x);
		refresh();
	}

	// if(!saved && tfptr != NULL){
	// 	//fclose(tfptr);
	// 	endwin();
	// }
	
	free(buf);
	endwin();
	if(saved == 1) printf("File saved to %s successfully\n", temp_path);
	printf("Done...\n");
	return 0;
}


// int tfptr = open(temp_path, O_RDWR, S_IRUSR | S_IWUSR);

		// //struct stat sb;

		// if(fstat(tfptr,&sb) == -1){
		// 	perror("Could not get file size.\n");
		// }

		// static char *file_in_memory = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, tfptr, 0);

		// for(int i=0; i < sb.st_size; i++){
		// 	addch(file_in_memory[i]);
		// 	refresh();
		// }

// backspace
	// if (x > 0){
			// 	mvdelch(y, x-1);
			// }else if (y > 0 && x == 0){
			// 	int prev_len = getmaxx(stdscr);
			// 	move(y-1, prev_len);
			// 	while(inch() == ' '){
			// 		prev_len -= 1;
			// 		move(y-1, prev_len);
			// 		if(prev_len == 0){
			// 			move(y-1, 0);
			// 			break;
			// 		}
			// 	}
			// 	if(prev_len != 0){
			// 		move(y-1, prev_len + 1);
			// 	}
			// 	//if(!isalnum(prev_len)){}
			// 	//move(y-1,getmaxx(stdscr) - 1);// prev_len - 1);
			// 	//delch();
			// }