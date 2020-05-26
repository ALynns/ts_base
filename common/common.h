#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <fstream>

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/file.h>  
#include <sys/stat.h> 
#include <netinet/in.h>
#include <sys/errno.h>
#include <arpa/inet.h>

using namespace std;

typedef char byte;
//int logWrite(string logPath,string str);

int packHeadStuff(byte *packBuf, byte headOpt1, byte headOpt2, short packLength, short headOpt3, short dataLength);
int logWrite(string logPath,int mode,string data);


#endif

