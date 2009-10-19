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

#include "../map_area.h"

#define GOSM_TYPE_ABOUT_DIALOG		(about_dialog_get_type ())
#define GOSM_ABOUT_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_ABOUT_DIALOG, AboutDialog))
#define GOSM_ABOUT_DIALOG_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_ABOUT_DIALOG, AboutDialogClass))
#define GOSM_IS_ABOUT_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_ABOUT_DIALOG))
#define GOSM_IS_ABOUT_DIALOG_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_ABOUT_DIALOG))
#define GOSM_ABOUT_DIALOG_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_ABOUT_DIALOG, AboutDialogClass))

#ifndef _ABOUT_DIALOG_H_
#define _ABOUT_DIALOG_H_

typedef struct _AboutDialog		AboutDialog;
typedef struct _AboutDialogClass	AboutDialogClass;

struct _AboutDialog
{
	GtkWindow parent;

	GtkWidget *notebook;
};

struct _AboutDialogClass
{
	GtkWindowClass parent_class;
};

GtkWidget * about_dialog_new(GtkWindow * parent_window, MapArea * map_area);
GtkNotebook * about_dialog_get_notebook(AboutDialog * about_dialog);

#endif
