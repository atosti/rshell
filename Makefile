CC = "g++"
CFLAGS = "-g -Wall -Werror -ansi -pedantic"
ALLFILES = rshell.o ./bin 
out =./bin/rshell

src=./src/exec.cpp

all: rshell

#Does this need a different name(just rshell, rshell.o)?
rshell:
	#if[ ! -d "./bin"]; then mdkir ./bin fi
	${CC} ${CFLAGS} ${src}
clean:
	#ALLFILES works for bin
	rm -rf ${ALLFILES}

#all:
    #[-d "./bin"]
    #mkdir ./bin
    #g++ exec.cpp -0 bin/rshell
#target:
    #[-d "./bin"]
    #mkdir ./bin
    #g++ exec.cpp -0 bin/rshell
