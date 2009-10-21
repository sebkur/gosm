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

#ifndef _FOO_WIDGET_H_
#define _FOO_WIDGET_H_

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_FOO_WIDGET           (foo_widget_get_type ())
#define GOSM_FOO_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_FOO_WIDGET, FooWidget))
#define GOSM_FOO_WIDGET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_FOO_WIDGET, FooWidgetClass))
#define GOSM_IS_FOO_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_FOO_WIDGET))
#define GOSM_IS_FOO_WIDGET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_FOO_WIDGET))
#define GOSM_FOO_WIDGET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_FOO_WIDGET, FooWidgetClass))

typedef struct _FooWidget        FooWidget;
typedef struct _FooWidgetClass   FooWidgetClass;

struct _FooWidget
{
	GtkVBox parent;
};

struct _FooWidgetClass
{
	GtkVBoxClass parent_class;
};

GtkWidget * foo_widget_new();

#endif /* _FOO_WIDGET_H_ */
