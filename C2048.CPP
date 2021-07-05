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

void set_mode(unsigned char mode){
	union REGS regs;
  regs.x.ax = (unsigned int) mode;
  int86(0x10, &regs, &regs);
}

#define ROW_LENGTH 80

char far *screen;
char far *screenalt;

int PATTERN_CYAN = 0x5555;
int PATTERN_MAGENTA = 0xAAAA;
int sprite_width_masks[9] = {0x0, 0xc0, 0xf0, 0xfc, 0xff, 0xc0ff, 0xf0ff, 0xfcff, 0xffff};
int sprite_0[8] = {0xc00f,0x3030,0x3030,0x3030,0x3030,0x3030,0xc00f,0x0000};
int sprite_1[8] = {0xc003,0xc00c,0xc000,0xc000,0xc000,0xc000,0xfc0f,0x0000};
int sprite_2[8] = {0xc00f,0x3000,0x3000,0xc000,0x0003,0x000c,0xf00f,0x0000};
int sprite_3[8] = {0xf00f,0x3000,0x3000,0xc003,0x3000,0x3000,0xc00f,0x0000};
int sprite_4[8] = {0x3000,0xf000,0x3003,0x300c,0xfc0f,0x3000,0x3000,0x0000};
int sprite_5[8] = {0xf00f,0x000c,0x000c,0xc00f,0x3000,0x3000,0xc00f,0x0000};
int sprite_6[8] = {0xf003,0x000c,0x0030,0xc033,0x303c,0x3030,0xc00f,0x0000};
int sprite_7[8] = {0xfc0f,0x3000,0x3000,0xc000,0x0003,0x0003,0x000c,0x0000};
int sprite_8[8] = {0xc00f,0x3030,0xc030,0xc00f,0x3030,0x3030,0xc00f,0x0000};
int sprite_9[8] = {0xc00f,0x3030,0x3030,0xf00f,0x3000,0xc000,0x003f,0x0000};

int *sprites[10] = {sprite_0, sprite_1, sprite_2, sprite_3, sprite_4, sprite_5,\
								 sprite_6, sprite_7, sprite_8, sprite_9};

#define SPRITE_FULL_HEIGHT 48
#define SPRITE_WIDTH 8
int sprite_full[SPRITE_FULL_HEIGHT];

int digits_0[4] = {-1,-1,-1,-1};
int digits_2[4] = {-1,-1,-1,2};
int digits_4[4] = {-1,-1,-1,4};
int digits_8[4] = {-1,-1,-1,8};
int digits_16[4] = {-1,-1,1,6};
int digits_32[4] = {-1,-1,3,2};
int digits_64[4] = {-1,-1,6,4};
int digits_128[4] = {-1,1,2,8};
int digits_256[4] = {-1,2,5,6};
int digits_512[4] = {-1,5,1,2};
int digits_1024[4] = {1,0,2,4};
int digits_2048[4] = {2,0,4,8};
int digits_4096[4] = {4,0,9,6};
int digits_8192[4] = {8,1,9,2};

int* digits(int number){
	switch(number){
		case 2: return digits_2;
		case 4: return digits_4;
		case 8: return digits_8;
		case 16: return digits_16;
		case 32: return digits_32;
		case 64: return digits_64;
		case 128: return digits_128;
		case 256: return digits_256;
		case 512: return digits_512;
		case 1024: return digits_1024;
		case 2048: return digits_2048;
		case 4096: return digits_4096;
		case 8192: return digits_8192;
		default:
			return digits_0;
	}
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


//pattern can be:
void colorize_sprite(int *sprite, int *dst, int length, int pattern){
	for(int i = 0; i < length; i++){
		dst[i] = sprite[i] & pattern;
	}
}

// stamp a sprite the size of (TODO type)
// for now dest_x needs to be aligned by 4
void draw_sprite(int *sprite, int dest_x, int dest_y, int w, int h){
	for(int row = 0; row < h; row++){
		//each scanline is 80 bytes long, 200 scanlines alternating
		int y = dest_y + row;
		int offset = dest_x >> 2; //4
		//_fmemcpy((y % 2 == 0 ? screen : screenalt) + ((y/2)*80) + offset , sprite+row, sizeof(int));
		char far *dstc = (char far *) (y % 2 == 0 ? screen : screenalt) + ((y/2)*80) + offset;
		int far *dst = (int far *)dstc;
		int pattern = sprite_width_masks[w];
		//to apply "w" we'd need to AND with a pattern based on w
		*dst ^= (*(sprite+row) & pattern);
	}
}

void setup_graphics(){
	set_mode(0x4);
	screen = (char far*) MK_FP(0xB800,0);
	screenalt = (char far*) MK_FP(0xB800,0x2000);
	for(int j = 0; j < 48; j++)
		sprite_full[j] = 0xffff;
}

void fill_block(int x, int y, int* sprite){
	//TODO precalculate dithered patterns on start
	//block is already 48 px tall
	int dst_y = y*48;
	for(int dst_x = x*48; dst_x < (x+1)*48; dst_x+=SPRITE_WIDTH){
		draw_sprite(sprite, dst_x, dst_y, SPRITE_WIDTH, SPRITE_FULL_HEIGHT);
	}
}

void draw_block(int x, int y){
	//fill the outline
	if(is_blank(x,y)){
	 //todo clear the block - we probably need a black sprite

	}
	else{
		int mysprite[48];
		colorize_sprite(sprite_full, mysprite, SPRITE_FULL_HEIGHT, PATTERN_CYAN);
		fill_block(x,y, mysprite);
		//draw the value
		int value = tiles[I(x,y)];
		int* num_digits = digits(value);
		for(int d = 0; d < 4; d++){
			int sprite_num = num_digits[d];
			if(sprite_num == -1)
				continue;
			draw_sprite(sprites[num_digits[d]], x*48+d*8+4, y*48+16, 8, 8);
		}
//		printf("|%4d", tiles[I(x,y)]);
	}

	//horizontal
	//draw the digits

}

//void vertical_line(int x, int y,

void draw_grid(){
	for(int i = 0; i <= 4; i++){
	 //horizontal lines
	 for(int x = 0; x < 192; x+=8)
		 draw_sprite(sprite_full,x, i*48, 8, 1);
	 //vertical line
	 for(int y = 0; y < 192; y+=SPRITE_FULL_HEIGHT)
		 draw_sprite(sprite_full,i*48, y, 1, SPRITE_FULL_HEIGHT);
	}
}

void draw_blocks(){
	for(int y = 0; y < 4; y++){
		for(int x = 0; x < 4; x++){
		 draw_block(x,y);
		}
	}
}

int main(void){
	setup_graphics();
	new_game();
	draw_grid();
	getch();
	draw_blocks();
	getch();

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