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

#define GOSM_TYPE_MANUAL_DIALOG		(manual_dialog_get_type ())
#define GOSM_MANUAL_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_MANUAL_DIALOG, ManualDialog))
#define GOSM_MANUAL_DIALOG_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_MANUAL_DIALOG, ManualDialogClass))
#define GOSM_IS_MANUAL_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_MANUAL_DIALOG))
#define GOSM_IS_MANUAL_DIALOG_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_MANUAL_DIALOG))
#define GOSM_MANUAL_DIALOG_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_MANUAL_DIALOG, ManualDialogClass))

#ifndef _MANUAL_DIALOG_H_
#define _MANUAL_DIALOG_H_

typedef struct _ManualDialog		ManualDialog;
typedef struct _ManualDialogClass	ManualDialogClass;

struct _ManualDialog
{
	GtkWindow parent;
};

struct _ManualDialogClass
{
	GtkWindowClass parent_class;
};

GtkWidget * manual_dialog_new(GtkWindow * parent_window);

#endif
