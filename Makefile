CC=gcc -std=c11 -pthread -lgomp -fopenmp
CFLAGS= -O2
DEPS=

all: cell_distance

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cell_distance: main.o
	$(CC) -o cell_distance main.o -lm

clean:
	rm -f *.o
