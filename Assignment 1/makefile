all: gputil.c gputilmain.c
	gcc gputil.c gputilmain.c -o main -g -Wall -pedantic -std=c99

gputil: gputil.c
	gcc gputil.c -o gputil -g -Wall -std=c99

gputilmain: gputilmain.c
	gcc gputilmain.c -o gputilmain -g -Wall -std=c99

clean:
	rm main gputil gputilmain gputilmain.o gputil.o