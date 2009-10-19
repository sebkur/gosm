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

#define GOSM_TYPE_NAMEFINDER_COUNTRIES           (namefinder_countries_get_type ())
#define GOSM_NAMEFINDER_COUNTRIES(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_NAMEFINDER_COUNTRIES, NamefinderCountries))
#define GOSM_NAMEFINDER_COUNTRIES_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_NAMEFINDER_COUNTRIES, NamefinderCountriesClass))
#define GOSM_IS_NAMEFINDER_COUNTRIES(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_NAMEFINDER_COUNTRIES))
#define GOSM_IS_NAMEFINDER_COUNTRIES_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_NAMEFINDER_COUNTRIES))
#define GOSM_NAMEFINDER_COUNTRIES_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_NAMEFINDER_COUNTRIES, NamefinderCountriesClass))

typedef struct _NamefinderCountries        NamefinderCountries;
typedef struct _NamefinderCountriesClass   NamefinderCountriesClass;

typedef struct {
	gchar * name;
	double lon;
	double lat;
	double lon1;
	double lat1;
	double lon2;
	double lat2;
} country;

struct _NamefinderCountries
{
	GtkVBox parent;

	country * countries;
	int activated;
};

struct _NamefinderCountriesClass
{
	GtkVBoxClass parent_class;

	void (* country_activated) (NamefinderCountries *namefinder_countries);
};

GtkWidget * namefinder_countries_new();

int namefinder_countries_get_activated_id(NamefinderCountries * namefinder_countries);

gpointer namefinder_countries_get_countries(NamefinderCountries * namefinder_countries);
