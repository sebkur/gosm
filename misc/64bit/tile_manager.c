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
#include <sched.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <sys/time.h>

#include <string.h>

#include "tile_manager.h"

#define MATCHES(a,b)	(a & b) == b
#define TILESIZE	256

// OSM 1 == G 16
#define FORMAT_FILES	"/%d_%d_%d.png"

G_DEFINE_TYPE (TileManager, tile_manager, G_TYPE_OBJECT);

enum
{
	TILE_LOADED_FROM_DISK,
	TILE_LOADED_FROM_NETW,
	LAST_SIGNAL
};

static guint tile_manager_signals[LAST_SIGNAL] = { 0 };


void function_load_from_netw();

void tile_manager_tile_load_function(TileManager *tile_manager);
void get_tile_from_harddisk(TileManager * tile_manager, int x, int y, int zoom);


GObject * tile_manager_new()
{
	return g_object_new(GOSM_TYPE_TILE_MANAGER, NULL);
}

static void tile_manager_class_init(TileManagerClass *class)
{
	tile_manager_signals[TILE_LOADED_FROM_DISK] = g_signal_new(
		"tile-loaded-from-disk",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (TileManagerClass, tile_loaded_from_disk),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
}

static void tile_manager_init(TileManager *tile_manager)
{

	pthread_mutex_init(&(tile_manager -> mutex_load_from_disk), NULL);
	pthread_mutex_init(&(tile_manager -> mutex_load_from_netw), NULL);
	pthread_mutex_init(&(tile_manager -> mutex_wait_load_from_disk), NULL);
	pthread_mutex_init(&(tile_manager -> mutex_wait_load_from_netw), NULL);
	pthread_cond_init (&(tile_manager -> cond_wait_load_from_disk), NULL);
	pthread_cond_init (&(tile_manager -> cond_wait_load_from_netw), NULL);

	pthread_t thread_disk, thread_netw;
	pthread_attr_t tattr;
	int ret;
	int newprio = 30; 
	struct sched_param param;
	ret = pthread_attr_init (&tattr);
	ret = pthread_attr_getschedparam (&tattr, &param);
	param.sched_priority = newprio;
	ret = pthread_attr_setschedparam (&tattr, &param);
	int x = 0; for (x = 0; x < 8; x++){
		pthread_attr_t tattrn;
		pthread_attr_init(&tattrn);
		size_t stacksize;
		pthread_attr_getstacksize(&tattrn, &stacksize);
		pthread_attr_setstacksize(&tattrn, stacksize);
		int p_netw = pthread_create(&thread_netw, &tattrn, (void *) function_load_from_netw, tile_manager);
	}
}

void function_load_from_netw(TileManager * tile_manager)
{
	CURL * easyhandle = curl_easy_init();
	while(TRUE){
		while(TRUE){	
			pthread_mutex_lock(&(tile_manager -> mutex_load_from_netw));
			if (tile_manager -> load_from_netw -> len == 0){
				break;
			}else{
//				MapTile map_tile = g_array_index(tile_manager -> load_from_netw, MapTile, tile_manager -> load_from_netw -> len - 1);
//				g_array_append_val(tile_manager -> actual_load_from_netw, map_tile);
//				g_array_remove_index(tile_manager -> load_from_netw, tile_manager -> load_from_netw -> len - 1);
//				pthread_mutex_unlock(&(tile_manager -> mutex_load_from_netw));
//				// do actual work here
//				tile_manager_download_tile(tile_manager, easyhandle, map_tile.zoom, map_tile.x, map_tile.y);
//				
//				pthread_mutex_lock(&(tile_manager -> mutex_load_from_netw));
//				int pos = pos_in_list(tile_manager -> actual_load_from_netw, &map_tile);
//				g_array_remove_index_fast(tile_manager -> actual_load_from_netw, pos);
//				//printf("list len: %d\n", tile_manager -> actual_load_from_netw -> len);
//				pthread_mutex_unlock(&(tile_manager -> mutex_load_from_netw));
//
//				g_signal_emit (tile_manager, tile_manager_signals[TILE_LOADED_FROM_DISK], 0);
			}
		}
		pthread_mutex_lock(&(tile_manager -> mutex_wait_load_from_netw));
		pthread_mutex_unlock(&(tile_manager -> mutex_load_from_netw));
		pthread_cond_wait(&(tile_manager -> cond_wait_load_from_netw), &(tile_manager -> mutex_wait_load_from_netw));
		pthread_mutex_unlock(&(tile_manager -> mutex_wait_load_from_netw));
	}
}
