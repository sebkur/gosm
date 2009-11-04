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

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint bookmark_manager_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, bookmark_manager_signals[SIGNAL_NAME_n], 0);

BookmarkManager * bookmark_manager_new()
{
	BookmarkManager * bookmark_manager = g_object_new(GOSM_TYPE_BOOKMARK_MANAGER, NULL);
	bookmark_manager -> bookmarks = g_array_new(FALSE, FALSE, sizeof(Bookmark*));
	return bookmark_manager;
}

static void bookmark_manager_class_init(BookmarkManagerClass *class)
{
        /*bookmark_manager_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (BookmarkManagerClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void bookmark_manager_init(BookmarkManager *bookmark_manager)
{
}

GArray * bookmark_manager_get_bookmarks(BookmarkManager * bookmark_manager)
{
	return bookmark_manager -> bookmarks;
}

void bookmark_manager_add_bookmark(BookmarkManager * bookmark_manager, Bookmark * bookmark)
{
	g_array_append_val(bookmark_manager -> bookmarks, bookmark);
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
