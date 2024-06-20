CC=g++
CFLAGS= -Wall -Werror -lm -ldl -Wno-unused-variable -O3

DEBUG = 0
LIBOBJ = external/malloc_count/malloc_count.o

DEFINES = -DDEBUG=$(DEBUG)

DATA = dataset/input3.txt

all: main 

main: main.cpp $(LIBOBJ) -ldl
	$(CC) $(CFLAGS) $(DEFINES) $^ -o $@

%.o: %.c %.h
	gcc -c $< -o $@

run:
	./main $(DATA) -t -k 10 -o -A 1

clean:
	rm -rf *.o main fred $(LIBOBJ)

fred: fred.cpp $(LIBOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $^ -o $@
