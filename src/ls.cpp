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
#include <clocale>

using namespace std;

#define FLAG_a 1
#define FLAG_l 2
#define FLAG_R 4

//TODO:
//Format the output left to right in columns
//Sort output alphabetically case insensitive
//Allow multiple files to be input and then output them properly
//    this means mult. separate files, not a path of numerous files

//Note: Input is as follows:
//bin/ls <FLAGS> <FILES/DIR> "bin/ls -al"
//or bin/ls <FILES/DIR> <FLAGS> "bin/ls test.cpp -l"

int main(int argc, char** argv){
    //Main variables and setup for later
    int flags = 0;
    struct stat statbuf;
    bool isSysLink = false;
    bool isDir = false;
    char const *dirName = "."; //if no dir named, then sets it to curr dir
    //FIXME - Below is an outine of how I think dirNames should be held
    //		both it an fileName should be arrays/vectors that can hold unlimited values
    //		Vectors makes more sense with pushback now that I think on it
    string fileName = "";
    setlocale(LC_ALL, "en_US.UTF-8");

    //Flag assignment and Optional param check
    for(int i = 1; i < argc; i++){
	lstat(argv[i], &statbuf);

	//Checks if param is a dir, reg, link, or a flag
        if(S_ISDIR(statbuf.st_mode)){
	    statbuf.st_mode = 0;
	    isDir = true;
	    dirName = argv[i];
	}else if(S_ISREG(statbuf.st_mode)){
	    statbuf.st_mode = 0;//Resets statbuf
	    fileName = argv[i]; //Holds file name if reg file is passed in
	}else if(S_ISLNK(statbuf.st_mode)){
	    if(lstat(".", &statbuf) == -1){
		perror("lstat2 failed");
		exit(1);
	    }
	    fileName = argv[i]; //Holds file name if sys link
	    isSysLink = true;
	    statbuf.st_mode = 0;//Resets statbuf
	//Flag checking
	}else{
	    statbuf.st_mode = 0;//Resets statbuf
	    if(argv[i][0] == '-'){
	        for(int j = 1; argv[i][j] != 0; j++){
	            if(argv[i][j] == 'a'){
		        flags |= FLAG_a;
	            }else if(argv[i][j] == 'l'){
		        flags |= FLAG_l;
	            }else if(argv[i][j] == 'R'){
		        flags |= FLAG_R;
	            }
	        }
            }else{
		//Assigns garbage value to dirName so it will
		//return an error on opendir
		dirName = argv[i];
	    }
	}
    }

    //Output vector
    vector<string> v;
    char temp[512];

    //Checks validity of directory
    DIR *dirp;
    dirent *direntp;
    if((dirp = opendir(dirName)));
    else{
	perror("opendir failed");
	exit(1);
    }

    //Skips . and .. if -a not passed
    if((flags == 0) || (flags == 2) || (flags == 4)){
	if(direntp = readdir(dirp)){
	}else{
	    perror("readdir1 failed");
	}
	if(direntp = readdir(dirp)){
	}else{
	    perror("readdir2 failed");
	}
    }

    //Remaining part of dir is read
    errno = 0;
    while((direntp = readdir(dirp))){
	v.push_back(direntp->d_name);
    }
    if(errno != 0){
	perror("readdir3 failed");
    }
    //Closes dir
    if(closedir(dirp) == -1){
        perror("closedir error"); 
    }

    //FIXME - Make case insensitive later
    sort(v.begin(), v.end());


    //Output Handler
    for(unsigned i = 0; i < v.size(); i++){
	    bool isValid = true; //Used to check with fileName's passed in
	    //ls or ls -a
	    if((flags == 0) || (flags == 1)){
		    if(fileName != ""){//if a file was passed
			isValid = false;
			if(fileName == v.at(i)){ //if this matches that file
			    isValid = true;
			}
		    }
		    //No file passed or a match found, print
		    if(isValid){
			cout << v.at(i) << endl;
		    }
	    //ls -l or ls -la
	    }else if((flags == 2) || (flags == 3)){
		if(fileName != ""){//if file passed
		    isValid = false;
		    if(fileName == v.at(i)){//of this IS file
			isValid = true;
		    }
		}
		if(isValid){//no file passed
		    //File type check
		    if(isSysLink){
			cout << "l";
	    	    }else if(S_ISDIR(statbuf.st_mode)){
			cout << "d";
		    }else{
			cout << "-";
		    }
		    
		    //FIXME - Use dirName.at().size() + v.at(i).size() + 1?
		    //Appends ./ and prepends / to dirName
		    char str[sizeof(dirName) + sizeof(v.at(i)) + 128];
		    strcpy(str, "./");
		    strcat(str, dirName);
		    strcat(str, "/");
		    string str1 = v.at(i);
		    strcat(str, str1.c_str()); 

		    if(stat(str, &statbuf) == -1){//Sets statbuf
			perror("stat() failed");
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
			perror("st_nlink error");
			exit(1);
		    }
		    struct passwd *pwd;
		    if((pwd = getpwuid(statbuf.st_uid)) != NULL){//User ID
			cout << pwd->pw_name << " ";
		    }else{
			perror("getpwuid failed");
			exit(1);
		    }
		    struct group *grp;
		    if((grp = getgrgid(statbuf.st_gid)) != NULL){//Group ID
			cout << grp->gr_name << " ";
		    }else{
			perror("getgrgid failed");
			exit(1);
		    }
		    cout << statbuf.st_size << " "; //File size in bytes
		    if(num = statbuf.st_mtime){//Time of last modification
			time_t rawtime = statbuf.st_mtime;
			char buffer [80];
		        struct tm *timeinfo;
			timeinfo = localtime(&rawtime);
			strftime(buffer, 80, "%b %e %H:%M", timeinfo);
			cout << buffer << " ";
		    }else{
			perror("st_mtime failed");
			exit(1);
		    }
		    cout << v.at(i) << endl;
	    }else if((flags == 4) || (flags == 5)){
		//if FileName is a directory
		//if Curr file is directory - enter it
	    }else if((flags == 6) || (flags == 7)){//ls -lR and ls -alR
		//-l AND -R are set
		//if curr file is dir; enter it
		//output with -l info
	    }
	}
    }
    return 0;
}
