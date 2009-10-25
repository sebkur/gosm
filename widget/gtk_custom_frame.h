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

#ifndef _GTK_CUSTOM_FRAME_H_
#define _GTK_CUSTOM_FRAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_GTK_CUSTOM_FRAME           (gtk_custom_frame_get_type ())
#define GOSM_GTK_CUSTOM_FRAME(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_GTK_CUSTOM_FRAME, GtkCustomFrame))
#define GOSM_GTK_CUSTOM_FRAME_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_GTK_CUSTOM_FRAME, GtkCustomFrameClass))
#define GOSM_IS_GTK_CUSTOM_FRAME(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_GTK_CUSTOM_FRAME))
#define GOSM_IS_GTK_CUSTOM_FRAME_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_GTK_CUSTOM_FRAME))
#define GOSM_GTK_CUSTOM_FRAME_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_GTK_CUSTOM_FRAME, GtkCustomFrameClass))

typedef struct _GtkCustomFrame        GtkCustomFrame;
typedef struct _GtkCustomFrameClass   GtkCustomFrameClass;

struct _GtkCustomFrame
{
	GtkVBox parent;

	GtkWidget * button_rollup;
	GtkWidget * frame_child;
	GtkWidget * child;
	gboolean child_visible;
};

struct _GtkCustomFrameClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (GtkCustomFrame *gtk_custom_frame);
};

GtkWidget * gtk_custom_frame_new(char * title);

void gtk_custom_frame_add(GtkCustomFrame * gtk_custom_frame, GtkWidget * widget);

gboolean gtk_custom_frame_get_child_visible(GtkCustomFrame * gtk_custom_frame);
void gtk_custom_frame_set_child_visible(GtkCustomFrame * gtk_custom_frame, gboolean visible);

#endif /* _GTK_CUSTOM_FRAME_H_ */
