#ifndef

#include "../common/common.h"

#endif

clientInfo::clientInfo()
{
    map<string, string> configOpt = {{"服务器IP地址", ""},
                                     {"端口号", "53935"},
                                     {"进程接收成功后退出", "1"},
                                     {"最小配置终端数量", "5"},
                                     {"最大配置终端数量", "28"},
                                     {"每个终端最小虚屏数量", "3"},
                                     {"每个终端最大虚屏数量", "10"},
                                     {"DEBUG 设置", "111111"},
                                     {"DEBUG 屏幕显示", "0"}};

    fstream fs("../ts.conf",ios::in);

     fs.getline()
}