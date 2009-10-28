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
#include <unistd.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <png.h>
#include <pthread.h>
#include <math.h>

#include "imageglue.h"
#include "image_glue.h"
#include "../tiles/tilemath.h"
#include "../map_types.h"
#include "../atlas/atlas.h"

/****************************************************************************************************
* ImageGlue uses libpng to 'glue' some tiles together to form larger images
****************************************************************************************************/
G_DEFINE_TYPE (ImageGlue, image_glue, G_TYPE_OBJECT);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
        TILE_COMPLETED,
        LAST_SIGNAL
};

static guint image_glue_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* function declarations
****************************************************************************************************/
void image_glue_single_thread_function(ImageGlue * image_glue);
void image_glue_sequence_thread_function(ImageGlue * image_glue);

/****************************************************************************************************
* constructor
****************************************************************************************************/
ImageGlue * image_glue_new()
{
	ImageGlue * image_glue = g_object_new(GOSM_TYPE_IMAGE_GLUE, NULL);
	return image_glue;
}

static void image_glue_class_init(ImageGlueClass *class)
{
        image_glue_signals[TILE_COMPLETED] = g_signal_new(
                "tile-completed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ImageGlueClass, tile_completed),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
}

static void image_glue_init(ImageGlue *image_glue)
{
}

/****************************************************************************************************
* the number of tiles that an area given by lons/lats/zoom covers
****************************************************************************************************/
int image_glue_single_get_number_of_tiles(int zoom, double lon1, double lon2, double lat1, double lat2)
{
	double x1_d = lon_to_x(lon1, zoom);
	double x2_d = lon_to_x(lon2, zoom);
	double y1_d = lat_to_y(lat1, zoom);
	double y2_d = lat_to_y(lat2, zoom);
	int x1 = (int) x1_d;
	int x2 = (int) x2_d;
	int y1 = (int) y1_d;
	int y2 = (int) y2_d;
	return (x2 - x1 + 1) * (y2 - y1 + 1);
}

/****************************************************************************************************
* the number of tiles that an area given by a selection covers
* this function is for generating multiple images, where the number of tiles may be greater than at
* image_glue_single_get_number_of_tiles, since all images are concerned that are (maybe paritially)
* covered by the selection
****************************************************************************************************/
int image_glue_sequence_get_number_of_tiles(int zoom, Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y)
{
	int count = 0;
	double x1_d = lon_to_x(selection.lon1, zoom);
	double x2_d = lon_to_x(selection.lon2, zoom);
	double y1_d = lat_to_y(selection.lat1, zoom);
	double y2_d = lat_to_y(selection.lat2, zoom);
	int x1 = (int) x1_d;
	int x2 = (int) x2_d;
	int y1 = (int) y1_d;
	int y2 = (int) y2_d;
	int x1_o = (x1_d - x1) * 256;
	int x2_o = (x2_d - x2) * 256;
	int y1_o = (y1_d - y1) * 256;
	int y2_o = (y2_d - y2) * 256;
	int width  = (x2-x1+1) * 256 - x1_o - 256 + x2_o;
	int height = (y2-y1+1) * 256 - y1_o - 256 + y2_o;
	int parts_x = 1 + ceil(((double)(width - image_dimension.width)) / (image_dimension.width - intersect_x));
	int parts_y = 1 + ceil(((double)(height - image_dimension.height)) / (image_dimension.height - intersect_y));
	int a,b;
	int step_x = image_dimension.width - intersect_x;
	int step_y = image_dimension.height - intersect_y;
	for (a = 0; a < parts_x; a++){
		int s_x = x1 + (x1_o + a * step_x) / 256;
		int s_x_o = (x1_o + a * step_x) % 256;
		int e_x = s_x + (s_x_o + image_dimension.width) / 256;
		int e_x_o = (s_x_o + image_dimension.width) % 256;
		for (b = 0; b < parts_y; b++){
			int s_y = y1 + (y1_o + b * step_y) / 256;
			int s_y_o = (y1_o + b * step_y) % 256;
			int e_y = s_y + (s_y_o + image_dimension.height) / 256;
			int e_y_o = (s_y_o + image_dimension.height) % 256;
			count += (e_x - s_x + 1) * (e_y - s_y + 1);
		}
	}
	return count;
}

/****************************************************************************************************
* figure out the number of sequence images will be procues by a given selection and image_dimension
****************************************************************************************************/
void image_glue_sequence_get_number_of_parts(int * parts_x, int * parts_y,
	int zoom, Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y)
{
	double x1_d = lon_to_x(selection.lon1, zoom);
	double x2_d = lon_to_x(selection.lon2, zoom);
	double y1_d = lat_to_y(selection.lat1, zoom);
	double y2_d = lat_to_y(selection.lat2, zoom);
	int x1 = (int) x1_d;
	int x2 = (int) x2_d;
	int y1 = (int) y1_d;
	int y2 = (int) y2_d;
	int x1_o = (x1_d - x1) * 256;
	int x2_o = (x2_d - x2) * 256;
	int y1_o = (y1_d - y1) * 256;
	int y2_o = (y2_d - y2) * 256;
	int width  = (x2-x1+1) * 256 - x1_o - 256 + x2_o;
	int height = (y2-y1+1) * 256 - y1_o - 256 + y2_o;
	*parts_x = 1 + ceil(((double)(width - image_dimension.width)) / (image_dimension.width - intersect_x));
	*parts_y = 1 + ceil(((double)(height - image_dimension.height)) / (image_dimension.height - intersect_y));
}

/****************************************************************************************************
* setup ImageGlue for creating a single image
****************************************************************************************************/
void image_glue_single_setup(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom, double lon1, double lon2, double lat1, double lat2)
{
	ImageJobSingle job;
	double x1_d = lon_to_x(lon1, zoom);
	double x2_d = lon_to_x(lon2, zoom);
	double y1_d = lat_to_y(lat1, zoom);
	double y2_d = lat_to_y(lat2, zoom);
	job.x1 = (int) x1_d;
	job.x2 = (int) x2_d;
	job.y1 = (int) y1_d;
	job.y2 = (int) y2_d;
	job.x1_o = (x1_d - job.x1) * 256;
	job.x2_o = (x2_d - job.x2) * 256;
	job.y1_o = (y1_d - job.y1) * 256;
	job.y2_o = (y2_d - job.y2) * 256;
	job.filename = filename;
	job.cache_dir = cache_dir;
	job.zoom = zoom;
	image_glue -> job_single = job;
}

/****************************************************************************************************
* setup ImageGlue for creating a sequence of images
****************************************************************************************************/
void image_glue_sequence_setup(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom,
                               Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y)
{
	ImageJobSequence job;
	job.filename = filename;
	job.cache_dir = cache_dir;
	job.zoom = zoom;
	job.selection = selection;
	job.image_dimension = image_dimension;
	job.intersect_x = intersect_x;
	job.intersect_y = intersect_y;
	image_glue -> job_sequence = job;
}

/****************************************************************************************************
* after setup, now create the image in another thread
****************************************************************************************************/
void image_glue_single_process(ImageGlue * image_glue)
{
	pthread_t thread;
	int p = pthread_create(&thread, NULL, (void *) image_glue_single_thread_function, image_glue);	
}


/****************************************************************************************************
* after setup, now create the images in another thread
****************************************************************************************************/
void image_glue_sequence_process(ImageGlue * image_glue)
{
	pthread_t thread;
	int p = pthread_create(&thread, NULL, (void *) image_glue_sequence_thread_function, image_glue);
}

/****************************************************************************************************
* this function calls the actual image-generating function, used for threading
****************************************************************************************************/
void image_glue_single_thread_function(ImageGlue * image_glue)
{
	ImageJobSingle job = image_glue -> job_single;
	make_image_1(image_glue, job.filename, job.cache_dir, job.zoom,
			job.x1,   job.x2,   job.y1,   job.y2,
			job.x1_o, job.x2_o, job.y1_o, job.y2_o);
}

/****************************************************************************************************
* this function calls the actual image-generating functions, used for threading
****************************************************************************************************/
void image_glue_sequence_thread_function(ImageGlue * image_glue)
{
	ImageJobSequence job = image_glue -> job_sequence;
	double x1_d = lon_to_x(job.selection.lon1, job.zoom);
	double x2_d = lon_to_x(job.selection.lon2, job.zoom);
	double y1_d = lat_to_y(job.selection.lat1, job.zoom);
	double y2_d = lat_to_y(job.selection.lat2, job.zoom);
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
	int width  = (x2-x1+1) * 256 - x1_o - 256 + x2_o;
	int height = (y2-y1+1) * 256 - y1_o - 256 + y2_o;
	int parts_x = 1 + ceil(((double)(width - job.image_dimension.width)) / (job.image_dimension.width - job.intersect_x));
	int parts_y = 1 + ceil(((double)(height - job.image_dimension.height)) / (job.image_dimension.height - job.intersect_y));
	printf("%d, %d\n", parts_x, parts_y);
	int a,b;
	int step_x = job.image_dimension.width - job.intersect_x;
	int step_y = job.image_dimension.height - job.intersect_y;
	int buffer_len = strlen(job.filename) + 1 + 8 + 1 + 8 + 4 + 1;
	//  ^----------: /some/path/to/prefix   _   x   _   y .png  \0 ... quite pessimitic 8 chars for x and y values
	char buf[buffer_len];
	//TODO: maybe cut the last row/col at selection end
	for (a = 0; a < parts_x; a++){
		int s_x = x1 + (x1_o + a * step_x) / 256;
		int s_x_o = (x1_o + a * step_x) % 256;
		int e_x = s_x + (s_x_o + job.image_dimension.width) / 256;
		int e_x_o = (s_x_o + job.image_dimension.width) % 256;
		//printf("%d,%d,%d,%d\n", s_x, s_x_o, e_x, e_x_o);
		for (b = 0; b < parts_y; b++){
			int s_y = y1 + (y1_o + b * step_y) / 256;
			int s_y_o = (y1_o + b * step_y) % 256;
			int e_y = s_y + (s_y_o + job.image_dimension.height) / 256;
			int e_y_o = (s_y_o + job.image_dimension.height) % 256;
			sprintf(buf, "%s_%d_%d.png", job.filename, a+1, b+1);
			printf("%s\n", buf);
			make_image_1(image_glue, buf, job.cache_dir, job.zoom, s_x, e_x, s_y, e_y, s_x_o, e_x_o, s_y_o, e_y_o);
		}
	}
}


/****************************************************************************************************
* create an image, simple interface
****************************************************************************************************/
int make_image(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom, double lon1, double lon2, double lat1, double lat2) 
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
	//printf("%d %d %d %d\n", x1, x2, y1, y2);dd
	//printf("%d %d %d %d\n", x1_o, x2_o, y1_o, y2_o);
	make_image_1(image_glue, filename, cache_dir, zoom, x1, x2, y1, y2, x1_o, x2_o, y1_o, y2_o);
}

/****************************************************************************************************
* another simple interface for a small commandline-tool for easy handling
***************************************************************************************************/
int make_image_simple(ImageGlue * image_glue, char * filename, int zoom, int x1, int x2, int y1, int y2)
{
	//make_image_1(image_glue, filename, zoom, x1, x2, y1, y2, 0, 0, 256, 256);
}

/****************************************************************************************************
* this is the function that actually does the work. it's interface is quite complex, so that
* some easyier ones are provieded
****************************************************************************************************/
int make_image_1(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom,
			int x1,   int x2,   int y1,   int y2,
			int x1_o, int x2_o, int y1_o, int y2_o){
	char * file_format = "/%d_%d_%d.png";
	char * format = malloc(sizeof(char) * (strlen(cache_dir) + strlen(file_format) + 1));
	strcpy(format, cache_dir);
	strcpy(format + strlen(cache_dir), file_format);
	int blen = strlen(cache_dir) + 30;
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
			g_signal_emit (G_OBJECT(image_glue), image_glue_signals[TILE_COMPLETED], 0, 1);
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
	g_signal_emit (G_OBJECT(image_glue), image_glue_signals[TILE_COMPLETED], 0, 2);
	//printf("%d\n", info_ptr -> valid);

	return EXIT_SUCCESS;
}
