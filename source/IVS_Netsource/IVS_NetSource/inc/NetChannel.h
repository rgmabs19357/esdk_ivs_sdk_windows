/********************************************************************
filename    :    NetChannel.h
author      :    s00191067
created     :    2012/11/01
description :    端口资源
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/01 初始版本
*********************************************************************/
#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include "hwsdk.h"
#include "ivs_error.h"
#include "NetSourceCommon.h"
#include "ChannelInfo.h"

class CNetSource;
class CConnection;
class CNetChannel
{
public:
    CNetChannel();
    ~CNetChannel(void);

    /******************************************************************
     function   : SetChannelNo
     description: 设置通道号
     input      : unsigned int ulChannel
     output     : NA
     return     : void
    *******************************************************************/
    void SetChannelNo(unsigned int ulChannel);

    /******************************************************************
     function   : GetChannelNo
     description: 获取通道号
     output     : NA
     return     : unsigned int 通道号;
    *******************************************************************/
    unsigned int GetChannelNo();

    /******************************************************************
     function   : SetProtocolType
     description: 设置协议类型
     input      : NET_PROTOCOL_TYPE enProtocolType
     output     : NA
     return     : void
    *******************************************************************/
    void SetProtocolType(IVS_PROTOCOL_TYPE enProtocolType);

    /******************************************************************
     function   : GetProtocolType
     description: 获取协议类型
     output     : NA
     return     : NET_PROTOCOL_TYPE 协议类型;
    *******************************************************************/
    IVS_PROTOCOL_TYPE GetProtocolType();

    /******************************************************************
     function   : SetConnection
     description: 设置连接对象
     input      : SOCKET_TYPE enSocketType  类型;
     input      : CConnection * pConnection 连接对象;
     output     : NA
     return     : void
    *******************************************************************/
    void SetConnection(SOCKET_TYPE enSocketType, CConnection* pConnection);

    /******************************************************************
     function   : GetConnection
     description: 获取连接对象
     input      : SOCKET_TYPE enSocketType
     output     : NA
     return     : CConnection* 连接对象;
    *******************************************************************/
    CConnection* GetConnection(SOCKET_TYPE enSocketType);

    /******************************************************************
     function   : SetLocalPort
     description: 设置本地端口
     input      : PORT_TYPE enPortType
     input      : unsigned int uiPort
     output     : NA
     return     : void
    *******************************************************************/
    void SetLocalPort(PORT_TYPE enPortType, unsigned int uiPort);

    /******************************************************************
     function   : GetLocalPort
     description: 获取本地端口
     input      : PORT_TYPE enPortType 端口类型;
     output     : unsigned int & uiPort 
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetLocalPort(PORT_TYPE enPortType, unsigned int& uiPort);

    /******************************************************************
     function   : SetRemotePort
     description: 设置远端端口
     input      : PORT_TYPE enPortType
     input      : unsigned int uiPort
     output     : NA
     return     : void
    *******************************************************************/
    void SetRemotePort(PORT_TYPE enPortType, unsigned int uiPort);

    /******************************************************************
     function   : GetRemotePort
     description: 获取远端端口
     input      : PORT_TYPE enPortType  端口类型;
     output     : unsigned int & uiPort 端口号;
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetRemotePort(PORT_TYPE enPortType, unsigned int& uiPort);

    /******************************************************************
     function   : SetStreamIP
     description: 设置IP地址，本地和远端;
     input      : STREAM_IP_TYPE enStreamIPType 类型
     input      : const char * pszIP    IP地址
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int SetStreamIP(STREAM_IP_TYPE enStreamIPType, const char* pszIP);

    /******************************************************************
     function   : GetStreamIP
     description: 获取IP地址;
     input      : STREAM_IP_TYPE enStreamIPType
     input      : std::string & strStreamIP
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetStreamIP(STREAM_IP_TYPE enStreamIPType, std::string& strStreamIP);

    /******************************************************************
     function   : OpenNetStream
     description: 打开网络流
     input      : const char * pszRemoteSendStreamIp
     input      : unsigned int uiRemoteSendStreamVideoPort
     input      : unsigned int uiRemoteSendStreamAudioPort
     input      : NET_EXCEPTION_CALLBACK cbExceptionCallBack
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int OpenNetStream(const char* pszRemoteSendStreamIp, 
                      unsigned int uiRemoteSendStreamVideoPort, 
                      unsigned int uiRemoteSendStreamAudioPort,
                      NET_EXCEPTION_CALLBACK cbExceptionCallBack);

    /******************************************************************
     function   : CloseNetStream
     description: 关闭网络流
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int CloseNetStream();

    /******************************************************************
     function   : StartRecvStream
     description: 启动收流
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int StartRecvStream();

    /******************************************************************
     function   : SetDataCallBack
     description: 设置数据回调
     input      : NET_DATA_CALLBACK cbDataCallBack
     input      : void * pUser
     output     : NA
     return     : void
    *******************************************************************/
    void SetDataCallBack(NET_DATA_CALLBACK cbDataCallBack, void* pUser);

    /******************************************************************
     function   : SetSessionInfo
     description: 设置Session信息
     input      : char* pSessionUrl
     input      : char* pSessionId
     output     : NA
     return     : void
    *******************************************************************/
    void SetSessionInfo(const char* pSessionUrl, const char* pSessionId);

private:
	int BindMulticastPort(unsigned int uiRemoteSendStreamVideoPort, unsigned int uiRemoteSendStreamAudioPort);

private:
    CNetSource*     m_pSource;          //网络收流;
    CChannelInfo    m_channelInfo;      //通道信息,存放数据;
};
#endif //NET_CHANNEL_H
