/********************************************************************
filename    :    NetChannelMgr.h
author      :    s00191067
created     :    2012/11/01
description :    端口资源管理
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/01 初始版本
*********************************************************************/
#ifndef NET_CHANNEL_MGR_H
#define NET_CHANNEL_MGR_H

#include <list>
#include "NetSourceCommon.h"
#include "Lock.h"

class CNetChannel;
class CConnection;
class CNetChannelMgr
{
private:
    // 单实例，构造函数设为私有;
    CNetChannelMgr(void); 

public:
    /******************************************************************
     function   : instance
     description: 单实例
     output     : NA
     return     : CNetChannelMgr& 单实例对象;
    *******************************************************************/
    static CNetChannelMgr& instance()
    {
        static CNetChannelMgr NetChannelMgr;
        return NetChannelMgr;
    }

    /******************************************************************
     function   : ~CNetChannelMgr
     description: 析构函数;
     input      : void
     output     : NA
     return     : NA
    *******************************************************************/
    ~CNetChannelMgr(void);

    /******************************************************************
     function   : Init
     description: 初始化
     input      : void
     output     : NA
     return     : void
    *******************************************************************/
    void Init(void);

    /******************************************************************
     function   : UnInit
     description: 清空;
     input      : void
     output     : NA
     return     : void
    *******************************************************************/
    void UnInit(void);

    /******************************************************************
     function   : GetChannel
     description: 获取可用句柄
     input      : NA
     output     : unsigned long & ulChannel
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetChannel(unsigned long& ulChannel);

    /******************************************************************
     function   : FreeChannel
     description: 释放指定句柄
     input      : unsigned long ulChannel
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int FreeChannel(unsigned long ulChannel);

    /******************************************************************
     function   : SetProtocolType
     description: 设置媒体流传输协议类型
     input      : unsigned long ulChannel
     input      : unsigned int uiProtocolType
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int SetProtocolType(unsigned long ulChannel, unsigned int uiProtocolType);

    /******************************************************************
     function   : GetProtocolType
     description: 获得媒体流传输协议类型
     input      : unsigned long ulChannel
     output     : unsigned int & uiProtocolType
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetProtocolType(unsigned long ulChannel, unsigned int& uiProtocolType);

    /******************************************************************
     function   : SetLocalRecvStreamIP
     description: 设置接收码流的Ip地址;
     input      : unsigned long ulChannel
     input      : const char * pszLocalRecvStreamIp
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int SetLocalRecvStreamIP(unsigned long ulChannel, const char* pszLocalRecvStreamIp);

    /******************************************************************
     function   : GetLocalRecvStreamIP
     description: 获得接收码流的Ip地址
     input      : unsigned long ulChannel
     output     : char * pszLocalRecvStreamIp
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetLocalRecvStreamIP(unsigned long ulChannel, char* pszLocalRecvStreamIp);

    /******************************************************************
     function   : GetLocalRecvStreamPort
     description: 获取接收码流的地址IP和端口号;
     input      : unsigned long ulChannel
     output     : unsigned int & uiLocalRecvStreamVideoRtpPort
     output     : unsigned int & uiLocalRecvStreamAudioRtpPort
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetLocalRecvStreamPort(unsigned long ulChannel, unsigned int& uiLocalRecvStreamVideoRtpPort, 
                               unsigned int& uiLocalRecvStreamAudioRtpPort);

	/******************************************************************
     function   : SetLocalRecvStreamPort
     description: 设置接收码流的地址IP和端口号;
     input      : unsigned long ulChannel
     input      : unsigned int  uiLocalRecvStreamVideoRtpPort
     input      : unsigned int & uiLocalRecvStreamAudioRtpPort
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int SetLocalRecvStreamPort(unsigned long ulChannel, unsigned int uiLocalRecvStreamVideoRtpPort, 
                               unsigned int uiLocalRecvStreamAudioRtpPort);

    /******************************************************************
     function   : OpenNetStream
     description: 打开网络流
     input      : unsigned long ulChannel
     input      : const char * pRemoteSendStreamIp
     input      : unsigned int uiRemoteSendStreamVideoPort
     input      : unsigned int uiRemoteSendStreamAudioPort
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int OpenNetStream(unsigned long ulChannel, const char* pRemoteSendStreamIp, 
                      unsigned int uiRemoteSendStreamVideoPort, 
                      unsigned int uiRemoteSendStreamAudioPort);

    /******************************************************************
     function   : CloseNetStream
     description: 关闭网络流
     input      : unsigned long ulChannel
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int CloseNetStream(unsigned long ulChannel);

    /******************************************************************
     function   : StartRecvStream
     description: 启动收流
     input      : unsigned long ulChannel
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int StartRecvStream(unsigned long ulChannel);

    /******************************************************************
     function   : SetExceptionCallBack
     description: 设置异常回调
     input      : NET_EXCEPTION_CALLBACK cbExceptionCallBack
     input      : void * pUser
     output     : NA
     return     : void
    *******************************************************************/
    void SetExceptionCallBack(NET_EXCEPTION_CALLBACK cbExceptionCallBack, void* pUser);

    /******************************************************************
     function   : SetDataCallBack
     description: 设置数据回调
     input      : unsigned long ulChannel
     input      : NET_DATA_CALLBACK cbDataCallBack
     input      : void * pUser
     output     : NA
     return     : void
    *******************************************************************/
    void SetDataCallBack(unsigned long ulChannel, NET_DATA_CALLBACK cbDataCallBack, void* pUser);

    /******************************************************************
     function   : DoExceptionCallBack
     description: 反馈异常，该函数由内部模块调用，将异常统一反馈到管理类中，再统一回调上层;
     input      : int iMsgType      异常类型;
     input      : void * pParam     参数信息，根据异常类型可扩展;
     output     : NA
     return     : void
    *******************************************************************/
    void DoExceptionCallBack(int iMsgType, void* pParam);

	/******************************************************************
     function   : FreePortfromList
     description: 回收端口，移到空闲端口列表中;
     input      : unsigned int uiSocketPort 待回收端口号
     output     : NA
     return     : void
    *******************************************************************/
    void FreePortfromList(unsigned int uiSocketPort);

    /******************************************************************
     function   : SetSessionInfo
     description: 设置Session信息
     input      : unsigned long ulChannel
     input      : char* pSessionUrl
     input      : char* pSessionId
     output     : NA
     return     : void;
    *******************************************************************/
    void SetSessionInfo(unsigned long ulChannel, char* pSessionUrl, char* pSessionId);
private:
    /******************************************************************
     function   : GetUdpSocketPort
     description: 获取UDP模式下RTP端口及连接对象;
     input      : const char * pszLocalRecvStreamIp 本地IP
     output     : unsigned int & uiRtpPort          rtp端口
     output     : CConnection *  & pRtpConnection   rtp连接对象
     output     : CConnection *  & pRtcpConnection  rtcp连接对象
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int GetUdpSocketPort(const char* pszLocalRecvStreamIp, 
                         unsigned int& uiRtpPort, 
                         CConnection* &pRtpConnection, 
                         CConnection* &pRtcpConnection);

	/******************************************************************
     function   : SetUdpSocketPort
     description: 设置UDP模式下RTP端口及连接对象;
     input      : const char * pszLocalRecvStreamIp 本地IP
     input      : unsigned int & uiRtpPort          rtp端口
     output     : CConnection *  & pRtpConnection   rtp连接对象
     output     : CConnection *  & pRtcpConnection  rtcp连接对象
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int SetUdpSocketPort(const char* pszLocalRecvStreamIp, 
                         unsigned int uiRtpPort, 
                         CConnection* &pRtpConnection, 
                         CConnection* &pRtcpConnection);

    /******************************************************************
     function   : GetTcpSocketPort
     description: 获取TCP模式下端口及连接对象;
     input      : const char * pszLocalRecvStreamIp 本地IP;
     input      : unsigned int & uiPort             本地端口;
     input      : CConnection *  & pConnection      连接对象;
     output     : NA
     return     : int
    *******************************************************************/
    int GetTcpSocketPort(const char* pszLocalRecvStreamIp, 
                        unsigned int& uiPort, 
                        CConnection* &pConnection);

	/******************************************************************
     function   : SetTcpSocketPort
     description: 设置TCP模式下端口及连接对象;
     input      : const char * pszLocalRecvStreamIp 本地IP;
     input      : unsigned int  uiPort             本地端口;
     input      : CConnection *  & pConnection      连接对象;
     output     : NA
     return     : int
    *******************************************************************/
    int SetTcpSocketPort(const char* pszLocalRecvStreamIp, 
                        unsigned int uiPort, 
                        CConnection* &pConnection);

    /******************************************************************
     function   : GetChannelByID
     description: 根据通道号获取网络通道对象;
     input      : unsigned int ulChannel
     output     : NA
     return     : CNetChannel* 通道对象;
    *******************************************************************/
    CNetChannel* GetChannelByID(unsigned int ulChannel);

private:
    // 异常相关;
    NET_EXCEPTION_CALLBACK      m_cbExceptionCallBack;       // 上层异常回调指针;
    void*                       m_pUserData;                 // 上层异常回调相关的用户数据;
    CIVSMutex                   m_mutexExceptionCallback;    

    // 通道号与通道资源的映射表;
    typedef std::map<unsigned long, CNetChannel*> NetChannelMap;
    typedef NetChannelMap::iterator NetChannelMapIter;
    NetChannelMap               m_mapNetChannel;	
    CIVSMutex                   m_mutexChannel;

    // 端口号列表;
    typedef std::list<unsigned int> PortList;
    typedef PortList::iterator PortListIter;
    PortList                    m_lstFreePort;             // 空闲端口列表;
    PortList                    m_lstBusyPort;             // 已用端口列表;
    CIVSMutex                   m_mutexFreePortList;
    CIVSMutex                   m_mutexBusyPortListLock;
};
#endif //NET_CHANNEL_MGR_H
