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
#include "../map_types.h"
#include "../tool.h"
#include "tilemath.h"
#include "tile_cache.h"

#define MATCHES(a,b)	(a & b) == b
#define TILESIZE	256

// OSM 1 == G 16
#define FORMAT_FILES	"/%d_%d_%d.png"

/****************************************************************************************************
* a TileManager manages everything about tiles of a map-tile-source
****************************************************************************************************/
G_DEFINE_TYPE (TileManager, tile_manager, G_TYPE_OBJECT);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
	TILE_LOADED_FROM_DISK,
	TILE_LOADED_FROM_NETW,
	LAST_SIGNAL
};

static guint tile_manager_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* a mutex for initializing curl easyhandles
****************************************************************************************************/
static pthread_mutex_t mutex_curl = PTHREAD_MUTEX_INITIALIZER;

/**
 * add functions for adding tile to list / waking up loader thread
 */
//g_signal_emit (widget, tile_manager_signals[TILE_LOADED_SUCCESFULLY], 0);


/****************************************************************************************************
* method declaration
****************************************************************************************************/
void function_load_from_disk();
void function_load_from_netw();

void tile_manager_tile_load_function(TileManager *tile_manager);
void get_tile_from_harddisk(TileManager * tile_manager, int x, int y, int zoom);


/****************************************************************************************************
* constructor
****************************************************************************************************/
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

/****************************************************************************************************
* object init
****************************************************************************************************/
static void tile_manager_init(TileManager *tile_manager)
{
	tile_manager -> tile_cache	= cache_new(64); 					// TODO: CACHE_SIZE
	tile_manager -> load_from_disk	= g_array_new(TRUE, TRUE, sizeof(struct MapTile));
	tile_manager -> load_from_netw	= g_array_new(TRUE, TRUE, sizeof(struct MapTile));
	tile_manager -> actual_load_from_netw	= g_array_new(TRUE, TRUE, sizeof(struct MapTile));

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
	size_t stacksize = PTHREAD_STACK_MIN * 8;
	pthread_attr_setstacksize(&tattr, stacksize);
	int p_disk = pthread_create(&thread_disk, &tattr, (void *) function_load_from_disk, tile_manager);
	pthread_attr_destroy(&tattr);
	int x = 0; for (x = 0; x < 8; x++){
		pthread_attr_t tattrn;
		pthread_attr_init(&tattrn);
		size_t stacksize = PTHREAD_STACK_MIN * 8;
		pthread_attr_setstacksize(&tattrn, stacksize);
		int p_netw = pthread_create(&thread_netw, &tattrn, (void *) function_load_from_netw, tile_manager);
		pthread_attr_destroy(&tattrn);
	}
}

/****************************************************************************************************
* set/get whether the TileManager is on- or off-line
****************************************************************************************************/
void tile_manager_set_network_state(TileManager * tile_manager, gboolean state)
{
	tile_manager -> network_state = state;
}

gboolean tile_manager_get_network_state(TileManager * tile_manager)
{
	return tile_manager -> network_state;
}

/****************************************************************************************************
* set the url used to retrieve tiles via network
****************************************************************************************************/
void tile_manager_set_url_format(TileManager * tile_manager, char * format)
{
	tile_manager -> format_url = malloc(sizeof(char) * (strlen(format) + 1));
	strcpy(tile_manager -> format_url, format);
}

/****************************************************************************************************
* set the cache directory used to store retrieved tiles
****************************************************************************************************/
void tile_manager_set_cache_directory(TileManager * tile_manager, char * directory)
{
	// TODO: directory-string might contain a trailing backslash...
	// currently, this leads to an error, since another one is appended
	if (tile_manager -> cache_dir != NULL){
		free(tile_manager -> cache_dir);
	}
	tile_manager -> cache_dir = malloc(sizeof(char) * (strlen(directory) + 1));
	strcpy(tile_manager -> cache_dir, directory);

	int dir_len = strlen(directory);
	int format_len = strlen(FORMAT_FILES);
	int len = dir_len + format_len;
	tile_manager -> cache_files_format = malloc(sizeof(char) * (len + 1));
	strcpy(tile_manager -> cache_files_format, directory);
	strcpy(tile_manager -> cache_files_format + dir_len, FORMAT_FILES);
}

/****************************************************************************************************
* find out whether a tile has already been appended to the list of tiles-to-load
****************************************************************************************************/
gboolean is_in_list(GArray * array, MapTile * map_tile){
	int i;
	for (i = 0; i < array -> len; i++){
		MapTile element = g_array_index(array, MapTile, i);
		if (element.zoom == map_tile->zoom && element.x == map_tile->x && element.y == map_tile->y){
			return TRUE;
		}
	}
	return FALSE;
}

/****************************************************************************************************
* return where a MapTile is in the queue
****************************************************************************************************/
int pos_in_list(GArray * array, MapTile * map_tile){
	int i;
	for (i = 0; i < array -> len; i++){
		MapTile element = g_array_index(array, MapTile, i);
		if (element.zoom == map_tile->zoom && element.x == map_tile->x && element.y == map_tile->y){
			return i;
		}
	}
	return -1;
}

/****************************************************************************************************
* request a MapTile
* if the MapTile is in cache, return it immediatly
* otherwise try to load it from disk.
* 	if the file is present on harddisk, block until it's loaded
*	otherwise add it to the queue of tiles, that shall be loaded via network
*		-> return NULL
****************************************************************************************************/
gpointer tile_manager_request_tile(TileManager * tile_manager, int x, int y, int zoom)
{
	//check if in memory-cache
	pthread_mutex_lock(&(tile_manager -> mutex_tile_cache));
	gpointer tile_p = cache_find_tile(tile_manager -> tile_cache, zoom, x, y);	
	pthread_mutex_unlock(&(tile_manager -> mutex_tile_cache));
	if (tile_p != NULL){
		// tile is in memory cache already -> return it
		return tile_p;
	}
	// tile not in cache -> tell hd-thread to look for it
	get_tile_from_harddisk(tile_manager, x, y, zoom);
	return NULL;
}

/****************************************************************************************************
* remove a MapTile from cache and harddisk.
* -> it will be retrieved from network the next time it is requested
*	-> this is used to mark tiles for reloading or removal from harddisk
****************************************************************************************************/
void tile_manager_delete_tile(TileManager * tile_manager, int x, int y, int zoom)
{
	// delete from harddisk
	char path[tile_manager -> cache_files_format_len + 22];
	sprintf(path, tile_manager -> cache_files_format, zoom, x, y);
	unlink(path);
	// remove from memory
	pthread_mutex_lock(&(tile_manager -> mutex_tile_cache));
	TileCache * cache = tile_manager -> tile_cache;
	gpointer tile = cache_find_tile(cache, zoom, x, y);
	if (tile != NULL){
		cache_purge_tile(cache, zoom, x, y);
		//CachedMapTile * cmt = (CachedMapTile*) tile;
		//GdkPixmap * pm_old = cmt -> pixbuf;
		pthread_mutex_unlock(&(tile_manager -> mutex_tile_cache));
		//gdk_threads_enter();
		//g_object_unref(tile);
		//gdk_threads_leave();
		//printf("%p\n", tile);
	}else{
		pthread_mutex_unlock(&(tile_manager -> mutex_tile_cache));
	}
}

/****************************************************************************************************
* load a MapTile from harddisk
****************************************************************************************************/
void get_tile_from_harddisk(TileManager * tile_manager, int x, int y, int zoom)
{
	//printf("get tile from hd: %d %d %d\n", x, y, zoom);
	pthread_mutex_lock(&(tile_manager -> mutex_load_from_disk));
	MapTile map_tile;
	map_tile.x = x;
	map_tile.y = y;
	map_tile.zoom = zoom;
	if (!(is_in_list(tile_manager -> load_from_disk, &map_tile) || !memcmp(&(tile_manager -> actual_load_from_disk), &map_tile, sizeof(MapTile)))){
		g_array_append_val(tile_manager -> load_from_disk, map_tile);
		//printf("appended\n");
	}
	pthread_mutex_unlock(&(tile_manager -> mutex_load_from_disk));
	// notify loader thread
	pthread_mutex_lock(&(tile_manager -> mutex_wait_load_from_disk));
	pthread_cond_signal(&(tile_manager -> cond_wait_load_from_disk));
	pthread_mutex_unlock(&(tile_manager -> mutex_wait_load_from_disk));
}

/****************************************************************************************************
* load a MapTile from network
****************************************************************************************************/
void get_tile_from_network(TileManager * tile_manager, int x, int y, int zoom)
{
	//printf("get tile from nw: %d %d %d\n", x, y, zoom);
	pthread_mutex_lock(&(tile_manager -> mutex_load_from_netw));
	MapTile map_tile;
	map_tile.x = x;
	map_tile.y = y;
	map_tile.zoom = zoom;
	if (!(is_in_list(tile_manager -> load_from_netw, &map_tile) || is_in_list(tile_manager -> actual_load_from_netw, &map_tile))){
		g_array_append_val(tile_manager -> load_from_netw, map_tile);
		//printf("appended\n");
	}
	pthread_mutex_unlock(&(tile_manager -> mutex_load_from_netw));
	// notify loader thread
	pthread_mutex_lock(&(tile_manager -> mutex_wait_load_from_netw));
	pthread_cond_signal(&(tile_manager -> cond_wait_load_from_netw));
	pthread_mutex_unlock(&(tile_manager -> mutex_wait_load_from_netw));
}

/****************************************************************************************************
* actual thread-function for loading tiles from disk
****************************************************************************************************/
void function_load_from_disk(TileManager * tile_manager)
{
	while (TRUE){
		gboolean got = FALSE;
		while (TRUE){
			pthread_mutex_lock(&(tile_manager -> mutex_load_from_disk));
			if (tile_manager -> load_from_disk -> len == 0){
				break;
			}else{
				MapTile map_tile = g_array_index(tile_manager -> load_from_disk, MapTile, tile_manager -> load_from_disk -> len - 1);
				memcpy(&(tile_manager -> actual_load_from_disk), &map_tile, sizeof(MapTile));
				g_array_remove_index(tile_manager -> load_from_disk, tile_manager -> load_from_disk -> len - 1);
				pthread_mutex_unlock(&(tile_manager -> mutex_load_from_disk));
				// do actual work here
				char path[tile_manager -> cache_files_format_len + 22];
				sprintf(path, tile_manager -> cache_files_format, map_tile.zoom, map_tile.x, map_tile.y);
				gdk_threads_enter();
				GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(path, NULL);
				if (!GDK_IS_PIXBUF(pixbuf)){
					gdk_threads_leave();
					// not on disk -> load from network
					memset(&(tile_manager -> actual_load_from_disk), 0, sizeof(MapTile));
					if (tile_manager -> network_state){
						get_tile_from_network(tile_manager, map_tile.x, map_tile.y, map_tile.zoom);
					}
					/*FILE * f = fopen(path, "r");
					if (f == NULL){
						get_tile_from_network(map_tile.x, map_tile.y, map_tile.zoom);
					}else{
						fclose(f);
					}*/
				}else{
					got = TRUE;
					//gdk_threads_enter();
					GdkPixmap * pixmap = gdk_pixmap_new(NULL, 256, 256, 24);
					GdkVisual * visual = gdk_visual_get_best_with_depth(24);
					GdkColormap * colormap = gdk_colormap_new(visual, TRUE);
					gdk_drawable_set_colormap(pixmap, colormap);
					GdkGC * gc = gdk_gc_new(pixmap);
					gdk_draw_pixbuf(pixmap, gc, pixbuf,
						0, 0, 0, 0,
						TILESIZE, TILESIZE,
						GDK_RGB_DITHER_NONE, 0, 0);
					g_object_unref(pixbuf);
					g_object_unref(gc);
					g_object_unref(colormap);
					gdk_threads_leave();
					pthread_mutex_lock(&(tile_manager -> mutex_tile_cache));
					gboolean replace = cache_add_tile(tile_manager -> tile_cache, map_tile.zoom, map_tile.x, map_tile.y, pixmap);
					//printf("cache size: %d\n", tile_cache -> ring_buffer -> len);
					if (replace){
						//printf("replaced\n");
						CachedMapTile * cmt = (CachedMapTile*)(tile_manager -> tile_cache -> ring_buffer -> overwritten);
						GdkPixmap * pm_old = cmt -> pixbuf;
						pthread_mutex_unlock(&(tile_manager -> mutex_tile_cache));
						gdk_threads_enter();
						g_object_unref(pm_old);
						gdk_threads_leave();
					}else{
						pthread_mutex_unlock(&(tile_manager -> mutex_tile_cache));
					}
					memset(&(tile_manager -> actual_load_from_disk), 0, sizeof(MapTile));
				}
			}

		}
		pthread_mutex_unlock(&(tile_manager -> mutex_load_from_disk));
		if (got){
			g_signal_emit (tile_manager, tile_manager_signals[TILE_LOADED_FROM_DISK], 0);
		}
		pthread_mutex_lock(&(tile_manager -> mutex_load_from_disk));
		if (tile_manager -> load_from_disk -> len != 0){
			pthread_mutex_unlock(&(tile_manager -> mutex_load_from_disk));
			continue;
		}else{
			pthread_mutex_lock(&(tile_manager -> mutex_wait_load_from_disk));
			pthread_mutex_unlock(&(tile_manager -> mutex_load_from_disk));
			pthread_cond_wait(&(tile_manager -> cond_wait_load_from_disk), &(tile_manager -> mutex_wait_load_from_disk));
			pthread_mutex_unlock(&(tile_manager -> mutex_wait_load_from_disk));
		}
	}
}


/****************************************************************************************************
* download a tile, internally used function
****************************************************************************************************/
void tile_manager_download(TileManager * tile_manager, CURL * easyhandle, const char* url, const char* file_name)
{ // TODO: recycle easyhandle like in tile_loader
  // better: use one method for both classes
	//if (tile_manager -> easyhandle == NULL){
	//	tile_manager -> easyhandle = curl_easy_init() ;
	//}
	//CURL * easyhandle = tile_manager -> easyhandle;
	curl_easy_setopt( easyhandle, CURLOPT_URL, url ) ;
	FILE * file = fopen( file_name, "w" ) ;
	//curl_easy_setopt( easyhandle, CURLOPT_PROXY, "130.133.8.114:80");
	curl_easy_setopt( easyhandle, CURLOPT_WRITEDATA, file ) ;
	//curl_easy_setopt(easyhandle, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, "Pure Data");
	curl_easy_perform( easyhandle );
	//curl_easy_cleanup( easyhandle );
	fclose(file);
}

/****************************************************************************************************
* download a tile, easy interface
****************************************************************************************************/
void tile_manager_download_tile(TileManager * tile_manager, CURL * easyhandle, int zoom, int x, int y)
{
	char url[100];
	char filename[tile_manager -> cache_files_format_len + 22];
	sprintf(url, tile_manager -> format_url, zoom, x, y);
	sprintf(filename, tile_manager -> cache_files_format, zoom, x, y);
	//printf("downloading %s to %s\n", url, filename);
	tile_manager_download(tile_manager, easyhandle, url, filename);
	//printf("got %s\n", filename);
}

/****************************************************************************************************
* actually used thread-function for loading a tile
****************************************************************************************************/
void function_load_from_netw(TileManager * tile_manager)
{
	pthread_mutex_lock(&mutex_curl);
	CURL * easyhandle = curl_easy_init();
	pthread_mutex_unlock(&mutex_curl);
	while(TRUE){
		while(TRUE){	
			pthread_mutex_lock(&(tile_manager -> mutex_load_from_netw));
			if (tile_manager -> load_from_netw -> len == 0){
				break;
			}else{
				MapTile map_tile = g_array_index(tile_manager -> load_from_netw, MapTile, tile_manager -> load_from_netw -> len - 1);
				g_array_append_val(tile_manager -> actual_load_from_netw, map_tile);
				g_array_remove_index(tile_manager -> load_from_netw, tile_manager -> load_from_netw -> len - 1);
				pthread_mutex_unlock(&(tile_manager -> mutex_load_from_netw));
				// do actual work here
				tile_manager_download_tile(tile_manager, easyhandle, map_tile.zoom, map_tile.x, map_tile.y);
				
				pthread_mutex_lock(&(tile_manager -> mutex_load_from_netw));
				int pos = pos_in_list(tile_manager -> actual_load_from_netw, &map_tile);
				g_array_remove_index_fast(tile_manager -> actual_load_from_netw, pos);
				//printf("list len: %d\n", tile_manager -> actual_load_from_netw -> len);
				pthread_mutex_unlock(&(tile_manager -> mutex_load_from_netw));

				g_signal_emit (tile_manager, tile_manager_signals[TILE_LOADED_FROM_DISK], 0);
			}
		}
		pthread_mutex_lock(&(tile_manager -> mutex_wait_load_from_netw));
		pthread_mutex_unlock(&(tile_manager -> mutex_load_from_netw));
		pthread_cond_wait(&(tile_manager -> cond_wait_load_from_netw), &(tile_manager -> mutex_wait_load_from_netw));
		pthread_mutex_unlock(&(tile_manager -> mutex_wait_load_from_netw));
	}
}
