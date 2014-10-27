CC = "g++"
CFLAGS = "-g -Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o exec.o ./bin
#Add rshell to it's own bin directory

all: rshell

rshell: ./src/exec.o
	${CC} ${CFLAGS} ./exec.o -o ./bin/rshell
./src/exec.o: ./src/exec.cpp
	${CC} ${CFLAGS} -c ./src/exec.cpp -o ./exec.o
clean:
	rm -rf ${ALLFILES}
