#include "client.h"

using namespace std;

extern u_char secret[4096];
extern map<short, int> ServerPackType;
extern map<short, int> ClientPackType;

int client::clientMain()
{
    if(logOpt)
    {
        remove("*.log");
    }
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
            int packType = packAnalysis(buf);
            switch (packType)
            {
                case PACK_SYS_INFO_REQ_S:
                {
                    cout<<"收到对方获取系统信息请求"<<endl;
                    sysInfoAns();
                    break;
                }
                case PACK_CONF_INFO_REQ_S:
                {
                    cout<<"收到对方获取配置请求"<<endl;
                    configAns();
                    break;
                }
                case PACK_PROC_INFO_REQ_S:
                {
                    cout<<"收到对方获取进程信息请求"<<endl;
                    procInfoAns();
                    break;
                }
                case PACK_ETH_PORT_INFO_REQ_S:
                {
                    cout<<"收到对方获取网口信息请求"<<endl;
                    ethInfoAns(*((short*)(buf+4)));
                    break;
                }
                case PACK_FLASH_INFO_REQ_S:
                {
                    cout<<"收到对方获取U盘信息请求"<<endl;
                    usbInfoAns();
                    break;
                }
                case PACK_PRINT_PORT_INFO_REQ_S:
                {
                    cout<<"收到对方获取打印口信息请求"<<endl;
                    printPortAns();
                    break;
                }
                case PACK_TTY_SER_INFO_REQ_S:
                {
                    cout<<"收到对方获取终端服务器信息请求"<<endl;
                    ttySerInfoAns();
                    break;
                }
                case PACK_TTY_INFO_REQ_S:
                {
                    cout<<"收到对方获取哑终端信息请求"<<endl;
                    ttyInfoAns(0x0a,ntohs(*(short *)(buf+4)));
                    break;
                }
                case PACK_IP_TTY_INFO_REQ_S:
                {
                    cout<<"收到对方获取IP终端信息请求"<<endl;
                    ttyInfoAns(0x0b,ntohs(*(short *)(buf+4)));
                    break;
                }
                case PACK_FLASH_FILE_INFO_REQ_S:
                {
                    cout<<"收到对方获取U盘文件信息请求"<<endl;
                    usbFileInfoAns();
                    break;
                }
                case PACK_PRINT_QUE_INFO_REQ_S:
                {
                    cout<<"收到对方获取打印队列信息请求"<<endl;
                    printQueAns();
                    break;
                }
                case PACK_DISCON_REQ_S:
                {
                    cout<<"收到对方中断连接请求"<<endl;
                    disconAns();
                    break;
                }

                default:
                    break;
            }
        }
        if (closeFlag)
        {
            if (closeFlag == 1)
            {
                sleep(fail_reConnnectSec);
                close(socketfd);
                continue;
            }    
            else
            {
                if(!exitOpt)
                {
                    sleep(succ_reConnnectSec);
                    continue;
                }
                else
                {
                    xlsWrite(devid,totalDevNum,totalScrNum);
                    break;
                }
            }
        }
    }
    close(socketfd);
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
    string logStr="服务器已连接";
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
    if(!recvSize)
        return ;
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
        if(ret<0)
        {
            closeFlag = 1;
            string logStr="对方关闭了连接";
            logWrite(LocalLogPath, 0, logStr, nullptr, 0);
            return;
        }    
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
    {
        cout<<"对方关闭了连接"<<endl;
        return 0;
    }
    int dataLength = ntohs(*(short *)(buf + 2));
    dataRecv(buf + 8, dataLength - 8);
    if(closeFlag==1)
    {
        cout<<"对方关闭了连接"<<endl;
        return 0;
    }
    string logStr="读取"+to_string(dataLength)+"字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr="(读取数据为:)";
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
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    short ver1 = ntohs(*(short *)(buf + 8));
    u_char ver2 = *(buf + 10);
    u_char ver3 = *(buf + 11);

    logStr="版本号"+to_string(ver1)+"."+to_string(ver2)+"."+to_string(ver3);
    logWrite(LocalLogPath, 0, logStr.c_str(), nullptr, 0);
    if (ver1 < 2)
    {
        logStr="版本过低";
        cout << logStr << endl;
        logWrite(LocalLogPath, 0, logStr, nullptr, 0);
        closeFlag = 1;
        return -1;
    }

    time_t svr_time = (time_t)ntohl((*(u_int *)(buf + 56))^ (u_int)(0xFFFFFFFF)) ;
    struct tm *st;
    st = localtime(&svr_time);
    

    logStr="服务器时间:"+to_string(st->tm_year + 1900)+"-"+to_string(st->tm_mon)+"-"+to_string(st->tm_mday)+" "+to_string(st->tm_hour)+":"+to_string(st->tm_min)+":"+to_string(st->tm_sec);
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
        logStr="本地认证成功";
        cout<<logStr<<endl;
        logWrite(LocalLogPath, 0, logStr, nullptr, 0);
        return 0;
    }
    else
    {
        logStr="本地认证非法";
        cout << logStr << endl;
        logWrite(LocalLogPath, 0, logStr, nullptr, 0);
        closeFlag = 1;
        return -1;
    }
    
    return 0;
}

int client::minimumVerReq()
{
    byte buf[12] = {0};
    packHeadStuff(buf,0x91,0x00,12,0x0000,4);
    *(short *)(buf + 8) = htons(2);
    dataSend(buf, 12);

    string logStr = "发送最低版本信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(12) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "(发送数据为:)";
    logWrite(LocalLogPath, 1, logStr, buf, 12);
    return 0;
}

int client::idsAns()
{
    byte buf[116] = {0};
    packHeadStuff(buf,0x91,0x01,116,0x0000,108);

    short cpuMhz=htons((short)getCPUMHz());//CPU主频
    memcpy(buf+8,&cpuMhz,sizeof(short));

    short ram=htons((short)getRAM());//RAM大小
    memcpy(buf+10,&ram,sizeof(short));

    *(short *)(buf+12)=htons((short)(rand()));//FLASH大小

    *(short *)(buf+14)=htons((short)(1));//内部序列号

    memcpy((buf+16),randStr(16).c_str(),16);//组序列号

    memcpy((buf+32),randStr(16).c_str(),16);//设备型号

    memcpy((buf+48),randStr(16).c_str(),16);//软件版本号

    *(buf+64)=rand()%3;//以太口数量

    *(buf+65)=rand()%3;//同步口数量

    *(buf+66)=(asyNum=rand()%3*8);//异步口数量

    *(buf+67)=rand()%4*8;//交换口数量

    *(buf+68)=rand()%2;//USB口数量

    *(buf+69)=rand()%2;//打印口数量

    *(int *)(buf+72)=htonl(devid);//机构号

    *(buf+76)=1;

    memcpy(buf+80,IdsStr.c_str(),32);

    u_int random_num=(u_int)rand(); 
    int pos = random_num % 4093;
    for(int i=0;i<104;++i,++pos)
    {
        buf[8+i]=buf[8+i]^secret[pos % 4093];
    }


    *(u_int *)(buf+112)=htonl(random_num);

    dataSend(buf,116);

    string logStr = "发送认证信息与基本配置信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(116) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 116);

    return 0;
}

int client::sysInfoAns()
{
    byte buf[28]={0};
    packHeadStuff(buf,0x91,0x02,28,0x0000,20);
    fstream fs1("/proc/stat",ios::in);
    string tp;
    getline(fs1,tp);
    int v1,v2,v3,v4;
    
    size_t pos=0;
    pos=tp.find_first_of("0123456789");
    v1=atoi(&(tp.c_str()[pos]));
    pos=tp.find_first_not_of("0123456789",pos);
    pos=tp.find_first_of("0123456789",pos);
    v2=atoi(&(tp.c_str()[pos]));
    pos=tp.find_first_not_of("0123456789",pos);
    pos=tp.find_first_of("0123456789",pos);
    v3=atoi(&(tp.c_str()[pos]));
    pos=tp.find_first_not_of("0123456789",pos);
    pos=tp.find_first_of("0123456789",pos);
    v4=atoi(&(tp.c_str()[pos]));

    int freed_mem=0;
    fstream fs2("/proc/meminfo",ios::in);
    getline(fs2,tp);
    getline(fs2,tp);
    pos=tp.find_first_of("0123456789");
    freed_mem=atoi(&(tp.c_str()[pos]));
    getline(fs2,tp);
    getline(fs2,tp);
    pos=tp.find_first_of("0123456789");
    freed_mem=freed_mem+atoi(&(tp.c_str()[pos]));
    getline(fs2,tp);
    pos=tp.find_first_of("0123456789");
    freed_mem=freed_mem+atoi(&(tp.c_str()[pos]));

    *(int *)(buf+8)=htonl(v1);
    *(int *)(buf+12)=htonl(v2);
    *(int *)(buf+16)=htonl(v3);
    *(int *)(buf+20)=htonl(v4);
    *(int *)(buf+24)=htonl(freed_mem);

    dataSend(buf,28);

    string logStr = "发送系统信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(28) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 28);

    return 0;
}

int client::configAns()
{
    int maxFileLength=8192;
    byte buf[10000]={0};
    string bufData="";

    fstream fs("config.dat",ios::in|ios::out);

    if(!fs)
        cout<<"未读取到配置信息"<<endl;
    while (!fs.eof())
    {
        string tp;
        getline(fs,tp);
        bufData=bufData+tp;
        if((int)(bufData.size())>maxFileLength)
            break;
    }

    int length=bufData.size()>8192?8192:bufData.size();
    memcpy(buf+8,bufData.c_str(),length);
    buf[8191+8]=0;

    packHeadStuff(buf,0x91,0x03,length+8,0x0000,length);
    dataSend(buf,length+8);

    string logStr = "发送配置信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(length+8) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, length+8);

    return 0;
}

int client::procInfoAns()
{
    int maxFileLength=8192;
    byte buf[10000]={0};
    string bufData="";
    
    fstream fs("process.dat",ios::in|ios::out);

    if(!fs)
        cout<<"未读取到进程信息"<<endl;
    while (!fs.eof())
    {
        string tp;
        getline(fs,tp);
        bufData=bufData+tp;
        if((int)(bufData.size())>maxFileLength)
            break;
    }

    int length=bufData.size()>8192?8192:bufData.size();
    memcpy(buf+8,bufData.c_str(),length);
    buf[8191+8]=0;

    packHeadStuff(buf,0x91,0x04,length+8,0x0000,length);

    dataSend(buf,length+8);

    string logStr = "发送进程信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(length+8) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, length+8);


    return 0;
}

int client::ethInfoAns(short eth)
{
    byte buf[132]={0};
    packHeadStuff(buf,0x91,0x05,132,htons(eth),124);
    *(buf+8)=rand()%2;
    *(buf+9)=rand()%2;
    *(buf+10)=rand()%2;

    *(buf+12)=rand()%256;
    *(buf+13)=rand()%256;
    *(buf+14)=rand()%256;
    *(buf+15)=rand()%256;
    *(buf+16)=rand()%256;
    *(buf+17)=rand()%256;

    *(short *)(buf+18)=htons(rand()%8);

    *(int *)(buf+20)=htonl(rand());//ip
    *(int *)(buf+24)=htonl(rand());
    *(int *)(buf+28)=htonl(rand());//ip 1
    *(int *)(buf+32)=htonl(rand());
    *(int *)(buf+36)=htonl(rand());//ip2
    *(int *)(buf+40)=htonl(rand());
    *(int *)(buf+44)=htonl(rand());//ip3
    *(int *)(buf+48)=htonl(rand());
    *(int *)(buf+52)=htonl(rand());//ip4
    *(int *)(buf+56)=htonl(rand());
    *(int *)(buf+60)=htonl(rand());//ip5
    *(int *)(buf+64)=htonl(rand());

    string ethInfo;
    if(!eth)
        ethInfo=getEth("ens");
    else
        ethInfo=getEth("lo");

    int i=0;
    size_t pos=ethInfo.find_first_of(":");;
    for(;i<16;++i)
    {
        pos=ethInfo.find_first_of("0123456789",pos);
        *(int *)(buf+68+i*4)=htonl(atoi(&(ethInfo.c_str()[pos])));
        pos=ethInfo.find_first_not_of("0123456789",pos);
    }

    

    string logStr = "发送网口信息";
    cout<<logStr<<endl;

    dataSend(buf,132);
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(132) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 132);

    return 0;
}

int client::usbInfoAns()
{
    byte buf[12]={0};
    packHeadStuff(buf,0x91,0x07,12,0x0000,4);
    *(buf+8)=rand()%2;

    dataSend(buf,12);

    string logStr = "发送" + to_string(12) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 12);

    return 0;
}

int client::usbFileInfoAns()
{
    int maxFileLength=4096;
    byte buf[10000]={0};
    string bufData="";

    fstream fs("usbfiles.dat",ios::in|ios::out);

    if(!fs)
        cout<<"未读取到配置信息"<<endl;
    while (!fs.eof())
    {
        string tp;
        getline(fs,tp);
        bufData=bufData+tp;
        if((int)(bufData.size())>maxFileLength)
            break;
    }

    int length=(int)(bufData.size())>8192?8192:bufData.size();
    memcpy(buf+8,bufData.c_str(),length);
    buf[8191+8]=0;

    packHeadStuff(buf,0x91,0x0c,length+8,0x0000,length);
    

    string logStr = "发送U盘文件信息";
    cout<<logStr<<endl;
    dataSend(buf,length+8);
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(length+8) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, length+8);


    return 0;
}

int client::printPortAns()
{
    byte buf[44]={0};
    packHeadStuff(buf,0x91,0x08,44,0x0000,36);
    string str=randStr(4);
    *(buf+8)=rand()%2;
    *(short *)(buf+10)=htonl(rand());

    memcpy(buf+12,str.c_str(),4);

    

    string logStr = "发送打印口信息";
    cout<<logStr<<endl;

    dataSend(buf,44);
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(44) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 44);
    return 0;
}

int client::printQueAns()
{
    byte buf[9]={0};
    packHeadStuff(buf,0x91,0x0d,9,0x0000,1);

    string logStr = "发送打印队列信息";
    cout<<logStr<<endl;
    dataSend(buf,9);
    
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(9) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 9);

    return 0;
}

int client::ttySerInfoAns()
{
    byte buf[280]={0};

    int total=rand()%(maxDevNum-minDevNum)+minDevNum;
    int async_term_num=asyNum==0?0:rand()%asyNum+1;
    if(total<async_term_num)
        total=async_term_num;
    int ipterm_num=total-async_term_num;

    totalDevNum=totalDevNum+total;

    packHeadStuff(buf,0x91,0x09,280,0x0000,272);

    while(async_term_num)
    {
        int r=rand()%asyNum;
        if(*(buf+8+r))
            continue;
        else
        {
            *(buf+8+r)=1;
            --async_term_num;
        }
    }

    while(ipterm_num)
    {
        int r=rand()%254;
        if(*(buf+8+16+r))
            continue;
        else
        {
            *(buf+8+16+r)=1;
            --ipterm_num;
        }
    }

    *(short *)(buf+278)=htons(rand()%(270-total)+total);

    dataSend(buf,280);

    string logStr = "发送终端服务器信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(280) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 280);

    return 0;
}


int client::ttyInfoAns(short ttyType,short devid)
{
    const int everyScreenLength=96;
    byte screenNum=rand()%(maxScrNum-minScrNum)+minScrNum;
    byte buf[5000]={0};

    totalScrNum=totalScrNum+screenNum;

    if(ttyType == 0x0a)
    {
        *(int*)(buf+12)=0;
        memcpy(buf+16,"串口终端",8);
    }
    else
    {
        *(int*)(buf+12)=htonl(rand());
        rand()%2?memcpy(buf+16,"IP终端",6):memcpy(buf+16,"IP代理",6);
    }

    packHeadStuff(buf,0x91,ttyType,36+everyScreenLength*screenNum,devid,28+everyScreenLength*screenNum);

    *(buf+8)=devid;
    *(buf+9)=devid;
    *(buf+10)=rand()%screenNum+1;
    *(buf+11)=screenNum;
    rand()%2?memcpy(buf+28,"正常",4):memcpy(buf+28,"菜单",4);

    byte *screen=buf+36;
    int screenNoSet[16]={0};

    for(int i=0;i<screenNum;++i,screen=screen+everyScreenLength)
    {
        while(1)
        {
            int screenNo=rand()%screenNum+1;
            if(screenNoSet[screenNo])
                continue;
            *screen=screenNo;
            screenNoSet[screenNo]=1;
            break;

        }
        *(short *)(screen+2)=htons(rand());
        *(int *)(screen+4)=htonl(rand());
        rand()%2?memcpy(screen+8,"SSH",3):memcpy(screen+8,"专用SSH",7);
        memcpy(screen+20,"已登录",6);
        memcpy(screen+28,"储蓄系统",8);
        memcpy(screen+52,"vt100",5);
        *(int *)(screen+64)=htonl((int)time(0));
        *(int *)(screen+68)=htonl(rand());
        *(int *)(screen+72)=htonl(rand());
        *(int *)(screen+76)=htonl(rand());
        *(int *)(screen+80)=htonl(rand());
        *(int *)(screen+84)=htonl(rand()%123457);
        *(int *)(screen+88)=htonl(rand()%123457);
        *(int *)(screen+92)=htonl(rand()%123457);
    }

    dataSend(buf,36+everyScreenLength*screenNum);

    string logStr = "发送终端虚屏信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(36+everyScreenLength*screenNum) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 36+everyScreenLength*screenNum);

    return 0;
}

int client::disconAns()
{
    byte buf[8];
    packHeadStuff(buf,0x91,0xff,8,0x0000,0);

    dataSend(buf,8);

    string logStr = "发送断开信息";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr = "发送" + to_string(8) + "字节";
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    logStr ="(发送数据为：)";
    logWrite(LocalLogPath, 1, logStr, buf, 8);

    logStr = "数据发送完毕,断开连接";
    cout<<logStr<<endl;
    logWrite(LocalLogPath, 0, logStr, nullptr, 0);

    closeFlag=2;

    return 0;
}

int xlsWrite(int devid,int devNum,int scrNum)
{
    int fd;
    
    fd = open("ts_cout.xls", O_RDWR| O_APPEND | O_CREAT, 0777);
    if(fd < 0)
	{   	
		printf("Open file error\n");
        return -1;
    }
    time_t t = time(nullptr);
    struct tm *tmTime = localtime(&t);
    char temp[100] = {0};
    strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S ", tmTime);

    
    string buf=temp;
    buf=buf+'\t'+to_string(devid)+'\t'+to_string(1)+'\t'+to_string(devNum)+'\t'+to_string(scrNum)+'\n';
    write(fd, buf.c_str(), buf.size());

    return 0;
}