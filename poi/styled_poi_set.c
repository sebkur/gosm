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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "styled_poi_set.h"

/****************************************************************************************************
* a StyledPoiSet extends a NamedPoiSet by an attached colour
****************************************************************************************************/
G_DEFINE_TYPE (StyledPoiSet, styled_poi_set, GOSM_TYPE_NAMED_POI_SET);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint styled_poi_set_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, styled_poi_set_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* create a new StyledPoiSet 
****************************************************************************************************/
StyledPoiSet * styled_poi_set_new(char * key, char * value, double r, double g, double b, double a)
{
	StyledPoiSet * styled_poi_set = g_object_new(GOSM_TYPE_STYLED_POI_SET, NULL);
	styled_poi_set_constructor(styled_poi_set, key, value, r, g, b, a);
	return styled_poi_set;
}

/****************************************************************************************************
* constructor
****************************************************************************************************/
void styled_poi_set_constructor(StyledPoiSet * styled_poi_set, char * key, char * value,
	double r, double g, double b, double a){
	/* call superclass constructor */
	named_poi_set_constructor(GOSM_NAMED_POI_SET(styled_poi_set), key, value);
	styled_poi_set -> r = r;
	styled_poi_set -> g = g;
	styled_poi_set -> b = b;
	styled_poi_set -> a = a;
}

static void styled_poi_set_class_init(StyledPoiSetClass *class)
{
        /*styled_poi_set_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (StyledPoiSetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void styled_poi_set_init(StyledPoiSet *styled_poi_set)
{
}

/****************************************************************************************************
* set/get colour
****************************************************************************************************/
void styled_poi_set_get_colour(StyledPoiSet * styled_poi_set, double * r, double * g, double * b, double * a)
{
	*r = styled_poi_set -> r;
	*g = styled_poi_set -> g;
	*b = styled_poi_set -> b;
	*a = styled_poi_set -> a;
}
void styled_poi_set_set_colour(StyledPoiSet * styled_poi_set, double r, double g, double b, double a)
{
	styled_poi_set -> r = r;
	styled_poi_set -> g = g;
	styled_poi_set -> b = b;
	styled_poi_set -> a = a;
}
