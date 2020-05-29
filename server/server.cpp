#include "server.h"

using namespace std;


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

                    struct timeval unAcktv;//未应答超时
                    unAcktv.tv_sec = unAckTimeOut;

                    struct timeval transktv;//未应答超时
                    transktv.tv_sec = transTimeOut;

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
    clientSocket = accept(localSocket, NULL, NULL); //accept非阻塞
    fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);
    return 0;
}

int server::packAnalysis(byte *buf)
{
    dataRecv(buf,8);
    if(closeFlag==1)
    {
        cout<<"对方关闭了连接"<<endl;
        return 0;
    }
    int dataLength = ntohs(*(short *)(buf + 2));
    dataRecv(buf + 8, dataLength - 8);
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

void server::dataRecv(byte *recvBuf,int recvSize)
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
        ret = select(clientSocket + 1, &fdsr, NULL, NULL, 0);
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
    //FD_ISSET(clientSocket, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
	while (1)
	{
		ret = recv(clientSocket, recvBuf, recvSize, 0);
        if(ret==0)
        {
            closeFlag = 1;
            string logStr="对方关闭了连接";
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

int server::identity()
{
    byte buf[60];
}