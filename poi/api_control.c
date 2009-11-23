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

#include <curl/curl.h>

#include "api_control.h"
#include "../config/config.h"

G_DEFINE_TYPE (ApiControl, api_control, G_TYPE_OBJECT);

#define API(x) "http://api06.dev.openstreetmap.org/api/0.6/" x

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint api_control_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, api_control_signals[SIGNAL_NAME_n], 0);

extern Config * config;

ApiControl * api_control_new()
{
	ApiControl * api_control = g_object_new(GOSM_TYPE_API_CONTROL, NULL);
	api_control -> sane = TRUE;
	api_control -> handle = curl_easy_init();
	return api_control;
}

static void api_control_class_init(ApiControlClass *class)
{
        /*api_control_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ApiControlClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void api_control_init(ApiControl *api_control)
{
}

size_t send_data(void * ptr, size_t size, size_t nmemb, void * data)
{
	ApiControl * api_control = GOSM_API_CONTROL(data);
	//printf("PUT requested %d bytes\n", size*nmemb);
	if (api_control -> bytes_sent < api_control -> bytes_total){
		int bytes_left = api_control -> bytes_total - api_control -> bytes_sent;
		int requested = size * nmemb;
		int len = bytes_left <= requested ? bytes_left : requested;
		strncpy(ptr, api_control -> send + api_control -> bytes_sent, len);
		api_control -> bytes_sent += len;
		return len;
	}
	return 0;
}

size_t recv_data(void * ptr, size_t size, size_t nmemb, void * data)
{
	ApiControl * api_control = GOSM_API_CONTROL(data);
	int s = size * nmemb;
	char * recv = malloc(sizeof(char) * (s + api_control -> bytes_recv + 1));
	if (api_control -> bytes_recv > 0){
		strncpy(recv, api_control -> recv, api_control -> bytes_recv);
		free(api_control -> recv);
	}
	strncpy(recv, ptr, s);
	api_control -> recv = recv;
	return s;
}

void api_control_initialize(ApiControl * api_control)
{
	CURL * handle = api_control -> handle;
	char * user = (char*)config_get_entry_data(config, "osm_account_user");
	char * pass = (char*)config_get_entry_data(config, "osm_account_pass");
	char * pwd = g_strconcat(user, ":", pass, NULL);
	//curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(handle, CURLOPT_USERPWD, pwd);
	curl_easy_setopt(handle, CURLOPT_PUT, 1);
	curl_easy_setopt(handle, CURLOPT_READFUNCTION, send_data);
	curl_easy_setopt(handle, CURLOPT_READDATA, (gpointer) api_control);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, recv_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (gpointer) api_control);
}

int api_control_create_changeset(ApiControl * api_control)
{
	CURL * handle = api_control -> handle;
	long response;
	char * url = API("changeset/create");
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, NULL);
	api_control -> send = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<osm version=\"0.6\">\
<changeset>\
<tag k=\"created_by\" v=\"gosm/0.1.0\" />\
</changeset>\
</osm>";
	api_control -> bytes_total = strlen(api_control -> send);
	int perform = curl_easy_perform(handle);
	int status  = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("error\n");
		return -1;
	}
	int cs_id = atoi(api_control -> recv);
	free(api_control -> recv);
	api_control -> bytes_recv = 0;
	printf("changeset id %d\n", cs_id);
	return cs_id;
}

int api_control_close_changeset(ApiControl * api_control, int cs_id)
{
	CURL * handle = api_control -> handle;
	long response;
	char * url_s = API("changeset/%d/close");
	char * url = malloc(sizeof(char) * (strlen(url_s) + 20));
	sprintf(url, url_s, cs_id);
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, NULL);
	api_control -> bytes_sent = 0;
	api_control -> bytes_total = 0;
	int perform = curl_easy_perform(handle);
	free(url);
	int status  = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("error\n");
		return;
	}
	printf("successfully closed changeset\n");
}

int api_control_create_node(ApiControl * api_control, int cs_id, Node * node)
{
	CURL * handle = api_control -> handle;
	long response;
	char * url = API("node/create");
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, NULL);
	char * text1 = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<osm version=\"0.6\">\
<node changeset=\"";
	char * text2 = "\" lon=\"";
	char * text3 = "\" lat=\"";
	char * text4 = "\">";
	char * text5 = "<tag k=\"%s\" v=\"%s\"/>";
	char * text6 = "</node></osm>";
	char * s_id = malloc(sizeof(char) * 20);
	char * s_lon = malloc(sizeof(char) * 20);
	char * s_lat = malloc(sizeof(char) * 20);
	sprintf(s_id, "%d", cs_id);
	sprintdouble(s_lon, node -> lon, 7);
	sprintdouble(s_lat, node -> lat, 7);
	/* create key value strings */
	GHashTable * tags = node -> tags;
	int num_tags = g_hash_table_size(tags);
	char ** kvs = malloc(sizeof(char*) * num_tags);
	int n = 0;
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, tags);
	gpointer hash_key, hash_val;
	while(g_hash_table_iter_next(&iter, &hash_key, &hash_val)){
		char * hash_k = (char*) hash_key;
		char * hash_v = (char*) hash_val;
		kvs[n] = malloc(sizeof(char) * (strlen(text5) + 1000));
		sprintf(kvs[n], text5, hash_k, hash_v);
		n++;
	}
	/* create payload */
	char * text = malloc(sizeof(char) * (strlen(text1) + 100 + num_tags * 1100));
	text[0] = '\0';
	strcat(text, text1);
	strcat(text, s_id);
	strcat(text, text2);
	strcat(text, s_lon);
	strcat(text, text3);
	strcat(text, s_lat);
	strcat(text, text4);
	for (n = 0; n < num_tags; n++){
		strcat(text, kvs[n]);
	}
	strcat(text, text6);
	free(s_id); free(s_lon); free(s_lat);
	api_control -> send = text;
	printf("%s\n", api_control -> send);
	api_control -> bytes_sent = 0;
	api_control -> bytes_total = strlen(api_control -> send);
	int perform = curl_easy_perform(handle);
	free(text);
	for (n = 0; n < num_tags; n++){
		free(kvs[n]);
	}
	free(kvs);
	int status = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("error\n");
		return -1;
	}
	int node_id = atoi(api_control -> recv);
	free(api_control -> recv);
	api_control -> bytes_recv = 0;
	printf("node id %d\n", node_id);
	return node_id;
}

int api_control_change_delete_node(ApiControl * api_control, int cs_id, Node * node, gboolean delete)
{
	CURL * handle = api_control -> handle;
	long response;
	char * url_s = API("node/%d");
	char * url = malloc(sizeof(char) * (strlen(url_s) + 20));
	sprintf(url, url_s, node -> id);
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, NULL);
	if (delete) curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
	char * text1 = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<osm version=\"0.6\">\
<node changeset=\"";
	char * text_id = "\" id=\"";
	char * text2 = "\" lon=\"";
	char * text3 = "\" lat=\"";
	char * text_ver = "\" version=\"";
	char * text4 = "\">";
	char * text5 = "<tag k=\"%s\" v=\"%s\"/>";
	char * text6 = "</node></osm>";
	char * s_id = malloc(sizeof(char) * 20);
	char * s_node_id = malloc(sizeof(char) * 20);
	char * s_lon = malloc(sizeof(char) * 20);
	char * s_lat = malloc(sizeof(char) * 20);
	char * s_version = malloc(sizeof(char) * 20);
	sprintf(s_id, "%d", cs_id);
	sprintf(s_node_id, "%d", node -> id);
	sprintdouble(s_lon, node -> lon, 7);
	sprintdouble(s_lat, node -> lat, 7);
	sprintf(s_version, "%d", node -> version);
	/* create key value strings */
	GHashTable * tags = node -> tags;
	int num_tags = g_hash_table_size(tags);
	char ** kvs = malloc(sizeof(char*) * num_tags);
	int n = 0;
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, tags);
	gpointer hash_key, hash_val;
	while(g_hash_table_iter_next(&iter, &hash_key, &hash_val)){
		char * hash_k = (char*) hash_key;
		char * hash_v = (char*) hash_val;
		kvs[n] = malloc(sizeof(char) * (strlen(text5) + 1000));
		sprintf(kvs[n], text5, hash_k, hash_v);
		n++;
	}
	/* create payload */
	char * text = malloc(sizeof(char) * (strlen(text1) + 100 + num_tags * 1100));
	text[0] = '\0';
	strcat(text, text1);
	strcat(text, s_id);
	strcat(text, text_id);
	strcat(text, s_node_id);
	strcat(text, text2);
	strcat(text, s_lon);
	strcat(text, text3);
	strcat(text, s_lat);
	strcat(text, text_ver);
	strcat(text, s_version);
	strcat(text, text4);
	for (n = 0; n < num_tags; n++){
		strcat(text, kvs[n]);
	}
	strcat(text, text6);
	free(s_id); free(s_lon); free(s_lat);
	api_control -> send = text;
	printf("%s\n", api_control -> send);
	api_control -> bytes_sent = 0;
	api_control -> bytes_total = strlen(api_control -> send);
	int perform = curl_easy_perform(handle);
	free(text);
	for (n = 0; n < num_tags; n++){
		free(kvs[n]);
	}
	free(kvs);
	int status = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("error\n");
		return -1;
	}
	int version = atoi(api_control -> recv);
	free(api_control -> recv);
	api_control -> bytes_recv = 0;
	printf("new version %d\n", version);
	return version;
}

int api_control_change_node(ApiControl * api_control, int cs_id, Node * node)
{
	api_control_change_delete_node(api_control, cs_id, node, FALSE);
}

int api_control_delete_node(ApiControl * api_control, int cs_id, Node * node)
{
	api_control_change_delete_node(api_control, cs_id, node, TRUE);
}
