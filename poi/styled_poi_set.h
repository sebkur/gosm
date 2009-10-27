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

#ifndef _STYLED_POI_SET_H_
#define _STYLED_POI_SET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "named_poi_set.h"

#define GOSM_TYPE_STYLED_POI_SET           (styled_poi_set_get_type ())
#define GOSM_STYLED_POI_SET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_STYLED_POI_SET, StyledPoiSet))
#define GOSM_STYLED_POI_SET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_STYLED_POI_SET, StyledPoiSetClass))
#define GOSM_IS_STYLED_POI_SET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_STYLED_POI_SET))
#define GOSM_IS_STYLED_POI_SET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_STYLED_POI_SET))
#define GOSM_STYLED_POI_SET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_STYLED_POI_SET, StyledPoiSetClass))

typedef struct _StyledPoiSet        StyledPoiSet;
typedef struct _StyledPoiSetClass   StyledPoiSetClass;

struct _StyledPoiSet
{
	NamedPoiSet parent;

	double r;
	double g;
	double b;
	double a;
};

struct _StyledPoiSetClass
{
	NamedPoiSetClass parent_class;

	//void (* function_name) (StyledPoiSet *styled_poi_set);
};

StyledPoiSet * styled_poi_set_new(char * key, char * value, double r, double g, double b, double a);
void styled_poi_set_constructor(StyledPoiSet * styled_poi_set, char * key, char * value,
	double r, double g, double b, double a);

void styled_poi_set_get_colour(StyledPoiSet * styled_poi_set, double * r, double * g, double * b, double * a);
void styled_poi_set_set_colour(StyledPoiSet * styled_poi_set, double r, double g, double b, double a);

#endif /* _STYLED_POI_SET_H_ */
