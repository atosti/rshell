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
/* Broken - Fix later to help clean up code
void tokenize(char* arr[], char* token, string delims){
    unsigned cnt = 0;
    while(token != NULL){
	cout << "arr[" << cnt << "]: " << arr[cnt] << endl;
	arr[cnt] = token;
	token = strtok(NULL, delims);
	cnt++;
    }
    strcat(arr[cnt - 1], "\0");
    arr[cnt] = token;
    return;
}*/

void redirect(char* argv[], int num, bool hasOp){//consider making a bool to check if any operators are found
    int x = 0;
    int y = 0;
    int last = -1;//char loc. of last symbol
    int flags = -1; //Holds val corresponding to type of sym

    while(x != num){//for each string
        y = 0;//Resets y to beginning of a string
	while(argv[x][y] != '\0'){// char in argv[]
	    if(argv[x][y] == '>'){//is Output
		if(argv[x][y+1] == '>'){//Append
		    //cout << "Append found!" << endl;
		    flags = 1;
		    hasOp = true;
		}else{
		    //cout << "Output found!" << endl;
		    last = x;
		    flags = 0;
		    hasOp = true;
		}
	    }else if(argv[x][y] == '<'){//is Input
		//cout << "Input found!" << endl;
		flags = 2;
		hasOp = true;
	    }else if(argv[x][y] == '|'){//is Pipe
		//cout << "Pipe found!" << endl;
		flags = 3;
		hasOp = true;
	    }
	    y++;//iterate through chars
	}//End of a string
	x++;//iterate through strings
    }
    //cout << "Flags before check: " << flags << endl;
    if(flags == 0){//Output
	if(last == (x - 1)){//Ends with >
	    cout << "error: expected argument for '>'" << endl; 
	    //FIXME - Ensure exit goes back to prompt properly
	    exit(0);
	}else if(last == 0){//Symbol is first cmd
	    //FIXME - need to fdo open then cout will overwrite output file
	    cout << "";//Clears the output file
	}else{
	    //New char* [] goes here - FIXME FIXME FIXME
	    cerr << "argv[x-1]: " << argv[x-1] << endl;
	    cerr << "argv[x-2]: " << argv[x-2] << endl;
	    int fdo = open(argv[x - 1], O_WRONLY);
	    if(fdo == -1){
		perror("open output");
		exit(0);
		//Go back to prompt
	        //ensure execvp is NOT run on this
	    }
	    close(1);//close stdout
	    dup(fdo); //fdo is now in slot 1(stdout)
	    strcpy(argv[x-1], "\0");
	    strcpy(argv[x-2], "\0");
	}
    }
    if(flags != -1){//Recursive call
	redirect(argv, last, hasOp);
    }
    //cerr << "argv[x]: " << argv[x] << endl;
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
		cerr << "-----------" << endl;
	    	while(argv[num] != NULL){//counts length of ars in argv
		    cerr << "argv[" << num << "]: " << argv[num] << endl;
		    num++;
	        }
		cerr << "-----------" << endl;
		bool hasOp = false;
	        redirect(argv, num, hasOp); //Calls check for i/o redirection

		//FIXME - do these next lines even get called?
		cerr << "-----------" << endl;
	    	while(argv[num] != NULL){//counts length of ars in argv
		    cerr << "argv[" << num << "]: " << argv[num] << endl;
		    num++;
	        }
		cerr << "-----------" << endl;

		/*if(hasOp == true){//Confirm that real cmds don't get called twice
		    //This should avoid children from running execvp multiple times
		    exit(0);
		}*/
		//Create new arr[] here?

	        int pid3 = fork();
	        if(pid3 == -1){
		    perror("pid fork  failed");
		    exit(1);
	        }
	  	if(pid3 == 0){ //Child process
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
