Overview - main.cpp
-----------------------
The rshell project is designed to replicate the bash shell, allowing users to input various bash commands, such as "ls", "echo", and "pwd". Like in bash, commands and their arguments can be input into rshell with semicolons between them. Additionally, this program has an exit function(called with "exit") which allows a user to terminate an instance of rshell.

Rshell also handles operators, input and output (<, >), and connectors, and(&&) as well as or(||). However, it does not handle multiple types of connectors being passed together and will return an error if this is attempted.

ls.cpp
-----------------------
This  mimics the ls bash script and has the flags -l and -a working along with optional file/directory parameters that can be passed in. Currently the -R script is not functional and will not do anything when run.

Known errors in ls
-----------------------
- If an invalid(non-existent) file is passed in, then no error is passed when there should be one.
- Sometimes passing in "dir/filename" as the optional param causes seg faults.
- Alphabetizes output, but does organizes it via ASCII value, as such uppercase precedes lowercase.
- Passing in Multiple files/directories is largely untested and guaranteed to not work.
- Not all syscalls are error-checked due to unresolved errors causing some failures (likely a cause of the seg faults)
- Output is not ordered in columns.
