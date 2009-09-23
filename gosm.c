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

/** TODO:
 *	There is a of work to do in general:
 *		- clean up!!!
 *		- take care of memory. use free() more often!
 *
 * 	Besides:
 *	- use makefile for building
 *	- PARTIALLY (only colors) update changed preferences without restart
 *	- DONE add color selection buttons in configuration window
 *	- DONE adjust paper size on exporting pdf via imagesize
 * 	- DONE let user set path to java-binary (for pdf export)
 *	- DONE make color of atlas-selection configurable
 * 	- DONE sum atlas picture up into a single pdf optionally
 * 	- DONE template selection for atlas export
 *	- DONE progress monitor for image export / atlas export
 *	- DONE make imageglue a class with events
 *	- DONE there are 18 zoom-levels!! in some parts 17 are assumed
 * 	- use tile_loader for loading tiles in map_area
 *	- DONE on CACHE_MISS: let another thread load into memory from disk for complete
 *		fluid visuals. 3-level-cache should be represented in threading
 *	- DONE make config-dialog:
 *	- DONE config-dialog should save settings on apply
 *	- PARTIALLY (learned about extern) config should be accessible from (nearly) everywhere somehow USE EXTERN
 * 	- DONE function 'load_new_tiles' is called on every position change
 * 		it handles checking for new tile-downloading quite inefficent
 * 		since it check existance of every single file on the tmp-dir
 * 		lookup in cache could be much more efficient???
 * 		:: compare time in disk-check/cache-check...
 * 	- DONE bind positioning to lat/lon instead of tile-coords in most parts
 * 	- free visual and colormap when removed from cache
 * 		therefore store them in struct in cache / try to get them from pixmap
 *	- DONE improve memory-caching algorithm
 *	- recognize downloading failures (empty files)
 *
 *	- DONE add selection mechanism
 *	- DONE add features to selection mechanism
 *	- DONE add sidebars
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <locale.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "uri.h"
#include <webkit/webkit.h>

#include "paths.h"
#include "config.h"
#include "config_widget.h"
#include "configuration.h"
#include "map_area.h"
#include "map_navigator.h"
#include "select_tool.h"
#include "distance_tool.h"
#include "atlas.h"
#include "atlas_tool.h"
#include "namefinder/namefinder_cities.h"
#include "wizzard/wizzard_download.h"
#include "wizzard/wizzard_export.h"
#include "wizzard/atlas_template_dialog.h"
#include "wizzard/wizzard_atlas_sequence.h"
#include "wizzard/wizzard_atlas_pdf.h"
#include "about/about.h"
#include "manual/manual.h"
#include "tilemath.h"
#include "tilesets.h"

#include "imageglue/imageglue.h"
#include "imageglue/pdf_generator.h"

#include <unistd.h>
#include <wait.h>

#define CURSOR_HAND		0
#define CURSOR_SELECT		1
#define CURSOR_TARGET		2

char * urls[TILESET_LAST] = {
        "http://b.tile.openstreetmap.org/%d/%d/%d.png",
        "http://a.tah.openstreetmap.org/Tiles/tile/%d/%d/%d.png",
	"http://a.andy.sandbox.cloudmade.com/tiles/cycle/%d/%d/%d.png"
};

char * cache_dirs[TILESET_LAST];

Configuration * config;
char * 		cache_dir;
char * 		cache_dir_mapnik;
char * 		cache_dir_osmarender;
char * 		cache_dir_cycle;
char * 		format_url;
gboolean 	network_state;
gboolean 	show_map_controls 	= TRUE;
gboolean 	show_side_pane 		= TRUE;
gboolean	show_side_pane_left	= TRUE;
gboolean 	show_status_bar 	= TRUE;
gboolean 	show_tool_bar 		= TRUE;
gboolean 	show_menu_bar 		= TRUE;
Tileset 	tileset 		= TILESET_MAPNIK;

GtkWidget * 	main_window;
MapArea * 	area;
MapNavigator * 	navigator;
GtkWidget * 	side;
GtkWidget *	side_left;
GtkWidget *	notebook_side_left;
GtkWidget * 	menubar;
GtkWidget * 	toolbar;
GtkWidget * 	statusbar;
char 		status_bar_buffer[50];

SelectTool * 	select_tool;
DistanceTool * 	distance_tool;
AtlasTool * 	atlas_tool;

GtkWidget * 	web_legend;
GtkWidget *	namefinder;

GtkWidget ** 	toolbar_buttons; // first 3 buttons; CURSOR_* is used as index
GtkWidget * 	button_network;
GtkWidget * 	button_map_controls;
GtkWidget * 	button_side_bar;
GtkWidget * 	button_side_bar_left;
GtkWidget * 	combo_tiles;

typedef struct WidgetPlusPointer{
	GtkWidget * widget;
	gpointer * pointer;
} WidgetPlusPointer;

gboolean	check_for_cache_directory(char * fn);
static gboolean close_cb(GtkWidget *widget);
void		area_set_cursor(int id);
static gboolean action_select_cb(GtkWidget *widget, gpointer cursor_id_p);
static gboolean button_font_cb(GtkWidget *widget);
static gboolean button_grid_cb(GtkWidget *widget);
static gboolean button_map_controls_cb(GtkWidget *widget);
       void 	toggle_side_bar();
       void 	toggle_menu_bar();
       void 	toggle_tool_bar();
       void 	toggle_status_bar();
       void 	toggle_map_controls();
       void 	toggle_side_bar();
       void 	toggle_side_bar_left();
static void 	button_savemaptype_cb();
static gboolean button_side_bar_cb(GtkWidget *widget);
static gboolean button_side_bar_left_cb(GtkWidget *widget);
static gboolean button_zoom_cb(GtkWidget *widget, gpointer direction);
static gboolean button_move_cb(GtkWidget *widget, gpointer direction);
static gboolean combo_tiles_cb(GtkWidget *widget);
static gboolean menubar_fullscreen_cb(GtkWidget *widget);
static gboolean window_event_cb(GtkWidget *window, GdkEventWindowState *event);
static gboolean map_area_map_cb(GtkWidget *widget, GdkEventConfigure *event);
static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);
static gboolean selection_use_cb(GtkWidget *widget);
static gboolean selection_export_cb(GtkWidget *widget);
static gboolean selection_clipboard_cb(GtkWidget *widget);
static gboolean selection_check_snap_cb(GtkWidget *widget);
static gboolean selection_check_visible_cb(GtkWidget *widget);
static gboolean atlas_values_changed_cb(GtkWidget *widget);
static gboolean atlas_template_cb(GtkWidget *widget);
static gboolean atlas_download_cb(GtkWidget *widget);
static gboolean atlas_export_cb(GtkWidget *widget);
static gboolean atlas_export_pdf_cb(GtkWidget *widget);
static gboolean distance_remove_last_cb(GtkWidget *widget);
static gboolean distance_clear_cb(GtkWidget *widget);
static gboolean map_moved_cb(GtkWidget *widget);
static gboolean map_zoom_cb(GtkWidget *widget);
static gboolean map_tileset_cb(GtkWidget *widget);
static void	set_legend(Tileset tileset, int zoom);
static gboolean map_selection_changed_cb(GtkWidget *widget);
static gboolean map_path_cb(GtkWidget *widget);
static gboolean show_manual_cb(GtkWidget *widget);
static gboolean show_about_cb(GtkWidget *widget, gpointer nump);
static void 	set_button_network();
static gboolean button_network_cb(GtkWidget *widget);
static gboolean preferences_confirm_cb(GtkWidget * widget, WidgetPlusPointer * wpp);
static gboolean preferences_cancel_cb(GtkWidget * widget, GtkWindow *window);
static gboolean show_preferences_cb(GtkWidget *widget);
static gboolean focus_redirect_cb(GtkWidget *widget, GdkEventButton *event);
static gboolean legend_click_cb(GtkWidget *widget, GdkEventButton *event);
static gboolean namefinder_city_cb(GtkWidget *widget);
static gboolean exit_cb(GtkWidget *widget);
void   chdir_to_bin(char * arg0);

/*
 * Start auto-generated menu
 * :r !./misc/menu_gen.py 1 misc/Menu.txt
 */
GtkWidget * menu_file_quit;
GtkWidget * menu_view_fullscreen;
GtkWidget * menu_control_zoom_in;
GtkWidget * menu_control_zoom_out;
GtkWidget * menu_control_move_up;
GtkWidget * menu_control_move_down;
GtkWidget * menu_control_move_left;
GtkWidget * menu_control_move_right;
GtkWidget * menu_tiles_mapnik;
GtkWidget * menu_tiles_osmarender;
GtkWidget * menu_tiles_openaerial;
GtkWidget * menu_tiles_google;
GtkWidget * menu_tiles_yahoo;
GtkWidget * menu_selection_snap;
GtkWidget * menu_selection_show;
GtkWidget * menu_selection_export;
GtkWidget * menu_selection_download;
GtkWidget * menu_options_preferences;
GtkWidget * menu_help_manual;
GtkWidget * menu_help_about_gosm;
GtkWidget * menu_help_about_osm;
GtkWidget * menu_help_license;
/*
 * End auto-generated menu
 */

int main(int argc, char *argv[])
{
	// ensure, that we are in the dir, where the executable is
	// i.e. set cwd to the executable's dir
	chdir_to_bin(argv[0]);

	// create config struct
	config = config_new();
	// fill config struct from config file
	config_load_config_file(config);

	// use the configured values
	gboolean	set_position		= *(gboolean*)		config_get_entry_data(config, "set_position");
	int		pos_x			= *(int*)		config_get_entry_data(config, "position_x");
	int		pos_y			= *(int*)		config_get_entry_data(config, "position_y");
	gboolean	set_size		= *(gboolean*)		config_get_entry_data(config, "set_size");
	int		width 			= *(int*)		config_get_entry_data(config, "size_width");
	int		height			= *(int*)		config_get_entry_data(config, "size_height");
	gboolean	show_grid		= *(gboolean*)		config_get_entry_data(config, "show_grid");
	gboolean	show_tilenumbers	= *(gboolean*)		config_get_entry_data(config, "show_tilenumbers");
	gboolean	show_menubar		= *(gboolean*)		config_get_entry_data(config, "show_menubar");
	gboolean	show_toolbar		= *(gboolean*)		config_get_entry_data(config, "show_toolbar");
	gboolean	show_statusbar		= *(gboolean*)		config_get_entry_data(config, "show_statusbar");
	gboolean	show_controls		= *(gboolean*)		config_get_entry_data(config, "show_controls");
	gboolean	show_sidebar		= *(gboolean*)		config_get_entry_data(config, "show_sidebar");
	gboolean	show_sidebar_left	= *(gboolean*)		config_get_entry_data(config, "show_left_sidebar");
	double		longitude		= *(double*)		config_get_entry_data(config, "longitude");
	double		lattitude		= *(double*)		config_get_entry_data(config, "lattitude");
	int		zoom			= *(int*)		config_get_entry_data(config, "zoom");
			network_state		= *(gboolean*)		config_get_entry_data(config, "online_on_startup");
	 		cache_dir		= 			config_get_entry_data(config, "cache_dir_mapnik");
	 		cache_dir_mapnik	= 			config_get_entry_data(config, "cache_dir_mapnik");
	 		cache_dir_osmarender	= 			config_get_entry_data(config, "cache_dir_osmarender");
	 		cache_dir_cycle	= 			config_get_entry_data(config, "cache_dir_cycle");
	ColorQuadriple	color_selection 	= *(ColorQuadriple*)	config_get_entry_data(config, "color_selection");
	ColorQuadriple	color_selection_out 	= *(ColorQuadriple*)	config_get_entry_data(config, "color_selection_out");
	ColorQuadriple	color_selection_pad 	= *(ColorQuadriple*)	config_get_entry_data(config, "color_selection_pad");
	ColorQuadriple	color_atlas_lines	= *(ColorQuadriple*)	config_get_entry_data(config, "color_atlas_lines");
	tileset	= *(Tileset*)	config_get_entry_data(config, "tileset");

	if(!set_size){
		width 	= 900;
		height	= 600;
	}

	printf("tileset: %i\n", tileset);
	format_url = urls[tileset];
	cache_dirs[TILESET_MAPNIK] = cache_dir_mapnik;
	cache_dirs[TILESET_OSMARENDER] = cache_dir_osmarender;
	cache_dirs[TILESET_CYCLE] = cache_dir_cycle;

	// ensure, that the tmp-directory for tiles exists
	if (!check_for_cache_directory(cache_dir_mapnik)
		|| !check_for_cache_directory(cache_dir_osmarender) 
		|| !check_for_cache_directory(cache_dir_cycle) ){
		return EXIT_FAILURE;
	}

	// init g_threads, gtk_threads, gtk
	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	/**
	 * GTK WIDGETS AND LAYOUT, CALLBACKS
	 */
	// main window
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "GOsmView");
	gtk_window_set_icon_from_file(GTK_WINDOW(main_window), GOSM_ICON_DIR "gosm.png", NULL);
	gtk_window_set_default_size(GTK_WINDOW(main_window), width, height);
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(close_cb), NULL);

	// widget, that displays the map
	area = GOSM_MAP_AREA(map_area_new());
	// container for the area, showing the controls around it
	navigator = GOSM_MAP_NAVIGATOR(map_navigator_new(GTK_WIDGET(area)));

	// apply config to map_area
	area -> map_position.lon = longitude;
	area -> map_position.lat = lattitude;
	area -> map_position.zoom = zoom;
	//printf("network state: %d\n", network_state);
	map_area_set_network_state(area, network_state);
	map_area_set_cache_directory(area, TILESET_MAPNIK,	cache_dir_mapnik);
	map_area_set_cache_directory(area, TILESET_OSMARENDER,	cache_dir_osmarender);
	map_area_set_cache_directory(area, TILESET_CYCLE,	cache_dir_cycle);
	map_area_set_color_selection(area, color_selection, color_selection_out, color_selection_pad, color_atlas_lines);

	// Selection-Widget in sidebar
	select_tool = select_tool_new();
	GtkWidget *frame_select_tool= gtk_frame_new("Selection");
	gtk_container_add(GTK_CONTAINER(frame_select_tool), GTK_WIDGET(select_tool));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_tool -> check_snap), area -> snap_selection);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_tool -> check_show), area -> show_selection);
	g_signal_connect(G_OBJECT(select_tool -> button_action), "clicked", G_CALLBACK(selection_use_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> button_export), "clicked", G_CALLBACK(selection_export_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> button_clipboard), "clicked", G_CALLBACK(selection_clipboard_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> check_snap), "toggled", G_CALLBACK(selection_check_snap_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> check_show), "toggled", G_CALLBACK(selection_check_visible_cb), NULL);

	// Atlas-Widget in sidebar
	atlas_tool = atlas_tool_new();
	GtkWidget *frame_atlas_tool= gtk_frame_new("Atlas");
	gtk_container_add(GTK_CONTAINER(frame_atlas_tool), GTK_WIDGET(atlas_tool));
	PageInformation page_info = {
		PAPERSIZE_A4,
		ORIENTATION_PORTRAIT,
		210, 297, 20, 20, 15, 15, 150
	};
	atlas_tool_set_values(atlas_tool, area -> slice_zl, area -> show_slice);
	atlas_tool_set_page_info(atlas_tool, page_info);
	atlas_tool_set_intersection(atlas_tool, 150, 150);
	g_signal_connect(G_OBJECT(atlas_tool), "values-changed", G_CALLBACK(atlas_values_changed_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_conf_page), "clicked", G_CALLBACK(atlas_template_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_action), "clicked", G_CALLBACK(atlas_download_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_export), "clicked", G_CALLBACK(atlas_export_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_export_pdf), "clicked", G_CALLBACK(atlas_export_pdf_cb), NULL);

	// Distance-Widget in sidebar
	distance_tool = GOSM_DISTANCE_TOOL(distance_tool_new());
	GtkWidget *frame_distance_tool= gtk_frame_new("Distance");
	gtk_container_add(GTK_CONTAINER(frame_distance_tool), GTK_WIDGET(distance_tool));
	g_signal_connect(G_OBJECT(distance_tool -> button_remove_last), "clicked", G_CALLBACK(distance_remove_last_cb), NULL);
	g_signal_connect(G_OBJECT(distance_tool -> button_clear), "clicked", G_CALLBACK(distance_clear_cb), NULL);

	menubar = gtk_menu_bar_new();

	/*
	 * Start auto-generated menu
	 * :r !./misc/menu_gen.py 2 misc/Menu.txt
	 */
	
	GtkWidget *item_1                      = gtk_menu_item_new_with_label("File");
	GtkWidget *menu_1                      = gtk_menu_new();
	GtkWidget *item_1_1                    = gtk_menu_item_new_with_label("Quit");
	GtkWidget *item_2                      = gtk_menu_item_new_with_label("View");
	GtkWidget *menu_2                      = gtk_menu_new();
	GtkWidget *item_2_1                    = gtk_check_menu_item_new_with_label("Fullscreen");
	GtkWidget *item_2_2                    = gtk_menu_item_new_with_label("Control");
	GtkWidget *menu_2_2                    = gtk_menu_new();
	GtkWidget *item_2_2_1                  = gtk_menu_item_new_with_label("Zoom In");
	GtkWidget *item_2_2_2                  = gtk_menu_item_new_with_label("Zoom Out");
	GtkWidget *item_2_2_3                  = gtk_menu_item_new_with_label("Move Up");
	GtkWidget *item_2_2_4                  = gtk_menu_item_new_with_label("Move Down");
	GtkWidget *item_2_2_5                  = gtk_menu_item_new_with_label("Move Left");
	GtkWidget *item_2_2_6                  = gtk_menu_item_new_with_label("Move Right");
	GtkWidget *item_2_3                    = gtk_menu_item_new_with_label("Tiles");
	GtkWidget *menu_2_3                    = gtk_menu_new();
	GtkWidget *item_2_3_1                  = gtk_radio_menu_item_new_with_label(NULL, "Mapnik");
	GtkWidget *item_2_3_2                  = gtk_radio_menu_item_new_with_label(gtk_radio_menu_item_get_group((GtkRadioMenuItem*)item_2_3_1), "Osmarender");
	GtkWidget *item_2_3_3                  = gtk_radio_menu_item_new_with_label(gtk_radio_menu_item_get_group((GtkRadioMenuItem*)item_2_3_1), "OpenAerial");
	GtkWidget *item_2_3_4                  = gtk_radio_menu_item_new_with_label(gtk_radio_menu_item_get_group((GtkRadioMenuItem*)item_2_3_1), "Google");
	GtkWidget *item_2_3_5                  = gtk_radio_menu_item_new_with_label(gtk_radio_menu_item_get_group((GtkRadioMenuItem*)item_2_3_1), "Yahoo");
	GtkWidget *item_3                      = gtk_menu_item_new_with_label("Selection");
	GtkWidget *menu_3                      = gtk_menu_new();
	GtkWidget *item_3_1                    = gtk_check_menu_item_new_with_label("Snap to Map");
	GtkWidget *item_3_2                    = gtk_check_menu_item_new_with_label("Hide");
	GtkWidget *item_3_3                    = gtk_menu_item_new_with_label("Export");
	GtkWidget *item_3_4                    = gtk_menu_item_new_with_label("Download");
	GtkWidget *item_4                      = gtk_menu_item_new_with_label("Options");
	GtkWidget *menu_4                      = gtk_menu_new();
	GtkWidget *item_4_1                    = gtk_menu_item_new_with_label("Preferences");
	GtkWidget *item_5                      = gtk_menu_item_new_with_label("Help");
	GtkWidget *menu_5                      = gtk_menu_new();
	GtkWidget *item_5_1                    = gtk_menu_item_new_with_label("Manual");
	GtkWidget *item_5_2                    = gtk_menu_item_new_with_label("About GOsmView");
	GtkWidget *item_5_3                    = gtk_menu_item_new_with_label("About OpenStreetMap");
	GtkWidget *item_5_4                    = gtk_menu_item_new_with_label("License");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_1),              menu_1);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_2),              menu_2);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_2_2),            menu_2_2);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_2_3),            menu_2_3);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_3),              menu_3);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_4),              menu_4);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_5),              menu_5);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),            item_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_1),             item_1_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),            item_2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2),             item_2_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2),             item_2_2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_2),           item_2_2_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_2),           item_2_2_2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_2),           item_2_2_3);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_2),           item_2_2_4);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_2),           item_2_2_5);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_2),           item_2_2_6);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2),             item_2_3);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_3),           item_2_3_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_3),           item_2_3_2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_3),           item_2_3_3);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_3),           item_2_3_4);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_2_3),           item_2_3_5);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),            item_3);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_3),             item_3_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_3),             item_3_2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_3),             item_3_3);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_3),             item_3_4);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),            item_4);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_4),             item_4_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),            item_5);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_5),             item_5_1);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_5),             item_5_2);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_5),             item_5_3);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_5),             item_5_4);
	menu_file_quit                 = item_1_1;
	menu_view_fullscreen           = item_2_1;
	menu_control_zoom_in           = item_2_2_1;
	menu_control_zoom_out          = item_2_2_2;
	menu_control_move_up           = item_2_2_3;
	menu_control_move_down         = item_2_2_4;
	menu_control_move_left         = item_2_2_5;
	menu_control_move_right        = item_2_2_6;
	menu_tiles_mapnik              = item_2_3_1;
	menu_tiles_osmarender          = item_2_3_2;
	menu_tiles_openaerial          = item_2_3_3;
	menu_tiles_google              = item_2_3_4;
	menu_tiles_yahoo               = item_2_3_5;
	menu_selection_snap            = item_3_1;
	menu_selection_show            = item_3_2;
	menu_selection_export          = item_3_3;
	menu_selection_download        = item_3_4;
	menu_options_preferences       = item_4_1;
	menu_help_manual               = item_5_1;
	menu_help_about_gosm           = item_5_2;
	menu_help_about_osm            = item_5_3;
	menu_help_license              = item_5_4;
	/*
	 * End auto-generated menu
	 */

	gtk_widget_set_sensitive(menu_tiles_osmarender, FALSE);
	gtk_widget_set_sensitive(menu_tiles_openaerial, FALSE);
	gtk_widget_set_sensitive(menu_tiles_google, FALSE);
	gtk_widget_set_sensitive(menu_tiles_yahoo, FALSE);

	g_signal_connect(G_OBJECT(menu_file_quit), 		"activate", G_CALLBACK(exit_cb), NULL);
	g_signal_connect(G_OBJECT(menu_options_preferences), 	"activate", G_CALLBACK(show_preferences_cb), NULL);
	g_signal_connect(G_OBJECT(menu_help_manual),	 	"activate", G_CALLBACK(show_manual_cb), NULL);
	g_signal_connect(G_OBJECT(menu_help_about_gosm), 	"activate", G_CALLBACK(show_about_cb), GINT_TO_POINTER(0));
	g_signal_connect(G_OBJECT(menu_help_about_osm), 	"activate", G_CALLBACK(show_about_cb), GINT_TO_POINTER(1));
	g_signal_connect(G_OBJECT(menu_help_license),	 	"activate", G_CALLBACK(show_about_cb), GINT_TO_POINTER(2));
	g_signal_connect(G_OBJECT(menu_control_zoom_in), 	"activate", G_CALLBACK(button_zoom_cb), GINT_TO_POINTER(0));
	g_signal_connect(G_OBJECT(menu_control_zoom_out), 	"activate", G_CALLBACK(button_zoom_cb), GINT_TO_POINTER(1));
	g_signal_connect(G_OBJECT(menu_control_move_up), 	"activate", G_CALLBACK(button_move_cb), GINT_TO_POINTER(2));
	g_signal_connect(G_OBJECT(menu_control_move_down), 	"activate", G_CALLBACK(button_move_cb), GINT_TO_POINTER(7));
	g_signal_connect(G_OBJECT(menu_control_move_left), 	"activate", G_CALLBACK(button_move_cb), GINT_TO_POINTER(4));
	g_signal_connect(G_OBJECT(menu_control_move_right), 	"activate", G_CALLBACK(button_move_cb), GINT_TO_POINTER(5));

	/**
	 * Toolbar
	 */

	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	GtkWidget * icon1 = gtk_image_new_from_file(GOSM_ICON_DIR "navigate.png");
	GtkWidget * icon2 = gtk_image_new_from_file(GOSM_ICON_DIR "select.png");
	GtkWidget * icon3 = gtk_image_new_from_file(GOSM_ICON_DIR "measure.png");
	toolbar_buttons = malloc(3 * sizeof(GtkButton*));
	toolbar_buttons[0] = gtk_toggle_button_new();
	toolbar_buttons[1] = gtk_toggle_button_new();
	toolbar_buttons[2] = gtk_toggle_button_new();
	gtk_button_set_image(GTK_BUTTON(toolbar_buttons[0]), icon1);
	gtk_button_set_image(GTK_BUTTON(toolbar_buttons[1]), icon2);
	gtk_button_set_image(GTK_BUTTON(toolbar_buttons[2]), icon3);
	
	GtkWidget * separator1 = gtk_hseparator_new();

	GtkWidget * icon4 = gtk_image_new_from_file(GOSM_ICON_DIR "grid.png");
	GtkWidget * icon5 = gtk_image_new_from_stock("gtk-zoom-in", GTK_ICON_SIZE_BUTTON);
	GtkWidget * icon6 = gtk_image_new_from_stock("gtk-zoom-out", GTK_ICON_SIZE_BUTTON);
	GtkWidget * icon7 = gtk_image_new_from_file(GOSM_ICON_DIR "font.png");
	GtkWidget * icon8 = gtk_image_new_from_file(GOSM_ICON_DIR "stock_form-navigator.png");
	GtkWidget * icon9 = gtk_image_new_from_file(GOSM_ICON_DIR "stock_show-hidden-controls.png");
	GtkWidget * icon10 = gtk_image_new_from_file(GOSM_ICON_DIR "cross.png");
	GtkWidget * icon11 = gtk_image_new_from_file(GOSM_ICON_DIR "stock_show-hidden-controls2.png");
	GtkWidget * button_grid = gtk_toggle_button_new();
	GtkWidget * button_zoom_in = gtk_button_new();
	GtkWidget * button_zoom_out = gtk_button_new();
	GtkWidget * button_font = gtk_toggle_button_new();
	GtkWidget * button_savemaptype = gtk_button_new();
		    button_map_controls = gtk_toggle_button_new();
		    button_side_bar = gtk_toggle_button_new();
		    button_side_bar_left = gtk_toggle_button_new();
	gtk_button_set_image(GTK_BUTTON(button_grid), icon4);
	gtk_button_set_image(GTK_BUTTON(button_zoom_in), icon5);
	gtk_button_set_image(GTK_BUTTON(button_zoom_out), icon6);
	gtk_button_set_image(GTK_BUTTON(button_font), icon7);
	gtk_button_set_image(GTK_BUTTON(button_map_controls), icon8);
	gtk_button_set_image(GTK_BUTTON(button_side_bar), icon9);
	gtk_button_set_image(GTK_BUTTON(button_side_bar_left), icon11);
	gtk_button_set_image(GTK_BUTTON(button_savemaptype), icon10);

	button_network = gtk_button_new();
	set_button_network();

	combo_tiles = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_tiles), "Mapnik");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_tiles), "Osmarender");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_tiles), "Cycle");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(combo_tiles), "OpenAerial");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_tiles), tileset);
	combo_tiles_cb(combo_tiles);

	int tc = 0;
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_network, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar_buttons[0], NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar_buttons[1], NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), toolbar_buttons[2], NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_zoom_in, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_zoom_out, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_grid, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_font, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_side_bar_left, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_map_controls, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_side_bar, NULL, NULL, tc++);
	gtk_toolbar_insert_space( GTK_TOOLBAR(toolbar), tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), combo_tiles, NULL, NULL, tc++);
	gtk_toolbar_insert_widget(GTK_TOOLBAR(toolbar), button_savemaptype, NULL, NULL, tc++);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_map_controls), show_map_controls);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_side_bar), show_side_pane);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_side_bar_left), show_side_pane_left);

	gtk_widget_set_tooltip_text(toolbar_buttons[0],		"Navigation mode");
	gtk_widget_set_tooltip_text(toolbar_buttons[1],		"Selection mode");
	gtk_widget_set_tooltip_text(toolbar_buttons[2],		"Measure mode");
	gtk_widget_set_tooltip_text(button_zoom_in,		"zoom in");
	gtk_widget_set_tooltip_text(button_zoom_out,		"zoom out");
	gtk_widget_set_tooltip_text(button_grid,		"grid");
	gtk_widget_set_tooltip_text(button_font,		"tilenames");
	gtk_widget_set_tooltip_text(button_side_bar_left,	"legend");
	gtk_widget_set_tooltip_text(button_map_controls,	"navigation buttons");
	gtk_widget_set_tooltip_text(button_side_bar,		"sidebar");
	gtk_widget_set_tooltip_text(button_savemaptype,		"Save map and position");

	g_signal_connect(G_OBJECT(toolbar_buttons[0]), 	"toggled", G_CALLBACK(action_select_cb), GINT_TO_POINTER(CURSOR_HAND));
	g_signal_connect(G_OBJECT(toolbar_buttons[1]), 	"toggled", G_CALLBACK(action_select_cb), GINT_TO_POINTER(CURSOR_SELECT));
	g_signal_connect(G_OBJECT(toolbar_buttons[2]), 	"toggled", G_CALLBACK(action_select_cb), GINT_TO_POINTER(CURSOR_TARGET));
	g_signal_connect(G_OBJECT(button_grid), 	"toggled", G_CALLBACK(button_grid_cb), NULL);
	g_signal_connect(G_OBJECT(button_font), 	"toggled", G_CALLBACK(button_font_cb), NULL);
	g_signal_connect(G_OBJECT(button_zoom_in), 	"clicked", G_CALLBACK(button_zoom_cb), GINT_TO_POINTER(0));
	g_signal_connect(G_OBJECT(button_zoom_out), 	"clicked", G_CALLBACK(button_zoom_cb), GINT_TO_POINTER(1));
	g_signal_connect(G_OBJECT(button_side_bar_left),"toggled", G_CALLBACK(button_side_bar_left_cb), NULL);
	g_signal_connect(G_OBJECT(button_map_controls),	"clicked", G_CALLBACK(button_map_controls_cb), NULL);
	g_signal_connect(G_OBJECT(button_side_bar),	"toggled", G_CALLBACK(button_side_bar_cb), NULL);
	g_signal_connect(G_OBJECT(button_network), 	"clicked", G_CALLBACK(button_network_cb), NULL);
	g_signal_connect(G_OBJECT(button_savemaptype), 	"clicked", G_CALLBACK(button_savemaptype_cb), NULL);
	g_signal_connect(G_OBJECT(menu_view_fullscreen),"toggled", G_CALLBACK(menubar_fullscreen_cb), NULL);
	g_signal_connect(G_OBJECT(main_window),		"window-state-event", G_CALLBACK(window_event_cb), NULL);
	g_signal_connect(G_OBJECT(combo_tiles), 	"changed", G_CALLBACK(combo_tiles_cb), NULL);

	/**
	 * Statusbar
	 */

	statusbar = gtk_statusbar_new();

	/**
	 * Sidebar on the right
	 * (tools)
	 */

	side = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_size_request(side, 180, 0);
	gtk_box_pack_start(GTK_BOX(side), frame_select_tool, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(side), frame_atlas_tool, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(side), frame_distance_tool, FALSE, FALSE, 0);

	/**
	 * Sidebar on the left
	 * (legend)
	 */

	side_left = gtk_vbox_new(FALSE, 0);
	notebook_side_left = gtk_notebook_new();
	// legend
	web_legend = webkit_web_view_new();
	set_legend(
		map_area_get_tileset(area),
		map_area_get_zoom(area)
	);
        GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(scrolled), web_legend);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
        gtk_widget_set_size_request(scrolled, 180, -1);
	gtk_box_pack_start(GTK_BOX(side_left), notebook_side_left, TRUE, TRUE, 0);
	// this is a dummy to prevent the context-menu to appear
	g_signal_connect(G_OBJECT(web_legend), 	"button-press-event", G_CALLBACK(legend_click_cb), NULL);
	// bookmarks
	GtkWidget * placeholder_bookmarks = gtk_vbox_new(FALSE, 0);
	// namefinder
	namefinder = namefinder_cities_new();
	g_signal_connect(G_OBJECT(namefinder), "city-activated", G_CALLBACK(namefinder_city_cb), NULL);
	// --
	GtkWidget * image_legend = gtk_image_new_from_file(GOSM_ICON_DIR "stock_chart-toggle-legend.png");
	GtkWidget * image_bookmarks = gtk_image_new_from_file(GOSM_ICON_DIR "stock_bookmark.png");
	GtkWidget * image_namefinder = gtk_image_new_from_file(GOSM_ICON_DIR "stock_internet.png");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_side_left), scrolled, image_legend);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_side_left), placeholder_bookmarks, image_bookmarks);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_side_left), namefinder, image_namefinder);

	/**
	 * Other widgets
	 */

	GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), side_left, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(navigator), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), side, FALSE, FALSE, 0);

	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(area), "map", G_CALLBACK(map_area_map_cb), NULL);
	g_signal_connect(G_OBJECT(navigator), "key_press_event", G_CALLBACK(key_press_cb), NULL);
	g_signal_connect(G_OBJECT(area), "map-been-moved", G_CALLBACK(map_moved_cb), NULL);
	g_signal_connect(G_OBJECT(area), "map-zoom-changed", G_CALLBACK(map_zoom_cb), NULL);
	g_signal_connect(G_OBJECT(area), "map-tileset-changed", G_CALLBACK(map_tileset_cb), NULL);
	g_signal_connect(G_OBJECT(area), "map-selection-changed", G_CALLBACK(map_selection_changed_cb), NULL);
	g_signal_connect(G_OBJECT(area), "map-path-changed", G_CALLBACK(map_path_cb), NULL);

	gtk_container_add(GTK_CONTAINER(main_window), vbox);
	gtk_widget_show_all(main_window);
	gtk_widget_grab_focus(GTK_WIDGET(area));

	g_signal_connect(G_OBJECT(button_network), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(button_zoom_in), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(button_zoom_out), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(button_grid), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(button_font), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(button_map_controls), "button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(button_side_bar), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(button_side_bar_left),"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(toolbar_buttons[0]), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(toolbar_buttons[1]), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);
	g_signal_connect(G_OBJECT(toolbar_buttons[2]), 	"button-release-event", G_CALLBACK(focus_redirect_cb), NULL);

	// config
	if (!show_controls) toggle_map_controls();
	if (show_grid) area -> show_grid = TRUE;;
	if (show_tilenumbers) area -> show_font = TRUE;
	if (!show_menubar) toggle_menu_bar();
	if (!show_toolbar) toggle_tool_bar();
	if (!show_statusbar) toggle_status_bar();
	if (!show_sidebar) toggle_side_bar();
	if (!show_sidebar_left) toggle_side_bar_left();

	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();
}

void apply_new_config()
{
	ColorQuadriple	color_selection 	= *(ColorQuadriple*)config_get_entry_data(config, "color_selection");
	ColorQuadriple	color_selection_out 	= *(ColorQuadriple*)config_get_entry_data(config, "color_selection_out");
	ColorQuadriple	color_selection_pad 	= *(ColorQuadriple*)config_get_entry_data(config, "color_selection_pad");
	ColorQuadriple	color_atlas_lines	= *(ColorQuadriple*)config_get_entry_data(config, "color_atlas_lines");
	map_area_set_color_selection(area, color_selection, color_selection_out, color_selection_pad, color_atlas_lines);
	map_area_repaint(area);
}

/**
 * check if directory 'fn' exists
 * if not, try to create it, including parent directories
 * returns:	TRUE  => directory exists
 *		FALSE => directory does not exist and could not be created
 */
gboolean check_for_cache_directory(char * fn)
{
        struct stat info;
        int r_stat = stat(fn, &info);
        if (r_stat){
                printf("creating directory: %s\n", fn);
                int r_mkdir = g_mkdir_with_parents(fn, 0777);
        }
        r_stat = stat(fn, &info);
        if (r_stat || !S_ISDIR(info.st_mode)){
                printf("could not create chache directory: %s\n", fn);
		return FALSE;
        }
	return TRUE;
}

// called when close button is hit
static gboolean close_cb(GtkWidget *widget)
{
	printf("main window has been closed\n");
	gtk_main_quit();
}

// set cursor to CURSOR_*
void area_set_cursor(int id)
{
	GdkCursor * cursor = gdk_cursor_new(GDK_HAND1);
	switch(id){
		case CURSOR_SELECT: cursor = gdk_cursor_new(GDK_CROSS); break;
		case CURSOR_TARGET: cursor = gdk_cursor_new(GDK_TARGET); break;
	}
	gdk_window_set_cursor(GTK_WIDGET(area)->window, cursor);
}

// toolbar: mode select
static gboolean action_select_cb(GtkWidget *widget, gpointer cursor_id_p)
{
	int cursor_id = GPOINTER_TO_INT(cursor_id_p);
	gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	if (state){
		if (area -> action_state != cursor_id){
			int ex = area -> action_state;
			area -> action_state = cursor_id;
			if (ex >= 0){
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar_buttons[ex]), FALSE);
			}
			area_set_cursor(cursor_id);
		}
	}else{
		if (area -> action_state == cursor_id){
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar_buttons[cursor_id]), TRUE);
		}
	}
}

// toolbar: show fonts
static gboolean button_font_cb(GtkWidget *widget)
{
	area -> show_font = !area -> show_font;
	map_area_repaint(GOSM_MAP_AREA(area));
}

// toolbar: show grid
static gboolean button_grid_cb(GtkWidget *widget)
{
	area -> show_grid = !area -> show_grid;
	map_area_repaint(GOSM_MAP_AREA(area));
}

void toggle_menu_bar()
{
	show_menu_bar = !show_menu_bar;
	if (show_menu_bar) {gtk_widget_show(menubar);} else {gtk_widget_hide(menubar);}
}

void toggle_tool_bar()
{
	show_tool_bar = !show_tool_bar;
	if (show_tool_bar) {gtk_widget_show(toolbar);} else {gtk_widget_hide(toolbar);}
}

void toggle_status_bar()
{
	show_status_bar = !show_status_bar;
	if (show_status_bar) {gtk_widget_show(statusbar);} else {gtk_widget_hide(statusbar);}
}

void toggle_map_controls()
{
	show_map_controls = !show_map_controls;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_map_controls), show_map_controls);
	map_navigator_show_controls(navigator, show_map_controls);
}

void toggle_side_bar()
{
	show_side_pane = !show_side_pane;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_side_bar), show_side_pane);
	if (show_side_pane){
		gtk_widget_show(side);
	}else{
		gtk_widget_hide(side);
	}
}

void toggle_side_bar_left()
{
	show_side_pane_left = !show_side_pane_left;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button_side_bar_left), show_side_pane_left);
	if (show_side_pane_left){
		gtk_widget_show(side_left);
	}else{
		gtk_widget_hide(side_left);
	}
}

// toolbar: show map_controls
static gboolean button_map_controls_cb(GtkWidget *widget)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_map_controls)) != show_map_controls)
		toggle_map_controls();
}

// toolbar: show side_bar
static gboolean button_side_bar_cb(GtkWidget *widget)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_side_bar)) != show_side_pane)
		toggle_side_bar();
}

// toolbar: show side_bar
static gboolean button_side_bar_left_cb(GtkWidget *widget)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_side_bar_left)) != show_side_pane_left)
		toggle_side_bar_left();
}

// toolbar: zoom in/out
static gboolean button_zoom_cb(GtkWidget *widget, gpointer direction)
{
	int dir = GPOINTER_TO_INT(direction);
	if (dir == 0){
		map_area_zoom_in(area);
	}
	if (dir == 1){
		map_area_zoom_out(area);
	}
}

// menu: move map
static gboolean button_move_cb(GtkWidget *widget, gpointer direction)
{
	int dir = GPOINTER_TO_INT(direction);
	map_area_move(area, dir);
}

// toolbar: tile selection
static gboolean combo_tiles_cb(GtkWidget *widget)
{
	gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
	map_area_set_tileset(area, active);
	format_url = urls[active];
	cache_dir = cache_dirs[active];
	tileset = active;
	printf("tileset: %i\n", active);
}

// menubar: fullscreen
static gboolean menubar_fullscreen_cb(GtkWidget *widget)
{
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_view_fullscreen))){
			gdk_window_fullscreen(main_window->window);
	}else{
			gdk_window_unfullscreen(main_window->window);
	}
}

// window events of main window
static gboolean window_event_cb(GtkWidget *window, GdkEventWindowState *event)
{
	if ((event -> changed_mask & GDK_WINDOW_STATE_FULLSCREEN) != 0){
		gboolean fullscreened = (gdk_window_get_state(main_window -> window) & GDK_WINDOW_STATE_FULLSCREEN) != 0;
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_view_fullscreen), fullscreened);
	}
}

// map area appeared
static gboolean map_area_map_cb(GtkWidget *widget, GdkEventConfigure *event)
{
	printf("map\n");
	if (area -> action_state == -1){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar_buttons[0]), TRUE);
		action_select_cb(toolbar_buttons[0], GINT_TO_POINTER(CURSOR_HAND));
	}
}

// key press
static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event)
{
	//65470 - 65478 = F1 - F9
	//printf("signal %d\n", event->keyval);
	switch (event -> keyval){
	case 65473:{ // F4 - toggle left sidebar
		toggle_side_bar_left();
		break;
	}
	case 65474:{ // F5 - toggle controls
		toggle_map_controls();
		break;
	}
	case 65475:{ // F6 - toggle sidebar
		toggle_side_bar();
		break;
	}
	case 65476:{ // F7 - toggle statusbar
		toggle_status_bar();
		break;
	}
	case 65477:{ // F8 - toggle toolbar
		toggle_tool_bar();
		break;
	}
	case 65478:{ // F9 - toggle menubar
		toggle_menu_bar();
		break;
	}
	case 65480:{ // F11 - toggle fullscreen
		gboolean is_fullscreen = (gdk_window_get_state(main_window -> window) & GDK_WINDOW_STATE_FULLSCREEN) != 0;
		if (is_fullscreen){
			gdk_window_unfullscreen(main_window->window);
		}else{
			gdk_window_fullscreen(main_window->window);
		}
		break;
	}
	/* mode select */
	case 97:{ // a
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar_buttons[0]), TRUE);
		break;
	}
	case 115:{ // s
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar_buttons[1]), TRUE);
		break;
	}
	case 100:{ // d
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toolbar_buttons[2]), TRUE);
		break;
	}
	/* mode select */
	}
	return TRUE; 
}

static gboolean selection_use_cb(GtkWidget *widget)
{
	printf("use...\n");
	WizzardDownload * wizzard = wizzard_download_new(GTK_WINDOW(main_window), format_url, cache_dir, area->selection);
	wizzard_download_show(wizzard);
}

static gboolean selection_export_cb(GtkWidget *widget)
{
	printf("export...\n");
	WizzardExport * wizzard = wizzard_export_new(GTK_WINDOW(main_window), cache_dir, area -> selection, area -> map_position.zoom);
	wizzard_export_show(wizzard);
}

static gboolean selection_clipboard_cb(GtkWidget *widget)
{
	Selection s = area -> selection;
	printf("Selection (lon1, lon2, lat1, lat2): %f %f %f %f\n", s.lon1, s.lon2, s.lat1, s.lat2);

	double lon1, lon2, lat1, lat2;
	lon1 = s.lon1 < s.lon2 ? s.lon1 : s.lon2;
	lon2 = s.lon1 > s.lon2 ? s.lon1 : s.lon2;
	lat1 = s.lat1 < s.lat2 ? s.lat1 : s.lat2;
	lat2 = s.lat1 > s.lat2 ? s.lat1 : s.lat2;

	char buf[40];
	sprintdouble(buf, lon1, 7);
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), lat1, 7); 
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), lon2, 7); 
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), lat2, 7); 

	GtkClipboard * cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(cb, buf, strlen(buf));

}

static gboolean selection_check_snap_cb(GtkWidget *widget)
{
	area -> snap_selection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(select_tool -> check_snap));
}

static gboolean selection_check_visible_cb(GtkWidget *widget)
{
	area -> show_selection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(select_tool -> check_show));
	gtk_widget_queue_draw(GTK_WIDGET(area));
}

static gboolean atlas_values_changed_cb(GtkWidget *widget)
{
	printf("vals changed\n");
	area -> slice_zl = atlas_tool -> slice_zoom;
	area -> show_slice = atlas_tool -> visible;
	area -> slice_x = atlas_tool -> image_dimension.width;
	area -> slice_y = atlas_tool -> image_dimension.height;;
	area -> slice_intersect_x = atlas_tool -> intersect_x;
	area -> slice_intersect_y = atlas_tool -> intersect_y;
	map_area_repaint(GOSM_MAP_AREA(area));
}

static gboolean atlas_template_cb(GtkWidget *widget)
{
	GtkDialog * d = atlas_template_dialog_new(atlas_tool -> page_info);
	gtk_window_set_transient_for(GTK_WINDOW(d), GTK_WINDOW(main_window));
	gtk_window_set_title(GTK_WINDOW(d), "Set Papersize");
	gtk_window_set_position(GTK_WINDOW(d), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal(GTK_WINDOW(d), TRUE);
	int response = gtk_dialog_run(d);
	//printf("%d\n", response);
	gboolean apply = FALSE;
	PageInformation page_info;
	if(response == 0){ //TODO: use standard gtk-response-ids
		apply = TRUE;
		page_info = atlas_template_dialog_get_page_info(GOSM_ATLAS_TEMPLATE_DIALOG(d));
	}
	gtk_widget_destroy(GTK_WIDGET(d));
	if(apply){
		atlas_tool_set_page_info(atlas_tool, page_info);
	}
}

static gboolean atlas_download_cb(GtkWidget *widget)
{
	WizzardDownload * wizzard = wizzard_download_new(GTK_WINDOW(main_window), format_url, cache_dir, area->selection);
	wizzard_download_show(wizzard);
	wizzard_download_set_active(wizzard, GPOINTER_TO_INT(area->slice_zl), TRUE);
}

static gboolean atlas_export_cb(GtkWidget *widget)
{
	printf("exp\n");
	WizzardAtlasSequence * was = wizzard_atlas_sequence_new(cache_dir, atlas_tool -> slice_zoom, area -> selection, atlas_tool -> image_dimension,
								atlas_tool -> intersect_x, atlas_tool -> intersect_y);
	wizzard_atlas_sequence_show(was, GTK_WINDOW(main_window));
}

static gboolean atlas_export_pdf_cb(GtkWidget *widget)
{
	printf("exp\n");
	WizzardAtlasPdf * wap = wizzard_atlas_pdf_new(cache_dir, atlas_tool -> slice_zoom, area -> selection, atlas_tool -> page_info, 
							atlas_tool -> image_dimension, atlas_tool -> intersect_x, atlas_tool -> intersect_y);
	wizzard_atlas_pdf_show(wap, GTK_WINDOW(main_window));
}

static gboolean distance_remove_last_cb(GtkWidget *widget)
{
	map_area_path_remove_point(area);
}

static gboolean distance_clear_cb(GtkWidget *widget)
{
	map_area_path_clear(area);
}

static gboolean map_moved_cb(GtkWidget *widget)
{
	sprintf(status_bar_buffer, "lon: %f lat: %f zoom: %d",
		map_area_position_get_center_lon(area),
		map_area_position_get_center_lat(area),
		area -> map_position.zoom);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, status_bar_buffer);
	select_tool_set_lons_n_lats(select_tool,
		area -> selection.lon1,
		area -> selection.lon2,
		area -> selection.lat1,
		area -> selection.lat2);
}

static gboolean map_zoom_cb(GtkWidget *widget)
{
	set_legend(
		map_area_get_tileset(area),
		map_area_get_zoom(area)
	);
}

static gboolean map_tileset_cb(GtkWidget *widget)
{
	set_legend(
		map_area_get_tileset(area),
		map_area_get_zoom(area)
	);
}

static void set_legend(Tileset tileset, int zoom)
{
	if (tileset == TILESET_MAPNIK){
		char path[100];
		sprintf(path, GOSM_LEGEND_DIR "mapnik_%d.html", zoom);
		char * uri_legend = get_abs_uri(path);
		webkit_web_view_open(WEBKIT_WEB_VIEW(web_legend), uri_legend);
		free(uri_legend);
	}else{
		char * uri_legend = get_abs_uri(GOSM_LEGEND_DIR "no_legend.html");
		webkit_web_view_open(WEBKIT_WEB_VIEW(web_legend), uri_legend);
		free(uri_legend);
	}
}

static gboolean map_selection_changed_cb(GtkWidget *widget)
{
	select_tool_set_lons_n_lats(select_tool,
		area -> selection.lon1,
		area -> selection.lon2,
		area -> selection.lat1,
		area -> selection.lat2);
}

double dist(double lon1, double lat1, double lon2, double lat2)
{
	double RADIUS_EARTH = 6371000.785;
	lon1 = deg_to_rad(lon1);
	lon2 = deg_to_rad(lon2);
	lat1 = deg_to_rad(lat1);
	lat2 = deg_to_rad(lat2);
	double angle = acos(cos(lat1) * cos(lon1) * cos(lat2) * cos(lon2)
		+ cos(lat1) * sin(lon1) * cos(lat2) * sin(lon2)
		+ sin(lat1) * sin(lat2));
	return angle * RADIUS_EARTH;
}

double distance()
{
	double d = 0.0;
	gboolean first = TRUE;
	GList * node = area->path->next;
	double lat1, lat2, lon1, lon2;
	while(node != NULL){
		LonLatPair * ll = (LonLatPair*) node -> data;
		lon1 = ll -> lon;
		lat1 = ll -> lat;
		if (first){
			first = FALSE;
		}else{
			d += dist(lon1, lat1, lon2, lat2);
		}
		lon2 = lon1; lat2 = lat1;
		node = node->next;
	}
	return d;
}

static gboolean map_path_cb(GtkWidget *widget)
{
	char buf[100];
	double meters = distance();
	if (meters > 1000){
		sprintf(buf, "%.3f km", meters/1000.0);
	}else{
		sprintf(buf, "%.3f m", meters);
	}
	gtk_entry_set_text(GTK_ENTRY(distance_tool -> entry_distance), buf);
}

static gboolean show_manual_cb(GtkWidget *widget)
{
	GtkWidget * manual_win = manual_dialog_new(GTK_WINDOW(main_window));
	gtk_widget_show_all(manual_win);
}

static gboolean show_about_cb(GtkWidget *widget, gpointer nump)
{
	int num = GPOINTER_TO_INT(nump);
	printf("%d\n", num);
	GtkWidget * about_win = about_dialog_new(GTK_WINDOW(main_window), area);
	gtk_widget_show_all(about_win);
	gtk_notebook_set_current_page(about_dialog_get_notebook(GOSM_ABOUT_DIALOG(about_win)), num);
}

static void set_button_network()
{
	GtkWidget * icon;
	printf("%d\n", network_state);
	if (network_state){
		icon = gtk_image_new_from_file(GOSM_ICON_DIR "network-idle.png");
		gtk_widget_set_tooltip_text(button_network, "set offline");
	}else{
		icon = gtk_image_new_from_file(GOSM_ICON_DIR "network-offline.png");
		gtk_widget_set_tooltip_text(button_network, "set online");
	}
	gtk_button_set_image(GTK_BUTTON(button_network), icon);
}

static gboolean button_network_cb(GtkWidget *widget)
{
	network_state = !network_state;
	map_area_set_network_state(area, network_state);
	set_button_network();
}

static void button_savemaptype_cb()
{
	char tmp[20]; 
	sprintf(tmp,"%d",tileset);
	config_set_entry(config, "tileset", tmp);
	sprintdouble(tmp,map_area_position_get_center_lon(area),6);
	config_set_entry(config, "longitude", tmp);
	sprintdouble(tmp,map_area_position_get_center_lat(area),6);
	config_set_entry(config, "lattitude", tmp);
	sprintf(tmp,"%d",area -> map_position.zoom);
	config_set_entry(config, "zoom", tmp);
	config_save_config_file(config);
}

static gboolean preferences_confirm_cb(GtkWidget * widget, WidgetPlusPointer * wpp)
{
	ConfigWidget * conf_widget = GOSM_CONFIG_WIDGET(wpp -> pointer);
	gboolean ** changed = config_widget_get_new_configuration(conf_widget);
	config_save_config_file(config);
	gtk_widget_destroy(wpp -> widget);
	free(wpp);
	apply_new_config();
}

static gboolean preferences_cancel_cb(GtkWidget * widget, GtkWindow *window)
{
	gtk_widget_destroy(GTK_WIDGET(window));
}

static gboolean show_preferences_cb(GtkWidget *widget)
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Preferences");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(main_window));
	gtk_window_set_modal(GTK_WINDOW(window), TRUE);
	//g_signal_connect(G_OBJECT(window), "hide", G_CALLBACK(close_cb), NULL);
	ConfigWidget *config_widget = GOSM_CONFIG_WIDGET(config_widget_new(config));
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(config_widget));
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

	WidgetPlusPointer * wpp = malloc(sizeof(WidgetPlusPointer));
	wpp -> widget = GTK_WIDGET(window);
	wpp -> pointer = (gpointer)config_widget;

	g_signal_connect(G_OBJECT(config_widget -> button_cancel), "clicked", G_CALLBACK(preferences_cancel_cb), window);
	g_signal_connect(G_OBJECT(config_widget -> button_confirm), "clicked", G_CALLBACK(preferences_confirm_cb), wpp);

	gtk_widget_show_all(window);
}

static gboolean focus_redirect_cb(GtkWidget *widget, GdkEventButton * event)
{
	gtk_widget_grab_focus(GTK_WIDGET(area));
	return FALSE;
}

static gboolean legend_click_cb(GtkWidget *widget, GdkEventButton *event)
{
	return TRUE;
}

static gboolean namefinder_city_cb(GtkWidget *widget)
{
	int idx = namefinder_cities_get_activated_id(GOSM_NAMEFINDER_CITIES(namefinder));
	city * cities = namefinder_cities_get_cities(GOSM_NAMEFINDER_CITIES(namefinder));
	city thiscity = cities[idx];
	printf("Hopping to: %s, lon: %f, lat: %f\n", thiscity.name, thiscity.lon, thiscity.lat);
	map_area_goto_lon_lat_zoom(area, thiscity.lon, thiscity.lat, map_area_get_zoom(area));
	map_area_repaint(area);
}

static gboolean exit_cb(GtkWidget *widget)
{
	exit(0);
}

void chdir_to_bin(char * arg0)
{
	char * dir = dirname(arg0);
	chdir(dir);
}
