#include "NetChannel.h"
#include "NetSource.h"
#include "UdpSource.h"
#include "TcpSource.h"
#include "MulticastSource.h"
#include "IVSCommon.h"
#include "Connection.h"
#include "ivs_log.h"
#include "NetChannelMgr.h"

static const char* G_MULTI_CAST_IP_BEGIN = "224.0.0.0";    // 广播开始地址
static const char* G_MULTI_CAST_IP_END = "239.255.255.255"; // 广播结束地址

CNetChannel::CNetChannel()
	: m_pSource(NULL)
{
    
}

CNetChannel::~CNetChannel(void)
{
    try
    {
        // 清空数据
        m_channelInfo.Clear();
    }
    catch(...)
    {
		IVS_LOG(IVS_LOG_ERR, "~CNetChannel", "Clear m_channelInfo throw exception.");  
    }

	try
	{
		//释放source对象资源;
		if(NULL != m_pSource)
		{
			m_pSource->Release();
			IVS_DELETE(m_pSource);
		}
	}
	catch(...)
	{
		IVS_LOG(IVS_LOG_ERR, "~CNetChannel", "Clear m_pSource throw exception.");  
	}
}

void CNetChannel::SetChannelNo(unsigned int ulChannel)
{ 
    m_channelInfo.SetChannelNo(ulChannel);
}

unsigned int CNetChannel::GetChannelNo()
{ 
    return m_channelInfo.GetChannelNo(); 
}

void CNetChannel::SetProtocolType(IVS_PROTOCOL_TYPE enProtocolType)
{
    m_channelInfo.SetProtocolType(enProtocolType);
}

IVS_PROTOCOL_TYPE CNetChannel::GetProtocolType()
{ 
    return m_channelInfo.GetProtocolType();
}

void CNetChannel::SetConnection(SOCKET_TYPE enSocketType, CConnection* pConnection)
{
    if (NULL == pConnection)
    {
        IVS_LOG(IVS_LOG_ERR, "SetConnection", "CConnection param is null");
        return;
    }

    m_channelInfo.SetConnection(enSocketType, pConnection);
}

CConnection* CNetChannel::GetConnection(SOCKET_TYPE enSocketType)
{
    return m_channelInfo.GetConnection(enSocketType);
}

void CNetChannel::SetLocalPort(PORT_TYPE enPortType, unsigned int uiPort)
{
    m_channelInfo.SetLocalPort(enPortType, uiPort);
}

int CNetChannel::GetLocalPort(PORT_TYPE enPortType, unsigned int& uiPort)
{
    return m_channelInfo.GetLocalPort(enPortType, uiPort);
}

void CNetChannel::SetRemotePort(PORT_TYPE enPortType, unsigned int uiPort)
{
    m_channelInfo.SetRemotePort(enPortType, uiPort);
}

int CNetChannel::GetRemotePort(PORT_TYPE enPortType, unsigned int& uiPort)
{
    return m_channelInfo.GetRemotePort(enPortType, uiPort);
}

int CNetChannel::SetStreamIP(STREAM_IP_TYPE enStreamIPType, const char* pszIP)
{
    return m_channelInfo.SetStreamIP(enStreamIPType, pszIP);
}

int CNetChannel::GetStreamIP(STREAM_IP_TYPE enStreamIPType, std::string& strStreamIP)
{
    return m_channelInfo.GetStreamIP(enStreamIPType, strStreamIP);
}

void CNetChannel::SetDataCallBack(NET_DATA_CALLBACK cbDataCallBack, void* pUser)
{
    m_channelInfo.SetDataCallBack(cbDataCallBack, pUser);
}

int CNetChannel::OpenNetStream(const char* pszRemoteSendStreamIp, 
                               unsigned int uiRemoteSendStreamVideoPort, 
                               unsigned int uiRemoteSendStreamAudioPort,
                               NET_EXCEPTION_CALLBACK cbExceptionCallBack)
{
    int iRet = IVS_PARA_INVALID;
    if (NULL == pszRemoteSendStreamIp)
    {
        IVS_LOG(IVS_LOG_ERR, "OpenNetStream", "Remote send stream ip error");
        return iRet;
    }

	IVS_LOG(IVS_LOG_DEBUG, "OpenNetStream", "Video[%u %u] Audio[%u %u]", uiRemoteSendStreamVideoPort, uiRemoteSendStreamVideoPort+1, uiRemoteSendStreamAudioPort, uiRemoteSendStreamAudioPort+1);

    m_channelInfo.SetStreamIP(REMOTE_SEND_STREAM_IP, pszRemoteSendStreamIp);
    m_channelInfo.SetRemotePort(VIDEO_RTP_PORT, uiRemoteSendStreamVideoPort);
    m_channelInfo.SetRemotePort(VIDEO_RTCP_PORT, uiRemoteSendStreamVideoPort+1);
    m_channelInfo.SetRemotePort(AUDIO_RTP_PORT, uiRemoteSendStreamAudioPort);
    m_channelInfo.SetRemotePort(AUDIO_RTCP_PORT, uiRemoteSendStreamAudioPort+1);
    m_channelInfo.SetExceptionCallBack(cbExceptionCallBack);

    //根据协议类型调用不同的Source
    if (PROTOCOL_RTP_OVER_UDP == m_channelInfo.GetProtocolType())
    {
        std::string strRemoteSendStreamIP = pszRemoteSendStreamIp;
        // 组播;
        if ((strRemoteSendStreamIP.compare(G_MULTI_CAST_IP_BEGIN) > 0) && (strRemoteSendStreamIP.compare(G_MULTI_CAST_IP_END) < 0))
        {
            (void)IVS_NEW((CMulticastSource*&)m_pSource);
			iRet = BindMulticastPort(uiRemoteSendStreamVideoPort, uiRemoteSendStreamAudioPort);
			if (IVS_SUCCEED != iRet)
			{
				IVS_DELETE(m_pSource);
				return iRet;
			}

			m_channelInfo.SetMuilt(true);
        }
        else// 单播;
        {
            (void)IVS_NEW((CUdpSource*&)m_pSource);
        }
    }
    //必定是TCP
    else
    {
        (void)IVS_NEW((CTcpSource*&)m_pSource);
    }

    if (NULL == m_pSource)
    {
        IVS_LOG(IVS_LOG_ERR, "OpenNetStream", "Create Source fail");
        return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    }

    iRet = m_pSource->Init(&m_channelInfo);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "OpenNetStream", "initial source fail. result = %d", iRet);
        IVS_DELETE(m_pSource);
        return iRet;
    }

    return iRet; //lint !e732
}

int CNetChannel::BindMulticastPort(unsigned int uiRemoteSendStreamVideoPort, unsigned int uiRemoteSendStreamAudioPort)
{
	// 组播下，需要重新绑定端口，把原来的本地的端口释放掉，再重新绑定新的端口;

	int iRet = IVS_SUCCEED;
	std::string strLocalIp;
	iRet = m_channelInfo.GetStreamIP(LOCAL_RECV_STREAM_IP, strLocalIp);
    if (IVS_SUCCEED != iRet)
    {
        return iRet;
    }
	for (int i = 0; i < SOCKET_COUNT; i++)
	{
		CConnection* pConn = m_channelInfo.GetConnection((SOCKET_TYPE)i);
		if (NULL == pConn)
		{
			iRet = IVS_FAIL;
			IVS_LOG(IVS_LOG_ERR, "BindMulticastPort", "Get connection from ChannelInfo failed, pConn = NULL ,socket index:%d", i);
			break;
		}
	
		pConn->ShutDown();
		
		switch (i)
		{
		case VIDEO_RTP_PORT:
			iRet += pConn->Bind(strLocalIp.c_str(), uiRemoteSendStreamVideoPort);
			break;
		case VIDEO_RTCP_PORT:
			iRet += pConn->Bind(strLocalIp.c_str(), uiRemoteSendStreamVideoPort + 1);
			break;
		case AUDIO_RTP_PORT:
			iRet += pConn->Bind(strLocalIp.c_str(), uiRemoteSendStreamAudioPort);
			break;
		case AUDIO_RTCP_PORT:
			iRet += pConn->Bind(strLocalIp.c_str(), uiRemoteSendStreamAudioPort + 1);
			break;
		default:
			IVS_LOG(IVS_LOG_ERR, "BindMulticastPort", "In unreachable way.");
			break;
		}
	}

	if (IVS_SUCCEED != iRet)
	{
		for (int i = 0; i < SOCKET_COUNT; i++)
		{
			CConnection* pConn = m_channelInfo.GetConnection((SOCKET_TYPE)i);
			if (NULL != pConn)
			{
				pConn->ShutDown();
			}
		}
		IVS_LOG(IVS_PLAYER_RET_NET_BIND_PORT_FAIL, "BindMulticastPort", "Bind Port failed");
		return IVS_PLAYER_RET_NET_BIND_PORT_FAIL;
	}

	unsigned int uiVideoPort = 0;
	m_channelInfo.GetLocalPort(VIDEO_RTP_PORT, uiVideoPort);
	CNetChannelMgr::instance().FreePortfromList(uiVideoPort);
	unsigned int uiAudioPort = 0;
	m_channelInfo.GetLocalPort(AUDIO_RTP_PORT, uiAudioPort);
	CNetChannelMgr::instance().FreePortfromList(uiAudioPort);

	m_channelInfo.SetLocalPort(VIDEO_RTP_PORT, uiRemoteSendStreamVideoPort);
	m_channelInfo.SetLocalPort(VIDEO_RTCP_PORT, uiRemoteSendStreamVideoPort + 1);
	m_channelInfo.SetLocalPort(AUDIO_RTP_PORT, uiRemoteSendStreamAudioPort);
	m_channelInfo.SetLocalPort(AUDIO_RTCP_PORT, uiRemoteSendStreamAudioPort + 1);

	return IVS_SUCCEED;
}

int CNetChannel::CloseNetStream()
{
    if (NULL == m_pSource)
    {
        IVS_LOG(IVS_LOG_ERR, "CloseNetStream", "source is NULL.");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    //停止收流;
    m_pSource->Stop();

    return IVS_SUCCEED;
}

int CNetChannel::StartRecvStream()
{
    if (NULL == m_pSource)
    {
        IVS_LOG(IVS_LOG_ERR, "StartRecvStream", "source is NULL.");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    m_pSource->Start();
    return IVS_SUCCEED;
}

void CNetChannel::SetSessionInfo(const char* pSessionUrl, const char* pSessionId)   //lint !e830
{
    m_channelInfo.SetSessionInfo(pSessionUrl, pSessionId);
}
