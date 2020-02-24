CC=gcc
CFLAGS= -g -w
objects = main.o prime.o

all: oss prime

%.o: %.c $(Dependancies)
	$(CC) -c -o $@ $< $(CFLAGS)

oss: main.o
	$(CC) -o $@ $^ $(CFLAGS)

prime: prime.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f oss
	rm -f prime
