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
using namespace std;

void quit(){
    cout << "Exiting..." << endl;
    exit(0);
}
/*
void cd(){
    //If no params passed, change to HOME
    if(chdir(getenv("HOME")) == -1){
	perror("chdir failed");
	exit(1);
    }
    char buf[BUFSIZ];
    cout << "CWD: " << getcwd(buf, sizeof(buf)) << endl;
}
*/

void redirect(char* argv[], int num, bool &runExec){//consider making a bool to check if any operators are found
    int x = 0;
    int y = 0;
    int last = -1;//char loc. of last symbol
    int flags = -1; //Holds val corresponding to type of sym

    while(x != num){//for each string
        y = 0;//Resets y to beginning of a string
	while(argv[x][y] != '\0'){//char in argv[]
	    if(argv[x][y] == '>'){//Output check
		if(argv[x][y+1] == '>'){//Append check
		    flags = 1;
		    last = x;
		    y++; //used to avoid second > in >> being read as a solo >
		}else{
		    last = x;
		    flags = 0;
		}
	    }else if(argv[x][y] == '<'){//Input
		if((argv[x][y+1] == '<') &&
		   (argv[x][y+2] == '<')){
		    y+=2;
		    last = x;
		    flags = 4; //<<< found
		}else{
	  	    last = x;
		    flags = 2;
		}
	    }else if(argv[x][y] == '|'){//Pipe
		last = x;
		flags = 3;
	    }
	    y++;//iterate chars
	}//End of a string
	x++;//iterate strings
    }
    if(flags == 0 || flags == 1){//Output or Append
	if(last == (x - 1)){//Ends with '>' or '>>'
	    if(flags == 0){
	    	cerr << "error: expected argument for >" << endl; 
	    	exit(0);
	    }else if(flags == 1){
		cerr << "error: expected argument for >>" << endl;
		exit(0);
	    }
	}else if(flags == 0){//Output
	    //if(remove(argv[x - 1]) != 0){
		//perror("remove output");
	    //}
	    //FIXME - clear output of the file
	    int fdo = open(argv[x - 1], O_WRONLY | O_CREAT);
	    if(fdo == -1){
		perror("open output");
		exit(1);
	    }
	    if(close(1) == -1){//close stdout
		perror("close output");
		exit(1);
	    }
	    if(dup2(fdo, STDOUT_FILENO) == -1){//fdo is now in slot 1 (stdout)
		perror("dup output");
		exit(1);
	    }
	    strcpy(argv[x-1], "\0"); //Removes file name
	    strcpy(argv[x-2], "\0"); //Removes > operator
	}else if(flags == 1){//Append
	    int fdo = open(argv[x - 1], O_WRONLY | O_APPEND);
	    if(fdo == -1){
		perror("open append");
		exit(1);
	    }
	    if(close(1) == -1){
		perror("close append");
		exit(1);
	    }
	    if(dup(fdo) == -1){
		perror("dup append");
		exit(1);
	    }
	    strcpy(argv[x-1], "\0");
	    strcpy(argv[x-2], "\0");
	}
    }else if(flags == 2){//Input
	int fdi = open(argv[x - 1], O_RDONLY);
	if(fdi == -1){
	    perror("open input");
	    exit(1);
	}
	if(close(0) == -1){//close stdin
	    perror("close input");
	    exit(1);
	}
	if(dup2(fdi, STDIN_FILENO) == -1){//fdi is now in slot 0 (stdin)
	    perror("dup input");
	    exit(1);
	}
	for(unsigned i = 1; i < x; i++){
	    argv[i] = argv[i+1];
	}
    }else if(flags == 3){//Pipe
	int fdpc[2];//fds for parent to child
	int fdcp[2];//fds for child to parent

	if(pipe(fdpc) == -1){//Open pipe
	    perror("pipe fdpc");
	    exit(1);
	}
	if(pipe(fdcp) == -1){//Open pipe
	    perror("pipe fdcp");
	    exit(1);
	}

	int pid = fork();
	if(pid == -1){
	    perror("pipe fork");
	    exit(1);
	}else if(pid == 0){//In child process
	    //Connects fdpc to stdin
	    if(close(fdpc[1]) == -1){
		perror("close fdpc[1]");
		exit(1);
	    }
	    if(dup2(fdpc[0], STDIN_FILENO) == -1){
		perror("dup2 fdpc[0]");
		exit(1);
	    }
	    if(close(fdpc[0]) == -1){
		perror("close fdpc[0]");
		exit(1);
	    }

	    //Connects fdcp to stdout
	    if(close(fdcp[0]) == -1){
		perror("close fdcp[0]");
		exit(1);
	    }
	    if(dup2(fdcp[1], STDOUT_FILENO) == -1){
		perror("dup2 fdcp[1]");
		exit(1);
	    }
	    if(close(fdcp[1]) == -1){
		perror("close fdcp[1]");
		exit(1);
	    }
	    //Execs first command
	    char *buf[x];
	    for(unsigned i = 0; i < last; i++){
		//cerr << "argv[" << i << "]: " << argv[i] << endl;
		strcpy(buf[i], argv[i]);
	    }

	    if(execvp(buf[0], argv) == -1){
		perror("execvp pipe");
		exit(1);
	    }
	
	    //exit(1); //FIXME - needed?
	}else{//Parent process
	    //Close unneeded pipes
	    if(close(fdpc[0]) == -1){
		perror("close fdpc[0]");
		exit(1);
	    }
	    if(close(fdcp[1]) == -1){
		perror("close fdcp[1]");
		exit(1);
	    }

	    //Execs second command
	    char *buf[x];
	    for(unsigned i = 0; i < (x - last); i++){
		//cerr << "argv[" << i << "]: " << argv[i] << endl;
		strcpy(buf[i], argv[i]);
	    }

	    if(execvp(buf[0], argv) == -1){
		perror("execvp pipe");
		exit(1);
	    }

	    //Open pipes
	    /*
	    if(execvp(argv[last+1], argv) == -1){
		perror("execvp pipe");
		exit(1);
	    }*/

	    //Create temp file, write execvp of last +1 to it, then
	    //open that file in fdpc[1]

	    if(fdpc[1] = open(argv[last + 1], O_WRONLY) == -1){
		perror("fdpc[1] open");
		exit(1);
	    }
	    /*if(fdcp[0] = open(argv[last - 1], O_RDONLY) == -1){
		perror("fdcp[0] open");
		exit(1);
	    }*/

	    //redirect input to child
	    if(close(fdpc[1]) == -1){ //close out
		perror("close fdpc[1]");
		exit(1);
	    }
	    //read child process output
	    if(read(fdcp[0], buf, sizeof(buf)) == -1){ //read in
		perror("read fdcp[0]");
		exit(1);
	    }
	    if(close(fdcp[0]) == -1){
		perror("close fdcp[0]");
		exit(1);
	    }
	    //call wait on exited child
	    if(wait(0) == -1){
		perror("wait pipe");
		exit(1);
	    }
	}

    }else if(flags == 4){//<<< string
	cout << argv[x-1] << endl;
	for(unsigned i = 1; i < x; i++){
	    argv[i] = argv[i+1];
	}
	runExec = false;
    }
    if(flags != -1){//Recursive call
	redirect(argv, last, runExec);
    }
    return;

}

//Globals for use in sig handler
int pid = 0;
int pid2 = 0;
int pidParent = 0;

//Signal Handler
void sig(int signum){
    //^C handling 
    //FIXME - Find test cases for ^C
    if(signum == SIGINT){
	if(getpid() != pidParent){
	    kill(getpid(), SIGKILL);
	}
    }
}

int main(int argc, char* argv[]){
    signal(SIGINT, sig);
    pidParent = getpid();
    string usrInput; //Holds user input
    char *path = getenv("PATH"); //sets path
    chdir("./");

    //Creates a map for non-exec commands
    map<string, void (*)()> commands;
    commands["exit"] = quit;
    //commands["cd"] = cd;

    while(1){
	//Prints login/hostname prompt
	char buf[BUFSIZ];
	char *login;
	if((login = getlogin()) == NULL){
	    perror("getlogin failed");
	    exit(1);
	}
	for(unsigned i = 0; login[i] != '\0'; i++){
	    cout << login[i];
	}
	cout << "@";
	if(gethostname(login, 128) == -1){ //Should hostname be smaller?
	    perror("gethostname failed");
	    exit(1);
	}
	for(unsigned i = 0; login[i] != '\0'; i++){
	    cout << login[i];
	}
	//FIXME - Ensure this is proper output
	//Note: getcwd() causes "still reachable" mem leaks
	cout << ":" << getcwd(buf, sizeof(buf)) << " $ ";

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
	    //FIXME - Test with test cases!
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
	        int num = 0;
		bool runExec = true;
		//Counts # of args in argv
	    	while(argv[num] != NULL){
		    num++;
	        }
	        redirect(argv, num, runExec); //i/o redirection

		//Second fork
	        pid2 = fork();
	        if(pid2 == -1){
		    perror("pid fork failed");
		    exit(1);
	        }
		//Child process
	  	if(pid2 == 0){
		    //Stores PATH in a char*
		    //char *path = getenv("PATH");
		    if(path == NULL){
			cout << "Error: PATH is null" << endl;
			exit(1);
		    }

		    //Counts length of PATH
		    char *temp = path;
		    int cnt2 = 0;
		    for(; temp[cnt2] != '\0'; cnt2++);

		    //Creates an array of pathes for execv
		    char *token2 = strtok(path, ":");
		    char *arr[cnt2 + 1];
		    int cnt3 = 0; //Num of paths

	    	    while(token2 != NULL){
	    	   	arr[cnt3] = token2;
	    	    	token2 = strtok(NULL, ":");
	    	    	cnt3++;
	            }
	            strcat(arr[cnt3-1], "\0");
	            arr[cnt3] = token2; //Null terms array
		
		    //Appends "/cmd" to the paths and stores them in vector
		    string str;
		    vector<string> vc; 
		    for(unsigned q = 0; q < cnt3; q++){
			str = arr[q];
			str.append("/");
			str.append(argv[0]);
			vc.push_back(str);
		    }

		    if(!runExec){
			exit(1);
		    }else{
			//Searches each path for cmd
			for(unsigned x = 0; x < cnt3; x++){
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
