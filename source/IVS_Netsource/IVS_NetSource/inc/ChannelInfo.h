/********************************************************************
filename    :    ChannelInfo.h
author      :    s00191067
created     :    2012/11/01
description :    端口资源
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/01 初始版本
*********************************************************************/
#ifndef CHANNEL_INFO_H
#define CHANNEL_INFO_H

#include "hwsdk.h"
#include "ivs_error.h"
#include "NetSourceCommon.h"

class CConnection;
class CChannelInfo
{
public:
    CChannelInfo();
    ~CChannelInfo(void);

    /******************************************************************
     function   : SetChannelNo
     description: 设置通道号;
     input      : unsigned long ulChannel 通道号;
     output     : NA
     return     : void
    *******************************************************************/
    void SetChannelNo(unsigned long ulChannel){ m_ulChannel = ulChannel; }

    /******************************************************************
     function   : GetChannelNo
     description: 获取通道号;
     output     : NA
     return     : unsigned long 通道号;
    *******************************************************************/
    unsigned long GetChannelNo(){ return m_ulChannel; }

    /******************************************************************
     function   : SetProtocolType
     description: 设置协议类型;
     input      : NET_PROTOCOL_TYPE enProtocolType 协议类型;
     output     : NA
     return     : void
    *******************************************************************/
    void SetProtocolType(IVS_PROTOCOL_TYPE enProtocolType){ m_enProtocolType = enProtocolType;}

    /******************************************************************
     function   : GetProtocolType
     description: 获取协议类型
     output     : NA
     return     : NET_PROTOCOL_TYPE 协议类型;
    *******************************************************************/
    IVS_PROTOCOL_TYPE GetProtocolType(){ return m_enProtocolType;}

    /******************************************************************
     function   : SetConnection
     description: 设置连接
     input      : SOCKET_TYPE enSocketType  socket类型;
     input      : CConnection * pConnection 连接对象;
     output     : NA
     return     : void
    *******************************************************************/
    void SetConnection(SOCKET_TYPE enSocketType, CConnection* pConnection);

    /******************************************************************
     function   : GetConnection
     description: 获取连接对象;
     input      : SOCKET_TYPE enSocketType  socket类型;
     output     : NA
     return     : CConnection* 连接对象
    *******************************************************************/
    CConnection* GetConnection(SOCKET_TYPE enSocketType);

    /******************************************************************
     function   : GetSocket
     description: 获取socket对象
     input      : SOCKET_TYPE enSocketType
     output     : NA
     return     : SOCKET
    *******************************************************************/
    SOCKET GetSocket(SOCKET_TYPE enSocketType);

    /******************************************************************
     function   : SetLocalPort
     description: 设置本地端口
     input      : PORT_TYPE enPortType  端口类型;
     input      : unsigned int uiPort   端口号;
     output     : NA
     return     : void
    *******************************************************************/
    void SetLocalPort(PORT_TYPE enPortType, unsigned int uiPort);

    /******************************************************************
     function   : GetLocalPort
     description: 获取本地端口;
     input      : PORT_TYPE enPortType  端口类型;
     output     : unsigned int & uiPort 端口号;
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetLocalPort(PORT_TYPE enPortType, unsigned int& uiPort);

    /******************************************************************
     function   : SetRemotePort
     description: 设置远端端口;
     input      : PORT_TYPE enPortType  端口类型;
     input      : unsigned int uiPort   端口号;
     output     : NA
     return     : void
    *******************************************************************/
    void SetRemotePort(PORT_TYPE enPortType, unsigned int uiPort);

    /******************************************************************
     function   : GetRemotePort
     description: 获取远端端口;
     input      : PORT_TYPE enPortType  端口类型;
     output     : unsigned int & uiPort 端口号;
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetRemotePort(PORT_TYPE enPortType, unsigned int& uiPort);
    
    /******************************************************************
     function   : SetStreamIP
     description: 设置流IP地址;
     input      : STREAM_IP_TYPE enStreamIPType 流类型接收端或发送端;
     input      : const char * pszIP            对应的IP
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int SetStreamIP(STREAM_IP_TYPE enStreamIPType, const char* pszIP);

    /******************************************************************
     function   : GetStreamIP
     description: 获取流IP地址;
     input      : STREAM_IP_TYPE enStreamIPType
     output     : std::string & strStreamIP
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetStreamIP(STREAM_IP_TYPE enStreamIPType, std::string& strStreamIP);

    /******************************************************************
     function   : SetDataCallBack
     description: 设置数据回调指针和用户数据;
     input      : NET_DATA_CALLBACK cbDataCallBack
     input      : void * pUser
     output     : NA
     return     : void
    *******************************************************************/
    void SetDataCallBack(NET_DATA_CALLBACK cbDataCallBack, void* pUser);

    /******************************************************************
     function   : GetDataCallBack
     description: 获取数据回调指针和用户数据;
     input      : NA
     output     : NET_DATA_CALLBACK & cbDataCallBack
     output     : void * & pUser
     return     : void
    *******************************************************************/
    void GetDataCallBack(NET_DATA_CALLBACK& cbDataCallBack, void*& pUser);

    /******************************************************************
     function   : SetExceptionCallBack
     description: 设置异常回调指针;
     input      : NET_EXCEPTION_CALLBACK cbExceptionCallBack
     output     : NA
     return     : void
    *******************************************************************/
    void SetExceptionCallBack(NET_EXCEPTION_CALLBACK cbExceptionCallBack);

    /******************************************************************
     function   : GetExceptionCallBack
     description: 获取异常回调指针;
     input      : NA
     output     : NET_EXCEPTION_CALLBACK & cbExceptionCallBack
     return     : void
    *******************************************************************/
    void GetExceptionCallBack(NET_EXCEPTION_CALLBACK& cbExceptionCallBack);

    /******************************************************************
     function   : Clear
     description: 清空数据
     output     : NA
     return     : void
    *******************************************************************/
    void Clear();

	void SetMuilt(bool bIsMuilt){ m_bIsMuilt = bIsMuilt; }
	bool IsMuilt(){ return m_bIsMuilt; }

	void SetOnlyVideo(bool bIsOnlyVideo){ m_bIsOnlyVideo = bIsOnlyVideo; }
	bool IsOnlyVideo(){ return m_bIsOnlyVideo; }

    void SetSessionInfo(const char* pSessionUrl, const char* pSessionId);
    std::string GetSessionUrl();
    std::string GetSessionId();
private:
    unsigned int        m_uiLocalVideoRtpPort;     // 视频Rtp端口
    unsigned int        m_uiLocalVideoRtcpPort;    // 视频Rtcp端口
    unsigned int        m_uiLocalAudioRtpPort;     // 音频Rtp端口
    unsigned int        m_uiLocalAudioRtcpPort;    // 音频Rtcp端口
    unsigned int        m_uiRemoteVideoRtpPort;    // 视频Rtp端口
    unsigned int        m_uiRemoteVideoRtcpPort;   // 视频Rtcp端口
    unsigned int        m_uiRemoteAudioRtpPort;    // 音频Rtp端口
    unsigned int        m_uiRemoteAudioRtcpPort;   // 音频Rtcp端口

    std::string         m_strRemoteSendStreamIp;   // 源端IP
    std::string         m_strLocalRecvStreamIP;    // 本端IP

    IVS_PROTOCOL_TYPE   m_enProtocolType;          //协议类型;
    unsigned long       m_ulChannel;               //通道号;

    NET_DATA_CALLBACK   m_cbDataCallBack;          //网络数据回调函数;
    void*               m_pDataUserData;           //用户数据;
    NET_EXCEPTION_CALLBACK m_cbExceptionCallBack;  //异常回调函数;

    CConnection*        m_pConnVideoRtp;           //VideoRTP的连接对象
    CConnection*        m_pConnVideoRtcp;          //VideoRTCP的连接对象
    CConnection*        m_pConnAudioRtp;           //AudioRTP的连接对象
    CConnection*        m_pConnAudioRtcp;          //AudioRTCP的连接对象
    CConnection*        m_pConnMulticast;          //组播的连接对象;

	bool				m_bIsMuilt;				   // 组播情况;
	bool				m_bIsOnlyVideo;			   // 仅有视频端口，没有音频端口;
    std::string         m_strSessionUrl;           // SessionUrl
    std::string         m_strSessionId;            // SessionID
};
#endif //CHANNEL_INFO_H
