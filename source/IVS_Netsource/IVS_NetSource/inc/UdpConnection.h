/********************************************************************
filename    :    UdpConnection.h
author      :    s00191067
created     :    2012/11/10
description :    socket连接基类
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/10 初始版本
*********************************************************************/
#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include "hwsdk.h"
#include "NetSourceCommon.h"
#include "Connection.h"

class CUdpConnection : public CConnection
{
public:
    CUdpConnection(void);
    virtual ~CUdpConnection(void);

    /******************************************************************
     function   : Bind
     description: 绑定端口;
     input      : const char * pszLocalIP   本地IP
     input      : unsigned int uiLocalPort  本地端口
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    virtual int Bind(const char* pszLocalIP, unsigned int uiLocalPort);

    /******************************************************************
     function   : Connect
     description: 连接socket
     input      : const char * pszServerIP 服务器IP
     input      : unsigned int uiServerPort服务器端口
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
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
    virtual int RecvData(char* pData, unsigned int uiDataLen);

    /******************************************************************
     function   : SendData
     description: 发送数据;
     input      : char * pData 待发送的数据;
     input      : int uiDataLen 数据长度;
     output     : NA
     return     : int 与::sendto的返回值保持一致;
    *******************************************************************/
    virtual int SendData(const char* pData, unsigned int uiDataLen);

	/******************************************************************
     function   : SendMulitCastData
     description: 发送数据;
     input      : char * pData 待发送的数据;
     input      : int uiDataLen 数据长度;
	 input      : sockaddr_in& sockAddr 使用的socket对象;
     output     : NA
     return     : int 与::sendto的返回值保持一致;
    *******************************************************************/
	int SendMulitCastData(const char* pData, unsigned int uiDataLen, const sockaddr_in& sockAddr);
};
#endif //UDP_CONNECTION_H