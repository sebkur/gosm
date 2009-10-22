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

#include <curl/curl.h>

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include "../map_types.h"

#ifndef _GOSM_TILE_LOADER_H_
#define _GOSM_TILE_LOADER_H_

#define GOSM_TYPE_TILE_LOADER		(tile_loader_get_type ())
#define GOSM_TILE_LOADER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_TILE_LOADER, TileLoader))
#define GOSM_TILE_LOADER_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TILE_LOADER, TileLoaderClass))
#define GOSM_IS_TILE_LOADER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_TILE_LOADER))
#define GOSM_IS_TILE_LOADER_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_TILE_LOADER))
#define GOSM_TILE_LOADER_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_TILE_LOADER, TileLoaderClass))

#define TILE_LOADER_MODE_PAUSE		0
#define TILE_LOADER_MODE_RUN		1

typedef struct _TileLoader	TileLoader;
typedef struct _TileLoaderClass	TileLoaderClass;

struct _TileLoader
{
	GObject parent;
	/* public things? */

	int mode;

	CURL * easyhandle;

	pthread_mutex_t mutex_list;
	pthread_mutex_t mutex_wait;
	pthread_mutex_t mutex_mode;
	pthread_cond_t  cond_wait;
	GArray *list_to_load;

	char * cache_dir;
	char * cache_files_format;
	char * format_url;
	int cache_files_format_len;
};

struct _TileLoaderClass
{
	GObjectClass parent_class;

	void (* tile_loaded_succesfully) (TileLoader *tile_loader);
};

GObject * tile_loader_new();

void tile_loader_set_url_format(TileLoader *tile_loader, char * format);

void tile_loader_set_cache_directory(TileLoader *tile_loader, char * directory);

void tile_loader_start(TileLoader *tile_loader);

void tile_loader_add_tile(TileLoader *tile_loader, MapTile map_tile); 

void tile_loader_pause(TileLoader *tile_loader);

void tile_loader_resume(TileLoader *tile_loader); 

#endif
