BIN = gosm

SRC = \
sxdl_base.c \
sxdl_break.c \
sxdl_container.c \
sxdl_font.c \
sxdl_image.c \
sxdl_size_cache.c \
sxdl_table.c \
sxdl_table_cell.c \
sxdl_table_row.c \
sxdl_widget.c \
test.c

OBJ = \
sxdl_base.o \
sxdl_break.o \
sxdl_container.o \
sxdl_font.o \
sxdl_image.o \
sxdl_size_cache.o \
sxdl_table.o \
sxdl_table_cell.o \
sxdl_table_row.o \
sxdl_widget.o \
test.o

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -D BUILD=0 -g\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0`
LDFLAGS = -lm -lpthread -lexpat \
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0`

test: $(OBJ)
	$(CC) $(CFLAGS) -o test $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dep: $(SRC)
	$(CC) -MM $(SRC) > $(DEPENDFILE)

clean:
	rm -f $(BIN) $(OBJ)


-include $(DEPENDFILE)

