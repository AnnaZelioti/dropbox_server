CC=gcc 
CFLAGS=-Wall
all: dropbox_server.c funs.c
	$(CC) -o server dropbox_server.c funs.c $(CFLAGS)

clean:
	rm -f server
