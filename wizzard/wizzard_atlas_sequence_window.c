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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "wizzard_atlas_sequence_window.h"
#include "../paths.h"

G_DEFINE_TYPE (WizzardAtlasSequenceWindow, wizzard_atlas_sequence_window, GTK_TYPE_WINDOW);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint wizzard_atlas_sequence_window_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, wizzard_atlas_sequence_window_signals[SIGNAL_NAME_n], 0);

static gboolean button_filename_cb(GtkWidget * button, WizzardAtlasSequenceWindow * wasw);

WizzardAtlasSequenceWindow * wizzard_atlas_sequence_window_new()
{
	WizzardAtlasSequenceWindow * wizzard_atlas_sequence_window = g_object_new(GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE_WINDOW, NULL);
	return wizzard_atlas_sequence_window;
}

static void wizzard_atlas_sequence_window_class_init(WizzardAtlasSequenceWindowClass *class)
{
        /*wizzard_atlas_sequence_window_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (WizzardAtlasSequenceWindowClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void wizzard_atlas_sequence_window_init(WizzardAtlasSequenceWindow *wasw)
{
	wasw -> entry_filename = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(wasw -> entry_filename), "export");
	wasw -> button_filename = gtk_button_new();
	GtkWidget * icon_filename = gtk_image_new_from_file(GOSM_ICON_DIR "stock_save-template.png");
	gtk_button_set_image(GTK_BUTTON(wasw -> button_filename), icon_filename);
	wasw -> progress_bar  = gtk_progress_bar_new();
	wasw -> button_export = gtk_button_new_with_label("Export");
	wasw -> button_cancel = gtk_button_new_from_stock("gtk-cancel");
	GtkWidget * icon_image = gtk_image_new_from_file(GOSM_ICON_DIR "stock_insert_image.png");
	gtk_button_set_image(GTK_BUTTON(wasw -> button_export), icon_image);

	GtkWidget * box = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(wasw), box);

	GtkWidget * box_filename = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_filename), wasw -> entry_filename, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_filename), wasw -> button_filename, FALSE, FALSE, 0);

	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), wasw -> button_export, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), wasw -> button_cancel, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(box), box_filename, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), wasw -> progress_bar, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), box_buttons, FALSE, FALSE, 0);

	gtk_widget_set_size_request(wasw -> entry_filename, 400, -1);

	g_signal_connect(G_OBJECT(wasw -> button_filename), "clicked", G_CALLBACK(button_filename_cb), (gpointer)wasw);
}

void wizzard_atlas_sequence_window_set_inactive(WizzardAtlasSequenceWindow * wasw)
{
	gtk_widget_set_sensitive(wasw -> entry_filename, FALSE);
	gtk_widget_set_sensitive(wasw -> button_filename, FALSE);
	gtk_widget_set_sensitive(wasw -> button_export, FALSE);
}

static gboolean button_filename_cb(GtkWidget * button, WizzardAtlasSequenceWindow * wasw)
{
	GtkWidget * chooser = gtk_file_chooser_dialog_new("Select File Prefix",
					GTK_WINDOW(wasw),
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);
	gtk_window_set_modal(GTK_WINDOW(chooser), TRUE);
	int result = gtk_dialog_run(GTK_DIALOG(chooser));
	if (result == GTK_RESPONSE_CANCEL){
		gtk_widget_destroy(chooser);
	}
	if (result == GTK_RESPONSE_ACCEPT){
		char * filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
		gtk_widget_destroy(chooser);

		gtk_entry_set_text(GTK_ENTRY(wasw -> entry_filename), filename);
	}
}
