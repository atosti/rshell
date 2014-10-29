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
    string usrInput;
    while(isOn){
        cout << "$ "; //Prints prompt
	getline(cin, usrInput);

	char* inputCpy = new char[usrInput.length() + 1]; //pointer for input string
	strcpy(inputCpy, usrInput.c_str()); //inputCpy now holds c-string copy of input
	unsigned cnt = 0; //counter for slots in argv[]
	char* token = strtok(inputCpy, ";"); //removes semicolons
	//delete inputCpy //FIXME - Deallocate mem later	
	char* a[usrInput.length() + 1]; //Proper size? - FIXME

	while(token != NULL){
	    if(*token == '#'){ //Handles comments
		token = NULL;
	   // }else if(token == "||"){ //Will Handle OR
	   // }else if(token == "&&"){ //Will Handle AND
	    }else{ //Default scenario
	        //if(token == "exit") //Will handle exit
		cout << "Token: " << token << endl;
		a[cnt] = token;
		strcat(a[cnt], "\0"); //Null terminates the string - needed? FIXME
		token = strtok(NULL, ";");
		cnt++;
	    }
	}//Removes semicolons
	
	strcat(a[cnt - 1], "\0");
	a[cnt] = token; //Should null term argv[], as token will equal NULL
	
	int pid = fork(); //Forks the process
	if(pid == -1){
	    perror("fork() failed");
	    exit(1);
	}else if(pid == 0){ //Child process
	    cout << "I'm a kid again!" << endl; //Fixme - remove later
	    
	    //Now Tokenize a[] with " " as a delimiter and put it into argv[]
	    cnt = 0; //resets count
	    token = strtok(a[cnt], " ");
	    while(token != NULL){
		cout << "NewToken: " << token << endl;
		argv[cnt] = token;
		strcat(argv[cnt], "\0");
		cnt++;
		token = strtok(NULL, " ");
	    }
	    strcat(argv[cnt - 1], "\0");
	    argv[cnt] = token;

	    if(execvp(argv[0], argv) == -1){ //Fixme - argv[0]=a.out; argv 1 or 0?
		perror("execvp() failed");
	    }
	    exit(1);
	}else if(pid > 0){ //Parent function
	    cout << "Get off my lawn!" << endl; //Fixeme - remove later
	    if(-1 == wait(0)){ //waits for child to finish before continuing
	        perror("wait() failed");
	    }
	}
    }//End while loop

	/*
	for(unsigned i = 0; i < 10; i++){
	    cout << "Argv[" << i << "]: " << argv[i] << endl; //Fixme - remove later
	}
	*/

    return 0;
}

//---------------------OLD CODE BELOW THIS LINE---------------------

//___ERROR LIST___
//test exit after commands
//test semi-colons before/after commands
//Prompt multi-prints when space-separated garbage is entered 
//Test with whitespace before/inbetween/after commands
//Test touch with large, small, and symbol commands
//Must work with &&'s and ||'s as connectors

/*

int main(int argc, char*argv[]){
    bool isRunning = true;
    while(isRunning){
        cout << "$"; //Prints the Prompt
	string input;
	getline(cin, input);
	if(input == "exit"){ //Exit Check
	    cout << "Exiting rshell..." << endl;
	    return 0;
	}
	int cnt = 0;
	char * tokInput = new char[input.size() + 1];
	copy(input.begin(), input.end(), tokInput);//tokInput now = input
	tokInput[input.size()] = '\0'; //Null terminates array
	char* token = strtok(tokInput, ";"); //parses rawInput
	char* arr[] = strtok(tokInput, ";");


	while(token != NULL){//Tokenization
	    cout << "token: " << token << endl;//output - FIXME
	    //if(*token == '#'){
		//strcpy(argv[cnt], "\0"); //null terminates argv
		//token = NULL;
	    }
	    argv[cnt] = token;//places tokenized strings into argv
	    strcat(argv[cnt], "\0");//adds null char to strings in argv
	    token = strtok(NULL, " "); //looks for connectors
	    cnt++; //increment count
	}
	//strcat(argv[cnt], "\0");//Null terminates argv[]
	for(int i = 0; i < cnt; i++){ //prints all values of argv[] - FIXME
	    cout << "argv[" << i << "]: " << argv[i] << endl;
	}
	
	int pid = fork();//forks the process
        if(pid == 0){ //child process
	   //if(execvp(argv[0], &argv[0]) == -1){ - Old
	    if(execvp(argv[0], argv) == -1){
	        perror("execvp failure");
	    }
	    exit(1);
	}else{ //parent
	    if(-1 == wait(NULL)){
	        perror("wait() failure");
	    }
        }
    }//end isRunning loop
    return 0;
}
*/
