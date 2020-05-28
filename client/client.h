#ifndef CLIENT_H
#define CLIENT_H

#include "../common/common.h"

class client{
    private:
        int devid;
        string serverIPAddr;//¿Í»§¶ËIPµØÖ·
        int port;
        int exitOpt;
        int minDevNum;
        int maxDevNum;
        int totalDevNum;
        int minScrNum;
        int maxScrNum;
        int totalScrNum;
        int logOpt;
        string debugOpt;
        int debugOutput;
        int closeFlag;
        int socketfd;
        int asyNum;
    public:
        client(int id);
        void infoPrint();
        int clientMain();
        int localBind();
        int packAnalysis(byte *buf);
        int identity(byte idsBuf[]);//
        int minimumVerReq();
        int idsAns();
        int sysInfoAns();
        int configAns();
        int procInfoAns();
        int ethInfoAns(short eth);
        int usbInfoAns();
        int usbFileInfoAns();
        int printPortAns();
        int printQueAns();
        int ttySerInfoAns();
        int ttyInfoAns(short ttyType,short devid);
        int disconAns();
        void dataRecv(byte *recvBuf,int recvSize);
        void dataSend(const byte *sendBuf, int sendBufSize);

};

int xlsWrite(int devid,int devNum,int scrNum);


#endif

