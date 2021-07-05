#include <stdio.h>

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

int main(void){
	int no = 256;
	printf("digits of %d: ",no);
	int* digs = digits(no);
	for(int i = 0; i < 4; i++){
	 printf("%d ",digs[i]);
	}
	printf("\n");
	return 0;
}