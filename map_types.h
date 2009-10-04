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

#ifndef _MAP_TYPES_H_
#define _MAP_TYPES_H_

typedef struct MapPosition {
	int width;
	int height;
	int zoom;
	double lon;
	double lat;
	int tile_top;
	int tile_left;
	int tile_offset_y;
	int tile_offset_x;
	int tile_count_x;
	int tile_count_y;
} MapPosition;

typedef struct Selection {
	int x1;
	int x2;
	int y1;
	int y2;

	double lon1;
	double lon2;
	double lat1;
	double lat2;
} Selection;

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

typedef struct Point {
	int x;
	int y;
} Point;

typedef struct LonLatPair {
	double lon;
	double lat;
} LonLatPair;

typedef struct LonLatPairData {
	double lon;
	double lat;
	void * data;
} LonLatPairData;

typedef struct IdAndName {
	int id;
	char * name;
} IdAndName;

typedef struct ColorQuadriple {
	double r;
	double g;
	double b;
	double a;
} ColorQuadriple;
#endif
