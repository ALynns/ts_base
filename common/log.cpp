#include "common.h"

using namespace std;

int logWrite(string logPath, int mode, string opt, string data)
{
    int fd;
    char temp[100] = {0};
    fd = open(logPath.c_str(), O_RDWR | O_APPEND | O_CREAT, 0777);
    if(fd < 0)
	{   	
		printf("Open file error\n");
        return -1;
    }
    flock(fd, LOCK_EX);
    if (mode)
    {
        write(fd, buf, strlen(buf));
    }
    else
    {
        time_t t = time(nullptr);
        struct tm *tmTime = localtime(&t);

        strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S ", tmTime);
        string buf = temp;
        buf = buf + " [" + to_string(getpid()) + "]" + opt + "\n";

        switch (mode)
        {
            case 1:
            {
                string::iterator it = data.begin();
                string hexStr = "", str = "";

                for (int i = 0; i <= data.size() / 8; ++i)
                {
                    int j = 0;
                    for (; j < 8; ++j)
                    {
                        if(it<data.end())
                        {
                            if (*it > ' ' && *it < 127)
                            {
                                str = str + *it;
                                sprintf(buf,"0xd",)
                            }
                            else
                            {
                                str = str + ".";
                            }
                        }
                        else
                        {
                            
                        }
                        
                            
                    }

                    for (; j < 16; ++j)
                    {

                    }
                }
                break;
            }
            default:
                break;
        }

        write(fd, buf.c_str(), buf.size());
    }
    
    flock(fd,LOCK_UN);
}