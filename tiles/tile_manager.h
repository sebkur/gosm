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

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include <curl/curl.h>

#include "tile_cache.h"
#include "../map_types.h"

#ifndef _TILE_MANAGER_H_
#define _TILE_MANAGER_H_

#define GOSM_TYPE_TILE_MANAGER		(tile_manager_get_type ())
#define GOSM_TILE_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_TILE_MANAGER, TileManager))
#define GOSM_TILE_MANAGER_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TILE_MANAGER, TileManagerClass))
#define GOSM_IS_TILE_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_TILE_MANAGER))
#define GOSM_IS_TILE_MANAGER_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_TILE_MANAGER))
#define GOSM_TILE_MANAGER_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_TILE_MANAGER, TileManagerClass))

typedef struct _TileManager		TileManager;
typedef struct _TileManagerClass	TileManagerClass;

struct _TileManager
{
	GObject parent;
	/* public things? */

	TileCache *tile_cache;
	pthread_mutex_t mutex_tile_cache;

	GArray *load_from_disk;
	MapTile actual_load_from_disk;
	pthread_mutex_t mutex_load_from_disk;
	pthread_cond_t  cond_wait_load_from_disk;
	pthread_mutex_t mutex_wait_load_from_disk;

	GArray *load_from_netw;
	GArray *actual_load_from_netw;
	pthread_mutex_t mutex_load_from_netw;
	pthread_cond_t  cond_wait_load_from_netw;
	pthread_mutex_t mutex_wait_load_from_netw;

	gboolean network_state;
	char * cache_dir;
	char * cache_files_format;
	char * format_url;
	int cache_files_format_len;

	CURL * easyhandle; // TODO: unused now, remove
};

struct _TileManagerClass
{
	GObjectClass parent_class;

	void (* tile_loaded_from_disk) (TileManager *tile_manager);

	void (* tile_loaded_from_netw) (TileManager *tile_manager);
};

GObject * tile_manager_new();

gpointer tile_manager_request_tile(TileManager * tile_manager, int x, int y, int zoom);
void tile_manager_delete_tile(TileManager * tile_manager, int x, int y, int zoom);

void tile_manager_set_network_state(TileManager * tile_manager, gboolean state);
gboolean tile_manager_get_network_state(TileManager * tile_manager);
void tile_manager_set_cache_directory(TileManager * tile_manager, char * directory);
void tile_manager_set_url_format(TileManager * tile_manager, char * format);

#endif
