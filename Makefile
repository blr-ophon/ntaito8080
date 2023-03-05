CC = gcc
OPT = -O2
CFLAGS = -std=c99 -g -Wall -Wextra -pedantic $(OPT)

INCLUDES= -I./include -I./lib/8080nemu
LIBRARIES= -L. ./lib/8080nemu.so -lSDL2 

CFILES_DIR := ./src
BUILD_DIR := ./build

CFILES := $(wildcard ./src/*.c)
OBJECTS := $(CFILES:$(CFILES_DIR)/%.c=$(BUILD_DIR)/%.o)
EXEC := ./bin/Ntaito8080
ARGS := ./roms/invaders.concatenated


${EXEC}: ${OBJECTS}
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} ${OBJECTS} -o $@ ${LIBRARIES}

${BUILD_DIR}/%.o: ${CFILES_DIR}/%.c
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} -c $< -o $@

clean:
	rm -rf ${OBJECTS} 
	rm -rf ${EXEC}

run: ${EXEC}
	$^ ${ARGS}

debug: ${EXEC}
	cgdb --args $^ ${ARGS}

testleak: ${EXEC}
	valgrind --leak-check=full --show-leak-kinds=all $^ ${ARGS}

profile: ${EXEC}
	mkdir -p ./prof
	rm -rf ./prof/*
	valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ${EXEC} ${ARGS}
	mv ./callgrind.* ./prof

show-asm: ${SRC}
	${CC} -Og -S $^




