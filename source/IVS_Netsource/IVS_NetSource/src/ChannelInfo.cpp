#include "ChannelInfo.h"
#include "NetSource.h"
#include "UdpSource.h"
#include "Connection.h"
#include "IVSCommon.h"
#include "ivs_log.h"

CChannelInfo::CChannelInfo()
    : m_uiLocalVideoRtpPort(0)
    , m_uiLocalVideoRtcpPort(0)
    , m_uiLocalAudioRtpPort(0)
    , m_uiLocalAudioRtcpPort(0)
    , m_uiRemoteVideoRtpPort(0)
    , m_uiRemoteVideoRtcpPort(0)
    , m_uiRemoteAudioRtpPort(0)
    , m_uiRemoteAudioRtcpPort(0)
    , m_enProtocolType(PROTOCOL_RTP_OVER_UDP)
    , m_ulChannel(0)
    , m_cbDataCallBack(NULL)
    , m_pDataUserData(NULL)
    , m_cbExceptionCallBack(NULL)
    , m_pConnVideoRtp(NULL)
    , m_pConnVideoRtcp(NULL)
    , m_pConnAudioRtp(NULL)
    , m_pConnAudioRtcp(NULL)
    , m_pConnMulticast(NULL)
	, m_bIsMuilt(false)
	, m_bIsOnlyVideo(false)
    , m_strSessionUrl("")
    , m_strSessionId("")
{
}

CChannelInfo::~CChannelInfo(void)
{
    try
    {
        Clear();
    }
    catch (...)
    {
    }
}

void CChannelInfo::SetConnection(SOCKET_TYPE enSocketType, CConnection* pConnection)
{
    if (NULL == pConnection)
    {
        IVS_LOG(IVS_LOG_ERR, "SetConnection", "Input param error, pConnection = NULL");
        return;
    }

    switch (enSocketType)
    {
    case VIDEO_RTP_SOCKET:
        m_pConnVideoRtp = pConnection;
        break;
    case VIDEO_RTCP_SOCKET:
        m_pConnVideoRtcp = pConnection;
        break;
    case AUDIO_RTP_SOCKET:
        m_pConnAudioRtp = pConnection;
        break;
    case AUDIO_RTCP_SOCKET:
        m_pConnAudioRtcp = pConnection;
        break;
    case MULTICAST_SOCKET:
        m_pConnMulticast = pConnection;
        break;
    default:
        IVS_LOG(IVS_LOG_ERR, "SetSocket", "input param of socket type error, Socket Type[%d].", enSocketType); 
        break;
    }
}

CConnection* CChannelInfo::GetConnection(SOCKET_TYPE enSocketType)
{
    CConnection* pConnection = NULL;
    switch(enSocketType)
    {
    case VIDEO_RTP_SOCKET:
        pConnection = m_pConnVideoRtp;
        break;
    case VIDEO_RTCP_SOCKET:
        pConnection = m_pConnVideoRtcp;
        break;
    case AUDIO_RTP_SOCKET:
        pConnection = m_pConnAudioRtp;
        break;
    case AUDIO_RTCP_SOCKET:
        pConnection = m_pConnAudioRtcp;
        break;
    case MULTICAST_SOCKET:
        pConnection = m_pConnMulticast;
        break;
    default:
        IVS_LOG(IVS_LOG_ERR, "GetSocket", "input param of socket type error."); 
        break;
    }

    return pConnection;
}

SOCKET CChannelInfo::GetSocket(SOCKET_TYPE enSocketType)
{
    CConnection* pConnection = GetConnection(enSocketType);
    if (NULL == pConnection)
    {
        return INVALID_SOCKET;
    }

    return pConnection->GetSocket();
}

void CChannelInfo::SetLocalPort(PORT_TYPE enPortType, unsigned int uiPort)
{
    switch (enPortType)
    {
    case VIDEO_RTP_PORT:
        m_uiLocalVideoRtpPort = uiPort;
        break;
    case VIDEO_RTCP_PORT:
        m_uiLocalVideoRtcpPort = uiPort;
        break;
    case AUDIO_RTP_PORT:
        m_uiLocalAudioRtpPort = uiPort;
        break;
    case AUDIO_RTCP_PORT:
        m_uiLocalAudioRtcpPort = uiPort;
        break;
    case MULTICAST_PORT:
        // TODO 后续迭代增加广播的时候再扩展;
        break;
    default:
        IVS_LOG(IVS_LOG_ERR, "SetLocalPort", "input param of port type error."); 
        break;
    }
}

int CChannelInfo::GetLocalPort(PORT_TYPE enPortType, unsigned int& uiPort)
{
    switch(enPortType)
    {
    case VIDEO_RTP_PORT:
        uiPort = m_uiLocalVideoRtpPort;
        break;
    case VIDEO_RTCP_PORT:
        uiPort = m_uiLocalVideoRtcpPort;
        break;
    case AUDIO_RTP_PORT:
        uiPort = m_uiLocalAudioRtpPort;
        break;
    case AUDIO_RTCP_PORT:
        uiPort = m_uiLocalAudioRtcpPort;
        break;
    case MULTICAST_PORT:
        IVS_LOG(IVS_LOG_ERR, "GetLocalPort", "Multicast port do not need."); 
        break;
    default:
        IVS_LOG(IVS_LOG_ERR, "GetLocalPort", "input param of port type error."); 
        break;
    }

    return IVS_SUCCEED;
}

void CChannelInfo::SetRemotePort(PORT_TYPE enPortType, unsigned int uiPort)
{
    switch (enPortType)
    {
    case VIDEO_RTP_PORT:
        m_uiRemoteVideoRtpPort = uiPort;
        break;
    case VIDEO_RTCP_PORT:
        m_uiRemoteVideoRtcpPort = uiPort;
        break;
    case AUDIO_RTP_PORT:
        m_uiRemoteAudioRtpPort = uiPort;
        break;
    case AUDIO_RTCP_PORT:
        m_uiRemoteAudioRtcpPort = uiPort;
        break;
    case MULTICAST_PORT:
        // TODO
        break;
    default:
        IVS_LOG(IVS_LOG_ERR, "SetRemotePort", "input param of port type error."); 
        break;
    }
}

int CChannelInfo::GetRemotePort(PORT_TYPE enPortType, unsigned int& uiPort)
{
    int iRet = IVS_SUCCEED;
    switch(enPortType)
    {
    case VIDEO_RTP_PORT:
        uiPort = m_uiRemoteVideoRtpPort;
        break;
    case VIDEO_RTCP_PORT:
        uiPort = m_uiRemoteVideoRtcpPort;
        break;
    case AUDIO_RTP_PORT:
        uiPort = m_uiRemoteAudioRtpPort;
        break;
    case AUDIO_RTCP_PORT:
        uiPort = m_uiRemoteAudioRtcpPort;
        break;
    case MULTICAST_PORT:
        // TODO
        break;
    default:
        IVS_LOG(IVS_LOG_ERR, "GetRemotePort", "input param of port type error.");  
        iRet = IVS_FAIL;
        break;
    }

    return iRet;
}

int CChannelInfo::SetStreamIP(STREAM_IP_TYPE enStreamIPType, const char* pszIP)
{
    if (NULL == pszIP)
    {
        IVS_LOG(IVS_LOG_ERR, "SetStreamIP", "Input Param error, pszIP = NULL.");
        return IVS_PARA_INVALID;
    }

	if (INADDR_NONE == inet_addr(pszIP))
	{
		IVS_LOG(IVS_LOG_ERR, "SetStreamIP", "Input Param error, Transform ip[%s] error.", pszIP);
		return IVS_PARA_INVALID;
	}

    int iRet = IVS_SUCCEED;
    switch (enStreamIPType)
    {
    case LOCAL_RECV_STREAM_IP:
        m_strLocalRecvStreamIP = pszIP;
        break;
    case REMOTE_SEND_STREAM_IP:
        m_strRemoteSendStreamIp = pszIP;
        break;
    default:
        iRet = IVS_FAIL;
        break;
    }
    return iRet;
}

int CChannelInfo::GetStreamIP(STREAM_IP_TYPE enStreamIPType, std::string& strStreamIP)
{
    int iRet = IVS_SUCCEED;

    switch(enStreamIPType)
    {
    case LOCAL_RECV_STREAM_IP:
        strStreamIP = m_strLocalRecvStreamIP;
        break;
    case REMOTE_SEND_STREAM_IP:
        strStreamIP = m_strRemoteSendStreamIp;
        break;
    default:
        {
            iRet = IVS_FAIL;
            IVS_LOG(IVS_LOG_ERR, "GetStreamIP", "input param of stream ip type error.");  
        }
        break;
    }
    return iRet;
}

void CChannelInfo::SetDataCallBack(NET_DATA_CALLBACK cbDataCallBack, void* pUser)
{
    m_cbDataCallBack = cbDataCallBack;
    m_pDataUserData = pUser;
}

void CChannelInfo::GetDataCallBack(NET_DATA_CALLBACK& cbDataCallBack, void*& pUser)
{
    cbDataCallBack = m_cbDataCallBack;
    pUser = m_pDataUserData;
}

void CChannelInfo::SetExceptionCallBack(NET_EXCEPTION_CALLBACK cbExceptionCallBack)
{
    m_cbExceptionCallBack = cbExceptionCallBack;
}

void CChannelInfo::GetExceptionCallBack(NET_EXCEPTION_CALLBACK& cbExceptionCallBack)
{
    cbExceptionCallBack = m_cbExceptionCallBack;
}

void CChannelInfo::Clear()
{
    if (NULL != m_pConnVideoRtp)
    {
        m_pConnVideoRtp->ShutDown();
        IVS_DELETE(m_pConnVideoRtp);
    }

    if (NULL != m_pConnVideoRtcp)
    {
        m_pConnVideoRtcp->ShutDown();
        IVS_DELETE(m_pConnVideoRtcp);
    }

    if (NULL != m_pConnAudioRtp)
    {
        m_pConnAudioRtp->ShutDown();
        IVS_DELETE(m_pConnAudioRtp);
    }

    if (NULL != m_pConnAudioRtcp)
    {
        m_pConnAudioRtcp->ShutDown();
        IVS_DELETE(m_pConnAudioRtcp);
    }

    if (NULL != m_pConnMulticast)
    {
        m_pConnMulticast->ShutDown();
        IVS_DELETE(m_pConnMulticast);
    }
    
    m_uiLocalVideoRtpPort = 0;
    m_uiLocalVideoRtcpPort = 0;
    m_uiLocalAudioRtpPort = 0;
    m_uiLocalAudioRtcpPort = 0;
    m_uiRemoteVideoRtpPort = 0;
    m_uiRemoteVideoRtcpPort = 0;
    m_uiRemoteAudioRtpPort = 0;
    m_uiRemoteAudioRtcpPort = 0;
    m_strRemoteSendStreamIp = "";
    m_strLocalRecvStreamIP = "";
    m_enProtocolType = PROTOCOL_RTP_OVER_UDP;
    m_ulChannel = 0;

    m_cbDataCallBack = NULL;
    m_pDataUserData = NULL;
    m_cbExceptionCallBack = NULL;
	m_bIsMuilt = false;
	m_bIsOnlyVideo = false;
}

void CChannelInfo::SetSessionInfo(const char* pSessionUrl, const char* pSessionId)  //lint !e830
{
    //IVS_LOG(IVS_LOG_INFO, "SetSessionInfo", "pSessionUrl[%s], pSessionId[%s].", pSessionUrl, pSessionId); 
    m_strSessionUrl = pSessionUrl;
    m_strSessionId = pSessionId;
}

std::string CChannelInfo::GetSessionUrl()
{
    //IVS_LOG(IVS_LOG_INFO, "GetSessionUrl", "pSessionUrl[%s].", m_strSessionUrl.c_str()); 
    return m_strSessionUrl;      //lint !e1036
}

std::string CChannelInfo::GetSessionId()
{
    //IVS_LOG(IVS_LOG_INFO, "GetSessionId", "pSessionId[%s].", m_strSessionId.c_str()); 
    return m_strSessionId;       //lint !e1036
}
