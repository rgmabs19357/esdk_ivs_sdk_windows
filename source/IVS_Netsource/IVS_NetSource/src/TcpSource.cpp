#include "TcpSource.h"
#include "ChannelInfo.h"
#include "Connection.h"
#include "ivs_log.h"
#include "Rtp.h"

CTcpSource::CTcpSource(void)
{
	m_socket = INVALID_SOCKET;
    m_bIsFirstConnect = true;
    m_strRemoteIp = "";
    m_uiRemotePort = 0;
}


CTcpSource::~CTcpSource(void)
{
}

int CTcpSource::Init(CChannelInfo* pChannelInfo)
{
    int iRet = CNetSource::Init(pChannelInfo);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "Init function of base class failed");
        return iRet;
    }

    // 连接socket;
    iRet = pChannelInfo->GetStreamIP(REMOTE_SEND_STREAM_IP, m_strRemoteIp);
    if (m_strRemoteIp.empty() || IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "TCP GetStreamIP failed. strRemoteIp is empty");
        return IVS_PARA_INVALID;
    }

    CConnection* pConn = pChannelInfo->GetConnection(VIDEO_RTP_SOCKET);
    if (NULL != pConn)
    {
        pChannelInfo->GetRemotePort(VIDEO_RTP_PORT, m_uiRemotePort);
        iRet = pConn->Connect(m_strRemoteIp.c_str(), m_uiRemotePort);
        if (IVS_SUCCEED != iRet)
        {
            pConn->ShutDown();
            IVS_LOG(IVS_LOG_ERR, "Init", "Video rtp socket connect failed ,return code:%d", iRet);
            return IVS_PLAYER_RET_ORDER_ERROR;
        }
        
		m_socket = pConn->GetSocket();
		pConn->Select(READ);

		std::string strLocalIp;
		unsigned int uiLocalPort = 0;
		pConn->GetLocalIpPort(strLocalIp, uiLocalPort);

		//PacketRtspInfo(strLocalIp, uiLocalPort, strRemoteIp, uiRemotePort);
    }
    else
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "pConn is Null.");
    }
    
    return iRet;
}

void CTcpSource::ProcessRecevData()
{
    //有数据到来;
    CConnection* pConnection = m_pChannelInfo->GetConnection(VIDEO_RTP_SOCKET);
    if (NULL == pConnection)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcessRecevData", "pConnection is NULL.");
        return;
    }

    int len = pConnection->RecvData(m_pRecvBuf, sizeof(RTP::INTERLEAVED_HEAD));
    if (len == IVS_PLAYER_RET_RECV_DATA_TIMEOUT)
    {
        HandleTimeOutEvent();
        return;
    }
    else if (len <= 0)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcessRecevData", "Tcp source handle socket receive data failed.error:%d", WSAGetLastError());
    }

    RTP::INTERLEAVED_HEAD* pInterHdr = reinterpret_cast<RTP::INTERLEAVED_HEAD*>(m_pRecvBuf);//lint !e826
    if (NULL == pInterHdr)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcessRecevData", "Transform rtp head failed.");
        return;
    }
    if (pInterHdr->tag != '$')//不是标志，是RTSP
    {
		IVS_LOG(IVS_LOG_DEBUG, "ProcessRecevData", "pInterHdr->tag[%d] != '$'[%d]", pInterHdr->tag, '$');
		if (IVS_SUCCEED != RecvRTSP())//接受RTSP失败
		{
			//媒体流超时异常上报;
			/*unsigned int ulChannel = m_pChannelInfo->GetChannelNo();
			unsigned int* pChannelNo = &ulChannel;
			DoExceptionCallBack(IVS_PLAYER_RET_RECV_DATA_TIMEOUT, (void*)pChannelNo);*/
			return;//标记不对直接错误;
		}
		else
		{
			ProcRtsp();//处理RTSP包，即EOS
			return;
		}
    }

    unsigned short usPacketLen = ntohs(pInterHdr->usLen);
    char cChannel = pInterHdr->channel;
	IVS_LOG(IVS_LOG_DEBUG, "ProcessRecevData", "PacketLen[%u] Channel[%d]", usPacketLen, cChannel);
    if (usPacketLen > RTP_MEDIA_BUF_LEN)//超过缓冲区长度;
    {
		// 循环接收超过缓冲区大小的数据，接收后直接抛弃后上报数据异常;
		while(usPacketLen > RTP_MEDIA_BUF_LEN)
		{
			len = pConnection->RecvData(m_pRecvBuf, RTP_MEDIA_BUF_LEN);
            if (len == IVS_PLAYER_RET_RECV_DATA_TIMEOUT)
            {
                HandleTimeOutEvent();
                return;
            }
			usPacketLen -= (unsigned short)len;
		}
		int iRet = pConnection->RecvData(m_pRecvBuf, usPacketLen);
        if (iRet == IVS_PLAYER_RET_RECV_DATA_TIMEOUT)
        {
            HandleTimeOutEvent();
            return;
        }

        IVS_LOG(IVS_LOG_ERR, "ProcessRecevData", 
                "Tcp source loop receive data error,  RTP packet length:%u > RTP_MEDIA_BUF_LEN:%d",
                usPacketLen, RTP_MEDIA_BUF_LEN);
        //异常回调 媒体流超时异常上报;
        unsigned int ulChannel = m_pChannelInfo->GetChannelNo();
        DoExceptionCallBack(IVS_PLAYER_RET_RECV_DATA_ERROR, (void*)ulChannel);
        return;//如果没有接收到缓冲区，则肯定乱了，直接退出;
    }

    len = pConnection->RecvData(m_pRecvBuf, usPacketLen);
    if (len == IVS_PLAYER_RET_RECV_DATA_TIMEOUT)
    {
        HandleTimeOutEvent();
        return;
    }

    // 如果视频端口收不到数据，上报异常;
    if (len <= 0 )
    {
        IVS_LOG(IVS_LOG_ERR, "ProcessRecevData", "Udp source thread handle receive failed");
        //异常上报;
        unsigned int ulChannel = m_pChannelInfo->GetChannelNo();
        DoExceptionCallBack(IVS_PLAYER_RET_RECV_DATA_ERROR, (void*)ulChannel);
        return;
    }

    //暂时不处理RTCP
    if (RTP::INTERLEAVED_TYPE_RTP_VIDEO == cChannel || RTP::INTERLEAVED_TYPE_RTP_AUDIO == cChannel)
    {
        m_ulRecvBufLen = usPacketLen;
        ProcMediaPacket();//处理数据;
    }
    else if (RTP::INTERLEAVED_TYPE_RTCP_VIDEO == cChannel || RTP::INTERLEAVED_TYPE_RTCP_AUDIO == cChannel)//收到RTCP
    {
        IVS_LOG(IVS_LOG_DEBUG, "ProcessRecevData", "Tcp source loop receive data recv rtcp,do nothing,channel:%d",cChannel);
    }
    else
    {
        IVS_LOG(IVS_LOG_DEBUG, "ProcessRecevData", "Data is not RTP or RTCP,do nothing,channel:%d",cChannel);
    }
}

/**
* 处理NAT穿越
*/
int CTcpSource::CheckNatSend()
{
    int iRet = IVS_FAIL;

    //一连接上马上发送一个Nat穿越，不用判断时间
    if (!m_bIsFirstConnect)
    {
        // 检查是否满足发送Nat的条件;
        iRet = CNetSource::CheckNatSend();
        if (IVS_SUCCEED != iRet)
        {
            //IVS_LOG(IVS_LOG_ERR, "Check Nat Send", "Call base check nat send failed.");
            return IVS_FAIL;
        }
    }

    string strRtspHeartBeat = "";
    RTSP::NAT_REQ_PARA stPara = {0};
    stPara.ulProtocalType = RTSP::PROTOCAL_TYPE_RTP;

	std::string strLocalIp = "";
	(void)m_pChannelInfo->GetStreamIP(LOCAL_RECV_STREAM_IP, strLocalIp);

    //心跳发送错误打印日志;
	CConnection* pConn = m_pChannelInfo->GetConnection(VIDEO_RTP_SOCKET);
	if (NULL == pConn)
	{
		IVS_LOG(IVS_LOG_ERR, "Check Nat Send", "video connection obj = NULL.");
		return IVS_FAIL;
	}

	unsigned int uiLocalPort = 0;
	pConn->GetLocalIpPort(strLocalIp, uiLocalPort);
	stPara.usLocalPort = (unsigned short)uiLocalPort;

    PacketRtspInfo(strLocalIp, uiLocalPort, m_strRemoteIp, m_uiRemotePort);  //lint !e1036

	(void)m_RtspStack.GetNatPacket(stPara, strRtspHeartBeat);
	IVS_LOG(IVS_SUCCEED, "HandleNatSend", "m_RtspStack.strRtspHeartBeat:%s", strRtspHeartBeat.c_str());

	iRet = pConn->SendData(strRtspHeartBeat.c_str(), strRtspHeartBeat.length());
	if (SOCKET_ERROR != iRet)
	{
		IVS_LOG(IVS_SUCCEED, "HandleNatSend", "Tcp source handle nat send Succeed");
	}
	else
	{
		IVS_LOG(IVS_LOG_ERR, "HandleNatSend", "Tcp source handle nat send error, WSAGetLastError:%d", WSAGetLastError());
	}

    m_bIsFirstConnect = false;
	return IVS_SUCCEED;
}

long CTcpSource::RecvRTSP()
{
	IVS_LOG(IVS_LOG_DEBUG, "RecvRTSP", "Tcp source receive RTSP begin.");

	if (NULL == m_pRecvBuf)
	{
		IVS_LOG(IVS_LOG_ERR, "RecvRTSP", "Tcp source receive RTSP error, receive buffer is null.");  
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

	char* pRtspBuf = m_pRecvBuf + sizeof(RTP::INTERLEAVED_HEAD);//已经接受头了，这里跳过头

	memset(pRtspBuf, 0, RTP_MEDIA_BUF_LEN - sizeof(RTP::INTERLEAVED_HEAD));

	char c = 0;
	int nIndex = 0;
	bool bResponsed = false;

	while(!bResponsed && nIndex < (int)((RTP_MEDIA_BUF_LEN - sizeof(RTP::INTERLEAVED_HEAD)) -1))//循环接收头
	{
		long lRet = recv(m_socket,&c,1,0);
		if (lRet <= 0)
		{
			IVS_LOG(IVS_LOG_ERR, "RecvRTSP", "Tcp source receive RTSP error,  receive data length <= 0.lRet:%d,WSAGetLastError:%d",lRet,WSAGetLastError());
			return IVS_FAIL;
		}   
		*(pRtspBuf+nIndex) = c;
		nIndex++;
		if (nIndex >= 4)//检查是否把头接收全;
		{
			if (*(pRtspBuf + (nIndex- 4)) == '\r' && *(pRtspBuf + (nIndex - 3)) == '\n'
				&& *(pRtspBuf + (nIndex - 2)) == '\r' && *(pRtspBuf + (nIndex- 1)) == '\n')
			{
				bResponsed = true;//如果接收全，设置回复标志;
			}
		}
	}

	if (!bResponsed)
	{
		IVS_LOG(IVS_LOG_ERR, "RecvRTSP", "Tcp source receive RTSP error, response = false.RTSP content not full recved.");
		return IVS_FAIL;
	}

	IVS_LOG(IVS_LOG_DEBUG, "RecvRTSP", "Tcp source receive RTSP data:\n%s\n", m_pRecvBuf);//RTSP响应，暂时打印全部

	return IVS_SUCCEED;
}

/**
处理接受到的RTSP
*/
void CTcpSource::ProcRtsp()
{
	if (NULL == m_pRecvBuf)
	{
		IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "Net Source Process Rtsp error,receive buffer is null.");  
		return;
	}

	RTSP::DECODE_RESULT stRtsp;
	if (m_RtspStack.DecodePacket(m_pRecvBuf, stRtsp) != IVS_SUCCEED)//解析错误
	{
		IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "Net Source Process Rtsp error, Decode Packet failed");  
		return;
	}

	IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "decode messsage Type:%u, Play Information Type:%u",
			stRtsp.ulMsgType, stRtsp.ulPlayInfoType);

	//收到EOS 或者 BOS BOS当作EOS上报给界面
	if (stRtsp.ulMsgType == RTSP::MSG_TYPE_SET_PARAMETER && 
		stRtsp.ulPlayInfoType == RTSP::PLAY_INFO_TYPE_EOS || stRtsp.ulPlayInfoType == RTSP::PLAY_INFO_TYPE_BOS)
	{
		IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "Receive EOS or BOS message Type:%u,Play Information Type:%u",
				stRtsp.ulMsgType, stRtsp.ulPlayInfoType);

		//发送响应
		string strRsp;
		m_RtspStack.GetRspPacket(stRtsp.strCSeq, strRsp);
		IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "send rtsp response:\n%s", strRsp.c_str());

		if (send(m_socket, strRsp.c_str(), (int)strRsp.length(), 0) == SOCKET_ERROR)//发送出错，这里发送出错不处理
		{
			IVS_LOG(IVS_LOG_ERR, "ProcRtsp", "Net Source Process Rtsp error, send rtsp response failed.");
			return ;
		}

		if (stRtsp.ulPlayInfoType == RTSP::PLAY_INFO_TYPE_EOS)
		{
			IVS_LOG(IVS_LOG_ERR, "ProcRtsp", "Net Source callback EOS!");
			//m_bEos = true;
			//(void)m_EventNotify.NotifyStreamStatus(CUMW_STREAM_STATUS_EOS);//通知EOS
		}
		else//只可能是BOS
		{
			IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "Net Source callback BOS!");
			//(void)m_EventNotify.NotifyStreamStatus(CUMW_STREAM_STATUS_BOS);//通知BOS
		}

		IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "Net Source proecss RTSP packet end.");
	}

	if (stRtsp.ulMsgType == RTSP::MSG_TYPE_RESP && stRtsp.bHasRtpInfo)//收到rtp-info
	{
		IVS_LOG(IVS_LOG_DEBUG, "ProcRtsp", "Net Source process rtp-info sequence:%u,time Stamp:%u", 
			stRtsp.ulRtpSeq, stRtsp.ulRtpTime);        
	}
}

void CTcpSource::PacketRtspInfo(const string strLocalIp, unsigned long ulLocalPort, const string strSourceIp, unsigned long ulSourcePort)
{
	//设置RTSP信息
	RTSP::MEDIA_INFO stMediaInfo = {0};

	//组装strSessionUrl
	std::string strSessionUrl = "";
	char tmp[6];
	memset(tmp, 0, sizeof(tmp));

	strSessionUrl += "rtsp://";
	strSessionUrl += strSourceIp;
	strSessionUrl += ":";

	(void)_ultoa(ulSourcePort, tmp, 10);
	strSessionUrl += string(tmp);

	strSessionUrl += "/c02.sdp";
	//std::string strSessionId = "1";

    IVS_LOG(IVS_LOG_DEBUG, "PacketRtspInfo", "SessionId[%s].", m_pChannelInfo->GetSessionId().c_str());
	strncpy(stMediaInfo.szLocalIp, strLocalIp.c_str(), RTSP::MAX_LEN_IP);
    strncpy(stMediaInfo.szSessionId, m_pChannelInfo->GetSessionId().c_str(), RTSP::MAX_LEN_SESSION_ID);
	strncpy(stMediaInfo.szSessionUrl, strSessionUrl.c_str(), RTSP::MAX_LEN_SESSION_URL);
	stMediaInfo.usLocalPort = (unsigned short)ulLocalPort;

	m_RtspStack.SetMediaInfo(stMediaInfo);
}

void CTcpSource::HandleTimeOutEvent()
{
    //媒体流超时异常上报;
    unsigned int ulChannel = m_pChannelInfo->GetChannelNo();
    unsigned int* pChannelNo = &ulChannel;
    DoExceptionCallBack(IVS_PLAYER_RET_RECV_DATA_TIMEOUT, (void*)pChannelNo);
    IVS_LOG(IVS_LOG_ERR, "HandleTimeOutEvent", "Tcp source handle time out");
    return;
}
