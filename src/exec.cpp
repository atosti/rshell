#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <map>
#include <vector>
#include <errno.h>
using namespace std;

void quit(){
    cout << "Exiting..." << endl;
    exit(0);
}

int altInput(char** &argv, int len, int loc){
    //No first arg
    if(loc-1 < 0){
	cerr << "alt input, no LHS arg" << endl;
	return -1;
    //No second arg
    }else if(argv[loc+1] == NULL){
	cerr << "alt input, no RHS arg" << endl;
	return -1;
    }
    string str = "./.temp";

    //Opens file for reading
    int fd = open(str.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(fd == -1){
	perror("open altInput");
	return -1;
    }

    //Write to str before reading from it
    char buf[BUFSIZ];
    strcpy(buf, argv[loc+1]);
    strcat(buf, "\n");
    if(write(fd, buf, strlen(buf)) == -1){
	perror("write altInput");
	return -1;
    }

    //Closes stdin
    if(close(STDIN_FILENO) == -1){
        perror("close altInput");
	return -1;
    }
    //.temp now in stdin slot
    if(dup2(fd, STDIN_FILENO) == -1){
	perror("dup2 altInput");
	return -1;
    }

    //Close .temp to open stdout
    if(close(fd) == -1){
	perror("close2 altInput");
	return -1;
    }

    //Open .temp for reading
    fd = open(".temp", O_RDONLY, 0);
    if(fd == -1){
	perror("open2 altInput");
    }

    //Closes stdin
    if(close(STDIN_FILENO) == -1){
        perror("close input");
	return -1;
    }
    //Fd now in stdin slot
    if(dup2(fd, STDIN_FILENO) == -1){
	perror("dup2 input");
	return -1;
    }
    
    //Removes commands/ops that were run
    for(unsigned i = loc; i < len; i++){
	argv[i] = argv[len]; 
    }
    return 0;
}

int fdOutput(char** &argv, int len, int loc){
    //No first arg
    if(loc-1 < 0){
	cerr << "Fdoutput error, no LHS arg" << endl;
	return -1;
    //No second arg
    }else if(argv[loc+1] == NULL){
	cerr << "Fdoutput error, no RHS arg" << endl;
	return -1;
    }
    string str = "./";
    str.append(argv[loc+1]);
    str.append("/");

    int temp = int(argv[loc][0]) - 48;

    //Opens file to write to
    int fd = open(argv[loc+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd == -1){
    	perror("open Fdoutput");
	return -1;
    }

    //Closes passed fd
    if(close(temp) == -1){
	perror("close Fdoutput");
	return -1;
    }
    //Fd now in old fd slot
    if(dup2(fd, temp) == -1){
	perror("dup2 Fdoutput");
	return -1;
    }

    //FIXME put into a loop and remove proper pieces
    //Seems to work - Test later
    for(unsigned i = loc; i != len; i++){
	argv[i] = argv[len]; 
	//cerr << "argv[" << i << "]: " << argv[i] << endl;
    }
    return 0;
}

//FIXME - Implement piping
int pipe(char** &argv, int len, int loc){
    int fd[2];
    //Note:
    //fd[0] = Read end
    //fd[1] = Write end

    if(pipe(fd) == -1){
	perror("pipe failed");
	return -1;
    }
    //LHS of pipe cmd
    if(close(fd[0]) == -1){
	perror("close stdout");
	return -1;
    }
    if(dup2(fd[1], 1) == -1){
	perror("dup2 stdout");
	return -1;
    }

    //RHS of pipe cmd
    int pid = fork();
    if(pid == -1){
	perror("pipe fork");
	return -1;
    //Child
    }else if(pid == 0){
	if(close(fd[1]) == -1){
	    perror("close stdin");
	    return -1;
	}
	if(dup2(fd[0], 0) == -1){
	    perror("dup2 stdin");
	    return -1;
	}
	//FIXME - Does this go here?
	//Remove the other parths of argv
	if(execvp(argv[0], argv) == -1){
	    perror("pipe execvp");
	    return -1;
	}
	exit(0);
    //Parent
    }else{
	if(wait(0) == -1){
	    perror("wait failed");
	    return -1;
	}
    }

    return 0;
}

//FIXME - Only handles 1 file
int input(char** &argv, int len, int loc){
    //No first arg
    if(loc-1 < 0){
	cerr << "input, no LHS arg" << endl;
	return -1;
    //No second arg
    }else if(argv[loc+1] == NULL){
	cerr << "input, no RHS arg" << endl;
	return -1;
    }
    string str = "./";
    str.append(argv[loc+1]);

    //Opens file for reading
    int fd = open(str.c_str(), O_RDONLY, 0);
    if(fd == -1){
    	perror("open input");
	return -1;
    }
    //Closes stdin
    if(close(STDIN_FILENO) == -1){
        perror("close input");
	return -1;
    }
    //Fd now in stdin slot
    if(dup2(fd, STDIN_FILENO) == -1){
	perror("dup2 input");
	return -1;
    }
    //FIXME - Perhaps start at loc? (used to start @ i = 1)
    for(unsigned i = loc; i < len; i++){
	argv[i] = argv[i+1]; 
    }

    return 0;
}

//FIXME - Currently only handles 1 file at a time
int output(char** &argv, int len, int loc){
    //No first arg
    if(loc-1 < 0){
	cerr << "output error, no LHS arg" << endl;
	return -1;
    //No second arg
    }else if(argv[loc+1] == NULL){
	cerr << "output error, no RHS arg" << endl;
	return -1;
    }
    string str = "./";
    str.append(argv[loc+1]);
    str.append("/");

    int fd = open(argv[loc+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd == -1){
    	perror("open output");
	return -1;
    }
    //Closes stdout
    if(close(STDOUT_FILENO) == -1){
	perror("close output");
	return -1;
    }
    //Fd now in stdout slot
    if(dup2(fd, STDOUT_FILENO) == -1){
	perror("dup2 output");
	return -1;
    }
    //FIXME put into a loop and remove proper pieces
    //Seems to work - Test later
    for(unsigned i = loc; i != len; i++){
	argv[i] = argv[len]; 
    }

    //strcpy(argv[loc], "\0"); //Removes file name
    //strcpy(argv[loc+1], "\0"); //Removes > operator

    return 0;
}

//FIXME - Only works on single file passed in
int append(char** &argv, int len, int loc){
    //No first arg
    if(loc-1 < 0){
	cerr << "append, no LHS arg" << endl;
	return -1;
    //No second arg
    }else if(argv[loc+1] == NULL){
	cerr << "append, no RHS arg" << endl;
	return -1;
    }
    //Fork before modding fds
    string str = "./";
    str.append(argv[loc+1]);
    str.append("/");

    int fd = open(argv[loc+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
    if(fd == -1){
	perror("open append");
	return -1;
    }
    //Closes stdout
    if(close(STDOUT_FILENO) == -1){
	perror("close append");
	return -1;
    }
    //Fd now in stdout slot
    if(dup2(fd, STDOUT_FILENO) == -1){
	perror("dup2 append");
	return -1;
    }
    //FIXME - loop it instead
    for(unsigned i = loc; i != len; i++){
	argv[i] = argv[len]; 
    }

    return 0;
}

//Handles all forms of redirection
int redirHandler(char** &argv, int len){
    int ret = 0;
    int cnt = 0;
    for(unsigned i = 0; i < len; i++){
	if(strcmp(argv[i], "<") == 0){
	    cnt++;
	    ret = input(argv, len, i);
	    if(ret == -1){
		cerr << "Input failed" << endl;
		return -1;
	    //If a parent
	    }else if(ret == 0){
		return 0;
	    }
	}else if((strcmp(argv[i], ">") == 0)){
	    cnt++;
	    ret = output(argv, len, i);
	    if(ret == -1){
		cerr << "Output failed" << endl;
		return -1;
	    //If a parent
	    }else if(ret == 0){
		return 0;
	    }
	}else if((strcmp(argv[i], ">>")) == 0){
	    cnt++;
	    ret = append(argv, len, i);
	    if(ret == -1){
		cerr << "Append failed" << endl;
		return -1;
	    //If a parent
	    }else if(ret == 0){
		return 0;
	    }
	}else if(strcmp(argv[i], "|") == 0){
	    cnt++;
	    ret = pipe(argv, len, i);
	    if(ret == -1){
		cerr << "Pipe failed" << endl;
		return -1;
	    //If a parent
	    }else if(ret == 0){
		return 0;
	    }
	}else if(argv[i][1] == '>'){
	    bool fdRedir = false;
	    char curr = 'a';
	    for(unsigned j = 0; j < 10; j++){
		curr = (char)(((int)'0')+j);
		if(argv[i][0] == curr){
		    fdRedir = true;
		}
	    }
	    if(fdRedir){
		cnt++;
		ret = fdOutput(argv, len, i);
		if(ret == -1){
		    cerr << "fdOutput failed" << endl;
		    return -1;
		}else if(ret == 0){
		    return 0;
		}
	    }
	}else if(strcmp(argv[i], "<<<") == 0){
	    cnt++;
	    ret = altInput(argv, len, i);
	    if(ret == -1){
		cerr << "altInput failed" << endl;
		return -1;
	    }else if(ret == 0){
		return 0;
	    }
	}
    }
    return cnt;
}

//Globals for use in sig handler
int pid = 0;
int pid2 = 0;
int pidParent = 0;

//Signal Handler
void sig(int signum){
    //^C handling 
    if(signum == SIGINT){
	if(getpid() != pidParent){
	    kill(getpid(), SIGKILL);
	}
    }
}

int outputLogin(){
    char buf[BUFSIZ];
    char *login;
    if((login = getlogin()) == NULL){
	perror("getlogin failed");
	return -1;
    }
    for(unsigned i = 0; login[i] != '\0'; i++){
	cout << login[i];
    }
    cout << "@";
    if(gethostname(login, 128) == -1){ //Should hostname be smaller?
	perror("gethostname failed");
	return -1;
    }
    for(unsigned i = 0; login[i] != '\0'; i++){
	cout << login[i];
    }
    //Note: getcwd() causes "still reachable" mem leaks
    errno = 0;
    cout << ":" << getcwd(buf, sizeof(buf)) << " $ ";
    if(errno != 0){
	perror("getcwd failed");
	return -1;
    }

    return 0;
}

int main(int argc, char* argv[]){
    errno = 0;
    signal(SIGINT, sig);
    if(errno != 0){
	perror("signal failed");
	return -1;
    }
    pidParent = getpid();
    string usrInput; //Holds user input
    char *path = getenv("PATH"); //sets path
    if(chdir("./") == -1){
	perror("chdir failed");
	return -1;
    }

    //Creates a map for non-exec commands
    map<string, void (*)()> commands;
    commands["exit"] = quit;

    while(1){
	//Prints login/hostname prompt
	if(outputLogin() == -1){
	    cout << "outputLogin failed" << endl;
	    return -1;
	}

	//Handles input
	getline(cin, usrInput);
	unsigned cnt = 0; //Slots in argv[]
	char input[usrInput.length() + 1];
	strcpy(input, usrInput.c_str());

	//Comment Handling and Operator checking
	bool semiOp = false;
	bool andOp = false;
	bool orOp = false;
	for(unsigned i = 0; input[i] != '\0'; i++){
	    if(input[i] == '#'){
		while(input[i] != '\0'){
		    input[i] = '\0';
		    i++;
		}
	    }else if(input[i] == ';'){
		semiOp = true;
	    }else if((input[i] == '&') && (input[i+1] == '&')){
		andOp = true;
	    }else if((input[i] == '|') && (input[i+1] == '|')){
		orOp = true;
	    }
	}

	//Error checking for mult. ops
	if(semiOp && andOp){
	    cout << "Error mixed operators && ;" << endl;
	    continue;

	}else if(semiOp && orOp){
	    cout << "Error mixed operators ; ||" << endl;
	    continue;

	}else if(andOp && orOp){
	    cout << "Error mixed operators && ||" << endl;
	    continue;
	}

	//Beginning of Tokenization
	char *a[usrInput.length() + 1]; //array for cmds + args
	char *token;
	bool valid = true; //Validity of execvp()

	//Tokenizes Semicolons(;) and no Operators
	if((semiOp) || (!semiOp && !andOp && !orOp)){
	    token = strtok(input, ";");
	    while(token != NULL){
	    	a[cnt] = token;
	    	token = strtok(NULL, ";");
	    	cnt++;
	    }
	    strcat(a[cnt - 1], "\0");
	    a[cnt] = token; //Null terminates a[]
	//Tokenizes And operator(&&)
	}else if(andOp){
	    token = strtok(input, "&");
	    while(token != NULL){
		a[cnt] = token;
		token = strtok(NULL, "&");
		cnt++;
	    }
    	    strcat(a[cnt - 1], "\0");
	    a[cnt] = token; //Null terminates a[]
	//Tokenizes Or operator(||)
	}else if(orOp){
	    token = strtok(input, "|");
	    while(token != NULL){
		a[cnt] = token;
		token = strtok(NULL, "|");
		cnt++;
	    }
	    strcat(a[cnt - 1], "\0");
	    a[cnt] = token; //Null terminates a[]
	    valid = false; //Starting state for or
	}

	//Fork for each command (separated by semicolons)
	int curr = 0;
	bool isCmd = false;
	bool isCd = false;
	for(int i = 0; i < cnt; i++){//Tokenizes a[] removing " "
	    token = strtok(a[i], " ");
	    curr = 0;
	    isCmd = false;
	    isCd = false;

	    //Removes spaces and tabs, places result in argv[]
	    while(token != NULL){
	        argv[curr] = token;

		//Checks for custom commands
		if(commands.find(argv[0]) != commands.end()){
		    commands.at(argv[curr])();
		    isCmd = true;
		}
		//Cd check
		if(strcmp(argv[0], "cd") == 0){
		   isCd = true;
		}
		strcat(argv[curr], "\0");
		token = strtok(NULL, " \t");
		curr++; 
	    }
	    argv[curr] = token; //Null term argv

	    //Cd handler
	    if(isCd){
		//If no path passed, print an error message
		//Note: Bash changes it to HOME dir
		if(curr == 1){
		    cout << "Error: No argument passed to cd" << endl;
		//If it begins with a / or ./ proceed
		}else if((argv[1][0] == '/') || (argv[1][0] == '.' && argv[1][1] == '/')){
		   if(chdir(argv[1]) == -1){
			perror("cd failed");
		    }
		//If it needs to be appended with ./
		}else{
		    string path = "./";
		    path.append(argv[1]);
		    if(chdir(argv[1]) == -1){
			perror("cd failed");
		    }
		}
		//Note: cd doesn't care about more than 1 arg being passed
	    }

	    //Prevents execvp from running on commands
	    if(isCmd || isCd){
		continue;
	    }
	    //Connector Handling
	    if(andOp){
		if(!valid){
		    continue;
		}
	    }else if(orOp){
		if(valid){
		    continue;
		}
	    }

	    //Forks before redirect()
	    int ret = 0;
	    pid = fork();
	    if(pid == -1){
		perror("pid failed");
		exit(1);
	    }else if(pid == 0){//Child process 
	        int numArg = 0;
		bool runExec = true;

		//Counts # of args in argv
	    	while(argv[numArg] != NULL){
		    numArg++;
	        }

		//Redirection implementation
		int ret2 = 0;
		ret2 = redirHandler(argv, numArg);
		if(ret2 == -1){
		    cerr << "redirHandler failed" << endl;
		    return -1;
		}

		//cerr << "argv[0]: " << argv[0] << endl;
		//cerr << "argv[1]: " << argv[1] << endl;
		//cerr << "argv[2]: " << argv[2] << endl;
		//cerr << "argv[3]: " << argv[3] << endl;
		
		//Second fork
	        pid2 = fork();
	        if(pid2 == -1){
		    perror("pid fork failed");
		    exit(1);
	        }
		//Child process
	  	if(pid2 == 0){
		    //Stores PATH in a char*
		    if(path == NULL){
			cout << "Error: PATH is null" << endl;
			exit(1);
		    }

		    //Counts length of PATH
		    int pCnt = 0;
		    char *temp = path;
		    for(; temp[pCnt] != '\0'; pCnt++);

		    //Creates an array of pathes for execv
		    char *token2 = strtok(path, ":");
		    char *pArr[pCnt + 1];
		    int pNum = 0; //Num of paths

	    	    while(token2 != NULL){
	    	   	pArr[pNum] = token2;
	    	    	token2 = strtok(NULL, ":");
	    	    	pNum++;
	            }
	            strcat(pArr[pNum-1], "\0");
	            pArr[pNum] = token2; //Null terms array
		
		    //Appends "/cmd" to the paths and stores them in vector
		    string str;
		    vector<string> vc; 
		    for(unsigned q = 0; q < pNum; q++){
			str = pArr[q];
			str.append("/");
			str.append(argv[0]);
			vc.push_back(str);
		    }

		    if(!runExec){
			exit(1);
		    }else{
			//Searches each path for cmd
			for(unsigned x = 0; x < pNum; x++){
			    execv(vc.at(x).c_str(), argv);
			}
			perror("execv failed");
			exit(1);
		    }
		    /*}else if(execvp(argv[0], argv) == -1){//Runs on each argv[]
		    	perror("execvp() failed");
		        exit(1);
		    }*/
	    	}
	   	//Parent function
	   	if(-1 == waitpid(pid2, &ret,0)){ //waits on child-execs in order
		    perror("wait() failed");
		    exit(1);
	        }else if(WIFEXITED(ret) && WEXITSTATUS(ret) != 0){
		    return -1;
		}
		exit(0); //Exits child(end pid2)
	    //Pid parent
	    }else{
		if(-1 == waitpid(pid, &ret, 0)){
		    perror("wait() failed");
		    exit(1);
		}else if(WIFEXITED(ret) && WEXITSTATUS(ret) != 0){
		    valid = false;
		}else{
		    valid = true;
		}
	    }
	}
    }//End infinite while

    return 0;
}
