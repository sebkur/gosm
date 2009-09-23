BIN = gosm

SRC = \
tool.c \
ringbuffer.c \
tile_cache.c \
tilemath.c \
select_tool.c \
distance_tool.c \
atlas_tool.c \
wizzard/select_use_window.c \
wizzard/tile_download_window.c \
wizzard/wizzard_download.c \
wizzard/select_export_window.c \
wizzard/wizzard_export.c \
wizzard/atlas_template_dialog.c \
wizzard/wizzard_atlas_sequence.c \
wizzard/wizzard_atlas_sequence_window.c \
wizzard/wizzard_atlas_pdf.c \
wizzard/wizzard_atlas_pdf_window.c \
uri.c \
about/about.c \
manual/manual.c \
tile_manager.c \
tile_loader.c \
map_area.c \
map_navigator.c \
imageglue/imageglue.c \
imageglue/image_glue.c \
imageglue/pdf_generator.c \
config.c \
config_widget.c \
config/color_box.c \
config/color_button.c \
customio.c \
page_math.c \
namefinder/namefinder_cities.c \
gosm.c

OBJ = \
tool.o \
ringbuffer.o \
tile_cache.o \
tilemath.o \
select_tool.o \
distance_tool.o \
atlas_tool.o \
wizzard/select_use_window.o \
wizzard/tile_download_window.o \
wizzard/wizzard_download.o \
wizzard/select_export_window.o \
wizzard/wizzard_export.o \
wizzard/atlas_template_dialog.o \
wizzard/wizzard_atlas_sequence.o \
wizzard/wizzard_atlas_sequence_window.o \
wizzard/wizzard_atlas_pdf.o \
wizzard/wizzard_atlas_pdf_window.o \
uri.o \
about/about.o \
manual/manual.o \
tile_manager.o \
tile_loader.o \
map_area.o \
map_navigator.o \
imageglue/imageglue.o \
imageglue/image_glue.o \
imageglue/pdf_generator.o \
config.o \
config_widget.o \
config/color_box.o \
config/color_button.o \
customio.o \
page_math.o \
namefinder/namefinder_cities.o \
gosm.o

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -D BUILD=0 \
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 cairo glib-2.0 gthread-2.0 libpng webkit-1.0` \
 `curl-config --cflags --libs`
LDFLAGS = -lm -lpthread \
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 cairo glib-2.0 gthread-2.0 libpng webkit-1.0` \
 `curl-config --cflags --libs`

gosm: $(OBJ)
	$(CC) $(CFLAGS) -o gosm $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dep: $(SRC)
	$(CC) -MM $(SRC) > $(DEPENDFILE)

clean:
	rm -f $(BIN) $(OBJ)

-include $(DEPENDFILE)
