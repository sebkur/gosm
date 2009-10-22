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

#ifndef _WIZZARD_ATLAS_PDF_H_
#define _WIZZARD_ATLAS_PDF_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "wizzard_atlas_pdf_window.h"
#include "../imageglue/image_glue.h"
#include "../map_types.h"
#include "../atlas/atlas.h"

#define GOSM_TYPE_WIZZARD_ATLAS_PDF           (wizzard_atlas_pdf_get_type ())
#define GOSM_WIZZARD_ATLAS_PDF(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF, WizzardAtlasPdf))
#define GOSM_WIZZARD_ATLAS_PDF_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_WIZZARD_ATLAS_PDF, WizzardAtlasPdfClass))
#define GOSM_IS_WIZZARD_ATLAS_PDF(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF))
#define GOSM_IS_WIZZARD_ATLAS_PDF_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF))
#define GOSM_WIZZARD_ATLAS_PDF_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF, WizzardAtlasPdfClass))

typedef struct _WizzardAtlasPdf        WizzardAtlasPdf;
typedef struct _WizzardAtlasPdfClass   WizzardAtlasPdfClass;

struct _WizzardAtlasPdf
{
	GObject parent;

	int total_tiles;
	int ready_tiles;

	int total_parts;
	int ready_parts;

	int parts_x;
	int parts_y;

	char * cache_dir;
	char * filename_prefix;
	int zoom;
	Selection selection;
	PageInformation page_info;
	ImageDimension image_dimension;
	int intersect_x;
	int intersect_y;

	WizzardAtlasPdfWindow * wapw;
};

struct _WizzardAtlasPdfClass
{
	GObjectClass parent_class;

	//void (* function_name) (WizzardAtlasPdf *wizzard_atlas_pdf);
};

WizzardAtlasPdf * wizzard_atlas_pdf_new(char * cache_dir, int zoom, Selection selection, 
	PageInformation page_info, ImageDimension image_dimension, int intersect_x, int intersect_y);

void wizzard_atlas_pdf_show(WizzardAtlasPdf * wap, GtkWindow * parent);

#endif
