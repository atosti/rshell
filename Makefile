CC = "g++"
CFLAGS = "-g -Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o ./bin 
out =./bin/rshell

src=./src/exec.cpp

all: rshell

# Does this need a different name(just rshell, rshell.o)?
rshell: exec.o
	${CC} exec.o -o rshell
exec.o: exec.cpp
	${CC} ${CFLAGS} ${src}
clean:
	rm -rf ${ALLFILES}

#all:
    #[-d "./bin"]
    #mkdir ./bin
    #g++ exec.cpp -0 bin/rshell
#target:
    #[-d "./bin"]
    #mkdir ./bin
    #g++ exec.cpp -0 bin/rshell
