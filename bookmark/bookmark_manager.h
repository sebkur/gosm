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

#ifndef _BOOKMARK_MANAGER_H_
#define _BOOKMARK_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "bookmark.h"

#define GOSM_TYPE_BOOKMARK_MANAGER           (bookmark_manager_get_type ())
#define GOSM_BOOKMARK_MANAGER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_BOOKMARK_MANAGER, BookmarkManager))
#define GOSM_BOOKMARK_MANAGER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_BOOKMARK_MANAGER, BookmarkManagerClass))
#define GOSM_IS_BOOKMARK_MANAGER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_BOOKMARK_MANAGER))
#define GOSM_IS_BOOKMARK_MANAGER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_BOOKMARK_MANAGER))
#define GOSM_BOOKMARK_MANAGER_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_BOOKMARK_MANAGER, BookmarkManagerClass))

typedef struct _BookmarkManager        BookmarkManager;
typedef struct _BookmarkManagerClass   BookmarkManagerClass;

struct _BookmarkManager
{
	GObject parent;

	GArray * bookmarks;
};

struct _BookmarkManagerClass
{
	GObjectClass parent_class;

	void (* bookmark_added) (BookmarkManager *bookmark_manager, gpointer bookmark_p);
};

BookmarkManager * bookmark_manager_new();

gboolean bookmark_manager_read_bookmarks(BookmarkManager * bookmark_manager);

GArray * bookmark_manager_get_bookmarks(BookmarkManager * bookmark_manager);
void bookmark_manager_add_bookmark(BookmarkManager * bookmark_manager, Bookmark * bookmark);
gboolean bookmark_manager_save(BookmarkManager * bookmark_manager);

#endif /* _BOOKMARK_MANAGER_H_ */
