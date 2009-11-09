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

#include "node_add_tag_dialog.h"

G_DEFINE_TYPE (NodeAddTagDialog, node_add_tag_dialog, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint node_add_tag_dialog_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, node_add_tag_dialog_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* show a dialog asking for a key and a value for a new tag to add
* TODO: the returned strings should be freed on destroying the object
****************************************************************************************************/
NodeAddTagDialog * node_add_tag_dialog_new()
{
	NodeAddTagDialog * node_add_tag_dialog = g_object_new(GOSM_TYPE_NODE_ADD_TAG_DIALOG, NULL);
	return node_add_tag_dialog;
}

NodeAddTagDialog * node_add_tag_dialog_new_with_preset(char * key, char * value)
{
	NodeAddTagDialog * node_add_tag_dialog = g_object_new(GOSM_TYPE_NODE_ADD_TAG_DIALOG, NULL);
	node_add_tag_dialog -> key = key;
	node_add_tag_dialog -> value = value;
	return node_add_tag_dialog;
}

static void node_add_tag_dialog_class_init(NodeAddTagDialogClass *class)
{
        /*node_add_tag_dialog_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (NodeAddTagDialogClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void node_add_tag_dialog_init(NodeAddTagDialog *node_add_tag_dialog)
{
}

char * node_add_tag_dialog_get_key(NodeAddTagDialog * node_add_tag_dialog)
{
	return node_add_tag_dialog -> key;
}

char * node_add_tag_dialog_get_value(NodeAddTagDialog * node_add_tag_dialog)
{
	return node_add_tag_dialog -> value;
}

node_add_tag_dialog_key_press_cb(GtkWidget *widget, GdkEventKey *event, GtkDialog * dialog)
{
	if (event -> keyval == GDK_Return){
		gtk_dialog_response(dialog, GTK_RESPONSE_ACCEPT);
	}
	return FALSE;
}

/****************************************************************************************************
* show the dialog.
* return either GTK_RESPONSE_ACCEPT or GTK_RESPONSE_REJECT
* in the former case, the entered values can be retrieved with '_get_key()' and '_get_value()'
****************************************************************************************************/
int node_add_tag_dialog_run(NodeAddTagDialog * node_add_tag_dialog)
{
	GtkDialog * dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
		"Enter a new tag",
		NULL,
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,
		GTK_RESPONSE_ACCEPT,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_REJECT,
		NULL));
	GtkWidget * entry_key = gtk_entry_new();
	GtkWidget * entry_value = gtk_entry_new();
	if (node_add_tag_dialog -> key != NULL){
		gtk_entry_set_text(entry_key, node_add_tag_dialog -> key);
	}
	if (node_add_tag_dialog -> value != NULL){
		gtk_entry_set_text(entry_value, node_add_tag_dialog -> value);
	}
//	g_signal_connect(
//		G_OBJECT(entry_key), "key_press_event", 
//		G_CALLBACK(node_add_tag_dialog_key_press_cb), dialog);
	g_signal_connect(
		G_OBJECT(entry_value), "key_press_event", 
		G_CALLBACK(node_add_tag_dialog_key_press_cb), dialog);
	GtkBox * box = GTK_BOX(dialog -> vbox);
	gtk_box_pack_start(box, entry_key, FALSE, FALSE, 0);
	gtk_box_pack_start(box, entry_value, FALSE, FALSE, 0);
	gtk_widget_show_all(GTK_WIDGET(dialog));
	int response = gtk_dialog_run(dialog);
	if (response == GTK_RESPONSE_ACCEPT){
		const char * ckey = gtk_entry_get_text(GTK_ENTRY(entry_key));
		const char * cvalue = gtk_entry_get_text(GTK_ENTRY(entry_value));
		char * key = malloc(sizeof(char) * (strlen(ckey) + 1));
		char * value = malloc(sizeof(char) * (strlen(cvalue) + 1));
		strcpy(key, ckey);
		strcpy(value, cvalue);
		node_add_tag_dialog -> key = key;
		node_add_tag_dialog -> value = value;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return response == GTK_RESPONSE_ACCEPT ? response : GTK_RESPONSE_REJECT;
}
