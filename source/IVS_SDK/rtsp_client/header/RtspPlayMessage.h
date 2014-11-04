/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspPlayMessage.h
  作    者 : y00182336
  功能描述 : RTSP Play消息
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#ifndef __RTSPPLAYMESSAGE_H_
#define __RTSPPLAYMESSAGE_H_

#include "RtspMessage.h"

class CRtspPlayMessage : public CRtspMessage
{
public:
    CRtspPlayMessage();
    virtual ~CRtspPlayMessage();
    
    // 封装消息
    int encodeMessage(std::string &strMessage);
    
    // 封装播放时间段
    void encodeRangeField(std::string &strMessage);

    // 设置播放时间段
    void setRang(const MEDIA_RANGE_S* stRange);

    // 设置播放速率
    void setScale(const double dScale);

public:
    MEDIA_RANGE_S         m_stRange;             // 播放时间段
    double              m_dScale;               // 播放速率
};


#endif //__RTSPPLAYMESSAGE_H_

