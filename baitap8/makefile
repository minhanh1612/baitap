CC = gcc
CFLAGS = -Wall

all: mypipe

mypipe: mypipe.c
	$(CC) $(CFLAGS) mypipe.c -o mypipe

run: mypipe
	./mypipe

clean:
	rm -f mypipe
