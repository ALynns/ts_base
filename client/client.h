#ifndef CLIENT_H
#define CLIENT_H

#include "../common/common.h"



class client{
    private:
        string serverIPAddr;//�ͻ���IP��ַ
        int port;
        int exitOpt;
        int minDevNum;
        int maxDevNum;
        int minScrNum;
        int maxScrNum;
        int logOpt;
        string debugOpt;
        int debugOutput;

        int socketfd;
    public:
        client();
        void infoPrint();
        int clientMain();
        int localBind();
        int identity();//
        void dataRecv(char *recvBuf,int recvSize);
        void dataSend(const char *sendBuf, int sendBufSize);

};


#endif

