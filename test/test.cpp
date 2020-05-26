#include "../common/common.h"

using namespace std;

int main()
{
    byte b[22]={0,1,2,3,4,5,6,7,8,9,10,11,12,'a','b','c','d','e','f','g','\n',' '};
    logWrite("./test.log", 0, "test1","" ,0);
    logWrite("./test.log", 1, "test2", b, 22);
    return 0;
}