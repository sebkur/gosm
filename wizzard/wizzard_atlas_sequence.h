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

#ifndef _WIZZARD_ATLAS_SEQUENCE_H_
#define _WIZZARD_ATLAS_SEQUENCE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "wizzard_atlas_sequence_window.h"
#include "../imageglue/image_glue.h"
#include "../map_types.h"
#include "../atlas/atlas.h"

#define GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE           (wizzard_atlas_sequence_get_type ())
#define GOSM_WIZZARD_ATLAS_SEQUENCE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE, WizzardAtlasSequence))
#define GOSM_WIZZARD_ATLAS_SEQUENCE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_WIZZARD_ATLAS_SEQUENCE, WizzardAtlasSequenceClass))
#define GOSM_IS_WIZZARD_ATLAS_SEQUENCE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE))
#define GOSM_IS_WIZZARD_ATLAS_SEQUENCE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE))
#define GOSM_WIZZARD_ATLAS_SEQUENCE_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE, WizzardAtlasSequenceClass))

typedef struct _WizzardAtlasSequence        WizzardAtlasSequence;
typedef struct _WizzardAtlasSequenceClass   WizzardAtlasSequenceClass;

struct _WizzardAtlasSequence
{
	GObject parent;

	int total;
	int ready;

	char * cache_dir;
	int zoom;
	Selection selection;
	ImageDimension image_dimension;
	int intersect_x;
	int intersect_y;

	WizzardAtlasSequenceWindow * wasw;
};

struct _WizzardAtlasSequenceClass
{
	GObjectClass parent_class;

	//void (* function_name) (WizzardAtlasSequence *wizzard_atlas_sequence);
};

WizzardAtlasSequence * wizzard_atlas_sequence_new(char * cache_dir, int zoom, Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y);

void wizzard_atlas_sequence_show(WizzardAtlasSequence * was, GtkWindow * parent);

#endif
