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

#include "map_area.h"
#include "map_navigator.h"

G_DEFINE_TYPE (MapNavigator, map_navigator, GTK_TYPE_VBOX);

typedef struct AreaDirection{
	MapArea * map_area;
	int direction;
} AreaDirection;

enum
{
	CONTROLS_TOGGLED,
	LAST_SIGNAL
};

static guint map_navigator_signals[LAST_SIGNAL] = { 0 };

void map_navigator_construct(MapNavigator *map_navigator);

GtkWidget * map_navigator_new(GtkWidget * area)
{
	MapNavigator * map_navigator = g_object_new(GOSM_TYPE_MAP_NAVIGATOR, NULL);
	map_navigator -> area = area;
	map_navigator -> controls_visible = TRUE;
	map_navigator_construct(map_navigator);
	return GTK_WIDGET(map_navigator);
}

static void map_navigator_class_init(MapNavigatorClass *class)
{
	map_navigator_signals[CONTROLS_TOGGLED] = g_signal_new(
		"controls-toggled",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapNavigatorClass, controls_toggled),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
}

static void map_navigator_init(MapNavigator *map_navigator)
{
}

/****************************************************************************************************
* callback for the buttons around the map-widget
****************************************************************************************************/
static gboolean navigate_cb(GtkWidget *widget, AreaDirection * area_dir)
{
        map_area_move(area_dir -> map_area, area_dir -> direction);
	gtk_widget_grab_focus(GTK_WIDGET(area_dir -> map_area));
}

/****************************************************************************************************
* after interaction with the buttons, redirect focus to the map-widget
****************************************************************************************************/
static gboolean focus_redirect_cb(GtkWidget *widget, GdkEventButton * event, MapArea * area)
{
	gtk_widget_grab_focus(GTK_WIDGET(area));
	return FALSE;
}

/****************************************************************************************************
* get/set/toggle controls buttons around map-widget
****************************************************************************************************/
gboolean map_navigator_get_controls_visible(MapNavigator * map_navigator)
{
	return map_navigator -> controls_visible;
}
void map_navigator_show_controls(MapNavigator *map_navigator, gboolean show)
{
	if (show != map_navigator -> controls_visible){
		map_navigator -> controls_visible = show;
		int i; for (i = 0; i < 8; i++){
			if (show){
				gtk_widget_show(map_navigator -> buttons[i]);
			}else{
				gtk_widget_hide(map_navigator -> buttons[i]);
			}
		}
		g_signal_emit (map_navigator, map_navigator_signals[CONTROLS_TOGGLED], 0);
	}
}
void map_navigator_toggle_controls(MapNavigator *map_navigator)
{
	map_navigator_show_controls(map_navigator, !map_navigator_get_controls_visible(map_navigator));
}

/****************************************************************************************************
* build up the widget
****************************************************************************************************/
void map_navigator_construct(MapNavigator *map_navigator){
	GtkBox * vbox = GTK_BOX(map_navigator);
	GtkWidget ** buttons = map_navigator -> buttons;
	int i;
        for (i = 0; i < 8; i++){
                buttons[i] = gtk_button_new();
        }
        gtk_button_set_image(GTK_BUTTON(buttons[1]), gtk_image_new_from_stock(GTK_STOCK_GO_UP, GTK_ICON_SIZE_BUTTON));
        gtk_button_set_image(GTK_BUTTON(buttons[3]), gtk_image_new_from_stock(GTK_STOCK_GO_BACK, GTK_ICON_SIZE_BUTTON));
        gtk_button_set_image(GTK_BUTTON(buttons[4]), gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_BUTTON));
        gtk_button_set_image(GTK_BUTTON(buttons[6]), gtk_image_new_from_stock(GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_BUTTON));

        GtkWidget *hbox1 = gtk_hbox_new(FALSE, 1);
        GtkWidget *hbox2 = gtk_hbox_new(FALSE, 1);
        GtkWidget *hbox3 = gtk_hbox_new(FALSE, 1);

        GTK_OBJECT_SET_FLAGS(map_navigator -> area, GTK_CAN_FOCUS);

        gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, TRUE, 0);

        gtk_box_pack_start(GTK_BOX(hbox1), buttons[0], FALSE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox1), buttons[1], TRUE,  TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox1), buttons[2], FALSE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox2), buttons[3], FALSE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(map_navigator -> area),    TRUE,  TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox2), buttons[4], FALSE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox3), buttons[5], FALSE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox3), buttons[6], TRUE,  TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox3), buttons[7], FALSE, TRUE, 0);

	int ds = 26;
        gtk_widget_set_size_request(GTK_WIDGET(buttons[0]), ds, ds);
        gtk_widget_set_size_request(GTK_WIDGET(buttons[1]), 0,  ds);
        gtk_widget_set_size_request(GTK_WIDGET(buttons[2]), ds, ds);
        gtk_widget_set_size_request(GTK_WIDGET(buttons[3]), ds, 0);
        gtk_widget_set_size_request(GTK_WIDGET(buttons[4]), ds, 0);
        gtk_widget_set_size_request(GTK_WIDGET(buttons[5]), ds, ds);
        gtk_widget_set_size_request(GTK_WIDGET(buttons[6]), 0,  ds);
        gtk_widget_set_size_request(GTK_WIDGET(buttons[7]), ds, ds);

	AreaDirection * ad = malloc(sizeof(AreaDirection) * 8);
	int k;
	for (k = 0; k < 8; k++){
		ad[k].map_area = GOSM_MAP_AREA(map_navigator -> area);
		ad[k].direction = k + 1;
	}
	for (k = 0; k < 8; k++){
	        g_signal_connect(G_OBJECT(buttons[k]), "clicked", G_CALLBACK(navigate_cb), &ad[k]);
		g_signal_connect(G_OBJECT(buttons[k]), "button-release-event", G_CALLBACK(focus_redirect_cb), map_navigator -> area);
	}
	g_signal_emit (map_navigator, map_navigator_signals[CONTROLS_TOGGLED], 0);
}
