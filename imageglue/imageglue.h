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

#include <png.h>

/*
int make_image(char * filename, char * cache_dir, int zoom, double lon1, double lon2, double lat1, double lat2);

int make_image_simple(char * filename, int zoom, int x1, int x2, int y1, int y2);

int make_image_1(char * filename, char * cache_dir, int zoom,
			int x1,   int x2,   int y1,   int y2,
			int x1_o, int x2_o, int y1_o, int y2_o);*/

/****************************************************************************************************
* see implementation for explanation
****************************************************************************************************/
int create_read_structs(FILE * fp, png_structp * png_ptr, png_infop * info_ptr);

/****************************************************************************************************
* see implementation for explanation
****************************************************************************************************/
int create_write_structs(FILE * fp, png_structp * png_ptr, png_infop * info_ptr, png_colorp * palette, int width, int height);
