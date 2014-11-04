/********************************************************************
	filename	: 	SocketService.h
	author		:	s00194833
	created		:	2011/12/07	
	description	:	Socket通讯类
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/12/07 初始版本
*********************************************************************/
#ifndef SOCKETSERVICE_H
#define SOCKETSERVICE_H

/*!< 系统预编译头*/
#include <WinSock2.h>

class CSocketService
{
public:
    CSocketService();
    ~CSocketService();

    /**************************************************************************
    * name       : InitSock
    * description: 初始化Socket通讯库
    * input      : NA
    * output     : NA
    * return     : IVS_SUCCEED 成功，其他 失败
    * remark     : NA
    **************************************************************************/
    static int InitSock();

    /**************************************************************************
    * name       : FreeSock
    * description: 释放Socket通讯库资源
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    static void FreeSock();

    /**************************************************************************
    * name       : CreateSocket
    * description: 创建套字节
    * input      : iAF地址族 只支持AF_INET
                   iType Socket类型 支持SOCK_STREAM、SOCK_DGRAM
                   iRrotocol 协议方法 IPPROTO_TCP、IPPROTO_UDP
    * output     : NA
    * return     : IVS_SUCCEED 成功，其他 失败
    * remark     : NA
    **************************************************************************/
    int CreateSocket(int iAF =  AF_INET,int iType = SOCK_STREAM,int iProtocol = IPPROTO_TCP);

    /**************************************************************************
    * name       : SetServerAddr
    * description: 设置服务器IP和端口号
    * input      : iPort 服务器端口号
                   pszIP 服务器IP
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void SetServerAddr(u_short iPort,const char *pszIP = NULL);

    int Connect(int iTimeout);
    /**************************************************************************
    * name       : Listen
    * description: 监听端口
    * input      : NA
    * output     : NA
    * return     : IVS_SUCCEED 成功，其他 失败
    * remark     : NA
    **************************************************************************/
    int Listen();
    /**************************************************************************
    * name       : UDPListen
    * description: 监听端口
    * input      : NA
    * output     : NA
    * return     : IVS_SUCCEED 成功，其他 失败
    * remark     : NA
    **************************************************************************/
    int UDPListen();

    /**************************************************************************
    * name       : AcceptClientRequst
    * description: 等待客户请求
    * input      : iTimeout 连接超时时间 单位s
    * output     : NA
    * return     : IVS_SUCCEED 成功，其他 失败
    * remark     : NA
    **************************************************************************/
    int AcceptClientRequst(int iTimeout = 4);

    /**************************************************************************
    * name       : SendData
    * description: 发送数据 
    * input      : pszSendData 发送数据缓冲区
                   iLength 发送数据长度
                   iTimeout 超时时间
    * output     : NA
    * return     : totalSendBytes  已发送的数据长度
    * remark     : NA
    **************************************************************************/
    int Send(const char *pszSendData,int iLength,int iTimeout);

    /**************************************************************************
    * name       : ReceiveData
    * description: 接收数据
    * input      : iTimeout 超时时间
    * output     : pszReceiveData 接收数据缓冲区
                   iLength 接收数据缓冲区长度
    * return     : iReceiveLength 实际接收数据长度
    * remark     : NA
    **************************************************************************/
    int Receive(char *pszReceiveData,u_int iLength,int iTimeout);

    /**************************************************************************
    * name       : ReceiveData(UDP)
    * description: 接收数据
    * input      : iTimeout 超时时间
    * output     : pszReceiveData 接收数据缓冲区
                   iLength 接收数据缓冲区长度
    * return     : iReceiveLength 实际接收数据长度
    * remark     : NA
    **************************************************************************/
    int UDPReceive(char *pszReceiveData,u_int iLength, int iTimeout);

    /**************************************************************************
    * name       : CloseConnectSocket
    * description: 关闭连接套字节
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void CloseConnectSocket();

    /**************************************************************************
    * name       : CloseSocket
    * description: 关闭套字节
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void CloseSocket();
private:

    SOCKET m_hSocket;               //服务器套字节
    SOCKET m_hConnectSocket;              //连接套字节


    SOCKADDR_IN m_ServerAddr;       //服务器端地址
    SOCKADDR_IN m_ClientAddr;       //客户端地址

    static bool m_bInitLabel;       //标志通讯库是否初始化
    static int m_iInUse;            //统计有多少用户使用该通讯库
};
#endif //SOCKETSERVICE_H
