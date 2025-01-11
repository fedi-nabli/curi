FLAGS = -I./
OBJECTS = ./build/curi.o

all: $(OBJECTS)
	clang ${FLAGS} main.c ${OBJECTS} -o ./build/main

./build/curi.o: ./curi.c
	clang ${FLAGS} -c curi.c -o ./build/curi.o