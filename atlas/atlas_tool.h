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

#include <gtk/gtk.h>
#include <glib.h>

#include "atlas.h"

#ifndef _ATLAS_TOOL_H_
#define _ATLAS_TOOL_H_

#define GOSM_TYPE_ATLAS_TOOL		(atlas_tool_get_type ())
#define GOSM_ATLAS_TOOL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_ATLAS_TOOL, AtlasTool))
#define GOSM_ATLAS_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_ATLAS_TOOL, AtlasToolClass))
#define GOSM_IS_ATLAS_TOOL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_ATLAS_TOOL))
#define GOSM_IS_ATLAS_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_ATLAS_TOOL))
#define GOSM_ATLAS_TOOL_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_ATLAS_TOOL, AtlasToolClass))

typedef struct _AtlasTool		AtlasTool;
typedef struct _AtlasToolClass		AtlasToolClass;

typedef enum {
	MODE_BY_PAGESIZE,
	MODE_BY_IMAGESIZE
} SliceMode;

struct _AtlasTool
{
	GtkVBox parent;
	/* public things? */
	int mode;
	PageInformation page_info;
	PageInformation page_info_stored;
	gboolean border_alternating_horizontally;
	gboolean border_alternating_vertically;
	ImageDimension image_dimension;
	int intersect_x;
	int intersect_y;
	int visible;
	int slice_zoom;

	GtkWidget * radio_conf_page;
	GtkWidget * radio_conf_pixel;
	GtkWidget * entry_slice_x;
	GtkWidget * entry_slice_y;
	GtkWidget * entry_slice_intersect_x;
	GtkWidget * entry_slice_intersect_y;
	GtkWidget * check_show;
	GtkWidget * combo;
	GtkWidget * button_conf_page;
	GtkWidget * button_action;
	GtkWidget * button_export;
	GtkWidget * button_export_pdf;
};

struct _AtlasToolClass
{
	GtkVBoxClass parent_class;

	void (* values_changed) (AtlasTool *atlas_tool);
};

AtlasTool * atlas_tool_new();

void atlas_tool_set_page_info(AtlasTool * atlas_tool, PageInformation page_info);

void atlas_tool_set_image_dimension(AtlasTool * atlas_tool, ImageDimension image_dimension);

void atlas_tool_set_intersection(AtlasTool * atlas_tool, int intersect_x, int intersect_y);

void atlas_tool_set_values(AtlasTool * atlas_tool, int zoom, gboolean visible);

#endif
