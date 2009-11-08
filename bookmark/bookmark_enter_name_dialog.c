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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "bookmark_enter_name_dialog.h"

G_DEFINE_TYPE (BookmarkEnterNameDialog, bookmark_enter_name_dialog, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint bookmark_enter_name_dialog_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, bookmark_enter_name_dialog_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* show a dialog asking for a name of a bookmark
* TODO: the returned name-field should be freed on destroying the object
****************************************************************************************************/
BookmarkEnterNameDialog * bookmark_enter_name_dialog_new()
{
	BookmarkEnterNameDialog * bookmark_enter_name_dialog = g_object_new(GOSM_TYPE_BOOKMARK_ENTER_NAME_DIALOG, NULL);
	return bookmark_enter_name_dialog;
}

static void bookmark_enter_name_dialog_class_init(BookmarkEnterNameDialogClass *class)
{
        /*bookmark_enter_name_dialog_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (BookmarkEnterNameDialogClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void bookmark_enter_name_dialog_init(BookmarkEnterNameDialog *bookmark_enter_name_dialog)
{
}

char * bookmark_enter_name_dialog_get_name(BookmarkEnterNameDialog * bookmark_enter_name_dialog)
{
	return bookmark_enter_name_dialog -> name;
}

bookmark_enter_name_dialog_key_press_cb(GtkWidget *widget, GdkEventKey *event, GtkDialog * dialog)
{
	if (event -> keyval == GDK_Return){
		gtk_dialog_response(dialog, GTK_RESPONSE_ACCEPT);
	}
	return FALSE;
}

/****************************************************************************************************
* show the dialog.
* return either GTK_RESPONSE_ACCEPT or GTK_RESPONSE_REJECT
* in the former case, the entered name can be retrieved with '_get_name()'
****************************************************************************************************/
int bookmark_enter_name_dialog_run(BookmarkEnterNameDialog * bookmark_enter_name_dialog)
{
	GtkDialog * dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
		"Enter a name for this bookmark",
		NULL,
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,
		GTK_RESPONSE_ACCEPT,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_REJECT,
		NULL));
	GtkWidget * entry_name = gtk_entry_new();
	g_signal_connect(
		G_OBJECT(entry_name), "key_press_event", 
		G_CALLBACK(bookmark_enter_name_dialog_key_press_cb), dialog);
	GtkBox * box = GTK_BOX(dialog -> vbox);
	gtk_box_pack_start(box, entry_name, FALSE, FALSE, 0);
	gtk_widget_show_all(GTK_WIDGET(dialog));
	int response = gtk_dialog_run(dialog);
	if (response == GTK_RESPONSE_ACCEPT){
		const char * cname = gtk_entry_get_text(GTK_ENTRY(entry_name));
		char * name = malloc(sizeof(char) * (strlen(cname) + 1));
		strcpy(name, cname);
		bookmark_enter_name_dialog -> name = name;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return response == GTK_RESPONSE_ACCEPT ? response : GTK_RESPONSE_REJECT;
}
