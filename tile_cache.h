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

#include "ringbuffer.h"

#ifndef _TILE_CACHE_H_
#define _TILE_CACHE_H_

#ifndef _MAP_TILE_
#define _MAP_TILE_

typedef struct MapTile {
        int x;
        int y;
        int zoom;
} MapTile;

typedef struct CachedMapTile {
        MapTile map_tile_info;
        gpointer pixbuf;
} CachedMapTile;

#endif

typedef struct TileCache {
	GList ** table;
	RingBuffer * ring_buffer;
} TileCache;

typedef struct list_x {
	int x;
	GList * list;
} list_x;

typedef struct node_y {
	int y;
	gpointer pixbuf;
} node_y;

gint compare_list_x_int(gconstpointer a, gconstpointer b);

gint compare_list_x_list_x(gconstpointer a, gconstpointer b);

gint compare_node_y_int(gconstpointer a, gconstpointer b);

gint compare_node_y_node_y(gconstpointer a, gconstpointer b);

TileCache * cache_new(int cache_size);

gpointer cache_find_tile(TileCache * cache, int zoom, int x, int y);

gboolean cache_add_tile(TileCache * cache, int zoom, int x, int y, gpointer pixbuf);

void cache_remove_tile(TileCache * cache, int zoom, int x, int y);

void cache_purge_tile(TileCache * cache, int zoom, int x, int y);

#endif
