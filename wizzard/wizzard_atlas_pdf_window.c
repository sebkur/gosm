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

#include "wizzard_atlas_pdf_window.h"
#include "../paths.h"

G_DEFINE_TYPE (WizzardAtlasPdfWindow, wizzard_atlas_pdf_window, GTK_TYPE_WINDOW);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint wizzard_atlas_pdf_window_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, wizzard_atlas_pdf_window_signals[SIGNAL_NAME_n], 0);

static gboolean button_filename_cb(GtkWidget * button, WizzardAtlasPdfWindow * wapw);

WizzardAtlasPdfWindow * wizzard_atlas_pdf_window_new()
{
	WizzardAtlasPdfWindow * wizzard_atlas_pdf_window = g_object_new(GOSM_TYPE_WIZZARD_ATLAS_PDF_WINDOW, NULL);
	return wizzard_atlas_pdf_window;
}

static void wizzard_atlas_pdf_window_class_init(WizzardAtlasPdfWindowClass *class)
{
        /*wizzard_atlas_pdf_window_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (WizzardAtlasPdfWindowClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void wizzard_atlas_pdf_window_init(WizzardAtlasPdfWindow *wapw)
{
	wapw -> entry_filename = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(wapw -> entry_filename), "export");
	wapw -> button_filename = gtk_button_new();
	GtkWidget * icon_filename = gtk_image_new_from_file(GOSM_ICON_DIR "stock_save-template.png");
	gtk_button_set_image(GTK_BUTTON(wapw -> button_filename), icon_filename);
	wapw -> progress_bar_images  = gtk_progress_bar_new();
	wapw -> progress_bar_pdf  = gtk_progress_bar_new();
	wapw -> button_export = gtk_button_new_with_label("Export");
	wapw -> button_cancel = gtk_button_new_from_stock("gtk-cancel");
	GtkWidget * icon_image = gtk_image_new_from_file(GOSM_ICON_DIR "stock_insert_image.png");
	gtk_button_set_image(GTK_BUTTON(wapw -> button_export), icon_image);

	GtkWidget * box = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(wapw), box);

	GtkWidget * box_filename = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_filename), wapw -> entry_filename, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_filename), wapw -> button_filename, FALSE, FALSE, 0);

	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), wapw -> button_export, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), wapw -> button_cancel, FALSE, FALSE, 0);

	GtkWidget * box1 = gtk_hbox_new(FALSE, 0);
	GtkWidget * box2 = gtk_hbox_new(FALSE, 0);
	GtkWidget * label_images = gtk_label_new("Images:");
	GtkWidget * label_pdf = gtk_label_new("PDF:");
	gtk_box_pack_start(GTK_BOX(box1), label_images, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box2), label_pdf, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(box), box_filename, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), box1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), wapw -> progress_bar_images, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), box2, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), wapw -> progress_bar_pdf, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), box_buttons, FALSE, FALSE, 0);

	gtk_widget_set_size_request(wapw -> entry_filename, 400, -1);

	g_signal_connect(G_OBJECT(wapw -> button_filename), "clicked", G_CALLBACK(button_filename_cb), (gpointer)wapw);
}

void wizzard_atlas_pdf_window_set_inactive(WizzardAtlasPdfWindow * wapw)
{
	gtk_widget_set_sensitive(wapw -> entry_filename, FALSE);
	gtk_widget_set_sensitive(wapw -> button_filename, FALSE);
	gtk_widget_set_sensitive(wapw -> button_export, FALSE);
}

static gboolean button_filename_cb(GtkWidget * button, WizzardAtlasPdfWindow * wapw)
{
	GtkWidget * chooser = gtk_file_chooser_dialog_new("Select File Prefix",
					GTK_WINDOW(wapw),
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

		gtk_entry_set_text(GTK_ENTRY(wapw -> entry_filename), filename);
	}
}
