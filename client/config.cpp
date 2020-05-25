#include "client.h"
#include "../common/common.h"

using namespace std;

client::client()
{
    map<string, string> configOpt = {{"服务器IP地址", "0.0.0.0"},
                                     {"端口号", "53935"},
                                     {"进程接收成功后退出", "1"},
                                     {"最小配置终端数量", "5"},
                                     {"最大配置终端数量", "28"},
                                     {"每个终端最小虚屏数量", "3"},
                                     {"每个终端最大虚屏数量", "10"},
                                     {"删除日志文件", "0"},
                                     {"DEBUG 设置", "111111"},
                                     {"DEBUG 屏幕显示", "0"}};

    fstream fs("ts.conf",ios::in|ios::out);

    if(!fs){
        cout << "open config failed" << endl;
        return ;
    }

    while (!fs.eof())
    {
        string buf;
        getline(fs,buf);
        if(buf.find_first_of("#")!=string::npos)
            buf.erase(buf.find_first_of("#"),buf.find_first_of("\n"));

        if(buf.length()==0||buf.find_first_not_of(" \t") == string::npos)
            continue;
        else
        {
            //int blankNum = 0, tabNum = 0;
            
            if (buf.find_first_not_of(" \t") != 0)
                buf.erase(0, buf.find_first_not_of(" \t"));
            string optName(buf.begin(), buf.begin() + buf.find_first_of(" \t"));
            string optValue(buf.begin() + buf.find_first_not_of(" \t", optName.length()), buf.begin() + buf.find_last_not_of(" \t") + 1);
            configOpt[optName]=optValue;
            
        }
    }
    
    serverIPAddr=configOpt["服务器IP地址"];//客户端IP地址
    port=atoi(configOpt["端口号"].c_str());
    exitOpt=atoi(configOpt["进程接收成功后退出"].c_str());
    minDevNum=atoi(configOpt["最小配置终端数量"].c_str());
    maxDevNum=atoi(configOpt["最大配置终端数量"].c_str());
    minScrNum=atoi(configOpt["每个终端最小虚屏数量"].c_str());
    maxScrNum=atoi(configOpt["每个终端最大虚屏数量"].c_str());
    logOpt=atoi(configOpt["删除日志文件"].c_str());
    debugOpt=configOpt["DEBUG 设置"];
    debugOutput=atoi(configOpt["DEBUG 屏幕显示"].c_str());
}

void client::infoPrint()
{
    cout << "当前客户端配置：" << endl;


    cout << "服务器IP地址" << "\t" << serverIPAddr << endl; //客户端IP地址
    cout << "端口号" << "\t" << port << endl;
    cout << "进程接收成功后退出" << "\t" << exitOpt << endl;
    cout << "最小配置终端数量" << "\t" << minDevNum<< endl;
    cout << "最大配置终端数量" << "\t" << maxDevNum<< endl;
    cout << "每个终端最小虚屏数量" << "\t" << minScrNum<< endl;
    cout << "每个终端最大虚屏数量" << "\t" << maxScrNum<< endl;
    cout << "删除日志文件" << "\t" << logOpt<< endl;
    cout << "DEBUG设置" << "\t" << debugOpt<< endl;
    cout << "DEBUG屏幕显示" << "\t" << debugOutput<< endl;
}