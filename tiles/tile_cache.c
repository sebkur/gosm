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

#include <glib.h>

#include "tile_cache.h"

/****************************************************************************************************
* the TileCache hold references to tiles that have already been loaded into memory and provides
* efficient mechanisms to 
*	- find out whether a tile is in cache
*	- retrieve the pointer to a cached tile
****************************************************************************************************/

/****************************************************************************************************
* Comparism functions for the cache-structure
* the cache is organised like this:
*	- an array of sorted lists, one list for each zoomlevel
*	- these lists are contain further lists of tiles present in the cache
*	- the toplevel lists are sorted by x-values
*	- the 2nd-level lists are sorted by y-values
*	- the 2nd-level list contains references to the actual tiles
****************************************************************************************************/
/****************************************************************************************************
* TODO: add detailed descriptions of comparism functions
****************************************************************************************************/
gint compare_list_x_int(gconstpointer a, gconstpointer b)
{
	int a_x = ((list_x*) a) -> x;
	int b_x = *((int*)b);
	if (a_x > b_x){
		return 1;
	}
	if (a_x < b_x){
		return -1;
	}
	return 0;
	
}

/****************************************************************************************************
* 
****************************************************************************************************/
gint compare_list_x_list_x(gconstpointer a, gconstpointer b)
{
	int a_x = ((list_x*) a) -> x;
	int b_x = ((list_x*) b) -> x;
	if (a_x > b_x){
		return 1;
	}
	if (a_x < b_x){
		return -1;
	}
	return 0;
	
}

/****************************************************************************************************
* 
****************************************************************************************************/
gint compare_node_y_int(gconstpointer a, gconstpointer b)
{
	int a_y = ((node_y*) a) -> y;
	int b_y = *((int*)b);
	if (a_y > b_y){
		return 1;
	}
	if (a_y < b_y){
		return -1;
	}
	return 0;
	
}

/****************************************************************************************************
* 
****************************************************************************************************/
gint compare_node_y_node_y(gconstpointer a, gconstpointer b)
{
	int a_y = ((node_y*) a) -> y;
	int b_y = ((node_y*) b) -> y;
	if (a_y > b_y){
		return 1;
	}
	if (a_y < b_y){
		return -1;
	}
	return 0;
	
}

/****************************************************************************************************
* create a new TileCache
****************************************************************************************************/
TileCache * cache_new(int cache_size)
{
	TileCache * cache = malloc(sizeof(struct TileCache));
	cache -> table = malloc(sizeof(GList) * 18);
	memset(cache -> table, 0, sizeof(GList) * 18);
	cache -> ring_buffer = ringbuffer_new(cache_size, sizeof(struct CachedMapTile));
	return cache;
}

/****************************************************************************************************
* find a tile in cache; return the pointer if present, NULL otherwise
****************************************************************************************************/
gpointer cache_find_tile(TileCache * cache, int zoom, int x, int y)
{
	GList * list = (cache -> table)[zoom];
	GList * find = g_list_find_custom(list, &x, compare_list_x_int);
	if (find == NULL){
		return NULL;
	}
	GList * list2 = ((list_x*)(find -> data)) -> list;
	GList * find2 = g_list_find_custom(list2, &y, compare_node_y_int);
	if (find2 == NULL){
		return NULL;
	}
	return ((node_y*)(find2 -> data)) -> pixbuf;
}

/****************************************************************************************************
* add a tile to cache. return TRUE if an old tile has been replaced, FALSE otherwise
****************************************************************************************************/
gboolean cache_add_tile(TileCache * cache, int zoom, int x, int y, gpointer pixbuf)
{
	MapTile mt; mt.x = x; mt.y = y; mt.zoom = zoom;
	CachedMapTile cmt; cmt.map_tile_info = mt; cmt.pixbuf = pixbuf;
	gboolean insert = TRUE;
	GList ** list = &((cache -> table)[zoom]);
	if (*list == NULL){
		//printf("new list on first level\n");
		*list = g_list_alloc();
		list_x * listx = malloc(sizeof(struct list_x));
		listx -> x = x;
		listx -> list = g_list_alloc();
		(*list) -> data = listx;
		node_y * nodey = malloc(sizeof(struct node_y));
		nodey -> y = y;
		nodey -> pixbuf = pixbuf;
		listx -> list -> data = nodey;
	}else{
		//printf("list on first level exists\n");
		GList * find = g_list_find_custom(*list, &x, compare_list_x_int);
		if (find == NULL){
			//printf("new list on second level\n");
			list_x * listx = malloc(sizeof(struct list_x));
			listx -> x = x;
			listx -> list = g_list_alloc();
			*list = g_list_insert_sorted(*list, listx, compare_list_x_list_x);
			node_y * nodey = malloc(sizeof(struct node_y));
			nodey -> y = y;
			nodey -> pixbuf = pixbuf;
			listx -> list -> data = nodey;
		}else{
			//printf("list on second level exists\n");
			GList ** list2 = &(((list_x*)(find -> data)) -> list);
			GList * find2 = g_list_find_custom(*list2, &y, compare_node_y_int);
			if (find2 == NULL){
				//printf("tile not in cache\n");
				node_y * nodey = malloc(sizeof(struct node_y));
				nodey -> y = y;
				nodey -> pixbuf = pixbuf;
				*list2 = g_list_insert_sorted(*list2, nodey, compare_node_y_node_y);
			}else{
				printf("ERROR: tile in cache %d %d %d\n", zoom, x, y);
				insert = FALSE;
			}
		}
	}
	gboolean replace = FALSE;
	if (insert){
		replace = ringbuffer_append(cache -> ring_buffer, &cmt);
		if (replace){
			CachedMapTile * cmt = (CachedMapTile*)(cache -> ring_buffer -> overwritten);
			MapTile mt = cmt -> map_tile_info;
			cache_remove_tile(cache, mt.zoom, mt.x, mt.y);
		}
	}
	return replace;
}

/****************************************************************************************************
* this function not only removes from the
* 3-dimensional cache-list, but also from
* the ringbuffer (it sets zoomlevel to 0 in the CachedMapTile)
* -> when this removed tile is 'overwritten' by 'cache_add_tile'
* (since it resides in the ringbuffer), the function 'cache_remove_tile'
* will not be called on this tile again.
* PURPOSE: this function forces the tile-cache to reload this tile on next request
****************************************************************************************************/
void cache_purge_tile(TileCache * cache, int zoom, int x, int y)
{
	cache_remove_tile(cache, zoom, x, y);
	int len = cache -> ring_buffer -> len;
	int i;
	for (i = 0; i < len; i++){
		CachedMapTile * cmt = ringbuffer_index(cache -> ring_buffer, i);
		MapTile * map_tile = &(cmt -> map_tile_info);
		if (map_tile -> x == x && map_tile -> y == y && map_tile -> zoom == zoom){
			map_tile -> zoom = 0;
		}
	}
}

/****************************************************************************************************
* remove a tile from cache 
****************************************************************************************************/
void cache_remove_tile(TileCache * cache, int zoom, int x, int y)
{
	//printf("removing\n");
	GList ** list = &((cache -> table)[zoom]);
	if (*list == NULL){
		return;
	}
	GList * find = g_list_find_custom(*list, &x, compare_list_x_int);
	if (find == NULL){
		return;
	}
	GList ** list2 = &(((list_x*)find->data) -> list);
	GList * find2 = g_list_find_custom(*list2, &y, compare_node_y_int);
	if (find2 == NULL){
		return;
	}
	//printf("found!!!\n");
	*list2 = g_list_remove_link(*list2, find2);
	free(find2 -> data);
	g_list_free(find2);
	if (g_list_length(*list2) == 0){
		//printf("REM 1\n");
		*list = g_list_remove_link(*list, find);
		free(find -> data);
		g_list_free(find);
		//printf("REM_LEN: %d\n", g_list_length(*list));
		if (g_list_length(*list) == 0){
			//printf("REM 2\n");
			g_list_free(*list);
			*list = NULL;
		}
	}
}

/*
int main(int argc, char *argv[])
{
	TileCache * tc = cache_new();
	MapTile mt; mt.x = 11; mt.y = 5; mt.zoom = 11;
	CachedMapTile cmt; cmt.map_tile_info = mt;
	cache_add_tile(tc, cmt);

	mt.x = 12; mt.y = 5; mt.zoom = 11;
	cmt.map_tile_info = mt;
	cache_add_tile(tc, cmt);

	mt.x = 12; mt.y = 6; mt.zoom = 11;
	cmt.map_tile_info = mt;
	cache_add_tile(tc, cmt);
	cache_add_tile(tc,cmt);

	printf("%d\n", cache_find_tile(tc, 11, 12, 6));
	GList cache[16];
	GList * list = g_list_alloc();
	int * q = malloc(sizeof(int));
	*q = 99;
	list -> data = q;

	int a;
	for (a = 20; a > 10; a--){
		int * p = malloc(sizeof(int));
		*p = random();
		//GList * app = g_list_append(list, p);
		list = g_list_insert_sorted(list, p, compare);
	}

	GList * iter = list;
	while (iter != NULL){
		printf("%d\n", *((int*)(iter -> data)));
		iter = iter->next;
	}
}*/
