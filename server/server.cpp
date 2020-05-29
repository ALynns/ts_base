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

                    struct timeval unAcktv;//δӦ��ʱ
                    unAcktv.tv_sec = unAckTimeOut;

                    struct timeval transktv;//δӦ��ʱ
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
    memset(&serviceAddr,0,sizeof(serviceAddr));//���ڸýṹ���д��ڱ���λ����Ҫ��0
    serviceAddr.sin_family=AF_INET;//�����ַ�����ͣ�AF_INETΪIPv4��AF_INET6ΪIPv6
    serviceAddr.sin_port = htons(listenPort);
    //if(IPAddr!=NULL)
        //serviceAddr.sin_addr.s_addr = inet_addr(IPAddr);      //inet_addr("127.0.0.1");
    localSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Linux�ж���socketΪһ���ļ�������ֵΪһ���ļ�������
    if(localSocket<0)
    {
        printf("socket error\n");
        exit(-1);
    }
    
    fcntl(localSocket, F_SETFL, fcntl(localSocket, F_GETFL, 0) | O_NONBLOCK);

    int reuse=1;
    setsockopt(localSocket,SOL_SOCKET ,SO_REUSEADDR,(const char*)& reuse,sizeof(int));
    
    ret=bind(localSocket, (struct sockaddr *)&serviceAddr, sizeof(serviceAddr));//�󶨶˿�
    if (ret < 0)
    {
        printf("bind error\n");
        exit(-1);
    }
    ret = listen(localSocket, SOMAXCONN);
    if (ret < 0) //listen()����ʹ�׽��ֽ��뱻������״̬��SOMAXCONNΪ�������������
    {
        printf("listen error\n");
        exit(-1);
    }
    return localSocket;
}

int server::clientAccept()
{
    clientSocket = accept(localSocket, NULL, NULL); //accept������
    fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);
    return 0;
}

int server::packAnalysis(byte *buf)
{
    dataRecv(buf,8);
    if(closeFlag==1)
    {
        cout<<"�Է��ر�������"<<endl;
        return 0;
    }
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
    //FD_ISSET(clientSocket, &fdsr)�ж��׽����Ƿ��������������һ�������������Թ�
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
    //FD_ISSET(clientSocket, &fdsr)�ж��׽����Ƿ��������������һ�������������Թ�
	while (1)
	{
		ret = recv(clientSocket, recvBuf, recvSize, 0);
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

int server::identity()
{
    byte buf[60];
}