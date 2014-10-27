CC = "g++"
CFLAGS = "-Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o ./bin

all: rshell

rshell: exec.o
	${CC} exec.o -o rshell
./src/exec.o: ./src/exec.cpp
	${CC} ${CFLAGS} -c ./src/exec.cpp -o ./src/exec.o
clean:
	rm -rf ${ALLFILES}
