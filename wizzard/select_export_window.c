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

#include <gtk/gtk.h>
#include <glib.h>

#include <unistd.h>
#include <string.h>

#include "select_export_window.h"
#include "../paths.h"
#include "../tiles/tilemath.h"

G_DEFINE_TYPE (SelectExportWindow, select_export_window, GTK_TYPE_WINDOW);

void select_export_window_construct(SelectExportWindow *select_export_window, gint zoom);
void select_export_window_update_size_labels(SelectExportWindow * select_export_window, gint zoom);
static gboolean combo_changed_cb(GtkComboBox *combo, gpointer windowp);
static gboolean button_filename_cb(GtkWidget *button, gpointer windowp);


GtkWidget * select_export_window_new(Selection * s, gint zoom)
{
	SelectExportWindow * win = g_object_new(GOSM_TYPE_SELECT_EXPORT_WINDOW, NULL);
	memcpy(&(win -> selection), s, sizeof(Selection));
	select_export_window_construct(win, zoom);
	return GTK_WIDGET(win);
}

static void select_export_window_class_init(SelectExportWindowClass *class)
{
	GtkWidgetClass *widget_class;
	widget_class = GTK_WIDGET_CLASS(class);
}

static void select_export_window_init(SelectExportWindow *select_export_window)
{
	printf("selection export window\n");
}

void select_export_window_set_inactive(SelectExportWindow * sew)
{
	gtk_widget_set_sensitive(sew -> entry_filename, FALSE);
	gtk_widget_set_sensitive(sew -> button_filename, FALSE);
	gtk_widget_set_sensitive(sew -> combo, FALSE);
	gtk_widget_set_sensitive(sew -> button_export, FALSE);
}

void select_export_window_construct(SelectExportWindow *select_export_window, gint zoom)
{
	Selection * selection = &(select_export_window -> selection);

	GtkBox * box_fileselect = GTK_BOX(gtk_hbox_new(FALSE,0));
	GtkBox * box_width 	= GTK_BOX(gtk_hbox_new(FALSE,0));
	GtkBox * box_height	= GTK_BOX(gtk_hbox_new(FALSE,0));
	GtkBox * box_zoom	= GTK_BOX(gtk_hbox_new(FALSE,0));
	GtkBox * box_buttons    = GTK_BOX(gtk_hbox_new(FALSE,0));

	// fileselect
	select_export_window -> entry_filename = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(select_export_window -> entry_filename), "export.png");
	select_export_window -> button_filename = gtk_button_new();
	GtkWidget * icon_filename = gtk_image_new_from_file(GOSM_ICON_DIR "stock_save-template.png");
        gtk_button_set_image(GTK_BUTTON(select_export_window -> button_filename), icon_filename);

	// table
	GtkWidget * label_width			= gtk_label_new("width:");
	GtkWidget * label_height		= gtk_label_new("height:");
	GtkWidget * label_zoom			= gtk_label_new("zoom:");
	select_export_window -> text_width	= gtk_label_new("0");
	select_export_window -> text_height	= gtk_label_new("0");
	select_export_window -> combo 		= gtk_combo_box_new_text();
	select_export_window -> progress_bar	= gtk_progress_bar_new();
	int i; char text[5];
	for (i = 0; i < 18; i++){
		sprintf(text, "%d", i+1);
		gtk_combo_box_append_text(GTK_COMBO_BOX(select_export_window -> combo), text);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(select_export_window -> combo), zoom - 1);

	// buttons
	select_export_window -> button_export = gtk_button_new_with_label("Export");
	select_export_window -> button_cancel = gtk_button_new_from_stock("gtk-cancel");

	//GtkWidget * icon_load = gtk_image_new_from_stock("gtk-harddisk", GTK_ICON_SIZE_BUTTON);
	GtkWidget * icon_image = gtk_image_new_from_file(GOSM_ICON_DIR "stock_insert_image.png");
        gtk_button_set_image(GTK_BUTTON(select_export_window -> button_export), icon_image);

	// layout
	gtk_box_pack_start(box_fileselect, select_export_window -> entry_filename,  TRUE, TRUE, 0);
	gtk_box_pack_start(box_fileselect, select_export_window -> button_filename, FALSE, FALSE, 0);

	gtk_box_pack_start(box_width, 	label_width, FALSE, FALSE, 0);
	gtk_box_pack_end  (box_width, 	select_export_window -> text_width, FALSE, FALSE, 0);
	gtk_box_pack_start(box_height, 	label_height, FALSE, FALSE, 0);
	gtk_box_pack_end  (box_height, 	select_export_window -> text_height, FALSE, FALSE, 0);
	gtk_box_pack_start(box_zoom, 	label_zoom, FALSE, FALSE, 0);
	gtk_box_pack_end  (box_zoom, 	select_export_window -> combo, FALSE, FALSE, 0);

	gtk_box_pack_start(box_buttons, select_export_window -> button_export, FALSE, FALSE, 0);
	gtk_box_pack_start(box_buttons, select_export_window -> button_cancel, FALSE, FALSE, 0);

	GtkWidget * box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(select_export_window), box);

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(box_fileselect), 	FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(box_width), 	FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(box_height), 	FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(box_zoom), 		FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(select_export_window -> progress_bar), 		FALSE, FALSE, 2);
	gtk_box_pack_end  (GTK_BOX(box), GTK_WIDGET(box_buttons), 	FALSE, FALSE, 2);

	gtk_widget_set_size_request(select_export_window -> entry_filename, 400, -1);

	select_export_window_update_size_labels(select_export_window, zoom);
	g_signal_connect(G_OBJECT(select_export_window -> combo), "changed", G_CALLBACK(combo_changed_cb), (gpointer)select_export_window);
	g_signal_connect(G_OBJECT(select_export_window -> button_filename), "clicked", G_CALLBACK(button_filename_cb), (gpointer)select_export_window);
}

void select_export_window_update_size_labels(SelectExportWindow * select_export_window, gint zoom)
{
	Selection s = select_export_window -> selection;
	double x1 = lon_to_x(s.lon1, zoom);
	double x2 = lon_to_x(s.lon2, zoom);
	double y1 = lat_to_y(s.lat1, zoom);
	double y2 = lat_to_y(s.lat2, zoom);
	int width  = x2 * 256 - x1 * 256;
	int height = y2 * 256 - y1 * 256;

	char text_x[10];
	char text_y[10];
	sprintf(text_x, "%d", width);
	sprintf(text_y, "%d", height);

	gtk_label_set_text(GTK_LABEL(select_export_window -> text_width), text_x);
	gtk_label_set_text(GTK_LABEL(select_export_window -> text_height), text_y);
}

static gboolean combo_changed_cb(GtkComboBox *combo, gpointer windowp)
{
	SelectExportWindow * sew = GOSM_SELECT_EXPORT_WINDOW(windowp);
	gint zoom = gtk_combo_box_get_active(combo) + 1;
	select_export_window_update_size_labels(sew, zoom);
}

static gboolean button_filename_cb(GtkWidget *button, gpointer windowp)
{
	SelectExportWindow * sew = GOSM_SELECT_EXPORT_WINDOW(windowp);
	GtkWidget * chooser = gtk_file_chooser_dialog_new("Select Filename",
					GTK_WINDOW(sew),
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

		gtk_entry_set_text(GTK_ENTRY(sew -> entry_filename), filename);
	}
}
