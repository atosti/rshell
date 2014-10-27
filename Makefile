CC = "g++"
CFLAGS = "-Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o ./bin

all: rshell

rshell: ./src/exec.o
	${CC} ./src/exec.o -o rshell
./src/exec.o: ./src/exec.cpp
	${CC} ${CFLAGS} -c ./src/exec.cpp -o ./src/exec.o
clean:
	rm -rf ${ALLFILES}
