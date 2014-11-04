/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspAnnounceMessage.h
  作    者 : y00182336
  功能描述 : RTSP Announce消息
  修改历史 :
    1 日期 : 2011-11-8
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#ifndef __RTSPANNOUNCEMESSAGE_H_
#define __RTSPANNOUNCEMESSAGE_H_
#include "RtspMessage.h"

class CRtspAnnounceMessage : public CRtspMessage
{
public:
    CRtspAnnounceMessage();
    virtual ~CRtspAnnounceMessage();

    // 封装消息
    int encodeMessage(std::string &strMessage);
    
    // 解析消息
    int decodeMessage(const char* pszMessage,const unsigned int unMsgLen);
    
    // 设置SDP
    std::string getRtspSdp()const;
    
    void setStatusCode(unsigned int unStatusCode);
private:
    std::string     m_strRtspSdp;               // 消息体中的SDP字段，需要回调给CU
    unsigned int    m_unStatusCode;             // 响应消息中的返回码
};


#endif



