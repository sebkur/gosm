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

#ifndef _ATLAS_TOOL_H_
#define _ATLAS_TOOL_H_

#define GOSM_TYPE_ATLAS_TOOL		(atlas_tool_get_type ())
#define GOSM_ATLAS_TOOL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_ATLAS_TOOL, AtlasTool))
#define GOSM_ATLAS_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_ATLAS_TOOL, AtlasToolClass))
#define GOSM_IS_ATLAS_TOOL(obj)	(G_TYPE_CHECK_INSTANCE ((obj), GOSM_TYPE_ATLAS_TOOL))
#define GOSM_IS_ATLAS_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_ATLAS_TOOL))
#define GOSM_ATLAS_TOOL_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_ATLAS_TOOL, AtlasToolClass))

typedef struct _AtlasTool		AtlasTool;
typedef struct _AtlasToolClass		AtlasToolClass;

struct _AtlasTool
{
	GtkVBox parent;
	/* public things? */
	int slice_x;
	int slice_y;
	int slice_intersect_x;
	int slice_intersect_y;
	int visible;
	int slice_zoom;

	GtkWidget * entry_slice_x;
	GtkWidget * entry_slice_y;
	GtkWidget * entry_slice_intersect_x;
	GtkWidget * entry_slice_intersect_y;
	GtkWidget * check_show;
	GtkWidget * combo;
	GtkWidget * button_template;
	GtkWidget * button_action;
	GtkWidget * button_export;
};

struct _AtlasToolClass
{
	GtkVBoxClass parent_class;

	void (* values_changed) (AtlasTool *atlas_tool);
};

AtlasTool * atlas_tool_new();

void atlas_tool_set_dimensions(AtlasTool * atlas_tool, int slice_x, int slice_y, int slice_intersect_x, int slice_intersect_y);

void atlas_tool_set_values(AtlasTool * atlas_tool, int slice_x, int slice_y, int slice_intersect_x, int slice_intersect_y, int zoom, gboolean visible);

#endif
