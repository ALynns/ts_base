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
        int minScrNum;
        int maxScrNum;
        int logOpt;
        string debugOpt;
        int debugOutput;
        int closeFlag;
        int socketfd;
    public:
        client(int id);
        void infoPrint();
        int clientMain();
        int localBind();
        int packAnalysis(byte *buf);
        int identity(byte idsBuf[]);//
        int minimumVerReq();
        int idsAns();
        void dataRecv(byte *recvBuf,int recvSize);
        void dataSend(const byte *sendBuf, int sendBufSize);

};


#endif

