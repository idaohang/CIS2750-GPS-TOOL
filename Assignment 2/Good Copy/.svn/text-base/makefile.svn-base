CC = gcc
CFLAGS = -Wall -std=c99 -g -lm -pedantic

FILES = gputil.c gpstool.c

default: $(FILES)
	$(CC) $(FILES) -o gpstool $(CFLAGS)

gpstool: $(FILES)
	$(CC) $(FILES) -o gpstool $(CFLAGS)

clean:
	rm -f *.o
	rm -f *.*~
	rm -f makefile~