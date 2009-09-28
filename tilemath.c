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

#include <math.h>
#include "tilemath.h"

// taken from openstreetmap/applications/mobile/rana/modules/tilenames.py


double deg_to_rad(double deg)
{
	return deg * (M_PI / 180.0);
}

double rad_to_deg(double rad)
{
	return (rad * 180.0) / M_PI;
}

double sec (double x)
{
	return 1 / cos(x);
}

int num_tiles(int zoom)
{
	return pow(2, zoom);
}

double lon_to_rel_x(double lon)
{
	return (lon + 180) / 360.0;
}

double lat_to_rel_y(double lat)
{
	return (1 - log(tan(deg_to_rad(lat)) + sec(deg_to_rad(lat))) / M_PI) / 2.0;
}

double lon_to_x(double lon, int zoom)
{
	return lon_to_rel_x(lon) * (double)num_tiles(zoom);
}

double lat_to_y(double lat, int zoom)
{
	return lat_to_rel_y(lat) * (double)num_tiles(zoom);
}

double mercator_to_lat(double merc_y)
{
	return rad_to_deg(atan(sinh(merc_y)));
}

double x_to_lon(double x, int zoom)
{
	return -180.0 + 360.0 * x / num_tiles(zoom);
}

double y_to_lat(double y, int zoom)
{
	return mercator_to_lat(M_PI * (1 - 2 * (y / (double)num_tiles(zoom))));
}

double middle_lon(double lon1, double lon2)
{
	double lon1_off = 180 + lon1;
	double lon2_off = 180 + lon2;
	double lon_mid_off = (lon1_off + lon2_off) / 2;
	double lon_mid = lon1_off < lon2_off ? lon_mid_off - 180 : lon_mid_off;
	return lon_mid;
}

double middle_lat(double lat1, double lat2)
{
	return (lat1 + lat2) / 2;
}

/*int main(int argc, char *argv[])
{
	double x = x_to_lon(1100, 11);
	x = mercator_to_lat(1484048.0);
	printf("%g\n", x);
	double y = y_to_lat(671, 11);
	y = mercator_to_lat(6891721.0);
	printf("%g\n", y);
}*/
