#include "common.h"

using namespace std;

int packHeadStuff(byte *packBuf, byte headOpt1, byte headOpt2, short packLength, short headOpt3, short dataLength)
{
    packBuf[0]=headOpt1;
    packBuf[1]=headOpt2;
    memcpy(&packBuf[2], htons(packLength), sizeof(packLength));
    memcpy(&packBuf[4], htons(headOpt3), sizeof(headOpt3));
    memcpy(&packBuf[6], htons(dataLength), sizeof(dataLength));
    return 0;
}