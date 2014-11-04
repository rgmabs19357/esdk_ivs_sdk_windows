/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspMessage.h
  作    者 : y00182336
  功能描述 : RTSP消息
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#ifndef __RTSPMESSAGE_H_
#define __RTSPMESSAGE_H_

#include <string>
#include "ace_header.h"
#include "rtsp_client_datatype.h"
#include "RtspDef.h"

class CRtspMessage
{
public:
    CRtspMessage();
    virtual ~CRtspMessage();
    
    // 组装RTSP响应消息
    int encodeRtspRespMsg(unsigned int unStatusCode, std::string &strMessage);

    // 封装URL
    int encodeUrl(std::string &strMessage);
    
    // 封装RTSP序列号
    void encodeCSeq(std::string &strMessage);

    unsigned int getMethodType() const;

    // 设置消息序列号
    void setCSeq(unsigned int unCSeq);

    // 获取消息序列号
    unsigned int getCSeq() const;

    // 设置URL
    void setRtspUrl(const std::string &strUrl);
    
    // 解析是否为RTSP响应消息
    bool isRtspRespMsg(const char* psRtspMsg,const unsigned int unLen);
    
    // 虚函数，封装RTSP消息
    virtual int encodeMessage(std::string &strMessage) = 0;

    // 解析响应消息
    virtual int decodeMessage(const char* pszMessage,const unsigned int unMsgLen);

private:
    // 去除空格
    void trimString(std::string& srcString) const;

public:
    unsigned int        m_unRtspMethod;         // Rtsp消息类型
    std::string         m_strUrl;               // RTSP消息的URL

    static const char*  m_RtspCode[];           // RTSP 响应返回码
private:
    unsigned int        m_unCSeq;               // RTSP消息序列号
    static const char*  m_strRtspMethod[];      // RTSP 消息类型

};


#endif //__RTSPMESSAGE_H_

