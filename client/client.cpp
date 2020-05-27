#include "client.h"

using namespace std;

extern u_char secret[4096];
extern map<short, int> ServerPackType;
extern map<short, int> ClientPackType;

int client::clientMain()
{
    byte buf[1000];
    while(1)
    {
        short fail_reConnnectSec = 0, succ_reConnnectSec = 0;
        closeFlag = 0;
        this->localBind();
        while (!closeFlag)
        {
            int packType = this->packAnalysis(buf);
            if (packType == PACK_IDS_REQ_S)
            {
                int ret;
                fail_reConnnectSec = ntohs(*(short *)(buf + 12));
                succ_reConnnectSec = ntohs(*(short *)(buf + 14));
                ret = identity(buf);
                if (ret == -1)
                {
                    closeFlag = 1;
                }
                break;
            }
            else
            {
                closeFlag = 1;
                break;
            }
        }

        while (!closeFlag)
        {
            int packType = this->packAnalysis(buf);
            switch (packType)
            {
            case PACK_SYS_INFO_REQ_S:
            {
                break;
            }
            case PACK_CONF_INFO_REQ_S:
            {
                break;
            }
            case PACK_PROC_INFO_REQ_S:
            {
                break;
            }
            case PACK_ETH_PORT_INFO_REQ_S:
            {
                break;
            }
            case PACK_FLASH_INFO_REQ_S:
            {
                break;
            }
            case PACK_PRINT_PORT_INFO_REQ_S:
            {
                break;
            }
            case PACK_TTY_SER_INFO_REQ_S:
            {
                break;
            }
            case PACK_TTY_INFO_REQ_S:
            {
                break;
            }
            case PACK_IP_TTY_INFO_REQ_S:
            {
                break;
            }
            case PACK_FLASH_FILE_INFO_REQ_S:
            {
                break;
            }
            case PACK_PRINT_QUE_INFO_REQ_S:
            {
                break;
            }
            case PACK_DISCON_REQ_S:
            {
                break;
            }

            default:
                break;
            }
        }

        if (closeFlag)
        {
            close(socketfd);
            if (closeFlag == 1)
                sleep(fail_reConnnectSec);
            else
                sleep(succ_reConnnectSec);
            continue;
        }
    }
    return 0;
}

int client::localBind()
{
    this->socketfd=0;
    while(this->socketfd<=0)
        this->socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    fcntl(this->socketfd, F_SETFL, fcntl(this->socketfd, F_GETFL, 0) | O_NONBLOCK);
    fd_set fdsr;

    struct sockaddr_in serviceAddr;
    serviceAddr.sin_family=AF_INET;
    serviceAddr.sin_addr.s_addr=inet_addr(this->serverIPAddr.c_str());
    serviceAddr.sin_port=htons(this->port);

    struct sockaddr_in clientAddr;
    memset(&clientAddr,0,sizeof(clientAddr));
    clientAddr.sin_family=AF_INET;
    clientAddr.sin_port=htons(0);

    int reuse = 1;
    setsockopt(this->socketfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(int));

    int ret = -1;
    while (ret < 0)
        ret = bind(this->socketfd, (struct sockaddr *)&clientAddr, sizeof(clientAddr));

    while(1)
    {
        ret = connect(this->socketfd, (struct sockaddr *)&serviceAddr, sizeof(serviceAddr)); //客户端用connect与服务器端连接
        if (ret == -1)
        {
            if (errno != EINPROGRESS) //
                continue;
            else
            {
                FD_ZERO(&fdsr);
                FD_SET(this->socketfd, &fdsr);

                ret = select(this->socketfd + 1, NULL, &fdsr, NULL, 0);
                if (ret < 0)
                {
                    printf("select error\n");
                    exit(-1);
                }
                else
                {
                    int error = -1, slen = sizeof(int);
                    getsockopt(this->socketfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&slen);
                    if (error == 0)
                    {
                        //printf("Connection successful\n");
                        ret = 1;
                    }
                    
                    else
                        ret = 0;
                }
            }
        }
        if (ret == 0)
            continue;
        else
            break;
    }

    return 0;
}

void client::dataSend(const byte *sendBuf, int sendBufSize)
{
    int ret;

    fd_set fdsr;

    //struct timeval tv;
    //tv.tv_sec = 1;
    while (1)
    {
        FD_ZERO(&fdsr);
        FD_SET(socketfd, &fdsr);
        ret = select(socketfd + 1, NULL, &fdsr, NULL, 0);
        if (ret < 0 && errno != EINTR)
        {
            continue;
        }
        else
        {
            if (ret > 0)
                break;
            if (errno == EINTR)
                continue;
        }
    }
    //FD_ISSET(socketfd, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
    ret=0;
	while (1)
	{
		ret = send(socketfd, &sendBuf[ret],sendBufSize, 0);
        if(ret>0)
            sendBufSize=sendBufSize-ret;
		if(sendBufSize==0)
            break;
	}
}

void client::dataRecv(byte *recvBuf,int recvSize)
{
    int ret;

    fd_set fdsr;

    //struct timeval tv;
    //tv.tv_sec = 1;
    while (1)
    {
        FD_ZERO(&fdsr);
        FD_SET(socketfd, &fdsr);
        ret = select(socketfd + 1, &fdsr, NULL, NULL, 0);
        if (ret < 0 && errno != EINTR)
        {
            printf("select error,%d\n", ret);
            exit(-1);
        }
        else
        {
            if (ret > 0)
                break;
            if (errno == EINTR)
                continue;
            else
                break;
        }

    }
    //FD_ISSET(socketfd, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
	while (1)
	{
		ret = recv(socketfd, recvBuf, recvSize, 0);
        if(ret==0)
        {
            closeFlag = 1;
            return;
        }    

        if (ret < 0)
			continue;
        else
        {
            recvSize = recvSize - ret;
            if(!recvSize)
                break;
        }
    }


}

int client::packAnalysis(byte buf[])
{
    dataRecv(buf,8);
    int dataLength = ntohs(*(short *)(buf + 2));
    dataRecv(buf + 8, dataLength - 8);
    string logStr="读取"+to_string(dataLength)+"字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr="(读取数据为:)\n";
    logWrite(LocalLogPath, 1, logStr, buf, dataLength);

    switch ((u_char)buf[0])
    {
        case 0x11://服务器包
        {
            return ServerPackType[buf[1]] ;
            break;
        }
        case 0x91:
        {
            break;
        }

        default:
            break;
    }
    return -1;
}

int client::identity(byte buf[])
{
    string logStr="开始认证";
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    short ver1 = ntohs(*(short *)(buf + 8));
    u_char ver2 = *(buf + 10);
    u_char ver3 = *(buf + 11);

    logStr="版本号"+to_string(ver1)+"."+to_string(ver1)+"."+to_string(ver1);
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    if (ver1 < 2)
    {
        logStr="版本过低";
        cout << logStr << endl;
        logWrite(LocalLogPath, 0, logStr, nullptr, 0);
        closeFlag = 1;
        return -1;
    }

    time_t svr_time = (time_t)ntohs(*(int *)(buf + 56)) ^ (u_int)(0xFFFFFFFF);
    struct tm *st;
    st = localtime(&svr_time);

    logStr="服务器时间:"+to_string(st->tm_year + 1900)+"-"+to_string(st->tm_mon)+"-"+to_string(st->tm_mday)+" "+to_string(st->tm_hour)+"."+to_string(st->tm_min)+"."+to_string(st->tm_sec);
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    if(st->tm_year + 1900 < 2017)
    {
        logStr="数字证书过期";
        cout << logStr << endl;
        logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
        closeFlag = 1;
        return -1;
    }

    char encryptedStr[32];
    u_int random_num = ntohs(*(int *)(buf + 52));
    memcpy(encryptedStr, buf + 20, 32);
    int pos = random_num % 4093;
    for (int i = 0; i < 32; ++i, ++pos)
    {
        encryptedStr[i] = encryptedStr[i] ^ secret[pos % 4093];
    }
    string r_s = encryptedStr;
    if(r_s==IdsStr)
    {
        logWrite(LocalLogPath, 0, "认证成功", nullptr, 0);
        return 0;
    }
    else
    {
        cout << "认证非法" << endl;
        logWrite(LocalLogPath, 0, "认证非法", nullptr, 0);
        closeFlag = 1;
        return -1;
    }
    
    return 0;
}

int client::minimumVerReq()
{
    byte buf[12] = {0};
    packHeadStuff(buf,0x91,0x00,12,0x0000,4);
    *(short *)(buf + 8) = 2;
    dataSend(buf, 12);

    string logStr = "发送" + to_string(12) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "(发送数据为:)\n";
    logWrite(LocalLogPath, 1, logStr, buf, 12);
    return 0;
}

int client::idsAns()
{
    /*byte buf[116] = {0};
    packHeadStuff(buf,0x91,0x01,116,0x0000,108);
    FILE *fp = popen("cat /proc/cpuinfo | grep cpu |grep MHz","r");
	byte buff[1000];
	fgets(buff,1000,fp);
	printf("%s",buff);
	pclose(fp);
    return 0;*/
    return 0;
}