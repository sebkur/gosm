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

#ifndef _IMAGE_GLUE_H_
#define _IMAGE_GLUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

#include "../map_types.h"
#include "../atlas/atlas.h"

#define GOSM_TYPE_IMAGE_GLUE           (image_glue_get_type ())
#define GOSM_IMAGE_GLUE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_IMAGE_GLUE, ImageGlue))
#define GOSM_IMAGE_GLUE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_IMAGE_GLUE, ImageGlueClass))
#define GOSM_IS_IMAGE_GLUE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_IMAGE_GLUE))
#define GOSM_IS_IMAGE_GLUE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_IMAGE_GLUE))
#define GOSM_IMAGE_GLUE_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_IMAGE_GLUE, ImageGlueClass))

typedef struct _ImageGlue        ImageGlue;
typedef struct _ImageGlueClass   ImageGlueClass;

typedef struct  {
	int x1;
	int x2;
	int y1;
	int y2;
	int x1_o;
	int x2_o;
	int y1_o;
	int y2_o;
	char * filename;
	char * cache_dir;
	int zoom;
} ImageJobSingle;

typedef struct  {
	Selection selection;
	ImageDimension image_dimension;
	int intersect_x;
	int intersect_y;
	char * filename;
	char * cache_dir;
	int zoom;
} ImageJobSequence;

struct _ImageGlue
{
	GObject parent;
	
	ImageJobSingle job_single;
	ImageJobSequence job_sequence;
};

struct _ImageGlueClass
{
	GObjectClass parent_class;

	void (* tile_completed) (ImageGlue *image_glue,
				 gint number);
};

ImageGlue * image_glue_new();

// functions for single image production
int image_glue_single_get_number_of_tiles(int zoom, double lon1, double lon2, double lat1, double lat2);

void image_glue_single_setup(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom, double lon1, double lon2, double lat1, double lat2);

void image_glue_single_process(ImageGlue * image_glue);

// function for sequences of images
int image_glue_sequence_get_number_of_tiles(int zoom, Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y);

void image_glue_sequence_get_number_of_parts(int * parts_x, int * parts_y,
	int zoom, Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y);

void image_glue_sequence_setup(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom,
			       Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y);

void image_glue_sequence_process(ImageGlue * image_glue);

// generic image - producing functions
int make_image(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom, double lon1, double lon2, double lat1, double lat2);

int make_image_simple(ImageGlue * image_glue, char * filename, int zoom, int x1, int x2, int y1, int y2);

int make_image_1(ImageGlue * image_glue, char * filename, char * cache_dir, int zoom,
			int x1,   int x2,   int y1,   int y2,
			int x1_o, int x2_o, int y1_o, int y2_o);

#endif
