#include <iostream>
#include "GetFileInfo.h"
using namespace std;

void getInfo();

int main(){
	getInfo();
	return 0;
}

void getInfo(){
	char *path = "/home/jx/code/minegithub/ffmpegtest1/1.mp3";
	read_av_info(path);
}
