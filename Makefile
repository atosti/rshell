CC = "g++"
CFLAGS = "-g -Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o ./src/exec.o ./bin
#Add rshell to it's own bin directory

all: rshell

rshell: ./src/exec.o
	if [ ! -d "./bin" ]; then mkdir "./bin"; fi
	${CC} ${CFLAGS} ./src/exec.o -o ./bin/rshell
exec.o: ./src/exec.cpp
	${CC} ${CFLAGS} -c ./src/exec.cpp ./src/exec.o
clean:
	rm -rf ${ALLFILES}
