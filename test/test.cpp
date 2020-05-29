#include "../server/common.h"

using namespace std;

int main()
{

    fstream fs("../client/config.dat",ios::in|ios::out);

    string ethInfo=getEth("ens");
	int i=0;
    size_t pos=ethInfo.find_first_of(":");
    for(;i<16;++i)
    {
        pos=ethInfo.find_first_of("0123456789",pos);
        cout<<atoi(&(ethInfo.c_str()[pos]))<<endl;
        pos=ethInfo.find_first_not_of("0123456789",pos);
    }
    return 0;
}