#include <dos.h>
#include <stdio.h>
#include <string.h>

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

//pattern can be:
void colorize_sprite(int *sprite, int *dst, int length, int pattern){
	for(int i = 0; i < length; i++){
		dst[i] = sprite[i] & pattern;
	}
}

// stamp a sprite the size of (TODO type)
void draw_sprite(int *sprite, int dest_x, int dest_y, int w, int h){
	//TODO use the 'w' parameter -
	for(int row = 0; row < h; row++){
		//each scanline is 80 bytes long, 200 scanlines alternating
		//dst = screen (screenalt) + 80 * y
		//or _fmemcpy as it's a far pointer
		int y = dest_y + row;
		int offset = dest_x / 4;
		//to apply "w" we'd need to AND with a pattern based on w
		//_fmemcpy((y % 2 == 0 ? screen : screenalt) + ((y/2)*80) + offset , sprite+row, sizeof(int));
		char far *dstc = (char far *) (y % 2 == 0 ? screen : screenalt) + ((y/2)*80) + offset;
		int far *dst = (int far *)dstc;
//		int pattern = 0xffff >> (16-w*2);
//		int pattern = 0xc0; //sprite_width_masks[w];//0xf0ff; //0x00c0; // 0xc000;
		int pattern = sprite_width_masks[w];
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

	for(int row = 0; row < 2; row++){
	for(int i = 0; i < ROW_LENGTH; i++)
	{
		screen[i+(row*ROW_LENGTH)] = 0x1b; //00011011 pattern
		delay(1);
	}
	delay(50);
	}

	int sprite[8];
	sprite[0] = 0x3000;
	sprite[1] = 0xF000;
	sprite[2] = 0x3003;
	sprite[3] = 0x300c;
	sprite[4] = 0xfc0f;
	sprite[5] = 0x3000;
	sprite[6] = 0x3000;
	sprite[7] = 0x0000;

	int sprite_cyan[8];
	int sprite_magenta[8];
	colorize_sprite(sprite, sprite_cyan, 8, PATTERN_CYAN);
	colorize_sprite(sprite, sprite_magenta, 8, PATTERN_MAGENTA);

	draw_sprite(sprite, 16, 16, 8, 8);

	draw_sprite(sprite_cyan, 24, 24, 8, 8);

	draw_sprite(sprite_magenta, 32, 32, 8, 8);

	draw_sprite(sprite, 40, 40, 8, 8);

	int full_sprite[4];
	for(int i = 0; i < 4; i++)
		full_sprite[i] = 0xffff;

	int left_side = 0;
	for(i = 0; i <= 8; i++){
		draw_sprite(full_sprite, left_side, 10+i*4, i, 4);
//		left_side += i;
	}

	getchar();
	set_mode(0x1);
	return 0;
}