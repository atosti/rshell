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

int main(int argc, char* argv[]){
    bool isOn = true; //Keeps track of whether rshell is still running
    string usrInput; //Holds user input
    while(isOn){
        cout << "$ "; //Prints prompt
	getline(cin, usrInput);
	char* inputCpy = new char[usrInput.length() + 1]; //pointer for input str
	strcpy(inputCpy, usrInput.c_str()); //inputCpy holds c-str copy of input
	unsigned cnt = 0; //counter for slots in argv[]
	char buf[usrInput.length() + 1];
	char* token = strtok(inputCpy, ";"); //removes semicolons
	char* a[usrInput.length() + 1]; //Creates array for cmds + args
	while(token != NULL){ //Tokenizes semicolons
	    cout << "Token: " << token << endl; //Remove later - Fixme
	    a[cnt] = token;
	    token = strtok(NULL, ";");
	    cnt++;
	}
	strcat(a[cnt - 1], "\0");
	a[cnt] = token; //Null terminates a[]
	int pid = fork(); //Forks the process
	if(pid == -1){
	    perror("fork() failed");
	    exit(1);
	}else if(pid == 0){ //Child process
	    int curr = 0;
	    int pid2 = 1;
	    for(int i = 0; i < cnt; i++){//Tokenizes a[] removing " "
		token = strtok(a[i], " "); //Resets token
		curr = 0;
		pid = 1;
		int fdi;
		int fdo;
		bool isOutput = false;
		//char* in = new char[usrInput.length() + 1];
		//char* out = new char[usrInput.length() + 1];

		while(token != NULL){ //Remove spaces
		    argv[curr] = token;
		    //_____Operator Checks - IO Redirection_____
		    if(isOutput){
			cout << "argv[curr + 1]: " << argv[curr] << endl;
			//if((fdi = open(argv[curr + 2]
			isOutput = false; //Prevents mult. calls
		    }
		    if(strcmp(argv[curr], ">") == 0){//Output
			    //Check that ">" isn't first arg
			    //Check that it is surrounded by args 'A > B'
			isOutput = true;
			cout << "argv[curr - 1]: " << argv[curr - 1] << endl;
			//Note: curr - 1 is just the prior arg, you need to
			//couple it with the flags/command it's passed with
			//and redirect that OUTPUT to your output.txt
			
			//SOLUTION: Have an int "sym" that holds the position
			//of the last found symbol and then do a for loop
			//that combines all of the argv[]'s on one end
			//together and all of the other end together as well
			//and then update sym each time an operator is found
			//be sure to iterate it after the for loop concat, tho
			cout << "argv[curr]: " << argv[curr] << endl;
		/*
			close(1);
			close(0);
			if((dup(fdo) == -1)){//
			    perror("dup(fdo) failed\n");//Error check
			}
			if((dup(fdi) == -1)){
			    perror("dupe(fdi) failed\n");//Error check
			}
		*/
		    }
		    strcat(argv[curr], "\0");
		    curr++; //Fixme - check this is not OORange
		    token = strtok(NULL, " ");
	        }
	        strcat(argv[curr], "\0"); //Null terms the last string
	        argv[curr] = token; //Null terms the array

		pid2 = fork();
		if(pid2 == 0){ //Child process
		    if(execvp(argv[0], argv) == -1){//Runs on each argv[]
			perror("execvp() failed");
			exit(1);
		    }
		}
		//Parent function
		if(-1 == wait(0)){ //waits on children - execs in order
		    perror("wait() failed");
		    exit(1);
		}
	    }
	}
    delete inputCpy; //Deallocates memory - Fixme, might go elsewhere
    }//End while loop

    return 0;
}
