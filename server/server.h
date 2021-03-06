#ifndef SERVER_H
#define SERVER_H

#include "../common/common.h"
#include <mysql.h>

using namespace std;

class server{
    private:
        int listenPort;
        int connectInterval;
        int sampleInterval;
        string serverIPAddr;
        int serverPort;
        string dbName;
        string userName;
        string userPwd;
        struct timeval unAcktv;//δӦ��ʱ
        struct timeval transtv;
        int mainLogSize;
        int branchLogSize;
        int screenPrint;
        int tmp_packet;
        int tmp_socket;
        int dev_packet;
        int dev_socket;
        int sdram;

        int localSocket;
        int clientSocket;
        MYSQL *conn_ptr;

        int devid;
        int closeFlag;
        struct sockaddr_in clientAddr;
        int ethNum;
        int con;

    public:
        server();
        void infoPrint();
        int serverMain();

        void dataRecv(byte *recvBuf,int recvSize,struct timeval tv);
        void dataSend(const byte *sendBuf, int sendBufSize);
        
        int hostBind();
        int clientAccept();
        int packAnalysis(byte *buf,struct timeval tv);

        int identity();//
        int sysInfoReq();
        int configReq();
        int procInfoReq();
        int ethInfoReq();
        int usbInfoReq();
        int printPortReq();
        int ttySerInfoReq();
        int usbFileInfoReq();
        int printQueReq();
        int ttyDevInfoReq(short no,short tp);
        int disconReq();

        int mysqlInit();
        int mysqlOpt(const char *optStr, int *row, int *col, char **result[]);
        int mysqlSelect(const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[]);
        int mysqlInsert(const char *tableName, const char *opt);
        int mysqlUpdate(const char *tableName,const char *val, const char *opt);


};







#endif

