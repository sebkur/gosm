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

#define GOSM_TYPE_COLOR_BOX           (color_box_get_type ())
#define GOSM_COLOR_BOX(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_COLOR_BOX, ColorBox))
#define GOSM_COLOR_BOX_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_COLOR_BOX, ColorBoxClass))
#define GOSM_IS_COLOR_BOX(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_COLOR_BOX))
#define GOSM_IS_COLOR_BOX_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_COLOR_BOX))
#define GOSM_COLOR_BOX_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_COLOR_BOX, ColorBoxClass))

typedef struct _ColorBox        ColorBox;
typedef struct _ColorBoxClass   ColorBoxClass;

struct _ColorBox
{
	GtkHBox parent;

	GtkWidget * entry;
	GtkWidget * button;

	char * last_correct_value;
};

struct _ColorBoxClass
{
	GtkHBoxClass parent_class;

	//void (* function_name) (ColorBox *color_box);
};

GtkWidget * color_box_new();

const char * color_box_get_current_value(ColorBox * color_box);

void color_box_set_current_value(ColorBox * color_box, char * value);
