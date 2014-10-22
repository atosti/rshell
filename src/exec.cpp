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
	string input;
	getline(cin, input);
	if(input == "exit"){ //Exit Test
	    cout << "Exiting rshell..." << endl;
	    return 0;
	}
	char * tokInput = new char[input.size() + 1];
	copy(input.begin(), input.end(), tokInput);//input2 now = rawInput
	tokInput[input.size()] = '\0'; //Null terminates the array
        char* token = strtok(tokInput, ";|&"); //parses rawInput
        int cnt = 0; //counter
	string commands = tokInput;

	while(token != NULL){
	    if(*token == '#'){
		//token = strtok(NULL, "#");
		//*token = '\0';
		commands.at(cnt) = '\0';
		token = NULL;
	    }else{
		token = strtok(NULL, ";|&"); //searches for Null and ignores connectors
		cnt++; //increment count
	    }
	}
	/*for(unsigned i = 0; i < commands.size(); i++){
	    if(commands.at(i) == '\0');
	    else{
		commands.at(i) = tokInput[i];
	    }
	}*/

	strcat(tokInput,"\0");
	strcpy(argv[0], tokInput); //Gives argv[] the commands
	
	int pid = fork();
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
