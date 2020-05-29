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


#define PACK_IDS_REQ_S 0//��������֤����
#define PACK_VER_INFO_C 1//�豸�汾����
#define PACK_IDS_ANS_C 2//�ͻ�����֤��Ϣ
#define PACK_SYS_INFO_REQ_S 3//ϵͳ��Ϣ
#define PACK_SYS_INFO_ANS_C 4
#define PACK_CONF_INFO_REQ_S 5//������Ϣ
#define PACK_CONF_INFO_ANS_C 6
#define PACK_PROC_INFO_REQ_S 7//������Ϣ
#define PACK_PROC_INFO_ANS_C 8
#define PACK_ETH_PORT_INFO_REQ_S 9//��̫����Ϣ
#define PACK_ETH_PORT_INFO_ANS_C 10
#define PACK_FLASH_INFO_REQ_S 11//U�̿���Ϣ
#define PACK_FLASH_INFO_ANS_C 12
#define PACK_FLASH_FILE_INFO_REQ_S 13//U���ļ���Ϣ
#define PACK_FLASH_FILE_INFO_ANS_C 14
#define PACK_PRINT_PORT_INFO_REQ_S 15//��ӡ����Ϣ
#define PACK_PRINT_PORT_INFO_ANS_C 16
#define PACK_PRINT_QUE_INFO_REQ_S 17//��ӡ������Ϣ
#define PACK_PRINT_QUE_INFO_ANS_C 18
#define PACK_TTY_SER_INFO_REQ_S 19//�ն˷�����Ϣ
#define PACK_TTY_SER_INFO_ANS_C 20
#define PACK_TTY_INFO_REQ_S 21//���ն�������Ϣ
#define PACK_TTY_INFO_ANS_C 22
#define PACK_IP_TTY_INFO_REQ_S 23//IP�ն�������Ϣ
#define PACK_IP_TTY_INFO_ANS_C 24
#define PACK_DISCON_REQ_S 25//�Ͽ�����
#define PACK_DISCON_ANS_C 26

using namespace std;

typedef u_char byte;

const string IdsStr="yzmond:id*str&to!tongji@by#Auth^";
const string LocalLogPath = "./local.log";


int packHeadStuff(byte *packBuf, byte headOpt1, byte headOpt2, short packLength, short headOpt3, short dataLength);
int logWrite(string logPath, int mode, string opt, const byte data[], int dataLength);
int getCPUMHz();
int getRAM();
string randStr(int length);
string getEth(string ethName);
#endif

