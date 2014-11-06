#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

#define FLAG_a 1
#define FLAG_l 2
#define FLAG_R 4

int main(int argc, char** argv){
    if(argc < 1){//If too few args are passed in
	return 0;//should actually return an error, maybe
    }
    //bin/ls <FLAGS> <FILES/DIR> "bin/ls -al"
    //or bin/ls <FILES/DIR> <FLAGS> "bin/ls test.cpp -l"

    int flags = 0;//Holds flag values 
    cout << "Flags: " << flags << endl;
    flags |= FLAG_a;
    cout << "Flags: " << flags << endl;
    flags |= FLAG_l;
    cout << "Flags: " << flags << endl;
    flags |= FLAG_R;
    cout << "Flags: " << flags << endl;

    if(argv[1] == "ls"){
        for(int i = 1; i < argc; i++){ //Checks for flags
	    if(argv[i][0] == '-'){
	        for(int j = 1; argv[i][j] != 0; j++){
		    if(argv[i][j] == 'a'){ //Sets -a
		        flags |= FLAG_a;
		    }else if(argv[i][j] == 'l'){ //Sets -l
		        flags |= FLAG_l;
		    }else if(argv[i][j] == 'R'){ //Sets -R
		        flags |= FLAG_R;
		    }
	        }
	    }
        }
	char const *dirName;
	dirName = ".";
        DIR *dirp = opendir(dirName);//opens current file as a directory stream
        if(errno != 0){ //if dirName fails to open
	        cerr << "opendir error" << endl;
	        exit(1);
        }
	dirent *direntp;
        while((direntp = readdir(dirp))){
	    if((direntp->d_name == ".") || direntp->d_name == "..");
	    else{
	        cout << direntp->d_name << endl;
            }
	}
    }
    return 0;
}
