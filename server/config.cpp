#include "server.h"

using namespace std;

server::server()
{
    map<string, string> configOpt = {{"监听端口号", "43935"},
                                     {"设备连接间隔", "15"},
                                     {"设备采样间隔", "900"},
                                     {"服务器IP地址", "localhost"},
                                     {"服务器端口号", "3306"},
                                     {"数据库名", "yzmon_1753935"},
                                     {"用户名", "dbuser_1753935"},
                                     {"用户口令", "yzmond.1753935"},
                                     {"未应答超时", "30"},
                                     {"传输超时", "60"},
                                     {"主日志大小"," 10240"},
                                     {"分日志大小 "," 1024"},
                                     {"屏幕显示 "," 1"},
                                     {"tmp_packet "," 1"},
                                     {"tmp_socket "," 1"},
                                     {"dev_packet "," 1"},
                                     {"dev_socket "," 1"},};

    map<string, int> configFlag = {{"监听端口号", 0},
                                     {"设备连接间隔", 0},
                                     {"设备采样间隔", 0},
                                     {"服务器IP地址", 0},
                                     {"服务器端口号", 0},
                                     {"数据库名", 0},
                                     {"用户名", 0},
                                     {"用户口令", 0},
                                     {"未应答超时", 0},
                                     {"传输超时", 0},
                                     {"主日志大小", 0},
                                     {"分日志大小", 0},
                                     {"屏幕显示", 0},
                                     {"tmp_packet", 0},
                                     {"tmp_socket", 0},
                                     {"dev_packet", 0},
                                     {"dev_socket",0}};

    fstream fs("yzmond.conf",ios::in|ios::out);

    if(!fs){
        cout << "配置文件读取失败，将使用默认配置。" << endl;
    }

    while (!fs.eof())
    {
        string buf;
        getline(fs,buf);
        if(buf.find_first_of("#[")!=string::npos)
            buf.erase(buf.find_first_of("#["),buf.find_first_of("\n"));

        if(buf.length()==0||buf.find_first_not_of(" \t") == string::npos)
            continue;
        else
        {
            //int blankNum = 0, tabNum = 0;

            if (buf.find_first_not_of(" \t") != 0)
                buf.erase(0, buf.find_first_not_of(" \t"));
            string optName(buf.begin(), buf.begin() + buf.find_first_of(" \t"));

            if (configFlag[optName])
                continue;
                
            string optValue(buf.begin() + buf.find_first_not_of("= \t", optName.length()), buf.begin() + buf.find_last_not_of(" =\t") + 1);
            configOpt[optName] = optValue;
            configFlag[optName] = 1;
        }
    }
    
    idsFlag=0;

    listenPort=atoi(configOpt["监听端口号"].c_str());
    connectInterval=atoi(configOpt["设备连接间隔"].c_str());
    sampleInterval=atoi(configOpt["设备采样间隔"].c_str());
    serverIPAddr=configOpt["服务器IP地址"];
    serverPort=atoi(configOpt["服务器端口号"].c_str());
    dbName=configOpt["数据库名"];
    userName=configOpt["用户名"];
    userPwd=configOpt["用户口令"];
    unAckTimeOut=atoi(configOpt["未应答超时"].c_str());
    transTimeOut=atoi(configOpt["传输超时"].c_str());
    mainLogSize=atoi(configOpt["主日志大小"].c_str());
    branchLogSize=atoi(configOpt["分日志大小"].c_str());
    screenPrint=atoi(configOpt["屏幕显示"].c_str());
    tmp_packet=atoi(configOpt["tmp_packet"].c_str());
    tmp_socket=atoi(configOpt["tmp_socket"].c_str());
    dev_packet=atoi(configOpt["dev_packet"].c_str());
    dev_socket=atoi(configOpt["dev_socket"].c_str());
}

void server::infoPrint()
{
    cout << "当前客户端配置：" << endl;

    cout<<"[连接设置]"<<endl;
    cout << "监听端口号" << "\t" << listenPort << endl; //客户端IP地址
    cout << "设备连接间隔" << "\t" << connectInterval << endl;
    cout << "设备采样间隔" << "\t" << sampleInterval << endl;

    cout<<"[数据库]"<<endl;
    cout << "服务器IP地址" << "\t" << serverIPAddr<< endl;
    cout << "服务器端口号" << "\t" << serverPort<< endl;
    cout << "数据库名" << "\t" << dbName<< endl;
    cout << "用户名" << "\t" << userName<< endl;
    cout << "用户口令" << "\t" << userPwd<< endl;

    cout<<"[系统设置]"<<endl;
    cout << "未应答超时" << "\t" << unAckTimeOut<< endl;
    cout << "传输超时" << "\t" << transTimeOut<< endl;
    cout << "主日志大小" << "\t" << mainLogSize<< endl;
    cout << "分日志大小" << "\t" << branchLogSize<< endl;

    cout<<"[DEBUG设置]"<<endl;
    cout << "屏幕显示" << "\t" << screenPrint<< endl;
    cout << "tmp_packet" << "\t" << tmp_packet<< endl;
    cout << "tmp_socket" << "\t" << tmp_socket<< endl;
    cout << "dev_packet" << "\t" << dev_packet<< endl;
    cout << "dev_socket" << "\t" << dev_socket<< endl;
    
}