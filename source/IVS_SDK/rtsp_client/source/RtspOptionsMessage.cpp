#include "RtspOptionsMessage.h"
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include <string>
using namespace std;
CRtspOptionsMessage::CRtspOptionsMessage()
{
    m_unRtspMethod = RTSP_METHOD_OPTIONS;
}

CRtspOptionsMessage::~CRtspOptionsMessage()
{

}

// 封装消息
int CRtspOptionsMessage::encodeMessage(std::string &strMessage)
{
    strMessage.clear();
    // 封装RTSP URL
    if (RET_OK != encodeUrl(strMessage))
    {
        printf("encode rtsp options message url fail.\n");
        return RET_FAIL;
    }
    // CSeq:
    encodeCSeq(strMessage);
    // Require: 先填上
    strMessage += RTSP_TOKEN_STR_REQUIRE;
    strMessage += "implicit-play";
    strMessage += RTSP_END_TAG;
    // Proxy-Require:
    strMessage += RTSP_TOKEN_STR_PROXY;
    strMessage += "gzipped-messages";
    strMessage += RTSP_END_TAG;
    // Content-Length:0
    strMessage += RTSP_TOKEN_STR_GET;
    strMessage += "time and timetick";
    strMessage += RTSP_END_TAG;
    // 结束符
    strMessage += RTSP_END_TAG;
    return RET_OK;
}


// 解析消息
int CRtspOptionsMessage::decodeMessage(const char* pszMessage,const int nMsgLen)
{
	int iRet = CRtspMessage::decodeMessage(pszMessage,(unsigned int)nMsgLen);
	if (iRet)
	{
		 IVS_DBG_LOG("decode Rtsp options msg fail.\n");
		return iRet;
	}

    // 解析CSeq
    string strRtspMsg = pszMessage;
    string::size_type nPos = strRtspMsg.find(RTSP_TOKEN_STR_CSEQ);
    if(string::npos == nPos)
    {
        return RET_FAIL;
    }

    // 获取CSeq值
    string strValue = "";
    strValue = strRtspMsg.substr(nPos + strlen(RTSP_TOKEN_STR_CSEQ));
    nPos = strValue.find(RTSP_END_TAG);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    string strCSeq = "";
    strCSeq = strValue.substr(0, nPos);
    unsigned int unCSeq = strtoul(strCSeq.c_str(), NULL, 0);
    setCSeq(unCSeq);

    // 解析Content-Length:，解析失败，回响应
    nPos = strRtspMsg.find(RTSP_TOKEN_STR_CONTENT_LENGTH);
    if(string::npos == nPos)
    {
        return RET_FAIL;
    }
    strValue = strRtspMsg.substr(nPos + strlen(RTSP_TOKEN_STR_CONTENT_LENGTH));
    nPos = strValue.find(RTSP_END_TAG);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    string strContentLen = "";
    strContentLen = strValue.substr(0, nPos);
    unsigned int unContentLen = strtoul(strContentLen.c_str(), NULL, 0);
    if (0 == unContentLen)
    {
        IVS_DBG_LOG("decode announce message success, Content Length = 0");
        return RET_OK;
    }

    // 获取Content-Length:字段
    nPos = strRtspMsg.find(RTSP_MSG_END_TAG);
    m_strTimeinfo = strRtspMsg.substr(nPos + strlen(RTSP_MSG_END_TAG), unContentLen);
    IVS_DBG_LOG("get content result:%s.", m_strTimeinfo.c_str());
    return RET_OK;
}


// 解析Data
int CRtspOptionsMessage::parseData(const std::string &strRtspMessage)
{
    std::string::size_type nPos = strRtspMessage.find(RTSP_TOKEN_STR_DATE);
    if(std::string::npos == nPos)
    {
        return RET_FAIL;
    }
    std::string strValue = "";
    strValue = strRtspMessage.substr(nPos + strlen(RTSP_TOKEN_STR_DATE));
    nPos = strValue.find(RTSP_END_TAG);
    if(std::string::npos == nPos)
    {
        return RET_FAIL;
    }
    m_strDate = strValue.substr(0, nPos);
    return RET_OK;
}
