#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]){ 
    bool isOn = true; //Keeps track of whether rshell is still running
    string usrInput; //Holds user input
    while(isOn){
        cout << "$ "; //Prints prompt
	getline(cin, usrInput);
	char* inputCpy = new char[usrInput.length() + 1]; //pointer for input string
	strcpy(inputCpy, usrInput.c_str()); //inputCpy now holds c-string copy of input
	unsigned cnt = 0; //counter for slots in argv[]
	char* token = strtok(inputCpy, ";"); //removes semicolons
	char* a[usrInput.length() + 1]; //Proper size? - FIXME

	//delete inputCpy; //FIXME - Deallocate memory later	

	while(token != NULL){
	    if(*token == '#'){ //Handles comments
		token = NULL;
	   // }else if(token == "||"){ //Will Handle OR
	   // }else if(token == "&&"){ //Will Handle AND
	    }else{ //Default scenario
	        //if(token == "exit") //Will handle exit
		cout << "Token: " << token << endl;
		a[cnt] = token;
		//strcat(a[cnt], "\0"); //Null terminates the string - needed? FIXME
		token = strtok(NULL, ";");
		cnt++;
	    }
	}//Removes semicolons
	
	strcat(a[cnt - 1], "\0");
	a[cnt] = token; //Should null term argv[], as token will equal NULL

	for(int i = 0; i < cnt; i++){
	    cout << "a[" << i << "]: " << a[i] << endl;
	}
	
	int pid = fork(); //Forks the process
	if(pid == -1){
	    perror("fork() failed");
	    exit(1);
	}else if(pid == 0){ //Child process
	    cout << "I'm a kid again!" << endl; //Fixme - remove later
	    
	    //Now Tokenize a[] with " " as a delimiter and put it into argv[]
	    int j = 0;
	    int pid2 = 1;
	    cout << "Count: " << cnt << endl; //FIXME - Used to check val of cnt
	    for(int i = 0; i < cnt; i++){
		token = strtok(a[i], " "); //Resets token
		cout << "Current Tokens: " << token << endl;
	        j = 0;
		pid = 1;
		while(token != NULL){
		    //cout << "NewToken: " << token << endl; //FIXME - Checks outputs
		    argv[j] = token;
		    strcat(argv[j], "\0");
		    j++; //Might not be correct here
		    token = strtok(NULL, " ");
	        }
	        strcat(argv[j], "\0");
	        argv[j] = token;
	        cout << "While loop finished" << endl; //FIXME - should print mult. times
		//Should fork here, then restart the loop. In the fork, it should run execvp.
		pid2 = fork();
		if(pid2 == 0){
		    if(execvp(argv[0], argv) == -1){
			perror("execvp() failed");
			exit(1);
		    }
		}
		if(-1 == wait(0)){
		    perror("wait() failed");
		    exit(1);
		}
	    }
	    cout << "J: " << j << endl;
	    cout << "Count(Final): " << cnt << endl;
	}
   }//End while loop

    return 0;
}
