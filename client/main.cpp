#include "client.h"

using namespace std;

int main(int argc,char *argv[])
{
    if(argc<3)
    {
        cout<<"²ÎÊý²»×ã"<<endl;
        return 0;
    }

    int num=atoi(argv[2]);
    int first_id=atoi(argv[1]);
    
    for(int i=0;i<num;++i,++first_id)
    {
        int pid=fork();
        if(pid)
            continue;
        
        client ct(first_id);
        ct.clientMain();

        if(!pid)
            break;
    }
    return 0;
}