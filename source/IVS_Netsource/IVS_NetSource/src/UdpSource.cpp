#include "UdpSource.h"
#include "ChannelInfo.h"
#include "UdpConnection.h"
#include "ivs_log.h"

CUdpSource::CUdpSource(void)
    : m_bMulticast(false)
{
	ZeroMemory(&m_SenderVideoRTCPSockAddr,sizeof(sockaddr_in));
	ZeroMemory(&m_SenderAudioRTCPSockAddr,sizeof(sockaddr_in));
}


CUdpSource::~CUdpSource(void)
{
}

int CUdpSource::Init(CChannelInfo* pChannelInfo)
{
    int iRet = CNetSource::Init(pChannelInfo);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "Init function of base class failed");
        return iRet;
    }

    // 连接socket;
    std::string strRemoteSendStreamIp;
    iRet = pChannelInfo->GetStreamIP(REMOTE_SEND_STREAM_IP, strRemoteSendStreamIp);
    if (strRemoteSendStreamIp.empty() || IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "UDP GetStreamIP failed. strRemoteSendStreamIp is empty");
        return IVS_PARA_INVALID;
    }

    iRet = ConnSocket(strRemoteSendStreamIp);
    if (IVS_SUCCEED != iRet)
    {
        ShutDownSocket();
        return iRet;
    }
    return iRet;
}

void CUdpSource::ProcessRecevData()
{
    if (NULL == m_pChannelInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcessRecevData", "m_pChannelInfo = NULL.");
        return;
    }

    fd_set  fdread;

	/*
	 * DTS2013072800500
	 * fixed by w00210470, 设置select超时100毫秒
	 */
	struct timeval tv = {0, 100000}; 
	//struct timeval tv = {RECV_MEDIA_DATA_TIMEOUT, 0}; //指定超时秒数，没有数据则认为媒体流中断;
	FD_ZERO(&fdread);

    //将所有句柄加入fdread集合;
    for (int i = 0; i < NET_SOURCE_UDP_SOCKET_NUM; i++)
    {
		// 支持仅有视频端口，没有分配音频端口的情况;
		if ((AUDIO_RTP_SOCKET == i || AUDIO_RTCP_SOCKET == i) && m_pChannelInfo->IsOnlyVideo())
		{
			//IVS_LOG(IVS_LOG_ERR, "Process Recev Data", "Udp source handle socket Index[%d] Only video port, do not set audio socket to fdset", i);
			continue;
		}

        SOCKET socket = m_pChannelInfo->GetSocket((SOCKET_TYPE)i);
        if (INVALID_SOCKET == socket)
        {
            IVS_LOG(IVS_LOG_ERR, "Process Recev Data", "invalid socket, Socket type index:%d.", i);
            return;
        }

        FD_SET(socket, &fdread);
    } 

    int iSelectState = select(0, &fdread, NULL, NULL, &tv);
    // 网络异常;
    if (SOCKET_ERROR == iSelectState)
    {
        IVS_LOG(IVS_LOG_ERR, "Process Recev Data", "Select error, error code:%d", WSAGetLastError());
        return;
    }
    
    // 检查超时
    if (0 == iSelectState)
    {
        IVS_LOG(IVS_LOG_DEBUG, "Process Recev Data", "Select failed, select over time, error code:%d", WSAGetLastError());
        CheckTimeOut();
        return;
    }

    // 正常收流
    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(sockaddr_in));
    int nLen = sizeof(addr);
    for (int i = 0; i < NET_SOURCE_UDP_SOCKET_NUM; i++)
    {
		// 适配仅分配视频端口，未分配音频端口的问题;
		if ((AUDIO_RTCP_SOCKET == i || AUDIO_RTP_SOCKET == i) && m_pChannelInfo->IsOnlyVideo())
		{
			//IVS_LOG(IVS_LOG_ERR, "Process Recev Data", "Udp source handle socket Index[%d] Only video port,do not recevie audio stream", i);
			continue;
		}

        // 此处socket一定不为INVALID_SOCKET
        SOCKET socket = m_pChannelInfo->GetSocket((SOCKET_TYPE)i);
        if (!FD_ISSET(socket, &fdread))
        {
            continue;   
        }

        int len = recvfrom(socket, m_pRecvBuf, RTP_MEDIA_BUF_LEN, 0, (SOCKADDR *)&addr, &nLen);
        if (0 >= len)
        {
            IVS_LOG(IVS_LOG_ERR, "Process Recev Data", "Udp source handle socket Index[%d] receive data failed. error[%d]", i, WSAGetLastError());
			
			/*
			 * 不处理 recvfrom 返回值 -1
			 * 向对端端口发送NET穿越消息， 若对方对口未打开，会向本机发送ICMP端口不可达，
			 * 由于UDP Socket已经bind，recvfrom 会返回 -1
			 */
#if 0
            // 视频端口收流异常，反馈给上层;
            if (VIDEO_RTP_SOCKET == i)
            {
                unsigned int ulChannel = m_pChannelInfo->GetChannelNo();
				IVS_LOG(IVS_LOG_ERR, "Process Recev Data", "Udp source handle socket video do exception");
				DoExceptionCallBack(IVS_PLAYER_RET_RECV_DATA_ERROR, (void*)ulChannel);
            }
#endif 
            return;
        }

		// 保存组播情况收流的源端IP
		if (m_pChannelInfo->IsMuilt())
		{
			if (VIDEO_RTCP_SOCKET == i)
			{
				m_SenderVideoRTCPSockAddr.sin_addr = addr.sin_addr;
				m_SenderVideoRTCPSockAddr.sin_port = addr.sin_port;
				m_SenderVideoRTCPSockAddr.sin_family = addr.sin_family;
			}
			else if (AUDIO_RTCP_SOCKET == i)
			{
				m_SenderAudioRTCPSockAddr.sin_addr = addr.sin_addr;
				m_SenderAudioRTCPSockAddr.sin_port = addr.sin_port;
				m_SenderAudioRTCPSockAddr.sin_family = addr.sin_family;
			}
		}
		else
        {
			// 检查是否串流;判断是否为用户设置的IP和端口号发出的流，如果不是直接扔掉
			//视频RTP通道
			if (VIDEO_RTP_SOCKET == i)
			{
				unsigned int uiRemoteVideoRtpPort = 0;
				std::string strRemoteSendStreamIp;
				(void)m_pChannelInfo->GetStreamIP(REMOTE_SEND_STREAM_IP, strRemoteSendStreamIp);
				m_pChannelInfo->GetRemotePort(VIDEO_RTP_PORT, uiRemoteVideoRtpPort);

				if ((addr.sin_addr.s_addr != inet_addr(strRemoteSendStreamIp.c_str())) 
					|| (addr.sin_port != htons((unsigned short)uiRemoteVideoRtpPort)))
				{
					char szRemoteIp[IVS_IP_LEN] = {0};
					strncpy(szRemoteIp, inet_ntoa(addr.sin_addr), IVS_IP_LEN - 1);
					IVS_LOG(IVS_LOG_ERR, "Process Recev Data",
						"Udp source video ip or port is illegal, remote ip set[%s] socket[%s], video rtpport set[%u] socket[%u]", 
						strRemoteSendStreamIp.c_str(), szRemoteIp, uiRemoteVideoRtpPort, ntohs(addr.sin_port));
					return;
				}
			}

			//音频RTP通道
			if (AUDIO_RTP_SOCKET == i)
			{
				unsigned int uiRemoteAudioRtpPort = 0;
				std::string strRemoteSendStreamIp;
				(void)m_pChannelInfo->GetStreamIP(REMOTE_SEND_STREAM_IP, strRemoteSendStreamIp);
				m_pChannelInfo->GetRemotePort(AUDIO_RTP_PORT, uiRemoteAudioRtpPort);

				if ((addr.sin_addr.s_addr != inet_addr(strRemoteSendStreamIp.c_str())) 
					|| (addr.sin_port != htons((unsigned short)uiRemoteAudioRtpPort)))
				{
					char szRemoteIp[IVS_IP_LEN] = {0};
					strncpy(szRemoteIp, inet_ntoa(addr.sin_addr), IVS_IP_LEN - 1);

					IVS_LOG(IVS_LOG_ERR, "Process Recev Data",
						"Udp source audio ip or port is illegal, ip set[%s] socket[%s], audio rtpport set[%u] socket[%u]", 
						strRemoteSendStreamIp.c_str(), szRemoteIp, uiRemoteAudioRtpPort, ntohs(addr.sin_port));
					return;
				}
			}
        }

        ProcPacket(i, len);
    }
}

void CUdpSource::ProcPacket(int iIndex, int iLen)
{
    //过滤RTSP响应包
    if (0 == _strnicmp(m_pRecvBuf, RTSP::RTSP_VERSION, strlen(RTSP::RTSP_VERSION)))
    {
        //加上结束符;
        m_pRecvBuf[iLen] = 0;
        return;
    }

	//更新最新收流时间;
	m_ulLastCheckTimeoutTick = GetTickCount();

    switch (iIndex)
    {
        //媒体数据;
    case VIDEO_RTP_SOCKET:
    case AUDIO_RTP_SOCKET:
        m_ulRecvBufLen = (unsigned long)iLen;
        ProcMediaPacket();//处理数据;
        break;
        //RTCP
    case VIDEO_RTCP_SOCKET:
    case AUDIO_RTCP_SOCKET:
        IVS_LOG(IVS_LOG_DEBUG, "ProcPacket", "HandleRecv index:%d recv RTCP",iIndex);
        break;
    default:
        IVS_LOG(IVS_LOG_ERR, "ProcPacket", "Udp source process packet error, unknown index:%d", iIndex);
        break;
    }
}

// 检查是否满足NAT穿越条件,每隔30s向服务器发送nat消息;
int CUdpSource::CheckNatSend()
{
	// 检查是否满足发送Nat的条件;
    int iRet = CNetSource::CheckNatSend();
	if (IVS_SUCCEED != iRet)
	{
		return IVS_FAIL;
	}

	if (NULL == m_pChannelInfo)
	{
		IVS_LOG(IVS_LOG_ERR, "CheckNatSend", "Channel info object is null.");
		return IVS_FAIL;
	}

    string strRtspHeartBeat;
    RTSP::NAT_REQ_PARA stPara = {0};
    stPara.ulProtocalType = RTSP::PROTOCAL_TYPE_RTP;

    for (int i = 0; i < NET_SOURCE_UDP_SOCKET_NUM; i++)
    {
		// 适配仅分配视频端口，未分配音频端口的问题;
		if ((AUDIO_RTCP_SOCKET == i || AUDIO_RTP_SOCKET == i) 
			&& (NULL != m_pChannelInfo) 
			&& (m_pChannelInfo->IsOnlyVideo()))
		{
			//IVS_LOG(IVS_LOG_ERR, "Check Nat Send", "Udp source handle socket Index[%d] Only video port,do not send nat for audio port", i);
			continue;
		}

        //根据不同端口获取不同协议;
        if ((VIDEO_RTP_SOCKET == i) || (AUDIO_RTP_SOCKET == i))
        {
            stPara.ulProtocalType = RTSP::PROTOCAL_TYPE_RTP;
        }
        else
        {
            stPara.ulProtocalType = RTSP::PROTOCAL_TYPE_RTCP;
        }

        unsigned int uiPort = 0;
        m_pChannelInfo->GetLocalPort((PORT_TYPE)i, uiPort);
        stPara.usLocalPort = (unsigned short)uiPort;

        (void)m_RtspStack.GetNatPacket(stPara, strRtspHeartBeat);

        //心跳发送错误打印日志;
		CUdpConnection* pConn = dynamic_cast<CUdpConnection*>(m_pChannelInfo->GetConnection((SOCKET_TYPE)i));
		if (NULL == pConn)
		{
			continue;
		}

		if (m_pChannelInfo->IsMuilt())
		{
			if (VIDEO_RTCP_SOCKET == i)
			{
				iRet = pConn->SendMulitCastData(strRtspHeartBeat.c_str(), strRtspHeartBeat.length(), m_SenderVideoRTCPSockAddr);
			}
			else if (AUDIO_RTCP_SOCKET == i)
			{
				iRet = pConn->SendMulitCastData(strRtspHeartBeat.c_str(), strRtspHeartBeat.length(), m_SenderAudioRTCPSockAddr);
			}
		}
		else
		{
			iRet = pConn->SendData(strRtspHeartBeat.c_str(), strRtspHeartBeat.length());
		}

		if (SOCKET_ERROR != iRet)
		{
			IVS_LOG(IVS_SUCCEED, "HandleNatSend", "Udp source handle nat send Succeed,socket index:%d", i);
		}
		else
		{
			IVS_LOG(IVS_LOG_ERR, "HandleNatSend", "Udp source handle nat send error, WSAGetLastError:%d,socket index:%d", WSAGetLastError(), i);
		}
    }
	return IVS_SUCCEED;
}

int CUdpSource::ConnSocket(const std::string& strRemoteSendStreamIp)
{
    int iRet = IVS_FAIL;
    if (NULL == m_pChannelInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "m_pChannelInfo = NULL.");
        return iRet;
    }

    for (int i = 0; i < SOCKET_COUNT; i++)
    {
        CConnection* pConn = m_pChannelInfo->GetConnection((SOCKET_TYPE)i);
        if (NULL == pConn)
        {
            iRet = IVS_FAIL;
            IVS_LOG(IVS_LOG_ERR, "Init", "Get connection from ChannelInfo failed, pConn = NULL ,socket index:%d", i);
            break;
        }

        unsigned int uiPort = 0;
        iRet = m_pChannelInfo->GetRemotePort((PORT_TYPE)i, uiPort);

        // add for 兼容只有音频通道的场景
        if (0 == uiPort
            && VIDEO_RTP_PORT == i)
        {
            i = VIDEO_RTCP_SOCKET;
            continue;
        }
        // end add

		// add for 增加前端仅能分配视频端口，不能分配音频端口的问题,因为BT的前端分配错误一般是0和1，此处使用小于5适配大部分情况;
		if (5 > uiPort && (AUDIO_RTP_PORT == i || AUDIO_RTCP_PORT == i))
		{
			IVS_LOG(IVS_LOG_ERR, "Init", "Connection of audio port[%u] is error,socket index[%d]", uiPort, i);
			m_pChannelInfo->SetOnlyVideo(true);
			continue;
		}
		// end add

        iRet += pConn->Connect(strRemoteSendStreamIp.c_str(), uiPort);
        if (IVS_SUCCEED != iRet)
        {
            IVS_LOG(IVS_LOG_ERR, "Init", "socket connect failed ,socket index:%d", i);
            break;
        }
    }
    
    if (IVS_SUCCEED != iRet)
    {
        ShutDownSocket();
        iRet = IVS_FAIL;
    }
    return iRet;
}

void CUdpSource::ShutDownSocket()
{
    if (NULL == m_pChannelInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "m_pChannelInfo = NULL.");
        return;
    }

    for (int i = 0; i < SOCKET_COUNT; i++)
    {
        CConnection* pConn = m_pChannelInfo->GetConnection((SOCKET_TYPE)i);
        if (NULL == pConn)
        {
            continue;
        }

        pConn->ShutDown();
    }
}

