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

#ifndef _EDIT_WIDGET_H_
#define _EDIT_WIDGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "poi_manager.h"

#define GOSM_TYPE_EDIT_WIDGET           (edit_widget_get_type ())
#define GOSM_EDIT_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_EDIT_WIDGET, EditWidget))
#define GOSM_EDIT_WIDGET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_EDIT_WIDGET, EditWidgetClass))
#define GOSM_IS_EDIT_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_EDIT_WIDGET))
#define GOSM_IS_EDIT_WIDGET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_EDIT_WIDGET))
#define GOSM_EDIT_WIDGET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_EDIT_WIDGET, EditWidgetClass))

typedef struct _EditWidget        EditWidget;
typedef struct _EditWidgetClass   EditWidgetClass;

struct _EditWidget
{
	GtkVBox parent;

	PoiManager * poi_manager;
	GtkTreeView * view;

	GtkWidget * button_undo;
	GtkWidget * button_redo;
	GtkWidget * button_save;
};

struct _EditWidgetClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (EditWidget *edit_widget);
};

GtkWidget * edit_widget_new(PoiManager * poi_manager);

#endif /* _EDIT_WIDGET_H_ */
