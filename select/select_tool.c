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
#include <gdk/gdk.h>

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include "select_tool.h"
#include "../paths.h"

/****************************************************************************************************
* SelectTool is a widget that show information about the current selection and has some buttons
* to perform actions with this selection
****************************************************************************************************/
G_DEFINE_TYPE (SelectTool, select_tool, GTK_TYPE_VBOX);

/*enum
{
	TILE_LOADED_FROM_DISK,
	TILE_LOADED_FROM_NETW,
	LAST_SIGNAL
};*/

//static guint select_tool_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, select_tool_signals[TILE_LOADED_SUCCESFULLY], 0);

/****************************************************************************************************
* constructor
****************************************************************************************************/
SelectTool * select_tool_new()
{
	return g_object_new(GOSM_TYPE_SELECT_TOOL, NULL);
}

static void select_tool_class_init(SelectToolClass *class)
{
	/*select_tool_signals[TILE_LOADED_FROM_DISK] = g_signal_new(
		"tile-loaded-from-disk",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (SelectToolClass, tile_loaded_from_disk),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);*/
}

/****************************************************************************************************
* object init
****************************************************************************************************/
static void select_tool_init(SelectTool *select_tool)
{
	GtkWidget *label_lon = gtk_label_new("Lon:");
	GtkWidget *label_lat = gtk_label_new("Lat:");
	
	select_tool -> lon1 = gtk_entry_new();
	select_tool -> lon2 = gtk_entry_new();
	select_tool -> lat1 = gtk_entry_new();
	select_tool -> lat2 = gtk_entry_new();

	GtkWidget *icon1 = gtk_image_new_from_file(GOSM_ICON_DIR "square_top_left.png");
	GtkWidget *icon2 = gtk_image_new_from_file(GOSM_ICON_DIR "square_bottom_right.png");

	// VISIBLE
	select_tool -> check_show = gtk_check_button_new_with_label("visible");
	//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_tool->check_show), TRUE);
	GtkWidget *box_check_show = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_check_show), select_tool -> check_show, FALSE, FALSE, 0);

	// SNAP TO MAP
	select_tool -> check_snap = gtk_check_button_new_with_label("snap to map");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_tool->check_snap), FALSE);
	GtkWidget *box_check_snap = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_check_snap), select_tool -> check_snap, FALSE, FALSE, 0);

	GtkWidget * icon_download = gtk_image_new_from_file(GOSM_ICON_DIR "document-save.png");
	GtkWidget * icon_delete = gtk_image_new_from_file(GOSM_ICON_DIR "edit-delete.png");
	GtkWidget * icon_delete_adv = gtk_image_new_from_file(GOSM_ICON_DIR "edit-delete-advanced.png");
	GtkWidget * icon_export = gtk_image_new_from_file(GOSM_ICON_DIR "stock_insert_image.png");
	GtkWidget * icon_data = gtk_image_new_from_file(GOSM_ICON_DIR "stock_update-data.png");
	GtkWidget * icon_clipboard = gtk_image_new_from_file(GOSM_ICON_DIR "edit-copy.png");
	GtkWidget * icon_bookmark= gtk_image_new_from_file(GOSM_ICON_DIR "stock_bookmark.png");
	select_tool -> button_action = gtk_button_new();
	select_tool -> button_trash  = gtk_button_new();
	select_tool -> button_trash_adv  = gtk_button_new();
	select_tool -> button_export = gtk_button_new();
	select_tool -> button_data   = gtk_button_new();
	select_tool -> button_clipboard = gtk_button_new();
	select_tool -> button_bookmark = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(select_tool -> button_action), icon_download);
	gtk_button_set_image(GTK_BUTTON(select_tool -> button_trash), icon_delete);
	gtk_button_set_image(GTK_BUTTON(select_tool -> button_trash_adv), icon_delete_adv);
	gtk_button_set_image(GTK_BUTTON(select_tool -> button_export), icon_export);
	gtk_button_set_image(GTK_BUTTON(select_tool -> button_data), icon_data);
	gtk_button_set_image(GTK_BUTTON(select_tool -> button_clipboard), icon_clipboard);
	gtk_button_set_image(GTK_BUTTON(select_tool -> button_bookmark), icon_bookmark);
	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	GtkWidget * box_buttons2 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), select_tool -> button_action, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), select_tool -> button_trash, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), select_tool -> button_trash_adv, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), select_tool -> button_export, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons2), select_tool -> button_data, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons2), select_tool -> button_clipboard, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons2), select_tool -> button_bookmark, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(select_tool -> button_action, "load to disk");
	gtk_widget_set_tooltip_text(select_tool -> button_trash, "remove from disk (single layer)");
	gtk_widget_set_tooltip_text(select_tool -> button_trash_adv, "remove from disk (multiple layers)");
	gtk_widget_set_tooltip_text(select_tool -> button_export, "export image");
	gtk_widget_set_tooltip_text(select_tool -> button_data, "load xml data\n(copy api-url to clipboard)");
	gtk_widget_set_tooltip_text(select_tool -> button_clipboard, "copy to clipboard");
	gtk_widget_set_tooltip_text(select_tool -> button_bookmark, "bookmark(not implemented)");

	GtkWidget * table = gtk_table_new(6, 3, FALSE);
	gtk_table_attach(GTK_TABLE(table), label_lon,			1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_lat,			2, 3, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), icon1,			0, 1, 1, 2, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), icon2,			0, 1, 2, 3, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), select_tool->lon1,		1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), select_tool->lon2,		1, 2, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), select_tool->lat1,		2, 3, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), select_tool->lat2,		2, 3, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_check_show,		0, 3, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_check_snap,		0, 3, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_buttons,			0, 3, 5, 6, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_buttons2,		0, 3, 6, 7, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	gtk_widget_set_size_request(table, 200, -1);
	gtk_widget_set_size_request(select_tool -> lon1, 0 , -1);
	gtk_widget_set_size_request(select_tool -> lon2, 0 , -1);
	gtk_widget_set_size_request(select_tool -> lat1, 0 , -1);
	gtk_widget_set_size_request(select_tool -> lat2, 0 , -1);

	gtk_box_pack_start(GTK_BOX(select_tool), table, FALSE, FALSE, 0);
}

/****************************************************************************************************
* update the currently displayed values
****************************************************************************************************/
void select_tool_set_lons_n_lats(SelectTool * select_tool, double lon1, double lon2, double lat1, double lat2)
{
	double vals[4] = {lon1, lon2, lat1, lat2};
	GtkWidget* widgets[4] = {select_tool -> lon1, select_tool -> lon2, select_tool -> lat1, select_tool -> lat2};
	char buf[20]; int i;
	for (i = 0; i < 4; i++){
		sprintf(buf, "%.4f", (float)vals[i]);
		gtk_entry_set_text(GTK_ENTRY(widgets[i]), buf);
	}
}
