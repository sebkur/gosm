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
#include <string.h>

char * get_conf_path()
{
	char * homedir = getenv("HOME");
	char * real = realpath(homedir, NULL);
	char * conf_name = ".gosm";
	int len_real = strlen(real);
	int len_conf = strlen(conf_name);
	char * filename = malloc(sizeof(char) * (len_real + 1 + len_conf + 1));
	strncpy(filename, real, len_real);
	filename[len_real] = '/';
	strncpy(&filename[len_real+1], conf_name, len_conf);
	filename[len_real + 1 + len_conf]  = '\0';
	free(real);
	return filename;
}


int main(int argc, char *argv[])
{
	char * path = get_conf_path();
	printf("%s\n", path);
}
