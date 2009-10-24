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

#ifndef _GOSM_OSM_READER_H_
#define _GOSM_OSM_READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../map_types.h"

#define OSM_READER_ELEMENT_NODE 1
#define OSM_READER_ELEMENT_OTHER 2

#define GOSM_TYPE_OSM_READER           (osm_reader_get_type ())
#define GOSM_OSM_READER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_OSM_READER, OsmReader))
#define GOSM_OSM_READER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_OSM_READER, OsmReaderClass))
#define GOSM_IS_OSM_READER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_OSM_READER))
#define GOSM_IS_OSM_READER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_OSM_READER))
#define GOSM_OSM_READER_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_OSM_READER, OsmReaderClass))

typedef struct _OsmReader        OsmReader;
typedef struct _OsmReaderClass   OsmReaderClass;

struct _OsmReader
{
	GObject parent;
	
	GTree * tree_tags;
	GTree * tree_ids;
	int current_level;
	int current_element;
	int current_id;
	LonLatTags * current_node;
	char * filename;
	char * url;
};

struct _OsmReaderClass
{
	GObjectClass parent_class;

	void (* reading_progress) (OsmReader *osm_reader, int percent);
	void (* reading_finished) (OsmReader *osm_reader, int status);
	void (* api_finished) (OsmReader *osm_reader, int status);
};

OsmReader * osm_reader_new();
void osm_reader_clear(OsmReader * osm_reader);

int osm_reader_parse_file(OsmReader * osm_reader, char * filename);

GArray * osm_reader_find_ids_key_value(OsmReader * osm_reader, char * key, char * value);

void osm_reader_parse_api_url(OsmReader * osm_reader, char * url);

#endif /* _GOSM_OSM_READER_H_ */
