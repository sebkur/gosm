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

/*
 * theese are stupid functions
 * use is to convert a double to a string in english notation (e.g. 14.543)
 * and the other way around
 * the reason is, that i did not get setlocale() to work properly with sprintf()
 */

/*
 * the length of the buffer should at least have size of precision+4
 * ['-'] + '(int)d' + '.' + precision
 */
void sprintdouble(char * buf, double d, int precision);

double strtodouble(char * buf);

/* just copy file 'source' to file 'dest 
 * return 0 on success, <0 otherwise
*/
int copy_file(char * source, char * dest);
