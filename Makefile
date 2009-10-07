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
wizzard/wizzard_delete_tiles.c \
wizzard/wizzard_delete_tiles_window.c \
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
namefinder/namefinder_countries.c \
poi/poi_set.c \
poi/RTree/card.c \
poi/RTree/gammavol.c \
poi/RTree/index.c \
poi/RTree/node.c \
poi/RTree/rect.c \
poi/RTree/sphvol.c \
poi/RTree/split_l.c \
poi/osm_reader.c \
poi/poi_selector.c \
poi/poi_manager.c \
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
wizzard/wizzard_delete_tiles.o \
wizzard/wizzard_delete_tiles_window.o \
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
namefinder/namefinder_countries.o \
poi/poi_set.o \
poi/RTree/card.o \
poi/RTree/gammavol.o \
poi/RTree/index.o \
poi/RTree/node.o \
poi/RTree/rect.o \
poi/RTree/sphvol.o \
poi/RTree/split_l.o \
poi/osm_reader.o \
poi/poi_selector.o \
poi/poi_manager.o \
gosm.o

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -D BUILD=0 -g\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 libpng webkit-1.0` \
 `curl-config --cflags --libs`
LDFLAGS = -lm -lpthread -lexpat\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 libpng webkit-1.0` \
 `curl-config --cflags --libs`

gosm: $(OBJ)
	$(CC) $(CFLAGS) -o gosm $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dep: $(SRC)
	$(CC) -MM $(SRC) > $(DEPENDFILE)

clean:
	rm -f $(BIN) $(OBJ)

# PACKAGING

VERSION := $(shell cat VERSION)
DIRNAME = $(shell basename `pwd`)

packages: tar deb

tar:
	cd .. &&\
	tar --exclude=*.svn* --exclude=*.o --exclude=packages/* -cvzf $(DIRNAME)/packages/gosm.$(VERSION).tar.gz $(DIRNAME)

deb:
	rm -rf packages/deb 
	mkdir packages/deb 
	mkdir packages/deb/DEBIAN 
	mkdir packages/deb/usr 
	mkdir packages/deb/usr/bin 
	mkdir packages/deb/usr/share 
	mkdir packages/deb/usr/share/gosm 
	mkdir packages/deb/usr/share/gosm/about 
	mkdir packages/deb/usr/share/gosm/icons 
	mkdir packages/deb/usr/share/gosm/manual 
	mkdir packages/deb/usr/share/gosm/namefinder 
	mkdir packages/deb/usr/share/gosm/legend 
	mkdir packages/deb/usr/share/gosm/legend/mapnik 
	mkdir packages/deb/usr/share/gosm/legend/mapnik/images 
	mkdir packages/deb/usr/share/gosm/legend/cycle 
	mkdir packages/deb/usr/share/gosm/legend/cycle/images 
	mkdir packages/deb/usr/lib 
	mkdir packages/deb/usr/lib/gosm 
	mkdir packages/deb/usr/lib/gosm/imageglue 
	mkdir packages/deb/usr/lib/gosm/imageglue/pdf_creator 
	mkdir packages/deb/usr/lib/gosm/imageglue/pdf_creator/pdf 
	echo "Copying files" 
	cp gosm packages/deb/usr/bin/ 
	cp about/*png packages/deb/usr/share/gosm/about 
	cp about/*html packages/deb/usr/share/gosm/about 
	cp icons/* packages/deb/usr/share/gosm/icons/ 
	cp manual/*png packages/deb/usr/share/gosm/manual 
	cp manual/*html packages/deb/usr/share/gosm/manual 
	cp namefinder/*txt packages/deb/usr/share/gosm/namefinder 
	cp legend/*html packages/deb/usr/share/gosm/legend 
	cp legend/mapnik/*html packages/deb/usr/share/gosm/legend/mapnik 
	cp legend/mapnik/images/*png* packages/deb/usr/share/gosm/legend/mapnik/images 
	cp legend/cycle/*html packages/deb/usr/share/gosm/legend/cycle 
	cp legend/cycle/images/*png* packages/deb/usr/share/gosm/legend/cycle/images 
	cp imageglue/pdf_creator/*jar packages/deb/usr/lib/gosm/imageglue/pdf_creator 
	cp imageglue/pdf_creator/pdf/*class packages/deb/usr/lib/gosm/imageglue/pdf_creator/pdf 
	echo "creating control file"
	cd packages/deb/DEBIAN &&\
	touch control &&\
	echo "Package: gosm" >> control &&\
	echo "Version: $(VERSION)" >> control &&\
	echo "Section: net" >> control &&\
	echo "Priority: optional" >> control &&\
	echo "Architecture: i386" >> control &&\
	echo "Depends: curl (>= 7.18.2)" >> control &&\
	echo "Depends: libwebkit-1.0-1 (>= 1.0.1)" >> control &&\
	echo "Depends: libpng12-0 (>= 1.2.27)" >> control &&\
	echo "Depends: libgtk2.0-0 (>= 2.16.1)" >> control &&\
	echo "Depends: libcairo2 (>= 1.8.6)" >> control &&\
	echo "Installed-Size: 329" >> control &&\
	echo "Maintainer: Sebastian Kürten <sebastian.kuerten@fu-berlin.de>" >> control &&\
	echo "Description: Viewer for the OpenStreetMap" >> control
	dpkg-deb -b packages/deb packages/gosm.$(VERSION)_i386.deb
	rm -rf packages/deb



-include $(DEPENDFILE)

