BIN = main

SRC = \
main.c \
foo_widget.c \

OBJ = \
main.o \
foo_widget.o \

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -D BUILD=0 -g\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 glib-2.0 gthread-2.0` \
 `curl-config --cflags --libs`
LDFLAGS = -lm -lpthread -lexpat -lbz2\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 glib-2.0 gthread-2.0` \
 `curl-config --cflags --libs`

gosm: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dep: $(SRC)
	$(CC) -MM $(SRC) > $(DEPENDFILE)

clean:
	rm -f $(BIN) $(OBJ)

-include $(DEPENDFILE)

