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

// taken from openstreetmap/applications/mobile/rana/modules/tilenames.py

double deg_to_rad(double deg);

double rad_to_deg(double rad);

double sec (double x);

int num_tiles(int zoom);

double lon_to_rel_x(double lon);

double lat_to_rel_y(double lat);

double lon_to_x(double lon, int zoom);

double lat_to_y(double lat, int zoom);

double mercator_to_lat(double merc_y);

double x_to_lon(double x, int zoom);

double y_to_lat(double y, int zoom);

double middle_lon(double lon1, double lon2);

double middle_lat(double lat1, double lat2);
