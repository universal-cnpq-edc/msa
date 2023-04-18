CC=g++
CFLAGS= -Wall -Werror -lm -ldl -Wno-unused-variable

DEBUG = 0

LIBOBJ = external/malloc_count/malloc_count.o

DEFINES = -DDEBUG=$(DEBUG)

all: main 

main: main.cpp $(LIBOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $^ -o $@

%.o: %.c %.h
	gcc -c $< -o $@

run:
	./main dataset/input3_novo.txt -t -k 10 -o

clean:
	rm -rf *.o main fred $(LIBOBJ)

fred: fred.cpp $(LIBOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $^ -o $@
