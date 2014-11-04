/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspOptionsMessage.cpp
  作    者 : y00182336
  功能描述 : RTSP Options消息
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include "RtspAnnounceMessage.h"
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include "rtsp_client_datatype.h"
#include <sstream>
#include <string>

using namespace std;

CRtspAnnounceMessage::CRtspAnnounceMessage()
{
    m_strRtspSdp    = "";
    m_unStatusCode  = 0;
}

CRtspAnnounceMessage::~CRtspAnnounceMessage()
{

}


int CRtspAnnounceMessage::decodeMessage(const char* pszMessage,const unsigned int unMsgLen)
{
    std::string strRtspMsg = "";
    strRtspMsg.append(pszMessage, unMsgLen);
    IVS_DBG_LOG("start decode rtsp message:\n%s", strRtspMsg.c_str());
    // 解析CSeq，解析失败直接返回
    string::size_type nPos = strRtspMsg.find(RTSP_TOKEN_STR_CSEQ);
    if(string::npos == nPos)
    {
        return RET_FAIL;
    }
    string strValue = "";
    strValue = strRtspMsg.substr(nPos + strlen(RTSP_TOKEN_STR_CSEQ));
    nPos = strValue.find(RTSP_END_TAG);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    std::string strCSeq = "";
    strCSeq = strValue.substr(0, nPos);
    unsigned int unCSeq = strtoul(strCSeq.c_str(), NULL, 0);
    setCSeq(unCSeq);

    // 解析Content-Length:，解析失败，回响应
    nPos = strRtspMsg.find(RTSP_TOKEN_STR_CONTENT_LENGTH);
    if(string::npos == nPos)
    {
        // 返回错误
        return RET_FAIL;
    }
    strValue = strRtspMsg.substr(nPos + strlen(RTSP_TOKEN_STR_CONTENT_LENGTH));
    nPos = strValue.find(RTSP_END_TAG);
    if (string::npos == nPos)
    {
        // 返回错误
        return RET_FAIL;
    }
    std::string strContentLen = "";
    strContentLen = strValue.substr(0, nPos);
    unsigned int unContentLen = strtoul(strContentLen.c_str(), NULL, 0);
    if (0 == unContentLen)
    {
        IVS_DBG_LOG("decode announce message success, Content Length = 0");
        return RET_OK;
    }
    // 获取Content-Length:字段
    nPos = strRtspMsg.find(RTSP_MSG_END_TAG);
    m_strRtspSdp = strRtspMsg.substr(nPos + strlen(RTSP_MSG_END_TAG), unContentLen);
    IVS_DBG_LOG("get content result:%s.", m_strRtspSdp.c_str());
    return RET_OK;
}

// 封装RTSP响应消息
int CRtspAnnounceMessage::encodeMessage(std::string &strMessage)
{
    strMessage.clear();
    // 响应消息，构造状态行
    if (m_unStatusCode >= RTSP_STATUS_CODES_BUTT)
    {
        IVS_RUN_LOG_WAR("encode rtsp common response fail, status code[%u] invalid.",
            m_unStatusCode);
        return RET_FAIL;
    }
    strMessage = RTSP_PROTOCOL_VERSION;
    strMessage += " ";
    strMessage += CRtspMessage::m_RtspCode[m_unStatusCode];
    strMessage += RTSP_END_TAG;

    // Cseq
    std::stringstream strCSeq;
    strCSeq << getCSeq();
    strMessage += RTSP_TOKEN_STR_CSEQ;
    strMessage += strCSeq.str();
    strMessage += RTSP_END_TAG;
    
    // 增加Content-Length头域
    strMessage += RTSP_TOKEN_STR_CONTENT_LENGTH;
    strMessage += "0";
    strMessage += RTSP_END_TAG;

    // 增加Session头域
    strMessage += RTSP_TOKEN_STR_SESSION;
    strMessage += "1";
    strMessage += RTSP_END_TAG;
    strMessage += RTSP_END_TAG;

    IVS_DBG_LOG("success to encode common response.\n%s", strMessage.c_str());
    return RET_OK;
}


// 设置RTSP SDP
string CRtspAnnounceMessage::getRtspSdp()const
{
    return m_strRtspSdp;
}


// 设置返回码
void CRtspAnnounceMessage::setStatusCode(unsigned int unStatusCode)
{
    m_unStatusCode = unStatusCode;
}

