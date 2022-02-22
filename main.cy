#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

unsigned char *frontbuffer;

void putpixel(int x, int y, unsigned char *rgb, int pitch){
	unsigned char *memory = frontbuffer;
	memory += y*pitch+x;
	*memory++ = rgb[0];
	*memory++ = rgb[1];
	*memory++ = rgb[2];
}



void save_bmp(int width, int height){
	FILE *img = fopen("img.bmp","wb");

	BMP b;
	memset(&b, 0, sizeof(BMP));
	b.bformat.type = 0x4D42;
	b.bformat.size = sizeof(BMP)+(width*height*3);
	b.bformat.offset_bits = sizeof(BMP);
	b.binfo.size = sizeof(BMP_INFO);
	b.binfo.width = width;
	b.binfo.height = height;
	b.binfo.count = 24;
	b.binfo.size_imag = width * height * 3;

	unsigned char *p = frontbuffer;
	p +=width * height *3;
	fwrite(&b,1, sizeof(BMP), img);
	for(int y=0; y < height; y++){
                for(int x=0; x < width; x++){
                      	fwrite(p--, 1, 1, img);
			fwrite(p--, 1, 1, img);
			fwrite(p--, 1, 1, img);
                }
        }



	fflush(img);
        fclose(img);

}

int main(){

	frontbuffer = (unsigned char *) malloc(0x400000);
	memset(frontbuffer, 255, 0x400000);
	int width = 256;
	int height = 256;


	for(int y=0;y < height; y++){
                for(int x=0; x < width; x++){
			unsigned char rgb[3];
			float r = 0;
			float g = 0;
			float b = 0;

			rgb[0] = (unsigned char)b;
			rgb[1] = (unsigned char)g;
			rgb[2] = (unsigned char)r;
                	putpixel(x, y, rgb, width);
                }
        }

	save_bmp(width, height);
	return 0;
}
