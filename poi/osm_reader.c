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

#include <expat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include <bzlib.h>

#include <curl/curl.h>

#include "osm_reader.h"
#include "../map_types.h"
#include "../customio.h"


/****************************************************************************************************
* OsmReader reads in OSM-data either from a uncompressed or compressed .osm/osm.bz2 file from disk
* or from network via the OSM-API.
* it then parses the data via expat, extracts relevant data and stores in some data-structures, so
* that the current data can be efficently retrieved
****************************************************************************************************/
G_DEFINE_TYPE (OsmReader, osm_reader, G_TYPE_OBJECT);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
        READING_PROGRESS,
        READING_FINISHED,
	API_FINISHED,
        LAST_SIGNAL
};

static guint osm_reader_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
*****************************************************************************************************
* Destroy-function. used to clean up when glib-data-structures are being destroyed
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* free an int-pointer
****************************************************************************************************/
void destroy_int_p(gpointer data)
{
	//printf("%d\n", (int*)data);
	free(data);
}
/****************************************************************************************************
* free a string pointer
****************************************************************************************************/
void destroy_string(gpointer data)
{
	//printf("%s\n", (char*)data);
	free(data);
}
/****************************************************************************************************
* free a GTree
****************************************************************************************************/
void destroy_value_trees(gpointer data)
{
	//printf("DESTROY value_tree\n");
	g_tree_destroy((GTree*)data);
}
/****************************************************************************************************
* free a GArray
****************************************************************************************************/
void destroy_element_arrays(gpointer data)
{	//printf("DESTROY element array\n");
	g_array_free((GArray*)data, TRUE);
}

/****************************************************************************************************
*****************************************************************************************************
* Comparism functions. used in sorted data-structures
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* compare two strings
****************************************************************************************************/
gint osm_reader_compare_strings(gconstpointer a, gconstpointer b, gpointer user_data)
{
	return strcmp(a, b);
}

/****************************************************************************************************
* compare two ints
****************************************************************************************************/
gint osm_reader_compare_ints(gconstpointer a, gconstpointer b, gpointer user_data)
{
	return *(int*)a - *(int*)b;
}

/****************************************************************************************************
*****************************************************************************************************
* OsmReader
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* constructor
****************************************************************************************************/
void osm_reader_constructor(OsmReader * osm_reader)
{
	osm_reader -> tree_ids =  g_tree_new_full(osm_reader_compare_ints, NULL, destroy_int_p, NULL);
	osm_reader -> current_level = 0;
	osm_reader -> current_element = 0;
	osm_reader -> current_id = 0;
}

/****************************************************************************************************
* constructor
****************************************************************************************************/
OsmReader * osm_reader_new()
{
	OsmReader * osm_reader = g_object_new(GOSM_TYPE_OSM_READER, NULL);
	osm_reader_constructor(osm_reader);
	return osm_reader;
}

/****************************************************************************************************
* reset the OsmReader
****************************************************************************************************/
void osm_reader_clear(OsmReader * osm_reader)
{
	g_tree_destroy(osm_reader -> tree_ids);
	osm_reader_constructor(osm_reader);
}

/****************************************************************************************************
* class init
****************************************************************************************************/
static void osm_reader_class_init(OsmReaderClass *class)
{
        osm_reader_signals[READING_PROGRESS] = g_signal_new(
                "reading-progress",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (OsmReaderClass, reading_progress),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        osm_reader_signals[READING_FINISHED] = g_signal_new(
                "reading-finished",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (OsmReaderClass, reading_finished),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        osm_reader_signals[API_FINISHED] = g_signal_new(
                "api-finished",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (OsmReaderClass, api_finished),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
}

static void osm_reader_init(OsmReader *osm_reader)
{
}

/****************************************************************************************************
* find the value of a key in a xml-element
****************************************************************************************************/
const char * osm_reader_get_value(const XML_Char ** atts, char * search_key)
{
	const XML_Char ** ptr = atts;
	while(*ptr != NULL){
		const XML_Char * key = *ptr;
		const XML_Char * value = *(ptr + 1);
		if (strcmp(key, search_key) == 0){
			return value;
		}
		ptr = ptr + 2;
	}
	return NULL;
}

/****************************************************************************************************
* when an element has started
****************************************************************************************************/
static void XMLCALL osm_reader_StartElementCallback(	void * userData,
						const XML_Char * name,
						const XML_Char ** atts)
{
	OsmReader * osm_reader = (OsmReader*) userData;
	osm_reader -> current_level ++;
	if (osm_reader -> current_level == 2){
		if (strcmp(name, "node") == 0){
			osm_reader -> current_element = OSM_READER_ELEMENT_NODE;
			const char * id_string = osm_reader_get_value(atts, "id");
			int id = atoi(id_string);
			osm_reader -> current_id = id;
			const char * lon_s = osm_reader_get_value(atts, "lon");
			const char * lat_s = osm_reader_get_value(atts, "lat");
			double lon = strtodouble(lon_s);
			double lat = strtodouble(lat_s);
			GHashTable * tags = g_hash_table_new_full(g_str_hash, g_str_equal, destroy_string, destroy_string);
			LonLatTags * llt = malloc(sizeof(LonLatTags));
			llt -> lon = lon;
			llt -> lat = lat;
			llt -> tags = tags;
			osm_reader -> current_node = llt;
		}else{
			osm_reader -> current_element = OSM_READER_ELEMENT_OTHER;
		}
	}
	if(osm_reader -> current_level == 3 && osm_reader -> current_element == OSM_READER_ELEMENT_NODE){
		if (strcmp(name, "tag") == 0){
			const char * k = osm_reader_get_value(atts, "k");
			if (strcmp(k, "created_by") != 0){
				const char * v = osm_reader_get_value(atts, "v");
				int len_k = strlen(k) + 1;
				int len_v = strlen(v) + 1;
				/* put into node's hashmap */
				char * key_map = malloc(sizeof(char) * len_k);
				char * val_map = malloc(sizeof(char) * len_v);
				strcpy(key_map, k);
				strcpy(val_map, v);
				g_hash_table_insert(osm_reader -> current_node -> tags, key_map, val_map);
			}
		}
	}
	//printf("%s\n", name);
}

/****************************************************************************************************
* when an element has ended
****************************************************************************************************/
static void XMLCALL osm_reader_EndElementCallback(	void * userData,
					const XML_Char * name)
{
	OsmReader * osm_reader = (OsmReader*) userData;
	osm_reader -> current_level --;
	if (osm_reader -> current_level == 1 && osm_reader -> current_element == OSM_READER_ELEMENT_NODE){
		if (g_hash_table_size(osm_reader -> current_node -> tags) > 0){
			/* insert finished node into tree holding nodes, sorted by ids */
			int * id_insert = malloc(sizeof(int));
			*id_insert = osm_reader -> current_id;
			g_tree_insert(osm_reader -> tree_ids, id_insert, osm_reader -> current_node);
		}else{
			/* the last node had no relevant tags
			   -> it is not inserted into tree */
			g_hash_table_destroy(osm_reader -> current_node -> tags);
			free(osm_reader -> current_node);
		}
	}
}

/****************************************************************************************************
* method declaration
****************************************************************************************************/
void osm_reader_parse_file_thread_fun(OsmReader * osm_reader);

/****************************************************************************************************
* parse a file
****************************************************************************************************/
int osm_reader_parse_file(OsmReader * osm_reader, char * filename)
{
	osm_reader -> filename = filename;
	pthread_t thread;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	size_t stacksize = PTHREAD_STACK_MIN * 8;
	pthread_attr_setstacksize(&tattr, stacksize);
	int p_id = pthread_create(&thread, &tattr, (void*) osm_reader_parse_file_thread_fun, osm_reader);
	pthread_attr_destroy(&tattr);
}

/****************************************************************************************************
* actual parsing is done in a seperate thread
****************************************************************************************************/
void osm_reader_parse_file_thread_fun(OsmReader * osm_reader)
{
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, (void*)osm_reader);
	XML_SetStartElementHandler(parser, osm_reader_StartElementCallback);
	XML_SetEndElementHandler(parser, osm_reader_EndElementCallback);

	struct stat sb;
	int s = stat(osm_reader -> filename, &sb);
	if (s < 0){
		// file not found
		XML_ParserFree(parser);
		g_signal_emit (osm_reader, osm_reader_signals[READING_FINISHED], 0, 1);
		return;
	}
	int filesize = sb.st_size;
	int done_bytes = 0;
	int done_signaled = 0;
	double done_percent = 0;

	gboolean is_bz2 = g_str_has_suffix(osm_reader -> filename, ".bz2");

	if (!is_bz2){
		// uncompressed .osm file
		int f = open(osm_reader -> filename, O_RDONLY);
		if (f == 0) {
			g_signal_emit (osm_reader, osm_reader_signals[READING_FINISHED], 0, 1);
			return;
		}
		ssize_t size;
		int n = 1000;
		char buf[n];
		do{
			size = read(f, buf, n);
			XML_Parse(parser, buf, size, 0);
			done_bytes += size;
			done_percent = ((double)done_bytes) / ((double)filesize);
			int done_percent_int = (int) (done_percent * 100);
			if (done_percent_int > done_signaled || done_percent_int == 100){
				done_signaled = done_percent_int;
				//printf("done %d\n", done_percent_int);
				g_signal_emit (osm_reader, osm_reader_signals[READING_PROGRESS], 0, done_percent_int);
			}
		}while(size > 0);
		XML_Parse(parser, NULL, 0, 1);
	}else{
		// compressed .osm.bz2 file
		FILE * file = fopen(osm_reader -> filename, "r");
		int bzError;
		int unused;
		BZFILE * bz = BZ2_bzReadOpen(&bzError, file, 0, 0, &unused, 0);
		int len = 1000;
		char buf[len+1];
		int size;
		long last_pos;
		do{
			size = BZ2_bzRead(&bzError, bz, buf, len);
			XML_Parse(parser, buf, size, 0);
			long pos = ftell(file);
			if (pos > last_pos){
				last_pos = pos;
				done_bytes = last_pos;
				done_percent = ((double)done_bytes) / ((double)filesize);
				int done_percent_int = (int) (done_percent * 100);
				if (done_percent_int > done_signaled || done_percent_int == 100){
					done_signaled = done_percent_int;
					//printf("done %d\n", done_percent_int);
					g_signal_emit (osm_reader, osm_reader_signals[READING_PROGRESS], 0, done_percent_int);
				}
			}
		}while(size > 0);
		BZ2_bzReadClose(&bzError, bz);
		fclose(file);
		XML_Parse(parser, NULL, 0, 1);
	}
	XML_ParserFree(parser);
	g_signal_emit (osm_reader, osm_reader_signals[READING_FINISHED], 0, 0);
}

/****************************************************************************************************
* a callback for curl-downloading. used for displaying progress
****************************************************************************************************/
size_t curl_cb(void * data, size_t size, size_t nmemb, void * parser)
{
	//char * buf = malloc(sizeof(char) * (size * nmemb + 1));
	//strncpy(buf, data, size * nmemb);
	//buf[size * nmemb] = '\0';
	//printf("%s\n", buf);
	//XML_Parse((XML_Parser)parser, buf, size * nmemb, 0);
	XML_Parse((XML_Parser)parser, data, size * nmemb, 0);
	return size * nmemb;
}

/****************************************************************************************************
* method declaration
****************************************************************************************************/
void osm_reader_parse_api_url_thread_fun(OsmReader * osm_reader);

/****************************************************************************************************
* start an api-request and parse data
****************************************************************************************************/
void osm_reader_parse_api_url(OsmReader * osm_reader, char * url)
{
	osm_reader -> url = url;
	pthread_t thread;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	size_t stacksize = PTHREAD_STACK_MIN * 8;
	pthread_attr_setstacksize(&tattr, stacksize);
	int p_id = pthread_create(&thread, &tattr, (void*) osm_reader_parse_api_url_thread_fun, osm_reader);
	pthread_attr_destroy(&tattr);
}

/****************************************************************************************************
* actual downloading and parsing is done in another thread
****************************************************************************************************/
void osm_reader_parse_api_url_thread_fun(OsmReader * osm_reader)
{
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, (void*)osm_reader);
	XML_SetStartElementHandler(parser, osm_reader_StartElementCallback);
	XML_SetEndElementHandler(parser, osm_reader_EndElementCallback);

	CURL * handle = curl_easy_init();
	//curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(handle, CURLOPT_URL, osm_reader -> url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_cb);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (gpointer)parser);
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Pure Data");
	int perform = curl_easy_perform(handle);
	free(osm_reader -> url);
	long response;
	int status  = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("unable to receive data\n");
		XML_ParserFree(parser);
		g_signal_emit (osm_reader, osm_reader_signals[API_FINISHED], 0, 1);
		return;
	}
	printf("succesfully got data\n");
	XML_Parse(parser, NULL, 0, 1);
	XML_ParserFree(parser);
	curl_easy_cleanup(handle);
	g_signal_emit (osm_reader, osm_reader_signals[API_FINISHED], 0, 0);
}
