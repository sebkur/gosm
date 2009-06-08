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
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>

int main(int argc, char *argv[])
{
	printf("test\n");
	char * filename = "configuration";

	struct stat sb;
	int s = stat(filename, &sb);
	if (s == -1){
		printf("config file not found\n");
	}

	int fd = open(filename, O_RDONLY);
	if (fd == -1){
		printf("config file not found\n");
		return 0;
	}

	int size = sb.st_size;
	char buf[size+1];
	read(fd, buf, size);
	close(fd);
	buf[size] = '\0';

	gchar ** splitted = g_strsplit(buf, "\n", 100);
	gchar * current = splitted[0];
	int i = 0;
	while (current != NULL){
		if (strlen(current) > 0){
			gchar ** splitline = g_strsplit(current, "\t", 2);
			gchar * part1 = g_strstrip(splitline[0]);
			gchar * part2 = g_strstrip(splitline[1]);
			printf("%s = %s\n", part1, part2);
		}
		current = splitted[++i];
	}
}
