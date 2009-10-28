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

#ifndef _CONFIG_WIDGET_H_
#define _CONFIG_WIDGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config.h"

#define GOSM_TYPE_CONFIG_WIDGET           (config_widget_get_type ())
#define GOSM_CONFIG_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_CONFIG_WIDGET, ConfigWidget))
#define GOSM_CONFIG_WIDGET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_CONFIG_WIDGET, ConfigWidgetClass))
#define GOSM_IS_CONFIG_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_CONFIG_WIDGET))
#define GOSM_IS_CONFIG_WIDGET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_CONFIG_WIDGET))
#define GOSM_CONFIG_WIDGET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_CONFIG_WIDGET, ConfigWidgetClass))

typedef struct _ConfigWidget        ConfigWidget;
typedef struct _ConfigWidgetClass   ConfigWidgetClass;

struct _ConfigWidget
{
	GtkVBox parent;

	GtkWidget * button_confirm;
	GtkWidget * button_cancel;
	
	Config * config;
	GtkWidget ** entries;
};

struct _ConfigWidgetClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (ConfigWidget *config_widget);
};

GtkWidget * config_widget_new(Config * config);

gboolean * config_widget_get_new_configuration(ConfigWidget *config_widget);

#endif /* _CONFIG_WIDGET_H_ */
