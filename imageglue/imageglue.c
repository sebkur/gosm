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
#include <string.h>
#include <glib.h>

#include "../tiles/tilemath.h"

/****************************************************************************************************
* these functions provide easyer access to the parts of libpng needed by ImageGlue
****************************************************************************************************/

/****************************************************************************************************
* create png_structs for reading for the given FILE.
* return 0 on success, negative otherwise
****************************************************************************************************/
int create_read_structs(FILE * fp, png_structp * png_ptr, png_infop * info_ptr)
{
	if (fp == NULL){
		printf("fp NULL\n");
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
	png_set_strip_alpha(*png_ptr); // Cycle map is RGBA, alpha needs to be stripped
	//png_read_png(*png_ptr, *info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	return 0;
}

/****************************************************************************************************
* create png_structs for writing for the given FILE.
* return 0 on success, negative otherwise
****************************************************************************************************/
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
/*
int make_image(char * filename, char * cache_dir, int zoom, double lon1, double lon2, double lat1, double lat2) 
{
	double x1_d = lon_to_x(lon1, zoom);
	double x2_d = lon_to_x(lon2, zoom);
	double y1_d = lat_to_y(lat1, zoom);
	double y2_d = lat_to_y(lat2, zoom);
	int x1 = (int) x1_d;
	int x2 = (int) x2_d;
	int y1 = (int) y1_d;
	int y2 = (int) y2_d;
	int x1_o = (x1_d - x1) * 256;
	int x2_o = (x2_d - x2) * 256;
	int y1_o = (y1_d - y1) * 256;
	int y2_o = (y2_d - y2) * 256;
	//printf("%d %d %d %d\n", x1, x2, y1, y2);
	//printf("%d %d %d %d\n", x1_o, x2_o, y1_o, y2_o);
	make_image_1(filename, cache_dir, zoom, x1, x2, y1, y2, x1_o, x2_o, y1_o, y2_o);
}

// just for a small commandline-tool for easy handling
int make_image_simple(char * filename, int zoom, int x1, int x2, int y1, int y2)
{
	make_image_1(filename, zoom, x1, x2, y1, y2, 0, 0, 256, 256);
}

int make_image_1(char * filename, char * cache_dir, int zoom,
			int x1,   int x2,   int y1,   int y2,
			int x1_o, int x2_o, int y1_o, int y2_o){
	char * file_format = "/%d_%d_%d.png";
	char * format = malloc(sizeof(char) * (strlen(cache_dir) + strlen(file_format) + 1));
	strcpy(format, cache_dir);
	strcpy(format + strlen(cache_dir), file_format);
	int blen = 18 + 19;
	int cx = x2 - x1 + 1;
	int cy = y2 - y1 + 1;
	char namespace[cx * cy][blen];
	int ix, iy;
	for (ix = 0; ix < cx; ix++){
		for (iy = 0; iy < cy; iy++){
			sprintf(namespace[cx * iy + ix], format, zoom, x1 + ix, y1 + iy);
			printf("%s\n", namespace[cx * iy + ix]);
		}
	}
	int width  = cx * 256 - x1_o - 256 + x2_o;
	int height = cy * 256 - y1_o - 256 + y2_o;

	png_structp png_ptr;
	png_infop info_ptr;
	FILE *fp;

	png_structp png_ptr_write;
	png_infop info_ptr_write;
	png_colorp palette_write;
	FILE *fp_write = fopen(filename, "wb");
	int s_w = create_write_structs(fp_write, &png_ptr_write, &info_ptr_write, &palette_write, width, height);
	if (s_w != 0){
		fclose(fp_write);
		return EXIT_FAILURE;
	}
	// read/write
	int n;
	png_bytep rows[256];
	int r; for (r = 0; r < 256; r++){
		rows[r] = malloc(cx*256*3*sizeof(png_byte));
	}
	gboolean first_x, first_y, last_x, last_y;
	for (iy = 0; iy < cy; iy++){
		first_y = iy == 0 ? TRUE : FALSE;
		last_y  = iy + 1 == cy ? TRUE : FALSE;
		for (ix = 0; ix < cx; ix++){
			first_x = ix == 0 ? TRUE : FALSE;
			last_x  = ix + 1 == cx ? TRUE : FALSE;
			n = iy * cx + ix;
			printf("%s\n", namespace[n]);
			fp = fopen(namespace[n], "rb");
			png_bytep row_pointers[256];
			int r; for (r = 0; r < 256; r++){
				row_pointers[r] = &(rows[r][256*3*ix]);
			}
			int s_r = create_read_structs(fp, &png_ptr, &info_ptr);
			if (s_r != 0){
				printf("error opening %s\n", namespace[n]);
				int k; for (k = 0; k < 256; k++){
					memset(row_pointers[k], 255, 256*3);
				}
			}else{
				png_read_image(png_ptr, row_pointers);
			}
			if (fp != NULL){
				fclose(fp);
			}
		}
		int x, y, ly;
		y  = first_y ? y1_o : 0;
		ly = last_y  ? y2_o : 256; 
		for(; y < ly; y++){
			png_byte * pos = &rows[y][x1_o * 3];
			png_write_rows(png_ptr_write, &pos, 1);
		}
	}
	png_write_end(png_ptr_write, info_ptr_write);
	fclose(fp_write);

	printf("%d\n", s_w);
	//printf("%d\n", info_ptr -> valid);

	return EXIT_SUCCESS;
}
*/
/*int main(int argc, char *argv[])
{
	//imageglue <zoom> <x1> <x2> <y1> <y2> <filename>
	int zoom = 11;
	int x1 = 1089, x2 = 1091;
	int y1 = 670,  y2 = 672;
	make_image("test.png", zoom, x1, x2, y1, y2);
}*/
