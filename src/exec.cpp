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

int main(int argc, char* argv[]){
    string usrInput; //Holds user input
    while(1){
        cout << "$ "; //Prints prompt
	getline(cin, usrInput);
	char* inputCpy = new char[usrInput.length() + 1]; //pointer for input str
	strcpy(inputCpy, usrInput.c_str()); //inputCpy holds c-str copy of input
	unsigned cnt = 0; //counter for slots in argv[]
	//char buf[usrInput.length() + 1];
	char* token = strtok(inputCpy, ";"); //removes semicolons
	char* a[usrInput.length() + 1]; //Creates array for cmds + args
	while(token != NULL){ //Tokenizes semicolons
	    a[cnt] = token;
	    token = strtok(NULL, ";");
	    cnt++;
	}
	strcat(a[cnt - 1], "\0");
	a[cnt] = token; //Null terminates a[]

/*
	int x = 0;
	for(unsigned in = 0; in < cnt; in++){
	    cout << "a[" << in << "]: " << a[in] << endl;
	    x = in;
	}

//Old Fixme code - delete whenever checking for null in argv[] works
	cout << "Test:" << endl;
	cout << a[x+1] << endl;
	
	//cout << strcmp(a[x+1], "\0") << endl;
	cout << "......." << endl;
	return 0;
*/

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
		//_____Operator Checks - IO Redirection_____
			//Note: curr - 1 is just the prior arg, you need to
			//couple it with the flags/command it's passed with
			//and redirect that OUTPUT to your output.txt
			//--------------------------------------------------
			//SOLUTION: Have an int "sym" that holds the position
			//of the last found symbol and then do a for loop
			//that combines all of the argv[]'s on one end
			//together and all of the other end together as well
			//and then update sym each time an operator is found
			//be sure to iterate it after the for loop concat, tho
		strcat(argv[curr], "\0"); //Null terms
		token = strtok(NULL, " ");
		curr++; 
/*
	    	int x = 0; //Used for traversing
	   	int y = 0; //argv[]'s strings' chars
		int lastSym = 0; //char location of last symbol in an argv[] string
		while(strcmp(argv[x], "\0") != 0){//for each
		    while(argv[x][y] != '\0'){// char in argv[]
			if(argv[x][y] == '>'){//is Output
			    //if count + 1 == ">", then is Append
			    lastSym = y;
			}else if(argv[x][y] == '<'){//is Input
			    lastSym = y;
			}else if(argv[x][y] == '|'){//is Pipe
			    lastSym = y;
			}
			y++;//iterate through chars
		    }
		    //End of a string
		    if(y > 0){//if there is a symbol in argv[x]
			int pid2;
			if((pid2 = fork()) == -1){
			    perror("pid2() fork failed");
			    exit(1);
			}
		    }
		    x++;//iterate through strings
		    y = 0; //Resets y to beginning of string
		}
*/
	    }
	    strcpy(argv[curr], "\0");
	    //strcat(argv[curr], "\0"); //Null terms the last string
	    //argv[curr] = token; //Null terms array
	    int x = 0; //Used for traversing
	    int y = 0; //argv[]'s strings' chars
	    int lastSym = 0; //char location of last symbol in an argv[] string
	cout << "HALP" << endl;
		while(argv[x][0] != '\0'){//for each - FIXME - WHY THE HELL DOES THIS NOT TRIGGER?!
		    while(argv[x][y] != '\0'){// char in argv[]
			if(argv[x][y] == '>'){//is Output
			    //if count + 1 == ">", then is Append
			    lastSym = y;
			}else if(argv[x][y] == '<'){//is Input
			    lastSym = y;
			}else if(argv[x][y] == '|'){//is Pipe
			    lastSym = y;
			}
			cout << "char" << y << ": " << argv[x][y] << endl;
			y++;//iterate through chars
		    }
		    cout << "string" << x << ": " << argv[x] << endl;
		    cout << "strcmp-argv[" << x++ << "]: " << strcmp(argv[x], "\0") << endl;
		    //End of a string
		    /*if(y > 0){//if there is a symbol in argv[x]
			int pid2;
			if((pid2 = fork()) == -1){
			    perror("pid2() fork failed");
			    exit(1);
			}
			
		    }*/
		    x++;//iterate through strings
		    y = 0; //Resets y to beginning of string
		}
		cout << "Final argv[" << x << "]: " << argv[x] << endl;	

	    //strcat(argv[curr], "\0"); //Null terms the last string
	    //argv[curr] = token; //Null terms array
	    //for(int z = 0; z < curr; z++){
	//	cout << "argv[" << z << "]: " << argv[z] << endl;
	  //  }
	    

	    int pid = fork();
	    if(pid == -1){
		perror("pid fork  failed");
		exit(1);
	    }
	    if(pid == 0){ //Child process
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
        delete inputCpy; //Deallocates memory - Fixme, might go elsewhere
    }//End while loop

    return 0;
}
