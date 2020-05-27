#include "../common/common.h"

using namespace std;

int main()
{
    FILE *fp = popen("cat /proc/cpuinfo | grep cpu |grep MHz","r");
	char buff[1000];
	fgets(buff,1000,fp);
	printf("%s",buff);
	pclose(fp);

    return 0;
}