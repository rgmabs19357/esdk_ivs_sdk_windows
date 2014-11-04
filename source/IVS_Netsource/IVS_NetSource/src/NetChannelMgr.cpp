#include "NetChannelMgr.h"
#include "hwsdk.h"
#include "NetChannel.h"
#include "IVSCommon.h"
#include "Connection.h"
#include "TcpConnection.h"
#include "UdpConnection.h"
#include "ivs_log.h"
#include "MulticastSource.h"

static IVS_VOID CALLBACK ExceptionCallBack(IVS_ULONG /*ulChannel*/, IVS_INT32 iMsgType, IVS_VOID*  pParam, IVS_VOID*  /*pUser*/)
{
    // 内部模块异常通过该入口统一回调到管理类，再由该方法统一回调上层;
    // 内部模块不记录用户信息，由管理类统一将用户信息回调上层;
    CNetChannelMgr::instance().DoExceptionCallBack(iMsgType, pParam);
}

CNetChannelMgr::CNetChannelMgr(void)
{
    Init();
}

CNetChannelMgr::~CNetChannelMgr(void)
{
    try
    {
        UnInit(); 
    }
    catch(...)
    {
    }
}

void CNetChannelMgr::Init(void)
{
    m_cbExceptionCallBack = NULL;
    m_pUserData = NULL;

    m_mutexFreePortList.Lock();
    m_lstFreePort.clear();
    //初始化空闲列表;
    for (unsigned int i = NET_SOURCE_BEGIN_PORT; i < NET_SOURCE_END_PORT; i += 2)
    {
        m_lstFreePort.push_back(i);
    }
    m_mutexFreePortList.UnLock();

    m_mutexBusyPortListLock.Lock();
    m_lstBusyPort.clear();
    m_mutexBusyPortListLock.UnLock();
}

void CNetChannelMgr::UnInit(void)
{
    m_cbExceptionCallBack = NULL;
    m_pUserData = NULL;

    m_mutexFreePortList.Lock();
    m_lstFreePort.clear();
    m_mutexFreePortList.UnLock();

    m_mutexBusyPortListLock.Lock();
    m_lstBusyPort.clear();
    m_mutexBusyPortListLock.UnLock();

    CAutoLock lock(m_mutexChannel);
    CNetChannel* pNetChannel = NULL;
    NetChannelMapIter iter = m_mapNetChannel.begin();
    NetChannelMapIter iterEnd = m_mapNetChannel.end();
    for (; iter != iterEnd; iter++)
    {
        pNetChannel = dynamic_cast<CNetChannel*>(iter->second);//lint !e611
        IVS_DELETE(pNetChannel);
    }
    m_mapNetChannel.clear();
    return;
}

//获取可用句柄
int CNetChannelMgr::GetChannel(unsigned long& ulChannel)
{
    int iRet = IVS_PLAYER_RET_GET_CHANNEL_ERROR;
    CAutoLock lock(m_mutexChannel);

    unsigned int uiIndex = m_mapNetChannel.size();
    // 超过最大限制;
    if (NET_SOURCE_TOTAL_CHANNEL_NUM <= uiIndex)
    {
        IVS_LOG(IVS_LOG_ERR, "GetChannel", "Get channel over total number = %d", NET_SOURCE_TOTAL_CHANNEL_NUM);
        return iRet;
    }

    //在MAP中创建通道资源信息对象，与通道号对应插入MAP中
    CNetChannel* pNetChannel = IVS_NEW((CNetChannel*&)pNetChannel);
    if (NULL == pNetChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "GetChannel", "Create Port Resource failed.");
        return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    } 

    ulChannel = reinterpret_cast<unsigned long>(pNetChannel);
    (void)m_mapNetChannel.insert(std::make_pair(ulChannel, pNetChannel)); //lint !e1025 !e119

	pNetChannel->SetChannelNo(ulChannel);
    return IVS_SUCCEED;
}

// 释放指定句柄;
int CNetChannelMgr::FreeChannel(unsigned long ulChannel)
{
    CAutoLock lock(m_mutexChannel);
    
    int iRet = IVS_FAIL;
    NetChannelMapIter iter = m_mapNetChannel.find(ulChannel);
    if (iter != m_mapNetChannel.end())
    {
        CNetChannel* pNetChannel = dynamic_cast<CNetChannel*>(iter->second);//lint !e611
        IVS_DELETE(pNetChannel);
        m_mapNetChannel.erase(iter);
        iRet = IVS_SUCCEED;
    }
    return iRet;
}

// 设置媒体流传输协议类型;
int CNetChannelMgr::SetProtocolType(unsigned long ulChannel, unsigned int uiProtocolType)
{
    int iRet = IVS_FAIL;
    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        pNetChannel->SetProtocolType(static_cast<IVS_PROTOCOL_TYPE>(uiProtocolType));
        iRet = IVS_SUCCEED;
    }
    return iRet;
}

// 获得媒体流传输协议类型;
int CNetChannelMgr::GetProtocolType(unsigned long ulChannel, unsigned int& uiProtocolType)
{
    int iRet = IVS_FAIL;
    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        uiProtocolType = pNetChannel->GetProtocolType();
    }
    return iRet;
}

// 设置接收码流的Ip地址;
int CNetChannelMgr::SetLocalRecvStreamIP(unsigned long ulChannel, const char* pszLocalRecvStreamIp)
{
    int iRet = IVS_FAIL;
    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        iRet = pNetChannel->SetStreamIP(LOCAL_RECV_STREAM_IP, pszLocalRecvStreamIp);
    }
    return iRet;
}

// 获得接收码流的IP地址;
int CNetChannelMgr::GetLocalRecvStreamIP(unsigned long ulChannel, char* pszLocalRecvStreamIp)
{
    if (NULL == pszLocalRecvStreamIp)
    {
        IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "pszLocalRecvStreamIp = NULL.");
        return IVS_PARA_INVALID;
    }

    int iRet = IVS_FAIL;
    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        std::string strLocalRecvStreamIp;
        iRet = pNetChannel->GetStreamIP(LOCAL_RECV_STREAM_IP, strLocalRecvStreamIp);
        if (strLocalRecvStreamIp.empty() || IVS_SUCCEED != iRet)
        {
            IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", 
                    "Get receive stream ip error, need set receive stream ip before this function");
            return iRet;
        }

        // IP超过最大长度;
        if (IVS_IP_LEN <= strLocalRecvStreamIp.size())
        {
            IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "Ip length is over max length.");
            return IVS_FAIL;
        }

        strncpy(pszLocalRecvStreamIp, strLocalRecvStreamIp.c_str(), IVS_IP_LEN);
    }
    return iRet;
}

// 获取接收码流的地址IP和端口号;
int CNetChannelMgr::GetLocalRecvStreamPort(unsigned long ulChannel, unsigned int& uiLocalRecvStreamVideoRtpPort, 
                                           unsigned int& uiLocalRecvStreamAudioRtpPort)
{
    int iRet = IVS_FAIL;
    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL == pNetChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "Get channel error");
        return iRet;
    }

    /*pNetChannel->SetChannelNo(ulChannel);*/

    std::string strLocalRecvStreamIp;
    iRet = pNetChannel->GetStreamIP(LOCAL_RECV_STREAM_IP, strLocalRecvStreamIp);
    if (strLocalRecvStreamIp.empty() || IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", 
                "Get receive stream ip error, need set receive stream ip before this function");
        return iRet;
    }

    unsigned int uiVideoRtpPort = 0;
    unsigned int uiAudioRtpPort = 0;
    IVS_PROTOCOL_TYPE enProtocolType = pNetChannel->GetProtocolType();
    if (PROTOCOL_RTP_OVER_UDP == enProtocolType)
    {
        CConnection* pVideoRtpConnection = NULL;
        CConnection* pVideoRtcpConnection = NULL;
        CConnection* pAudioRtpConnection = NULL;
        CConnection* pAudioRtcpConnection = NULL;
        iRet = GetUdpSocketPort(strLocalRecvStreamIp.c_str(), uiVideoRtpPort, pVideoRtpConnection, pVideoRtcpConnection);
        if (IVS_SUCCEED != iRet)
        {
            if (NULL != pVideoRtpConnection)
            {
                pVideoRtpConnection->ShutDown();
                IVS_DELETE(pVideoRtpConnection);
            }

            if (NULL != pVideoRtcpConnection)
            {
                pVideoRtcpConnection->ShutDown();
                IVS_DELETE(pVideoRtcpConnection);
            }
            IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "GetUdpSocketPort failed, return value:%d", iRet);
            return iRet;
        }

        iRet = GetUdpSocketPort(strLocalRecvStreamIp.c_str(), uiAudioRtpPort, pAudioRtpConnection, pAudioRtcpConnection);
        if (IVS_SUCCEED != iRet)
        {
			if (NULL != pAudioRtpConnection)
			{
				pAudioRtpConnection->ShutDown();
				IVS_DELETE(pAudioRtpConnection);
			}

			if (NULL != pAudioRtcpConnection)
			{
				pAudioRtcpConnection->ShutDown();
				IVS_DELETE(pAudioRtcpConnection);
			}
            
            IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "GetUdpSocketPort failed, return value:%d", iRet);
            return iRet;
        }

        // 将视频Socket缓存设置为1MByte 满足高清视频;
        pVideoRtpConnection->SetSocketBuf(NET_RTP_MAX_SOCKET_BUFFER);

        pNetChannel->SetConnection(VIDEO_RTP_SOCKET, pVideoRtpConnection);
        pNetChannel->SetConnection(VIDEO_RTCP_SOCKET, pVideoRtcpConnection);
        pNetChannel->SetConnection(AUDIO_RTP_SOCKET, pAudioRtpConnection);
        pNetChannel->SetConnection(AUDIO_RTCP_SOCKET, pAudioRtcpConnection);
        uiLocalRecvStreamVideoRtpPort = uiVideoRtpPort;
        uiLocalRecvStreamAudioRtpPort = uiAudioRtpPort;

		pNetChannel->SetLocalPort(VIDEO_RTP_PORT, uiVideoRtpPort);
		pNetChannel->SetLocalPort(AUDIO_RTP_PORT, uiAudioRtpPort);
    }
    else
    {
        CConnection* pConnection = NULL;
        iRet = GetTcpSocketPort(strLocalRecvStreamIp.c_str(), uiVideoRtpPort, pConnection);
        if (IVS_SUCCEED != iRet)
        {
            if (NULL != pConnection)
            {
                pConnection->ShutDown();
                IVS_DELETE(pConnection);
            }

            IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "GetTcpSocketPort failed, return value:%d", iRet);
            return IVS_FAIL;
        }
        else
        {
            pNetChannel->SetLocalPort(VIDEO_RTP_PORT, uiVideoRtpPort);
            pNetChannel->SetConnection(VIDEO_RTP_SOCKET, pConnection);
            uiLocalRecvStreamVideoRtpPort = uiVideoRtpPort;
        }
    }

    IVS_LOG(IVS_LOG_DEBUG, 
			"GetLocalRecvStreamAddr", 
			"success Channel[%lu] VideoRtp[%u] AudioRtp[%u]", 
			ulChannel, uiLocalRecvStreamVideoRtpPort, uiLocalRecvStreamAudioRtpPort);
    return iRet;
}

// 设置接收码流的端口号;
int CNetChannelMgr::SetLocalRecvStreamPort(unsigned long ulChannel, unsigned int uiLocalRecvStreamVideoRtpPort, 
	unsigned int uiLocalRecvStreamAudioRtpPort)
{
	int iRet = IVS_FAIL;
	CNetChannel* pNetChannel = GetChannelByID(ulChannel);
	if (NULL == pNetChannel)
	{
		IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "Get channel error");
		return iRet;
	}

	std::string strLocalRecvStreamIp;
	iRet = pNetChannel->GetStreamIP(LOCAL_RECV_STREAM_IP, strLocalRecvStreamIp);
	if (strLocalRecvStreamIp.empty() || IVS_SUCCEED != iRet)
	{
		IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr","Get receive stream ip error, need set receive stream ip before this function");
		return iRet;
	}

	IVS_PROTOCOL_TYPE enProtocolType = pNetChannel->GetProtocolType();
	if (PROTOCOL_RTP_OVER_UDP == enProtocolType)
	{
		CConnection* pVideoRtpConnection = NULL;
		CConnection* pVideoRtcpConnection = NULL;
		CConnection* pAudioRtpConnection = NULL;
		CConnection* pAudioRtcpConnection = NULL;
		iRet = SetUdpSocketPort(strLocalRecvStreamIp.c_str(), uiLocalRecvStreamVideoRtpPort, pVideoRtpConnection, pVideoRtcpConnection);
		if (IVS_SUCCEED != iRet)
		{
			if (NULL != pVideoRtpConnection)
			{
				pVideoRtpConnection->ShutDown();
				IVS_DELETE(pVideoRtpConnection);
			}

			if (NULL != pVideoRtcpConnection)
			{
				pVideoRtcpConnection->ShutDown();
				IVS_DELETE(pVideoRtcpConnection);
			}
			IVS_LOG(IVS_LOG_ERR, "SetLocalRecvStreamPort", "SetUdpSocketPort failed, return value:%d", iRet);
			return iRet;
		}

		iRet = SetUdpSocketPort(strLocalRecvStreamIp.c_str(), uiLocalRecvStreamAudioRtpPort, pAudioRtpConnection, pAudioRtcpConnection);
		if (IVS_SUCCEED != iRet)
		{
			if (NULL != pAudioRtpConnection)
			{
				pAudioRtpConnection->ShutDown();
				IVS_DELETE(pAudioRtpConnection);
			}

			if (NULL != pAudioRtcpConnection)
			{
				pAudioRtcpConnection->ShutDown();
				IVS_DELETE(pAudioRtcpConnection);
			}

			IVS_LOG(IVS_LOG_ERR, "SetLocalRecvStreamPort", "SetUdpSocketPort failed, return value:%d", iRet);
			return iRet;
		}

		// 将视频Socket缓存设置为1MByte 满足高清视频;
		pVideoRtpConnection->SetSocketBuf(NET_RTP_MAX_SOCKET_BUFFER);

		pNetChannel->SetConnection(VIDEO_RTP_SOCKET, pVideoRtpConnection);
		pNetChannel->SetConnection(VIDEO_RTCP_SOCKET, pVideoRtcpConnection);
		pNetChannel->SetConnection(AUDIO_RTP_SOCKET, pAudioRtpConnection);
		pNetChannel->SetConnection(AUDIO_RTCP_SOCKET, pAudioRtcpConnection);

		pNetChannel->SetLocalPort(VIDEO_RTP_PORT, uiLocalRecvStreamVideoRtpPort);
		pNetChannel->SetLocalPort(VIDEO_RTCP_PORT, uiLocalRecvStreamVideoRtpPort+1);
		pNetChannel->SetLocalPort(AUDIO_RTP_PORT, uiLocalRecvStreamAudioRtpPort);
		pNetChannel->SetLocalPort(AUDIO_RTCP_PORT, uiLocalRecvStreamAudioRtpPort+1);
	}
	else
	{
		CConnection* pConnection = NULL;
		iRet = SetTcpSocketPort(strLocalRecvStreamIp.c_str(), uiLocalRecvStreamVideoRtpPort, pConnection);
		if (IVS_SUCCEED != iRet)
		{
			if (NULL != pConnection)
			{
				pConnection->ShutDown();
				IVS_DELETE(pConnection);
			}

			IVS_LOG(IVS_LOG_ERR, "GetLocalRecvStreamAddr", "GetTcpSocketPort failed, return value:%d", iRet);
			return IVS_FAIL;
		}
		else
		{
			pNetChannel->SetLocalPort(VIDEO_RTP_PORT, uiLocalRecvStreamVideoRtpPort);
			pNetChannel->SetConnection(VIDEO_RTP_SOCKET, pConnection);
		}
	}

	IVS_LOG(IVS_LOG_DEBUG,"GetLocalRecvStreamAddr", "success Channel[%lu] VideoRtp[%u] AudioRtp[%u]", 
		ulChannel, uiLocalRecvStreamVideoRtpPort, uiLocalRecvStreamAudioRtpPort);
	return iRet;
}

// 打开网络流;
int CNetChannelMgr::OpenNetStream(unsigned long ulChannel, const char* pRemoteSendStreamIp, 
                                  unsigned int uiRemoteSendStreamVideoPort, 
                                  unsigned int uiRemoteSendStreamAudioPort)
{
    int iRet = IVS_FAIL;

    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        iRet = pNetChannel->OpenNetStream(pRemoteSendStreamIp, 
                                          uiRemoteSendStreamVideoPort, 
                                          uiRemoteSendStreamAudioPort,
                                          ExceptionCallBack);
    }
    else
    {
        IVS_LOG(IVS_LOG_ERR, "OpenNetStream", "GetChannelByID failed, Input param ulChannel error. ulChannel = %lu.", ulChannel);
    }
    return iRet;
}

// 关闭网络流;
int CNetChannelMgr::CloseNetStream(unsigned long ulChannel)
{
    int iRet = IVS_FAIL;

    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL == pNetChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "CloseNetStream", "GetChannelByID failed, channel id do not existed, id:%d", ulChannel);
        return iRet;
    }

    unsigned int uiVideoRtpPort = 0;
    unsigned int uiAudioRtpPort = 0;
    IVS_PROTOCOL_TYPE enProtocol = pNetChannel->GetProtocolType();
    if (PROTOCOL_RTP_OVER_UDP == enProtocol)
    {
        (void)pNetChannel->GetLocalPort(VIDEO_RTP_PORT, uiVideoRtpPort);
        (void)pNetChannel->GetLocalPort(AUDIO_RTP_PORT, uiAudioRtpPort);
        FreePortfromList(uiVideoRtpPort);
        FreePortfromList(uiAudioRtpPort);
    }
    else
    {
        (void)pNetChannel->GetLocalPort(VIDEO_RTP_PORT, uiVideoRtpPort);
        FreePortfromList(uiVideoRtpPort);
    }
   
    iRet = pNetChannel->CloseNetStream();
    return iRet;
}

// 启动收流;
int CNetChannelMgr::StartRecvStream(unsigned long ulChannel)
{
    int iRet = IVS_FAIL;

    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        iRet = pNetChannel->StartRecvStream();
    }

    return iRet;
}

void CNetChannelMgr::SetExceptionCallBack(NET_EXCEPTION_CALLBACK cbExceptionCallBack, void* pUser)
{
    CAutoLock lock(m_mutexExceptionCallback);
    m_cbExceptionCallBack = cbExceptionCallBack;
    m_pUserData = pUser;
}

void CNetChannelMgr::DoExceptionCallBack(int iMsgType, void* pParam)
{
	IVS_LOG(IVS_LOG_DEBUG, "DoExceptionCallBack", "MsgType[%d]", iMsgType);

    if (NULL == pParam)
    {
        IVS_LOG(IVS_LOG_ERR, "DoExceptionCallBack", "pParam is null.");
        return;
    }

    if (NULL == m_cbExceptionCallBack)
    {
        IVS_LOG(IVS_LOG_ERR, "DoExceptionCallBack", "ExceptionCallBack pointer is null.");
        return;
    }

    CAutoLock lock(m_mutexExceptionCallback);
	
    unsigned int* pChannelNo = reinterpret_cast<unsigned int*>(pParam);
	unsigned int ulChannel = *pChannelNo;
    IVS_LOG(IVS_LOG_DEBUG, "DoExceptionCallBack", "Channel ID[%lu]", ulChannel);

	CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL == pNetChannel)
    {
		IVS_LOG(IVS_LOG_ERR, "DoExceptionCallBack", "GetChannelByID [%lu] failed .", ulChannel);
        return;
    }

    switch (iMsgType)
    {
    case IVS_PLAYER_RET_RECV_DATA_ERROR:
        {
            void* pErrorInfo = NULL;    //需要增加结构体反馈错误信息;
            m_cbExceptionCallBack(ulChannel, iMsgType, pErrorInfo, m_pUserData);
        }
        break;
    case IVS_PLAYER_RET_RECV_DATA_TIMEOUT:
        {
            void* pErrorInfo = NULL;    //需要增加结构体反馈错误信息;
            m_cbExceptionCallBack(ulChannel, iMsgType, pErrorInfo, m_pUserData);
        }
        break;
    default:
		IVS_LOG(IVS_LOG_ERR, "DoExceptionCallBack", "Msg type[%d] do not exist.", iMsgType);
        break;
    }

    return;
}

void CNetChannelMgr::SetDataCallBack(unsigned long ulChannel, NET_DATA_CALLBACK cbDataCallBack, void* pUser)
{
    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        pNetChannel->SetDataCallBack(cbDataCallBack, pUser);
    }
}

// 获取UDP模式下RTP端口及连接对象;UDP需要申请成对rtp\rtcp端口，所以不能与下面的申请TCP端口的函数合并;
int CNetChannelMgr::GetUdpSocketPort(const char* pszLocalRecvStreamIp, 
                                     unsigned int& uiRtpPort, CConnection* &pRtpConnection, CConnection* &pRtcpConnection)
{
    int iRet = IVS_PARA_INVALID;
    if (NULL == pszLocalRecvStreamIp)
    {
        IVS_LOG(IVS_LOG_ERR, "GetUdpSocketPort", "local receive stream ip error");
        return iRet;
    }

    //是否绑定成功标志
    bool bSuccess = false;
    unsigned int uiPort = 0;
    (void)IVS_NEW((CUdpConnection*&)pRtpConnection);
	if (NULL == pRtpConnection)
	{
		IVS_LOG(IVS_LOG_ERR, "GetUdpSocketPort", "create udp rtp connection error");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)IVS_NEW((CUdpConnection*&)pRtcpConnection);
	if (NULL == pRtcpConnection)
	{
		IVS_DELETE(pRtpConnection);
		IVS_LOG(IVS_LOG_ERR, "GetUdpSocketPort", "create udp rtcp connection error");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //上锁保护;
    m_mutexFreePortList.Lock();
    for (unsigned int i = 0; i < m_lstFreePort.size();i++)
    {
        //取出最前面一个元素;
        uiPort = m_lstFreePort.front();

        //循环尝试3次绑定
        for (unsigned int uiBindCount = 0; uiBindCount < NET_SOURCE_BIND_PORT_RETRY; uiBindCount++)
        {
            //绑定RTP端口
            iRet = pRtpConnection->Bind(pszLocalRecvStreamIp, uiPort);
            //绑定RTCP端口
            iRet += pRtcpConnection->Bind(pszLocalRecvStreamIp, uiPort + 1);

            if (IVS_SUCCEED != iRet)
            {
                pRtpConnection->ShutDown();
                pRtcpConnection->ShutDown();
                IVS_LOG(IVS_LOG_ERR, "GetUdpSocketPort", "bind socket[%d %d] error, bind times[%d]", uiPort, uiPort + 1, uiBindCount);
                continue;
            }

            //绑定成功;
            bSuccess = true;
            break; // break for bind.
        } // end for bind

        //绑定成功跳出循环;
        if (bSuccess)
        {
            // 绑定成功，从free list中弹出;
            m_lstFreePort.pop_front();
            break; // break for list.
        }
        // 绑定不成功，将该端口插入到空闲队列最后，进行下一轮循环;
        else
        {
            m_lstFreePort.pop_front();
            m_lstFreePort.push_back(uiPort);
        }

    }//end for list
    m_mutexFreePortList.UnLock();
    
    if (bSuccess)
    {
        //取出的端口号可用，绑定后将其插入busylist的头部
        m_mutexBusyPortListLock.Lock();
        m_lstBusyPort.push_front(uiPort);   // 插入rtp端口;
        m_mutexBusyPortListLock.UnLock();

        uiRtpPort = uiPort;

        IVS_LOG(IVS_LOG_DEBUG, "GetUdpSocketPort", "get rtp rtcp port success.");
        iRet = IVS_SUCCEED;
    }
    else
    {
        //没有可用端口号;
        IVS_DELETE(pRtpConnection);
        IVS_DELETE(pRtcpConnection);
        IVS_LOG(IVS_LOG_ERR, "GetUdpSocketPort", "ALL free port are used by other programs, get rtp rtcp port fail.");
        iRet = IVS_PLAYER_RET_ALL_PORT_BUSY_ERROR;
    }
    return iRet;
}

// 设置UDP模式下RTP端口及连接对象;UDP需要设置成对rtp\rtcp端口，所以不能与下面的设置TCP端口的函数合并;
int CNetChannelMgr::SetUdpSocketPort(const char* pszLocalRecvStreamIp, 
	unsigned int uiRtpPort, 
	CConnection* &pRtpConnection, 
	CConnection* &pRtcpConnection)
{
	int iRet = IVS_FAIL;
	if (NULL == pszLocalRecvStreamIp)
	{
		IVS_LOG(IVS_LOG_ERR, "SetUdpSocketPort", "local receive stream ip error");
		return iRet;
	}

	//是否绑定成功标志
	bool bSuccess = false;
	(void)IVS_NEW((CUdpConnection*&)pRtpConnection);
	if (NULL == pRtpConnection)
	{
		IVS_LOG(IVS_LOG_ERR, "SetUdpSocketPort", "create udp rtp connection error");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)IVS_NEW((CUdpConnection*&)pRtcpConnection);
	if (NULL == pRtcpConnection)
	{
		IVS_DELETE(pRtpConnection);
		IVS_LOG(IVS_LOG_ERR, "SetUdpSocketPort", "create udp rtcp connection error");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	//循环尝试3次绑定
	for (unsigned int uiBindCount = 0; uiBindCount < NET_SOURCE_BIND_PORT_RETRY; uiBindCount++)
	{
		iRet  = pRtpConnection->Bind(pszLocalRecvStreamIp,uiRtpPort);
		iRet += pRtcpConnection->Bind(pszLocalRecvStreamIp,uiRtpPort+1);
		if(IVS_SUCCEED != iRet)
		{
			pRtpConnection->ShutDown();
			pRtcpConnection->ShutDown();
			IVS_LOG(IVS_LOG_ERR, "SetUdpSocketPort", "bind socket[%d %d] error, bind times[%d]", uiRtpPort, uiRtpPort + 1, uiBindCount);
			continue;;
		}
		//绑定成功;
		bSuccess = true;
		break; // break for bind.
	}

	if (bSuccess)
	{
		IVS_LOG(IVS_LOG_DEBUG, "GetUdpSocketPort", "Set rtp rtcp port success.");
		iRet = IVS_SUCCEED;
	}
	else
	{
		//绑定失败，释放内存
		IVS_DELETE(pRtpConnection);
		IVS_DELETE(pRtcpConnection);
		IVS_LOG(IVS_LOG_ERR, "SetUdpSocketPort", "Set Port Failed.");
		iRet = IVS_FAIL;
	}
	return iRet;
}

// 获取TCP模式下端口及连接对象;
int CNetChannelMgr::GetTcpSocketPort(const char* pszLocalRecvStreamIp, 
                                     unsigned int& uiPort, 
                                     CConnection* &pConnection)
{
    int iRet = IVS_PARA_INVALID;
    if (NULL == pszLocalRecvStreamIp)
    {
        IVS_LOG(IVS_LOG_ERR, "GetTcpSocketPort", "local receive stream ip error");
        return iRet;
    }

    //是否绑定成功标志
    bool bSuccess = false;
    unsigned int uiPortTmp = 0;
    (void)IVS_NEW((CTcpConnection*&)pConnection);
	if (NULL == pConnection)
	{
		IVS_LOG(IVS_LOG_ERR, "GetTcpSocketPort", "create tcp connection error");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //上锁保护;
    m_mutexFreePortList.Lock();
    for (unsigned int i = 0; i < m_lstFreePort.size();i++)
    {
        //取出最前面一个元素;
        uiPortTmp = m_lstFreePort.front();

        //循环尝试3次绑定
        for (unsigned int uiBindCount = 0; uiBindCount < NET_SOURCE_BIND_PORT_RETRY; uiBindCount++)
        {
            //绑定端口
            iRet = pConnection->Bind(pszLocalRecvStreamIp, uiPortTmp);

            if (IVS_SUCCEED != iRet)
            {
                pConnection->ShutDown();
                IVS_LOG(IVS_LOG_ERR, "GetTcpSocketPort", "bind socket[%d] error, bind times[%d]", uiPortTmp, uiBindCount);
                continue;
            }

            //绑定成功;
            bSuccess = true;
            break; // break for bind
        } //end for bind

        //绑定成功跳出循环;
        if (bSuccess)
        {
            // 绑定成功，从free list中弹出;
            m_lstFreePort.pop_front();
            break;// break for list		
        }	
        // 绑定不成功，将该端口插入到空闲队列最后，进行下一轮循环;
        else
        {
            m_lstFreePort.pop_front();
            m_lstFreePort.push_back(uiPort);
        }

    }//end for list
    m_mutexFreePortList.UnLock();

    if (bSuccess)
    {
        //取出的端口号可用，绑定后将其插入busylist的头部
        m_mutexBusyPortListLock.Lock();
        m_lstBusyPort.push_front(uiPortTmp);   // 插入端口;
        m_mutexBusyPortListLock.UnLock();

        uiPort = uiPortTmp;

        IVS_LOG(IVS_LOG_DEBUG, "GetTcpSocketPort", "get port success.");
        iRet = IVS_SUCCEED;
    }
    else
    {
        //没有可用端口号;
        IVS_DELETE(pConnection);
        IVS_LOG(IVS_LOG_ERR, "GetTcpSocketPort", "ALL free port are used by other programs, get port fail.");
        iRet = IVS_PLAYER_RET_ALL_PORT_BUSY_ERROR;
    }
    return iRet;
}

// 设置TCP模式下端口及连接对象;
int CNetChannelMgr::SetTcpSocketPort(const char* pszLocalRecvStreamIp, 
	unsigned int uiPort, 
	CConnection* &pConnection)
{
	int iRet = IVS_FAIL;
	if (NULL == pszLocalRecvStreamIp)
	{
		IVS_LOG(IVS_LOG_ERR, "SetTcpSocketPort", "local receive stream ip error");
		return iRet;
	}

	//是否绑定成功标志
	bool bSuccess = false;
	(void)IVS_NEW((CTcpConnection*&)pConnection);
	if (NULL == pConnection)
	{
		IVS_LOG(IVS_LOG_ERR, "GetTcpSocketPort", "create tcp connection error");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	//循环尝试3次绑定
	for (unsigned int uiBindCount = 0; uiBindCount < NET_SOURCE_BIND_PORT_RETRY; uiBindCount++)
	{
		//绑定端口
		iRet = pConnection->Bind(pszLocalRecvStreamIp, uiPort);

		if (IVS_SUCCEED != iRet)
		{
			pConnection->ShutDown();
			IVS_LOG(IVS_LOG_ERR, "SetTcpSocketPort", "bind socket[%d] error, bind times[%d]", uiPort, uiBindCount);
			continue;
		}

		//绑定成功;
		bSuccess = true;
		break; // break for bind
	} //end for bind

	if (bSuccess)
	{
		IVS_LOG(IVS_LOG_DEBUG, "SetTcpSocketPort", "Set port success.");
		iRet = IVS_SUCCEED;
	}
	else
	{
		//端口被绑定
		IVS_DELETE(pConnection);
		IVS_LOG(IVS_LOG_ERR, "SetTcpSocketPort", "Set port failed.");
		iRet = IVS_FAIL;
	}
	return iRet;

}

//释放VideoRtp端口号;
void CNetChannelMgr::FreePortfromList(unsigned int uiSocketPort)
{
    //BusyList上锁
    m_mutexBusyPortListLock.Lock();
    PortListIter iter = m_lstBusyPort.begin();
    PortListIter iterEnd = m_lstBusyPort.end();

    //遍历查找端口号，找到后从BusyList擦除
    for (; iter != iterEnd; )
    {		
        if(*iter == uiSocketPort)
        {
            m_lstBusyPort.erase(iter++);
			IVS_LOG(IVS_LOG_DEBUG, "FreePortfromList", "port[%d] erase from busy list success.", uiSocketPort);
            continue;
        }
        iter++;
    }
    m_mutexBusyPortListLock.UnLock();

    //FreeList上锁，将端口号插入FreeList
    m_mutexFreePortList.Lock();
	if (uiSocketPort >= NET_SOURCE_BEGIN_PORT && uiSocketPort <= NET_SOURCE_END_PORT)
	{
		m_lstFreePort.push_back(uiSocketPort);
		IVS_LOG(IVS_LOG_DEBUG, "FreePortfromList", "port[%d] push back success.", uiSocketPort);
	}
	else
	{
		IVS_LOG(IVS_LOG_ERR, "FreePortfromList", "port[%d] is not in range.", uiSocketPort);
	}
    m_mutexFreePortList.UnLock();
}

CNetChannel* CNetChannelMgr::GetChannelByID(unsigned int ulChannel)
{
    CAutoLock lock(m_mutexChannel);

    CNetChannel* pNetChannel = NULL;
    NetChannelMapIter iter = m_mapNetChannel.find(ulChannel);
    if (iter != m_mapNetChannel.end())
    {
        pNetChannel = dynamic_cast<CNetChannel*>(iter->second);//lint !e611
    }
    return pNetChannel;
}

// 设置Session信息;
void CNetChannelMgr::SetSessionInfo(unsigned long ulChannel, char* pSessionUrl, char* pSessionId)
{
    CNetChannel* pNetChannel = GetChannelByID(ulChannel);
    if (NULL != pNetChannel)
    {
        IVS_LOG(IVS_LOG_DEBUG, "SetSessionInfo", "ulChannel[%d],pSessionUrl[%s],pSessionId[%s].", 
                ulChannel, pSessionUrl, pSessionId);
        pNetChannel->SetSessionInfo(pSessionUrl, pSessionId);
        return;
    }
    IVS_LOG(IVS_LOG_ERR, "SetSessionInfo", "err.ulChannel[%d],pSessionUrl[%s],pSessionId[%s].", 
            ulChannel, pSessionUrl, pSessionId);
    return;
}
