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
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "bookmark_manager.h"
#include "bookmark_location.h"
#include "../customio.h"
#include "../config/config.h"

G_DEFINE_TYPE (BookmarkManager, bookmark_manager, G_TYPE_OBJECT);

enum
{
        BOOKMARK_LOCATION_ADDED,
        BOOKMARK_LOCATION_REMOVED,
	BOOKMARK_LOCATION_MOVED,
        LAST_SIGNAL
};

static guint bookmark_manager_signals[LAST_SIGNAL] = { 0 };

BookmarkManager * bookmark_manager_new()
{
	BookmarkManager * bookmark_manager = g_object_new(GOSM_TYPE_BOOKMARK_MANAGER, NULL);
	bookmark_manager -> bookmarks_location = g_array_new(FALSE, FALSE, sizeof(Bookmark*));
	return bookmark_manager;
}

static void bookmark_manager_class_init(BookmarkManagerClass *class)
{
        bookmark_manager_signals[BOOKMARK_LOCATION_ADDED] = g_signal_new(
                "bookmark-location-added",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (BookmarkManagerClass, bookmark_location_added),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
        bookmark_manager_signals[BOOKMARK_LOCATION_REMOVED] = g_signal_new(
                "bookmark-location-removed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (BookmarkManagerClass, bookmark_location_removed),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        bookmark_manager_signals[BOOKMARK_LOCATION_MOVED] = g_signal_new(
                "bookmark-location-moved",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (BookmarkManagerClass, bookmark_location_moved),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void bookmark_manager_init(BookmarkManager *bookmark_manager)
{
}

GArray * bookmark_manager_get_bookmarks_location(BookmarkManager * bookmark_manager)
{
	return bookmark_manager -> bookmarks_location;
}

void bookmark_manager_add_bookmark(BookmarkManager * bookmark_manager, Bookmark * bookmark)
{
	int type = G_OBJECT_TYPE(bookmark);
	if (type == GOSM_TYPE_BOOKMARK_LOCATION){
		g_array_append_val(bookmark_manager -> bookmarks_location, bookmark);
		g_signal_emit (
			G_OBJECT(bookmark_manager), 
			bookmark_manager_signals[BOOKMARK_LOCATION_ADDED], 0, (gpointer) bookmark);
	}
}

void bookmark_manager_remove_bookmark_location(BookmarkManager * bookmark_manager, int index)
{
	g_array_remove_index(bookmark_manager -> bookmarks_location, index);
	g_signal_emit (
		G_OBJECT(bookmark_manager), 
		bookmark_manager_signals[BOOKMARK_LOCATION_REMOVED], 0, index);
}

void bookmark_manager_move_bookmark_location(BookmarkManager * bookmark_manager, int pos_old, int pos_new)
{
	Bookmark * bookmark = g_array_index(bookmark_manager -> bookmarks_location, Bookmark*, pos_old);
	int i;
	if (pos_new < pos_old){
		for (i = pos_old - 1; i >= pos_new; i--){
			Bookmark * b = g_array_index(bookmark_manager -> bookmarks_location, Bookmark*, i);
			((Bookmark**)(void*)(bookmark_manager -> bookmarks_location -> data))[i+1] = b;
		}
		((Bookmark**)(void*)(bookmark_manager -> bookmarks_location -> data))[pos_new] = bookmark;
	}
	if (pos_new > pos_old){
		for (i = pos_old + 1; i < pos_new; i++){
			Bookmark * b = g_array_index(bookmark_manager -> bookmarks_location, Bookmark*, i);
			((Bookmark**)(void*)(bookmark_manager -> bookmarks_location -> data))[i-1] = b;
		}
		((Bookmark**)(void*)(bookmark_manager -> bookmarks_location -> data))[pos_new-1] = bookmark;
	}
	int indices[2] = {pos_old, pos_new};
	g_signal_emit (
		G_OBJECT(bookmark_manager), 
		bookmark_manager_signals[BOOKMARK_LOCATION_MOVED], 0, (gpointer) indices);
}

/****************************************************************************************************
* read in the list of bookmarks
****************************************************************************************************/
//TODO: splitting in only 100 lines is stupid
gboolean bookmark_manager_read_bookmarks(BookmarkManager * bookmark_manager)
{
	char * filepath = config_get_bookmarks_file();
        struct stat sb;
        int s = stat(filepath, &sb);
        if (s == -1){
                printf("bookmarks file not found\n");
		return FALSE;
        }
        int fd = open(filepath, O_RDONLY);
        if (fd == -1){
                printf("bookmarks file could not be opened for reading\n");
                return FALSE;
        }
        int size = sb.st_size;
        char buf[size+1];
        read(fd, buf, size);
        close(fd);
        buf[size] = '\0';

        gchar ** splitted = g_strsplit(buf, "\n", 100);
        gchar * current = splitted[0];
        int i = 0;
        while (current != NULL){
                if (strlen(current) > 0){
                        gchar ** splitline = g_strsplit(current, "\t", 5);
			if (g_strv_length(splitline) == 5){
				gchar * name = g_strstrip(splitline[0]);
				gchar * type = g_strstrip(splitline[1]);
				double lon = strtodouble(g_strstrip(splitline[2]));
				double lat = strtodouble(g_strstrip(splitline[3]));
				int zoom = atoi(g_strstrip(splitline[4]));
				Bookmark * bookmark = bookmark_location_new(name, lon, lat, zoom);
				bookmark_manager_add_bookmark(bookmark_manager, bookmark);
			}
			g_strfreev(splitline);
                }
                current = splitted[++i];
        }
	g_strfreev(splitted);
	//g_signal_emit (bookmark_manager, bookmark_manager_signals[BOOKMARKS_LOADED], 0);
	return TRUE;
}

/****************************************************************************************************
* save bookmarks
****************************************************************************************************/
gboolean bookmark_manager_save(BookmarkManager * bookmark_manager)
{
	printf("saving bookmarks\n");
	char * filepath = config_get_bookmarks_file();
        int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1){
                printf("bookmarks file could not be opened for writing\n");
                return FALSE;
        }
	int i;
	for (i = 0; i < bookmark_manager -> bookmarks_location -> len; i++){
		Bookmark * bookmark = g_array_index(bookmark_manager -> bookmarks_location, Bookmark*, i);
		int type = G_OBJECT_TYPE(bookmark);
		char * type_name;
		if (type == GOSM_TYPE_BOOKMARK_LOCATION){
			BookmarkLocation * bl = GOSM_BOOKMARK_LOCATION(bookmark);
			type_name = "Location";
			char lon[20], lat[20], zoom[4];
			sprintdouble(lon, bl -> lon, 7);
			sprintdouble(lat, bl -> lat, 7);
			sprintf(zoom, "%d", bl -> zoom);
			write(fd, bl -> name, strlen(bl -> name));
			write(fd, "\t", 1);
			write(fd, type_name, strlen(type_name));
			write(fd, "\t", 1);
			write(fd, lon, strlen(lon));
			write(fd, "\t", 1);
			write(fd, lat, strlen(lat));
			write(fd, "\t", 1);
			write(fd, zoom, strlen(zoom));
			write(fd, "\n", 1);
		}
	}
	close(fd);
	return TRUE;
}
