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

#ifndef _POI_STATISTICS_H_
#define _POI_STATISTICS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "poi_manager.h"

#define GOSM_TYPE_POI_STATISTICS           (poi_statistics_get_type ())
#define GOSM_POI_STATISTICS(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_POI_STATISTICS, PoiStatistics))
#define GOSM_POI_STATISTICS_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_POI_STATISTICS, PoiStatisticsClass))
#define GOSM_IS_POI_STATISTICS(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_POI_STATISTICS))
#define GOSM_IS_POI_STATISTICS_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_POI_STATISTICS))
#define GOSM_POI_STATISTICS_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_POI_STATISTICS, PoiStatisticsClass))

typedef struct _PoiStatistics        PoiStatistics;
typedef struct _PoiStatisticsClass   PoiStatisticsClass;

typedef struct {
	char * key;
	char * val;
	int count;
} KeyValueCount;

struct _PoiStatistics
{
	GObject parent;

	GArray * key_value;
	char * current_key;
};

struct _PoiStatisticsClass
{
	GObjectClass parent_class;

	//void (* function_name) (PoiStatistics *poi_statistics);
};

PoiStatistics * poi_statistics_new();

void poi_statistics_analyze(PoiStatistics * poi_statistics, PoiManager * poi_manager);

void poi_statistics_show_window(PoiStatistics * poi_statistics);

#endif /* _POI_STATISTICS_H_ */
