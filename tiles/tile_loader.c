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

#include <curl/curl.h>

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <sys/time.h>

#include "tile_loader.h"
#include "../tool.h"
#include "tilemath.h"
#include "tile_cache.h"

#define MATCHES(a,b)	(a & b) == b
#define TILESIZE	256
#define FORMAT_FILES	"/%d_%d_%d.png"

/****************************************************************************************************
* TileLoader encapsulates functions for retrieving tiles over network
****************************************************************************************************/
G_DEFINE_TYPE (TileLoader, tile_loader, G_TYPE_OBJECT);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
	TILE_LOADED_SUCCESFULLY,
	TILE_FAILED,
	QUEUE_EMPTY,
	LAST_SIGNAL
};

static guint tile_loader_signals[LAST_SIGNAL] = { 0 };


/**
 * TODO: add functions for adding tile to list / waking up loader thread
 */

/****************************************************************************************************
* method declarations
****************************************************************************************************/
void tile_loader_tile_load_function(TileLoader *tile_loader);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GObject * tile_loader_new()
{
	return g_object_new(GOSM_TYPE_TILE_LOADER, NULL);
}

//g_signal_emit (widget, tile_loader_signals[TILE_LOADED_SUCCESFULLY], 0);

/****************************************************************************************************
* class init
****************************************************************************************************/
static void tile_loader_class_init(TileLoaderClass *class)
{
	tile_loader_signals[TILE_LOADED_SUCCESFULLY] = g_signal_new(
		"tile-loaded-succesfully",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (TileLoaderClass, tile_loaded_succesfully),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
	tile_loader_signals[TILE_FAILED] = g_signal_new(
		"tile-failed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (TileLoaderClass, tile_loaded_succesfully),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
	tile_loader_signals[QUEUE_EMPTY] = g_signal_new(
		"queue-empty",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (TileLoaderClass, tile_loaded_succesfully),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
}

/****************************************************************************************************
* object init
****************************************************************************************************/
static void tile_loader_init(TileLoader *tile_loader)
{
	tile_loader -> list_to_load = g_array_new(TRUE, TRUE, sizeof(struct MapTile));

	pthread_mutex_init(&(tile_loader -> mutex_list), NULL);
	pthread_mutex_init(&(tile_loader -> mutex_mode), NULL);
}

/****************************************************************************************************
* set the format of urls used to retrieve tiles
****************************************************************************************************/
void tile_loader_set_url_format(TileLoader * tile_loader, char * format)
{
	tile_loader -> format_url = malloc(sizeof(char) * (strlen(format) + 1));
	strcpy(tile_loader -> format_url, format);
}

/****************************************************************************************************
* set the location of where to save tiles
****************************************************************************************************/
void tile_loader_set_cache_directory(TileLoader * tile_loader, char * directory)
{
        // TODO: directory-string might contain a trailing backslash...
        // currently, this leads to an error, since another one is appended
        if (tile_loader -> cache_dir != NULL){
                free(tile_loader -> cache_dir);
        }
        tile_loader -> cache_dir = malloc(sizeof(char) * (strlen(directory) + 1));
        strcpy(tile_loader -> cache_dir, directory);

        int dir_len = strlen(directory);
        int format_len = strlen(FORMAT_FILES);
        int len = dir_len + format_len;
        tile_loader -> cache_files_format = malloc(sizeof(char) * (len + 1));
        strcpy(tile_loader -> cache_files_format, directory);
        strcpy(tile_loader -> cache_files_format + dir_len, FORMAT_FILES);
}

/****************************************************************************************************
* start loading the preset tiles
****************************************************************************************************/
void tile_loader_start(TileLoader *tile_loader)
{
	tile_loader -> mode = TILE_LOADER_MODE_RUN;
	pthread_t thread;
	int p = pthread_create(&thread, NULL, (void *) tile_loader_tile_load_function, tile_loader);
}

/****************************************************************************************************
* pause loading
****************************************************************************************************/
void tile_loader_pause(TileLoader *tile_loader)
{
	pthread_mutex_lock(&(tile_loader -> mutex_mode));
	tile_loader -> mode = TILE_LOADER_MODE_PAUSE;
	pthread_mutex_unlock(&(tile_loader -> mutex_mode));
}

/****************************************************************************************************
* resume loading
****************************************************************************************************/
void tile_loader_resume(TileLoader *tile_loader)
{
	pthread_mutex_lock(&(tile_loader -> mutex_mode));
	tile_loader -> mode = TILE_LOADER_MODE_RUN;
	pthread_cond_signal(&(tile_loader -> cond_wait));
	pthread_mutex_unlock(&(tile_loader -> mutex_mode));
}

/****************************************************************************************************
* download a single tile, internally used function
****************************************************************************************************/
void tile_loader_download(TileLoader * tile_loader, const char* url, const char* file_name)
{
	if (tile_loader -> easyhandle == NULL){
		tile_loader -> easyhandle = curl_easy_init() ;
		//curl_easy_setopt(tile_loader -> easyhandle, CURLOPT_PROXY, "130.133.8.114:80");
	}
	curl_easy_setopt(tile_loader -> easyhandle, CURLOPT_URL, url ) ;
	FILE * file = fopen( file_name, "w" ) ;
	curl_easy_setopt(tile_loader -> easyhandle, CURLOPT_WRITEDATA, file ) ;
	curl_easy_perform(tile_loader -> easyhandle);
	//curl_easy_cleanup(tile_loader -> easyhandle);
	fclose(file);
}

/****************************************************************************************************
* download a single tile
****************************************************************************************************/
void tile_loader_download_tile(TileLoader * tile_loader, int zoom, int x, int y)
{
	// TODO: bad constant buffer size
	char url[100];
	char filename[tile_loader -> cache_files_format_len + 22];
	sprintf(url, tile_loader -> format_url, zoom, x, y);
	sprintf(filename, tile_loader -> cache_files_format, zoom, x, y);
	printf("downloading %s to %s\n", url, filename);
	tile_loader_download(tile_loader, url, filename);
	printf("got %s\n", filename);
}

/****************************************************************************************************
* add a tile for downloading
****************************************************************************************************/
void tile_loader_add_tile(TileLoader *tile_loader, MapTile map_tile)
{
		pthread_mutex_lock(&(tile_loader -> mutex_list));
		g_array_append_val(tile_loader -> list_to_load, map_tile);
		pthread_mutex_unlock(&(tile_loader -> mutex_list));
}

/****************************************************************************************************
* the function used by the loading-thread to perform the downloading of the preset tiles
****************************************************************************************************/
void tile_loader_tile_load_function(TileLoader *tile_loader)
{
	//while(TRUE){ // this loop could be used, if adding tiles after initial start should be supported
		while(TRUE){
			pthread_mutex_lock(&(tile_loader -> mutex_mode));
			if (tile_loader -> mode == TILE_LOADER_MODE_PAUSE){
				pthread_cond_wait(&(tile_loader -> cond_wait), &(tile_loader -> mutex_mode));
				pthread_mutex_unlock(&(tile_loader -> mutex_mode));
			}else{
				pthread_mutex_unlock(&(tile_loader -> mutex_mode));
			}
			pthread_mutex_lock(&(tile_loader -> mutex_list));
			if (tile_loader -> list_to_load -> len > 0){
				MapTile map_tile = g_array_index(tile_loader -> list_to_load, MapTile, tile_loader -> list_to_load -> len - 1);
				g_array_remove_index(tile_loader -> list_to_load, tile_loader -> list_to_load -> len - 1);
				pthread_mutex_unlock(&(tile_loader -> mutex_list));

				printf("looking for %d %d %d\n", map_tile.zoom, map_tile.x, map_tile.y);
				char filename[tile_loader -> cache_files_format_len + 22];
				sprintf(filename, tile_loader -> cache_files_format, map_tile.zoom, map_tile.x, map_tile.y);
				FILE * f = fopen(filename, "r");
				if (f != NULL){
					//unsigned int usec = 300000; // remove this, just for testing pause mode
					//usleep(usec);	
					fclose(f);
					g_signal_emit (tile_loader, tile_loader_signals[TILE_LOADED_SUCCESFULLY], 0);
				}else{
					printf("downloading %d %d %d\n", map_tile.zoom, map_tile.x, map_tile.y);
					tile_loader_download_tile(tile_loader, map_tile.zoom, map_tile.x, map_tile.y);
					g_signal_emit (tile_loader, tile_loader_signals[TILE_LOADED_SUCCESFULLY], 0);
				}
			}else{
				pthread_mutex_unlock(&(tile_loader -> mutex_list));
				break; // remove this for adding tile after initial start support
			}
		}
	//	pthread_cond_wait(&(tile_loader -> cond_wait), &(tile_loader -> mutex_list));
	//}
}
