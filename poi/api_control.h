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

#ifndef _API_CONTROL_H_
#define _API_CONTROL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <curl/curl.h>

#define GOSM_TYPE_API_CONTROL           (api_control_get_type ())
#define GOSM_API_CONTROL(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_API_CONTROL, ApiControl))
#define GOSM_API_CONTROL_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_API_CONTROL, ApiControlClass))
#define GOSM_IS_API_CONTROL(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_API_CONTROL))
#define GOSM_IS_API_CONTROL_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_API_CONTROL))
#define GOSM_API_CONTROL_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_API_CONTROL, ApiControlClass))

typedef struct _ApiControl        ApiControl;
typedef struct _ApiControlClass   ApiControlClass;

struct _ApiControl
{
	GObject parent;

	gboolean sane;
	CURL * handle;
	char * send;
	int bytes_sent;
	int bytes_total;
	char * recv;
	int bytes_recv;
};

struct _ApiControlClass
{
	GObjectClass parent_class;

	//void (* function_name) (ApiControl *api_control);
};

ApiControl * api_control_new();

void api_control_test(ApiControl * api_control, double lon, double lat);

#endif /* _API_CONTROL_H_ */
