#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>

using namespace std;

//___ERROR LIST___
//if you enter lots of garbage with spaces inbetween, you get mult prompts
//test exit after commands
//test colons before/after commands
//Prompt multi-prints when space-separated garbage is entered 

int main(int argc, char*argv[]){
    bool isRunning = true;
    while(isRunning){
        cout << "$"; //Prompt
	string rawInput;
	getline(cin, rawInput); //Holds a cstring of input (ie. "ls")	
	if(rawInput == "exit"){ //ExitTest
	    cout << "Exiting rshell..." << endl;
	    return 0;
	}
	char input[10000]; //error?	
	cout << "input[]: ";
	for(unsigned i = 0; i < 100 && i < rawInput.length(); i++){
	    input[i] = rawInput.at(i);
	}
        char* token = strtok(input, ";"); //parses of ; turning it to \0
        int cnt = 0; //counter
// Build tokenization here

/*
        while(token != NULL){
	    if(*token = '#'){ //Comment check
	        argv[cnt] = NULL; //null terminates cmd loop
	        token = NULL; //exits while loop
	    }else{
	        argv[cnt] = token;
	        token = strtok(NULL, " ");
	        cnt++;
		cout << "token: " << token << endl; //FIXME line
	    }
        } //End tokenization
*/
	int pid = fork();
        if(pid == 0){ //child process
	   // if(execvp(argv[0], &argv[0]) == -1){ - Old
	    if(execvp(argv[0], argv) == -1){
	        perror("execvp failure");
	    }
	    exit(1);
        }else if(pid == -1){
	}
	else{ //parent
	    if(-1 == wait(NULL)){
	        perror("wait() failure");
	    }
        }
    }//end isRunning loop
        return 0;
}
