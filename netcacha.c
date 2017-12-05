#include "netcacha.h"
#include "stdio.h"
#include "string.h"

#define netcmdmax 256
#define netcmdcountmax 16
char netqueue[netcmdcountmax][netcmdmax];//命令队列
unsigned char queuehead=0;//队列头
unsigned char queueend=0;//队尾
unsigned char netcmdcount=0;//当前命令计数
unsigned char countflag = 0;//命令队列头尾交叉标志
char nettemp[256] = {0};//命令拆分临时空间
unsigned char frameflag = 0;//数据帧标志
unsigned char netququefull = 0;//queue full state
//拆分函数
int splitnetcmd(char* str)
{
    char nettemp1[256]={0};
    unsigned __int64 len = 0;
    unsigned int index = 0;
    len = strlen(str);
    for(unsigned __int64 i = 0;i<len;i++,str++)
    {
        if(*str == '>')
        {
            frameflag = 1;
            index = 0;
            memset(nettemp1,0,netcmdmax);
            nettemp1[index++] = *str;
            continue;
        }
        if(frameflag == 1)
        {
            nettemp1[index++] = *str;
            if(*str == 'Z')
            {
                frameflag = 0;
                pushnetqueue(nettemp1);
                cleartemp();
            }
        }
        if(index > netcmdmax)
        {
            frameflag = 0;
            cleartemp();
            return 1;
        }
    }
    return 0;
}
//经拆分后得到命令压入命令队列数
int pushnetqueue(char* src)
{
//    int count = 0;
//    count = strlen(src);
    if(countflag == 0)
    {
        strncpy(netqueue[queuehead],src,strlen(src));
        cleartemp();
        if(queuehead < netcmdcountmax-1)
        {
            queuehead++;
        }
        else if(queuehead == netcmdcountmax-1)
        {
            queuehead = 0;
            countflag = 1;
        }
        return 0;
    }
    else if(countflag == 1)
    {
        if(queuehead < queueend)
        {
            strncpy(netqueue[queuehead],src,strlen(src));
            cleartemp();
            queuehead++;
            return 0;
        }
        else if(queuehead == queueend)
        {
            netququefull = 1;
            return 1;
        }
        else if(queuehead > queueend)
        {
            return 2;//
        }
    }

}
unsigned char getnetcmdcount()
{
    unsigned char num = 0;
    if(countflag == 0)
    {
        num = queuehead - queueend;
        if(queuehead == queueend)
            queueend = queuehead = 0;//not must
    }
    if(queuehead != queueend)
    {
        netququefull = 0;
    }
    else if(countflag == 1)
        num = netcmdcountmax + queuehead - queueend;
    return num;
}
//获取命令队列的当前命令。
char* popnetqueue()
{
    unsigned char count = 0;
    count = getnetcmdcount();
    if(count<=0)
        return 0;//null
    else
    {
        if(queueend < netcmdcountmax-1)
        {
            queueend++;
            return netqueue[queueend-1];
        }
        else if(queueend == netcmdcountmax-1)
        {
            queueend = 0;
            return netqueue[netcmdcountmax-1];
        }

    }
}
//命令队列初始化
void netqueueinit()
{
    queuehead=0;
    queueend=0;
    netcmdcount=0;
    cleartemp();
    frameflag = 0;
}
//清空命令临时缓存。
void cleartemp()
{
    memset(nettemp,0,netcmdmax);
}
