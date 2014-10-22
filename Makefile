compiler = "g++"
opt = "-Wall -Werror -ansi - pedantic"
$(opt)
$(compiler) $(opt)

all:

target:
   mkdir -p  bin
    g++ exec.cpp -0 bin/rshell
