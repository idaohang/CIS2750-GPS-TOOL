CC = gcc
CFLAGS = -Wall -std=c99 -g -lm -pedantic
PYFLAGS = -I /usr/include/python2.5 -fPIC
FILES = gputil.c gpstool.c
PYFILES = gputil.c

default: $(FILES)
	$(CC) $(FILES) -o gpstool $(CFLAGS)

gputil: $(PYFILES)
	$(CC) $(PYFLAGS) $(PYFILES) -c gputil -Wall -std=c99 -g -pedantic -I.
	$(CC) -shared gputil.o -o Gps.so

gpstool: $(FILES)
	$(CC) $(FILES) $(PYFLAGS) -o gpstool $(CFLAGS) -I.

clean:
	rm -f *.o
	rm -f *.*~
	rm -f makefile~