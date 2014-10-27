CC = "g++"
CFLAGS = "-Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o ./bin 
VPATH=/src/exec.cpp

all: rshell

rshell: exec.o
	${CC} exec.o -o rshell
exec.o: exec.cpp
	${CC} ${CFLAGS} exec.cpp -o exec.o
clean:
	rm -rf ${ALLFILES}
