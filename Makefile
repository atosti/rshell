compiler = "g++"
opt = "-Wall -Werror -ansi - pedantic"
out =./bin/rshell
src=./src/exec.cpp

all:
    [-d "./bin"]
    mkdir ./bin
    g++ exec.cpp -0 bin/rshell
target:
    [-d "./bin"]
    mkdir ./bin
    g++ exec.cpp -0 bin/rshell
