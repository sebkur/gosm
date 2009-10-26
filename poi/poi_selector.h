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

#ifndef _POI_SELECTOR_H_
#define _POI_SELECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "poi_manager.h"
#include "styled_poi_set.h"

#define GOSM_TYPE_POI_SELECTOR           (poi_selector_get_type ())
#define GOSM_POI_SELECTOR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_POI_SELECTOR, PoiSelector))
#define GOSM_POI_SELECTOR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_POI_SELECTOR, PoiSelectorClass))
#define GOSM_IS_POI_SELECTOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_POI_SELECTOR))
#define GOSM_IS_POI_SELECTOR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_POI_SELECTOR))
#define GOSM_POI_SELECTOR_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_POI_SELECTOR, PoiSelectorClass))

typedef struct _PoiSelector        PoiSelector;
typedef struct _PoiSelectorClass   PoiSelectorClass;

typedef struct {
	PoiSelector * poi_selector;
	StyledPoiSet * poi_set;
} PoiSelectorCheckSignalData;

struct _PoiSelector
{
	GtkVBox parent;

	GtkWidget * view;	
	PoiManager * poi_manager;
};

struct _PoiSelectorClass
{
	GtkVBoxClass parent_class;
};

GtkWidget * poi_selector_new(PoiManager * poi_manager);

int poi_selector_get_active_item_index(PoiSelector * poi_selector);

#endif /* _POI_SELECTOR_H_ */
