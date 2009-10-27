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

#include "../tiles/tilesets.h"
#include "../map_types.h"
#include "../tiles/tile_manager.h"
#include "../map_area.h"
#include "wizzard_delete_tiles_window.h"

#define GOSM_TYPE_WIZZARD_DELETE_TILES           (wizzard_delete_tiles_get_type ())
#define GOSM_WIZZARD_DELETE_TILES(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES, WizzardDeleteTiles))
#define GOSM_WIZZARD_DELETE_TILES_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_WIZZARD_DELETE_TILES, WizzardDeleteTilesClass))
#define GOSM_IS_WIZZARD_DELETE_TILES(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES))
#define GOSM_IS_WIZZARD_DELETE_TILES_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES))
#define GOSM_WIZZARD_DELETE_TILES_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES, WizzardDeleteTilesClass))

typedef struct _WizzardDeleteTiles        WizzardDeleteTiles;
typedef struct _WizzardDeleteTilesClass   WizzardDeleteTilesClass;

struct _WizzardDeleteTiles
{
	GObject parent;

	Selection selection;
	MapArea * map_area;
	Tileset tileset;
	WizzardDeleteTilesWindow * wdtw;
};

struct _WizzardDeleteTilesClass
{
	GObjectClass parent_class;

	//void (* function_name) (WizzardDeleteTiles *wizzard_delete_tiles);
};

WizzardDeleteTiles * wizzard_delete_tiles_new(MapArea * map_area, Tileset tileset, Selection selection);

void wizzard_delete_tiles_show(WizzardDeleteTiles * wizzard, GtkWindow * parent);
