all: cachesim

clean:
	rm -f *.o cachesim

config.o: config.c util.h vector.h
	gcc config.c -o config.o -c -O2 ${CFLAGS} -std=c99

util.o: util.c util.h
	gcc util.c -o util.o -c -O2 ${CFLAGS} -std=c99

vector.o: vector.c vector.h
	gcc vector.c -o vector.o -c -O2 ${CFLAGS} -std=c99

cachesim.o: cachesim.c config.h
	gcc cachesim.c -o cachesim.o -c -O2 ${CFLAGS} -std=c99

cachesim: config.o util.o vector.o cachesim.o
	gcc -o cachesim config.o util.o vector.o cachesim.o -O2 ${LDFLAGS}
