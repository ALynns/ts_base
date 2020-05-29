#include "server.h"

using namespace std;

server::server()
{
    map<string, string> configOpt = {{"�����˿ں�", "43935"},
                                     {"�豸���Ӽ��", "15"},
                                     {"�豸�������", "900"},
                                     {"������IP��ַ", "localhost"},
                                     {"�������˿ں�", "3306"},
                                     {"���ݿ���", "yzmon_1753935"},
                                     {"�û���", "dbuser_1753935"},
                                     {"�û�����", "yzmond.1753935"},
                                     {"δӦ��ʱ", "30"},
                                     {"���䳬ʱ", "60"},
                                     {"����־��С"," 10240"},
                                     {"����־��С "," 1024"},
                                     {"��Ļ��ʾ "," 1"},
                                     {"tmp_packet "," 1"},
                                     {"tmp_socket "," 1"},
                                     {"dev_packet "," 1"},
                                     {"dev_socket "," 1"},};

    map<string, int> configFlag = {{"�����˿ں�", 0},
                                     {"�豸���Ӽ��", 0},
                                     {"�豸�������", 0},
                                     {"������IP��ַ", 0},
                                     {"�������˿ں�", 0},
                                     {"���ݿ���", 0},
                                     {"�û���", 0},
                                     {"�û�����", 0},
                                     {"δӦ��ʱ", 0},
                                     {"���䳬ʱ", 0},
                                     {"����־��С", 0},
                                     {"����־��С", 0},
                                     {"��Ļ��ʾ", 0},
                                     {"tmp_packet", 0},
                                     {"tmp_socket", 0},
                                     {"dev_packet", 0},
                                     {"dev_socket",0}};

    fstream fs("yzmond.conf",ios::in|ios::out);

    if(!fs){
        cout << "�����ļ���ȡʧ�ܣ���ʹ��Ĭ�����á�" << endl;
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

    listenPort=atoi(configOpt["�����˿ں�"].c_str());
    connectInterval=atoi(configOpt["�豸���Ӽ��"].c_str());
    sampleInterval=atoi(configOpt["�豸�������"].c_str());
    serverIPAddr=configOpt["������IP��ַ"];
    serverPort=atoi(configOpt["�������˿ں�"].c_str());
    dbName=configOpt["���ݿ���"];
    userName=configOpt["�û���"];
    userPwd=configOpt["�û�����"];
    unAckTimeOut=atoi(configOpt["δӦ��ʱ"].c_str());
    transTimeOut=atoi(configOpt["���䳬ʱ"].c_str());
    mainLogSize=atoi(configOpt["����־��С"].c_str());
    branchLogSize=atoi(configOpt["����־��С"].c_str());
    screenPrint=atoi(configOpt["��Ļ��ʾ"].c_str());
    tmp_packet=atoi(configOpt["tmp_packet"].c_str());
    tmp_socket=atoi(configOpt["tmp_socket"].c_str());
    dev_packet=atoi(configOpt["dev_packet"].c_str());
    dev_socket=atoi(configOpt["dev_socket"].c_str());
}

void server::infoPrint()
{
    cout << "��ǰ�ͻ������ã�" << endl;

    cout<<"[��������]"<<endl;
    cout << "�����˿ں�" << "\t" << listenPort << endl; //�ͻ���IP��ַ
    cout << "�豸���Ӽ��" << "\t" << connectInterval << endl;
    cout << "�豸�������" << "\t" << sampleInterval << endl;

    cout<<"[���ݿ�]"<<endl;
    cout << "������IP��ַ" << "\t" << serverIPAddr<< endl;
    cout << "�������˿ں�" << "\t" << serverPort<< endl;
    cout << "���ݿ���" << "\t" << dbName<< endl;
    cout << "�û���" << "\t" << userName<< endl;
    cout << "�û�����" << "\t" << userPwd<< endl;

    cout<<"[ϵͳ����]"<<endl;
    cout << "δӦ��ʱ" << "\t" << unAckTimeOut<< endl;
    cout << "���䳬ʱ" << "\t" << transTimeOut<< endl;
    cout << "����־��С" << "\t" << mainLogSize<< endl;
    cout << "����־��С" << "\t" << branchLogSize<< endl;

    cout<<"[DEBUG����]"<<endl;
    cout << "��Ļ��ʾ" << "\t" << screenPrint<< endl;
    cout << "tmp_packet" << "\t" << tmp_packet<< endl;
    cout << "tmp_socket" << "\t" << tmp_socket<< endl;
    cout << "dev_packet" << "\t" << dev_packet<< endl;
    cout << "dev_socket" << "\t" << dev_socket<< endl;
    
}