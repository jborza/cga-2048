#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void set_mode(unsigned char mode){
	union REGS regs;
	regs.x.ax = (unsigned int) mode;
	int86(0x10, &regs, &regs);
}

//mode 4, setting random bytes

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

int sprite_full[48];

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
		int offset = dest_x / 4;
		//_fmemcpy((y % 2 == 0 ? screen : screenalt) + ((y/2)*80) + offset , sprite+row, sizeof(int));
		char far *dstc = (char far *) (y % 2 == 0 ? screen : screenalt) + ((y/2)*80) + offset;
		int far *dst = (int far *)dstc;
		int pattern = sprite_width_masks[w];
		//to apply "w" we'd need to AND with a pattern based on w
		*dst = (*(sprite+row) & pattern);
	}
}

main(){
	set_mode(0x4);
	screen = (char far*) MK_FP(0xB800,0);
	screenalt = (char far*) MK_FP(0xB800,0x2000);

	screenalt[0] = 0xff;
	screenalt[1] = 0x33;
	screenalt[2] = 0x66;
	screenalt[3] = 0xcc;

	for(int j = 0; j < 48; j++)
		sprite_full[j] = 0xffff;

	for(int row = 0; row < 2; row++){
	for(int i = 0; i < ROW_LENGTH; i++)
	{
		screen[i+(row*ROW_LENGTH)] = 0x1b; //00011011 pattern
		screenalt[i+(row*ROW_LENGTH)] = 0xd8;
		delay(1);
	}
	delay(50);
	}

	int sprite_cyan[8];
	int sprite_magenta[8];
	colorize_sprite(sprite_0, sprite_cyan, 8, PATTERN_CYAN);
	colorize_sprite(sprite_1, sprite_magenta, 8, PATTERN_MAGENTA);

	draw_sprite(sprite_2, 16, 16, 8, 8);

	draw_sprite(sprite_cyan, 24, 24, 8, 8);

	draw_sprite(sprite_magenta, 32, 32, 8, 8);

	draw_sprite(sprite_3, 40, 40, 8, 8);

	int full_sprite[4];
	for(int i = 0; i < 4; i++)
		full_sprite[i] = 0xffff;

	int left_side = 0;
	for(i = 0; i <= 8; i++){
		draw_sprite(sprite_full, left_side, 10+i*4, i, 4);
//		left_side += i;
	}


	int* rnd_sprite;
	for(i = 0; i < 10000; i++)
	{
	rnd_sprite = sprites[random(10)];
	int col = random(3);
	if(col > 0)
		colorize_sprite(rnd_sprite, sprite_cyan, 8, col==1?PATTERN_CYAN:PATTERN_MAGENTA);

	draw_sprite(col?sprite_cyan:rnd_sprite, random(320), random(200), 8, 8);
	}
	getchar();
	set_mode(0x1);
	return 0;
}