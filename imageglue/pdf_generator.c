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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <pthread.h>

#include <math.h>

#include "pdf_generator.h"

#include "../atlas/atlas.h" 
#include "../map_types.h"
#include "../tiles/tilemath.h"
#include "../paths.h"
#include "image_glue.h"

/****************************************************************************************************
* PdfGenerator takes a sequence of images as input and creates a PDF from it.
* for this purpose, a new process is spawned, that executes java to use the iText library to generate
* the document
****************************************************************************************************/
G_DEFINE_TYPE (PdfGenerator, pdf_generator, G_TYPE_OBJECT);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
        PAGE_PROGRESS,
        LAST_SIGNAL
};

static guint pdf_generator_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* method declarations
****************************************************************************************************/
void pdf_generator_thread_function(PdfGenerator * pdf_generator);

/****************************************************************************************************
* constructor
****************************************************************************************************/
PdfGenerator * pdf_generator_new()
{
	PdfGenerator * pdf_generator = g_object_new(GOSM_TYPE_PDF_GENERATOR, NULL);
	return pdf_generator;
}

static void pdf_generator_class_init(PdfGeneratorClass *class)
{
        pdf_generator_signals[PAGE_PROGRESS] = g_signal_new(
                "page-progress",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PdfGeneratorClass, page_progress),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
}

static void pdf_generator_init(PdfGenerator *pdf_generator)
{
}

/****************************************************************************************************
* set the path to the java binary that PdfGenerator should use
****************************************************************************************************/
void pdf_generator_set_java_binary(PdfGenerator * pdf_generator, char * path)
{
	if (pdf_generator -> java_binary != NULL) free (pdf_generator -> java_binary);
	pdf_generator -> java_binary = malloc(sizeof(char) * (strlen(path) + 1));
	strcpy(pdf_generator -> java_binary, path);
}

/****************************************************************************************************
* setup PdfGenerator for processing some files
****************************************************************************************************/
void pdf_generator_setup(PdfGenerator * pdf_generator, PageInformation page_info, char * file_prefix, int parts_x, int parts_y)
{
	pdf_generator -> page_info = page_info;
	pdf_generator -> file_prefix = file_prefix;
	pdf_generator -> parts_x = parts_x;
	pdf_generator -> parts_y = parts_y;
}

/****************************************************************************************************
* create a new thread for the actual processing
****************************************************************************************************/
void pdf_generator_process(PdfGenerator * pdf_generator)
{
	pthread_t thread;
	int p = pthread_create(&thread, NULL, (void *) pdf_generator_thread_function, pdf_generator);
}

/****************************************************************************************************
* do the actual work here
* prepare command line arguments and execute java to build PDF
****************************************************************************************************/
void pdf_generator_thread_function(PdfGenerator * pdf_generator)
{
	PageInformation page_info = pdf_generator -> page_info;
	char * file_prefix = pdf_generator -> file_prefix;
	int parts_x = pdf_generator -> parts_x;
	int parts_y = pdf_generator -> parts_y;

	int a,b;
	int buffer_len = strlen(file_prefix) + 1 + 8 + 1 + 8 + 4 + 1;
	//  ^----------: /some/path/to/prefix  _   x   _   y .png  \0 ... quite pessimitic 8 chars for x and y values
	int num_args = 11 + parts_x * parts_y;
	char **args = malloc(num_args * sizeof(char*));
	for (a = 0; a < parts_x; a++){
		for (b = 0; b < parts_y; b++){
			int pos = 10 + b * parts_x + a;
			args[pos] = malloc(buffer_len * sizeof(char));
			sprintf(args[pos], "%s_%d_%d.png", file_prefix, a+1, b+1);
		}
	}
	// java -cp imageglue/pdf_creator/iText-2.1.5.jar:imageglue/pdf_creator/ test.Exporter 210 297 150 15 20 a.pdf image_1_1.png image_2_1.png
	args[0] = malloc(sizeof(char) * (strlen(pdf_generator -> java_binary) + 1));
	sprintf(args[0], "%s", pdf_generator -> java_binary);
	args[1] = malloc(30 * sizeof(char));
	sprintf(args[1], "%s", "-cp");
	args[2] = malloc(80 * sizeof(char));
	sprintf(args[2], "%s", GOSM_IMAGEGLUE_DIR "pdf_creator/iText-2.1.5.jar:" GOSM_IMAGEGLUE_DIR "pdf_creator/");
	args[3] = malloc(30 * sizeof(char));
	sprintf(args[3], "%s", "pdf.Exporter");
	args[4] = malloc(10 * sizeof(char));
	sprintf(args[4], "%d", page_info.page_width);
	args[5] = malloc(10 * sizeof(char));
	sprintf(args[5], "%d", page_info.page_height);
	args[6] = malloc(10 * sizeof(char));
	sprintf(args[6], "%d", page_info.resolution);
	args[7] = malloc(10 * sizeof(char));
	sprintf(args[7], "%d", page_info.border_left);
	args[8] = malloc(10 * sizeof(char));
	sprintf(args[8], "%d", page_info.border_bottom);
	args[9] = malloc(buffer_len * sizeof(char));
	sprintf(args[9], "%s.pdf", file_prefix);
	args[10 + parts_x * parts_y] = NULL;

	char buf[1000 + (parts_x * parts_y) * buffer_len]; buf[0] = '\0';
	int i; for (i = 0; i < num_args - 1; i++){
		strcat(buf, args[i]);
		strcat(buf, " ");
	}
	printf("%s\n", buf);
	
	int pipefd[2];
	pipe(pipefd);

	pid_t pid = fork();
	if (pid == 0){
		dup2(pipefd[1], 1);
		close(pipefd[0]);
		execv(pdf_generator -> java_binary, args);
	}
	close(pipefd[1]);
	char buffer[1];
	char line[100];
	int lc = 0;
	while (read(pipefd[0], &buffer, 1) > 0){
		line[lc++] = *buffer;
		if (buffer[0] == '\n'){
			line[lc] = '\0';
			printf("received %s", line);
			g_signal_emit (G_OBJECT(pdf_generator), pdf_generator_signals[PAGE_PROGRESS], 0, 1);
			lc = 0;
		}
	}
}

/*void pdf_generator_generate(PdfGenerator * pdf_generator, PageInformation page_info, char * file_prefix, int parts_x, int parts_y)
{
	int a,b;
	int buffer_len = strlen(file_prefix) + 1 + 8 + 1 + 8 + 4 + 1;
	//  ^----------: /some/path/to/prefix  _   x   _   y .png  \0 ... quite pessimitic 8 chars for x and y values
	int num_args = 11 + parts_x * parts_y;
	char **args = malloc(num_args * sizeof(char*));
	for (a = 0; a < parts_x; a++){
		for (b = 0; b < parts_y; b++){
			int pos = 10 + b * parts_x + a;
			args[pos] = malloc(buffer_len * sizeof(char));
			sprintf(args[pos], "%s_%d_%d.png", file_prefix, a+1, b+1);
		}
	}
	// java -cp imageglue/pdf_creator/iText-2.1.5.jar:imageglue/pdf_creator/ test.Exporter 210 297 150 15 20 a.pdf image_1_1.png image_2_1.png
	args[0] = malloc(30 * sizeof(char));
	sprintf(args[0], "%s", "/usr/bin/java");
	args[1] = malloc(30 * sizeof(char));
	sprintf(args[1], "%s", "-cp");
	args[2] = malloc(80 * sizeof(char));
	sprintf(args[2], "%s", "imageglue/pdf_creator/iText-2.1.5.jar:imageglue/pdf_creator/");
	args[3] = malloc(30 * sizeof(char));
	sprintf(args[3], "%s", "test.Exporter");
	args[4] = malloc(10 * sizeof(char));
	sprintf(args[4], "%d", page_info.page_width);
	args[5] = malloc(10 * sizeof(char));
	sprintf(args[5], "%d", page_info.page_height);
	args[6] = malloc(10 * sizeof(char));
	sprintf(args[6], "%d", page_info.resolution);
	args[7] = malloc(10 * sizeof(char));
	sprintf(args[7], "%d", page_info.border_left);
	args[8] = malloc(10 * sizeof(char));
	sprintf(args[8], "%d", page_info.border_bottom);
	args[9] = malloc(buffer_len * sizeof(char));
	sprintf(args[9], "%s.pdf", file_prefix);
	args[10 + parts_x * parts_y] = NULL;

	char buf[1000 + (parts_x * parts_y) * buffer_len]; buf[0] = '\0';
	int i; for (i = 0; i < num_args - 1; i++){
		strcat(buf, args[i]);
		strcat(buf, " ");
	}
	printf("%s\n", buf);
	
	pid_t pid = fork();
	if (pid == 0){
		execv("/usr/bin/java", args);
	}
	int status;
	wait(&status);
}*/

/****************************************************************************************************
* an old testing function
* TODO: remove this function, it's not used anywhere
****************************************************************************************************/
void generate_pdf(PdfGenerator * pdf_gen, char * cache_dir, Selection selection, PageInformation page_info,
	ImageDimension image_dimension, int intersect_x, int intersect_y, int zoom)
{
	double x1_d = lon_to_x(selection.lon1, zoom);
	double x2_d = lon_to_x(selection.lon2, zoom);
	double y1_d = lat_to_y(selection.lat1, zoom);
	double y2_d = lat_to_y(selection.lat2, zoom);
	int x1 = (int) x1_d;
	int x2 = (int) x2_d;
	int y1 = (int) y1_d;
	int y2 = (int) y2_d;
	int x1_o = (x1_d - x1) * 256;
	int x2_o = (x2_d - x2) * 256;
	int y1_o = (y1_d - y1) * 256;
	int y2_o = (y2_d - y2) * 256;
	//printf("%d %d %d %d\n", x1, x2, y1, y2);
	//printf("%d %d %d %d\n", x1_o, x2_o, y1_o, y2_o);
	int width  = (x2-x1+1) * 256 - x1_o - 256 + x2_o;
	int height = (y2-y1+1) * 256 - y1_o - 256 + y2_o;
	int parts_x = 1 + ceil(((double)(width - image_dimension.width)) / (image_dimension.width - intersect_x));
	int parts_y = 1 + ceil(((double)(height - image_dimension.height)) / (image_dimension.height - intersect_y));
	printf("%d, %d\n", parts_x, parts_y);
	int a,b;
	int step_x = image_dimension.width - intersect_x;
	int step_y = image_dimension.height - intersect_y;
	char buf[60];
	for (a = 0; a < parts_x; a++){
		int s_x = x1 + (x1_o + a * step_x) / 256;
		int s_x_o = (x1_o + a * step_x) % 256;
		int e_x = s_x + (s_x_o + image_dimension.width) / 256;
		int e_x_o = (s_x_o + image_dimension.width) % 256;
		//printf("%d,%d,%d,%d\n", s_x, s_x_o, e_x, e_x_o);
		for (b = 0; b < parts_y; b++){
			int s_y = y1 + (y1_o + b * step_y) / 256;
			int s_y_o = (y1_o + b * step_y) % 256;
			int e_y = s_y + (s_y_o + image_dimension.height) / 256;
			int e_y_o = (s_y_o + image_dimension.height) % 256;
			sprintf(buf, "image_%d_%d.png", a+1, b+1);
			printf("%s\n", buf);
			ImageGlue * image_glue = image_glue_new();
			make_image_1(image_glue, buf, cache_dir, zoom, s_x, e_x, s_y, e_y, s_x_o, e_x_o, s_y_o, e_y_o);
		}
	}
	// create PDF
	//int page_width = page_info.page_orientation == ORIENTATION_PORTRAIT ? page_info.page_width : page_info.page_height;
	//int page_height = page_info.page_orientation == ORIENTATION_PORTRAIT ? page_info.page_height : page_info.page_width;
	
	char **args = malloc((11 + parts_x * parts_y)*sizeof(char*));
	for (a = 0; a < parts_x; a++){
		for (b = 0; b < parts_y; b++){
			int pos = 10 + b * parts_x + a;
			args[pos] = malloc(30 * sizeof(char));
			sprintf(args[pos], "image_%d_%d.png", a+1, b+1);
		}
	}
	// TODO: insert arguments correctly from atlas_tool
	// java -cp imageglue/pdf_creator/iText-2.1.5.jar:imageglue/pdf_creator/ test.Exporter 210 297 150 15 20 a.pdf image_1_1.png image_2_1.png
	args[0] = malloc(30 * sizeof(char));
	sprintf(args[0], "%s", "/usr/bin/java");
	args[1] = malloc(30 * sizeof(char));
	sprintf(args[1], "%s", "-cp");
	args[2] = malloc(80 * sizeof(char));
	sprintf(args[2], "%s", "imageglue/pdf_creator/iText-2.1.5.jar:imageglue/pdf_creator/");
	args[3] = malloc(30 * sizeof(char));
	sprintf(args[3], "%s", "test.Exporter");
	args[4] = malloc(10 * sizeof(char));
	sprintf(args[4], "%d", page_info.page_width);
	args[5] = malloc(10 * sizeof(char));
	sprintf(args[5], "%d", page_info.page_height);
	args[6] = malloc(10 * sizeof(char));
	sprintf(args[6], "%d", page_info.resolution);
	args[7] = malloc(10 * sizeof(char));
	sprintf(args[7], "%d", page_info.border_left);
	args[8] = malloc(10 * sizeof(char));
	sprintf(args[8], "%d", page_info.border_bottom);
	args[9] = malloc(30 * sizeof(char));
	sprintf(args[9], "%s", "a.pdf");
	args[10 + parts_x * parts_y] = NULL;
	
	pid_t pid = fork();
	if (pid == 0){
		execv("/usr/bin/java", args);
	}
	int status;
	wait(&status);
}
