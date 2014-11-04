#include "MulticastSource.h"
// #include <errno.h>
#include "ChannelInfo.h"
#include "Connection.h"
#include "ivs_log.h"
//#include "ws2ipdef.h"

CMulticastSource::CMulticastSource(void)
{
    m_bMulticast = true;
}

CMulticastSource::~CMulticastSource(void)
{
}

int CMulticastSource::Init(CChannelInfo* pChannelInfo)
{
    IVS_LOG(IVS_LOG_DEBUG, "Multicast Source", "Multicast Source Initial begin.");

    int iRet = CUdpSource::Init(pChannelInfo);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Multicast Source",
            "Multicast Source Initial error,initial base Udp Source fail.");
        return iRet;
    }

    iRet = InitMulticast();

    if (IVS_SUCCEED != iRet)
    {
        ShutDownSocket();
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Multicast Source", "Multicast Source Initial End.");
    return IVS_SUCCEED;
}


int CMulticastSource::InitMulticast()
{
    int iRet = IVS_FAIL;
    if (NULL == m_pChannelInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "m_pChannelInfo = NULL.");
        return iRet;
    }

    // 连接socket;
    std::string strRemoteSendStreamIp;
    iRet = m_pChannelInfo->GetStreamIP(REMOTE_SEND_STREAM_IP, strRemoteSendStreamIp);
    if (strRemoteSendStreamIp.empty() || IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "UDP GetStreamIP failed. strRemoteSendStreamIp is empty");
        return IVS_PARA_INVALID;
    }

    std::string strLocalRecvStreamIp;
    iRet = m_pChannelInfo->GetStreamIP(LOCAL_RECV_STREAM_IP, strLocalRecvStreamIp);
    if (strLocalRecvStreamIp.empty() || IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "Multicast GetStreamIP failed. strLocalRecvStreamIp is empty");
        return IVS_PARA_INVALID;
    }

    for (int i = 0; i < SOCKET_COUNT; i++)
    {
		// 适配仅分配视频端口，未分配音频端口的问题;
		if ((AUDIO_RTCP_SOCKET == i || AUDIO_RTP_SOCKET == i) && m_pChannelInfo->IsOnlyVideo())
		{
			IVS_LOG(IVS_LOG_ERR, "Init Multicast", "Udp source handle socket Index[%d] Only video port,do not set audio port for multicast", i);
			continue;
		}

        CConnection* pConn = m_pChannelInfo->GetConnection((SOCKET_TYPE)i);
        if (NULL == pConn)
        {
            iRet = IVS_FAIL;
            IVS_LOG(IVS_LOG_ERR, "Init", "Get connection from ChannelInfo failed, pConn = NULL ,socket index:%d", i);
            break;
        }

        unsigned int uiServerPort = 0;
        iRet = m_pChannelInfo->GetRemotePort((PORT_TYPE)i, uiServerPort);

        iRet = pConn->InitMulticast(strRemoteSendStreamIp.c_str(), uiServerPort,
                                    strLocalRecvStreamIp.c_str());

        if (IVS_SUCCEED != iRet)
        {
            ShutDownSocket();
            IVS_LOG(IVS_LOG_ERR, "Init", "Init multicast failed ,socket index:%d", i);
            break;
        }
    }

    return iRet;
}
