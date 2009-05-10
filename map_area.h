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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include "map_types.h"
#include "tile_manager.h"

#define GOSM_TYPE_MAP_AREA		(map_area_get_type ())
#define GOSM_MAP_AREA(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_MAP_AREA, MapArea))
#define GOSM_MAP_AREA_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_MAP_AREA, MapAreaClass))
#define GOSM_IS_MAP_AREA(obj)		(G_TYPE_CHECK_INSTANCE ((obj), GOSM_TYPE_MAP_AREA))
#define GOSM_IS_MAP_AREA_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_MAP_AREA))
#define GOSM_MAP_AREA_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_MAP_AREA, MapAreaClass))

typedef struct _MapArea		MapArea;
typedef struct _MapAreaClass	MapAreaClass;

struct _MapArea
{
	GtkDrawingArea parent;

	GdkPixmap *pixmap;
	int width;
	int height;
	MapPosition map_position;
	Selection selection;
	GList * path;
	int action_state;
	gboolean show_grid;
	gboolean show_font;
	gboolean show_selection;
	gboolean snap_selection;

	gboolean need_repaint;

	TileManager *tile_manager;
	char * cache_dir;

	int selection_mouseover;
};

struct _MapAreaClass
{
	GtkDrawingAreaClass parent_class;

	void (* map_been_moved) (MapArea *map_area);

	void (* map_selection_changed) (MapArea *map_area);
	
	void (* map_path_changed) (MapArea *map_area);
};

GtkWidget * map_area_new();

void map_area_set_cache_directory(MapArea *map_area, char * directory);

// TODO remove: void map_has_moved();

// TODO: remove: void map_load_new_tiles();

void map_area_zoom_in(MapArea *map_area);
void map_area_zoom_out(MapArea *map_area);

void map_area_repaint(MapArea *map_area);

double map_area_position_get_center_lon(MapArea *map_area);
double map_area_position_get_center_lat(MapArea *map_area);

void map_area_goto_lon_lat_zoom(MapArea *map_area, double lon, double lat, int zoom);

void map_area_set_network_state(MapArea * map_area, gboolean state);

void map_area_path_remove_point(MapArea *map_area);
void map_area_path_clear(MapArea *map_area);

#endif
