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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libgen.h>
#include <locale.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <webkit/webkit.h>

#include "gosm.h"
#include "menu.h"
#include "toolbar.h"

#include "customio.h"
#include "tool.h"
#include "paths.h"
#include "config/config.h"
#include "config/config_widget.h"
#include "map_area.h"
#include "map_navigator.h"
#include "select/select_tool.h"
#include "distance/distance_tool.h"
#include "atlas/atlas.h"
#include "atlas/atlas_tool.h"
#include "namefinder/namefinder_cities.h"
#include "namefinder/namefinder_countries.h"
#include "wizzard/wizzard_download.h"
#include "wizzard/wizzard_export.h"
#include "wizzard/atlas_template_dialog.h"
#include "wizzard/wizzard_atlas_sequence.h"
#include "wizzard/wizzard_atlas_pdf.h"
#include "wizzard/wizzard_delete_tiles.h"
#include "about/about.h"
#include "manual/manual.h"
#include "tiles/tilemath.h"
#include "tiles/tilesets.h"

#include "imageglue/imageglue.h"
#include "imageglue/pdf_generator.h"

#include "poi/poi_set.h"
#include "poi/poi_manager.h"
#include "poi/poi_tool.h"
#include "poi/poi_selector.h"
#include "poi/poi_source_load_progress.h"
#include "poi/node_tool.h"
#include "poi/poi_statistics.h"

#include <unistd.h>
#include <wait.h>

#include "gtk_safety.h"
#include "widget/gtk_custom_frame.h"

#define CURSOR_HAND		0
#define CURSOR_SELECT		1
#define CURSOR_TARGET		2
#define CURSOR_POINT		3

char * urls[TILESET_LAST] = {
        "http://b.tile.openstreetmap.org/%d/%d/%d.png",
        "http://a.tah.openstreetmap.org/Tiles/tile/%d/%d/%d.png",
	"http://a.andy.sandbox.cloudmade.com/tiles/cycle/%d/%d/%d.png"
};

char * cache_dirs[TILESET_LAST];

Config * 	config;

GtkWidget * 	main_window;
MapArea * 	map_area;
MapNavigator * 	navigator;
GtkWidget *	sidebar_left;
GtkWidget * 	sidebar_right;
GtkWidget * 	menubar;
GtkWidget * 	toolbar;
GtkWidget * 	statusbar;
char 		status_bar_buffer[50];

SelectTool * 	select_tool;
DistanceTool * 	distance_tool;
AtlasTool * 	atlas_tool;
NodeTool * 	node_tool;

GtkWidget * 	web_legend;
GtkWidget *	namefinder_cities;
GtkWidget *	namefinder_countries;

PoiManager *	poi_manager;
PoiSourceLoadProgress * pslp;

void foo();

int main(int argc, char *argv[])
{
	/* ensure, that we are in the dir, where the executable is */
	/* i.e. set cwd to the executable's dir */
	// TODO: don't do this for distro-builds
	chdir_to_bin(argv[0]);

	/* init g_threads, gtk_threads, gtk */
	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	/* the compiler may optimize boilerplate-macro-defined functions away,
	 * to avoid resulting failures every macro is used here once with a volatile variable*/
	make_gtk_types_safe();

	/****************************************************************************************************
	 * CONFIGURATION
	****************************************************************************************************/
	/* create config struct */
	config = GOSM_CONFIG(config_new());
	g_signal_connect(
		G_OBJECT(config), "config-changed",
		G_CALLBACK(apply_new_config), NULL);
	/* fill config struct from config file */
	config_load_config_file(config);

	/* use the configured values */
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
	gboolean	show_sidebar_left	= *(gboolean*)		config_get_entry_data(config, "show_left_sidebar");
	gboolean	show_sidebar_right	= *(gboolean*)		config_get_entry_data(config, "show_sidebar");
	double		longitude		= *(double*)		config_get_entry_data(config, "longitude");
	double		lattitude		= *(double*)		config_get_entry_data(config, "lattitude");
	int		zoom			= *(int*)		config_get_entry_data(config, "zoom");
	gboolean	network_state		= *(gboolean*)		config_get_entry_data(config, "online_on_startup");
 	cache_dirs[TILESET_MAPNIK]		= 			config_get_entry_data(config, "cache_dir_mapnik");
 	cache_dirs[TILESET_OSMARENDER]		= 			config_get_entry_data(config, "cache_dir_osmarender");
 	cache_dirs[TILESET_CYCLE]		= 			config_get_entry_data(config, "cache_dir_cycle");
	ColorQuadriple	color_selection 	= *(ColorQuadriple*)	config_get_entry_data(config, "color_selection");
	ColorQuadriple	color_selection_out 	= *(ColorQuadriple*)	config_get_entry_data(config, "color_selection_out");
	ColorQuadriple	color_selection_pad 	= *(ColorQuadriple*)	config_get_entry_data(config, "color_selection_pad");
	ColorQuadriple	color_atlas_lines	= *(ColorQuadriple*)	config_get_entry_data(config, "color_atlas_lines");
	Tileset tileset				= *(Tileset*)		config_get_entry_data(config, "tileset");

	/* if the config doesn't tell to SET size, use a default size, even if width and height are saved */
	if(!set_size){
		width 	= 900;
		height	= 600;
	}

	/* ensure, that the tmp-directory for tiles exists */
	/* this function attempts to create non-existant directories */
	int t;
	gboolean dirs_ok = TRUE;
	for (t = 0; t < TILESET_LAST; t++){
		dirs_ok &= check_for_cache_directory(cache_dirs[t]);
	}
	if (!dirs_ok) return EXIT_FAILURE;

	/***************************************************************************
	 * GTK WIDGETS AND LAYOUT, CALLBACKS
	 ***************************************************************************/
	/* main window */
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "GOsmView");
	gtk_window_set_icon_from_file(GTK_WINDOW(main_window), GOSM_ICON_DIR "gosm.png", NULL);
	gtk_window_set_default_size(GTK_WINDOW(main_window), width, height);
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(close_cb), NULL);

	/* widget, that displays the map */
	map_area = GOSM_MAP_AREA(map_area_new());
	/* container for the map_area, showing the controls around it */
	navigator = GOSM_MAP_NAVIGATOR(map_navigator_new(GTK_WIDGET(map_area)));

	/* apply config to map_area */
	map_area_goto_lon_lat_zoom(map_area, longitude, lattitude, zoom);
	/* apply network state */
	map_area_set_network_state(map_area, network_state);
	/* apply cache diretories to map */
	for (t = 0; t < TILESET_LAST; t++){
		map_area_set_cache_directory(map_area, t, cache_dirs[t]);
	}
	/* apply colour of selection */
	map_area_set_color_selection(map_area,
		color_selection, color_selection_out, color_selection_pad, color_atlas_lines);

	/* add poi_manager to map-area */
	poi_manager = poi_manager_new();
	map_area_set_poi_manager(map_area, poi_manager);
	poi_manager_set_map_area(poi_manager, map_area);
	g_signal_connect(G_OBJECT(poi_manager),"layer-added", G_CALLBACK(poi_manager_layer_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"layer-deleted", G_CALLBACK(poi_manager_layer_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"layer-toggled", G_CALLBACK(poi_manager_layer_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"colour-changed", G_CALLBACK(poi_manager_colour_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"source-activated", G_CALLBACK(poi_manager_source_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"file-parsing-started", G_CALLBACK(poi_manager_parse_start_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"file-parsing-ended", G_CALLBACK(poi_manager_parse_end_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"api-request-started", G_CALLBACK(poi_manager_api_start_cb), NULL);
	g_signal_connect(G_OBJECT(poi_manager),"api-request-ended", G_CALLBACK(poi_manager_api_end_cb), NULL);

	/* Selection-Widget in sidebar */
	select_tool = select_tool_new();
	GtkWidget * frame_select_tool = gtk_custom_frame_new("Selection");
	gtk_custom_frame_add(GOSM_GTK_CUSTOM_FRAME(frame_select_tool), GTK_WIDGET(select_tool));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_tool -> check_snap), map_area -> snap_selection);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_tool -> check_show), map_area -> show_selection);
	g_signal_connect(G_OBJECT(select_tool -> button_action), "clicked", G_CALLBACK(selection_download_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> button_trash), "clicked", G_CALLBACK(selection_trash_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> button_trash_adv), "clicked", G_CALLBACK(selection_trash_adv_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> button_export), "clicked", G_CALLBACK(selection_export_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> button_data), "clicked", G_CALLBACK(selection_xml_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> button_clipboard), "clicked", G_CALLBACK(selection_clipboard_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> check_snap), "toggled", G_CALLBACK(selection_check_snap_cb), NULL);
	g_signal_connect(G_OBJECT(select_tool -> check_show), "toggled", G_CALLBACK(selection_check_visible_cb), NULL);

	/* Atlas-Widget in sidebar */
	atlas_tool = atlas_tool_new();
	GtkWidget * frame_atlas_tool = gtk_custom_frame_new("Atlas");
	gtk_custom_frame_add(GOSM_GTK_CUSTOM_FRAME(frame_atlas_tool), GTK_WIDGET(atlas_tool));
	PageInformation page_info = {
		PAPERSIZE_A4,
		ORIENTATION_PORTRAIT,
		210, 297, 20, 20, 15, 15, 150
	};
	atlas_tool_set_values(atlas_tool, map_area -> slice_zl, map_area -> show_slice);
	atlas_tool_set_page_info(atlas_tool, page_info);
	atlas_tool_set_intersection(atlas_tool, 150, 150);
	g_signal_connect(G_OBJECT(atlas_tool), "values-changed", G_CALLBACK(atlas_values_changed_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_conf_page), "clicked", G_CALLBACK(atlas_template_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_action), "clicked", G_CALLBACK(atlas_download_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_export), "clicked", G_CALLBACK(atlas_export_cb), NULL);
	g_signal_connect(G_OBJECT(atlas_tool -> button_export_pdf), "clicked", G_CALLBACK(atlas_export_pdf_cb), NULL);

	/* Distantce-Widget in sidebar */
	distance_tool = GOSM_DISTANCE_TOOL(distance_tool_new());
	GtkWidget * frame_distance_tool = gtk_custom_frame_new("Distance");
	gtk_custom_frame_add(GOSM_GTK_CUSTOM_FRAME(frame_distance_tool), GTK_WIDGET(distance_tool));
	g_signal_connect(G_OBJECT(distance_tool -> button_remove_last), "clicked", G_CALLBACK(distance_remove_last_cb), NULL);
	g_signal_connect(G_OBJECT(distance_tool -> button_clear), "clicked", G_CALLBACK(distance_clear_cb), NULL);

	/* Node-Properties-Widget in sidebar */
	node_tool = GOSM_NODE_TOOL(node_tool_new());
	GtkWidget * frame_node_tool = gtk_custom_frame_new("Nodes");
	gtk_custom_frame_add(GOSM_GTK_CUSTOM_FRAME(frame_node_tool), GTK_WIDGET(node_tool));

	gtk_custom_frame_set_child_visible(GOSM_GTK_CUSTOM_FRAME(frame_select_tool), TRUE);
	gtk_custom_frame_set_child_visible(GOSM_GTK_CUSTOM_FRAME(frame_node_tool), TRUE);

	/***************************************************************************
	 * Menubar
	 ***************************************************************************/
	menubar = menu_new(GTK_WINDOW(main_window), map_area, config);

	/***************************************************************************
	 * Statusbar
	 ***************************************************************************/
	statusbar = gtk_statusbar_new();

	/***************************************************************************
	 * right sidebar (tools)
	 ***************************************************************************/
	sidebar_right = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_size_request(sidebar_right, 180, 0);
	gtk_box_pack_start(GTK_BOX(sidebar_right), frame_select_tool, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(sidebar_right), frame_atlas_tool, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(sidebar_right), frame_distance_tool, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(sidebar_right), frame_node_tool, TRUE, TRUE, 0);

	/***************************************************************************
	 * left sidebar (legend, namefinder, pois, bookmarks)
	 ***************************************************************************/
	sidebar_left = gtk_vbox_new(FALSE, 0);
	GtkWidget * notebook_side_left = gtk_notebook_new();
	/* legend */
	web_legend = webkit_web_view_new();
	set_legend(
		map_area_get_tileset(map_area),
		map_area_get_zoom(map_area)
	);
        GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(scrolled), web_legend);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
        gtk_widget_set_size_request(scrolled, 180, -1);
	gtk_box_pack_start(GTK_BOX(sidebar_left), notebook_side_left, TRUE, TRUE, 0);
	/* this is a dummy-callback to prevent the context-menu to appear */
	g_signal_connect(G_OBJECT(web_legend), 	"button-press-event", G_CALLBACK(legend_click_cb), NULL);
	/* pois */
	PoiTool * poi_tool = GOSM_POI_TOOL(poi_tool_new(poi_manager));
	/* bookmarks */
	GtkWidget * placeholder_bookmarks = gtk_vbox_new(FALSE, 0);
	/* namefinder */
	GtkWidget * notebook_namefinder = gtk_notebook_new();
	namefinder_cities = namefinder_cities_new();
	namefinder_countries = namefinder_countries_new();
	g_signal_connect(G_OBJECT(namefinder_cities), "city-activated", G_CALLBACK(namefinder_city_cb), NULL);
	g_signal_connect(G_OBJECT(namefinder_countries), "country-activated", G_CALLBACK(namefinder_country_cb), NULL);
	GtkWidget * label_namefinder_city = gtk_label_new("Cities");
	GtkWidget * label_namefinder_country = gtk_label_new("Countries");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_namefinder), namefinder_cities, label_namefinder_city);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_namefinder), namefinder_countries, label_namefinder_country);
	/* -- */
	GtkWidget * image_legend = gtk_image_new_from_file(GOSM_ICON_DIR "stock_chart-toggle-legend.png");
	GtkWidget * image_pois = gtk_image_new_from_file(GOSM_ICON_DIR "stock_draw-cube.png");
	GtkWidget * image_bookmarks = gtk_image_new_from_file(GOSM_ICON_DIR "stock_bookmark.png");
	GtkWidget * image_namefinder = gtk_image_new_from_file(GOSM_ICON_DIR "stock_internet.png");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_side_left), scrolled, image_legend);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_side_left), notebook_namefinder, image_namefinder);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_side_left), GTK_WIDGET(poi_tool), image_pois);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_side_left), placeholder_bookmarks, image_bookmarks);
	gtk_widget_set_tooltip_text(gtk_notebook_get_tab_label(
		GTK_NOTEBOOK(notebook_side_left), scrolled), "map-key");
	gtk_widget_set_tooltip_text(gtk_notebook_get_tab_label(
		GTK_NOTEBOOK(notebook_side_left), notebook_namefinder), "namefinder");
	gtk_widget_set_tooltip_text(gtk_notebook_get_tab_label(
		GTK_NOTEBOOK(notebook_side_left), GTK_WIDGET(poi_tool)), "points of interest");
	gtk_widget_set_tooltip_text(gtk_notebook_get_tab_label(
		GTK_NOTEBOOK(notebook_side_left), placeholder_bookmarks), "bookmarks (not implemented)");

	/***************************************************************************
	 * Toolbar
	 ***************************************************************************/
	toolbar = toolbar_new(map_area, config, navigator, sidebar_left, sidebar_right);

	/***************************************************************************
	 * Layout of main window
	 ***************************************************************************/
	GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), sidebar_left, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(navigator), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), sidebar_right, FALSE, FALSE, 0);

	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(map_area), "map", G_CALLBACK(map_area_map_cb), NULL);
	g_signal_connect(G_OBJECT(navigator), "key_press_event", G_CALLBACK(key_press_cb), NULL);
	g_signal_connect(G_OBJECT(map_area), "map-been-moved", G_CALLBACK(map_moved_cb), NULL);
	g_signal_connect(G_OBJECT(map_area), "map-zoom-changed", G_CALLBACK(map_zoom_cb), NULL);
	g_signal_connect(G_OBJECT(map_area), "map-tileset-changed", G_CALLBACK(map_tileset_cb), NULL);
	g_signal_connect(G_OBJECT(map_area), "map-selection-changed", G_CALLBACK(map_selection_changed_cb), NULL);
	g_signal_connect(G_OBJECT(map_area), "map-path-changed", G_CALLBACK(map_path_cb), NULL);
	g_signal_connect(G_OBJECT(map_area), "map-node-selected", G_CALLBACK(map_node_cb), NULL);

	gtk_container_add(GTK_CONTAINER(main_window), vbox);
	gtk_widget_show_all(main_window);
	gtk_widget_grab_focus(GTK_WIDGET(map_area));

	/***************************************************************************
	 * apply configuration
	 ***************************************************************************/
	map_navigator_show_controls(navigator, show_controls);
	map_area_set_tileset(map_area, tileset);
	if (show_grid) 		map_area -> show_grid = TRUE;
	if (show_tilenumbers) 	map_area -> show_font = TRUE;
	set_widget_visibility(menubar, show_menubar);
	set_widget_visibility(toolbar, show_toolbar);
	set_widget_visibility(statusbar, show_statusbar);
	set_widget_visibility(sidebar_right, show_sidebar_right);
	set_widget_visibility(sidebar_left, show_sidebar_left);

	/***************************************************************************
	 * main loop
	 ***************************************************************************/
	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();
}

/****************************************************************************************************
* when config has changed, apply new config
* TODO: some changes are only applied on restart
****************************************************************************************************/
static gboolean apply_new_config(Config * config, gpointer data)
{
	ColorQuadriple	color_selection 	= *(ColorQuadriple*)config_get_entry_data(config, "color_selection");
	ColorQuadriple	color_selection_out 	= *(ColorQuadriple*)config_get_entry_data(config, "color_selection_out");
	ColorQuadriple	color_selection_pad 	= *(ColorQuadriple*)config_get_entry_data(config, "color_selection_pad");
	ColorQuadriple	color_atlas_lines	= *(ColorQuadriple*)config_get_entry_data(config, "color_atlas_lines");
	map_area_set_color_selection(map_area, color_selection, color_selection_out, color_selection_pad, color_atlas_lines);
	map_area_repaint(map_area);
	return FALSE;
}

/****************************************************************************************************
* check if directory 'fn' exists
* if not, try to create it, including parent directories
* returns:	TRUE  => directory exists
*		FALSE => directory does not exist and could not be created
****************************************************************************************************/
gboolean check_for_cache_directory(char * fn)
{
        struct stat info;
        int r_stat = stat(fn, &info);
        if (r_stat){
                printf("creating cache directory: %s\n", fn);
                int r_mkdir = g_mkdir_with_parents(fn, 0777);
        }
        r_stat = stat(fn, &info);
        if (r_stat || !S_ISDIR(info.st_mode)){
                printf("could not create chache directory: %s\n", fn);
		return FALSE;
        }
	return TRUE;
}

/****************************************************************************************************
* called when close button is hit 
****************************************************************************************************/
static gboolean close_cb(GtkWidget *widget)
{
	printf("main window has been closed\n");
	gtk_main_quit();
}

/****************************************************************************************************
* map area appeared
****************************************************************************************************/
static gboolean map_area_map_cb(GtkWidget *widget, GdkEventConfigure *event)
{
	//printf("map appeared\n");
	if (map_area -> mouse_mode == -1){
		map_area_set_mouse_mode(map_area, MAP_MODE_MOVE);
	}
}

/****************************************************************************************************
* a key has been pressed
****************************************************************************************************/
static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event)
{
	/* 65470 - 65478 = F1 - F9 */
	//printf("signal %d\n", event -> keyval);
	switch (event -> keyval){
	case 65473:{ /* F4 - toggle left sidebar */
		toggle_widget_visibility(sidebar_left);
		break;
	}
	case 65474:{ /* F5 - toggle controls */
		map_navigator_toggle_controls(navigator);
		break;
	}
	case 65475:{ /* F6 - toggle sidebar */
		toggle_widget_visibility(sidebar_right);
		break;
	}
	case 65476:{ /* F7 - toggle statusbar */
		toggle_widget_visibility(statusbar);
		break;
	}
	case 65477:{ /* F8 - toggle toolbar */
		toggle_widget_visibility(toolbar);
		break;
	}
	case 65478:{ /* F9 - toggle menubar */
		toggle_widget_visibility(menubar);
		break;
	}
	case 65480:{ /* F11 - toggle fullscreen */
		gboolean is_fullscreen = (gdk_window_get_state(main_window -> window) & GDK_WINDOW_STATE_FULLSCREEN) != 0;
		if (is_fullscreen){
			gdk_window_unfullscreen(main_window->window);
		}else{
			gdk_window_fullscreen(main_window->window);
		}
		break;
	}
	case 65481:{ /* F12 */
		foo();
		break;
	}
	/* mode select */
	case 97:{ /* a */
		map_area_set_mouse_mode(map_area, MAP_MODE_MOVE);
		break;
	}
	case 115:{ /* s */
		map_area_set_mouse_mode(map_area, MAP_MODE_SELECT);
		break;
	}
	case 100:{ /* d */
		map_area_set_mouse_mode(map_area, MAP_MODE_PATH);
		break;
	}
	case 102:{ /* f */
		map_area_set_mouse_mode(map_area, MAP_MODE_POI);
		break;
	}
	/* mode select */
	}
	return TRUE; 
}

/****************************************************************************************************
* when a selection shall be downloaded
****************************************************************************************************/
static gboolean selection_download_cb(GtkWidget *widget)
{
	WizzardDownload * wizzard = wizzard_download_new(
		GTK_WINDOW(main_window),
		urls[map_area_get_tileset(map_area)],
		cache_dirs[map_area_get_tileset(map_area)],
		map_area->selection);
	wizzard_download_show(wizzard);
	return FALSE;
}

/****************************************************************************************************
* when a selection's tiles shall be deleted, only on the current layer
****************************************************************************************************/
static gboolean selection_trash_cb(GtkWidget *widget)
{
	printf("trash...\n");
	Selection * selection = &(map_area -> selection);
	int i = map_area_get_zoom(map_area);
	double x1 = lon_to_x(selection->lon1, i);
	double x2 = lon_to_x(selection->lon2, i);
	double y1 = lat_to_y(selection->lat1, i);
	double y2 = lat_to_y(selection->lat2, i);
	int count_x = ((int) x2) - ((int)x1) + 1;
	int count_y = ((int) y2) - ((int)y1) + 1;
	int xs = (int) x1;
	int ys = (int) y1;
	printf("%d %d %d %d\n", xs, ys, count_x, count_y);
	int cx, cy;
	TileManager * tile_manager = map_area_get_tile_manager(map_area, map_area_get_tileset(map_area));
	for (cx = xs; cx < xs + count_x; cx++){
		for (cy = ys; cy < ys + count_y; cy++){
			printf("%d %d\n", cx, cy); 
			tile_manager_delete_tile(tile_manager, cx, cy, i);
		}
	}
	map_area_repaint(map_area);
	return FALSE;
}

/****************************************************************************************************
* when a selection's tiles shall be deleted, multiple layers
****************************************************************************************************/
static gboolean selection_trash_adv_cb(GtkWidget *widget)
{
	WizzardDeleteTiles * wizzard = wizzard_delete_tiles_new(
		map_area,
		map_area_get_tileset(map_area),
		map_area -> selection);
	wizzard_delete_tiles_show(wizzard, GTK_WINDOW(main_window));
	return FALSE;
}

/****************************************************************************************************
* when a selection shall be exported as an image
****************************************************************************************************/
static gboolean selection_export_cb(GtkWidget *widget)
{
	WizzardExport * wizzard = wizzard_export_new(
		GTK_WINDOW(main_window),
		cache_dirs[map_area_get_tileset(map_area)],
		map_area -> selection,
		map_area -> map_position.zoom);
	wizzard_export_show(wizzard);
	return FALSE;
}

/****************************************************************************************************
* when a the api-url shall be copied to clipboard
* TODO: what about clipboard memory management?
****************************************************************************************************/
static gboolean selection_xml_cb(GtkWidget *widget)
{
	Selection s = map_area -> selection;
	double lon1, lon2, lat1, lat2;
	lon1 = s.lon1 < s.lon2 ? s.lon1 : s.lon2;
	lon2 = s.lon1 > s.lon2 ? s.lon1 : s.lon2;
	lat1 = s.lat1 < s.lat2 ? s.lat1 : s.lat2;
	lat2 = s.lat1 > s.lat2 ? s.lat1 : s.lat2;
	char * buf = get_api_url_get(lon1, lat1, lon2, lat2);
	GtkClipboard * cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(cb, buf, strlen(buf));
	printf("API-URL: %s\n", buf);
	return FALSE;
}

/****************************************************************************************************
* when the current selection's coordinates shall be copied to clipboard
* TODO: what about clipboard memory management?
****************************************************************************************************/
static gboolean selection_clipboard_cb(GtkWidget *widget)
{
	Selection s = map_area -> selection;
	printf("Selection (lon1, lon2, lat1, lat2): %f %f %f %f\n", s.lon1, s.lon2, s.lat1, s.lat2);

	double lon1, lon2, lat1, lat2;
	lon1 = s.lon1 < s.lon2 ? s.lon1 : s.lon2;
	lon2 = s.lon1 > s.lon2 ? s.lon1 : s.lon2;
	lat1 = s.lat1 < s.lat2 ? s.lat1 : s.lat2;
	lat2 = s.lat1 > s.lat2 ? s.lat1 : s.lat2;

	char buf[60];
	sprintdouble(buf, lon1, 7);
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), lat1, 7); 
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), lon2, 7); 
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), lat2, 7); 

	GtkClipboard * cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(cb, buf, strlen(buf));
	return FALSE;
}

/****************************************************************************************************
* checked, wheter the selection should snap to the marked area
****************************************************************************************************/
static gboolean selection_check_snap_cb(GtkWidget *widget)
{
	map_area -> snap_selection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(select_tool -> check_snap));
	return FALSE;
}

/****************************************************************************************************
* checked, wheter the selection should be visible
****************************************************************************************************/
static gboolean selection_check_visible_cb(GtkWidget *widget)
{
	map_area -> show_selection = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(select_tool -> check_show));
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
	return FALSE;
}

/****************************************************************************************************
* when the values in the entries of atlas tool have changed
****************************************************************************************************/
static gboolean atlas_values_changed_cb(GtkWidget *widget)
{
	//printf("vals changed\n");
	map_area -> slice_zl = atlas_tool -> slice_zoom;
	map_area -> show_slice = atlas_tool -> visible;
	map_area -> slice_x = atlas_tool -> image_dimension.width;
	map_area -> slice_y = atlas_tool -> image_dimension.height;;
	map_area -> slice_intersect_x = atlas_tool -> intersect_x;
	map_area -> slice_intersect_y = atlas_tool -> intersect_y;
	map_area_repaint(GOSM_MAP_AREA(map_area));
	return FALSE;
}

/****************************************************************************************************
* when the setup-button in atlas-tool is clicked
****************************************************************************************************/
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
	return FALSE;
}

/****************************************************************************************************
* when the download-button in atlas-tool is clicked
****************************************************************************************************/
static gboolean atlas_download_cb(GtkWidget *widget)
{
	WizzardDownload * wizzard = wizzard_download_new(
		GTK_WINDOW(main_window),
		urls[map_area_get_tileset(map_area)],
		cache_dirs[map_area_get_tileset(map_area)],
		map_area->selection);
	wizzard_download_show(wizzard);
	wizzard_download_set_active(wizzard, GPOINTER_TO_INT(map_area->slice_zl), TRUE);
	return FALSE;
}

/****************************************************************************************************
* when the export-as-image-button in atlas-tool is clicked
****************************************************************************************************/
static gboolean atlas_export_cb(GtkWidget *widget)
{
	//printf("exp\n");
	WizzardAtlasSequence * was = wizzard_atlas_sequence_new(
		cache_dirs[map_area_get_tileset(map_area)],
		atlas_tool -> slice_zoom,
		map_area -> selection,
		atlas_tool -> image_dimension,
		atlas_tool -> intersect_x,
		atlas_tool -> intersect_y);
	wizzard_atlas_sequence_show(was, GTK_WINDOW(main_window));
	return FALSE;
}

/****************************************************************************************************
* when the export-as-pdf-button in atlas-tool is clicked
****************************************************************************************************/
static gboolean atlas_export_pdf_cb(GtkWidget *widget)
{
	//printf("exp\n");
	WizzardAtlasPdf * wap = wizzard_atlas_pdf_new(
		cache_dirs[map_area_get_tileset(map_area)],
		atlas_tool -> slice_zoom,
		map_area -> selection,
		atlas_tool -> page_info, 
		atlas_tool -> image_dimension,
		atlas_tool -> intersect_x,
		atlas_tool -> intersect_y);
	wizzard_atlas_pdf_show(wap, GTK_WINDOW(main_window));
	return FALSE;
}

/****************************************************************************************************
* when the last node of the distance-path shall be removed
****************************************************************************************************/
static gboolean distance_remove_last_cb(GtkWidget *widget)
{
	map_area_path_remove_point(map_area);
	return FALSE;
}

/****************************************************************************************************
* when all nodes of the distance-path shall be cleared
****************************************************************************************************/
static gboolean distance_clear_cb(GtkWidget *widget)
{
	map_area_path_clear(map_area);
	return FALSE;
}

/****************************************************************************************************
* when the map has changed the position
****************************************************************************************************/
static gboolean map_moved_cb(GtkWidget *widget)
{
	sprintf(status_bar_buffer, "lon: %f lat: %f zoom: %d",
		map_area_position_get_center_lon(map_area),
		map_area_position_get_center_lat(map_area),
		map_area -> map_position.zoom);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, status_bar_buffer);
	select_tool_set_lons_n_lats(select_tool,
		map_area -> selection.lon1,
		map_area -> selection.lon2,
		map_area -> selection.lat1,
		map_area -> selection.lat2);
	return FALSE;
}

/****************************************************************************************************
* when the map's zoomlevel has changed
****************************************************************************************************/
static gboolean map_zoom_cb(GtkWidget *widget)
{
	set_legend(
		map_area_get_tileset(map_area),
		map_area_get_zoom(map_area)
	);
	return FALSE;
}

/****************************************************************************************************
* when the map's tileset has changed
****************************************************************************************************/
static gboolean map_tileset_cb(GtkWidget *widget)
{
	set_legend(
		map_area_get_tileset(map_area),
		map_area_get_zoom(map_area)
	);
	return FALSE;
}

/****************************************************************************************************
* display the legend according to zoomlevel and zoom
****************************************************************************************************/
static void set_legend(Tileset tileset, int zoom)
{
	if (tileset == TILESET_MAPNIK){
		char path[100];
		sprintf(path, GOSM_LEGEND_DIR "mapnik/mapnik_%d.html", zoom);
		char * uri_legend = get_abs_uri(path);
		webkit_web_view_open(WEBKIT_WEB_VIEW(web_legend), uri_legend);
		free(uri_legend);
	}else if (tileset == TILESET_CYCLE){
		char path[100];
		sprintf(path, GOSM_LEGEND_DIR "cycle/cycle_%d.html", zoom);
		char * uri_legend = get_abs_uri(path);
		webkit_web_view_open(WEBKIT_WEB_VIEW(web_legend), uri_legend);
		free(uri_legend);
	}else{
		char * uri_legend = get_abs_uri(GOSM_LEGEND_DIR "no_legend.html");
		webkit_web_view_open(WEBKIT_WEB_VIEW(web_legend), uri_legend);
		free(uri_legend);
	}
}

/****************************************************************************************************
* when the selected area has changed
****************************************************************************************************/
static gboolean map_selection_changed_cb(GtkWidget *widget)
{
	select_tool_set_lons_n_lats(select_tool,
		map_area -> selection.lon1,
		map_area -> selection.lon2,
		map_area -> selection.lat1,
		map_area -> selection.lat2);
	return FALSE;
}

/****************************************************************************************************
* calculate the distance between these two points
****************************************************************************************************/
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

/****************************************************************************************************
* calculate the distance of the whole distance-path currently there
****************************************************************************************************/
double distance()
{
	double d = 0.0;
	gboolean first = TRUE;
	GList * node = map_area->path->next;
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

/****************************************************************************************************
* when the current distance-path has changed
* TODO: what about memory management of the old values in the entries?
****************************************************************************************************/
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
	return FALSE;
}

/****************************************************************************************************
* when the legend has been clicked. return TRUE, so that no context menu will be displayed
****************************************************************************************************/
static gboolean legend_click_cb(GtkWidget *widget, GdkEventButton *event)
{
	return TRUE;
}

/****************************************************************************************************
* when a city has been activated in the namefinder
****************************************************************************************************/
static gboolean namefinder_city_cb(GtkWidget *widget)
{
	int idx = namefinder_cities_get_activated_id(GOSM_NAMEFINDER_CITIES(namefinder_cities));
	city * cities = namefinder_cities_get_cities(GOSM_NAMEFINDER_CITIES(namefinder_cities));
	city thiscity = cities[idx];
	printf("Hopping to: %s, lon: %f, lat: %f\n", thiscity.name, thiscity.lon, thiscity.lat);
	map_area_goto_lon_lat_zoom(map_area, thiscity.lon, thiscity.lat, map_area_get_zoom(map_area));
	map_area_repaint(map_area);
	return FALSE;
}

/****************************************************************************************************
* when a country has been activated in the namefinder
****************************************************************************************************/
static gboolean namefinder_country_cb(GtkWidget *widget)
{
	int idx = namefinder_countries_get_activated_id(GOSM_NAMEFINDER_COUNTRIES(namefinder_countries));
	country * countries = namefinder_countries_get_countries(GOSM_NAMEFINDER_COUNTRIES(namefinder_countries));
	country thiscountry = countries[idx];
	printf("Hopping to: %s, lon: %f, lat: %f\n", thiscountry.name, thiscountry.lon, thiscountry.lat);
	//map_area_goto_lon_lat_zoom(map_area, thiscountry.lon, thiscountry.lat, map_area_get_zoom(map_area));
	map_area_goto_bbox(map_area, thiscountry.lon1, thiscountry.lat1, thiscountry.lon2, thiscountry.lat2);
	map_area_repaint(map_area);
	return FALSE;
}

/****************************************************************************************************
* set CWD to the the binaries directory
****************************************************************************************************/
void chdir_to_bin(char * arg0)
{
	char * dir = dirname(arg0);
	chdir(dir);
}

/****************************************************************************************************
* when a poi-layer has been activated / deactivated
****************************************************************************************************/
static gboolean poi_manager_layer_cb(PoiSelector * poi_selector, int index)
{
	map_area_repaint(map_area);
	return FALSE;
}

/****************************************************************************************************
* when a poi-layer's colour has changed
****************************************************************************************************/
static gboolean poi_manager_colour_cb(PoiManager * poi_manager, int index)
{
	map_area_repaint(map_area);
	return FALSE;
}

/****************************************************************************************************
* when a poi_manager's source-file has been loaded
****************************************************************************************************/
static gboolean poi_manager_source_cb(PoiManager * poi_manager, int index)
{
	map_area_repaint(map_area);
	return FALSE;
}

/****************************************************************************************************
* when parsing of a source file started
****************************************************************************************************/
static gboolean poi_manager_parse_start_cb(PoiManager * poi_manager, int index)
{
	pslp = poi_source_load_progress_new();
	poi_source_load_progress_show(pslp, GTK_WINDOW(main_window), poi_manager -> osm_reader);
	return FALSE;
}

/****************************************************************************************************
* when parsing of a source file finished
****************************************************************************************************/
static gboolean poi_manager_parse_end_cb(PoiManager * poi_manager, int index)
{
	gdk_threads_enter();
	poi_source_load_progress_destroy(pslp);
	gtk_widget_queue_draw(main_window);
	gdk_threads_leave();
	return FALSE;
}

/****************************************************************************************************
* when a request api-call for pois has started
****************************************************************************************************/
static gboolean poi_manager_api_start_cb(PoiManager * poi_manager)
{
	return FALSE;
}

/****************************************************************************************************
* when a requested api-call for pois has finished
****************************************************************************************************/
static gboolean poi_manager_api_end_cb(PoiManager * poi_manager, int index)
{
	gdk_threads_enter();
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
	gtk_widget_queue_draw(main_window);
	gdk_threads_leave();
	return FALSE;
}

/****************************************************************************************************
* when a poi-node has been clicked
****************************************************************************************************/
static gboolean map_node_cb(GtkWidget *widget, gpointer llt_p)
{
	LonLatTags * llt = (LonLatTags*) llt_p;
	node_tool_set_tags(node_tool, llt);
	return FALSE;
}

void foo()
{
	PoiStatistics * poi_stats = poi_statistics_new();
	poi_statistics_analyze(poi_stats, poi_manager);
	poi_statistics_show_window(poi_stats);
}
