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

#include <glib.h>
#include <unistd.h>

#define GOSM_TYPE_DISTANCE_TOOL		(distance_tool_get_type ())
#define GOSM_DISTANCE_TOOL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_DISTANCE_TOOL, DistanceTool))
#define GOSM_DISTANCE_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_DISTANCE_TOOL, DistanceToolClass))
#define GOSM_IS_DISTANCE_TOOL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_DISTANCE_TOOL))
#define GOSM_IS_DISTANCE_TOOL_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_DISTANCE_TOOL))
#define GOSM_DISTANCE_TOOL_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_DISTANCE_TOOL, DistanceToolClass))

typedef struct _DistanceTool		DistanceTool;
typedef struct _DistanceToolClass	DistanceToolClass;

struct _DistanceTool
{
	GtkVBox parent;
	GtkWidget * entry_distance;
	GtkWidget * button_remove_last;
	GtkWidget * button_clear;
	GtkWidget * button_bookmark;
};

struct _DistanceToolClass
{
	GtkVBoxClass parent_class;

};

GtkWidget * distance_tool_new();
