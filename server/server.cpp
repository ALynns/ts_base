#include "server.h"

using namespace std;


extern u_char secret[4096];
extern map<short, int> ServerPackType;
extern map<short, int> ClientPackType;


int server::serverMain()
{
    mysqlInit();
    hostBind();

    int ret;

    fd_set fdsr;
    

    while(1)
    {
        waitpid(0, NULL, WNOHANG);
        FD_ZERO(&fdsr);
        FD_SET(localSocket, &fdsr);
        ret = select(localSocket + 1, &fdsr, NULL, NULL, NULL);

        if (ret > 0)
        {
            if (FD_ISSET(localSocket, &fdsr) > 0)
            {
                int pid = fork();
                
                if (pid == 0)
                {
                    clientAccept();
                    identity();

                    while (!closeFlag)
                    {
                        /*switch (packType)
                        {
                            case PACK_VER_INFO_C:
                            {
                                cout<<"收到对方最低版本要求信息"<<endl;
                                sysInfoAns();
                                break;
                            }
                            case PACK_SYS_INFO_ANS_C:
                            {
                                cout<<"收到对方系统信息"<<endl;
                                configAns();
                                break;
                            }
                            case PACK_CONF_INFO_ANS_C:
                            {
                                cout<<"收到对方配置信息"<<endl;
                                procInfoAns();
                                break;
                            }
                            case PACK_PROC_INFO_ANS_C:
                            {
                                cout<<"收到对方进程信息"<<endl;
                                ethInfoAns(*((short*)(buf+4)));
                                break;
                            }
                            case PACK_ETH_PORT_INFO_ANS_C:
                            {
                                cout<<"收到对方网口信息"<<endl;
                                usbInfoAns();
                                break;
                            }
                            case PACK_FLASH_INFO_ANS_C:
                            {
                                cout<<"收到对方U盘信息"<<endl;
                                printPortAns();
                                break;
                            }
                            case PACK_PRINT_PORT_INFO_ANS_C:
                            {
                                cout<<"收到对方打印口信息"<<endl;
                                ttySerInfoAns();
                                break;
                            }
                            case PACK_TTY_SER_INFO_ANS_C:
                            {
                                cout<<"收到对方终端服务器信息"<<endl;
                                ttyInfoAns(0x0a,ntohs(*(short *)(buf+4)));
                                break;
                            }
                            case PACK_TTY_INFO_ANS_C:
                            {
                                cout<<"收到对方哑终端信息"<<endl;
                                ttyInfoAns(0x0b,ntohs(*(short *)(buf+4)));
                                break;
                            }
                            case PACK_IP_TTY_INFO_ANS_C:
                            {
                                cout<<"收到对方IP终端信息"<<endl;
                                usbFileInfoAns();
                                break;
                            }
                            case PACK_FLASH_FILE_INFO_ANS_C:
                            {
                                cout<<"收到对方U盘文件信息"<<endl;
                                printQueAns();
                                break;
                            }
                            case PACK_PRINT_QUE_INFO_ANS_C:
                            {
                                cout<<"收到对方打印队列信息"<<endl;
                                disconAns();
                                break;
                            }
                            case PACK_DISCON_ANS_C:
                            {
                                cout<<"收到对方中断连接信息"<<endl;
                                disconAns();
                                break;
                            }

                            default:
                                break;
                        }*/
                    }

                }
                else
                {
                    continue;
                }
                
            }
        }
    }

    return 0;


    return 0;
}




int server::hostBind()
{
    struct sockaddr_in serviceAddr;
    int ret;
    memset(&serviceAddr,0,sizeof(serviceAddr));//由于该结构体中存在保留位，需要置0
    serviceAddr.sin_family=AF_INET;//定义地址族类型，AF_INET为IPv4，AF_INET6为IPv6
    serviceAddr.sin_port = htons(listenPort);
    //if(IPAddr!=NULL)
        //serviceAddr.sin_addr.s_addr = inet_addr(IPAddr);      //inet_addr("127.0.0.1");
    localSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Linux中定义socket为一个文件，返回值为一个文件描述符
    if(localSocket<0)
    {
        printf("socket error\n");
        exit(-1);
    }
    
    fcntl(localSocket, F_SETFL, fcntl(localSocket, F_GETFL, 0) | O_NONBLOCK);

    int reuse=1;
    setsockopt(localSocket,SOL_SOCKET ,SO_REUSEADDR,(const char*)& reuse,sizeof(int));
    
    ret=bind(localSocket, (struct sockaddr *)&serviceAddr, sizeof(serviceAddr));//绑定端口
    if (ret < 0)
    {
        printf("bind error\n");
        exit(-1);
    }
    ret = listen(localSocket, SOMAXCONN);
    if (ret < 0) //listen()函数使套接字进入被动监听状态，SOMAXCONN为缓冲区长度最大
    {
        printf("listen error\n");
        exit(-1);
    }
    return localSocket;
}

int server::clientAccept()
{
    clientSocket = accept(localSocket, (struct sockaddr *)&clientAddr, NULL); //accept非阻塞
    fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);
    string logStr = "收到未认证客户端连接请求: ";
    logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port));
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    closeFlag=0;
    return 0;
}

int server::packAnalysis(byte *buf,struct timeval tv )
{
    dataRecv(buf,8,tv);
    if(closeFlag)
    {
        string logStr;
        if(closeFlag==1)
        {
            logStr="对方关闭了连接";
        }
        if(closeFlag==3)
        {
            logStr="超时，连接关闭";
        }
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        return -1;
    }
    int dataLength = ntohs(*(short *)(buf + 2));
    dataRecv(buf + 8, dataLength - 8,tv);
    string logStr="读取"+to_string(dataLength)+"字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    logStr="(读取数据为:)";
    logWrite(LocalLogPath, devid,1, logStr, buf, dataLength);

    switch ((u_char)buf[0])
    {
        case 0x11://服务器包
        {
            return ServerPackType[buf[1]] ;
            break;
        }
        case 0x91:
        {
            return ClientPackType[buf[1]] ;
            break;
        }

        default:
            break;
    }
    return -1;
}

void server::dataSend(const byte *sendBuf, int sendBufSize)
{
    int ret;

    fd_set fdsr;

    //struct timeval tv;
    //tv.tv_sec = 1;
    while (1)
    {
        FD_ZERO(&fdsr);
        FD_SET(clientSocket, &fdsr);
        ret = select(clientSocket + 1, NULL, &fdsr, NULL, 0);
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
    //FD_ISSET(clientSocket, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
    ret=0;
	while (1)
	{
		ret = send(clientSocket, &sendBuf[ret],sendBufSize, 0);
        if(ret>0)
            sendBufSize=sendBufSize-ret;
		if(sendBufSize==0)
            break;
	}
}

void server::dataRecv(byte *recvBuf,int recvSize,struct timeval tv)
{
    if(!recvSize)
        return ;
    int ret;

    fd_set fdsr;

    //struct timeval tv;
    //tv.tv_sec = 1;
    while (1)
    {
        FD_ZERO(&fdsr);
        FD_SET(clientSocket, &fdsr);
        ret = select(clientSocket + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0 && errno != EINTR)
        {
            printf("select error,%d\n", ret);
            exit(-1);
        }
        else
        {
            if(ret==0)
            {
                closeFlag = 3;
                return;
            }
            if (ret > 0)
                break;
            if (errno == EINTR)
                continue;
            else
                break;
        }

    }
    //FD_ISSET(clientSocket, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
	while (1)
	{
		ret = recv(clientSocket, recvBuf, recvSize, 0);
        if(ret==0)
        {
            closeFlag = 1;
            string logStr="对方关闭了连接";
            logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
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

int server::identity()
{
    byte buf[60]={0};

    packHeadStuff(buf,0x11,0x01,60,0x0000,52);
    *(short *)(buf + 8) = htons(2);
    *(buf+10)=0;
    *(buf+11)=0;
    *(short *)(buf + 12) = htons(rand());
    *(short *)(buf + 14) = htons(rand());
    *(buf+16)=rand()%2;

    u_int random_num=rand(),svr_time=(((u_int)(time(0))^(u_int)0xFFFFFFFF));
    int pos = random_num % 4093;
    for (int i = 0; i < 32; ++i, ++pos)
    {
        buf[20+i] = IdsStr[i] ^ secret[pos % 4093];
    }

    *(int *)(buf + 52) = htons(random_num);
    *(int *)(buf + 56) = htons(svr_time);

    string logStr = "发送认证信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    memset(buf,0,60);

    int packType = packAnalysis(buf,unAcktv);
    if(packType!=PACK_IDS_ANS_C)
    {
        string logStr = "认证失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        return -1;
    }

    random_num=ntohl(*(u_int *)(buf+112)); 
    pos = random_num % 4093;
    for(int i=0;i<104;++i,++pos)
    {
        buf[8+i]=buf[8+i]^secret[pos % 4093];
    }
    
    if(strncmp((char *)(buf+80),IdsStr.c_str(),32))
    {
        string logStr = "认证失败，认证串错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        return -1;
    }

    string cpuMhz=to_string(ntohs(*(buf+8)));
    string ram=to_string(ntohs(*(buf+10)));
    string flash=to_string(ntohs(*(buf+12)));

    string sid;
    sid.assign((char *)(buf),16,16);
    sid=sid+"-"+to_string(ntohs(*(short *)(buf+14)));

    string devType;
    devType.assign((char *)(buf),32,16);

    string version;
    version.assign((char *)(buf),48,16);

    string eth=to_string(*(buf+64));
    string syn=to_string(*(buf+65));
    string asyn=to_string(*(buf+66));
    string sw=to_string(*(buf+67));
    string usb=to_string(*(buf+68));
    string print=to_string(*(buf+69));
    devid = ntohl(*(int *)(buf+72));
    string devno=to_string(1);

    string table="devstate_base (devstate_base_devid,devstate_base_devno,devstate_base_time,devstate_base_ipaddr,devstate_base_sid,devstate_base_type,devstate_base_version,devstate_base_cpu,devstate_base_sdram,devstate_base_flash,devstate_base_ethnum,devstate_base_syncnum,devstate_base_asyncunm,devstate_base_swithcnum,devstate_base_usbnum,devstate_base_prnnum)";
    string opt="("+to_string(devid)+','+devno+','+"now()"+','+devno+','+devno+','+inet_ntoa(clientAddr.sin_addr)+','+sid+','+devType+','+version+','+cpuMhz+','+ram+','+flash+','+eth+','+syn+','+asyn+','+sw+','+usb+','+print+')';
    cout << table<<endl<<opt<<endl;

    //mysqlInsert(table,opt);




    return 0;
}