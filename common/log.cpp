#include "common.h"

using namespace std;

int logWrite(string logPath, int mode, string opt, const byte data[], int dataLength)
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

    time_t t = time(nullptr);
    struct tm *tmTime = localtime(&t);

    strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S ", tmTime);
    string buf = temp;
    buf = buf + " [" + to_string(getpid()) + "]" + opt + "\n";

    switch (mode)
    {
        case 1:
        {
            int it = 0;
            for (int i = 0; i <= dataLength / 16; ++i)
            {
                string line="", str = "";
                sprintf(temp, "  %04x: ", i * 16);
                line = line + temp;

                int j = 0;
                for (; j < 8; ++j, ++it)
                {
                    if (it < dataLength)
                    {
                        sprintf(temp, " %02x", data[it]);
                        line = line + temp;

                        if (data[it] > ' ' && data[it] < 127)
                        {
                            str = str + data[it];
                        }
                        else
                        {
                            str = str + ".";
                        }
                    }
                    else
                    {
                        str=str+" ";
                        line = line + "   ";
                    }
                }

                if (it < dataLength)
                {
                    line = line + " -";
                }
                else
                {
                    line = line + "  ";
                }

                for (; j < 16; ++j, ++it)
                {
                    if (it < dataLength)
                    {
                        sprintf(temp, " %02x", data[it]);
                        line = line + temp;

                        if (data[it] > ' ' && data[it] < 127)
                        {
                            str = str + data[it];
                        }
                        else
                        {
                            str = str + ".";
                        }
                    }
                    else
                    {
                        str=str+" ";
                        line = line + "   ";
                    }
                }
                line = line + "  " + str + "\n";
                buf = buf + line;
            }
            break;
        }
        default:
            break;
    }

    write(fd, buf.c_str(), buf.size());

    flock(fd,LOCK_UN);
    return 0;
}