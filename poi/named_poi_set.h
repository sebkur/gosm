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

#ifndef _NAMED_POI_SET_H_
#define _NAMED_POI_SET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "poi_set.h"

#define GOSM_TYPE_NAMED_POI_SET           (named_poi_set_get_type ())
#define GOSM_NAMED_POI_SET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_NAMED_POI_SET, NamedPoiSet))
#define GOSM_NAMED_POI_SET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_NAMED_POI_SET, NamedPoiSetClass))
#define GOSM_IS_NAMED_POI_SET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_NAMED_POI_SET))
#define GOSM_IS_NAMED_POI_SET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_NAMED_POI_SET))
#define GOSM_NAMED_POI_SET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_NAMED_POI_SET, NamedPoiSetClass))

typedef struct _NamedPoiSet        NamedPoiSet;
typedef struct _NamedPoiSetClass   NamedPoiSetClass;

struct _NamedPoiSet
{
	PoiSet parent;

	char * key;
	char * value;
};

struct _NamedPoiSetClass
{
	PoiSetClass parent_class;

	//void (* function_name) (NamedPoiSet *named_poi_set);
};

NamedPoiSet * named_poi_set_new(char * key, char * value);
void named_poi_set_constructor(NamedPoiSet * named_poi_set, char * key, char * value);

char * named_poi_set_get_key(NamedPoiSet * named_poi_set);

char * named_poi_set_get_value(NamedPoiSet * named_poi_set);

#endif /* _NAMED_POI_SET_H_ */
