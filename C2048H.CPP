//2048
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>

#define WIDTH 4
#define HEIGHT 4
#define TEST

int tiles [WIDTH * HEIGHT]; //tile storage
long score;
char far *screen;

void set_mode(unsigned char mode){
	union REGS regs;
  regs.x.ax = (unsigned int) mode;
  int86(0x10, &regs, &regs);
}


int I(int x, int y){
 return y*WIDTH+x; //TODO macro
}

void spawn_block(){
retry:
	int x,y;
	x = rand() % WIDTH;
	y = rand() % HEIGHT;
	int index = I(x,y);
	if(tiles[index] != 0)
		goto retry;
	tiles[index] = 2;
}

void new_game(){
	for(int y = 0; y < HEIGHT; y++){
	 for(int x = 0; x < WIDTH; x++){
		tiles[I(x,y)] = 0;
	 }
	}
	//spawn 2 random blocks

#ifdef TEST
	//debugging board with fixed layout
	tiles[4] = 2048;
	tiles[5] = 1024;
	tiles[6] = 512;
	tiles[7] = 256;
	tiles[8] = 8;
	tiles[9] = 4;
	tiles[10] = 2;
	tiles[12] = 128;
	tiles[13] = 64;
	tiles[14] = 32;
	tiles[15] = 16;
#else
	spawn_block();
	spawn_block();
#endif
}

int is_blank(int x, int y){
	return tiles[I(x,y)] == 0;
}

void print_game(){
 printf("\n *2048*       Score: %d\n", score);
 printf("\n-------------------------\n");
 for(int y = 0; y < HEIGHT; y++){
	for(int x = 0; x < WIDTH; x++){
		if(is_blank(x,y))
			printf("|    ");
		else
			printf("|%4d", tiles[I(x,y)]);
	}
	printf("|\n-------------------------\n");
}
}

void move(int x, int y, int dx, int dy, int fence_x, int fence_y, int &merged_rowcol){
	if(is_blank(x,y))
			return;
		//try to move this block all the way in this direction
		//and merge if we hit the same value (4 <- 4)

		//check how much to the direction can we move
		int moveto_x = x, moveto_y = y;
		if(dx != 0){
			while(moveto_x != fence_x && is_blank(moveto_x+dx, moveto_y))
				moveto_x += dx;
		}
		if(dy != 0){
			while(moveto_y != fence_y && is_blank(moveto_x, moveto_y+dy))
				moveto_y += dy;
		}

		//move to furthest coord first
		if(is_blank(moveto_x, moveto_y)){
		 tiles[I(moveto_x,moveto_y)] = tiles[I(x,y)];
		 tiles[I(x,y)] = 0;
		}

		//now try to merge to the left with the nonblank tile
		if(dy != 0 && moveto_y == fence_y)
			return; //don't merge over the wall
		if(dx != 0 && moveto_x == fence_x)
			return; //don't merge over the wall

		int merge_x = moveto_x + dx;
		int merge_y = moveto_y + dy;
		if(tiles[I(moveto_x,moveto_y)] == tiles[I(merge_x,merge_y)]){
		 if(!merged_rowcol){
			 tiles[I(merge_x,merge_y)] = tiles[I(moveto_x,moveto_y)] * 2;
			 score += tiles[I(merge_x,merge_y)];
			 tiles[I(moveto_x,moveto_y)] = 0;
			 merged_rowcol = 1;
		 }
		}
}

void move_up(){
	int dx = 0;
	int dy = -1;
	int fence_x = -1; //must be invalid for Y move
	int fence_y = 0;

	for(int y = 1; y < HEIGHT; y++){
	 //merge only one cell in the row/column at once
	 int merged_rowcol = 0;
	 for(int x = 0; x < WIDTH; x++){

		move(x,y,dx,dy,fence_x,fence_y,merged_rowcol);
	 } //for y
	} //for x
	spawn_block();
}

void move_down(){
	int dx = 0;
	int dy = 1;
	int fence_x = -1; //must be invalid for Y move
	int fence_y = 3;

	for(int y = 2; y >= 0; y--){
	 //merge only one cell in the row/column at once
	 int merged_rowcol = 0;
	 for(int x = 0; x < WIDTH; x++){

		move(x,y,dx,dy,fence_x,fence_y,merged_rowcol);
	 } //for y
	} //for x
	spawn_block();
}

void move_right(){
	int dx = 1;
	int dy = 0;
	int fence_x = 3; //must be invalid for Y move
	int fence_y = -1;

	for(int x = 2; x >=0; x--){
	 //merge only one cell in the row/column at once
	 int merged_rowcol = 0;
	 for(int y = 0; y < HEIGHT; y++){
		move(x,y,dx,dy,fence_x,fence_y,merged_rowcol);
	 } //for y
	} //for x
	spawn_block();
}

void move_left(){
	int dx = -1;
	int dy = 0;
	int fence_x = 0; //must be invalid for Y move
	int fence_y = -1;

	for(int x = 1; x < WIDTH; x++){
	 //merge only one cell in the row/column at once
	 int merged_rowcol = 0;
	 for(int y = 0; y < HEIGHT; y++){
		move(x,y,dx,dy,fence_x,fence_y,merged_rowcol);
	 } //for y
	} //for x
	spawn_block();
}

void setup_graphics(){
	set_mode(0x4);
	screen = (char far*) MK_FP(0xB800,0);
}

int main(void){
	setup_graphics();
	new_game();

  while(1){
		print_game();
		int key = getch();
		switch(key){
			//let's do WASD instead of arrow keys and worry about keycodes later
			case 'a':
				move_left();
				break;
			case 'w':
				move_up();
				break;
			case 's':
				move_down();
				break;
			case 'd':
				move_right();
				break;
			case 'q':
				return 0;
		}

		print_game();
//    break; //I don't want to restart my computer yet to exit the program ;)
	}

//	return 0;
}