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

#include "color_button.h"

/****************************************************************************************************
* this is a simple button that displays a colour
****************************************************************************************************/
G_DEFINE_TYPE (ColorButton, color_button, GTK_TYPE_DRAWING_AREA);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint color_button_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, color_button_signals[SIGNAL_NAME_n], 0);

static gboolean expose_cb (GtkWidget * area, GdkEventExpose *event);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * color_button_new()
{
	ColorButton * color_button = g_object_new(GOSM_TYPE_COLOR_BUTTON, NULL);
	color_button -> r = 0.0; 
	color_button -> g = 0.0; 
	color_button -> b = 1.0; 
	color_button -> a = 0.8; 
	g_signal_connect (color_button, "expose-event", G_CALLBACK (expose_cb), NULL);
	gtk_widget_set_events(GTK_WIDGET(color_button), gtk_widget_get_events(GTK_WIDGET(color_button)) | GDK_BUTTON_PRESS_MASK);
	return GTK_WIDGET(color_button);
}

static void color_button_class_init(ColorButtonClass *class)
{
        /*color_button_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColorButtonClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void color_button_init(ColorButton *color_button)
{
}

/****************************************************************************************************
* set the colour the button shall show
****************************************************************************************************/
void color_button_set_color(ColorButton * color_button, double r, double g, double b, double a)
{
	color_button -> r = r;
	color_button -> g = g;
	color_button -> b = b;
	color_button -> a = a;
	gtk_widget_queue_draw(GTK_WIDGET(color_button));
}

/****************************************************************************************************
* paint the widget
* derived from gtk_color_sel.c
****************************************************************************************************/
static gboolean expose_cb (GtkWidget * area, GdkEventExpose *event)
{
	ColorButton * cb = GOSM_COLOR_BUTTON(area);
	gint x, y, wid, heig, goff;
	cairo_t *cr;
	goff = 0;
	int CHECK_SIZE = 5;
	cr = gdk_cairo_create (area->window);
	wid = area->allocation.width;
	heig = area->allocation.height;

	cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
	cairo_rectangle (cr, 0, 0, wid, heig);
	cairo_fill (cr);

	cairo_set_source_rgb (cr, 0.75, 0.75, 0.75);
	for (x = goff & -CHECK_SIZE; x < goff + wid; x += CHECK_SIZE)
		for (y = 0; y < heig; y += CHECK_SIZE)
			if ((x / CHECK_SIZE + y / CHECK_SIZE) % 2 == 0)
				cairo_rectangle (cr, x - goff, y, CHECK_SIZE, CHECK_SIZE);
	cairo_fill (cr);

	cairo_set_source_rgba (cr, cb -> r, cb -> g, cb -> b, cb -> a);
	cairo_rectangle (cr, 0, 0, wid, heig);
	cairo_fill (cr);
	cairo_destroy (cr);
}
