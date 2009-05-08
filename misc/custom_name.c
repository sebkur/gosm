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

#include <glib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "custom_name.h"

G_DEFINE_TYPE (CustomName, custom_name, G_TYPE_OBJECT);

/*enum
{
	TILE_LOADED_FROM_DISK,
	TILE_LOADED_FROM_NETW,
	LAST_SIGNAL
};*/

//static guint custom_name_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, custom_name_signals[TILE_LOADED_SUCCESFULLY], 0);

GObject * custom_name_new()
{
	return g_object_new(GOSM_TYPE_CUSTOM_NAME, NULL);
}

static void custom_name_class_init(CustomNameClass *class)
{
	/*custom_name_signals[TILE_LOADED_FROM_DISK] = g_signal_new(
		"tile-loaded-from-disk",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (CustomNameClass, tile_loaded_from_disk),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);*/
}

static void custom_name_init(CustomName *custom_name)
{
}

