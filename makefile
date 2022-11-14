# makefile

CC = gcc
CFLAGS = -Wall -O1 -pedantic-errors -pthread
CFLAGS_COMMON = -I ./
OBJS_SERVER = server.o

all : server

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) $(OBJS_SERVER) $(CFLAGS_COMMON) -o server

.PHONY: clean
clean:
	rm -rf *.o server