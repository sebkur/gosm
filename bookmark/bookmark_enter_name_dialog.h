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

#ifndef _BOOKMARK_ENTER_NAME_DIALOG_H_
#define _BOOKMARK_ENTER_NAME_DIALOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_BOOKMARK_ENTER_NAME_DIALOG           (bookmark_enter_name_dialog_get_type ())
#define GOSM_BOOKMARK_ENTER_NAME_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_BOOKMARK_ENTER_NAME_DIALOG, BookmarkEnterNameDialog))
#define GOSM_BOOKMARK_ENTER_NAME_DIALOG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_BOOKMARK_ENTER_NAME_DIALOG, BookmarkEnterNameDialogClass))
#define GOSM_IS_BOOKMARK_ENTER_NAME_DIALOG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_BOOKMARK_ENTER_NAME_DIALOG))
#define GOSM_IS_BOOKMARK_ENTER_NAME_DIALOG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_BOOKMARK_ENTER_NAME_DIALOG))
#define GOSM_BOOKMARK_ENTER_NAME_DIALOG_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_BOOKMARK_ENTER_NAME_DIALOG, BookmarkEnterNameDialogClass))

typedef struct _BookmarkEnterNameDialog        BookmarkEnterNameDialog;
typedef struct _BookmarkEnterNameDialogClass   BookmarkEnterNameDialogClass;

struct _BookmarkEnterNameDialog
{
	GObject parent;

	char * name;
};

struct _BookmarkEnterNameDialogClass
{
	GObjectClass parent_class;

	//void (* function_name) (BookmarkEnterNameDialog *bookmark_enter_name_dialog);
};

BookmarkEnterNameDialog * bookmark_enter_name_dialog_new();

char * bookmark_enter_name_dialog_get_name(BookmarkEnterNameDialog * bookmark_enter_name_dialog);

int bookmark_enter_name_dialog_run(BookmarkEnterNameDialog * bookmark_enter_name_dialog);

#endif /* _BOOKMARK_ENTER_NAME_DIALOG_H_ */
