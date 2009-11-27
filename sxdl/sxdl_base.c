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

#include "sxdl_base.h"

G_DEFINE_TYPE (SxdlBase, sxdl_base, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_base_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_base_signals[SIGNAL_NAME_n], 0);

SxdlBase * sxdl_base_new()
{
	SxdlBase * sxdl_base = g_object_new(GOSM_TYPE_SXDL_BASE, NULL);
	return sxdl_base;
}

static void sxdl_base_class_init(SxdlBaseClass *class)
{
        /*sxdl_base_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlBaseClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_base_init(SxdlBase *sxdl_base)
{
}

void sxdl_base_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, Clip * clip)
{
	SxdlBaseClass * class = GOSM_SXDL_BASE_GET_CLASS(sxdl_base);
	if (class -> render) class -> render(sxdl_base, widget, x, y, width_proposed, height_proposed,
		used_width, used_height, clip);
}

void sxdl_base_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height)
{
	SxdlBaseClass * class = GOSM_SXDL_BASE_GET_CLASS(sxdl_base);
	if (class -> get_size) class -> get_size(sxdl_base, widget, width_proposed, height_proposed, used_width, used_height);
}
