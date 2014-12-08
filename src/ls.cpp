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

int flagCheck(int &argc, char** &argv){
    int flags = 0;
    for(unsigned i = 1; i < argc; i++){
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
        }
    }
    return flags;
}

//1 = files passed, 2 = dirs passed, 3 = both passed, 0 = none passed, -1 = error
int nameSort(int argc, char** argv, vector<string> &dirName, vector<string> &fileName){
    struct stat sb;
    for(unsigned i = 1; i < argc; i++){
	//Do nothing, is flag
	if(argv[i][0] == '-');
	else{
	    if(lstat(argv[i], &sb) == -1){
		perror("lstat() failed");
		return -1;
	    }
	    if(S_ISDIR(sb.st_mode)){
	    	dirName.push_back(argv[i]);
	    }else{
	    	fileName.push_back(argv[i]);
	    }
	}
    }
    //Nothing passed
    if((fileName.size() == 0) && (dirName.size() == 0)){
	return 0;
    //Only files passed
    }else if((fileName.size() > 0) && (dirName.size() == 0)){
	return 1; 
    //Only dirs passed
    }else if((fileName.size() == 0) && (dirName.size() > 0)){
	return 2;
    //Both dirs and files passed
    }else{
	return 3;
    }
}

int printAll(string currFile, string dirName){
    struct stat statbuf;
    //Appends ./ and prepends / to dirName
    string str = "./";
    str.append(dirName);
    str.append("/");
    str.append(currFile);

    if(lstat(str.c_str(), &statbuf) == -1){//Sets statbuf
	perror("printAll lstat() failed");
	exit(1);
    }

    //File type check
    if(S_ISLNK(statbuf.st_mode)){
	cout << "l";
    }else if(S_ISDIR(statbuf.st_mode)){
	cout << "d";
    }else{
	cout << "-";
    }

    //Owner permissions
    if(statbuf.st_mode & S_IRUSR){
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
    //Group permissions
    if(statbuf.st_mode & S_IRGRP){
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
    //Other permissions
    if(statbuf.st_mode & S_IROTH){
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
    //Num of system links
    int num = 0;
    if(num = statbuf.st_nlink){
	cout << " " << num << " ";
    }else{
	perror("st_nlink error");
	exit(1);
    }
    //User ID
    struct passwd *pwd;
    if((pwd = getpwuid(statbuf.st_uid)) != NULL){
	cout << pwd->pw_name << " ";
    }else{
	perror("getpwuid failed");
	exit(1);
    }
    //Group ID
    struct group *grp;
    if((grp = getgrgid(statbuf.st_gid)) != NULL){
	cout << grp->gr_name << " ";
    }else{
	perror("getgrgid failed");
	exit(1);
    }
    //File size in bytes and Time of last mod.
    cout << statbuf.st_size << " ";
    if(num = statbuf.st_mtime){
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
    cout << currFile << endl;

    return 0;
}

int dirOutput(vector<string> &dirName, vector<string> &vout, int flags){
    string currDir = "";
    DIR *dirp;
    dirent *direntp;
    sort(dirName.begin(), dirName.end());
    for(unsigned i = 0; i < dirName.size(); i++){
	cout << dirName.at(i) << ":" << endl;
    	currDir = dirName.at(i);
	if((dirp = opendir(currDir.c_str())));
    	else{
	    perror("opendir failed");
	    return -1;
    	}
	//Skips . and .. if -a not passed
    	if((flags == 0) || (flags == 2) || (flags == 4) || (flags == 6)){
	    if(direntp = readdir(dirp));
	    else{
	        perror("readdir1 failed");
		return -1;
	    }
	    if(direntp = readdir(dirp)){
	    }else{
	        perror("readdir2 failed");
		return -1;
	    }
        }
	errno = 0;
    	while((direntp = readdir(dirp))){
	    vout.push_back(direntp->d_name);
    	}
    	if(errno != 0){
	    perror("readdir failed");
	    return -1;
    	}
    	//Closes directory
    	if(closedir(dirp) == -1){
            perror("closedir error"); 
	    return -1;
    	}
	//Outputs vout
	sort(vout.begin(), vout.end());
	//If -l was passed
        if((flags == 2) || (flags == 3) || (flags == 6) || (flags == 7)){
	    struct stat sb;
	    if(lstat(currDir.c_str(), &sb) == -1){
		perror("lstat failed");
		return -1;
	    }
	    //FIXME - Incorrect total being displayed
	    cout << "Total " << sb.st_blocks << endl;
	    for(unsigned j = 0; j < vout.size(); j++){
	    	printAll(vout.at(j), currDir);
	    }
	}else{
	    for(unsigned j = 0; j < vout.size(); j++){
	    	cout << vout.at(j) << "  ";
	    }
	    cout << "\n" << endl;
	}
	//Clears vout for next dir
	while(vout.size() > 0){
	    vout.pop_back();
	}
    }
    return 0;
}

int fileOutput(vector<string> &fileName, vector<string> &vout, int flags){
    //FIXME - How to handle Files that are paths
    DIR *dirp;
    dirent *direntp;
    string currDir = ".";
    string currFile = "";	
    if(dirp = opendir(currDir.c_str()));
    else{
	perror("opendir failed");
        return -1;
    }
    errno = 0;
    while(direntp = readdir(dirp)){
	currFile = direntp->d_name;
        for(unsigned i = 0; i < fileName.size(); i++){
	    //If currFile matches a file in the vector
	    if(fileName.at(i) == currFile){
	       vout.push_back(currFile);
	    }
	}
    }
    if(errno != 0){
        perror("readdir failed");
	return -1;
    }
    if(closedir(dirp) == -1){
        perror("closedir failed");
	return -1;
    }
    //Outputs file in vout
    sort(vout.begin(), vout.end());
    //If -l flag was passed
    if((flags == 2) || (flags == 3) || (flags == 6) || (flags == 7)){
	for(unsigned i = 0; i < vout.size(); i++){
	    printAll(vout.at(i), currDir);
	}
    //No -l flag
    }else{
    	for(unsigned i = 0; i < vout.size(); i++){
            cout << vout.at(i) << " ";
    	}
        cout << endl;
    }
    return 0;
}

int noOutput(vector<string> &vout, int flags){
    DIR *dirp;
    dirent *direntp;
    string currDir = ".";
    string currFile = "";
    if((dirp = opendir(currDir.c_str())));
    else{
	perror("opendir failed");
	return -1;
    }
    //Skips . and .. if -a not passed
    if((flags == 0) || (flags == 2) || (flags == 4) || (flags == 6)){
        if(direntp = readdir(dirp));
	else{
	    perror("readdir1 failed");
	}
	if(direntp = readdir(dirp));
	else{
	    perror("readdir2 failed");
	}
    }
    errno = 0;
    while((direntp = readdir(dirp))){
        vout.push_back(direntp->d_name);
    }
    if(errno != 0){
	perror("readdir3 failed");
	return -1;
    }
    //Closes directory
    if(closedir(dirp) == -1){
        perror("closedir failed"); 
	return -1;
    }
    //Outputs files
    sort(vout.begin(), vout.end());
    //If -l is set
    if((flags == 2) || (flags == 3) || (flags == 6) || (flags == 7)){
	for(unsigned i = 0; i < vout.size(); i++){
	    //int printAll(string currFile, string dirName)
	    printAll(vout.at(i), currDir);
	}
    //If not long list
    }else{
	for(unsigned i = 0; i < vout.size(); i++){
	    cout << vout.at(i) << "  ";
        }
        cout << endl;
    }
    return 0;
}

int outputHandler(int ns, int flags, vector<string> dirName, vector<string> fileName){
    string currDir = "";
    string currFile = "";
    vector<string> vout; //holds files to be output

    //---No files passed---
    if(ns == 0){
	if(noOutput(vout, flags) == -1){
	    cout << "noOutput failed" << endl;
	    return -1;
	}
    //---Only files---
    }else if(ns == 1){
	if(fileOutput(fileName, vout, flags) == -1){
	    cout << "fileOutput failed" << endl;
	    return -1;
	}
    //---Only dirs---
    }else if(ns == 2){
	if(dirOutput(dirName, vout, flags) == -1){
	    cout << "dirOutput failed" << endl;
	    return -1;
	}
    //---Both passed---
    }else{
	if(fileOutput(fileName, vout, flags) == -1){
	    cout << "fileOutput failed" << endl;
	    return -1;
	}
	if(dirOutput(dirName, vout, flags) == -1){
	    cout << "dirOutput failed" << endl;
	    return -1;
	}
    }
    return 0;
} 

int main(int argc, char** argv){
    int flags = flagCheck(argc, argv);
    vector<string> dirName;
    vector<string> fileName;

    int ns = nameSort(argc, argv, dirName, fileName);
    //Exit on ns error
    if(ns == -1){
	cout << "nameSort failed" << endl;
	exit(1);
    }
    if(outputHandler(ns, flags, dirName, fileName) == -1){
	cout << "outputHandler failed" << endl;
	exit(1);
    }

    return 0;
}

//TODO:
//Format the output left to right in columns
//Sort output alphabetically case insensitive
//Allow multiple files to be input and then output them properly
//    this means mult. separate files, not a path of numerous files
//    make dirName and fileName vectors

//Note: Input is as follows:
//bin/ls <FLAGS> <FILES/DIR> "bin/ls -al"
//or bin/ls <FILES/DIR> <FLAGS> "bin/ls test.cpp -l"

//Handles -l output
/*
int printAll(string currFile, struct stat statbuf, const char* dirName){
    //File type check
    if(lstat(currFile.c_str(), &statbuf) == -1){
	perror("lstat() failed");
	exit(1);
    }
    if(S_ISLNK(statbuf.st_mode)){
	cout << "l";
    }else if(S_ISDIR(statbuf.st_mode)){
	cout << "d";
    }else{
	cout << "-";
    }    

    //FIXME - Use dirName.at().size() + currFile.size() + 1?
    //		when they're vectors
    //Appends ./ and prepends / to dirName
    //FIXME - This size array is wrong
    char str[sizeof(dirName) + sizeof(currFile) + 128];
    strcpy(str, "./");
    strcat(str, dirName);
    strcat(str, "/");
    string str1 = currFile;
    strcat(str, str1.c_str()); 

    //FIXME - implement -l into its own function
    //str, statbuf

    if(lstat(str, &statbuf) == -1){//Sets statbuf
	perror("lstat() failed");
	exit(1);
    }
    //Owner permissions
    if(statbuf.st_mode & S_IRUSR){
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
    //Group permissions
    if(statbuf.st_mode & S_IRGRP){
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
    //Other permissions
    if(statbuf.st_mode & S_IROTH){
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
    //Num of system links
    int num = 0;
    if(num = statbuf.st_nlink){
	cout << " " << num << " ";
    }else{
	perror("st_nlink error");
	exit(1);
    }
    //User ID
    struct passwd *pwd;
    if((pwd = getpwuid(statbuf.st_uid)) != NULL){
	cout << pwd->pw_name << " ";
    }else{
	perror("getpwuid failed");
	exit(1);
    }
    //Group ID
    struct group *grp;
    if((grp = getgrgid(statbuf.st_gid)) != NULL){
	cout << grp->gr_name << " ";
    }else{
	perror("getgrgid failed");
	exit(1);
    }
    //File size in bytes and Time of last mod.
    cout << statbuf.st_size << " ";
    if(num = statbuf.st_mtime){
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
    cout << currFile << endl;

    return 0;
}

bool sFunc(string i, string j){
    return i<j;
}

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
		//Assigns dirName garbage so opendir fails
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
    sort(v.begin(), v.end(), sFunc);

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
		printAll(v.at(i), statbuf, dirName);
	    //ls -R and ls -aR
	    }else if((flags == 4) || (flags == 5)){
		//if FileName is a directory
		//if Curr file is directory - enter it
	    //ls -lR and -alR
	    }else if((flags == 6) || (flags == 7)){//ls -lR and ls -alR
		//-l AND -R are set
		//if curr file is dir; enter it
		//output with -l info
	    }
    }
    return 0;
}
*/
