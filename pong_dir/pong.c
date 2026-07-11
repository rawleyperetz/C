#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 500

#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 50
#define PADDLE_SPEED 10

#define BALL_WIDTH 5
#define BALL_HEIGHT 5

#define FONT_SIZE 14
//#define BALL_SPEED 2

typedef struct{
	int x;
	int y;
	int width;
	int height;
} Paddle;

typedef struct{
	int x;
	int y;
	int width;
	int height;
	int dx;
	int dy;
} Ball;


typedef struct{
	SDL_Renderer *renderer;
	SDL_Window *window;
	Paddle padOne;
	Paddle padTwo;
	Ball ball;
	int scoreOne;
	int scoreTwo;
	TTF_Font *font;
} App;




App app;
int keyState[SDL_NUM_SCANCODES] = {0};




void initSDL(void){
	if (TTF_Init() < 0){
	    printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
	    exit(1);
	}

	app.font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", FONT_SIZE);

	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	
	app.window = SDL_CreateWindow("Trad Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if(!app.window){
		printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	if(!app.renderer){
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}
}

void doInput(void){
	SDL_Event event;

	while(SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_QUIT:
				exit(0);
				break;
			case SDL_KEYUP:
				keyState[event.key.keysym.scancode] = 0;
				break;
			case SDL_KEYDOWN:
				keyState[event.key.keysym.scancode] = 1;
				break;
			case SDLK_w:
				keyState[event.key.keysym.scancode] = 0;
				break;
			case SDLK_s:
				keyState[event.key.keysym.scancode] = 1;
				break;
			default:
				break;
		}
	}
}



void drawScore(void){
	SDL_Color white = {255, 255, 255, 255};
	char scoreText[20];
	sprintf(scoreText, "%d    %d", app.scoreOne, app.scoreTwo);
	SDL_Surface *surface = TTF_RenderText_Solid(app.font, scoreText, white);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(app.renderer, surface);
	SDL_FreeSurface(surface);
	int textW, textH;
	SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
	SDL_Rect dst = {(SCREEN_WIDTH / 2) - (textW / 2), 10, textW, textH};
	SDL_RenderCopy(app.renderer, texture, NULL, &dst);
	SDL_DestroyTexture(texture);
}

void prepareScene(void){
	SDL_RenderClear(app.renderer);
	//SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255); // draw to black
	//SDL_SetRenderDrawColor(app.padOne, 255, 255, 255, 1);
	
	SDL_Rect rectOne = {app.padOne.x, app.padOne.y, app.padOne.width, app.padOne.height};
	SDL_Rect rectTwo = {app.padTwo.x, app.padTwo.y, app.padTwo.width, app.padTwo.height};
	
	SDL_Rect ballCirc = {app.ball.x, app.ball.y, app.ball.width, app.ball.height};
	
	SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255); // this is white

	
	
	SDL_RenderFillRect(app.renderer, &rectOne);
	SDL_RenderFillRect(app.renderer, &rectTwo);
	
	SDL_RenderFillRect(app.renderer, &ballCirc);

	drawScore();
	
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	
}

void presentScene(void){
	SDL_RenderPresent(app.renderer);
}

void cleanup(void){
	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyWindow(app.window);
	SDL_Quit();
}
// 
int maxFunc(int a, int b)
{
    int z, i, max;

    // Perform the subtraction
    z = a - b;

    // Right shift and Bitwise AND
    i = (z >> 31) & 1;

    // Find the maximum number
    max = a - (i * z);

    // Return the maximum value
    return max;
}


// float maxFunc(float a, float b){
//     return (a > b) ? a : b;
// }

void update(void){

	int bottom_boundary = SCREEN_HEIGHT - PADDLE_HEIGHT;
	
	if(keyState[SDL_SCANCODE_W]) {
		app.padOne.y -= PADDLE_SPEED;
		if(app.padOne.y < 0){
			app.padOne.y = 0;
		}
	}
	if(keyState[SDL_SCANCODE_S]){
		app.padOne.y += PADDLE_SPEED;
		if(app.padOne.y >= bottom_boundary){
			app.padOne.y = bottom_boundary;
		}
	} 
	if(keyState[SDL_SCANCODE_UP]){
		app.padTwo.y -= PADDLE_SPEED;
		if(app.padTwo.y < 0){
			app.padTwo.y = 0;
		}
	} 
	if(keyState[SDL_SCANCODE_DOWN]){
		app.padTwo.y += PADDLE_SPEED;
		if(app.padTwo.y >= bottom_boundary){
			app.padTwo.y = bottom_boundary;
		}	
	}

	app.ball.x += app.ball.dx;
	app.ball.y += app.ball.dy; 

	if(app.ball.y < 0 || app.ball.y >= SCREEN_HEIGHT - BALL_HEIGHT){
		app.ball.dy = -app.ball.dy;
		app.ball.y += app.ball.dy;
	}


	SDL_Rect rectOne = {app.padOne.x, app.padOne.y, app.padOne.width, app.padOne.height};
	SDL_Rect rectTwo = {app.padTwo.x, app.padTwo.y, app.padTwo.width, app.padTwo.height};
	
	SDL_Rect ballCirc = {app.ball.x, app.ball.y, app.ball.width, app.ball.height};

	if(SDL_HasIntersection(&rectOne, &ballCirc)){
	 	app.ball.dx = - app.ball.dx + (rand() % 2);
		app.ball.x += app.ball.dx;
	}

	if(SDL_HasIntersection(&rectTwo, &ballCirc)){
	 	app.ball.dx = - app.ball.dx - (rand() % 2);
		app.ball.x += app.ball.dx;
	}
	// Collision of paddle with ball
	// using L-infinity norm

// apparently L-infinity norm works but I need to offset since padOne.x and padOne.y is the top left corner of the pad 
// 	int half_width = PADDLE_WIDTH >> 1;
// 	int half_height = PADDLE_HEIGHT >> 1 ;
// 
// 	if(maxFunc(abs(app.ball.x - app.padOne.x)/half_width, abs(app.ball.y - app.padOne.y)/half_height) <= 1){
// 		app.ball.dx = - app.ball.dx + (rand() % 2);
// 		app.ball.x += app.ball.dx;
// 	}
// 	 
// 	if(maxFunc(abs(app.ball.x - app.padTwo.x)/half_width, abs(app.ball.y - app.padTwo.y)/half_height) <= 1){
// 		app.ball.dx = -app.ball.dx - (rand() % 2);
// 		app.ball.x += app.ball.dx;
// 	}

	

	if(app.ball.x < 0){
		app.ball.dy = 1;
		app.ball.dx = 1;
		app.ball.x = (SCREEN_WIDTH >> 1) + app.ball.dx;
		app.ball.y = (SCREEN_HEIGHT >> 1) + app.ball.dy;
		app.scoreTwo += 1;
	}

	if(app.ball.x > SCREEN_WIDTH){
		app.ball.dy = 1;
		app.ball.dx = 1;
		app.ball.x = (SCREEN_WIDTH >> 1) - app.ball.dx;
		app.ball.y = (SCREEN_HEIGHT >> 1) - app.ball.dy;
		app.scoreOne += 1;
	}



// 	if (abs(app.ball.x - app.padTwo.x) <= half_width && abs(app.ball.y - app.padTwo.y) <= half_height){
// 	    app.ball.dx = -app.ball.dx - 1;
// 		app.ball.x += app.ball.dx;
// 	}
// 
// 	if (abs(app.ball.x - app.padOne.x) <= half_width && abs(app.ball.y - app.padOne.y) <= half_height){
// 	    app.ball.dx = -app.ball.dx + 1;
// 		app.ball.x += app.ball.dx;
// 	}
	// int half_width = PADDLE_WIDTH >> 1;
	// int half_height = PADDLE_HEIGHT >> 1;
}


int main(){

	srand(time(NULL));
	
	memset(&app, 0, sizeof(App));

	initSDL();

	atexit(cleanup);

	app.padOne.width = PADDLE_WIDTH;
	app.padTwo.width = PADDLE_WIDTH;
	
	app.padOne.height = PADDLE_HEIGHT;
	app.padTwo.height = PADDLE_HEIGHT;
	
	app.padTwo.x = SCREEN_WIDTH - PADDLE_WIDTH;
	app.padOne.x = 0;
	
	app.padOne.y = (SCREEN_HEIGHT - PADDLE_HEIGHT) >> 1;
	app.padTwo.y = (SCREEN_HEIGHT - PADDLE_HEIGHT) >> 1;

	app.ball.x = SCREEN_WIDTH >> 1;
	app.ball.y = SCREEN_HEIGHT >> 1;

	app.ball.width = BALL_WIDTH;
	app.ball.height = BALL_HEIGHT;
	
	app.ball.dx = 1;
	app.ball.dy = -1;

	
	while(1){
		prepareScene();
		doInput();
		update();
		presentScene();
		SDL_Delay(16);
	}

	return 0;
}
