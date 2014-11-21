#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <fcntl.h>

using namespace std;

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
	    int fdo = open(argv[x - 1], O_WRONLY);
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
				     //Handle if x-2 and x-1 don't exist?
	    cout << ""; //Should clear the output file
			//Above else if might not be needed if this is resolved
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
	//FIXME - old lines below, ensure above works before deletion
	//argv[x-2] = argv[x-1]; //Does this need to be put into a for
	//argv[x-1] = argv[x]; //loop to keep pushing all the next
			    //commands together?
    }else if(flags == 3){//Pipe
	int fdpc[2];//fd's for parent to child
	int fdcp[2];//fd's for child to parent

	if(pipe(fdpc) == -1){//Open pipe
	    perror("pipe fdpc");
	    exit(1);
	}
	if(pipe(fdcp) == -1){//Open pipe
	    perror("pipe fdcp");
	    exit(1);
	}

	int pid = 0;
	//int pid = fork();
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

	    //Conects fdcp to stdout
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
	    exit(1);
	}else{//Parent process
	    //Close unneeeded pipes
	    if(close(fdpc[0]) == -1){
		perror("close fdpc[0]");
		exit(1);
	    }
	    if(close(fdcp[1]) == -1){
		perror("close fdcp[1]");
		exit(1);
	    }
	}

    }else if(flags == 4){
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

int main(int argc, char* argv[]){
    string usrInput; //Holds user input
    while(1){
	unsigned cnt = 0; //counter for slots in argv[]
        cout << "$ "; //Prints prompt
	getline(cin, usrInput);
	char* inputCpy = new char[usrInput.length() + 1]; //pointer for input str
	strcpy(inputCpy, usrInput.c_str()); //inputCpy holds c-str copy of input
	char* token = strtok(inputCpy, ";"); //removes semicolons
	char* a[usrInput.length() + 1]; //Creates array for cmds + args
	while(token != NULL){ //Tokenizes semicolons
	    a[cnt] = token;
	    token = strtok(NULL, ";");
	    cnt++;
	}
	strcat(a[cnt - 1], "\0");
	a[cnt] = token; //Null terminates a[]

	//Fork for each command (separated by semicolons)
	int curr = 0;
	for(int i = 0; i < cnt; i++){//Tokenizes a[] removing " "
	    token = strtok(a[i], " "); //Resets token
	    curr = 0;
	    while(token != NULL){ //Remove spaces
	        argv[curr] = token; //argv[] = a[] (without spaces)	  
		if(strcmp(argv[curr], "exit") == 0){//Exit check
		    cout << "Exiting..." << endl;
		    delete inputCpy;
		    exit(0);
		}
		
		strcat(argv[curr], "\0"); //Null terms
		token = strtok(NULL, " ");
		curr++; 
	    }
	    argv[curr] = token; //Null terms array
	    int pid2 = fork(); //Forks before redirect()
	    if(pid2 == -1){
		perror("pid2 failed");
		exit(1);
	    }else if(pid2 == 0){//Child process 
	        int num = 0;
		bool runExec = true;
	    	while(argv[num] != NULL){//counts length of ars in argv
		    num++;
	        }
	        redirect(argv, num, runExec); //Calls check for i/o redirection

		//int pid3 = 0;
	        int pid3 = fork();//FIXME - is 3rd fork needed?
	        if(pid3 == -1){
		    perror("pid fork  failed");
		    exit(1);
	        }
	  	if(pid3 == 0){ //Child process
		    if(!runExec){
			exit(1);
		    }else if(execvp(argv[0], argv) == -1){//Runs on each argv[]
		    	perror("execvp() failed");
		        exit(1);
		    }
	    	}
	   	//Parent function
	   	if(-1 == wait(0)){ //waits on children - execs in order
		    perror("wait() failed");
		    exit(1);
	        }
		exit(0); //Exits the child process
	    //end pid2 process
	    }else{//pid2 parent
		if(-1 == wait(0)){
		    perror("wait() failed");
		    exit(1);
		}
	    }
	}
        delete inputCpy; //Deallocates memory - Fixme, might go elsewhere
    }//End while loop

    return 0;
}
