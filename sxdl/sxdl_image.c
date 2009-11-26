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

#include "sxdl_image.h"

G_DEFINE_TYPE (SxdlImage, sxdl_image, GOSM_TYPE_SXDL_BASE);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_image_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_image_signals[SIGNAL_NAME_n], 0);

void sxdl_image_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height);
void sxdl_image_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height);

SxdlImage * sxdl_image_new(char * src)
{
	SxdlImage * sxdl_image = g_object_new(GOSM_TYPE_SXDL_IMAGE, NULL);
	sxdl_image -> src = g_strdup(src);
	sxdl_image -> width = -1;
	sxdl_image -> height = -1;
	return sxdl_image;
}

static void sxdl_image_class_init(SxdlImageClass *class)
{
	SxdlBaseClass * sxdl_base_class = GOSM_SXDL_BASE_CLASS(class);
	sxdl_base_class -> render = sxdl_image_render;
	sxdl_base_class -> get_size = sxdl_image_get_size;
        /*sxdl_image_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlImageClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_image_init(SxdlImage *sxdl_image)
{
}

sxdl_image_paint(SxdlImage * image, GtkWidget * widget, int * w, int * h, gboolean actually_paint, int x, int y, int width_proposed)
{
	cairo_t * cr = gdk_cairo_create(widget -> window);
	if (image -> surface_img == NULL){
		image -> surface_img = cairo_image_surface_create_from_png (image -> src);
	}
	cairo_set_source_surface(cr, image -> surface_img, x, y);
	int img_w = cairo_image_surface_get_width(image -> surface_img);
	int img_h = cairo_image_surface_get_height(image -> surface_img);
	if (actually_paint){
		cairo_rectangle(cr, x, y, img_w, img_h);
		cairo_fill(cr);
	}
	*w = img_w;
	*h = img_h;
	//printf("image\n");
}

void sxdl_image_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height)
{
	SxdlImage * sxdl_image = (SxdlImage*)sxdl_base;
	int w, h;
	sxdl_image_paint(sxdl_image, widget, &w, &h, TRUE, x, y, width_proposed);
	*used_width = w;
	*used_height = h;
}

void sxdl_image_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed, int * used_width, int * used_height)
{
	SxdlImage * sxdl_image = (SxdlImage*)sxdl_base;
	sxdl_image_paint(sxdl_image, widget, used_width, used_height, FALSE, 0, 0, width_proposed);
}
