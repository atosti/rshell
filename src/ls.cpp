#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

#define FLAG_a 1
#define FLAG_l 2
#define FLAG_R 4

//Error check all syscalls (Readdir AND Stat)
//Have another function for -l flag handling and stat checking?
//Put output into a vector and sort it before output
//FIXME - Order output into columns AND alphabetize it
//Test entering multiple files/directories in
//Ensure that entering a directory enters that dir and outputs all files within it
//If passed in file doesn't exist, print error

void printLs(int flags, int statbuf, string fileName, dirent *direntp, DIR *dirp, char const *dirName, bool isSysLink){
    while((direntp = readdir(dirp))){//for each file
	if(fileName != ""){//reg file OR syslink was passed
	    if((flags == 0) || (flags == 1)){
		if(direntp->d_name == fileName){
		    //If reg file...
		    //If sys link...

		    //cout << '-'; //because it's a reg file, not a dir
		    cout << direntp->d_name << endl;
		}
	    }else if((flags == 2) || (flags == 3)){
		if(direntp->d_name == fileName){
		    //output THIS file only with -l info
		    
		}
	    }else if((flags == 4) || (flags == 5)){
		//if FileName is a directory
		//if Curr file is directory - enter it
		cout << direntp->d_name << endl;
	    }else if((flags == 6) || (flags == 7)){
		//-l AND -R are set
		//if curr file is dir; enter it
		//output with -l info
	    }
	}else{//directory or default was passed
	    if((flags == 0) || (flags == 1)){
		cout << direntp->d_name << endl;
	    }else if((flags == 2) || (flags == 3)){
		//output with -l info
	    }else if((flags == 4) || (flags == 5)){
		//if Curr file is directory - enter it
		cout << direntp->d_name << endl;
	    }else if((flags == 6) || (flags == 7)){
		//-l AND -R are set
		//if curr file is dir; enter it
		//output with -l info
	    }
	}
    }
    return;
}

int main(int argc, char** argv){
    if(argc < 1){//If too few args are passed in
	return 0;//FIXME - should actually return an error, maybe
    }
    //bin/ls <FLAGS> <FILES/DIR> "bin/ls -al"
    //or bin/ls <FILES/DIR> <FLAGS> "bin/ls test.cpp -l"
    //ls -l (second number?) is number of hardlinks
    //ls info or info ls

    struct stat statbuf;
    int flags = 0; //Holds value corresponding to flags active
    char const *dirName = "."; //if no dir named, then sets it to curr dir
    string fileName = "";
    bool isSysLink = false; //Holds whether passed file is a sys link

    for(int i = 1; i < argc; i++){ //Checks each argument
	lstat(argv[i], &statbuf);//statbuf gets updated
        if(S_ISDIR(statbuf.st_mode)){//argv[]'s param is a directory
	    statbuf.st_mode = 0;
	    cout << "Aiyyo, I'm a directory!" << endl; //FIXME - remove later
	    dirName = argv[i]; //Sets optional parameter into dir name
	}else if(S_ISREG(statbuf.st_mode)){
	    cout << "I'm a Regular File." << endl; //FIXME - rm later
	    //statbuf.st_mode = 0; //FIXME - is this needed?
	    fileName = argv[i]; //Holds file name if reg file is passed in
	}else if(S_ISLNK(statbuf.st_mode)){
	    cout << "I am a system link!" << endl; //FIXME
	    //statbuf.st_mode = 0; //FIXME - is this needed?
	    if(lstat(".", &statbuf) == -1){ //formerly stat(dirName, &statbuf)
		cerr << "stat() failed" << endl;
		exit(1);
	    }
	    fileName = argv[i]; //Holds file name if sys link
	    isSysLink = true;
	}else{ //argv[]'s param isn't reg file, should be a flag with '-'
	    statbuf.st_mode = 0;//Not sure if legal; seems to work
	    if(argv[i][0] == '-'){
	        for(int j = 1; argv[i][j] != 0; j++){
	            if(argv[i][j] == 'a'){ //Sets -a
		        flags |= FLAG_a;
			cout << "Flag -a triggered!" << endl; //FIXME - remove later
	            }else if(argv[i][j] == 'l'){ //Sets -l
		        flags |= FLAG_l;
			cout << "Flag -l triggered!" << endl; //FIXME - remove later
	            }else if(argv[i][j] == 'R'){ //Sets -R
		        flags |= FLAG_R;
			cout << "Flag -R triggered!" << endl; //FIXME - remove later
	            }
	        }
            }
	}
    } //End flag assignment/optional check
    DIR *dirp;
    dirent *direntp;
    cout << "dirName: " << dirName << endl; //FIXME - Remove later
    if((dirp = opendir(dirName)));
    else{
	cerr << "opendir error" << endl; //Error check
    }
    cout << "---------------------" << endl;//FIXME - Remove later
    if(flags == 0){ //only "ls" - Handles if ". .." appear
 	direntp = readdir(dirp);
	direntp = readdir(dirp);
    }

    printLs(flags, statbuf.st_mode, fileName, direntp, dirp, dirName, isSysLink); //Prints everything cleanly
/*
    if(flags == 0 || flags == 1){ //for "ls' and "ls -a"
	if(fileName != ""){ //If reg. file passed
	    while((direntp = readdir(dirp))){
	        if(direntp->d_name != fileName);
	        else{
		    cout << direntp->d_name << endl;
		    //FIXME - Output in a proper format
	        }
	    }
        }else{//if default or directory was passed
            while((direntp = readdir(dirp))){
	        cout << direntp->d_name << endl;
		//FIXME - Output in proper format
            }
        }
    }else if(flags == 2){ //only "ls -l"
	//FIXME - Error check when stat is called
	
    }else if(flags == 3){ //only "ls -la"

    }else if(flags == 4){ //only "ls -R"

    }else if(flags == 5){ //only "ls -aR"

    }else if(flags == 6){ //only "ls -lr"

    }else if(flags == 7){ //only "ls -alR"

    }
*/


/*
    if(fileName != ""){ //If regular file was passed
	while((direntp = readdir(dirp))){
	    if(direntp->d_name != fileName);
	    else{
		cout << direntp->d_name << endl;
	    }
	}
    }else{//if default or directory was passed
        while((direntp = readdir(dirp))){
	    cout << direntp->d_name << endl;
        }
    }
*/

    return 0;
}
