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

#include "toolbar.h"
#include "paths.h"

G_DEFINE_TYPE (Toolbar, toolbar, GTK_TYPE_TOOLBAR);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint toolbar_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, toolbar_signals[SIGNAL_NAME_n], 0);

static gboolean button_network_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_mouse_mode_cb(GtkWidget *widget, gpointer cursor_id_p);
static gboolean button_zoom_in_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_zoom_out_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_grid_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_font_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_side_bar_left_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_map_controls_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_side_bar_right_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean combo_tiles_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean button_savemaptype_cb(GtkWidget * widget, gpointer toolbar_p);

static gboolean toolbar_network_state_cb(MapArea * map_area, gpointer toolbar_p);
static gboolean toolbar_mouse_mode_cb(MapArea * map_area, gpointer toolbar_p);
static gboolean toolbar_grid_cb(MapArea * map_area, gpointer toolbar_p);
static gboolean toolbar_font_cb(MapArea * map_area, gpointer toolbar_p);
static gboolean toolbar_side_bar_left_visibility_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean toolbar_map_navigator_visibility_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean toolbar_side_bar_right_visibility_cb(GtkWidget *widget, gpointer toolbar_p);
static gboolean toolbar_tileset_cb(MapArea * map_area, gpointer toolbar_p);
static gboolean focus_redirect_cb(GtkWidget *widget, GdkEventButton * event, gpointer toolbar_p);

GtkWidget * toolbar_new(
	MapArea * map_area, 
	Config * config, 
	MapNavigator * map_navigator, 
	GtkWidget * side_left, 
	GtkWidget * side_right)
{
	Toolbar * toolbar = g_object_new(GOSM_TYPE_TOOLBAR, NULL);
	toolbar -> map_area = map_area;
	toolbar -> config = config;
	toolbar -> map_navigator = map_navigator;
	toolbar -> side_right = side_right;
	toolbar -> side_left = side_left;
	
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

	toolbar -> button_network = gtk_button_new();

	/********************************************************************************************
	* buttons that control the map's mouse mode
	********************************************************************************************/
	GtkWidget * icons_mouse_modes[MAP_MODE_COUNT];
	icons_mouse_modes[MAP_MODE_MOVE] = gtk_image_new_from_file(GOSM_ICON_DIR "navigate.png");
	icons_mouse_modes[MAP_MODE_SELECT] = gtk_image_new_from_file(GOSM_ICON_DIR "select.png");
	icons_mouse_modes[MAP_MODE_PATH] = gtk_image_new_from_file(GOSM_ICON_DIR "measure.png");
	icons_mouse_modes[MAP_MODE_POI] = gtk_image_new_from_file(GOSM_ICON_DIR "insert-object.png");
	toolbar -> toolbar_buttons = malloc(MAP_MODE_COUNT * sizeof(GtkButton*));
	int mode;
	for (mode = 0; mode < MAP_MODE_COUNT; mode++){
		toolbar -> toolbar_buttons[mode] = gtk_toggle_button_new();
		g_object_set_data(G_OBJECT(toolbar -> toolbar_buttons[mode]), "index", GINT_TO_POINTER(mode));
		gtk_button_set_image(GTK_BUTTON(toolbar -> toolbar_buttons[mode]), icons_mouse_modes[mode]);
	}

	/********************************************************************************************
	* all other items in the toolbar
	********************************************************************************************/
	GtkWidget * separator1 = gtk_hseparator_new();

	GtkWidget * icon_grid 		= gtk_image_new_from_file(GOSM_ICON_DIR "grid.png");
	GtkWidget * icon_font 		= gtk_image_new_from_file(GOSM_ICON_DIR "font.png");
	GtkWidget * icon_zoom_in 	= gtk_image_new_from_stock("gtk-zoom-in", GTK_ICON_SIZE_BUTTON);
	GtkWidget * icon_zoom_out 	= gtk_image_new_from_stock("gtk-zoom-out", GTK_ICON_SIZE_BUTTON);
	GtkWidget * icon_navi		= gtk_image_new_from_file(GOSM_ICON_DIR "stock_form-navigator.png");
	GtkWidget * icon_hide1		= gtk_image_new_from_file(GOSM_ICON_DIR "stock_show-hidden-controls.png");
	GtkWidget * icon_hide2		= gtk_image_new_from_file(GOSM_ICON_DIR "stock_show-hidden-controls2.png");
	GtkWidget * icon_cross		= gtk_image_new_from_file(GOSM_ICON_DIR "cross.png");

	GtkWidget * button_zoom_in = gtk_button_new();
	GtkWidget * button_zoom_out = gtk_button_new();
	toolbar -> button_grid = gtk_toggle_button_new();
	toolbar -> button_font = gtk_toggle_button_new();
	toolbar -> button_map_controls = gtk_toggle_button_new();
	toolbar -> button_side_bar_right = gtk_toggle_button_new();
	toolbar -> button_side_bar_left = gtk_toggle_button_new();
	GtkWidget * button_savemaptype = gtk_button_new();

	gtk_button_set_image(GTK_BUTTON(toolbar -> button_grid), icon_grid);
	gtk_button_set_image(GTK_BUTTON(button_zoom_in), icon_zoom_in);
	gtk_button_set_image(GTK_BUTTON(button_zoom_out), icon_zoom_out);
	gtk_button_set_image(GTK_BUTTON(toolbar -> button_font), icon_font);
	gtk_button_set_image(GTK_BUTTON(toolbar -> button_map_controls), icon_navi);
	gtk_button_set_image(GTK_BUTTON(toolbar -> button_side_bar_right), icon_hide1);
	gtk_button_set_image(GTK_BUTTON(toolbar -> button_side_bar_left), icon_hide2);
	gtk_button_set_image(GTK_BUTTON(button_savemaptype), icon_cross);

	/********************************************************************************************
	* the combo box
	********************************************************************************************/
	toolbar -> combo_tiles = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(toolbar -> combo_tiles), "Mapnik");
	gtk_combo_box_append_text(GTK_COMBO_BOX(toolbar -> combo_tiles), "Osmarender");
	gtk_combo_box_append_text(GTK_COMBO_BOX(toolbar -> combo_tiles), "Cycle");
	gtk_combo_box_set_active(GTK_COMBO_BOX(toolbar -> combo_tiles), map_area_get_tileset(toolbar -> map_area));
	combo_tiles_cb(toolbar -> combo_tiles, (gpointer)toolbar);

	/********************************************************************************************
	* insert items into toolbar
	********************************************************************************************/
	int tc = 0;
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> button_network, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> toolbar_buttons[0], NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> toolbar_buttons[1], NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> toolbar_buttons[2], NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> toolbar_buttons[3], NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_zoom_in, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_zoom_out, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> button_grid, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> button_font, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> button_side_bar_left, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> button_map_controls, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> button_side_bar_right, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar -> combo_tiles, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_savemaptype, NULL, NULL, tc++);

	/********************************************************************************************
	* tooltips
	********************************************************************************************/
	gtk_widget_set_tooltip_text(toolbar -> toolbar_buttons[0], "Navigation mode");
	gtk_widget_set_tooltip_text(toolbar -> toolbar_buttons[1], "Selection mode");
	gtk_widget_set_tooltip_text(toolbar -> toolbar_buttons[2], "Measure mode");
	gtk_widget_set_tooltip_text(toolbar -> toolbar_buttons[3], "Point of Interest mode");
	gtk_widget_set_tooltip_text(button_zoom_in, "zoom in");
	gtk_widget_set_tooltip_text(button_zoom_out, "zoom out");
	gtk_widget_set_tooltip_text(toolbar -> button_grid, "grid");
	gtk_widget_set_tooltip_text(toolbar -> button_font, "tilenames");
	gtk_widget_set_tooltip_text(toolbar -> button_side_bar_left, "left sidebar");
	gtk_widget_set_tooltip_text(toolbar -> button_map_controls, "navigation buttons");
	gtk_widget_set_tooltip_text(toolbar -> button_side_bar_right, "right sidebar");
	gtk_widget_set_tooltip_text(button_savemaptype, "Save map and position");

	/********************************************************************************************
	* react on properties that can be changed by toolbar
	********************************************************************************************/
	g_signal_connect(
		G_OBJECT(toolbar -> map_area), 	"map-mouse-mode-changed", 
		G_CALLBACK(toolbar_mouse_mode_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> map_area), 	"map-network-state-changed", 
		G_CALLBACK(toolbar_network_state_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> map_area), "map-tileset-changed", 
		G_CALLBACK(toolbar_tileset_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> map_area), "map-grid-toggled", 
		G_CALLBACK(toolbar_grid_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> map_area), "map-font-toggled", 
		G_CALLBACK(toolbar_font_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> side_left),	"hide", 
		G_CALLBACK(toolbar_side_bar_left_visibility_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> side_left),	"show", 
		G_CALLBACK(toolbar_side_bar_left_visibility_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> side_right), "hide", 
		G_CALLBACK(toolbar_side_bar_right_visibility_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> side_right), "show", 
		G_CALLBACK(toolbar_side_bar_right_visibility_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> map_navigator), "controls-toggled", 
		G_CALLBACK(toolbar_map_navigator_visibility_cb), (gpointer)toolbar);

	/********************************************************************************************
	* item callbacks
	********************************************************************************************/
	for (mode = 0; mode < MAP_MODE_COUNT; mode++){
		g_signal_connect(
			G_OBJECT(toolbar -> toolbar_buttons[mode]), "toggled", 
			G_CALLBACK(button_mouse_mode_cb), (gpointer)toolbar);
	}
	g_signal_connect(
		G_OBJECT(toolbar -> button_grid), "toggled", 
		G_CALLBACK(button_grid_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_font), "toggled", 
		G_CALLBACK(button_font_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(button_zoom_in), "clicked", 
		G_CALLBACK(button_zoom_in_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(button_zoom_out), "clicked", 
		G_CALLBACK(button_zoom_out_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_side_bar_left),"toggled", 
		G_CALLBACK(button_side_bar_left_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_map_controls), "clicked", 
		G_CALLBACK(button_map_controls_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_side_bar_right),"toggled", 
		G_CALLBACK(button_side_bar_right_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_network), "clicked", 
		G_CALLBACK(button_network_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(button_savemaptype), "clicked", 
		G_CALLBACK(button_savemaptype_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> combo_tiles), "changed", 
		G_CALLBACK(combo_tiles_cb), (gpointer)toolbar);

	/********************************************************************************************
	* focus redirection
	********************************************************************************************/
	g_signal_connect(
		G_OBJECT(toolbar -> button_network), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	for (mode = 0; mode < MAP_MODE_COUNT; mode++){
		g_signal_connect(
			G_OBJECT(toolbar -> toolbar_buttons[mode]), "button-release-event", 
			G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	}
	g_signal_connect(
		G_OBJECT(button_zoom_in), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(button_zoom_out), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_grid), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_font), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_side_bar_left), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_map_controls), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(toolbar -> button_side_bar_right), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);
	g_signal_connect(
		G_OBJECT(button_savemaptype), "button-release-event", 
		G_CALLBACK(focus_redirect_cb), (gpointer)toolbar);

	toolbar_network_state_cb(toolbar -> map_area, toolbar);
	gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(toolbar -> button_map_controls),
		map_navigator_get_controls_visible(map_navigator));

	return GTK_WIDGET(toolbar);
}

static void toolbar_class_init(ToolbarClass *class)
{
        /*toolbar_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ToolbarClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void toolbar_init(Toolbar *toolbar)
{
}

/****************************************************************************************************
* when a button was clicked, redirect the focus to the map,
* so that keyboard-shortcuts still work
****************************************************************************************************/
static gboolean focus_redirect_cb(GtkWidget *widget, GdkEventButton * event, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gtk_widget_grab_focus(GTK_WIDGET(toolbar -> map_area));
	return FALSE;
}

/****************************************************************************************************
* show / hide tilenames
****************************************************************************************************/
static gboolean toolbar_font_cb(MapArea * map_area, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gboolean visible = map_area_get_show_font(toolbar -> map_area);
	gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_font));
	if (visible != active){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar -> button_font), visible);
	}
}
static gboolean button_font_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	map_area_set_show_font(toolbar -> map_area, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

/****************************************************************************************************
* show / hide grid
****************************************************************************************************/
static gboolean toolbar_grid_cb(MapArea * map_area, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gboolean visible = map_area_get_show_grid(toolbar -> map_area);
	gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_grid));
	if (visible != active){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar -> button_grid), visible);
	}
}
static gboolean button_grid_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	map_area_set_show_grid(toolbar -> map_area, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

/****************************************************************************************************
* show / hide the navigation-buttons around the map
****************************************************************************************************/
static gboolean toolbar_map_navigator_visibility_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gboolean visible = map_navigator_get_controls_visible(toolbar -> map_navigator);
	gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_map_controls));
	if (visible != active){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar -> button_map_controls), visible);
	}
}
static gboolean button_map_controls_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	map_navigator_show_controls(toolbar -> map_navigator,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_map_controls)));
}

/****************************************************************************************************
* show / hide the sidebar on the right
****************************************************************************************************/
static gboolean toolbar_side_bar_right_visibility_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gboolean visible = GTK_WIDGET_VISIBLE(widget);
	gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_side_bar_right));
	if (visible != active){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar -> button_side_bar_right), visible);
	}
}
static gboolean button_side_bar_right_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_side_bar_right));
	if (state){
		gtk_widget_show(toolbar -> side_right);
	}else{
		gtk_widget_hide(toolbar -> side_right);
	}
}

/****************************************************************************************************
* show / hide the sidebar on the left
****************************************************************************************************/
static gboolean toolbar_side_bar_left_visibility_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gboolean visible = GTK_WIDGET_VISIBLE(widget);
	gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_side_bar_left));
	if (visible != active){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar -> button_side_bar_left), visible);
	}
}
static gboolean button_side_bar_left_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toolbar -> button_side_bar_left));
	if (state){
		gtk_widget_show(toolbar -> side_left);
	}else{
		gtk_widget_hide(toolbar -> side_left);
	}
}

/****************************************************************************************************
* select tileset / react on change of tileset
****************************************************************************************************/
static gboolean toolbar_tileset_cb(MapArea * map_area, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	GtkComboBox * combo = GTK_COMBO_BOX(toolbar -> combo_tiles);
	Tileset active = map_area_get_tileset(map_area);
	if (gtk_combo_box_get_active(combo) != active){
		gtk_combo_box_set_active(combo, active);
	}
}
static gboolean combo_tiles_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
	map_area_set_tileset(toolbar -> map_area, active);
	return FALSE;
}

/****************************************************************************************************
* save the current position and selected tileset to the config
****************************************************************************************************/
static gboolean button_savemaptype_cb(GtkWidget * widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	char tmp[20]; 
	sprintf(tmp,"%d",map_area_get_tileset(toolbar -> map_area));
	config_set_entry(toolbar -> config, "tileset", tmp);
	sprintdouble(tmp,map_area_position_get_center_lon(toolbar -> map_area),6);
	config_set_entry(toolbar -> config, "longitude", tmp);
	sprintdouble(tmp,map_area_position_get_center_lat(toolbar -> map_area),6);
	config_set_entry(toolbar -> config, "lattitude", tmp);
	sprintf(tmp,"%d",toolbar -> map_area -> map_position.zoom);
	config_set_entry(toolbar -> config, "zoom", tmp);
	config_save_config_file(toolbar -> config);
	return FALSE;
}

/****************************************************************************************************
* change mouse mode / react on change of mouse mode
****************************************************************************************************/
static gboolean button_mouse_mode_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	int mouse_mode = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "index"));
	gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	if (state){
		map_area_set_mouse_mode(toolbar -> map_area, mouse_mode);
	}
}
static gboolean toolbar_mouse_mode_cb(MapArea * map_area, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	MouseMode mouse_mode = map_area_get_mouse_mode(map_area);
	int m;
	for (m = 0; m < MAP_MODE_COUNT; m++){
		GtkToggleButton * button = GTK_TOGGLE_BUTTON(toolbar -> toolbar_buttons[m]);
		if (m == mouse_mode){
			if (!gtk_toggle_button_get_active(button)){
				gtk_toggle_button_set_active(button, TRUE);
			}
		}else{
			if (gtk_toggle_button_get_active(button)){
				gtk_toggle_button_set_active(button, FALSE);
			}
		}
	}
}

/****************************************************************************************************
* set network state / react on change of network state
****************************************************************************************************/
static gboolean button_network_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	map_area_set_network_state(toolbar -> map_area, !map_area_get_network_state(toolbar -> map_area));
	return FALSE;
}
static gboolean toolbar_network_state_cb(MapArea * map_area, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	GtkWidget * icon;
	if (map_area_get_network_state(map_area)){
		icon = gtk_image_new_from_file(GOSM_ICON_DIR "network-idle.png");
		gtk_widget_set_tooltip_text(toolbar -> button_network, "set offline");
	}else{
		icon = gtk_image_new_from_file(GOSM_ICON_DIR "network-offline.png");
		gtk_widget_set_tooltip_text(toolbar -> button_network, "set online");
	}
	gtk_button_set_image(GTK_BUTTON(toolbar -> button_network), icon);
}

/****************************************************************************************************
* zoom in / out
****************************************************************************************************/
static gboolean button_zoom_in_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	map_area_zoom_in(toolbar -> map_area);
}
static gboolean button_zoom_out_cb(GtkWidget *widget, gpointer toolbar_p)
{
	Toolbar * toolbar = GOSM_TOOLBAR(toolbar_p);
	map_area_zoom_out(toolbar -> map_area);
}
