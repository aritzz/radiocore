CC = gcc
CFLAGS = -g -O2 -lm $(shell pkg-config --cflags gstreamer-1.0) $(shell mysql_config --libs --cflags) $(shell pkg-config --libs gstreamer-1.0)

SOURCES = src/aras.c src/database.c src/free_data.c src/main.c src/player.c src/read_data.c  src/log.c src/support.c
OBJECTS = $(SOURCES:.c=.o)
LIBS = $(pkg-config --cflags --libs gstreamer-1.0) -lm $(mysql_config --libs)

bin/radiocore: $(SOURCES)
	$(CC) -Wall -o bin/radiocore $(CFLAGS) $(SOURCES) 
