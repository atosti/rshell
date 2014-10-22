Rshell - Homework #0
---------------------
This project is designed to replicate the bash shell allowing users to input various commands, such as "ls", "echo", and "pwd".
Ideally, it will also allow for the use of connectors(";", "||", "&&") and allow comments to be used in the command line through the use of the hash symbol ("#").
Moreover, this program has an exit function, called with "exit", which allows a user to close rshell.
At present this shell can only handle simple commands such as "ls", "echo", and "pwd", since it is unable to handle flags or quoted text after commands.
However, the exit command works properly and can exit the rhsell.


Bugs
-----
1) Cannot handle code with flags, returns a "no directory" error
2) Cannot handle comments, treats them as an extension of command names.
3) Cannot open rhsell within rhsell, gives a "no directory" error
4) Cannot execute multiple commands, ands, or ors in the shell. It will treat them as command names. 

