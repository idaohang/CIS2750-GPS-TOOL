CC = gcc
FLAGS = -Wall -std=c99 -g -lm -pedantic

FILES = gputil.c gpstool.c

default: $(FILES)
	$(CC) $(FILES) -o gpstool $(FLAGS)

gpstool: $(FILES)
	$(CC) $(FILES) -o gpstool $(FLAGS)

clean:
	rm -f *.o
	rm -f *.*~
	rm -f makefile~