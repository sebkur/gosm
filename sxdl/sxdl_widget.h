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

#ifndef _SXDL_WIDGET_H_
#define _SXDL_WIDGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include "sxdl_base.h"
#include "sxdl_container.h"

#define GOSM_TYPE_SXDL_WIDGET           (sxdl_widget_get_type ())
#define GOSM_SXDL_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_WIDGET, SxdlWidget))
#define GOSM_SXDL_WIDGET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_WIDGET, SxdlWidgetClass))
#define GOSM_IS_SXDL_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_WIDGET))
#define GOSM_IS_SXDL_WIDGET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_WIDGET))
#define GOSM_SXDL_WIDGET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_WIDGET, SxdlWidgetClass))

typedef struct _SxdlWidget        SxdlWidget;
typedef struct _SxdlWidgetClass   SxdlWidgetClass;

struct _SxdlWidget
{
	GtkDrawingArea parent;

	GdkEventExpose * event;
	int tag;
	GArray * stack;
	char * text;
	SxdlContainer * document;

	GtkAdjustment * hadj;
	GtkAdjustment * vadj;
	int height_total;
	int height_visible;
	int offset_h;
	int offset_v;
};

struct _SxdlWidgetClass
{
	GtkDrawingAreaClass parent_class;

	//void (* function_name) (SxdlWidget *sxdl_widget);
};

SxdlWidget * sxdl_widget_new();

void sxdl_widget_set_uri(SxdlWidget * sxdl, char * filename);

#endif /* _SXDL_WIDGET_H_ */
