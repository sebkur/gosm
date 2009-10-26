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

#ifndef _MAP_AREA_H_
#define _MAP_AREA_H_

typedef struct _MapArea		MapArea;
typedef struct _MapAreaClass	MapAreaClass;

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <glib.h>
#include "map_types.h"
#include "tiles/tile_manager.h"
#include "tiles/tilesets.h"
#include "poi/poi_set.h"
#include "poi/poi_manager.h"

#define GOSM_TYPE_MAP_AREA		(map_area_get_type ())
#define GOSM_MAP_AREA(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_MAP_AREA, MapArea))
#define GOSM_MAP_AREA_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_MAP_AREA, MapAreaClass))
#define GOSM_IS_MAP_AREA(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_MAP_AREA))
#define GOSM_IS_MAP_AREA_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_MAP_AREA))
#define GOSM_MAP_AREA_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_MAP_AREA, MapAreaClass))

#define DIRECTION_TOP_LEFT      1
#define DIRECTION_TOP           2
#define DIRECTION_TOP_RIGHT     3
#define DIRECTION_LEFT          4
#define DIRECTION_RIGHT         5
#define DIRECTION_DOWN_LEFT     6
#define DIRECTION_DOWN          7
#define DIRECTION_DOWN_RIGHT    8

typedef enum{
	MAP_MODE_MOVE = 0,
	MAP_MODE_SELECT,
	MAP_MODE_PATH,
	MAP_MODE_POI,
	MAP_MODE_COUNT
} MouseMode;

struct _MapArea
{
	GtkDrawingArea parent;

	GdkPixmap * pixmap;
	GdkPixmap * no_pic;
	MapPosition map_position;
	Selection selection;
	GList * path;
	MouseMode mouse_mode;
	gboolean show_grid;
	gboolean show_font;
	gboolean show_selection;
	gboolean snap_selection;

	gboolean show_slice;
	int slice_zl;
	int slice_x;
	int slice_y;
	int slice_intersect_x;
	int slice_intersect_y;

	gboolean need_repaint;
	gboolean map_moved;
	Point point_drag;

	Tileset tileset;
	TileManager * tile_manager[TILESET_LAST];
	char * cache_dir[TILESET_LAST];

	int selection_mouseover;
	ColorQuadriple color_selection;
	ColorQuadriple color_selection_out;
	ColorQuadriple color_selection_pad;
	ColorQuadriple color_atlas_lines;

	PoiManager * poi_manager;
	int next_marker_id;
	int poi_active_id;
	cairo_surface_t * icon_marker;
};

struct _MapAreaClass
{
	GtkDrawingAreaClass parent_class;

	void (* map_been_moved) (MapArea *map_area);

	void (* map_zoom_changed) (MapArea *map_area);

	void (* map_tileset_changed) (MapArea *map_area);

	void (* map_selection_changed) (MapArea *map_area);
	
	void (* map_path_changed) (MapArea *map_area);

	void (* map_node_selected) (MapArea *map_area, gpointer llt_p);

	void (* map_mouse_mode_changed) (MapArea *map_area);

	void (* map_network_state_changed) (MapArea *map_area);

	void (* map_grid_toggled) (MapArea *map_area);

	void (* map_font_toggled) (MapArea *map_area);
};

GtkWidget * map_area_new();

void map_area_set_mouse_mode(MapArea *map_area, MouseMode mouse_mode);
MouseMode map_area_get_mouse_mode(MapArea *map_area);

void map_area_set_tileset(MapArea *map_area, Tileset tileset);
Tileset map_area_get_tileset(MapArea *map_area);

void map_area_set_cache_directory(MapArea *map_area, Tileset tileset, char * directory);

void map_area_move(MapArea *map_area, int direction);
void map_area_zoom_in(MapArea *map_area);
void map_area_zoom_out(MapArea *map_area);

void map_area_repaint(MapArea *map_area);

double map_area_position_get_center_lon(MapArea *map_area);
double map_area_position_get_center_lat(MapArea *map_area);

int map_area_get_zoom(MapArea *map_area);

void map_area_goto_lon_lat_zoom(MapArea *map_area, double lon, double lat, int zoom);
void map_area_goto_bbox(MapArea *map_area, double lon1, double lat1, double lon2, double lat2);

void map_area_set_network_state(MapArea * map_area, gboolean state);
gboolean map_area_get_network_state(MapArea * map_area);

void map_area_path_remove_point(MapArea *map_area);
void map_area_path_clear(MapArea *map_area);

void map_area_add_marker(MapArea *map_area, double lon, double lat);

void map_area_set_color_selection(MapArea *map_area, ColorQuadriple c_s, ColorQuadriple c_s_out, ColorQuadriple c_s_pad, ColorQuadriple c_a_lines);

TileManager * map_area_get_tile_manager(MapArea *map_area, Tileset tileset);

void map_area_set_poi_manager(MapArea *map_area, PoiManager *poi_manager);

void map_area_get_visible_area(MapArea * map_area, double *min_lon, double *min_lat, double *max_lon, double *max_lat);

void map_area_set_show_grid(MapArea * map_area, gboolean show);
gboolean map_area_get_show_grid(MapArea * map_area);

void map_area_set_show_font(MapArea * map_area, gboolean show);
gboolean map_area_get_show_font(MapArea * map_area);

#endif
