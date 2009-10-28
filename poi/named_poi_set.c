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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "named_poi_set.h"

/****************************************************************************************************
* a NamedPoiSet is a PoiSet that is extended by key and value
****************************************************************************************************/
G_DEFINE_TYPE (NamedPoiSet, named_poi_set, GOSM_TYPE_POI_SET);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint named_poi_set_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, named_poi_set_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* create a new NamedPoiSet
****************************************************************************************************/
NamedPoiSet * named_poi_set_new(char * key, char * value)
{
	NamedPoiSet * named_poi_set = g_object_new(GOSM_TYPE_NAMED_POI_SET, NULL);
	named_poi_set_constructor(named_poi_set, key, value);
	return named_poi_set;
}

/****************************************************************************************************
* constructor
****************************************************************************************************/
void named_poi_set_constructor(NamedPoiSet * named_poi_set, char * key, char * value)
{
	poi_set_constructor(GOSM_POI_SET(named_poi_set));
	named_poi_set -> key = malloc(sizeof(char) * (strlen(key) + 1));
	named_poi_set -> value = malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(named_poi_set -> key, key);
	strcpy(named_poi_set -> value, value);
}

static void named_poi_set_class_init(NamedPoiSetClass *class)
{
        /*named_poi_set_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (NamedPoiSetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void named_poi_set_init(NamedPoiSet *named_poi_set)
{
}

/****************************************************************************************************
* get the key
****************************************************************************************************/
char * named_poi_set_get_key(NamedPoiSet * named_poi_set)
{
	return named_poi_set -> key;
}

/****************************************************************************************************
* get the value
****************************************************************************************************/
char * named_poi_set_get_value(NamedPoiSet * named_poi_set)
{
	return named_poi_set -> value;
}
