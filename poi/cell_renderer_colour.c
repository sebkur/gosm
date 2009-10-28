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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>

#include "cell_renderer_colour.h"

#define FIXED_WIDTH 10
#define FIXED_HEIGHT 10

/****************************************************************************************************
* this class is a renderer that can be used in treeviews
* it shows a coloured square in its cell
****************************************************************************************************/
G_DEFINE_TYPE (CellRendererColour, cell_renderer_colour, GTK_TYPE_CELL_RENDERER);

/****************************************************************************************************
* the properties of the cell: red, gree, blue, alpha, and whether this can be edited
****************************************************************************************************/
enum
{
	PROP_R = 1,
	PROP_G,
	PROP_B,
	PROP_A,
	PROP_ACTIVATABLE
};

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
        EDIT_COLOUR,
        LAST_SIGNAL
};

static guint cell_renderer_colour_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* method declarations
****************************************************************************************************/
static void cell_renderer_colour_get_property (GObject *object,
					guint param_id,
					GValue *value,
					GParamSpec *pspec);

static void cell_renderer_colour_set_property (GObject *object,
					guint param_id,
					const GValue *value,
					GParamSpec *pspec);

static void cell_renderer_colour_get_size (GtkCellRenderer *cell,
					GtkWidget *widget,
					GdkRectangle *cell_area,
					gint *x_offset,
					gint *y_offset,
					gint *width,
					gint *height);

static void cell_renderer_colour_render (GtkCellRenderer *cell,
					GdkWindow *window,
					GtkWidget *widget,
					GdkRectangle *background_area,
					GdkRectangle *cell_area,
					GdkRectangle *expose_area,
					guint flags);

static gint cell_renderer_colour_activate (GtkCellRenderer *cell,
					GdkEvent *event,
					GtkWidget *widget,
					const gchar *path,
					GdkRectangle *background_area,
					GdkRectangle *cell_area,
					GtkCellRendererState flags);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkCellRenderer * cell_renderer_colour_new()
{
	CellRendererColour * cell_renderer_colour = g_object_new(GOSM_TYPE_CELL_RENDERER_COLOUR, NULL);
	return GTK_CELL_RENDERER(cell_renderer_colour);
}

/****************************************************************************************************
* class init
****************************************************************************************************/
static void cell_renderer_colour_class_init(CellRendererColourClass *class)
{
	GtkCellRendererClass	*cell_class	= GTK_CELL_RENDERER_CLASS(class);
	GObjectClass		*object_class	= G_OBJECT_CLASS(class);

	object_class -> get_property = cell_renderer_colour_get_property;
	object_class -> set_property = cell_renderer_colour_set_property;
	cell_class -> get_size = cell_renderer_colour_get_size;
	cell_class -> render = cell_renderer_colour_render;
	cell_class -> activate = cell_renderer_colour_activate;

	g_object_class_install_property (object_class,
		PROP_R,
		g_param_spec_double ("r",
		"r",
		"the RED part of the colour to display",
		0.0, 1.0, 0.0,
		G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
		PROP_G,
		g_param_spec_double ("g",
		"g",
		"the GREEN part of the colour to display",
		0.0, 1.0, 0.0,
		G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
		PROP_B,
		g_param_spec_double ("b",
		"b",
		"the BLUE part of the colour to display",
		0.0, 1.0, 0.0,
		G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
		PROP_A,
		g_param_spec_double ("a",
		"a",
		"the ALPHA part of the colour to display",
		0.0, 1.0, 1.0,
		G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
		PROP_ACTIVATABLE,
		g_param_spec_boolean ("activatable",
		"Activatable",
		"The colour can be changed",
		GTK_CELL_RENDERER_MODE_ACTIVATABLE,
		G_PARAM_READWRITE));
	cell_renderer_colour_signals[EDIT_COLOUR] =
		g_signal_new ("edit-colour",
		G_OBJECT_CLASS_TYPE (object_class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (CellRendererColourClass, edit_colour),
		NULL, NULL,
		g_cclosure_marshal_VOID__STRING,
		G_TYPE_NONE, 1,
		G_TYPE_STRING);

}

/****************************************************************************************************
* object init
****************************************************************************************************/
static void cell_renderer_colour_init(CellRendererColour *cell_renderer_colour)
{
	GTK_CELL_RENDERER (cell_renderer_colour) -> mode = GTK_CELL_RENDERER_MODE_ACTIVATABLE;
}

/****************************************************************************************************
* get a property
****************************************************************************************************/
static void cell_renderer_colour_get_property (GObject *object,
		guint param_id,
		GValue *value,
		GParamSpec *psec)
{
	CellRendererColour *cell_renderer_colour = GOSM_CELL_RENDERER_COLOUR(object);
	switch (param_id)
	{
	case PROP_R:{
		g_value_set_double(value, cell_renderer_colour -> r);
		break;
	}
	case PROP_G:{
		g_value_set_double(value, cell_renderer_colour -> g);
		break;
	}
	case PROP_B:{
		g_value_set_double(value, cell_renderer_colour -> b);
		break;
	}
	case PROP_A:{
		g_value_set_double(value, cell_renderer_colour -> a);
		break;
	}
	case PROP_ACTIVATABLE:{
		g_value_set_boolean(value, cell_renderer_colour -> activatable);
	}
	default:{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, psec);
		break;
	}
	}
}

/****************************************************************************************************
* set a property
****************************************************************************************************/
static void cell_renderer_colour_set_property (GObject *object,
		guint param_id,
		const GValue *value,
		GParamSpec *pspec)
{
	CellRendererColour *cell_renderer_colour = GOSM_CELL_RENDERER_COLOUR (object);
	switch (param_id)
	{
	case PROP_R:{
		cell_renderer_colour -> r = g_value_get_double(value);
		break;
	}
	case PROP_G:{
		cell_renderer_colour -> g = g_value_get_double(value);
		break;
	}
	case PROP_B:{
		cell_renderer_colour -> b = g_value_get_double(value);
		break;
	}
	case PROP_A:{
		cell_renderer_colour -> a = g_value_get_double(value);
		break;
	}
	case PROP_ACTIVATABLE:{
		cell_renderer_colour -> activatable = g_value_get_boolean(value);
	}
	default:{
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
		break;
	}
	}
}

/****************************************************************************************************
* get size of the cell
****************************************************************************************************/
static void cell_renderer_colour_get_size (GtkCellRenderer *cell,
					GtkWidget *widget,
					GdkRectangle *cell_area,
					gint *x_offset,
					gint *y_offset,
					gint *width,
					gint *height)
{
	gint calc_width = (gint) cell -> xpad * 2 + FIXED_WIDTH;
	gint calc_height = (gint) cell -> ypad * 2 + FIXED_HEIGHT;
	if (width)
		*width = calc_width;
	if (height)
		*height = calc_height;
	if (cell_area){
		if (x_offset){
			*x_offset = cell -> xalign * (cell_area -> width - calc_width);
			*x_offset = MAX (*x_offset, 0);
		}
		if (y_offset){
			*y_offset = cell -> yalign * (cell_area -> height - calc_height);
			*y_offset = MAX (*y_offset, 0);
		}
	}
}

/****************************************************************************************************
* draw the cell
* derived from gtk_color_sel.c (config/color_button.c)
****************************************************************************************************/
static void cell_renderer_colour_render (GtkCellRenderer *cell,
					GdkWindow *window,
					GtkWidget *widget,
					GdkRectangle *background_area,
					GdkRectangle *cell_area,
					GdkRectangle *expose_area,
					guint flags)
{
	CellRendererColour * crc = GOSM_CELL_RENDERER_COLOUR(cell);
	gint width, height, x_offset, y_offset;
	cell_renderer_colour_get_size(cell, widget, cell_area, &x_offset, &y_offset, &width, &height);
	gdk_draw_rectangle(window, widget->style->black_gc,
			FALSE,
			cell_area->x + x_offset + cell->xpad,
			cell_area->y + y_offset + cell->ypad,
			width - 1, height - 1);
	gint x, y, wid, heig, goff;
	wid = width;
	heig = height;
	cairo_t *cr;
	goff = 0;
	int CHECK_SIZE = 5;
	cr = gdk_cairo_create (window);

	cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
	cairo_rectangle (cr,
			cell_area->x + x_offset + cell->xpad,
			cell_area->y + y_offset + cell->ypad,
			width, height);
	cairo_fill (cr);

	cairo_set_source_rgb (cr, 0.75, 0.75, 0.75);
	for (x = goff & -CHECK_SIZE; x < goff + wid; x += CHECK_SIZE)
		for (y = 0; y < heig; y += CHECK_SIZE)
			if ((x / CHECK_SIZE + y / CHECK_SIZE) % 2 == 0)
				cairo_rectangle (cr,
					cell_area -> x + x_offset + x - goff,
					cell_area -> y + y_offset + y,
					CHECK_SIZE, CHECK_SIZE);
	cairo_fill (cr);

	cairo_set_source_rgba (cr, crc -> r, crc -> g, crc -> b, crc -> a);
	cairo_rectangle (cr,
			cell_area->x + x_offset + cell->xpad,
			cell_area->y + y_offset + cell->ypad,
			width, height);
	cairo_fill (cr);
	cairo_destroy (cr);
}

/****************************************************************************************************
* when the cell is clicked
****************************************************************************************************/
static gint cell_renderer_colour_activate (GtkCellRenderer *cell,
				 GdkEvent *event,
				 GtkWidget *widget,
				 const gchar *path,
				 GdkRectangle *background_area,
				 GdkRectangle *cell_area,
				 GtkCellRendererState flags)
{
	CellRendererColour *cellcolour = GOSM_CELL_RENDERER_COLOUR (cell);
	g_signal_emit (cell, cell_renderer_colour_signals[EDIT_COLOUR], 0, path);
	/*if (cellcolour -> activatable){
		g_signal_emit (cell, cell_renderer_colour_signals[EDIT_COLOUR], 0, path);
		return TRUE;
	}*/
	return FALSE;
}

