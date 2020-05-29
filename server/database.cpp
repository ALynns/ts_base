#include "server.h"

using namespace std;

int server::mysqlInit()
{
    conn_ptr = mysql_init(NULL);
    conn_ptr = mysql_real_connect(conn_ptr, serverIPAddr.c_str(), dbName.c_str(), userName.c_str(), userPwd.c_str(), 0, NULL, 0);
    if(!conn_ptr)
        return -1;
    return 0;
}

int server::mysqlOpt(const char *optStr, int *row, int *col, char **result[])
{
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow; 
    int res, r, c;

    res = mysql_query(conn_ptr, optStr); //查询语句

    if (res)
    {
        //printf("SELECT error:%s\n",mysql_error(conn_ptr));查询错误
        return -1;
    }
    else
    {
        res_ptr = mysql_store_result(conn_ptr); //取出结果集
        if (res_ptr && result)
        {
            r  =mysql_num_rows(res_ptr);
            c = mysql_num_fields(res_ptr);
            int i = 0, j = 0;
            while ((sqlrow = mysql_fetch_row(res_ptr)))
            { //依次取出记录

                for (i = 0; i < c; ++i)
                {
                    strcpy(result[j][i], sqlrow[i]);
                }    
                ++j;
            }
            if (mysql_errno(conn_ptr))
            {
                cerr<< "Retrive error:"<< mysql_error(conn_ptr)<<endl;
            }
        }
        else
        {
            return -1;
        }
        
        mysql_free_result(res_ptr);
        if (row)
            (*row) = r;
        if (col)
            (*col) = c;
    }

    return 0;
}

int server::mysqlSelect(const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[])
{
    char optStr[200] = {0};
    sprintf(optStr,"select %s from %s ",selectItem,tableName);
    if(opt)
    {
        strcat(optStr, "where ");
        strcat(optStr,opt);
    }    
    strcat(optStr,";");

    mysqlOpt(optStr, row, col, result);

    return 0;
}

int server::mysqlInsert(const char *tableName, const char *opt)
{
    char optStr[200] = {0};
    sprintf(optStr,"insert into %s values(%s);",tableName,opt);
    return mysqlOpt(optStr, NULL, NULL, NULL);
}