#ifndef

#include "../common/common.h"

#include

#define debugOptNum 6

class clientInfo{
    private:
        string clientIPAddr;//¿Í»§¶ËIPµØÖ·
        int exitOpt;
        int minDevNum;
        int maxDevNum;
        int minScrNum;
        int maxScrNum;
        int logOpt;
        char debugOpt[debugOptNum];
        int debugOutput;
    public:
        clientInfo();


};