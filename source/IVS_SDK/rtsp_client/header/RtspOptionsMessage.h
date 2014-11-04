#ifndef __RTSPOPTIONSMESSAGE_H_
#define __RTSPOPTIONSMESSAGE_H_

#include "RtspMessage.h"

class CRtspOptionsMessage : public CRtspMessage
{
public:
    CRtspOptionsMessage();
    virtual ~CRtspOptionsMessage();
    
    // 封装消息
    int encodeMessage(std::string &strMessage);

    // 解析消息
    int decodeMessage(const char* pszMessage,const int nMsgLen);

    // 解析Data
    int parseData(const std::string &strRtspMessage);

public:
    std::string         m_strDate;          // 时间信息
    std::string         m_strTimeinfo;
};

#endif //__RTSPOPTIONSMESSAGE_H_
