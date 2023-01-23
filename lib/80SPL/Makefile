FLAGS=  -g -Wall

OBJECTS= ./cpu8080/cpu8080.o 

all: ${OBJECTS}
	gcc  -shared -g -o lib80SPL.so ${OBJECTS} ${FLAGS}

clean:
	rm ./lib80SPL.so

