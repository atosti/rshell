CC = "g++"
CFLAGS = "-g -Wall -Werror -ansi -pedantic"
BIN = "./bin"
#Add rshell to it's own bin directory

.PHONY: all
all: rshell ls cp
ls: ./src/ls.o
	if [ ! -d "./bin" ]; then mkdir "./bin"; fi
	g++ $(CFLAGS) ./src/ls.o -o ./bin/ls
cp: ./src/cp.o
	if [ ! -d "./bin" ]; then mkdir "./bin"; fi
	g++ $(CFLAGS) ./src/cp.o -o ./bin/cp
rshell: ./src/exec.o
	if [ ! -d "./bin" ]; then mkdir "./bin"; fi
	g++ $(CFLAGS) ./src/exec.o -o ./bin/rshell
clean:
	cd bin; if [ -a rshell ] ; then rm -rf rshell; fi
	cd bin; if [ -a ls ] ; then rm -rf ls; fi
	cd bin; if [ -a cp ] ; then rm -rf cp; fi
	rmdir bin
