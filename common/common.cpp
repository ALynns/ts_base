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