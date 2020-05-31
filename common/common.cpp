#include "common.h"

using namespace std;



int packHeadStuff(byte *packBuf, byte headOpt1, byte headOpt2, short packLength, short headOpt3, short dataLength)
{
    packBuf[0]=headOpt1;
    packBuf[1]=headOpt2;

    short packLength_n=htons(packLength);
    short headOpt3_n=htons(headOpt3);
    short dataLength_n=htons(dataLength);

    memcpy(&packBuf[2], &packLength_n, sizeof(packLength_n));
    memcpy(&packBuf[4], &headOpt3_n, sizeof(headOpt3_n));
    memcpy(&packBuf[6], &dataLength_n, sizeof(dataLength_n));
    return 0;
}

int getCPUMHz()
{
    char buff[1000];
    FILE *fp = popen("cat /proc/cpuinfo | grep cpu |grep MHz","r");
	fgets(buff,1000,fp);
	string s=buff;
    string optValue(s.begin()+s.find_first_of("1234567890"), s.begin()+s.find_last_of("1234567890")+1) ;
	pclose(fp);

    return atoi(optValue.c_str());
}

int getRAM()
{
    char buff[1000];
    FILE *fp = popen("cat /proc/meminfo | grep MemTotal ","r");
	fgets(buff,1000,fp);
	string s=buff;
    string optValue(s.begin()+s.find_first_of("1234567890"), s.begin()+s.find_last_of("1234567890")+1) ;
	pclose(fp);

    return atoi(optValue.c_str())/1024/1024;
}

string randStr(int length)
{
    string str;
    for(int i=0;i<length;++i)
    {
        char c=rand()%94+'!';
        if(c=='\''||c=='\"')
        {
            --i;
            continue;
        }
        str=str+c;
    }

    return str;

}

string getEth(string ethName)
{
    char buff[1000];
    string cmd="cat /proc/net/dev | grep "+ethName;
    FILE *fp = popen(cmd.c_str(),"r");
	fgets(buff,1000,fp);
	string s=buff;
	pclose(fp);

    return s;
}

string INTtoIP(uint32_t num)
{  

    string strRet = "";  
    for (int i=0;i<4;i++)  
    {  
        uint32_t tmp=(num>>((3-i)*8))&0xFF;  

        char chBuf[8] = "";
        sprintf(chBuf,"%d",tmp);
        strRet += chBuf;

        if (i < 3)
        {
            strRet += ".";
        }
    }  

    return strRet;  
}