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

#include "gtk_rollup_button.h"

#define BUTTONSIZE 10

G_DEFINE_TYPE (GtkRollupButton, gtk_rollup_button, GTK_TYPE_DRAWING_AREA);

enum
{
        TOGGLED,
        LAST_SIGNAL
};

static guint gtk_rollup_button_signals[LAST_SIGNAL] = { 0 };

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event);
static gboolean mouse_button_cb(GtkWidget *widget, GdkEventButton *event);

GtkWidget * gtk_rollup_button_new()
{
	GtkRollupButton * gtk_rollup_button = g_object_new(GOSM_TYPE_GTK_ROLLUP_BUTTON, NULL);
	gtk_widget_set_size_request(GTK_WIDGET(gtk_rollup_button), BUTTONSIZE, BUTTONSIZE);
	g_signal_connect(
		G_OBJECT(gtk_rollup_button), "button_release_event",
		G_CALLBACK(mouse_button_cb), NULL);
	gtk_widget_set_events(
		GTK_WIDGET(gtk_rollup_button),
		gtk_widget_get_events(GTK_WIDGET(gtk_rollup_button))
		| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_EXPOSURE_MASK );
	return GTK_WIDGET(gtk_rollup_button);
}

static void gtk_rollup_button_class_init(GtkRollupButtonClass *class)
{
	GtkWidgetClass *widget_class;
	widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> expose_event = expose_cb;

        gtk_rollup_button_signals[TOGGLED] = g_signal_new(
                "toggled",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (GtkRollupButtonClass, toggled),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void gtk_rollup_button_init(GtkRollupButton *gtk_rollup_button)
{
}

gboolean gtk_rollup_button_get_active(GtkRollupButton * button)
{
	return button -> active;
}

void gtk_rollup_button_set_active(GtkRollupButton * button, gboolean active)
{
	button -> active = active;
}

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event)
{
	int width  = widget -> allocation.width;
	int height = widget -> allocation.height;
	int x = (width - BUTTONSIZE)/2;
	int y = (height - BUTTONSIZE)/2;
	gdk_draw_rectangle(widget -> window,
				widget->style->white_gc,
				TRUE, x, y, BUTTONSIZE, BUTTONSIZE);
	return FALSE;
}

static gboolean mouse_button_cb(GtkWidget *widget, GdkEventButton *event)
{
	GtkRollupButton * button = GOSM_GTK_ROLLUP_BUTTON(widget);
	if (event -> type == GDK_BUTTON_RELEASE){
		button -> active = !button -> active;
		g_signal_emit (widget, gtk_rollup_button_signals[TOGGLED], 0);
	}
	return FALSE;
}
