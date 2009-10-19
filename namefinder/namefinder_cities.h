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

#define GOSM_TYPE_NAMEFINDER_CITIES           (namefinder_cities_get_type ())
#define GOSM_NAMEFINDER_CITIES(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_NAMEFINDER_CITIES, NamefinderCities))
#define GOSM_NAMEFINDER_CITIES_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_NAMEFINDER_CITIES, NamefinderCitiesClass))
#define GOSM_IS_NAMEFINDER_CITIES(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_NAMEFINDER_CITIES))
#define GOSM_IS_NAMEFINDER_CITIES_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_NAMEFINDER_CITIES))
#define GOSM_NAMEFINDER_CITIES_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_NAMEFINDER_CITIES, NamefinderCitiesClass))

typedef struct _NamefinderCities        NamefinderCities;
typedef struct _NamefinderCitiesClass   NamefinderCitiesClass;

typedef struct {
	gchar * name;
	double lon;
	double lat;
} city;

struct _NamefinderCities
{
	GtkVBox parent;

	city * cities;
	int activated;
};

struct _NamefinderCitiesClass
{
	GtkVBoxClass parent_class;

	void (* city_activated) (NamefinderCities *namefinder_cities);
};

GtkWidget * namefinder_cities_new();

int namefinder_cities_get_activated_id(NamefinderCities * namefinder_cities);

gpointer namefinder_cities_get_cities(NamefinderCities * namefinder_cities);
