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
                clientAccept();
                if(clientSocket>0)
                {
                    int pid = fork();
                    if (pid == 0)
                    {
                        if(identity()<0)
                            return 0;

                        int req=0x02;

                        while (!closeFlag)
                        {
                            switch (req)
                            {
                                case PACK_SYS_INFO_REQ_S:
                                {
                                    sysInfoReq();
                                    break;
                                }
                                case PACK_CONF_INFO_REQ_S:
                                {
                                    configReq();
                                    break;
                                }
                                case PACK_PROC_INFO_REQ_S:
                                {
                                    procInfoReq();
                                    break;
                                }
                                case PACK_ETH_PORT_INFO_REQ_S:
                                {
                                    ethInfoReq();
                                    break;
                                }
                                case PACK_FLASH_INFO_REQ_S:
                                {
                                    usbInfoReq();
                                    break;
                                }
                                case PACK_PRINT_PORT_INFO_REQ_S:
                                {
                                    printPortReq();
                                    break;
                                }
                                case PACK_TTY_SER_INFO_REQ_S:
                                {
                                    ttySerInfoReq();
                                    break;
                                }
                                case PACK_DISCON_REQ_S:
                                {
                                    disconReq();
                                    break;
                                }

                                default:
                                    break;
                            }
                            ++req;
                        }

                    }
                    else
                    {
                        close(clientSocket);
                        continue;
                    }
                }
                
            }
        }
        if(closeFlag)
            break;
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
    //serviceAddr.sin_addr.s_addr = inet_addr("0.0.0.0");      //inet_addr("127.0.0.1");
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
    socklen_t len=sizeof(sockaddr_in);
    clientSocket = accept(localSocket, (struct sockaddr*)&clientAddr, &len); //accept非阻塞
    fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);
    if(clientSocket<=0)
    {
        cout<<"accept error"<<endl;
        return 0;
    }
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
        ret = select(clientSocket + 1, NULL, &fdsr, NULL, NULL);
        if (ret < 0 && errno != EINTR)
        {
            continue;
        }
        else
        {
            if (ret > 0)
                break;
        }
    }
    //FD_ISSET(socketfd, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
    int sendSize = 0;
    while(1)
    {
        ret = send(clientSocket, &sendBuf[sendSize], sendBufSize, 0);
        sendSize = sendSize + ret;
        if (ret <= 0)
        {
            cout<<"send error"<<endl;
            continue;
        }
        if (sendSize == sendBufSize)
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
    byte buf[1000]={0};
    packHeadStuff(buf,0x11,0x01,60,0x0000,52);
    *(short *)(buf + 8) = htons(2);
    *(buf+10)=0;
    *(buf+11)=0;
    *(short *)(buf + 12) = htons(rand());
    *(short *)(buf + 14) = htons(rand());
    *(buf+16)=rand()%2;

    u_int random_num=rand();
    u_int svr_time=(u_int)(time(0));
    svr_time=svr_time^(u_int)0xFFFFFFFF;
    int pos = random_num % 4093;
    for (int i = 0; i < 32; ++i, ++pos)
    {
        buf[20+i] = IdsStr[i] ^ secret[pos % 4093];
    }
    
    *(int *)(buf + 52) = htonl(random_num);
    *(int *)(buf + 56) = htonl(svr_time);
    
    string logStr = "发送认证信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    dataSend(buf,60);

    logStr = "发送" + to_string(60) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 60);

    

    memset(buf,0,1000);

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
    

    string cpuMhz=to_string(ntohs(*(short *)(buf+8)));
    sdram=ntohs(*(short *)(buf+10));
    string flash=to_string(ntohs(*(short *)(buf+12)));

    char temp[17]={0};
    memcpy(temp,(char *)(buf+16),16);
    string sid=temp;
    sid=sid+"-"+to_string(ntohs(*(short *)(buf+14)));

    memcpy(temp,(char *)(buf+32),16);
    string devType=temp;

    memcpy(temp,(char *)(buf+48),16);
    string version=temp;

    ethNum=*(buf+64);
    string syn=to_string(*(buf+65));
    string asyn=to_string(*(buf+66));
    string sw=to_string(*(buf+67));
    string usb=to_string(*(buf+68));
    string print=to_string(*(buf+69));
    devid = ntohl(*(int *)(buf+72));
    string devno=to_string(1);

    string table="devstate_base (devstate_base_devid,devstate_base_devno,devstate_base_time,devstate_base_ipaddr,devstate_base_sid,devstate_base_type,devstate_base_version,devstate_base_cpu,devstate_base_sdram,devstate_base_flash,devstate_base_ethnum,devstate_base_syncnum,devstate_base_asyncnum,devstate_base_switchnum,devstate_base_usbnum,devstate_base_prnnum)";
    string opt="\'"+to_string(devid)+"\',"+devno+","+"now()"+",\'"+inet_ntoa(clientAddr.sin_addr)+"\',\'"+sid+"\',\'"+devType+"\',\'"+version+"\',"+cpuMhz+","+to_string(sdram)+","+flash+","+to_string(ethNum)+","+syn+","+asyn+","+sw+","+usb+","+print;

    mysqlInsert(table.c_str(),opt.c_str());

    logStr = "认证成功";
    logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port));
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);


    return 0;
}

int server::sysInfoReq()
{
    string logStr = "发送系统信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    byte buf[28]={0};
    packHeadStuff(buf,0x11,0x02,8,0x0000,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);
    if(packType!=PACK_SYS_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    double cpu_used=double(ntohl(*(int*)(buf+8))+ntohl(*(int*)(buf+16)))/double(ntohl(*(int*)(buf+8))+ntohl(*(int*)(buf+12))+ntohl(*(int*)(buf+16))+ntohl(*(int*)(buf+20)));
    double sdram_used=double(ntohl(*(int*)(buf+24))/1024/1024)/double(sdram);

    logStr="CPU使用率："+to_string(cpu_used);
    cout<<logStr<<endl;
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    logStr="RAM使用率："+to_string(sdram_used);
    cout<<logStr<<endl;
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    char val[500];
    sprintf(val,"devstate_base_cpu_used = %f,devstate_base_sdram_used = %f ",cpu_used,sdram_used);

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);

    return 0;
}

int server::configReq()
{
    string logStr = "发送配置信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    byte buf[10000]={0};
    packHeadStuff(buf,0x11,0x03,8,0x0000,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);
    if(packType!=PACK_CONF_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    char val[10000]={0};
    sprintf(val,"devstate_base_config = \' %s \'",buf+8);

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);

    return 0;
}

int server::procInfoReq()
{
    string logStr = "发送进程信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    byte buf[10000]={0};
    packHeadStuff(buf,0x11,0x04,8,0x0000,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);
    if(packType!=PACK_PROC_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    char val[10000];
    sprintf(val,"devstate_base_process = \' %s \'",buf+8);

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);

    return 0;
}

int server::ethInfoReq()
{
    if(!ethNum)
        return 0;
    string logStr = "发送网口状态信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    byte buf[500]={0};
    packHeadStuff(buf,0x11,0x05,8,0x0000,0);
    dataSend(buf,8);

    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);
    if(packType!=PACK_ETH_PORT_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    char val[2000]={0};
    sprintf(val,"devstate_base_eth%d_ip = \' %s \',",*(short *)(buf+4),INTtoIP(*(u_int*)(buf+20)).c_str());
    sprintf(val,"devstate_base_eth%d_mac = \' %x-%x-%x-%x-%x-%x \',",*(short *)(buf+4),*(buf+12),*(buf+13),*(buf+14),*(buf+15),*(buf+16),*(buf+17));
    sprintf(val+strlen(val),"devstate_base_eth%d_mask = \' %s \',",*(short *)(buf+4),INTtoIP(*(u_int*)(buf+24)).c_str());
    sprintf(val+strlen(val),"devstate_base_eth%d_state = \' %s \',",*(short *)(buf+4),*(buf+10)?"up":"down");
    sprintf(val+strlen(val),"devstate_base_eth%d_speed =  \' %s \' ,",*(short *)(buf+4),*((short *)(buf+22))&0x0001?"100":"10");
    sprintf(val+strlen(val),"devstate_base_eth%d_duplex =  \' %s \' ,",*(short *)(buf+4),*((short *)(buf+22))&0x0002?"全双工":"半双工");
    sprintf(val+strlen(val),"devstate_base_eth%d_autonego =  \' %s \' ,",*(short *)(buf+4),*((short *)(buf+22))&0x0004?"是":"否");
    sprintf(val+strlen(val),"devstate_base_eth%d_txbytes =  %d ,",*(short *)(buf+4),*(int *)(buf+100));
    sprintf(val+strlen(val),"devstate_base_eth%d_txpackets = %d ,",*(short *)(buf+4),*(int *)(buf+104));
    sprintf(val+strlen(val),"devstate_base_eth%d_rxbytes =  %d ,",*(short *)(buf+4),*(int *)(buf+68));
    sprintf(val+strlen(val),"devstate_base_eth%d_rxpackets =  %d ",*(short *)(buf+4),*(int *)(buf+72));


    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);
    
    return 0;
}

int server::usbInfoReq()
{
    string logStr = "发送U盘状态信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    byte buf[100]={0};
    packHeadStuff(buf,0x11,0x07,8,0x0000,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);

    if(packType!=PACK_FLASH_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }


    char val[100];
    sprintf(val,"devstate_base_usbstate = \' %s \'",*(buf+8)?"已插入":"未插入");

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);


    if(*(buf+8))
    {
        usbFileInfoReq();
    }

    return 0;
}

int server::usbFileInfoReq()
{
    string logStr = "发送U盘文件信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    byte buf[6000]={0};
    packHeadStuff(buf,0x11,0x0c,8,0x0000,0);

    dataSend(buf,8);

    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);

    if(packType!=PACK_PRINT_PORT_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    char val[6000];
    sprintf(val,"devstate_base_usbfiles = \' %s \'",(buf+8));

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);

    return 0;
}

int server::printPortReq()
{
    string logStr = "发送打印口信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    byte buf[100]={0};
    packHeadStuff(buf,0x11,0x08,8,0x0000,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);

    if(packType!=PACK_PRINT_PORT_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }


    char val[200];
    sprintf(val,"devstate_base_prnname = \' %s \',devstate_base_prnstate = \' %s \'",buf+12,*(buf+8)?"已启动":"未启动");

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);

    if(*(buf+8))
    {
        printQueReq();
    }

    return 0;
}

int server::printQueReq()
{
    string logStr = "发送打印队列信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    byte buf[100]={0};
    packHeadStuff(buf,0x11,0x08,8,0x0000,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);

    if(packType!=PACK_PRINT_PORT_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }


    char val[200];
    sprintf(val,"devstate_base_prnfiles = \' %s \'",buf+8);

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);

    return 0;
}

int server::ttySerInfoReq()
{
    string logStr = "发送终端服务器信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    byte buf[100]={0};
    packHeadStuff(buf,0x11,0x09,8,0x0000,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);

    if(packType!=PACK_TTY_SER_INFO_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    char val[200];
    sprintf(val,"devstate_base_tty_configed =  %d ",ntohs(*(u_short *)(buf+278)));

    char opt[100];
    sprintf(opt,"devstate_base.devstate_base_devid=\'%d\' ",devid);
    mysqlUpdate("devstate_base",val,opt);

    

    byte dev_used[16]={0};
    byte ip_dev_used[254]={0};

    dev_used[0]=0;
    ip_dev_used[0]=0;

    for(int i=0;i<16;++i)
    {
        if(*(buf+8+i))
        {
            
            dev_used[i]=1;
            ttyDevInfoReq(i,0x0a);
        }
        
    }

    for(int i=0;i<254;++i)
    {
        if(*(buf+24+i))
        {
            
            ip_dev_used[i]=1;
            ttyDevInfoReq(i,0x0b);
        }
    }

    con++;

    return 0;
}

int server::ttyDevInfoReq(short ttyno,short tp)
{
    string logStr = "发送终端配置和虚屏信息请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    byte buf[10000]={0};
    packHeadStuff(buf,0x11,tp,8,ttyno+1,0);

    dataSend(buf,8);
    
    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    packAnalysis(buf,transtv);

    if(buf[1]!=tp)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    string tableName="devstate_ttyinfo(devstate_ttyinfo_devid,devstate_ttyinfo_devno,devstate_ttyinfo_ttyno,devstate_ttyinfo_time,devstate_ttyinfo_readno,devstate_ttyinfo_type,devstate_ttyinfo_state,devstate_ttyinfo_ttyip,devstate_ttyinfo_scrnum)";
    string opt="\'";
    opt=opt +to_string(devid)+"\',"+to_string(1)+","+to_string(ttyno+tp==0x0a?900:0)+","+"now()"+","+to_string(*(buf+9))+",\'"+(char*)(buf+16)+"\',\'"+(char*)(buf+28)+"\',\'"+(tp==0x0a?"0":INTtoIP(ntohl(*(int *)(buf+16))).c_str())+"\',"+to_string(*(buf+11));
    mysqlInsert(tableName.c_str(), opt.c_str());

    byte *screen=buf+36;
    for(int i=0;i<*(buf+11);++i,screen=screen+96)
    {
        string tableName="devstate_scrinfo(devstate_scrinfo_devid,devstate_scrinfo_devno,devstate_scrinfo_ttyno,devstate_scrinfo_scrno,devstate_scrinfo_time,devstate_scrinfo_is_current,devstate_scrinfo_protocol,devstate_scrinfo_serverip,devstate_scrinfo_serverport,devstate_scrinfo_state,devstate_scrinfo_ttytype,devstate_scrinfo_tx_server,devstate_scrinfo_rx_server,devstate_scrinfo_tx_terminal,devstate_scrinfo_rx_terminal,devstate_scrinfo_ping_min,devstate_scrinfo_ping_avg,devstate_scrinfo_ping_max)";
        string opt="\'";
        opt=opt+to_string(devid)+"\',\'"+to_string(1)+"\',"+to_string(ttyno+tp==0x0a?900:0)+","+to_string(i)+","+"now()"+",\'"+((i==*(buf+10)+1)?"*":"NULL")+"\',\'"+(char*)(screen+8)+"\',\'"+INTtoIP(ntohl(*(int *)(screen+4))).c_str()+"\',"+to_string(ntohs(*(short *)(screen+2)))+",\'"+(char*)(screen+20)+"\',\'"+(char*)(screen+52)+"\',"+to_string(*(u_int*)(screen+76))+","+to_string(ntohl(*(u_int*)(screen+80)))+","+to_string(ntohl(*(u_int*)(screen+68)))+","+to_string(ntohl(*(u_int*)(screen+72)))+","+to_string(double(ntohl(*(u_int*)(screen+84)))/10)+","+to_string(double(ntohl(*(u_int*)(screen+88)))/10)+","+to_string(double(*(u_int*)(screen+92))/10);
        mysqlInsert(tableName.c_str(), opt.c_str());
    }

    return 0;
}



int server::disconReq()
{
    string logStr = "发送断开连接请求";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);

    byte buf[8]={0};
    packHeadStuff(buf,0x11,0xff,8,0x0000,0);

    dataSend(buf,8);

    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
    
    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, devid,1, logStr, buf, 8);

    int packType = packAnalysis(buf,transtv);

    if(packType!=PACK_DISCON_ANS_C)
    {
        string logStr = "接收失败，包类型错误:";
        logStr=logStr+ inet_ntoa(clientAddr.sin_addr) +":"+ to_string(ntohs(clientAddr.sin_port))+"  连接关闭";
        logWrite(LocalLogPath, devid,0, logStr, nullptr, 0);
        closeFlag=1;
        return -1;
    }

    closeFlag=2;

    return 0;
}