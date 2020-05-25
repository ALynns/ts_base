#include "client.h"
#include "../common/common.h"

using namespace std;

client::client()
{
    map<string, string> configOpt = {{"������IP��ַ", "0.0.0.0"},
                                     {"�˿ں�", "53935"},
                                     {"���̽��ճɹ����˳�", "1"},
                                     {"��С�����ն�����", "5"},
                                     {"��������ն�����", "28"},
                                     {"ÿ���ն���С��������", "3"},
                                     {"ÿ���ն������������", "10"},
                                     {"ɾ����־�ļ�", "0"},
                                     {"DEBUG ����", "111111"},
                                     {"DEBUG ��Ļ��ʾ", "0"}};

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
    
    serverIPAddr=configOpt["������IP��ַ"];//�ͻ���IP��ַ
    port=atoi(configOpt["�˿ں�"].c_str());
    exitOpt=atoi(configOpt["���̽��ճɹ����˳�"].c_str());
    minDevNum=atoi(configOpt["��С�����ն�����"].c_str());
    maxDevNum=atoi(configOpt["��������ն�����"].c_str());
    minScrNum=atoi(configOpt["ÿ���ն���С��������"].c_str());
    maxScrNum=atoi(configOpt["ÿ���ն������������"].c_str());
    logOpt=atoi(configOpt["ɾ����־�ļ�"].c_str());
    debugOpt=configOpt["DEBUG ����"];
    debugOutput=atoi(configOpt["DEBUG ��Ļ��ʾ"].c_str());
}

void client::infoPrint()
{
    cout << "��ǰ�ͻ������ã�" << endl;


    cout << "������IP��ַ" << "\t" << serverIPAddr << endl; //�ͻ���IP��ַ
    cout << "�˿ں�" << "\t" << port << endl;
    cout << "���̽��ճɹ����˳�" << "\t" << exitOpt << endl;
    cout << "��С�����ն�����" << "\t" << minDevNum<< endl;
    cout << "��������ն�����" << "\t" << maxDevNum<< endl;
    cout << "ÿ���ն���С��������" << "\t" << minScrNum<< endl;
    cout << "ÿ���ն������������" << "\t" << maxScrNum<< endl;
    cout << "ɾ����־�ļ�" << "\t" << logOpt<< endl;
    cout << "DEBUG����" << "\t" << debugOpt<< endl;
    cout << "DEBUG��Ļ��ʾ" << "\t" << debugOutput<< endl;
}