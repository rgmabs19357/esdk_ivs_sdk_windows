/********************************************************************
filename    :    TcpSource.h
author      :    s00191067
created     :    2012/11/05
description :    TCP收流类
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/05 初始版本
*********************************************************************/
#ifndef TCP_SOURCE_H
#define TCP_SOURCE_H

#include "NetSource.h"
#include "RtspPacket.h"

class CChannelInfo;
class CTcpSource : public CNetSource
{
public:
    CTcpSource(void);
    virtual ~CTcpSource(void);

    /******************************************************************
     function   : Init
     description: 初始化;
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
    virtual void ProcessRecevData();

	/******************************************************************
     function   : CheckNatSend
     description: 检查是否满足NAT穿越条件,每隔30s向服务器发送nat消息;
     output     : NA
     return     : int IVS_FAIL表示不满足条件， IVS_SUCCEED表示满足条件;
    *******************************************************************/
    int CheckNatSend();

private:
	long RecvRTSP();

	void ProcRtsp();

	void PacketRtspInfo(const string strLocalIp, unsigned long ulLocalPort, const string strSourceIp, unsigned long ulSourcePort);

    void HandleTimeOutEvent();
private:
    CRtspPacket      m_RtspStack;               // RTSP协议栈;
	SOCKET           m_socket;                  // soket对象;
    bool             m_bIsFirstConnect;         // 第一次连接
    std::string      m_strRemoteIp;             // 远端IP
    unsigned int     m_uiRemotePort;            // 远端端口
}; 
#endif // TCP_SOURCE_H
