#ifndef CLIENT_H
#define CLIENT_H

#include "../common/common.h"



class client{
    private:
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

        int socketfd;
    public:
        client();
        void infoPrint();
        int clientMain();
        int localBind();
        int identity();
        int dataRecv();
        int dataSend();

};


#endif

