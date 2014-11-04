/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspSetupMessage.cpp
  作    者 : y00182336
  功能描述 : RTSP连接
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include <sstream>
#include "ace_header.h"
#include "Rtsp_Log.h"
#include "RtspSetupMessage.h"
#include "RtspMessage.h"
#include "RtspRetCode.h"
#include "rtsp_client_datatype.h"

using namespace std;

/***************************************************************************
 函 数 名 : CRtspSetupMessage
 功能描述 : 构造函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSetupMessage::CRtspSetupMessage()
{
    m_unRtspMethod      = RTSP_METHOD_SETUP;
    m_unTransType       = TRANS_TYPE_UDP;
    m_usClientVideoPort = 0;
    m_usClientAudioPort = 0;
    m_unDestIp          = 0;
    m_usServerVideoPort = 0;
    m_usServerAudioProt = 0;
    m_unSrcIp           = 0;

    m_bTcpFlag          = false;
    m_bIsNoAudio        = false;
    //m_stPeerAddr        = NULL;
}


/***************************************************************************
 函 数 名 : ~CRtspSetupMessage
 功能描述 : 析构函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSetupMessage::~CRtspSetupMessage()
{
    //m_stPeerAddr        = NULL;
}


/***************************************************************************
 函 数 名 : encodeMessage
 功能描述 : 组装RTSP SETUP消息
 输入参数 : 无
 输出参数 : strMessage              封装后的字符串
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSetupMessage::encodeMessage(std::string &strMessage)
{
    strMessage.clear();
    if (RET_OK != encodeUrl(strMessage))
    {
        return RET_FAIL;
    }
    // 组装Transport
    encodeTransport(strMessage);
    encodeCSeq(strMessage);
    strMessage += RTSP_END_TAG; // 消息结束
    return RET_OK;
}



/***************************************************************************
 函 数 名 : encodeTransport
 功能描述 : 组装RTSP SETUP消息的Transport部分
 输入参数 : 无
 输出参数 : strMessage              封装后的字符串
 返 回 值 : 无
***************************************************************************/
void CRtspSetupMessage::encodeTransport(std::string &strMessage)
{
    // Transport
    strMessage += RTSP_TOKEN_STR_TRANSPORT;
    //   打包类型(RTP)
    strMessage += RTSP_TRANSPORT_RTP;
    strMessage += RTSP_TRANSPORT_SPEC_SPLITER;
    strMessage += RTSP_TRANSPORT_PROFILE_AVP;
    //  传输类型(TCP/UDP)
    if (TRANS_TYPE_TCP == m_unTransType)
    {
        strMessage += RTSP_TRANSPORT_SPEC_SPLITER;
        strMessage += RTSP_TRANSPORT_TCP;
    }
    strMessage += SIGN_SEMICOLON;
    // 如果是TCP，需要交织
    if (TRANS_TYPE_TCP == m_unTransType)
    {
        // TCP传输，需要交织通道号interleaved=:
        strMessage += RTSP_TRANSPORT_INTERLEAVED;
        stringstream strChannelNo;
        strChannelNo << RTSP_INTER_LEAVE_NUM;
        strMessage += strChannelNo.str() + SIGN_H_LINE;

        strChannelNo.str("");
        strChannelNo << (RTSP_INTER_LEAVE_NUM + 1); //lint !e835
        strMessage += strChannelNo.str();
        if (0 != m_unDestIp)
        {
            strMessage += SIGN_SEMICOLON;
            strMessage += RTSP_TRANSPORT_UNICAST;
            strMessage += SIGN_SEMICOLON;
            ACE_INET_Addr addr;
            addr.set(m_usClientVideoPort, m_unDestIp);
            strMessage += RTSP_TRANSPORT_DESTINATIION;

			const char* pszHostAddr = addr.get_host_addr();
            strMessage += ((NULL == pszHostAddr) ? "" : pszHostAddr);
            strMessage += SIGN_SEMICOLON;
            stringstream strPort;
            strPort << (unsigned int)m_usClientVideoPort;
            strMessage += RTSP_TRANSPORT_CLIENT_PORT;
            strMessage += strPort.str();
        }
    }
    else
    {
        // 如果是UDP，需要客户端和服务端的地址
        strMessage += RTSP_TRANSPORT_UNICAST;
        strMessage += SIGN_SEMICOLON;

        ACE_INET_Addr addr;
        addr.set(m_usClientVideoPort, m_unDestIp);
        // destination参数
        strMessage += RTSP_TRANSPORT_DESTINATIION;
		const char* pszHostAddr = addr.get_host_addr();
        strMessage += ((NULL == pszHostAddr) ? "" : pszHostAddr);
        strMessage += SIGN_SEMICOLON;
        // client_port参数
        strMessage += RTSP_TRANSPORT_CLIENT_PORT;
        stringstream strPort;
        strPort << (unsigned int)m_usClientVideoPort;
        strMessage += strPort.str() + SIGN_H_LINE;
        strPort.str("");
        strPort << (unsigned int)(m_usClientVideoPort + 1);
        strMessage += strPort.str();
        strPort.str("");
        strPort << (unsigned int)m_usClientAudioPort;
        strMessage += SIGN_COMMA;
        strMessage += strPort.str() + SIGN_H_LINE;
        strPort.str("");
        strPort << (unsigned int)(m_usClientAudioPort + 1);
        strMessage += strPort.str();
        if (0 == m_usClientAudioPort)
        {
            m_bIsNoAudio = true;
        }

    }
    strMessage += RTSP_END_TAG;         // Transport结束
}


/***************************************************************************
 函 数 名 : decodeMessage
 功能描述 : 解析RTSP SETUP响应消息
 输入参数 : pszMessage              消息字符串
            nMsgLen                 消息长度
 输出参数 : 无
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSetupMessage::decodeMessage(const char* pszMessage, const unsigned int nMsgLen)
{
	int iRet = CRtspMessage::decodeMessage(pszMessage, nMsgLen);
    if (iRet)
    {
        IVS_RUN_LOG_CRI("decode Rtsp Setup messsage fail.");
        return iRet;
    }
    string strRtspMsg = "";
    strRtspMsg.assign(pszMessage, nMsgLen);
    // 如果是UDP方式，需要解析Transport
    if (TRANS_TYPE_UDP == m_unTransType)
    {
        if (RET_OK != parseUdpTransParam(strRtspMsg))
        {
            return RET_FAIL;
        }
    }
    else
    {
        if (RET_OK != parseTcpTransParam(strRtspMsg))
        {
            return RET_FAIL;
        }
    }
    return RET_OK;
}



/***************************************************************************
 函 数 名 : parseUdpTransParam
 功能描述 : 解析RTSP SETUP响应Transport
 输入参数 : strRtspMessage          消息字符串
 输出参数 : 无
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSetupMessage::parseUdpTransParam(const std::string &strRtspMessage)
{
    // 解析source.
    string::size_type nPos = strRtspMessage.find(RTSP_TRANSPORT_SOURCE);
    if(string::npos == nPos)
    {
        return RET_FAIL;
    }
    // ip还有";",则取此区间的IP;
    string strValue = "";
    strValue = strRtspMessage.substr(nPos + strlen(RTSP_TRANSPORT_SOURCE));
    nPos = strValue.find(SIGN_SEMICOLON);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    string strIP = strValue.substr(0, nPos);
    m_unSrcIp = ACE_OS::inet_addr(strIP.c_str());
    //m_unSrcIp = ACE_NTOHL(m_unSrcIp);

    // 解析server_port
    nPos = strRtspMessage.find(RTSP_TRANSPORT_SERVER_PORT);
    if(string::npos == nPos)
    {
        return RET_FAIL;
    }
    // port还有"-",则取此区间的port;
    strValue.clear();
    strValue = strRtspMessage.substr(nPos + strlen(RTSP_TRANSPORT_SERVER_PORT));
    nPos = strValue.find(SIGN_H_LINE);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    string strPort = strValue.substr(0, nPos);
    m_usServerVideoPort = (unsigned short) atoi(strPort.c_str());

    if (!m_bIsNoAudio)
    {
        nPos = strValue.find(SIGN_COMMA);
        if(string::npos == nPos)
        {
            return RET_FAIL;
        }
        // port还有"-",则取此区间的port;
        string strPorts = "";
        strPorts = strValue.substr(nPos + strlen(SIGN_COMMA));
        nPos = strPorts.find(SIGN_H_LINE);
        if (string::npos == nPos)
        {
            return RET_FAIL;
        }
        strPort = strPorts.substr(0, nPos);
        m_usServerAudioProt = (unsigned short) atoi(strPort.c_str());
    }

    IVS_DBG_LOG("decode transport success, serverIP[%s], Video Port[%d], Audio Port[%d].", 
                                strIP.c_str(), 
                                m_usServerVideoPort,
                                m_usServerAudioProt);
    return RET_OK;
}


/***************************************************************************
 函 数 名 : parseTcpTransParam
 功能描述 : 解析RTSP SETUP响应Transport
 输入参数 : strRtspMessage          消息字符串
 输出参数 : 无
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSetupMessage::parseTcpTransParam(const std::string &strRtspMessage)
{
    // 解析source.
    string::size_type nPos = strRtspMessage.find(RTSP_TRANSPORT_SOURCE);
    if(string::npos == nPos)
    {
        // 不是硬解
        return RET_OK;
    }
    string strValue = "";
    strValue = strRtspMessage.substr(nPos + strlen(RTSP_TRANSPORT_SOURCE));
    nPos = strValue.find(SIGN_SEMICOLON);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    string strIP = strValue.substr(0, nPos);
    m_unSrcIp = ACE_OS::inet_addr(strIP.c_str());
    // 解析server_port
    nPos = strRtspMessage.find(RTSP_TRANSPORT_SERVER_PORT);
    if(string::npos == nPos)
    {
        return RET_FAIL;
    }
    // port还有"-",则取此区间的port;
    strValue.clear();
    strValue = strRtspMessage.substr(nPos + strlen(RTSP_TRANSPORT_SERVER_PORT));
    nPos = strValue.find(RTSP_END_TAG);
    if (string::npos == nPos)
    {
        return RET_FAIL;
    }
    string strPort = strValue.substr(0, nPos);
    m_usServerVideoPort = (unsigned short) atoi(strPort.c_str());

    return RET_OK;
}


/***************************************************************************
 函 数 名 : setTransType
 功能描述 : 设置传输模式
 输入参数 : unTransType             传输模式
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSetupMessage::setTransType(unsigned int unTransType)
{
    m_unTransType = unTransType;
    return;
}



/***************************************************************************
 函 数 名 : setTransType
 功能描述 : 设置接收端口
 输入参数 : unTransType                 接收端口号
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSetupMessage::setClientPort(const unsigned short usVedioPort,
                                      const unsigned short usAudioPort)
{
    m_usClientVideoPort = usVedioPort;
    m_usClientAudioPort = usAudioPort;
    return;
}



/***************************************************************************
 函 数 名 : setDestinationIp
 功能描述 : 设置接收地址
 输入参数 : unIp                    接收地址
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSetupMessage::setDestinationIp(unsigned int unIp)
{
    m_unDestIp = unIp;
    return;
}


/***************************************************************************
 函 数 名 : getServerIp
 功能描述 : 获取发送地址
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 发送地址
***************************************************************************/
unsigned int CRtspSetupMessage::getServerIp() const
{
    return m_unSrcIp;
}



/***************************************************************************
 函 数 名 : getServerVideoPort
 功能描述 : 获取视频发送端口
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 发送端口
***************************************************************************/
unsigned short CRtspSetupMessage::getServerVideoPort()const
{
    return m_usServerVideoPort;
}


/***************************************************************************
 函 数 名 : getServerAudioPort
 功能描述 : 获取音频发送端口
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 发送端口
***************************************************************************/
unsigned short CRtspSetupMessage::getServerAudioPort()const
{
    return m_usServerAudioProt;
}



