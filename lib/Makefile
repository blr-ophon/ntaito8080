FLAGS=  -g -Wall

OBJECTS= ./8080nemu/cpu8080.o

all: ${OBJECTS}
	gcc  -shared -o 8080nemu.so ${OBJECTS} ${FLAGS}

clean:
	rm ./8080nemu.so

