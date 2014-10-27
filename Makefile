CC = "g++"
CFLAGS = "-g -Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o ./bin
#Add rshell to it's own bin directory


all: rshell

rshell: ./src/exec.o
	${CC} ${CFLAGS} ./src/exec.o -o rshell
./src/exec.o: ./src/exec.cpp
	${CC} ${CFLAGS} -c ./src/exec.cpp -o ./src/exec.o
clean:
	rm -rf ${ALLFILES}
