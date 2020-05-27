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
        localBind();
        
        int packType = packAnalysis(buf);
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
            else
            {
                idsAns();
            }
        }
        else
        {
            closeFlag = 1;
        }
        

        while (!closeFlag)
        {
            memset(buf,0,1000);
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

        /*if (closeFlag)
        {
            close(socketfd);
            if (closeFlag == 1)
                sleep(fail_reConnnectSec);
            else
                sleep(succ_reConnnectSec);
            continue;
        }*/
        break;
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
        ret = connect(this->socketfd, (struct sockaddr *)&serviceAddr, sizeof(serviceAddr)); //�ͻ�����connect�������������
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
    string logStr="������������";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);
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
    //FD_ISSET(socketfd, &fdsr)�ж��׽����Ƿ��������������һ�������������Թ�
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
    //FD_ISSET(socketfd, &fdsr)�ж��׽����Ƿ��������������һ�������������Թ�
	while (1)
	{
		ret = recv(socketfd, recvBuf, recvSize, 0);
        if(ret==0)
        {
            closeFlag = 1;
            string logStr="�Է��ر�������";
            logWrite(LocalLogPath, 0, logStr, nullptr, 0);
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
    if(closeFlag==1)
        return 0;
    int dataLength = ntohs(*(short *)(buf + 2));
    dataRecv(buf + 8, dataLength - 8);
    string logStr="��ȡ"+to_string(dataLength)+"�ֽ�";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr="(��ȡ����Ϊ:)";
    logWrite(LocalLogPath, 1, logStr, buf, dataLength);

    switch ((u_char)buf[0])
    {
        case 0x11://��������
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
    string logStr="��ʼ��֤";
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    short ver1 = ntohs(*(short *)(buf + 8));
    u_char ver2 = *(buf + 10);
    u_char ver3 = *(buf + 11);

    logStr="�汾��"+to_string(ver1)+"."+to_string(ver2)+"."+to_string(ver3);
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    if (ver1 < 2)
    {
        logStr="�汾����";
        cout << logStr << endl;
        logWrite(LocalLogPath, 0, logStr, nullptr, 0);
        closeFlag = 1;
        return -1;
    }

    time_t svr_time = (time_t)ntohl((*(u_int *)(buf + 56))^ (u_int)(0xFFFFFFFF)) ;
    struct tm *st;
    st = localtime(&svr_time);
    

    logStr="������ʱ��:"+to_string(st->tm_year + 1900)+"-"+to_string(st->tm_mon)+"-"+to_string(st->tm_mday)+" "+to_string(st->tm_hour)+":"+to_string(st->tm_min)+":"+to_string(st->tm_sec);
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    if(st->tm_year + 1900 < 2017)
    {
        logStr="����֤�����";
        cout << logStr << endl;
        logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
        closeFlag = 1;
        return -1;
    }

    char encryptedStr[32];
    u_int random_num = ntohl(*(u_int *)(buf + 52));
    memcpy(encryptedStr, buf + 20, 32);
    int pos = random_num % 4093;
    for (int i = 0; i < 32; ++i, ++pos)
    {
        encryptedStr[i] = encryptedStr[i] ^ secret[pos % 4093];
    }
    string r_s = encryptedStr;
    if(r_s==IdsStr)
    {
        logStr="��֤�ɹ�";
        cout<<logStr<<endl;
        logWrite(LocalLogPath, 0, "��֤�ɹ�", nullptr, 0);
        return 0;
    }
    else
    {
        logStr="��֤�Ƿ�";
        cout << logStr << endl;
        logWrite(LocalLogPath, 0, "��֤�Ƿ�", nullptr, 0);
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

    string logStr = "����" + to_string(12) + "�ֽ�";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "(��������Ϊ:)";
    logWrite(LocalLogPath, 1, logStr, buf, 12);
    return 0;
}

int client::idsAns()
{
    byte buf[116] = {0};
    packHeadStuff(buf,0x91,0x01,116,0x0000,108);

    short cpuMhz=htons((short)getCPUMHz());//CPU��Ƶ
    memcpy(buf+8,&cpuMhz,sizeof(short));

    short ram=htons((short)getRAM());//RAM��С
    memcpy(buf+10,&ram,sizeof(short));

    *(short *)(buf+12)=htons((short)(rand()));//FLASH��С

    *(short *)(buf+14)=htons((short)(1));//�ڲ����к�

    memcpy((buf+16),randRam(16).c_str(),16);//�����к�

    memcpy((buf+32),randRam(16).c_str(),16);//�豸�ͺ�

    memcpy((buf+48),randRam(16).c_str(),16);//����汾��

    *(buf+64)=rand()%3;//��̫������

    *(buf+65)=rand()%3;//ͬ��������

    *(buf+66)=rand()%3*8;//�첽������

    *(buf+67)=rand()%4*8;//����������

    *(buf+68)=rand()%2;//USB������

    *(buf+69)=rand()%2;//��ӡ������

    *(int *)(buf+72)=htonl(devid);//������

    *(buf+76)=1;

    u_int random_num=(u_int)rand(); 
    int pos=random_num%4093;
    for(int i=0;i<32;++i,++pos)
    {
        buf[80+i]=IdsStr[i]^secret[pos % 4093];
    }
    *(u_int *)(buf+112)=htonl(random_num);

    dataSend(buf,116);

    string logStr = "����" + to_string(116) + "�ֽ�";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(��������Ϊ��)";
    logWrite(LocalLogPath, 1, logStr, buf, 116);

    return 0;
}