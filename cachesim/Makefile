all: solution

clean:
	rm -f *.o cachesim solution

config.o: config.c util.h vector.h
	gcc config.c -o config.o -c -O2 ${CFLAGS} -std=c99 -Wall -Wextra -g

util.o: util.c util.h
	gcc util.c -o util.o -c -O2 ${CFLAGS} -std=c99 -Wall -Wextra -g

vector.o: vector.c vector.h
	gcc vector.c -o vector.o -c -O2 ${CFLAGS} -std=c99 -Wall -Wextra -g

cachesim.o: cachesim.c config.h
	gcc cachesim.c -o cachesim.o -c -O2 ${CFLAGS} -std=c99 -Wall -Wextra -g

memory.o: memory.c memory.h
	gcc memory.c -o memory.o -c -O2 ${CFLAGS} -std=c99 -Wall -Wextra -g

cachesim: config.o util.o vector.o cachesim.o memory.o
	gcc -o cachesim $^ -O2 ${LDFLAGS} -g

solution: cachesim
	cp cachesim solution
