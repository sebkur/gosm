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

#ifndef _GTK_ROLLUP_BUTTON_H_
#define _GTK_ROLLUP_BUTTON_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_GTK_ROLLUP_BUTTON           (gtk_rollup_button_get_type ())
#define GOSM_GTK_ROLLUP_BUTTON(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_GTK_ROLLUP_BUTTON, GtkRollupButton))
#define GOSM_GTK_ROLLUP_BUTTON_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_GTK_ROLLUP_BUTTON, GtkRollupButtonClass))
#define GOSM_IS_GTK_ROLLUP_BUTTON(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_GTK_ROLLUP_BUTTON))
#define GOSM_IS_GTK_ROLLUP_BUTTON_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_GTK_ROLLUP_BUTTON))
#define GOSM_GTK_ROLLUP_BUTTON_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_GTK_ROLLUP_BUTTON, GtkRollupButtonClass))

typedef struct _GtkRollupButton        GtkRollupButton;
typedef struct _GtkRollupButtonClass   GtkRollupButtonClass;

struct _GtkRollupButton
{
	GtkDrawingArea parent;

	gboolean active;
};

struct _GtkRollupButtonClass
{
	GtkDrawingAreaClass parent_class;

	void (* toggled) (GtkRollupButton *gtk_rollup_button);
};

GtkWidget * gtk_rollup_button_new();

gboolean gtk_rollup_button_get_active(GtkRollupButton * button);
void gtk_rollup_button_set_active(GtkRollupButton * button, gboolean active);

#endif /* _GTK_ROLLUP_BUTTON_H_ */
