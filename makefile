CC = gcc
CFLAGS = -I.

hello: main.o
	$(CC) -o main main.o

run:
	./main.exe width=500 height=500
	gimp-2.10 output.ppm