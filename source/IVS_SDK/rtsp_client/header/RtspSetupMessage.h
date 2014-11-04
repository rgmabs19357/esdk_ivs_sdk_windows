/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspSetupMessage.h
  作    者 : y00182336
  功能描述 : RTSP连接
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#ifndef __RTSPSETUPMESSAGE_H_
#define __RTSPSETUPMESSAGE_H_

#include <string>
#include "ace_header.h"
#include "RtspMessage.h"
#include "RtspDef.h"

class CRtspSetupMessage : public CRtspMessage
{
public:
    CRtspSetupMessage();
    virtual ~CRtspSetupMessage();

    int encodeMessage(std::string &strMessage);

    // 组装RTSP Transport部分
    void encodeTransport(std::string &strMessage);
    
    // 设置传输模式
    void setTransType(unsigned int unTransType);

    // 设置接收端口
    void setClientPort(const unsigned short usVedioPort,
                       const unsigned short usAedioPort);
    
    // 设置接收地址
    void setDestinationIp(unsigned int unIp);

    // 获取发送地址
    unsigned int getServerIp() const;
    
    // 获取发送视频端口
    unsigned short getServerVideoPort()const;

    // 获取发送音频端口
    unsigned short getServerAudioPort()const;
    
    // 解析响应消息
    int decodeMessage(const char* pszMessage, const unsigned int nMsgLen);

private:
    // 解析Transport
    int parseUdpTransParam(const std::string &strRtspMessage);

    int parseTcpTransParam(const std::string &strRtspMessage);

public:
    bool                m_bTcpFlag;                     // 连接方式： true TCP, false UDP
    //MEDIA_ADDR*         m_stPeerAddr;                   // 异步模式下，记录Peer地址

private:
    unsigned int        m_unTransType;                  // 传输模式: TCP/UDP 
    unsigned short      m_usClientVideoPort;            // Rtp端口，RTCP默认为+1
    unsigned short      m_usClientAudioPort;
    unsigned int        m_unDestIp;
    unsigned short      m_usServerVideoPort;            // Rtp视频端口，RTCP默认为+1
    unsigned short      m_usServerAudioProt;            // Rtp音频断开，RTCP默认为+1
    unsigned int        m_unSrcIp;
    bool                m_bIsNoAudio;
};

#endif //__RTSPSETUPMESSAGE_H_

