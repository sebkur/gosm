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

#define GOSM_TYPE_COLOR_BUTTON_AUTO           (color_button_auto_get_type ())
#define GOSM_COLOR_BUTTON_AUTO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_COLOR_BUTTON_AUTO, ColorButtonAuto))
#define GOSM_COLOR_BUTTON_AUTO_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_COLOR_BUTTON_AUTO, ColorButtonAutoClass))
#define GOSM_IS_COLOR_BUTTON_AUTO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_COLOR_BUTTON_AUTO))
#define GOSM_IS_COLOR_BUTTON_AUTO_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_COLOR_BUTTON_AUTO))
#define GOSM_COLOR_BUTTON_AUTO_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_COLOR_BUTTON_AUTO, ColorButtonAutoClass))

typedef struct _ColorButtonAuto        ColorButtonAuto;
typedef struct _ColorButtonAutoClass   ColorButtonAutoClass;

struct _ColorButtonAuto
{
	GtkHBox parent;

	GtkWidget * button;
};

struct _ColorButtonAutoClass
{
	GtkHBoxClass parent_class;

	//void (* function_name) (ColorButtonAuto *color_button_auto);
};

GtkWidget * color_button_auto_new();

void color_button_auto_get_current_value(ColorButtonAuto * color_button_auto, double *r, double *g, double *b, double *a);

void color_button_auto_set_current_value(ColorButtonAuto * color_button_auto, double r, double g, double b, double a);
