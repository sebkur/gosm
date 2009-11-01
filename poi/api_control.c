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

G_DEFINE_TYPE (ApiControl, api_control, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint api_control_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, api_control_signals[SIGNAL_NAME_n], 0);

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

size_t send_data(void * ptr, size_t size, size_t nmemb, void * data);
size_t recv_data(void * ptr, size_t size, size_t nmemb, void * data);

void api_control_test(ApiControl * api_control, double lon, double lat)
{
	CURL * handle = api_control -> handle;
	char * pwd = "zsebastian:openrevolution23";
	//curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(handle, CURLOPT_USERPWD, pwd);
	curl_easy_setopt(handle, CURLOPT_PUT, 1);
	curl_easy_setopt(handle, CURLOPT_READFUNCTION, send_data);
	curl_easy_setopt(handle, CURLOPT_READDATA, (gpointer) api_control);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, recv_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (gpointer) api_control);
	char * url;
	int perform, status;
	long response;
	// create a changeset
	url = "http://api06.dev.openstreetmap.org/api/0.6/changeset/create";
	curl_easy_setopt(handle, CURLOPT_URL, url);
	api_control -> send = 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
		<osm version=\"0.6\">\
		<changeset>\
		<tag k=\"created_by\" v=\"gosm/0.1.0\" />\
		</changeset>\
		</osm>";
	api_control -> bytes_total = strlen(api_control -> send);
	perform = curl_easy_perform(handle);
	status  = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("error\n");
		return;
	}
	int cs_id = atoi(api_control -> recv);
	free(api_control -> recv);
	api_control -> bytes_recv = 0;
	printf("changeset id %d\n", cs_id);
	// create a node
	url = "http://api06.dev.openstreetmap.org/api/0.6/node/create";
	curl_easy_setopt(handle, CURLOPT_URL, url);
	char * text1 = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
		<osm version=\"0.6\">\
		<node changeset=\"";
	char * text2 = "\" lon=\"";
	char * text3 = "\" lat=\"";
	char * text4 = "\">\
		<tag k=\"name\" v=\"Foo Inn\"/>\
		<tag k=\"amenity\" v=\"restaurant\"/>\
		</node>\
		</osm>";
	char * text = malloc(sizeof(char) * (strlen(text1) + strlen(text4) + 100));
	char * s_id = malloc(sizeof(char) * 20);
	char * s_lon = malloc(sizeof(char) * 20);
	char * s_lat = malloc(sizeof(char) * 20);
	sprintf(s_id, "%d", cs_id);
	sprintdouble(s_lon, lon, 7);
	sprintdouble(s_lat, lat, 7);
	text[0] = '\0';
	strcat(text, text1);
	strcat(text, s_id);
	strcat(text, text2);
	strcat(text, s_lon);
	strcat(text, text3);
	strcat(text, s_lat);
	strcat(text, text4);
	free(s_id); free(s_lon); free(s_lat);
	api_control -> send = text;
	printf("%s\n", api_control -> send);
	api_control -> bytes_sent = 0;
	api_control -> bytes_total = strlen(api_control -> send);
	perform = curl_easy_perform(handle);
	free(text);
	status  = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("error\n");
		return;
	}
	int node_id = atoi(api_control -> recv);
	free(api_control -> recv);
	api_control -> bytes_recv = 0;
	printf("node id %d\n", node_id);
	// close changeset
	char * url_s = "http://api06.dev.openstreetmap.org/api/0.6/changeset/%d/close";
	url = malloc(sizeof(char) * (strlen(url_s) + 20));
	sprintf(url, url_s, cs_id);
	curl_easy_setopt(handle, CURLOPT_URL, url);
	api_control -> bytes_sent = 0;
	api_control -> bytes_total = 0;
	perform = curl_easy_perform(handle);
	free(url);
	status  = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
	printf("response code: %ld\n", response);
	if (response != 200){
		printf("error\n");
		return;
	}
	printf("successfully closed changeset\n");
}

size_t send_data(void * ptr, size_t size, size_t nmemb, void * data)
{
	ApiControl * api_control = GOSM_API_CONTROL(data);
	printf("PUT requested %d bytes\n", size*nmemb);
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
