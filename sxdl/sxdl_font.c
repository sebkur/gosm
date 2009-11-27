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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_font.h"

G_DEFINE_TYPE (SxdlFont, sxdl_font, GOSM_TYPE_SXDL_BASE);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_font_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_font_signals[SIGNAL_NAME_n], 0);

void sxdl_font_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height);
void sxdl_font_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height);
void sxdl_font_paint(SxdlFont * sxdl_font, GtkWidget * widget, int * width, int * height, 
	gboolean actually_paint, int x, int y, int layout_width);

SxdlFont * sxdl_font_new(int size)
{
	SxdlFont * sxdl_font = g_object_new(GOSM_TYPE_SXDL_FONT, NULL);
	SxdlBase * base = GOSM_SXDL_BASE(sxdl_font);
	base -> size_cache = sxdl_size_cache_new();
	sxdl_font -> size = size;
	return sxdl_font;
}

void sxdl_font_set_text(SxdlFont * sxdl_font, char * text)
{
	sxdl_font -> text = g_strdup(text);
	//printf("%s %s\n", text, sxdl_font -> text);
}

static void sxdl_font_class_init(SxdlFontClass *class)
{
	SxdlBaseClass * sxdl_base_class = GOSM_SXDL_BASE_CLASS(class);
	sxdl_base_class -> render = sxdl_font_render;
	sxdl_base_class -> get_size = sxdl_font_get_size;
        /*sxdl_font_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlFontClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_font_init(SxdlFont *sxdl_font)
{
}

void sxdl_font_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height)
{
	SxdlFont * sxdl_font = (SxdlFont*)sxdl_base;
	int w, h;
	sxdl_font_paint(sxdl_font, widget, &w, &h, TRUE, x, y, width_proposed);
	*used_width = w;
	*used_height = h;
}

void sxdl_font_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height)
{
	//printf("%d\n", width_proposed);
	if (!sxdl_size_cache_get(sxdl_base -> size_cache, width_proposed, used_width, used_height)){
		//printf("MISS\n");
		SxdlFont * sxdl_font = (SxdlFont*)sxdl_base;
		sxdl_font_paint(sxdl_font, widget, used_width, used_height, FALSE, 0, 0, width_proposed);
		sxdl_size_cache_add(sxdl_base -> size_cache, width_proposed, *used_width, *used_height);
	}
}

void sxdl_font_paint(SxdlFont * sxdl_font, GtkWidget * widget, int * width, int * height, 
	gboolean actually_paint, int x, int y, int layout_width)
{
	char * text = sxdl_font -> text;
	cairo_t * cr_font = gdk_cairo_create(widget -> window);
	PangoContext * pc_marker = pango_cairo_create_context(cr_font);
	PangoLayout * pl_marker = pango_layout_new(pc_marker);
	pango_layout_set_text(pl_marker, text, -1);
	PangoAttrList * attrs = pango_attr_list_new();
	//PangoAttribute * weight = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
	PangoAttribute * size = pango_attr_size_new(PANGO_SCALE * 10);
	PangoAttribute * family = pango_attr_family_new("Sans-Serif");
	//pango_attr_list_insert(attrs, weight);
	pango_attr_list_insert(attrs, size);
	pango_attr_list_insert(attrs, family);
	pango_layout_set_attributes(pl_marker, attrs);
	pango_layout_set_width(pl_marker, PANGO_SCALE * layout_width);
	//pango_layout_set_alignment(pl_marker, PANGO_ALIGN_CENTER);

	PangoRectangle rect1, rect2;
	pango_layout_get_pixel_extents(pl_marker, &rect1, &rect2);
	*width = rect2.width; *height = rect2.height;
	if (actually_paint){
		cairo_move_to(cr_font, x, y);
		cairo_pattern_t * pat_font = cairo_pattern_create_rgba(0.0, 0.0, 0.0, 1.0);
		cairo_set_source(cr_font, pat_font);
		cairo_move_to(cr_font, x, y);
		pango_cairo_show_layout(cr_font, pl_marker);
		cairo_fill(cr_font);
		cairo_pattern_destroy(pat_font);
	}
	cairo_destroy(cr_font);
	g_object_unref(pc_marker);
	g_object_unref(pl_marker);
	pango_attr_list_unref(attrs);
	//printf("font\n");
}
