CC=g++ -std=c++11
LIB_DIR = ${HOME}/lib
INC_DIR = ${HOME}/include

CCLIB=  -I$(INC_DIR) -L$(LIB_DIR) -ldl -lsdsl -ldivsufsort -ldivsufsort64
CFLAGS= -Wall -Werror -lm -ldl -Wno-unused-variable -O3
#CFLAGS= -O0 -g -lm -ldl

SDSL = 0
DEFINES = -DDEBUG=$(DEBUG) -DSDSL=$(SDSL)

ifeq ($(SDSL),1)
DEFINES += $(CCLIB)
endif

DEBUG = 0
LIBOBJ = external/malloc_count/malloc_count.o


DATA = dataset/input3.txt

all: main 

main: main.cpp $(LIBOBJ)
	$(CC) $(CFLAGS) $^  $(DEFINES) -o $@

%.o: %.c %.h
	gcc -c $< -o $@

run:
	./main $(DATA) -t -k 10 -o -A 1

clean:
	rm -rf *.o main fred $(LIBOBJ)

fred: fred.cpp $(LIBOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $^ -o $@
