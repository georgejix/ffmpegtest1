#include <iostream>
#include <stdlib.h>
#include <string>
#include "GetFileInfo.h"
using namespace std;

void getInfo(char *);

int main(int args, char *argv[]){
	getInfo(argv[0]);
	return 0;
}

void getInfo(char *projectPath){
	string *audioPath = new string(projectPath);
	*audioPath = audioPath->substr(0, audioPath->find("/Debug"));
	audioPath->append("/1.mp3");
	read_av_info(audioPath->c_str());
	delete audioPath;
}
