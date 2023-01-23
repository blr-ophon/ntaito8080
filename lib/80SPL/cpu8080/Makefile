INCLUDES= -I ./include
FLAGS=  -g -Wall

OBJECTS= ./build/cpu.o ./build/stack.o ./build/flags.o ./build/registerbank.o ./build/instructions.o ./build/memory.o

all: ${OBJECTS}
	ld -r -o ./cpu8080.o ${OBJECTS} 
	gcc ${FLAGS} ${INCLUDES} ./src/test.c ${OBJECTS} -o ./cpu8080test

./build/cpu.o:src/cpu.c
	mkdir -p ./build
	gcc -fPIC ${FLAGS} ${INCLUDES} ./src/cpu.c -c -o ./build/cpu.o

./build/stack.o:src/stack.c
	mkdir -p ./build
	gcc -fPIC ${FLAGS} ${INCLUDES} ./src/stack.c -c -o ./build/stack.o

./build/flags.o:src/flags.c
	mkdir -p ./build
	gcc -fPIC ${FLAGS} ${INCLUDES} ./src/flags.c -c -o ./build/flags.o

./build/registerbank.o:src/registerbank.c
	mkdir -p ./build
	gcc -fPIC ${FLAGS} ${INCLUDES} ./src/registerbank.c -c -o ./build/registerbank.o

./build/instructions.o:src/instructions.c
	mkdir -p ./build
	gcc -fPIC ${FLAGS} ${INCLUDES} ./src/instructions.c -c -o ./build/instructions.o

./build/memory.o:src/memory.c
	mkdir -p ./build
	gcc -fPIC ${FLAGS} ${INCLUDES} ./src/memory.c -c -o ./build/memory.o

clean:
	rm ./build/*
	rm ./cpu8080.o
	rm ./cpu8080test

test_all: test1 test2 test3

test1: 
	./cpu8080test ./tests/TST8080.COM 

test2:
	./cpu8080test ./tests/8080PRE.COM

test3:
	./cpu8080test ./tests/8080EXM.COM
