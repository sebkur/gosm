/* GOSM - the Gtk OpenStreetMap Tool
 *
 * Copyright (C) 2009  Sebastian Kuerten
 *
 * This file is part of Gosm.
 *
 * Gosm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gosm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gosm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include <png.h>

int create_read_structs(FILE * fp, png_structp * png_ptr, png_infop * info_ptr)
{
	if (fp == NULL){
		return -1;
	}
	png_byte buffer[8];
	fread(&buffer, 1, 8, fp);
	int is_png = !png_sig_cmp(FAR buffer, 0, 8);
	if (!is_png){
		printf("no png\n");
		return -1;
	}
	*png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr){
		printf("cannot create png_structp\n");
		return -1;
	}
	*info_ptr = png_create_info_struct(*png_ptr);
	if (!info_ptr){
		printf("cannot create png_infop: info_ptr\n");
		return -1;
	}
	png_infop end_info = png_create_info_struct(*png_ptr);
	if (!end_info){
		printf("cannot create png_infop: end_ptr\n");
		return -1;
	}
	png_init_io(*png_ptr, fp);
	png_set_sig_bytes(*png_ptr, 8);

	png_uint_32 width, height;
	int depth, color, interlace, compression, filter;
	png_read_info(*png_ptr, *info_ptr);
	png_get_IHDR(*png_ptr, *info_ptr, &width, &height, &depth, &color, &interlace, &compression, &filter);
	if (color == PNG_COLOR_TYPE_PALETTE){
		png_set_palette_to_rgb(*png_ptr);
	}
	//png_read_png(*png_ptr, *info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	return 0;
}

int create_write_structs(FILE * fp, png_structp * png_ptr, png_infop * info_ptr, png_colorp * palette, int width, int height)
{
	if (fp == NULL){
		return -1;
	}
	*png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (*png_ptr == NULL){
		return -1;
	}
	*info_ptr = png_create_info_struct(*png_ptr);
	if (*info_ptr == NULL){
		png_destroy_write_struct(png_ptr, png_infopp_NULL);
		return -1;
	}
	if (setjmp(png_jmpbuf(*png_ptr))){
		png_destroy_write_struct(png_ptr, info_ptr);
		return -1;
	}
	png_init_io(*png_ptr, fp);
	//png_set_IHDR(*png_ptr, *info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
	png_set_IHDR(*png_ptr, *info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_color_8 sig_bit;
	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	//sig_bit.alpha = 8;
	png_set_sBIT(*png_ptr, *info_ptr, &sig_bit);
	png_write_info(*png_ptr, *info_ptr);
	return 0;
}

int main(int argc, char *argv[])
{
	/*if (argc < 2){
		printf("usage: libpng <filename>\n");
		return EXIT_FAILURE;
	}
	printf("libpng\n");*/
	int zoom = 11;
	int x1 = 1100, x2 = 1102;
	int y1 = 671,  y2 = 673;
	int blen = 18 + 9;
	int cx = x2 - x1 + 1, cy = y2 - y1 + 1;
	char namespace[cx * cy][blen];
	int ix, iy;
	for (ix = 0; ix < cx; ix++){
		for (iy = 0; iy < cy; iy++){
			sprintf(namespace[cx * iy + ix], "/tmp/osm/%d_%d_%d.png", zoom, x1 + ix, y1 + iy);
			printf("%s\n", namespace[cx * iy + ix]);
		}
	}

	png_structp png_ptr;
	png_infop info_ptr;
	FILE *fp;

	png_structp png_ptr_write;
	png_infop info_ptr_write;
	png_colorp palette_write;
	FILE *fp_write = fopen("test.png", "wb");
	int s_w = create_write_structs(fp_write, &png_ptr_write, &info_ptr_write, &palette_write, 256, 512);
	if (s_w != 0){
		fclose(fp_write);
		return EXIT_FAILURE;
	}
	// read/write
	//char * names[2] = {"next.png", "prev.png" };
	char * names[2] = {"11_1100_671.png", "11_1100_672.png"};
	int i, k;
	for (i = 0; i < 2; i++){
		k = i % 2;
		fp = fopen(names[k], "rb");
		int s_r = create_read_structs(fp, &png_ptr, &info_ptr);
		if (s_r != 0){
			return EXIT_FAILURE;
		}
		png_bytep row_pointers[256];
		int r; for (r = 0; r < 256; r++){
			row_pointers[r] = malloc(256*3*sizeof(png_byte));
		}
		png_read_image(png_ptr, row_pointers);
		//png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
		int x, y;
		for(y = 0; y < info_ptr -> height; y++){
			png_write_rows(png_ptr_write, &row_pointers[y], 1);
		}
		fclose(fp);
	}
	png_write_end(png_ptr_write, info_ptr_write);
	fclose(fp_write);

	/*
	printf("Width: %d\n", info_ptr -> width);
	printf("Height: %d\n", info_ptr -> height);
	png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
	int x, y;
	for(y = 0; y < info_ptr -> height; y++){
		for(x = 0; x < info_ptr -> width * 4; x++){
			printf("%d ", row_pointers[y][x]);
		}
		printf("\n");
	}
	*/
	printf("%d\n", s_w);
	printf("%d\n", info_ptr -> valid);

	return EXIT_SUCCESS;
}
