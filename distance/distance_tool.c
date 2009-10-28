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

#include <glib.h>
#include <unistd.h>

#include "../paths.h"
#include "distance_tool.h"

/****************************************************************************************************
* this widget displays an entry with the current distance measured and some buttons for conrolling
* the current measurement-path
****************************************************************************************************/
G_DEFINE_TYPE (DistanceTool, distance_tool, GTK_TYPE_VBOX);

GtkWidget * distance_tool_new()
{
	DistanceTool * distance_tool = g_object_new(GOSM_TYPE_DISTANCE_TOOL, NULL);
	return GTK_WIDGET(distance_tool);
}

static void distance_tool_class_init(DistanceToolClass *class)
{
}

/****************************************************************************************************
* construct the widget
****************************************************************************************************/
static void distance_tool_init(DistanceTool *distance_tool)
{
	distance_tool -> entry_distance = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(distance_tool -> entry_distance), FALSE);
	distance_tool -> button_remove_last = gtk_button_new();
	distance_tool -> button_clear = gtk_button_new();
	distance_tool -> button_bookmark = gtk_button_new();

	GtkWidget *icon_remove_last = gtk_image_new_from_file(GOSM_ICON_DIR "stock_node-delete.png");
	GtkWidget *icon_clear = gtk_image_new_from_file(GOSM_ICON_DIR "edit-clear.png");
	GtkWidget *icon_bookmark = gtk_image_new_from_file(GOSM_ICON_DIR "stock_bookmark.png");

	gtk_button_set_image(GTK_BUTTON(distance_tool -> button_remove_last), icon_remove_last); 
	gtk_button_set_image(GTK_BUTTON(distance_tool -> button_clear), icon_clear); 
	gtk_button_set_image(GTK_BUTTON(distance_tool -> button_bookmark), icon_bookmark); 

	gtk_widget_set_tooltip_text(distance_tool -> button_remove_last, "remove last node");
	gtk_widget_set_tooltip_text(distance_tool -> button_clear, "clear path");
	gtk_widget_set_tooltip_text(distance_tool -> button_bookmark, "bookmark(not implemented)");

	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), distance_tool -> button_remove_last, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), distance_tool -> button_clear, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), distance_tool -> button_bookmark, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(distance_tool), distance_tool -> entry_distance, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(distance_tool), box_buttons, FALSE, FALSE, 0);
}

/****************************************************************************************************
* set the distance currently displayed
* TODO: this function is neither implemented nor used, though it should be
****************************************************************************************************/
void distance_tool_set_distance(DistanceTool * select_tool, int meters)
{
}
