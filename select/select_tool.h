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

#ifndef _SELECT_TOOL_H_
#define _SELECT_TOOL_H_

#define GOSM_TYPE_SELECT_TOOL		(select_tool_get_type ())
#define GOSM_SELECT_TOOL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SELECT_TOOL, SelectTool))
#define GOSM_SELECT_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_SELECT_TOOL, SelectToolClass))
#define GOSM_IS_SELECT_TOOL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SELECT_TOOL))
#define GOSM_IS_SELECT_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SELECT_TOOL))
#define GOSM_SELECT_TOOL_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SELECT_TOOL, SelectToolClass))

typedef struct _SelectTool		SelectTool;
typedef struct _SelectToolClass		SelectToolClass;

struct _SelectTool
{
	GtkVBox parent;
	/* public things? */
	GtkWidget * lon1;
	GtkWidget * lon2;
	GtkWidget * lat1;
	GtkWidget * lat2;
	GtkWidget * check_show;
	GtkWidget * check_snap;
	GtkWidget * button_action;
	GtkWidget * button_trash;
	GtkWidget * button_trash_adv;
	GtkWidget * button_export;
	GtkWidget * button_data;
	GtkWidget * button_clipboard;
	GtkWidget * button_bookmark;
};

struct _SelectToolClass
{
	GtkVBoxClass parent_class;
};

SelectTool * select_tool_new();

void select_tool_set_lons_n_lats(SelectTool * select_tool, double lon1, double lon2, double lat1, double lat2);

#endif
