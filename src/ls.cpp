#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

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
//FIXME - if "./a.out syslink flags" gets passed, there's a seg fault

//Input is as follows:
    //bin/ls <FLAGS> <FILES/DIR> "bin/ls -al"
    //or bin/ls <FILES/DIR> <FLAGS> "bin/ls test.cpp -l"
    //ls -l (second number?) is number of hardlinks
    //ls info or info ls
int main(int argc, char** argv){
    if(argc < 1){//If too few args are passed in
	cout << "ls error: must enter at least 1 arguments" << endl;
	exit(1); //Outputs an error
    }

    struct stat statbuf;
    int flags = 0; //Holds value corresponding to flags active
    char const *dirName = "."; //if no dir named, then sets it to curr dir
    string fileName = ""; //Holds name of non-directories that get passed in
    bool isSysLink = false; //Holds whether passed file is a sys link
    bool isDir = false;

    for(int i = 1; i < argc; i++){ //Checks each argument
	lstat(argv[i], &statbuf);//statbuf gets updated
        if(S_ISDIR(statbuf.st_mode)){//argv[]'s param is a directory
	    statbuf.st_mode = 0;
	    cout << "Aiyyo, I'm a directory!" << endl; //FIXME - remove later
	    isDir = true;
	    dirName = argv[i]; //Sets optional parameter into dir name
	}else if(S_ISREG(statbuf.st_mode)){
	    cout << "I'm a Regular File." << endl; //FIXME - rm later
	    statbuf.st_mode = 0;//Resets statbuf
	    fileName = argv[i]; //Holds file name if reg file is passed in
	}else if(S_ISLNK(statbuf.st_mode)){
	    cout << "I am a system link!" << endl; //FIXME
	    if(lstat(".", &statbuf) == -1){
		cerr << "stat() failed" << endl;
		exit(1);
	    }
	    fileName = argv[i]; //Holds file name if sys link
	    isSysLink = true;
	    statbuf.st_mode = 0;//Resets statbuf
	}else{ //argv[]'s param isn't reg file, should be a flag with '-'
	    statbuf.st_mode = 0;//Resets statbuf
	    if(argv[i][0] == '-'){
	        for(int j = 1; argv[i][j] != 0; j++){
	            if(argv[i][j] == 'a'){ //Sets -a
		        flags |= FLAG_a;
			cout << "Flag -a triggered!" << endl; //FIXME - remove
	            }else if(argv[i][j] == 'l'){ //Sets -l
		        flags |= FLAG_l;
			cout << "Flag -l triggered!" << endl; //FIXME - remove
	            }else if(argv[i][j] == 'R'){ //Sets -R
		        flags |= FLAG_R;
			cout << "Flag -R triggered!" << endl; //FIXME - remove
	            }
	        }
            }
	}
    } //End flag assignment/optional check
    
    vector<string> v; //Vector to hold output
    char temp[512];
    DIR *dirp;
    dirent *direntp;
    cout << "dirName: " << dirName << endl; //FIXME - Remove
    if((dirp = opendir(dirName)));
    else{
	cerr << "opendir error" << endl; //Error check
    }
    cout << "---------------------" << endl;//FIXME - Remove later
    if((flags == 0) || (flags == 2) || (flags == 4)){ //handles . and ..
 	direntp = readdir(dirp);
	direntp = readdir(dirp);
    }
    while(direntp = readdir(dirp)){
	v.push_back(direntp->d_name);
    }
    sort(v.begin(), v.end());

    for(unsigned i = 0; i < v.size(); i++){
	    if((flags == 0) || (flags == 1)){//ls or ls -a
        	if((isDir) || (v.at(i) == fileName)){
		    cout << v.at(i) << endl;
		}
	    }else if((flags == 2) || (flags == 3)){//ls -l or ls -al
		if((isDir) || (v.at(i) == fileName)){
		    if(isSysLink){
			cout << "l";
		    }else if(isDir){ //check for something else here
			cout << "d";
		    }else{
			cout << "-";
		    }//End file type check
		    
		    char str[sizeof(dirName) + sizeof(v.at(i)) + 128];
		    strcpy(str, "./");
		    strcat(str, dirName);
		    strcat(str, "/");
		    string str1 = v.at(i);
		    strcat(str, str1.c_str()); 

		    if(stat(str, &statbuf) == -1){//Sets statbuf
			cerr << "stat() failed" << endl;
			exit(1);
	    	    }
		    if(statbuf.st_mode & S_IRUSR){//Owner permissions
			cout << "r";
		    }else{
			cout << "-";
		    }
		    if(statbuf.st_mode & S_IWUSR){
			cout << "w";
		    }else{
		        cout << "-";
		    }
		    if(statbuf.st_mode & S_IXUSR){
			cout << "x";
		    }else{
		        cout << "-";
		    }
		    if(statbuf.st_mode & S_IRGRP){//Group permissions
			cout << "r";
		    }else{
			cout << "-";
		    }
		    if(statbuf.st_mode & S_IWGRP){
			cout << "w";
		    }else{
			cout << "-";
		    }
		    if(statbuf.st_mode & S_IXGRP){
			cout << "x";
		    }else{
			cout << "-";
		    }
		    if(statbuf.st_mode & S_IROTH){//Other permissions
			cout << "r";
		    }else{
		        cout << "-";
		    }
		    if(statbuf.st_mode & S_IWOTH){
			cout << "w";
		    }else{
		        cout << "-";
		    }
		    if(statbuf.st_mode & S_IXOTH){
			cout << "x";
		    }else{
		        cout << "-";
		    }
		    int num = 0;
		    if(num = statbuf.st_nlink){//Num system links
			cout << " " << num << " ";
		    }else{
			cerr << "st_nlink error" << endl;
			exit(1);
		    }
		    struct passwd *pwd;
		    if((pwd = getpwuid(statbuf.st_uid)) != NULL){//User ID
			cout << pwd->pw_name << " ";
		    }else{
			cerr << "Error with getpwuid()" << endl;
			exit(1);
		    }
		    struct group *grp;
		    if((grp = getgrgid(statbuf.st_gid)) != NULL){//Group ID
			cout << grp->gr_name << " ";
		    }else{
			cerr << "Error with getgrgid()" << endl;
			exit(1);
		    }
		    cout << statbuf.st_size << " "; //File size in bytes
		    if(num = statbuf.st_mtime){//Time of last modification
			time_t num;
			char buffer [80];
			struct tm *timeinfo;
			time(&num);
			timeinfo = localtime(&num);
			cout << buffer << " ";
		    }else{
			cerr << "st_mtime error" << endl;
			exit(1);
		    }
		    cout << v.at(i) << endl;
	    }else if((flags == 4) || (flags == 5)){
		//if FileName is a directory
		//if Curr file is directory - enter it
		cout << v.at(i) << endl;
	    }else if((flags == 6) || (flags == 7)){//ls -lR and ls -alR
		//-l AND -R are set
		//if curr file is dir; enter it
		//output with -l info
	    }
	}
    }
    return 0;
}
