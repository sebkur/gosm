BIN = gosm

SRC = \
main.c \
tile_manager.c \
foo_widget.c \

OBJ = \
main.o \
tile_manager.o \
foo_widget.o \

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -D BUILD=0 -g\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 libpng webkit-1.0` \
 `curl-config --cflags --libs`
LDFLAGS = -lm -lpthread -lexpat -lbz2\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 libpng webkit-1.0` \
 `curl-config --cflags --libs`

gosm: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dep: $(SRC)
	$(CC) -MM $(SRC) > $(DEPENDFILE)

clean:
	rm -f $(BIN) $(OBJ)

-include $(DEPENDFILE)

