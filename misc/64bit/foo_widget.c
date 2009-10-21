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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "foo_widget.h"

G_DEFINE_TYPE (FooWidget, foo_widget, GTK_TYPE_VBOX);

GtkWidget * foo_widget_new()
{
	FooWidget * foo_widget = g_object_new(GOSM_TYPE_FOO_WIDGET, NULL);
	return GTK_WIDGET(foo_widget);
}

static void foo_widget_class_init(FooWidgetClass *class)
{
}

static void foo_widget_init(FooWidget *foo_widget)
{
	GtkWidget * button = gtk_button_new_with_label("Foo");
	//gtk_box_pack_start(GTK_BOX(foo_widget), button, TRUE, TRUE, 0);
}
