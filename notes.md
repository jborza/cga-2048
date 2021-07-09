Multiple operations: 
	- Spawn new block (always 2?)
	- Move left/right/up/down
	- Merge with the move
	- Check for game over
	- New game: spawn two blocks
	

Drawing:
	- Should I do a prototype in JS + Canvas?
	- Sprite engine - similar to chip-8?
		○ Byte encoded sprites, 2 bits per pixel
		○ How to deal with alternating rows?
 
		
		
Drawing sprites of specified width:
	- Sprites are arrays of 16-bit ints, 8 pixels wide
	int PATTERN_CYAN = 0x5555;
	int PATTERN_MAGENTA = 0xAAAA;
	int sprite_width_masks[9] = {0x0, 0xc0, 0xf0, 0xfc, 0xff, 0xc0ff, 0xf0ff, 0xfcff, 0xffff};
	
	

Tiles index:
0	1	2	3
4	5	6	7
8	9	10	11
12	13	14	15


GUI
4x4 grid
	- Blocks have changing font color based on the exponent
		○ 2,4,8,16 (colors then repeat)
		○ Make a sprite for the individual numbers?
	- Start with a premade grid to make it easier
	- No animation

We only have two colors to work with:
	- Black
	- White
	- Cyan #55ffff
	- Magenta #ff55ff

Could we go with dithering?
	- We can enumerate 8 combinations that are not pure black or white:
		○ Black/white
		○ Black/cyan
		○ Cyan
		○ White/cyan
		○ White/magenta
		○ Magenta
		○ Black/magenta
		○ Magenta/cyan
	- We assign them to 2,4,8,16,32,64,128,256 … 512,1024,2048,4096,8192,…

- implemented a sprite generator (from pngs) - see the tools/spritegen folder

for example:
4 is encoded as:
int sprite_4[8] = {0x3000,0xf000,0x3003,0x300c,0xfc0f,0x3000,0x3000,0x0000};

note: The bytes are swapped


Font used for sprites: Lucida Console 8pt sharp
 
