#ifndef

#include "../common/common.h"

#endif

clientInfo::clientInfo()
{
    map<string, string> configOpt = {{"������IP��ַ", ""},
                                     {"�˿ں�", "53935"},
                                     {"���̽��ճɹ����˳�", "1"},
                                     {"��С�����ն�����", "5"},
                                     {"��������ն�����", "28"},
                                     {"ÿ���ն���С��������", "3"},
                                     {"ÿ���ն������������", "10"},
                                     {"DEBUG ����", "111111"},
                                     {"DEBUG ��Ļ��ʾ", "0"}};

    fstream fs("../ts.conf",ios::in);

     fs.getline()
}