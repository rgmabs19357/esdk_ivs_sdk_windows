/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspPlayMessage.cpp
  作    者 : y00182336
  功能描述 : RTSP Play消息
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include <sstream>
#include "Rtsp_Log.h"
#include "RtspPlayMessage.h"
#include "RtspRetCode.h"
using namespace std;


/***************************************************************************
 函 数 名 : CRtspPlayMessage
 功能描述 : 构造函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspPlayMessage::CRtspPlayMessage()
{
    m_unRtspMethod  = RTSP_METHOD_PLAY;
    m_dScale       = 0;
    memset (&m_stRange, 0x0, sizeof(MEDIA_RANGE_S));
}

/***************************************************************************
 函 数 名 : ~CRtspPlayMessage
 功能描述 : 析构函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspPlayMessage::~CRtspPlayMessage()
{
    
}


/***************************************************************************
 函 数 名 : encodeMessage
 功能描述 : 封装RTSP Play消息
 输入参数 : 无
 输出参数 : strMessage              封装后的字符串
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspPlayMessage::encodeMessage(std::string &strMessage)
{
    strMessage.clear();
    // 封装RTSP URL
    if (RET_OK != encodeUrl(strMessage))
    {
        IVS_RUN_LOG_CRI("encode rtsp setup message url fail.");
        return RET_FAIL;
    }
    // Scale: 0.0
    stringstream   strValue;
    strValue<<m_dScale;
    strMessage += RTSP_TOKEN_STR_SCALE;
    strMessage += strValue.str();
    strMessage += RTSP_END_TAG;
    // Range: npt=0.000-
    encodeRangeField(strMessage);
    // CSeq:
    encodeCSeq(strMessage);
    strMessage += RTSP_END_TAG;
    return RET_OK;
}

/***************************************************************************
 函 数 名 : setRang
 功能描述 : 设置播放时间段
 输入参数 : strRange                播放时间字符串
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspPlayMessage::setRang(const MEDIA_RANGE_S* stRange)
{
    if (NULL == stRange)
    {
        // 实时浏览模式，全设为0
        m_stRange.enRangeType       = (MEDIA_RANGE_TYPE_E)0;
        m_stRange.MediaBeginOffset  = 0;
        m_stRange.MediaEndOffset    = 0;
    }
    else
    {
        m_stRange.enRangeType       = stRange->enRangeType;
        m_stRange.MediaBeginOffset  = stRange->MediaBeginOffset;
        m_stRange.MediaEndOffset    = stRange->MediaEndOffset;
    }
    return;
}


/***************************************************************************
 函 数 名 : setScale
 功能描述 : 设置播放速率
 输入参数 : dScale                  播放速率字符串
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspPlayMessage::setScale(const double dScale)
{
    m_dScale = dScale;
    return;
}



/***************************************************************************
 函 数 名 : encodeRangeField
 功能描述 : 封装播放时间段
 输入参数 : 无
 输出参数 : strMessage                  封装后的字符串
 返 回 值 : 无
***************************************************************************/
void CRtspPlayMessage::encodeRangeField(std::string &strMessage)
{
    char strTime[32] = { 0 };
    if (RANGE_TYPE_UTC == m_stRange.enRangeType)
    {
        // clock时间
        if ((OFFSET_CUR == m_stRange.MediaBeginOffset)
            || (OFFSET_END == m_stRange.MediaBeginOffset))
        {
            // 当前点，直接返回，没有range字段
            return;
        }
        else if(OFFSET_BEGIN == m_stRange.MediaBeginOffset)
        {
            // 起始点，为0
            m_stRange.MediaBeginOffset = 0;
        }
        strMessage += RTSP_TOKEN_STR_RANGE;
        time_t rangeTime = (time_t) m_stRange.MediaBeginOffset;
        struct tm tmv;

        (void)ACE_OS::localtime_r(&rangeTime, &tmv);
        (void)ACE_OS::snprintf(strTime, 32, "%04d%02d%02dT%02d%02d%02dZ", tmv.tm_year
            + 1900, tmv.tm_mon + 1, tmv.tm_mday, tmv.tm_hour, tmv.tm_min,
            tmv.tm_sec);

        strMessage += RTSP_RANGE_CLOCK;
        strMessage += strTime;
        strMessage += SIGN_H_LINE;

        if (OFFSET_END != m_stRange.MediaEndOffset)
        {
            rangeTime = (time_t) m_stRange.MediaEndOffset;
            (void) ACE_OS::localtime_r(&rangeTime, &tmv);
            (void) ACE_OS::snprintf(strTime, 32, "%04d%02d%02dT%02d%02d%02dZ",
                tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
                tmv.tm_hour, tmv.tm_min, tmv.tm_sec);

            strMessage += strTime;
        }

        strMessage += RTSP_END_TAG;
        return;
    }

    if ((OFFSET_CUR == m_stRange.MediaBeginOffset)
        || (OFFSET_END == m_stRange.MediaBeginOffset))
    {
        // ，直接返回，没有range字段
        return;
    }
    else
    {
        strMessage += RTSP_TOKEN_STR_RANGE;
        // npt时间
        strMessage += RTSP_RANGE_NPT;
        (void) ACE_OS::snprintf(strTime, 32, "%u", m_stRange.MediaBeginOffset);
        strMessage += strTime;
    }
    strMessage += SIGN_H_LINE;

    if (OFFSET_END != m_stRange.MediaEndOffset)
    {
        (void) ACE_OS::snprintf(strTime, 32, "%u", m_stRange.MediaEndOffset);
        strMessage += strTime;
    }
    strMessage += RTSP_END_TAG;

    return;
}

