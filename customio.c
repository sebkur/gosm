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
#include <math.h>

void sprintdouble(char * buf, double d, int precision)
{
	int z = (int)d;
	sprintf(buf, "%d.", z);
	int pos = strlen(buf);
	double x = fabs(d - z);
	int i; int y;
	for (i = 0; i < precision; i++){
		x *= 10;
		y = (int) x;
		x -= y;
		sprintf(&buf[pos+i], "%d", y);
	}
}

double strtodouble(char * buf)
{
	int pos = 0;
	int negative = 0;
	double result = 0.0;
	int dot = 0;
	int factor = 10;
	char * ptr = buf;
	while(*ptr != '\0'){
		char c = *ptr;
		pos++;
		if (pos == 1){
			if (c == '-'){
				negative = 1; 
				ptr++;
				continue;
			}
		}
		if (!dot){
			switch(c){
				case '.': dot = 1; break;
				case '0': result *= 10; result += 0; break;
				case '1': result *= 10; result += 1; break;
				case '2': result *= 10; result += 2; break;
				case '3': result *= 10; result += 3; break;
				case '4': result *= 10; result += 4; break;
				case '5': result *= 10; result += 5; break;
				case '6': result *= 10; result += 6; break;
				case '7': result *= 10; result += 7; break;
				case '8': result *= 10; result += 8; break;
				case '9': result *= 10; result += 9; break;
				default: return negative ? -result : result;
			}
		}else{
			switch(c){
				case '.': return negative ? -result : result; 
				case '0': result += 0.0 / factor; factor *= 10; break;
				case '1': result += 1.0 / factor; factor *= 10; break;
				case '2': result += 2.0 / factor; factor *= 10; break;
				case '3': result += 3.0 / factor; factor *= 10; break;
				case '4': result += 4.0 / factor; factor *= 10; break;
				case '5': result += 5.0 / factor; factor *= 10; break;
				case '6': result += 6.0 / factor; factor *= 10; break;
				case '7': result += 7.0 / factor; factor *= 10; break;
				case '8': result += 8.0 / factor; factor *= 10; break;
				case '9': result += 9.0 / factor; factor *= 10; break;
				default: return negative ? -result : result;
			}
		}
		ptr++;
	}
	return negative ? -result : result;
}
/*
int main(int argc, char *argv[])
{
	char * buf = malloc(sizeof(char) * 20);
	sprintdouble(buf, 12.7652, 4);
	double d = strtodouble("12");
	printf("%s\n", buf);
	printf("%f\n", d);
}
*/
