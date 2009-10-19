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

#define GOSM_TYPE_COLOR_BUTTON           (color_button_get_type ())
#define GOSM_COLOR_BUTTON(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_COLOR_BUTTON, ColorButton))
#define GOSM_COLOR_BUTTON_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_COLOR_BUTTON, ColorButtonClass))
#define GOSM_IS_COLOR_BUTTON(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_COLOR_BUTTON))
#define GOSM_IS_COLOR_BUTTON_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_COLOR_BUTTON))
#define GOSM_COLOR_BUTTON_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_COLOR_BUTTON, ColorButtonClass))

typedef struct _ColorButton        ColorButton;
typedef struct _ColorButtonClass   ColorButtonClass;

struct _ColorButton
{
	GtkDrawingArea parent;
	
	double r;
	double g;
	double b;
	double a;
};

struct _ColorButtonClass
{
	GtkDrawingAreaClass parent_class;

	//void (* function_name) (ColorButton *color_button);
};

GtkWidget * color_button_new();

void color_button_set_color(ColorButton * color_button, double r, double g, double b, double a);
