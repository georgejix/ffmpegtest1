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
	string deletePath = "/Debug/ffmpegtest1";
	*audioPath = audioPath->replace(audioPath->find(deletePath), deletePath.length(), "/1.mp3");
	read_av_info(audioPath->c_str());
	delete audioPath;
}
