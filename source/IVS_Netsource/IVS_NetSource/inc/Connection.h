/********************************************************************
filename    :    Connection.h
author      :    s00191067
created     :    2012/11/10
description :    socket连接基类
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/10 初始版本
*********************************************************************/
#ifndef CONNECTION_H
#define CONNECTION_H

#include "hwsdk.h"
#include "ivs_error.h"
#include "NetSourceCommon.h"

//select 模式下，三种状态查询值;
typedef enum SelectType 
{
    READ,
    WRITE,
    EXCEPT
}SELECT_TYPE;

//阻塞模式;
#define BLOCKSTATE      0
//非阻塞模式;
#define NOBLOCKSTATE    1

class CConnection
{
    
public:
    CConnection(void);
    virtual ~CConnection(void);

    /******************************************************************
     function   : Bind
     description: 绑定指定端口;基类仅存储IP和Port，由子类实现绑定;
     input      : const char * pszLocalIP   本地IP;
     input      : unsigned int uiLocalPort  本地端口;
     output     : NA
     return     : int   IVS_SUCCEED 成功 IVS_FAIL 失败;
    *******************************************************************/
    virtual int Bind(const char* pszLocalIP, unsigned int uiLocalPort);

    /******************************************************************
     function   : Connect
     description: 连接服务器;基类仅存储IP和Port，由子类实现连接;
     input      : const char * pszServerIP  服务器IP;
     input      : unsigned int uiServerPort 服务器端口;
     output     : NA
     return     : int IVS_SUCCEED 成功 IVS_FAIL 失败;
    *******************************************************************/
    virtual int Connect(const char* pszServerIP, unsigned int uiServerPort);

    /******************************************************************
     function   : RecvData
     description: 接收数据;
     input      : NA
     input      : char * pData  存储接收后的数据
     output     : int uiDataLen 接收的数据长度;
     return     : int 与::recvfrom的返回值保持一致
    *******************************************************************/
    virtual int RecvData(char* pData, unsigned int uiDataLen) = 0;
    
    /******************************************************************
     function   : SendData
     description: 发送数据;
     input      : char * pData 待发送的数据;
     input      : int uiDataLen 数据长度;
     output     : NA
     return     : int 与::sendto的返回值保持一致;
    *******************************************************************/
    virtual int SendData(const char* pData, unsigned int uiDataLen) = 0;

    /******************************************************************
     function   : SetSocketBuf
     description: 设置Socket缓冲区大小;
     input      : unsigned int uiSize   缓冲区大小;
     output     : NA
     return     : int 
    *******************************************************************/
    int SetSocketBuf(unsigned int uiSize);

    /******************************************************************
     function   : SetIOState
     description: 设置IO状态;
     input      : bool bIsBlock true阻塞, false非阻塞;
     output     : NA
     return     : void
    *******************************************************************/
    void SetIOState(bool bIsBlock);

    /******************************************************************
     function   : ShutDown
     description: 关闭Socket;
     output     : NA
     return     : void
    *******************************************************************/
    void ShutDown();

    /******************************************************************
     function   : Select
     description: 
     input      : SELECT_TYPE enType    模式类型,read,write,except;
     input      : long lUsec            等待时间，默认为5微秒;
     output     : NA
     return     : int 成功: 处于就绪状态并且已经包含在fd_set结构中的描述字总数
                  失败: 0 -- 超时, 其它错误返回SOCKET_ERROR,调用WSAGetLastError查找错误码;  
    *******************************************************************/
    int Select(SELECT_TYPE enType, long lUsec = 5);

    /******************************************************************
     function   : GetLocalIpPort
     description: 获取本地IP和端口;
     input      : std::string & strIP   本地IP;
     input      : unsigned int & uiPort 本地端口;
     output     : NA
     return     : void
    *******************************************************************/
    void GetLocalIpPort(std::string& strIP, unsigned int& uiPort);

    /******************************************************************
     function   : GetServerIpPort
     description: 获取服务器IP和端口;
     input      : std::string & strIP   服务器IP;
     input      : unsigned int & uiPort 服务器端口;
     output     : NA
     return     : void
    *******************************************************************/
    void GetServerIpPort(std::string& strIP, unsigned int& uiPort);

    /******************************************************************
     function   : InitMulticast
     description: 初始化组播;
     input      : NA
     output     : NA
     return     : int IVS_SUCCEED 成功 IVS_FAIL 失败;
    *******************************************************************/
    int InitMulticast(const std::string& strRemoteSendStreamIp,
                      unsigned int uRemoteSendStreamPort,
                      const std::string& strLocalRecvStreamIp);

    // 返回连接实例绑定的socket对象;
    SOCKET GetSocket() const { return m_socket; };

protected:
    SOCKET          m_socket;           // soket对象;
    std::string     m_strLocalIP;       // 本地IP;
    std::string     m_strServerIP;      // 服务器IP;
    unsigned int    m_uiLocalPort;      // 本地端口;
    unsigned int    m_uiServerPort;     // 服务器端口;
};
#endif //CONNECTION_H