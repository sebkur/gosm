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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include <sys/time.h>
#include <string.h>
#include <math.h>

#include "map_area.h"
#include "map_types.h"
#include "tool.h"
#include "tiles/tilemath.h"
#include "tiles/tile_cache.h"
#include "tiles/tile_manager.h"
#include "tiles/tilesets.h"
#include "paths.h"

#define MATCHES(a,b)	((a & b) == b)
#define TILESIZE	256
#define CACHE_SIZE	164

#define DIRECTION_TOP_LEFT	1
#define DIRECTION_TOP		2
#define DIRECTION_TOP_RIGHT	3
#define DIRECTION_LEFT		4
#define DIRECTION_RIGHT		5
#define DIRECTION_DOWN_LEFT	6
#define DIRECTION_DOWN		7
#define DIRECTION_DOWN_RIGHT	8

// selection resize pads
#define MAX_PAD_SIZE		40
#define PAD_FRACTION		3

G_DEFINE_TYPE (MapArea, map_area, GTK_TYPE_DRAWING_AREA);

enum
{
	MAP_BEEN_MOVED,
	MAP_ZOOM_CHANGED,
	MAP_TILESET_CHANGED,
	MAP_SELECTION_CHANGED,
	MAP_PATH_CHANGED,
	MAP_NODE_SELECTED,
	MAP_MOUSE_MODE_CHANGED,
	MAP_NETWORK_STATE_CHANGED,
	MAP_GRID_TOGGLED,
	MAP_FONT_TOGGLED,
	LAST_SIGNAL
};

static guint map_area_signals[LAST_SIGNAL] = { 0 };

extern char * urls[];

struct timeval t1, t2;

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event);
static gboolean mouse_button_cb(GtkWidget *widget, GdkEventButton *event);
static gboolean mouse_motion_cb(GtkWidget *widget, GdkEventMotion *event);
static gboolean key_press_cb(MapArea *map_area, GdkEventKey *event);
static gboolean scroll_cb(MapArea *map_area, GdkEventScroll *event);
static gboolean tile_loaded_cb(TileManager * tile_manager, MapArea *map_area);

void map_has_moved(MapArea *map_area);
void map_area_adjust_map_position_values(MapArea *map_area);
void map_area_adjust_tile_count_information(MapArea *map_area);
double map_area_get_poi_square_size(MapArea *map_area);

GtkWidget * map_area_new()
{
	MapArea * map_area = g_object_new(GOSM_TYPE_MAP_AREA, NULL);
	return GTK_WIDGET(map_area);
}

static void map_area_class_init(MapAreaClass *class)
{
	GtkWidgetClass *widget_class;
	widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> expose_event = expose_cb;

	map_area_signals[MAP_BEEN_MOVED] = g_signal_new(
		"map-been-moved",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_been_moved),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_ZOOM_CHANGED] = g_signal_new(
		"map-zoom-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_zoom_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_TILESET_CHANGED] = g_signal_new(
		"map-tileset-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_tileset_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_SELECTION_CHANGED] = g_signal_new(
		"map-selection-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_selection_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_PATH_CHANGED] = g_signal_new(
		"map-path-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_path_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
	
	map_area_signals[MAP_NODE_SELECTED] = g_signal_new(
		"map-node-selected",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_node_selected),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1, G_TYPE_POINTER);

	map_area_signals[MAP_MOUSE_MODE_CHANGED] = g_signal_new(
		"map-mouse-mode-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_mouse_mode_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_NETWORK_STATE_CHANGED] = g_signal_new(
		"map-network-state-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_network_state_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_GRID_TOGGLED] = g_signal_new(
		"map-grid-toggled",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_grid_toggled),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_FONT_TOGGLED] = g_signal_new(
		"map-font-toggled",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_font_toggled),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
}

/****************************************************************************************************
* init function
****************************************************************************************************/
static void map_area_init(MapArea *map_area)
{
	map_area -> tileset = TILESET_MAPNIK;
	// TODO: this initialization is useless!
	map_area -> map_position.width  	= 1280;
	map_area -> map_position.height 	= 600;
	map_area -> map_position.zoom 		= 11;
	double x 				= lon_to_x(13.411, 11) - 500/256.0;
	double y 				= lat_to_y(52.523, 11) - 300/256.0;
	map_area -> map_position.tile_top 	= (int)y;
	map_area -> map_position.tile_left 	= (int)x;
	map_area -> map_position.tile_offset_y 	= ((int)(y * 256.0)) % 256;
	map_area -> map_position.tile_offset_x 	= ((int)(x * 256.0)) % 256;
	map_area -> map_position.tile_count_x 	= 2;
	map_area -> map_position.tile_count_y 	= 2;

	map_area -> mouse_mode			= -1;
	map_area -> show_grid			= FALSE;
	map_area -> show_font			= FALSE;
	map_area -> show_selection		= TRUE;
	map_area -> snap_selection		= TRUE;
	// end useless part

	map_area -> map_moved			= TRUE;
	map_area -> need_repaint		= TRUE;

	map_area -> selection.x1 		= 0;
	map_area -> selection.x2 		= 0;
	map_area -> selection.y1 		= 0;
	map_area -> selection.y2 		= 0;
	// test selection
	/*
	map_area -> selection.lon1 		= 12.94;
	map_area -> selection.lat1 		= 52.74;
	map_area -> selection.lon2 		= 13.75;
	map_area -> selection.lat2 		= 52.3;
	*/

	map_area -> show_slice = FALSE;
	map_area -> slice_zl= 13;
        map_area -> slice_x = 1700;
        map_area -> slice_y = 1200;
        map_area -> slice_intersect_x = 150;
        map_area -> slice_intersect_y = 150;

	map_area -> path			= g_list_alloc();

	g_signal_connect(G_OBJECT(map_area), "motion_notify_event",	G_CALLBACK(mouse_motion_cb),	NULL);
	g_signal_connect(G_OBJECT(map_area), "button_press_event", 	G_CALLBACK(mouse_button_cb),	NULL);
	g_signal_connect(G_OBJECT(map_area), "button_release_event",	G_CALLBACK(mouse_button_cb),	NULL);
	g_signal_connect(G_OBJECT(map_area), "key_press_event",		G_CALLBACK(key_press_cb),	NULL);
	g_signal_connect(G_OBJECT(map_area), "scroll_event",		G_CALLBACK(scroll_cb),		NULL);

	// don't use GDK_KEY_PRESS, old versions of gtk don't realize motion-notifys anymore
	gtk_widget_set_events(GTK_WIDGET(map_area), gtk_widget_get_events(GTK_WIDGET(map_area)) |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_EXPOSURE_MASK );
	// GDK_SCROLL_MASK should be there since we are using it
	int i;
	for (i = 0; i < TILESET_LAST; i++){
		map_area -> tile_manager[i] = GOSM_TILE_MANAGER(tile_manager_new());
		tile_manager_set_url_format(map_area -> tile_manager[i], urls[i]);
		g_signal_connect(G_OBJECT(map_area -> tile_manager[i]), "tile_loaded_from_disk", G_CALLBACK(tile_loaded_cb), map_area);
	}
	map_area -> poi_active_id = 0;
	map_area -> next_marker_id = 1;
}

/****************************************************************************************************
*****************************************************************************************************
* SETTERS AND GETTERS
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* set/get the mouse mode, i.e. move, select, path or set pois
****************************************************************************************************/
void map_area_set_mouse_mode(MapArea *map_area, MouseMode mouse_mode)
{
	map_area -> mouse_mode = mouse_mode;
	GdkCursor * cursor; 
	switch(mouse_mode){
		case MAP_MODE_SELECT:	cursor = gdk_cursor_new(GDK_CROSS); break;
		case MAP_MODE_PATH:	cursor = gdk_cursor_new(GDK_TARGET); break;
		case MAP_MODE_POI:	cursor = gdk_cursor_new(GDK_TARGET); break;
		default: 		cursor = gdk_cursor_new(GDK_ARROW); break;
	}
	gdk_window_set_cursor(GTK_WIDGET(map_area)->window, cursor);
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_MOUSE_MODE_CHANGED], 0);
}
MouseMode map_area_get_mouse_mode(MapArea *map_area)
{
	return map_area -> mouse_mode;
}

/****************************************************************************************************
* set/get the current tileset displayed by this map-widget
****************************************************************************************************/
void map_area_set_tileset(MapArea *map_area, Tileset tileset)
{
	if (map_area -> tileset != tileset){
		map_area -> tileset = tileset;
		map_area_repaint(map_area);
		g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_TILESET_CHANGED], 0);
	}
}
Tileset map_area_get_tileset(MapArea *map_area)
{
	return map_area -> tileset;
}

/****************************************************************************************************
* set the cache directory for a tileset
****************************************************************************************************/
void map_area_set_cache_directory(MapArea * map_area, Tileset tileset, char * directory)
{
	if (map_area -> cache_dir[tileset] != NULL){
		free(map_area -> cache_dir[tileset]);
	}
	map_area -> cache_dir[tileset] = malloc(sizeof(char) * (strlen(directory) + 1));
	strcpy(map_area -> cache_dir[tileset], directory);
	tile_manager_set_cache_directory(map_area -> tile_manager[tileset], directory);
}

/****************************************************************************************************
* get the associated tile_manager for a given tileset
****************************************************************************************************/
TileManager * map_area_get_tile_manager(MapArea *map_area, Tileset tileset)
{
	return map_area -> tile_manager[tileset];
}

/****************************************************************************************************
* set/get whether the widget should load tiles over network
****************************************************************************************************/
void map_area_set_network_state(MapArea *map_area, gboolean state)
{
	int i;
	for (i = 0; i < TILESET_LAST; i++){
		tile_manager_set_network_state(map_area -> tile_manager[i], state);
		// TODO change state of tileloader
	}
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_NETWORK_STATE_CHANGED], 0);
}
gboolean map_area_get_network_state(MapArea * map_area)
{
	return tile_manager_get_network_state(map_area -> tile_manager[0]);
}

/****************************************************************************************************
* set/get whether the map-widget should display a tile-grid
****************************************************************************************************/
void map_area_set_show_grid(MapArea * map_area, gboolean show)
{
	if (map_area -> show_grid != show){
		map_area -> show_grid = show;
		map_area_repaint(map_area);
		g_signal_emit (map_area, map_area_signals[MAP_GRID_TOGGLED], 0);
	}
}
gboolean map_area_get_show_grid(MapArea * map_area)
{
	return map_area -> show_grid;
}

/****************************************************************************************************
* set/get whether the map-widget should display tile-names
****************************************************************************************************/
void map_area_set_show_font(MapArea * map_area, gboolean show)
{
	if (map_area -> show_font != show){
		map_area -> show_font = show;
		map_area_repaint(map_area);
		g_signal_emit (map_area, map_area_signals[MAP_FONT_TOGGLED], 0);
	}
}
gboolean map_area_get_show_font(MapArea * map_area)
{
	return map_area -> show_font;
}

/****************************************************************************************************
* get current positions' x (in pixels of the whole underlying zoomlevels' map images)
****************************************************************************************************/
int map_area_position_get_center_x(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	return map_position -> tile_left * 256 + map_position -> tile_offset_x + map_position -> width/2;
}

/****************************************************************************************************
* get current positions' y (in pixels of the whole underlying zoomlevels' map images)
****************************************************************************************************/
int map_area_position_get_center_y(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	return map_position -> tile_top * 256 + map_position -> tile_offset_y + map_position -> height/2;
}

/****************************************************************************************************
* get current positions' longitude
****************************************************************************************************/
double map_area_position_get_center_lon(MapArea *map_area)
{
	return x_to_lon(map_area_position_get_center_x(map_area) / 256.0, map_area -> map_position.zoom);
}

/****************************************************************************************************
* get current positions' lattitude
****************************************************************************************************/
double map_area_position_get_center_lat(MapArea *map_area)
{
	return y_to_lat(map_area_position_get_center_y(map_area) / 256.0, map_area -> map_position.zoom);
}

/****************************************************************************************************
* get current zoom level
****************************************************************************************************/
int map_area_get_zoom(MapArea *map_area)
{
	return map_area -> map_position.zoom;
}

/****************************************************************************************************
* locate a longitude on the widget
****************************************************************************************************/
int map_area_lon_to_area_x(MapArea *map_area, double lon)
{
	MapPosition *map_position = &(map_area -> map_position);
	double x = lon_to_x(lon, map_position -> zoom);
	return (x - map_position -> tile_left - map_position -> tile_offset_x / 256.0) * 256.0;
}

/****************************************************************************************************
* locate a lattitude on the widget
****************************************************************************************************/
int map_area_lat_to_area_y(MapArea *map_area, double lat)
{
	MapPosition *map_position = &(map_area -> map_position);
	double y = lat_to_y(lat, map_position -> zoom);
	return (y - map_position -> tile_top - map_position -> tile_offset_y / 256.0) * 256.0;
}

/****************************************************************************************************
* get the longitude of a position on the widget
****************************************************************************************************/
double map_area_x_to_lon(MapArea *map_area, int x)
{
	MapPosition *map_position = &(map_area -> map_position);
	double pos = map_position -> tile_left + map_position -> tile_offset_x / 256.0 + x / 256.0;
	return x_to_lon(pos, map_position -> zoom);
}

/****************************************************************************************************
* get the lattitude of a position on the widget
****************************************************************************************************/
double map_area_y_to_lat(MapArea *map_area, int y)
{
	MapPosition *map_position = &(map_area -> map_position);
	double pos = map_position -> tile_top + map_position -> tile_offset_y / 256.0 + y / 256.0;
	return y_to_lat(pos, map_position -> zoom);
}

/****************************************************************************************************
* put the boudaries of the currently visible area into the lon/lat parameters
****************************************************************************************************/
void map_area_get_visible_area(MapArea * map_area, double *min_lon, double *min_lat, double *max_lon, double *max_lat)
{
	int width = map_area -> map_position.width;
	int height = map_area -> map_position.height;
	double lon_m = map_area -> map_position.lon;
	double lat_m = map_area -> map_position.lat;
	int zoom = map_area -> map_position.zoom;
	double width_half = ((double) width) / 2;
	double height_half = ((double) height) / 2;
	double x = lon_to_x(lon_m, zoom);
	double x_le = x - width_half / 256;
	double x_ri = x + width_half / 256;
	*min_lon = x_to_lon(x_le, zoom);
	*max_lon  = x_to_lon(x_ri, zoom);
	double y = lat_to_y(lat_m, zoom);
	double y_to = y - height_half / 256;
	double y_bo = y + height_half / 256;
	*max_lat = y_to_lat(y_to, zoom);
	*min_lat = y_to_lat(y_bo, zoom);
}

/****************************************************************************************************
* check whether the given bounding box could be displayed in a map-widget with the size given by
* width and height
****************************************************************************************************/
gboolean map_area_check_if_fits(double lon1, double lat1, double lon2, double lat2, int zoom, int width, int height)
{
	double width_half = ((double) width) / 2;
	double height_half = ((double) height) / 2;

	double lon_m = middle_lon(lon1, lon2);
	double lat_m = middle_lat(lat1, lat2);
	double x = lon_to_x(lon_m, zoom);
	double x_le = x - width_half / 256;
	double x_ri = x + width_half / 256;
	double lon_le = x_to_lon(x_le, zoom);
	double lon_ri = x_to_lon(x_ri, zoom);
	double y = lat_to_y(lat_m, zoom);
	double y_to = y - height_half / 256;
	double y_bo = y + height_half / 256;
	double lat_to = y_to_lat(y_to, zoom);
	double lat_bo = y_to_lat(y_bo, zoom);
	printf("x = %f, left = %f, right = %f\n", x, lon_le, lon_ri);
	return lon_le <= lon1 && lon_ri >= lon2 && lat_to >= lat1 && lat_bo <= lat2;
}

/****************************************************************************************************
* return the maximum zoomlevel that could be used to display the given bounding box in a map-widget
* with the size given by width and height
****************************************************************************************************/
int map_area_get_max_bbox_zoomlevel(double lon1, double lat1, double lon2, double lat2, int width, int height)
{
	int zoom = 1;
	int zoom_test;
	for (zoom_test = 2; zoom_test <= 18; zoom_test++){
		if (map_area_check_if_fits(lon1, lat1, lon2, lat2, zoom_test, width, height)){
			zoom = zoom_test;
		}else{
			break;
		}
	}
	return zoom;
}

/****************************************************************************************************
* set the poi_manager that should be used by this map-widget
****************************************************************************************************/
void map_area_set_poi_manager(MapArea *map_area, PoiManager *poi_manager)
{
	map_area -> poi_manager = poi_manager;
}

/****************************************************************************************************
* set the colours of the selection
****************************************************************************************************/
void map_area_set_color_selection(MapArea *map_area, ColorQuadriple c_s, ColorQuadriple c_s_out, ColorQuadriple c_s_pad, ColorQuadriple c_a_lines)
{
	map_area -> color_selection = c_s;
	map_area -> color_selection_out = c_s_out;
	map_area -> color_selection_pad = c_s_pad;
	map_area -> color_atlas_lines = c_a_lines;
}

/****************************************************************************************************
*****************************************************************************************************
* WIDGET CONTROL
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* just repaint the widget
****************************************************************************************************/
void map_area_repaint(MapArea *map_area)
{
	map_area -> need_repaint = TRUE;
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

/****************************************************************************************************
* reposition the map to the given parameters
****************************************************************************************************/
void map_area_goto_lon_lat_zoom(MapArea *map_area, double lon, double lat, int zoom)
{
	//printf("%f %f %d\n", lon, lat, zoom);
	map_area -> map_position.lon = lon;
	map_area -> map_position.lat = lat;
	map_area -> map_position.zoom = zoom;
	map_area_adjust_map_position_values(map_area);
	map_area_adjust_tile_count_information(map_area);
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_BEEN_MOVED], 0);
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_ZOOM_CHANGED], 0);
	map_has_moved(map_area);
}

/****************************************************************************************************
* reposition the map to a bounding-box
****************************************************************************************************/
void map_area_goto_bbox(MapArea *map_area, double lon1, double lat1, double lon2, double lat2)
{
	int width = map_area -> map_position.width;
	int height = map_area -> map_position.height;
	int zoom = map_area_get_max_bbox_zoomlevel(lon1, lat1, lon2, lat2, width, height);
	printf("proposed zoomlevel: %d\n", zoom);
	map_area_goto_lon_lat_zoom(map_area, middle_lon(lon1, lon2), middle_lat(lat1, lat2), zoom);
}

/****************************************************************************************************
* PRIVATE: adjust internal values after moving the map
****************************************************************************************************/
void map_has_moved(MapArea *map_area)
{
	map_area -> map_moved = TRUE;
	if (map_area -> snap_selection){
		map_area -> selection.x1 = map_area_lon_to_area_x(map_area, map_area -> selection.lon1);
		map_area -> selection.x2 = map_area_lon_to_area_x(map_area, map_area -> selection.lon2);
		map_area -> selection.y1 = map_area_lat_to_area_y(map_area, map_area -> selection.lat1);
		map_area -> selection.y2 = map_area_lat_to_area_y(map_area, map_area -> selection.lat2);
	}else{
		map_area -> selection.lon1 = map_area_x_to_lon(map_area, map_area -> selection.x1),
		map_area -> selection.lon2 = map_area_x_to_lon(map_area, map_area -> selection.x2),
		map_area -> selection.lat1 = map_area_y_to_lat(map_area, map_area -> selection.y1),
		map_area -> selection.lat2 = map_area_y_to_lat(map_area, map_area -> selection.y2);
	}
	map_area -> map_position.lon = map_area_x_to_lon(map_area, map_area -> map_position.width / 2);
	map_area -> map_position.lat = map_area_y_to_lat(map_area, map_area -> map_position.height / 2);
	g_signal_emit (map_area, map_area_signals[MAP_BEEN_MOVED], 0);
}

/****************************************************************************************************
* PRIVATE: adjust internal position values
****************************************************************************************************/
void map_area_adjust_map_position_values(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	double x = lon_to_x(map_position -> lon, map_position -> zoom) - map_position -> width/2/256.0;
	double y = lat_to_y(map_position -> lat, map_position -> zoom) - map_position -> height/2/256.0;
	map_area -> map_position.tile_top 	= (int)y;
	map_area -> map_position.tile_left 	= (int)x;
	map_area -> map_position.tile_offset_y 	= ((int)(y * 256.0)) % 256;
	map_area -> map_position.tile_offset_x 	= ((int)(x * 256.0)) % 256;	
}

/****************************************************************************************************
* PRIVATE: adjust the internal information about the currently visible number of tiles
****************************************************************************************************/
void map_area_adjust_tile_count_information(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	int width = map_position -> width;
	int height = map_position -> height;
	int width_1 = width - TILESIZE +  map_position -> tile_offset_x;
	int height_1 = height - TILESIZE +  map_position -> tile_offset_y;
	width_1 = width_1 > 0 ? width_1 : 0;
	height_1 = height_1 > 0 ? height_1 :0;
	map_position -> tile_count_x = width_1 / 256 + 2;
	map_position -> tile_count_y = height_1 / 256 + 2;
}

/****************************************************************************************************
* PRIVATE: return the current size of poi-squares
****************************************************************************************************/
double map_area_get_poi_square_size(MapArea *map_area)
{
	switch(map_area -> map_position.zoom){
	case(7):
	case(8): return 3;
	case(9):
	case(10): return 4;
	case(11):
	case(12): return 5;
	case(13):
	case(14): return 6;
	case(15): return 7;
	case(16): return 8;
	case(17): return 9;
	case(18): return 10;
	default: return 2;
	}
}

/****************************************************************************************************
* move the map position for one map-tile in the given direction
****************************************************************************************************/
void map_area_move(MapArea *map_area, int direction)
{
	MapPosition * map_position = &(map_area -> map_position);
	switch(direction){
		case DIRECTION_LEFT:
		case DIRECTION_TOP_LEFT:
		case DIRECTION_DOWN_LEFT:
			map_position -> tile_left -= 1; break;
		case DIRECTION_RIGHT:
		case DIRECTION_TOP_RIGHT:
		case DIRECTION_DOWN_RIGHT:
			map_position -> tile_left += 1; break;
	}
	switch(direction){
		case DIRECTION_TOP:
		case DIRECTION_TOP_LEFT:
		case DIRECTION_TOP_RIGHT:
			map_position -> tile_top -= 1; break;
		case DIRECTION_DOWN:
		case DIRECTION_DOWN_LEFT:
		case DIRECTION_DOWN_RIGHT:
			map_position -> tile_top += 1; break;
	}
	map_has_moved(map_area);
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

/****************************************************************************************************
* zoom in one step
****************************************************************************************************/
void map_area_zoom_in(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	if (map_position -> zoom < 18){
		map_position -> zoom += 1;
		int middle_x_old = map_position -> tile_left * 256 + map_position -> tile_offset_x + map_position -> width/2;
		int corner_x_new = middle_x_old * 2 - map_position -> width/2;
		map_position -> tile_offset_x = corner_x_new % 256;
		map_position -> tile_left = corner_x_new / 256;
		int middle_y_old = map_position -> tile_top * 256 + map_position -> tile_offset_y + map_position -> height/2;
		int corner_y_new = middle_y_old * 2 - map_position -> height/2;
		map_position -> tile_offset_y = corner_y_new % 256;
		map_position -> tile_top = corner_y_new / 256;

		map_has_moved(map_area);
		g_signal_emit (map_area, map_area_signals[MAP_ZOOM_CHANGED], 0);
		gtk_widget_queue_draw(GTK_WIDGET(map_area));
	}
}

/****************************************************************************************************
* zoom out one step
****************************************************************************************************/
void map_area_zoom_out(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	if (map_position -> zoom > 1){
		map_position -> zoom -= 1;
		int middle_x_old = map_position -> tile_left * 256 + map_position -> tile_offset_x + map_position -> width/2;
		int corner_x_new = middle_x_old / 2.0 - map_position -> width/2;
		map_position -> tile_offset_x = corner_x_new % 256;
		map_position -> tile_left = corner_x_new / 256;
		int middle_y_old = map_position -> tile_top * 256 + map_position -> tile_offset_y + map_position -> height/2;
		int corner_y_new = middle_y_old / 2.0 - map_position -> height/2;
		map_position -> tile_offset_y = corner_y_new % 256;
		map_position -> tile_top = corner_y_new / 256;
		
		map_has_moved(map_area);
		g_signal_emit (map_area, map_area_signals[MAP_ZOOM_CHANGED], 0);
		gtk_widget_queue_draw(GTK_WIDGET(map_area));
	}
}

/****************************************************************************************************
* add one point to the existing distance-path
****************************************************************************************************/
void path_add_point(MapArea *map_area, double lon, double lat)
{
	LonLatPair * llp = malloc(sizeof(LonLatPair));
	llp -> lon = lon;
	llp -> lat = lat;
	map_area -> path = g_list_append(map_area -> path, llp);
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_PATH_CHANGED], 0);
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

/****************************************************************************************************
* remove the last point from the distance-path
****************************************************************************************************/
void map_area_path_remove_point(MapArea *map_area)
{
	if (g_list_length(map_area -> path) > 1){
		GList * last = g_list_last(map_area -> path);
		free(last->data);
		map_area -> path = g_list_remove_link(map_area -> path, last);
		g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_PATH_CHANGED], 0);
		gtk_widget_queue_draw(GTK_WIDGET(map_area));
	}
}

/****************************************************************************************************
* remove all points from the distance-path
****************************************************************************************************/
void map_area_path_clear(MapArea *map_area)
{
	
	GList * node = map_area -> path -> next;
	while(node != NULL){
		free(node -> data);
		map_area -> path = g_list_remove_link(map_area -> path, node);
		node = map_area -> path -> next;
	}
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_PATH_CHANGED], 0);
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

/****************************************************************************************************
* add a marker to the area
* TODO: this has to be adjusted for use by poi-editing
****************************************************************************************************/
void map_area_add_marker(MapArea *map_area, double lon, double lat)
{
	/*IdAndName * id_name = malloc(sizeof(IdAndName));
	id_name -> id = map_area -> next_marker_id ++;
	id_name -> name = malloc(sizeof(char));
	strcpy(id_name -> name, "");
	poi_set_add(map_area -> poi_set, lon, lat, (void*)id_name);
	gtk_widget_queue_draw(GTK_WIDGET(map_area));*/
}

/****************************************************************************************************
*****************************************************************************************************
* CALLBACKS
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* when a key is pressed while map has focus
****************************************************************************************************/
static gboolean key_press_cb(MapArea *map_area, GdkEventKey *event)
{
	//printf("%d\n", event->keyval); //6536x
	switch (event->keyval){
		case 65361: map_area_move(map_area, DIRECTION_LEFT); break;
		case 65362: map_area_move(map_area, DIRECTION_TOP); break;
		case 65363: map_area_move(map_area, DIRECTION_RIGHT); break;
		case 65364: map_area_move(map_area, DIRECTION_DOWN); break;
		case 65365: map_area_zoom_in(map_area); break;
		case 65366: map_area_zoom_out(map_area); break;
	}
	return FALSE;
}

/****************************************************************************************************
* when the mouse is scrolled while map has focus
****************************************************************************************************/
static gboolean scroll_cb(MapArea *map_area, GdkEventScroll *event)
{
	switch (event -> direction){
		case GDK_SCROLL_UP: map_area_zoom_in(map_area); break;
		case GDK_SCROLL_DOWN: map_area_zoom_out(map_area); break;
	}
}

/****************************************************************************************************
* when a tile has been loaded, repaint the area
****************************************************************************************************/
static gboolean tile_loaded_cb(TileManager * tile_manager, MapArea *map_area)
{
	//printf("received a repaint signal\n");
	map_area -> need_repaint = TRUE;
	gdk_threads_enter();
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
	gdk_threads_leave();
}



/****************************************************************************************************
* when the mouse has been clicked on the map-widget
****************************************************************************************************/
static gboolean mouse_button_cb(GtkWidget *widget, GdkEventButton *event)
{
	MapArea *map_area = GOSM_MAP_AREA(widget);
	gtk_widget_grab_focus(widget);
	double lon = map_area_x_to_lon(map_area, (int) event -> x);
	double lat = map_area_y_to_lat(map_area, (int) event -> y);
	//printf("button %f %f\n", lon, lat);
	map_area -> point_drag.x = (int) event -> x;
	map_area -> point_drag.y = (int) event -> y;
	if (event -> type == GDK_2BUTTON_PRESS){
		if (event -> button == 1){
			map_area_zoom_in(map_area);
		}
		if (event -> button == 3){
			map_area_zoom_out(map_area);
		}
	}
	// TODO: put action_state (== gosm.c.CURSOR) somewhere else
	if (event -> type == GDK_BUTTON_PRESS){
		if (map_area -> mouse_mode == MAP_MODE_MOVE){
			if (map_area -> poi_active_id != 0){
				poi_manager_print_node_information(map_area -> poi_manager, map_area -> poi_active_id);
				LonLatTags * llt = poi_manager_get_node(map_area -> poi_manager, map_area -> poi_active_id);
				g_signal_emit (map_area, map_area_signals[MAP_NODE_SELECTED], 0, (gpointer)llt);
			}
		}else if (map_area -> mouse_mode == MAP_MODE_PATH){
			path_add_point(map_area, lon, lat);
		}else if(map_area -> mouse_mode == MAP_MODE_POI){
			map_area_add_marker(map_area, lon, lat);
		}
	}
}

/****************************************************************************************************
* when the mouse has been moved around on the map-widget
* TODO: this function could be split up into multiple functions
****************************************************************************************************/
static gboolean mouse_motion_cb(GtkWidget *widget, GdkEventMotion *event)
{
	MapArea *map_area = GOSM_MAP_AREA(widget);
	MapPosition *map_position = &(map_area -> map_position);
	// NAVIGATION mode
	if (map_area -> mouse_mode == MAP_MODE_MOVE){
		if (MATCHES(event->state, GDK_BUTTON1_MASK)){
			//printf("motion %d\n", event->state);
			//printf("x: %d, y: %d\n", (int)event->x, (int)event->y);
			int diff_x = ((int) event -> x) - map_area -> point_drag.x;
			int diff_y = ((int) event -> y) - map_area -> point_drag.y;
			map_area -> point_drag.x = (int) event -> x;
			map_area -> point_drag.y = (int) event -> y;
			//printf("motion %d %d\n", diff_x, diff_y);
			map_position -> tile_offset_x -= diff_x;
			map_position -> tile_offset_y -= diff_y;
			while (map_position -> tile_offset_x >= TILESIZE){
				map_position -> tile_offset_x -= TILESIZE;
				map_position -> tile_left += 1;
			}
			while (map_position -> tile_offset_x < 0){
				map_position -> tile_offset_x += TILESIZE;
				map_position -> tile_left -= 1;
			}
			while (map_position -> tile_offset_y >= TILESIZE){
				map_position -> tile_offset_y -= TILESIZE;
				map_position -> tile_top += 1;
			}
			while (map_position -> tile_offset_y < 0){
				map_position -> tile_offset_y += TILESIZE;
				map_position -> tile_top -= 1;
			}
			//printf("offset now: %d\n", map_position -> tile_offset_x);
			map_has_moved(map_area);
			gtk_widget_queue_draw(widget);
		}else{
			// NORMAL mode + mouse not pressed
			double lon = map_area_x_to_lon(map_area, (int) event -> x);
			double lat = map_area_y_to_lat(map_area, (int) event -> y);
			double squaresize = map_area_get_poi_square_size(map_area);
			double lon1 = map_area_x_to_lon(map_area, (int) (event -> x - squaresize / 2));
			double lat1 = map_area_y_to_lat(map_area, (int) (event -> y + squaresize / 2));
			double lon2 = map_area_x_to_lon(map_area, (int) (event -> x + squaresize / 2));
			double lat2 = map_area_y_to_lat(map_area, (int) (event -> y - squaresize / 2));
			//printf("%f %f %f %f\n", lon1, lon2, lat1, lat2);
			
			int num_poi_sets = poi_manager_get_number_of_poi_sets(map_area -> poi_manager);
			int poi;
			int new_active_id = 0;
			int old_active_id = map_area -> poi_active_id;
			for (poi = 0; poi < num_poi_sets; poi++){
				PoiSet * poi_set = GOSM_POI_SET(poi_manager_get_poi_set(map_area -> poi_manager, poi));
				if (poi_set_get_visible(poi_set)){
					GArray * points = poi_set_get(poi_set, lon1, lat1, lon2, lat2);
					if (points -> len > 0){
						new_active_id = g_array_index(points, LonLatPairId, 0).node_id;
					}
					g_array_free(points, TRUE);
				}
			}
			map_area -> poi_active_id = new_active_id;
			if (new_active_id != old_active_id){
				map_area_repaint(map_area);		
			}
			//printf("%d %f\n", marker_count, event -> x);
		}
	}
	// SELECTION mode
	if (map_area -> mouse_mode == MAP_MODE_SELECT){
		/* RESCALE */
		Selection selection = map_area -> selection;
		int s_width = selection.x2 - selection.x1;
		int s_height = selection.y2 - selection.y1;
		int w = s_width / PAD_FRACTION > MAX_PAD_SIZE ? MAX_PAD_SIZE : s_width / PAD_FRACTION;
		int h = s_height / PAD_FRACTION > MAX_PAD_SIZE ? MAX_PAD_SIZE : s_height / PAD_FRACTION;
		int x = (int)event->x;
		int y = (int)event->y;
		int mouseover = 0;
		// if mouse is not pressed:
		// check if pointer is over one of the pads
		if (!MATCHES(event->state, GDK_BUTTON1_MASK)){
			if (y >= selection.y1 && y <= selection.y1 + h && x >= selection.x1 && x <= selection.x1 + w){
				mouseover = DIRECTION_TOP_LEFT;
			}
			if (y >= selection.y1 && y <= selection.y1 + h && x >= selection.x2 - w && x <= selection.x2){
				mouseover = DIRECTION_TOP_RIGHT;
			}
			if (y >= selection.y2 - h && y <= selection.y2 && x >= selection.x1 && x <= selection.x1 + w){
				mouseover = DIRECTION_DOWN_LEFT;
			}
			if (y >= selection.y2 - h && y <= selection.y2 && x >= selection.x2 - w && x <= selection.x2){
				mouseover = DIRECTION_DOWN_RIGHT;
			}
			if (y >= selection.y1 + h && y <= selection.y2 - h){
				if (x >= selection.x1 && x <= selection.x1 + w){
					mouseover = DIRECTION_LEFT;
				}
				if (x <= selection.x2 && x >= selection.x2 - w){
					mouseover = DIRECTION_RIGHT;
				}
			}
			if (x >= selection.x1 + w && x <= selection.x2 - w){
				if (y >= selection.y1 && y <= selection.y1 + h){
					mouseover = DIRECTION_TOP;
				}
				if (y <= selection.y2 && y >= selection.y2 - h){
					mouseover = DIRECTION_DOWN;
				}
			}
			if (map_area -> selection_mouseover != mouseover){
				map_area -> selection_mouseover = mouseover;
				gtk_widget_queue_draw(widget);
			}
		}
		// if mouse is pressed:
		// check how selection pad has been dragged
		if (MATCHES(event->state, GDK_BUTTON1_MASK)){
			int mo = map_area -> selection_mouseover;
			if (mo == DIRECTION_LEFT || mo == DIRECTION_TOP_LEFT || mo == DIRECTION_DOWN_LEFT){
				int diff = x - map_area -> point_drag.x;
				if (selection.x1 + diff < selection.x2){
					map_area -> selection.x1 += diff;
					map_area -> selection.lon1 = map_area_x_to_lon(map_area, map_area -> selection.x1),
					map_area -> point_drag.x = x;
					g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_SELECTION_CHANGED], 0);
					gtk_widget_queue_draw(widget);
				}
			}
			if (mo == DIRECTION_RIGHT || mo == DIRECTION_TOP_RIGHT || mo == DIRECTION_DOWN_RIGHT){
				int diff = x - map_area -> point_drag.x;
				if (selection.x2 + diff > selection.x1){
					map_area -> selection.x2 += diff;
					map_area -> selection.lon2 = map_area_x_to_lon(map_area, map_area -> selection.x2),
					map_area -> point_drag.x = x;
					g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_SELECTION_CHANGED], 0);
					gtk_widget_queue_draw(widget);
				}
			}
			if (mo == DIRECTION_TOP || mo == DIRECTION_TOP_LEFT || mo == DIRECTION_TOP_RIGHT){
				int diff = y - map_area -> point_drag.y;
				if (selection.y1 + diff < selection.y2){
					map_area -> selection.y1 += diff;
					map_area -> selection.lat1 = map_area_y_to_lat(map_area, map_area -> selection.y1),
					map_area -> point_drag.y = y;
					g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_SELECTION_CHANGED], 0);
					gtk_widget_queue_draw(widget);
				}
			}
			if (mo == DIRECTION_DOWN || mo == DIRECTION_DOWN_LEFT || mo == DIRECTION_DOWN_RIGHT){
				int diff = y - map_area -> point_drag.y;
				if (selection.y2 + diff > selection.y1){
					map_area -> selection.y2 += diff;
					map_area -> selection.lat2 = map_area_y_to_lat(map_area, map_area -> selection.y2),
					map_area -> point_drag.y = y;
					g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_SELECTION_CHANGED], 0);
					gtk_widget_queue_draw(widget);
				}
			}
		}
		/* RESCALE end */
		/* SELECT */
		// if mouse is not over a pad and mouse is pressed:
		// set selection to:
		// (last press position) to (current pointer position)
		if (map_area -> selection_mouseover == 0 && MATCHES(event->state, GDK_BUTTON1_MASK)){
			if (map_area -> point_drag.x > (int) event -> x){	
				map_area -> selection.x1 = (int) event -> x;
				map_area -> selection.x2 = map_area -> point_drag.x;
			}else{	
				map_area -> selection.x2 = (int) event -> x;
				map_area -> selection.x1 = map_area -> point_drag.x;
			}
			if (map_area -> point_drag.y > (int) event -> y){
				map_area -> selection.y1 = (int) event -> y;
				map_area -> selection.y2 = map_area -> point_drag.y;
			}else{
				map_area -> selection.y2 = (int) event -> y;
				map_area -> selection.y1 = map_area -> point_drag.y;
			}
			map_area -> selection.lon1 = map_area_x_to_lon(map_area, map_area -> selection.x1),
			map_area -> selection.lon2 = map_area_x_to_lon(map_area, map_area -> selection.x2),
			map_area -> selection.lat1 = map_area_y_to_lat(map_area, map_area -> selection.y1),
			map_area -> selection.lat2 = map_area_y_to_lat(map_area, map_area -> selection.y2);

			g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_SELECTION_CHANGED], 0);
			gtk_widget_queue_draw(widget);
		}
		/* SELECT end */
	}
}

/****************************************************************************************************
* when the expose event has been sent to the map-widget
* TODO: this function could be split up into multiple function
****************************************************************************************************/
static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event)
{
	//printf("function map_area.expose_cb\n");
	MapArea *map_area = GOSM_MAP_AREA(widget);
	MapPosition *map_position = &(map_area -> map_position);
	/* recreate == true <=> first exposure OR widget-size changed */
	gboolean recreate = map_area -> pixmap == NULL
				|| widget -> allocation.width != map_position -> width
				|| widget -> allocation.height != map_position -> height;
	if (recreate){
		/* store new size */
		map_position -> width  = widget -> allocation.width;
		map_position -> height = widget -> allocation.height;
		map_area_adjust_map_position_values(map_area);
		/* throw away the old pixmap, if exists */
		if (map_area -> pixmap != NULL) g_object_unref(map_area -> pixmap);
		map_area -> pixmap = gdk_pixmap_new(widget->window,
				widget->allocation.width,
				widget->allocation.height, 24);
		map_has_moved(map_area);
	}
	map_area_adjust_tile_count_information(map_area);
	//printf("visible tiles: %d\n", map_position -> tile_count_x * map_position -> tile_count_y);
	/* paint into the pixmap-buffer */
	if(map_area -> map_moved || map_area -> need_repaint){
		map_area -> map_moved = FALSE;
		map_area -> need_repaint = FALSE;
		/* draw white background */
		gdk_draw_rectangle(map_area -> pixmap,
				widget->style->white_gc,
				TRUE, 0, 0, 
				widget -> allocation.width,
				widget -> allocation.height);
		/* draw tiles */
		int x, y = 0; int t_x, t_y; int c;
		for (x = 0; x < map_position -> tile_count_x; x++){
		  for(y = 0; y < map_position -> tile_count_y; y++){
			GdkPixbuf *pixbuf;
			GdkPixmap *pixmap;
			t_x = map_position -> tile_left + x;
			t_y = map_position -> tile_top  + y;
			gpointer pointer = tile_manager_request_tile(
				map_area -> tile_manager[map_area -> tileset],
				t_x, t_y, map_position -> zoom); 
			gboolean in_cache = pointer != NULL;
			if (in_cache){
				pixmap = GDK_PIXMAP(pointer);
			}else{
				//printf("CACHE MISS\n");
				/* if the empty pic has not been initialised */
				if (map_area -> no_pic == NULL){
					map_area -> no_pic = gdk_pixmap_new(
						NULL, 256, 256, 24);
					GdkVisual * visual = gdk_visual_get_best_with_depth(24);
					GdkColormap * colormap = gdk_colormap_new(visual, TRUE);
					gdk_drawable_set_colormap(map_area -> no_pic, colormap);
					gdk_draw_rectangle(map_area -> no_pic,
						widget->style->white_gc,
						TRUE, 0, 0, TILESIZE, TILESIZE);
				}
				pixmap = map_area -> no_pic;
			}
			gdk_draw_drawable(map_area -> pixmap, widget->style->black_gc, pixmap,
				0, 0,
				TILESIZE * x - map_position -> tile_offset_x,
				TILESIZE * y - map_position -> tile_offset_y,
				TILESIZE, TILESIZE);
			/* DRAW_GRID */
			if (map_area -> show_grid){
				gdk_draw_rectangle(map_area -> pixmap, widget->style->black_gc,
					FALSE,
					TILESIZE * x - map_position -> tile_offset_x,
					TILESIZE * y - map_position -> tile_offset_y,
					256, 256);
			}
			/* DRAW_TILE_NAMES */
			if (map_area -> show_font){
				char text[20];
				//TODO: use pango instead
				sprintf(text, "%d_%d_%d", map_position -> zoom, t_x, t_y);
				GdkFont *font = gdk_font_load("-*-terminal-*-*-*-*-14-*-*-*-*-*-*-*");
				gdk_draw_text(map_area -> pixmap, font, widget->style->black_gc,
					TILESIZE * x - map_position -> tile_offset_x + 10,
					TILESIZE * y - map_position -> tile_offset_y + 20,
					text, strlen(text));
				gdk_font_unref(font);
			}
		  } /* for y */
		} /* for x */
	} /* endif recreate */
	/* draw buffer-pixmap to screen */
	gdk_draw_drawable(widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		map_area -> pixmap,
		event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
	/* DRAW SELECTION */
	if (map_area -> show_selection){
		/* fill */
		cairo_t * cr_sel = gdk_cairo_create(widget->window);
		ColorQuadriple * c = &(map_area -> color_selection);
		cairo_pattern_t * pat_sel = cairo_pattern_create_rgba(c -> r, c -> g, c -> b, c -> a);
		cairo_set_source(cr_sel, pat_sel);
		cairo_rectangle(cr_sel,
			map_area -> selection.x1,
			map_area -> selection.y1, 
			map_area -> selection.x2 - map_area -> selection.x1,
			map_area -> selection.y2 - map_area -> selection.y1);
		cairo_fill(cr_sel);
		/* outline */
		c = &(map_area -> color_selection_out);
		cairo_pattern_destroy(pat_sel);
		pat_sel = cairo_pattern_create_rgba(c -> r, c -> g, c -> b, c -> a);
		cairo_set_source(cr_sel, pat_sel);
		cairo_rectangle(cr_sel,
			map_area -> selection.x1,
			map_area -> selection.y1, 
			map_area -> selection.x2 - map_area -> selection.x1,
			map_area -> selection.y2 - map_area -> selection.y1);
		cairo_set_line_width(cr_sel, 1.0);
		cairo_stroke(cr_sel);
		cairo_pattern_destroy(pat_sel);
		/* slicing */
		if (map_area -> show_slice){
			int z_diff = map_position -> zoom - map_area -> slice_zl;
			double z_factor = pow(2, z_diff);
			int z_slice_x = map_area -> slice_x * z_factor;
			int z_slice_y = map_area -> slice_y * z_factor;
			int z_slice_intersect_x = map_area -> slice_intersect_x * z_factor;
			int z_slice_intersect_y = map_area -> slice_intersect_y * z_factor;
			int sel_width  = map_area -> selection.x2 - map_area -> selection.x1;
			int sel_height = map_area -> selection.y2 - map_area -> selection.y1;
			int parts_x = 1 + ceil(((double)(sel_width  - z_slice_x)) / (z_slice_x - z_slice_intersect_x));
			int parts_y = 1 + ceil(((double)(sel_height - z_slice_y)) / (z_slice_y - z_slice_intersect_y));
			parts_x = parts_x > 0 ? parts_x : 1;
			parts_y = parts_y > 0 ? parts_y : 1;
			//TODO: more intelligent painting of squares
			//	draw grid-lines instead of rectangles
			if (parts_x * parts_y < 1000){
				//printf("slices: %d %d\n", parts_x, parts_y);
				ColorQuadriple * c = &(map_area -> color_atlas_lines);
				pat_sel = cairo_pattern_create_rgba(c -> r, c -> g, c -> b, c -> a);
				cairo_set_source(cr_sel, pat_sel);
				int s_x, s_y;
				for(s_x = 0; s_x < parts_x; s_x++){
					for(s_y = 0; s_y < parts_y; s_y++){
						cairo_rectangle(cr_sel,
							map_area -> selection.x1 + s_x * (z_slice_x - z_slice_intersect_x),
							map_area -> selection.y1 + s_y * (z_slice_y - z_slice_intersect_y), 
							z_slice_x,
							z_slice_y);
					}
				}
				cairo_set_line_width(cr_sel, 1.0);
				cairo_stroke(cr_sel);
				cairo_pattern_destroy(pat_sel);
			}
		}

		/* RESIZE PADS */	
		Selection selection = map_area -> selection;
		int s_width = selection.x2 - selection.x1;
		int s_height = selection.y2 - selection.y1;
		int w = s_width / PAD_FRACTION > MAX_PAD_SIZE ? MAX_PAD_SIZE : s_width / PAD_FRACTION;
		int h = s_height / PAD_FRACTION > MAX_PAD_SIZE ? MAX_PAD_SIZE : s_height / PAD_FRACTION;
		if (map_area -> selection_mouseover != 0){
			ColorQuadriple * c = &(map_area -> color_selection_pad);
			pat_sel = cairo_pattern_create_rgba(c -> r, c -> g, c -> b, c -> a);
			cairo_set_source(cr_sel, pat_sel);
			switch(map_area -> selection_mouseover){
			case(DIRECTION_TOP_LEFT):{
				cairo_rectangle(cr_sel, selection.x1, selection.y1, w, h);
				break;
			}
			case(DIRECTION_TOP_RIGHT):{
				cairo_rectangle(cr_sel, selection.x2 - w, selection.y1, w, h);
				break;
			}
			case(DIRECTION_DOWN_LEFT):{
				cairo_rectangle(cr_sel, selection.x1, selection.y2 - h, w, h);
				break;
			}
			case(DIRECTION_DOWN_RIGHT):{
				cairo_rectangle(cr_sel, selection.x2 - w, selection.y2 - h, w, h);
				break;
			}
			case(DIRECTION_LEFT):{
				cairo_rectangle(cr_sel, selection.x1, selection.y1 + h, w, s_height - 2 * h );
				break;
			}
			case(DIRECTION_RIGHT):{
				cairo_rectangle(cr_sel, selection.x2 - w, selection.y1 + h, w, s_height - 2 * h );
				break;
			}
			case(DIRECTION_TOP):{
				cairo_rectangle(cr_sel, selection.x1+w, selection.y1, s_width - 2 * w, h );
				break;
			}
			case(DIRECTION_DOWN):{
				cairo_rectangle(cr_sel, selection.x1+w, selection.y2 - h, s_width - 2 * w, h );
				break;
			}
			}
			cairo_fill(cr_sel);
		}
		cairo_destroy(cr_sel);
		/* RESIZE PADS end */
	}
	/* DRAW PATH */
	int last_x, last_y; gboolean first = TRUE;
	GList * node = map_area -> path -> next;
	cairo_t * cr_lines = gdk_cairo_create(widget->window);
	cairo_set_line_width(cr_lines, 1.5);
	cairo_t * cr_arcs = gdk_cairo_create(widget->window);
	while(node != NULL){
		LonLatPair * ll = (LonLatPair*) node -> data;
		int x = map_area_lon_to_area_x(map_area, ll -> lon);
		int y = map_area_lat_to_area_y(map_area, ll -> lat);
		if (first){
			first = FALSE;
			cairo_move_to(cr_lines, x, y);
		}else{
			cairo_line_to(cr_lines, x, y);
		}
		cairo_move_to(cr_arcs, x, y);
		cairo_arc(cr_arcs, x, y, 3, 0.0, 2 * 3.1415927);
		node = node -> next;
	}
	cairo_pattern_t * pat_lines = cairo_pattern_create_rgb(0.55, 0.15, 0.15);
	cairo_set_source(cr_lines, pat_lines);
	cairo_stroke(cr_lines);
	cairo_pattern_t * pat_arcs = cairo_pattern_create_rgb(0.85, 0.25, 0.25);
	cairo_set_source(cr_arcs, pat_arcs);
	cairo_fill(cr_arcs);
	cairo_destroy(cr_lines);
	cairo_destroy(cr_arcs);
	//gettimeofday(&t1, NULL);
	/* DRAW MARKERS */
	if (map_area -> icon_marker == NULL){
		map_area -> icon_marker = cairo_image_surface_create_from_png(GOSM_ICON_DIR "point.png");
	}
	double min_lon, min_lat, max_lon, max_lat;
	map_area_get_visible_area(map_area, &min_lon, &min_lat, &max_lon, &max_lat);
	int num_poi_sets = poi_manager_get_number_of_poi_sets(map_area -> poi_manager);
	int poi;
	int active_x, active_y;
	double active_r, active_g, active_b, active_a;
	int n_active = -1;
	int p_active = -1;
	//IdAndName * id_name_active;
	
	/* adjust visible size */
	double square_size = map_area_get_poi_square_size(map_area);
	double square_size_half = square_size / 2;
	/* iterate poi sets */
	for (poi = 0; poi < num_poi_sets; poi++){
		StyledPoiSet * poi_set = poi_manager_get_poi_set(map_area -> poi_manager, poi);
		if (poi_set_get_visible(GOSM_POI_SET(poi_set))){
			GArray * points = poi_set_get(
				GOSM_POI_SET(poi_set), min_lon, min_lat, max_lon, max_lat);
			cairo_t * cr_marker = gdk_cairo_create(widget->window);
//			cairo_pattern_t * pat_dots = cairo_pattern_create_rgba(0.85, 0.25, 0.25, 0.9);
			cairo_pattern_t * pat_dots = cairo_pattern_create_rgba(
				poi_set -> r, poi_set -> g, poi_set -> b, poi_set -> a);
			cairo_set_source(cr_marker, pat_dots);
			/* n_active is the one, where mouse is over */
			int p;
			//printf("%d\n", map_area -> poi_active_id);
			for (p = 0; p < points -> len; p++){
				LonLatPairId * llpi = &g_array_index(points, LonLatPairId, p);
				int x = map_area_lon_to_area_x(map_area, llpi -> lon);
				int y = map_area_lat_to_area_y(map_area, llpi -> lat);
				cairo_move_to(cr_marker, x, y);
				if (llpi -> node_id != map_area -> poi_active_id){
					cairo_rectangle(cr_marker, x-square_size_half, y-square_size_half, square_size, square_size); 
					//cairo_arc(cr_marker, x, y, 3, 0.0, 2 * 3.1415927);
					//cairo_set_source_surface(cr_marker, map_area -> icon_marker, x - 12, y - 12);
					//cairo_rectangle(cr_marker, x-12, y-12, 24, 24);
					//cairo_paint(cr_marker);
				}else{
					n_active = p;
					p_active = poi;
					//id_name_active = (IdAndName*) points[p].data;
					map_area -> poi_active_id = llpi -> node_id;
					active_x = x;
					active_y = y;
					active_r = poi_set -> r;
					active_g = poi_set -> g;
					active_b = poi_set -> b;
					active_a = poi_set -> a;
				}
			}
			cairo_fill(cr_marker);
			/* n_active == -1 means: mouse not over any marker */
			g_array_free(points, TRUE);
			cairo_pattern_destroy(pat_dots);
			cairo_destroy(cr_marker);
		}
	}
	if (n_active >= 0){
		LonLatTags * llt = (LonLatTags*) g_tree_lookup(map_area -> poi_manager -> tree_ids, &(map_area -> poi_active_id));
		char * name = g_hash_table_lookup(llt -> tags, "name");
		if (name == NULL){
			name = "";
		}
		cairo_t * cr_marker = gdk_cairo_create(widget->window);
		cairo_pattern_t * pat_dots = cairo_pattern_create_rgba(active_r, active_g, active_b, active_a);
		cairo_set_source(cr_marker, pat_dots);
		int p = n_active;
		int x = active_x;
		int y = active_y;
		cairo_move_to(cr_marker, x, y);
		cairo_rectangle(cr_marker, x-square_size, y-square_size, square_size * 2, square_size * 2);
		cairo_fill(cr_marker);
		cairo_pattern_destroy(pat_dots);

		int layout_width = 250;
		cairo_t * cr_font = gdk_cairo_create(widget->window);
		PangoContext * pc_marker = pango_cairo_create_context(cr_font);
		PangoLayout * pl_marker = pango_layout_new(pc_marker);
		pango_layout_set_text(pl_marker, name, -1);
		PangoAttrList * attrs = pango_attr_list_new();
		PangoAttribute * weight = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
		PangoAttribute * size = pango_attr_size_new(PANGO_SCALE * 14);
		pango_attr_list_insert(attrs, weight);
		pango_attr_list_insert(attrs, size);
		pango_layout_set_attributes(pl_marker, attrs);
		pango_layout_set_width(pl_marker, PANGO_SCALE * layout_width);
		pango_layout_set_alignment(pl_marker, PANGO_ALIGN_CENTER);

		PangoRectangle rect1, rect2;
		pango_layout_get_pixel_extents(pl_marker, &rect1, &rect2);

		cairo_move_to(cr_font, x - layout_width/2, y - square_size - 2 - rect2.height);
		cairo_set_line_width(cr_font, 4.0);
		pango_cairo_layout_path(cr_font, pl_marker);
		cairo_pattern_t * pat_font = cairo_pattern_create_rgba(0.99, 0.99, 0.99, 0.9);
		cairo_set_source(cr_font, pat_font);
		cairo_stroke(cr_font);
		cairo_pattern_destroy(pat_font);
		pat_font = cairo_pattern_create_rgba(0.15, 0.15, 0.15, 0.9);
		cairo_set_source(cr_font, pat_font);
		cairo_move_to(cr_font, x - layout_width/2, y - square_size - 2 - rect2.height);
		pango_cairo_show_layout(cr_font, pl_marker);
		cairo_fill(cr_font);
		cairo_destroy(cr_font);
		cairo_pattern_destroy(pat_font);
		g_object_unref(pc_marker);
		g_object_unref(pl_marker);
		pango_attr_list_unref(attrs);
		cairo_fill(cr_marker);
		cairo_destroy(cr_marker);
	}
	//gettimeofday(&t2, NULL);
	//printf("time %d\n", time_diff(&t1, &t2));
	/* cairo_t * cr_font = gdk_cairo_create(widget->window);
	cairo_pattern_t * pat_font = cairo_pattern_create_rgba(0.85, 0.25, 0.25, 0.9);
	cairo_set_source(cr_font, pat_font);
	cairo_move_to(cr_font, 0, 0);
	PangoContext * pc_marker = pango_cairo_create_context(cr_font);
	PangoLayout * pl_marker = pango_layout_new(pc_marker);
	pango_layout_set_markup(pl_marker, "<b>FOO Bar</b>", -1);
	pango_cairo_show_layout(cr_font, pl_marker);
	cairo_fill(cr_font); */
	return TRUE;
}

