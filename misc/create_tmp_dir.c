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

//#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *fn = "/tmp/osm";
	struct stat info;
	int r_stat = stat(fn, &info);
	if (r_stat){
		printf("creating directory: %s\n", fn);
		int r_mkdir = mkdir(fn, 0777);
	}
	r_stat = stat(fn, &info);
	if (r_stat || !S_ISDIR(info.st_mode)){
		printf("could not create chache directory: %s\n", fn);
		return EXIT_FAILURE;
	}
}
