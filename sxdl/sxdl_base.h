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

#ifndef _SXDL_BASE_H_
#define _SXDL_BASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_size_cache.h"

#define GOSM_TYPE_SXDL_BASE           (sxdl_base_get_type ())
#define GOSM_SXDL_BASE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_BASE, SxdlBase))
#define GOSM_SXDL_BASE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_BASE, SxdlBaseClass))
#define GOSM_IS_SXDL_BASE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_BASE))
#define GOSM_IS_SXDL_BASE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_BASE))
#define GOSM_SXDL_BASE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_BASE, SxdlBaseClass))

typedef struct _SxdlBase        SxdlBase;
typedef struct _SxdlBaseClass   SxdlBaseClass;

struct _SxdlBase
{
	GObject parent;

	SxdlSizeCache * size_cache;
};

struct _SxdlBaseClass
{
	GObjectClass parent_class;
	
	void (* render) (SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
		int * used_width, int * used_height);
	void (* get_size) (SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
		int * used_width, int * used_height);
};

SxdlBase * sxdl_base_new();

void sxdl_base_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed, int * used_width, int * used_height);
void sxdl_base_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed, int * used_width, int * used_height);

#endif /* _SXDL_BASE_H_ */
