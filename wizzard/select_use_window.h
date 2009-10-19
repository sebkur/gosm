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

#define GOSM_TYPE_SELECT_USE_WINDOW		(select_use_window_get_type ())
#define GOSM_SELECT_USE_WINDOW(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SELECT_USE_WINDOW, SelectUseWindow))
#define GOSM_SELECT_USE_WINDOW_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_SELECT_USE_WINDOW, SelectUseWindowClass))
#define GOSM_IS_SELECT_USE_WINDOW(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SELECT_USE_WINDOW))
#define GOSM_IS_SELECT_USE_WINDOW_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SELECT_USE_WINDOW))
#define GOSM_SELECT_USE_WINDOW_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SELECT_USE_WINDOW, SelectUseWindowClass))

#ifndef _SELECT_USE_WINDOW_H_
#define _SELECT_USE_WINDOW_H_

typedef struct _SelectUseWindow		SelectUseWindow;
typedef struct _SelectUseWindowClass	SelectUseWindowClass;

struct _SelectUseWindow
{
	GtkWindow parent;

	GtkWidget *button_load;
	GtkWidget *button_cancel;

	Selection selection;
	//gboolean to_load[18];
	GtkWidget* to_load_wid[18];
};

struct _SelectUseWindowClass
{
	GtkWindowClass parent_class;
};

GtkWidget * select_use_window_new(Selection * selection);

//void select_use_window_setup_for_selection(SelectUseWindow *select_use_window, Selection * selection);

#endif
