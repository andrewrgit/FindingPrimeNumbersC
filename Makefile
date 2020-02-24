CC=gcc
CFLAGS= -g -w
objects = main.o

%.o: %.c $(Dependancies)
	$(CC) -c -o $@ $< $(CFLAGS)

oss: $(objects)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f oss
