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

#include <gtk/gtk.h>
#include "../map_types.h"

#define GOSM_TYPE_SELECT_EXPORT_WINDOW		(select_export_window_get_type ())
#define GOSM_SELECT_EXPORT_WINDOW(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SELECT_EXPORT_WINDOW, SelectExportWindow))
#define GOSM_SELECT_EXPORT_WINDOW_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_SELECT_EXPORT_WINDOW, SelectExportWindowClass))
#define GOSM_IS_SELECT_EXPORT_WINDOW(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SELECT_EXPORT_WINDOW))
#define GOSM_IS_SELECT_EXPORT_WINDOW_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SELECT_EXPORT_WINDOW))
#define GOSM_SELECT_EXPORT_WINDOW_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SELECT_EXPORT_WINDOW, SelectExportWindowClass))

#ifndef _SELECT_EXPORT_WINDOW_H_
#define _SELECT_EXPORT_WINDOW_H_

typedef struct _SelectExportWindow		SelectExportWindow;
typedef struct _SelectExportWindowClass	SelectExportWindowClass;

struct _SelectExportWindow
{
	GtkWindow parent;

	Selection selection;

	GtkWidget *button_export;
	GtkWidget *button_cancel;
	
	GtkWidget *entry_filename;
	GtkWidget *button_filename;
	GtkWidget *text_width;
	GtkWidget *text_height;
	GtkWidget *combo;
	GtkWidget *progress_bar;
};

struct _SelectExportWindowClass
{
	GtkWindowClass parent_class;
};

GtkWidget * select_export_window_new(Selection * selection, gint zoom);

void select_export_window_set_inactive(SelectExportWindow * sew);

#endif
