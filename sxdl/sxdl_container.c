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

#include "sxdl_container.h"
#include "sxdl_break.h"

G_DEFINE_TYPE (SxdlContainer, sxdl_container, GOSM_TYPE_SXDL_BASE);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_container_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_container_signals[SIGNAL_NAME_n], 0);

void sxdl_container_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, Clip * clip);
void sxdl_container_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height);
void sxdl_container_get_line_size(SxdlContainer * container, int index, GtkWidget * widget,
	int width_proposed, int height_proposed, int * used_width, int * used_height);

SxdlContainer * sxdl_container_new()
{
	SxdlContainer * sxdl_container = g_object_new(GOSM_TYPE_SXDL_CONTAINER, NULL);
	sxdl_container -> lines = g_array_new(FALSE, FALSE, sizeof(GArray*));
	sxdl_container -> current_line = 0;
	return sxdl_container;
}

void sxdl_container_finalize(SxdlContainer * container)
{
	printf("finalize\n");
	int i, e;
	for (i = 0; i < container -> lines -> len; i++){
		GArray * array = g_array_index(container -> lines, GArray*, i);
		for (e = 0; e < array -> len; e++){
			g_object_unref(g_array_index(array, GObject*, e));
		}
		g_array_free(array, TRUE);
	}
	g_array_free(container -> lines, TRUE);
}

static void sxdl_container_class_init(SxdlContainerClass *class)
{
	GObjectClass * object_class = G_OBJECT_CLASS(class);
	SxdlBaseClass * sxdl_base_class = GOSM_SXDL_BASE_CLASS(class);
	object_class -> finalize = sxdl_container_finalize;
	sxdl_base_class -> render = sxdl_container_render;
	sxdl_base_class -> get_size = sxdl_container_get_size;
        /*sxdl_container_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlContainerClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_container_init(SxdlContainer *sxdl_container)
{
}

void sxdl_container_render_line(SxdlContainer * container, int index, GtkWidget * widget, int x, int y, 
	int width_proposed, int height_proposed, int * used_width, int * used_height, Clip * clip)
{
	int min_w, min_w_h, max_w, max_w_h;
	sxdl_container_get_line_size(container, index, widget, 0, 0, &min_w, &min_w_h);
	sxdl_container_get_line_size(container, index, widget, -1, -1, &max_w, &max_w_h);
	int too_much_w = max_w - width_proposed;
	too_much_w = too_much_w > 0 ? too_much_w : 0;
	int lw = 0, lh = 0;
	int px = x;
	GArray * a = g_array_index(container -> lines, GArray*, index);
	int e;
	for (e = 0; e < a -> len; e++){
		SxdlBase * base = g_array_index(a, SxdlBase*, e);
		int min_ew, min_ew_eh, max_ew, max_ew_eh, use_ew, use_ew_eh;
		sxdl_base_get_size(base, widget, 0, 0, &min_ew, &min_ew_eh);
		sxdl_base_get_size(base, widget, -1, -1, &max_ew, &max_ew_eh);
		int layout_w = max_ew;
		if (too_much_w > 0){
			int play = max_w - min_w;
			layout_w = (int)(max_ew - (max_ew - min_ew) * ((double)too_much_w / (double)play));
		}
		sxdl_base_get_size(base, widget, layout_w, -1, &use_ew, &use_ew_eh);
		int w, h;
		sxdl_base_render(base, widget, px, y, layout_w, -1, &w, &h, clip);
		px += layout_w;
		lw += layout_w;
		lh = use_ew_eh > lh ? use_ew_eh : lh;
	}
	*used_width = lw;
	*used_height = lh;
}

void sxdl_container_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, Clip * clip)
{
	SxdlContainer * container = (SxdlContainer*)sxdl_base;
	int py = y;
	int cw = 0, ch = 0;
	int i;
	for (i = 0; i < container -> lines -> len; i++){
		int w, h;
		sxdl_container_render_line(container, i, widget, x, py, width_proposed, height_proposed, &w, &h, clip);
		py += h;
		ch += h;
		cw = w > cw ? w : cw;
	}
	*used_width = cw;
	*used_height = ch;
}

void sxdl_container_get_line_size(SxdlContainer * container, int index, GtkWidget * widget,
	int width_proposed, int height_proposed, int * used_width, int * used_height)
{
	int lw = 0, lh = 0;
	GArray * a = g_array_index(container -> lines, GArray*, index);
	int e;
	for (e = 0; e < a -> len; e++){
		int ew, eh;
		SxdlBase * base = g_array_index(a, SxdlBase*, e);
		sxdl_base_get_size(base, widget, width_proposed, height_proposed, &ew, &eh);
		lw += ew;
		lh = eh > lh ? eh : lh;
	}
	*used_width = lw;
	*used_height = lh;
}

void sxdl_container_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed, int * used_width, int * used_height)
{
	SxdlContainer * container = (SxdlContainer*)sxdl_base;
	int cw = 0, ch = 0;
	int i;
	for (i = 0; i < container -> lines -> len; i++){
		int lw, lh;
		sxdl_container_get_line_size(container, i, widget, width_proposed, height_proposed, &lw, &lh);
		ch += lh;
		cw = lw > cw ? lw : cw;
	}
	*used_width = cw;
	*used_height = ch;
}

void sxdl_container_add_line(SxdlContainer * container)
{
	GArray * line = g_array_new(FALSE, FALSE, sizeof(SxdlBase*));
	g_array_append_val(container -> lines, line);
}

void sxdl_container_add(SxdlContainer * container, SxdlBase * child)
{
	int type = G_OBJECT_TYPE(child);
	if (container -> current_line >= container -> lines -> len){
		sxdl_container_add_line(container);
	}
	GArray * line = g_array_index(container -> lines, GArray*, container -> current_line);
	if (type == GOSM_TYPE_SXDL_BREAK){
		if (line -> len != 0){
			sxdl_container_add_line(container);
			container -> current_line += 1;
			line = g_array_index(container -> lines, GArray*, container -> current_line);
		}
		g_array_append_val(line, child);
		sxdl_container_add_line(container);
		container -> current_line += 1;
	}else{
		g_array_append_val(line, child);
	}
}
