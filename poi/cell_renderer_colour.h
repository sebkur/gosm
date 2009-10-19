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

#ifndef _CELL_RENDERER_COLOUR_H_
#define _CELL_RENDERER_COLOUR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_CELL_RENDERER_COLOUR           (cell_renderer_colour_get_type ())
#define GOSM_CELL_RENDERER_COLOUR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_CELL_RENDERER_COLOUR, CellRendererColour))
#define GOSM_CELL_RENDERER_COLOUR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_CELL_RENDERER_COLOUR, CellRendererColourClass))
#define GOSM_IS_CELL_RENDERER_COLOUR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_CELL_RENDERER_COLOUR))
#define GOSM_IS_CELL_RENDERER_COLOUR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_CELL_RENDERER_COLOUR))
#define GOSM_CELL_RENDERER_COLOUR_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_CELL_RENDERER_COLOUR, CellRendererColourClass))

typedef struct _CellRendererColour        CellRendererColour;
typedef struct _CellRendererColourClass   CellRendererColourClass;

struct _CellRendererColour
{
	GtkCellRenderer parent;

	double r;
	double g;
	double b;
	double a;
	gboolean activatable;
};

struct _CellRendererColourClass
{
	GtkCellRendererClass parent_class;

	void (* edit_colour) (GtkCellRendererToggle *cell_renderer_toggle, const gchar *path);

};

GtkCellRenderer * cell_renderer_colour_new();

#endif /* _CELL_RENDERER_COLOUR_H_ */
