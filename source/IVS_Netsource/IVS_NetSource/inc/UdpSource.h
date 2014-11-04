/********************************************************************
filename    :    UdpSource.h
author      :    s00191067
created     :    2012/11/05
description :    UDP收流类
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/05 初始版本
*********************************************************************/
#ifndef UDP_SOURCE_H
#define UDP_SOURCE_H

#include "NetSource.h"
#include "RtspPacket.h"
#include "Lock.h"

#define SOCKET_COUNT  4     // socket的个数;

class CChannelInfo;
class CUdpSource : public CNetSource
{
public:
    CUdpSource(void);
    virtual ~CUdpSource(void);

    /******************************************************************
     function   : Init
     description: 初始化
     input      : CChannelInfo * pChannelInfo
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    virtual int Init(CChannelInfo* pChannelInfo); 

protected:
    /******************************************************************
     function   : ProcessRecevData
     description: 收流
     output     : NA
     return     : void
    *******************************************************************/
    void ProcessRecevData();

    /******************************************************************
     function   : ProcPacket
     description: 分发报文
     input      : int iIndex    soket连接索引号;
     input      : int iLen       数据长度;
     output     : NA
     return     : void
    *******************************************************************/
    void ProcPacket(int iIndex, int iLen);

    /******************************************************************
     function   : ConnSocket
     description: 连接所有socket，包括音视频的rtp\rtcp连接;
     input      : strRemoteSendStreamIp   远端IP;
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int ConnSocket(const std::string& strRemoteSendStreamIp);

    /******************************************************************
     function   : ShutDownSocket
     description: 关闭所有socket，包括音视频的rtp\rtcp连接;
     output     : NA
     return     : NA
    *******************************************************************/
    void ShutDownSocket();

	/******************************************************************
     function   : CheckNatSend
     description: 检查是否满足NAT穿越条件,每隔30s向服务器发送nat消息;
     output     : NA
     return     : int IVS_FAIL表示不满足条件， IVS_SUCCEED表示满足条件;
    *******************************************************************/
    int CheckNatSend();

protected:
    bool m_bMulticast;                          // 组播标识;

private:
    CRtspPacket      m_RtspStack;               // RTSP协议栈;
	sockaddr_in		 m_SenderVideoRTCPSockAddr;	// 组播情况下记录源端IPC打流的视频rtcp socket对象，穿越保活报文使用;
	sockaddr_in		 m_SenderAudioRTCPSockAddr; // 组播情况下记录源端IPC打流的音频rtcp socket对象，穿越保活报文使用;
};
#endif //UDP_SOURCE_H