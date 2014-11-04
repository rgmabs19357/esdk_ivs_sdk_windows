/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspMessage.cpp
  作    者 : y00182336
  功能描述 : RTSP连接
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include <sstream>
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include "RtspMessage.h"
using namespace std;

const char* CRtspMessage::m_strRtspMethod[] = RTSP_METHOD_STRING;
const char* CRtspMessage::m_RtspCode[]      = RTSP_CODE_STRING;

/***************************************************************************
 函 数 名 : CRtspMessage
 功能描述 : 构造函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspMessage::CRtspMessage()
{
    m_unRtspMethod = RTSP_METHOD_NUM;
    m_unCSeq       = 0;
    m_strUrl       = string("");
}

/***************************************************************************
 函 数 名 : ~CRtspMessage
 功能描述 : 析构函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspMessage::~CRtspMessage()
{

}



/***************************************************************************
 函 数 名 : encodeUrl
 功能描述 : 封装RTSP URL
 输入参数 : 无
 输出参数 : strMessage              封装后的字符串
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspMessage::encodeUrl(std::string &strMessage)
{
    if (RTSP_METHOD_NUM <= getMethodType())
    {
        return RET_FAIL;
    }
    strMessage += m_strRtspMethod[getMethodType()];
    strMessage += " " + m_strUrl + " " + RTSP_PROTOCOL_VERSION;
    strMessage += RTSP_END_TAG;
    return RET_OK;
} //lint !e1762


/***************************************************************************
 函 数 名 : encodeCSeq
 功能描述 : 封装RTSP系列号
 输入参数 : 无
 输出参数 : strMessage              封装后的字符串
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
void CRtspMessage::encodeCSeq(std::string &strMessage)
{
    std::stringstream strCSeq;
    strCSeq<<m_unCSeq;
    strMessage += RTSP_TOKEN_STR_CSEQ;
    strMessage += strCSeq.str();
    strMessage += RTSP_END_TAG;
	strCSeq.str("");//需重置，否则stringstream会泄漏内存;
}

/***************************************************************************
 函 数 名 : getMethodType
 功能描述 : 获取RTSP消息类型
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 消息类型
***************************************************************************/
unsigned int CRtspMessage::getMethodType() const
{
    return m_unRtspMethod;
}

/***************************************************************************
 函 数 名 : getMethodType
 功能描述 : 设置RTSP序列号
 输入参数 : unCSeq                  序列号
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspMessage::setCSeq(unsigned int unCSeq)
{
//    ACE_Guard <ACE_Thread_Mutex> locker (m_CSeqMutex);
    m_unCSeq = unCSeq;
}


/***************************************************************************
 函 数 名 : getMethodType
 功能描述 : 获取消息序列号
 输入参数 : unCSeq                  序列号
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
unsigned int CRtspMessage::getCSeq() const
{
    return m_unCSeq;
}



/***************************************************************************
 函 数 名 : setRtspUrl
 功能描述 : 设置URL
 输入参数 : unCSeq                  RTSP URL
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspMessage::setRtspUrl(const std::string &strUrl)
{
    m_strUrl = strUrl;
    return;
}



/***************************************************************************
 函 数 名 : isRtspRespMsg
 功能描述 : 判断是否是RTSP消息，并检查CSeq值
 输入参数 : psRtspMsg               RTSP消息字符串
            nLen                    消息长度
 输出参数 : 无
 返 回 值 : true-是             false-否
***************************************************************************/
bool CRtspMessage::isRtspRespMsg(const char* psRtspMsg, const unsigned int unLen)
{
    // 检查是否是以RTSP/1.0开头的响应消息
    if((NULL == psRtspMsg) 
        || (0 != strncmp(psRtspMsg, RTSP_PROTOCOL_VERSION, strlen(RTSP_PROTOCOL_VERSION))))
    {
        return false;
    }
    
    // 定位"CSeq: "字段
    const char* pCSeq = strstr(psRtspMsg, RTSP_TOKEN_STR_CSEQ);
    if (NULL == pCSeq)
    {
        return false;
    }
    unsigned int unCSeq = 0;
    // 取出"CSeq: "后的数字，检查与发送CSeq是否一致
	(void)sscanf_s(pCSeq, RTSP_TOKEN_STR_CSEQ "%u", &unCSeq);
    if (unCSeq != getCSeq())
    {
        IVS_DBG_LOG("CSeq wrong, message CSeq[%u].", unCSeq);
        return false;
    }
    return true;
} //lint !e1762



/***************************************************************************
 函 数 名 : decodeMessage
 功能描述 : 解析响应消息,此处只解析200 ok
 输入参数 : psRtspMsg               RTSP消息字符串
            nLen                    消息长度
 输出参数 : 无
 返 回 值 : RET_OK-成功             RET_FAIL-失败
***************************************************************************/
int CRtspMessage::decodeMessage(const char* pszMessage, const unsigned int unMsgLen)
{
    if((NULL == pszMessage) || (0 == unMsgLen))
    {
        return RET_FAIL;
    }
    std::string strRtspMsg = "";
    strRtspMsg.append(pszMessage, unMsgLen);
    // 取出响应结果
    std::string strValue = "";
    strValue = strRtspMsg.substr(strlen(RTSP_PROTOCOL_VERSION));
    string::size_type nPos = strValue.find(RTSP_END_TAG);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    std::string strRet = "";
    strRet = strValue.substr(0, nPos);
    trimString(strRet);
    unsigned int i = 0;
    for(; i < RTSP_STATUS_CODES_BUTT; i++)
    {
        if(0 == strncmp(m_RtspCode[i], strRet.c_str(), strRet.length()))
        {
            break;
        }
    }
    // 解析结果非200 OK返回失败
    if (RTSP_SUCCESS_OK != i)
    {
        return i;
    }
    return RET_OK;

}

/***************************************************************************
 函 数 名 : trimString
 功能描述 : 删除字符串前后的空字符
 输入参数 : srcString               字符串
 输出参数 : srcString
 返 回 值 : 无
***************************************************************************/
void CRtspMessage::trimString(std::string& srcString) const
{
    string::size_type pos = srcString.find_last_not_of(' ');
    if (pos != string::npos)
    {
        (void) srcString.erase(pos + 1);
        pos = srcString.find_first_not_of(' ');
        if (pos != string::npos)
            (void) srcString.erase(0, pos);
    }
    else
        (void) srcString.erase(srcString.begin(), srcString.end());
    return;
}


