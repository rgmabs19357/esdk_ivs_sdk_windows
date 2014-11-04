/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspSession.cpp
  作    者 : y00182336
  功能描述 : RTSP连接
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include "RtspSession.h"
#include "RtspReactorManager.h"
#include "RtspMessage.h"
#include "RtspSetupMessage.h"
#include "RtspPlayMessage.h"
#include "RtspSessionManager.h"
#include "RtspAnnounceMessage.h"
#include "RtspOptionsMessage.h"

const char* CRtspSession::m_strRtspMethod[] = {
    "RTSP/1.0",
    "SETUP",
    "PLAY",
    "PAUSE",
    "TEARDOWN",
    "ANNOUNCE",
    "DESCRIBE"
};

/***************************************************************************
 函 数 名 : CRtspSession
 功能描述 : 构造函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSession::CRtspSession()
{
    m_bBlocked        = true;
    m_tcpStream.set_handle(ACE_INVALID_HANDLE);

    m_fDataCallBack   = NULL;
    m_pUser           = NULL;
    m_pRtspMsg        = NULL;
    //m_strDate         = "";
    m_unPlayTimePos   = 0;

    m_pRtspSemaphore  = NULL;
    m_bRecvRetFlag    = false;

    m_enSessionStatus = RTSP_SESSION_STATUS_INIT;
    m_unCSeq          = 0;
    m_lTimerId        = -1;    
    
    m_pRecvBuffer     = NULL;

    m_unSessionIndex  = 0;
    m_bDelete         = false;

	m_bDataRecved = false;      // 接收数据标记，用于超时检测
	m_uTmoCounter = 0;          // 超时计数器
	m_lDataTimer  = -1;         // 数据接收检测定时器
	m_bEnableTmoCheck = false;  // 默认不使能超时检测

	m_iRetCode = 0;

	m_bConnectFail = false;
}

/***************************************************************************
 函 数 名 : ~CRtspSession
 功能描述 : 析构函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSession::~CRtspSession()
{
	{
		IVS_RUN_LOG_CRI("RtspSession Destroy %08x %08x", this, m_pRecvBuffer);
		
		ACE_Guard <ACE_Thread_Mutex> lockerSession (m_Mutex);
	    try
	    {
	        if (NULL != m_pRecvBuffer)
	        {
	            delete m_pRecvBuffer;
	            m_pRecvBuffer = NULL;
	        }
	    }
	    catch (...)
	    {
	    }

	    try
	    {
	        if (NULL != m_pRtspSemaphore)
	        {
	            delete m_pRtspSemaphore;
	            m_pRtspSemaphore = NULL;
	        }
	    }
	    catch (...)
	    {
	    }

		try
		{
			if (NULL != m_pRtspMsg)
			{
				delete m_pRtspMsg;
				m_pRtspMsg = NULL;
			}
		}
		catch (...)
		{
		}
	}
    m_fDataCallBack   = NULL;
    m_pUser           = NULL;
    m_pRtspMsg        = NULL;

	// 去注册后，该对象注册的定时器无效 ？ FIX ME!!!
}


/***************************************************************************
 函 数 名 : handle_timeout
 功能描述 : 异步方式下，响应消息超时回调函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 0-成功          -1-失败
***************************************************************************/
int CRtspSession::handle_timeout(const ACE_Time_Value &tv, const void *arg)
{
#if 0
	ACE_Guard <ACE_Thread_Mutex> locker (m_Mutex);
    if (NULL == m_pRtspMsg)
    {
        return RET_FAIL;
    }
    // 若是Teardown消息，则立即断开连接
    unsigned int unMethodType = m_pRtspMsg->getMethodType();
    if (RTSP_METHOD_TEARDOWN == unMethodType)
    {
        return disconnectPeer();
    }

    //取消超时定时器
    cancelRecvTimer();
    
    // 删除消息
    destroyRtspMsg();

    IVS_RUN_LOG_CRI("rtsp receive message timeout. method type[%u].", unMethodType);

    // 调用异常回调函数
    CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                          (unsigned int)unMethodType, 
                                                          (unsigned int)RTSP_ERR_TIMEOUT,
                                                          NULL,
                                                          CRtspSessionManager::instance()->m_pUser);
#endif 
	
	if (!m_bEnableTmoCheck)
	{
		return 0;
	}

	if (CheckRtpTimeout())
	{
		KillRtpTimer();
		CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
			(unsigned int)RTSP_ERR_TIMEOUT, 
			(unsigned int)RTSP_ERR_TIMEOUT,
			NULL,
			CRtspSessionManager::instance()->m_pUser);
	}
	else 
	{
		//KillRtpTimer();
		//m_lDataTimer = -1;
		//SetupRtpTimer();
	}
    return 0;
}


/***************************************************************************
 函 数 名 : connectPeer
 功能描述 : 建立连接
 输入参数 : remoteAddr      对端地址
 输出参数 : 无
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSession::connectPeer(const ACE_INET_Addr &remoteAddr)
{
    if ((m_bBlocked) && (NULL == m_pRtspSemaphore))
    {
        // 同步方式下，创建信号量
        try
        {
            m_pRtspSemaphore = new ACE_Thread_Semaphore(0);
        }
        catch(...)
        {
            IVS_RUN_LOG_CRI("create thread semaphore fail.");
            return RET_FAIL;
        }
    }

    ACE_SOCK_Connector peerConnector;
    ACE_Time_Value     tv((time_t)1);
    m_remoteAddr = remoteAddr;
    // 主动发起连接，避免重复连接
    if (ACE_INVALID_HANDLE != m_tcpStream.get_handle())
    {
        IVS_DBG_LOG("session already connected, handle[%p].", this);
        return RET_OK;
    }

	// 解决网络带宽占满时，建立tcp连接失败的问题
	bool bConFlag = false;
	int iConCount = 3;
	while(iConCount > 0)
	{
		if (-1 == peerConnector.connect(m_tcpStream, m_remoteAddr, &tv, ACE_Addr::sap_any, 1))
		{
			int iErrorNo = ACE_OS::last_error();
			IVS_DBG_LOG("connect [%s:%d] fail, error[%d].", 
						remoteAddr.get_host_addr(),
						remoteAddr.get_port_number(),
						iErrorNo);
			iConCount--;
			Sleep(50);
			continue;
		}
		else
		{
			bConFlag = true;
			break;
		}
	}

	if (!bConFlag)
	{
		if (m_tcpStream.get_handle() != ACE_INVALID_HANDLE)
		{
			m_tcpStream.close();
			m_tcpStream.set_handle(ACE_INVALID_HANDLE);
		}

		m_bConnectFail = true;
		IVS_DBG_LOG("connect [%s:%d] fail for three times.",
					remoteAddr.get_host_addr(),
					remoteAddr.get_port_number());
		return RET_FAIL;
	}

    unsigned int nSize = DEFAULT_BUFFER_SIZE;
    if (0 != ACE_OS::setsockopt(get_handle(), SOL_SOCKET, SO_RCVBUF, (const char*)&nSize, sizeof(nSize)))
    {
        IVS_RUN_LOG_WAR("Rtsp push session set recv buffer fail, errno=%d",
            errno);
    }


    // 设置非阻塞
#ifndef WIN32
    if (ACE_OS::fcntl(get_handle(), F_SETFL, ACE_OS::fcntl(get_handle(), F_GETFL) | O_NONBLOCK))
#else
    ULONG ulNoBlock = true;
    if (ACE_OS::ioctl(get_handle(), (long)(long)FIONBIO, &ulNoBlock))
#endif
    {
        IVS_RUN_LOG_WAR("connect[%s:%d] set options fail, errno[%d].",
            m_localAddr.get_host_addr(),
            m_localAddr.get_port_number(),
            errno);
        return RET_FAIL;
    }

    // 建立连接时，给m_localAddr成员赋值
    m_tcpStream.get_local_addr(m_localAddr);
    IVS_RUN_LOG_INF("Connect to [%s:%d] success, local port[%d].",
                    m_remoteAddr.get_host_addr(),
                    m_remoteAddr.get_port_number(),
                    m_localAddr.get_port_number());

    // 连接注册到反应器
    int nRet = CRtspReactorManager::instance()->RegistHandle(this, 
                                                             ACE_Event_Handler::READ_MASK);
    if (RET_OK != nRet)
    {
        //m_tcpStream.close();
        return RET_FAIL;
    } 
    return RET_OK;
}


/***************************************************************************
 函 数 名 : disconnectPeer
 功能描述 : 断开连接
 输入参数 : 无
 输出参数 : 无
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSession::disconnectPeer()
{
    // 取消定时器
    cancelRecvTimer();
    //destroyRtspMsg();
    // 撤销RTP接收定时器
	KillRtpTimer();

    // 检查连接是否建立
    ACE_HANDLE nHandle = m_tcpStream.get_handle();
    if (ACE_INVALID_HANDLE == nHandle)
    {
        return RET_FAIL;
    }

	// 拆除反应器
    int nRet = CRtspReactorManager::instance()->unRegistHandle((ACE_Event_Handler*)this,
                                                                ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);

    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("Unregist reactor fail,remote address[%s:%d].",
                        m_remoteAddr.get_host_addr(), m_remoteAddr.get_port_number());
    }

    // 注意！此处切不可释放缓存m_pRecvBuffer，会造出崩溃！！！
    // 原因：disconnect函数会在主线程终止时和监听线程两者调用，这两线程对缓存采用进程语义
    // 监听处delete并赋NULL后，主线程并未改变，造成两次delete，导致内存崩溃
    // 解决办法：在Session的析构函数里释放缓存，保证只释放一次即可

    // 关闭tcp连接
    (void)ACE_OS::shutdown(nHandle, -1);
    (void)m_tcpStream.close();
	m_tcpStream.set_handle(ACE_INVALID_HANDLE);


    //setStatus(RTSP_SESSION_STATUS_ABNORMAL);

    IVS_RUN_LOG_INF("RTSP session disconnect [%s:%d] success.",
                    m_remoteAddr.get_host_addr(), m_remoteAddr.get_port_number());
    return RET_OK;
}


/***************************************************************************
 函 数 名 : sendMessage
 功能描述 : 发送消息
 输入参数 : pDataBuffer         消息字符串
            unDataSize          消息长度
 输出参数 : 无
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSession::sendMessage(const char *pDataBuffer, unsigned int unDataSize)
{
    ACE_Guard <ACE_Thread_Mutex> locker (m_StreamMutex);
    int nRet = 0;
    unsigned int unBuffSize = unDataSize;
    IVS_RUN_LOG_INF("try to send message, len[%u]:\n%s", unDataSize, pDataBuffer);
    // 一次没有发送完需要重复发送
    do
    {
        nRet = m_tcpStream.send(pDataBuffer, unBuffSize);
        if (0 >= nRet)
        {
            int iErrorCode = ACE_OS::last_error();
            if (checkIsDisconnect(iErrorCode))
            {
                // 连接已经断开，发送失败，强制关闭连接，返回失败
                (void)disconnectPeer();
                return RTSP_ERR_DISCONNECT;
            }

            // 连接可用，但当前资源忙，需要主程序再次调用
            destroyRtspMsg();
            return RTSP_ERR_SEND_FAIL;
        }
    
        IVS_RUN_LOG_INF("send msg to [%s:%d] success, len[%d].", 
                        m_remoteAddr.get_host_addr(),
                        m_remoteAddr.get_port_number(),
                        nRet);
        
        pDataBuffer += nRet;
        unBuffSize -= (unsigned int)nRet;
    }while(unBuffSize > 0);

	IVS_RUN_LOG_INF("sendMessage [%s:%d] success.", 
					m_remoteAddr.get_host_addr(),
					m_remoteAddr.get_port_number());
    return RET_OK;
}



/***************************************************************************
 函 数 名 : destroyRtspMsg
 功能描述 : 删除RTSP消息类
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::destroyRtspMsg()
{
    if (NULL != m_pRtspMsg)
    {
        delete m_pRtspMsg;
        m_pRtspMsg = NULL;
        IVS_RUN_LOG_INF("delete Rtsp message success.");
    }
    return;
}


/***************************************************************************
 函 数 名 : getCSeq
 功能描述 : 获取RTSP消息的系列号
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 序列号
***************************************************************************/
unsigned int CRtspSession::getCSeq()
{
    return ++m_unCSeq;
}


/***************************************************************************
 函 数 名 : get_handle
 功能描述 : 获取handle句柄，用于反应器注册
 输入参数 : 无
 输出参数 : 无
 返 回 值 : >0-成功          -1-失败
***************************************************************************/
ACE_HANDLE CRtspSession::get_handle() const
{
    return m_tcpStream.get_handle();
}


/***************************************************************************
 函 数 名 : setRecvTimer
 功能描述 : 异步方式下，设置超时定时器
 输入参数 : timeout             超时时间
 输出参数 : 无
 返 回 值 : RET_OK-成功          RET_FAIL-失败
***************************************************************************/
int CRtspSession::setRecvTimer(unsigned int timeout)
{
    // 参数检查，避免定时器重复使用
    if ((0 == timeout) || (-1 != m_lTimerId))
    {
        return RET_FAIL;
    }
    time_t unTime = (time_t)timeout;
    ACE_Time_Value tv(unTime, 0);
    m_lTimerId = CRtspReactorManager::instance()->RegistTimer((ACE_Event_Handler*)this, m_unSessionIndex, tv, tv);
    if(-1 == m_lTimerId)
    {
        return RET_FAIL;
    }
    IVS_DBG_LOG("register receive timer success.");
    return RET_OK;
}



/***************************************************************************
 函 数 名 : cancelRecvTimer
 功能描述 : 取消超时定时器
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::cancelRecvTimer()
{
    if (-1 != m_lTimerId)
    {
        // 取消定时器
        (void)CRtspReactorManager::instance()->unRegistTimer(m_lTimerId, m_unSessionIndex);
        IVS_DBG_LOG("rtsp session[%p] cancel receive timer success.", this);
        m_lTimerId = -1;
    }
    return;
}


/***************************************************************************
 函 数 名 : handle_input
 功能描述 : 响应消息接收回调函数
 输入参数 : handle              句柄
 输出参数 : 无
 返 回 值 : 0-成功              -1-失败
***************************************************************************/
int CRtspSession::handle_input(ACE_HANDLE handle)
{
    ACE_Guard <ACE_Thread_Mutex> locker (m_Mutex);
    // 检查连接是否正常
    if ((ACE_INVALID_HANDLE == m_tcpStream.get_handle()) || (NULL == m_pRecvBuffer))
    {
		IVS_DBG_LOG("handle input fail, m_tcpStream handle is error[%d] or m_pRecvBuffer is null.", m_tcpStream.get_handle());
        return -1;
    }

    // 计算缓冲区剩余长度
    int iRecvLen = (int) m_pRecvBuffer->size() - (int) m_pRecvBuffer->length();
    if (0 >= iRecvLen)
    {
        // 缓冲区满了，不接收直接返回
        IVS_DBG_LOG("handle input fail, recv buff is full, iRecvLen=%d RecvBufferSize=%d Length=%d", iRecvLen, m_pRecvBuffer->size(), m_pRecvBuffer->length());
        //return 0;
    }
    else
    {
        //接收数据到缓冲区
        ACE_OS::last_error(0);
        iRecvLen = m_tcpStream.recv(m_pRecvBuffer->wr_ptr(), (size_t)iRecvLen);
        if (0 >= iRecvLen )
        {
            int iErrorCode = ACE_OS::last_error();
            if (checkIsDisconnect(iErrorCode))
            {
                (void)disconnectPeer();
                //唤醒阻塞线程,防止同步调用的CU继续等待。
                if (m_bBlocked)
                {
                    m_bRecvRetFlag = false;
                    m_pRtspSemaphore->release();
                }
                // 连接已经断开，调用回调函数通知用户
                CRtspSessionManager::instance()->m_fExceptionCallBack((int)this,
                                                                      RTSP_METHOD_NUM,
                                                                      RTSP_ERR_DISCONNECT,
                                                                      NULL, 
                                                                      CRtspSessionManager::instance()->m_pUser);
                return -1;
            }

            // 连接正常，资源忙，不做处理
            return 0;
        }

        //移动缓冲区读写指针
        m_pRecvBuffer->wr_ptr((size_t)(m_pRecvBuffer->length() + (size_t) iRecvLen));
        m_pRecvBuffer->rd_ptr((size_t) 0);
    }

    size_t processedSize = 0;
    size_t totalSize     = m_pRecvBuffer->length();
    int nSize            = 0;
    do
    {
        nSize = handleRecvedData(m_pRecvBuffer->rd_ptr() + processedSize,
                                 m_pRecvBuffer->length() - processedSize);
        if (nSize < 0)
        {
            // 处理返回失败
            IVS_RUN_LOG_WAR("rtsp connect handle recv data fail.");
            return -1;
        }

        if (0 == nSize)
        {
			IVS_RUN_LOG_WAR("handleRecvedData nSize = 0.");
            // 缓冲区中剩余数据长度不足下一个消息了
            break;
        }

		// 接收RTP报文标记为真
		m_bDataRecved = true;

        processedSize += (size_t)nSize;
    }while(processedSize < totalSize);

	//IVS_DBG_LOG("handleRecvedData out.");
	
	// 数据处理成功，更新接收缓冲区指针
    size_t dataSize = m_pRecvBuffer->length() - processedSize;
    (void) m_pRecvBuffer->copy(m_pRecvBuffer->rd_ptr() + processedSize, dataSize);
    m_pRecvBuffer->rd_ptr((size_t) 0);
    m_pRecvBuffer->wr_ptr(dataSize);
    return 0;
}


/***************************************************************************
 函 数 名 : handle_close
 功能描述 : 关闭连接回调函数
 输入参数 : handle              句柄
            close_mask          取消注册事件
 输出参数 : 无
 返 回 值 : 0-成功              -1-失败
***************************************************************************/
int CRtspSession::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
    if (RTSP_SESSION_STATUS_INIT == getStatus())
    {
        // 连接已经被关闭，直接返回
        return -1;
    }

    // 关闭连接，设置连接状态
    (void)disconnectPeer();
    IVS_RUN_LOG_INF("close rtsp session success,local addr[%s:%d].",
                    m_localAddr.get_host_addr(), m_localAddr.get_port_number());
    return 0;
}

/***************************************************************************
 函 数 名 : handleRecvedData
 功能描述 : 处理接收的消息
 输入参数 : pData               消息字符串
            unDataSize          消息长度
 输出参数 : 无
 返 回 值 : 成功：>=0，如果解析出消息，则返回实际处理的消息长度。如果消息未接收完，则返回0
            失败：<0
***************************************************************************/
int CRtspSession::handleRecvedData(const char* pData, unsigned int unDataSize)
{
    /*IVS_DBG_LOG("handle received data message from [%s:%d], len[%u].", 
                m_remoteAddr.get_host_addr(), m_remoteAddr.get_port_number(),
                unDataSize);*/

    int nRet = 0;
    if (RTP_INTERLEAVE_FLAG == pData[0]) 
    {
        // 处理媒体消息
        nRet = parseRtpRtcpPacket(pData, unDataSize);
    }
    else if (isRtspMessage(pData, unDataSize))
    {
        // 处理RTSP消息，包括请求和响应
        nRet = (int)parseRtspPacket(pData, unDataSize);
    }    
    else
    {   
        if ((RTSP_SESSION_STATUS_PLAYING == getStatus()) && (NULL != m_fDataCallBack))
        {
            // 其他消息，直接回调
            m_fDataCallBack((int)this, const_cast<char*>(pData), unDataSize, m_pUser);
        }
        nRet = (int)unDataSize;
    }
	//IVS_DBG_LOG("handleRecvedData out.");
    return nRet;
}

// 处理媒体消息
int CRtspSession::parseRtpRtcpPacket(const char* pData, unsigned int unDataSize)
{
    if (unDataSize < RTP_INTERLEAVE_LENGTH)
    {
		IVS_RUN_LOG_WAR("parseRtpRtcpPacket failed, unDataSize[%u].", unDataSize);
        // 剩余数据不够解析，返回0
        return 0;
    }

    // 根据交织长度解析出RTP或RTCP包的长度
    unsigned int unSize = (unsigned int) ACE_NTOHS(*(unsigned short*)const_cast<char*>(&pData[2])); //lint !e826
    if (unDataSize - RTP_INTERLEAVE_LENGTH < unSize)
    {
		IVS_RUN_LOG_WAR("parseRtpRtcpPacket jiaozhi, unDataSize[%u].", unDataSize);
        // 数据没有接收完，暂时返回
        return 0;
    }
    unSize += RTP_INTERLEAVE_LENGTH;

    // 媒体消息，检查状态，调用回调函数
    //if (RTSP_SESSION_STATUS_PLAYING == getStatus())
    
    if(NULL != m_fDataCallBack)
    {
	    m_fDataCallBack((int)this, const_cast <char*> (pData), (unsigned int)unSize, m_pUser);
    }
        
     // 返回实际消息长度
	//IVS_DBG_LOG("parseRtpRtcpPacket succeed.");
    return (int)unSize;
}

// 处理RTSP消息
unsigned int CRtspSession::parseRtspPacket(const char* pData, unsigned int unDataSize)
{
	IVS_RUN_LOG_WAR("parseRtspPacket in.");
    // 先确定边界
    string strRecvedData = "";
    strRecvedData.append(pData, unDataSize);
    string::size_type nPos = strRecvedData.find(RTSP_MSG_END_TAG);
    if (string::npos == nPos)
    {
		IVS_RUN_LOG_WAR("parseRtspPacket out npos.");
        // 消息没有接收完全，返回0
        return 0;
    }
    unsigned int unSize = (unsigned int)(nPos + strlen(RTSP_MSG_END_TAG));
    // 如果存在Content-Length:字段，加上该字段长度
    string::size_type nLenPos = strRecvedData.find(RTSP_TOKEN_STR_CONTENT_LENGTH);
    if ((string::npos != nLenPos) && (nPos > nLenPos))
    {
        string strValue = "";
        strValue = strRecvedData.substr(nLenPos + strlen(RTSP_TOKEN_STR_CONTENT_LENGTH));
        string strCententLen = "";
        strCententLen = strValue.substr(0, strValue.find(RTSP_END_TAG));
        unsigned int unContentLen = (unsigned int)ACE_OS::atoi(strCententLen.c_str());
        unSize += unContentLen;
    }
    if (unSize > unDataSize)
    {
		IVS_RUN_LOG_WAR("parseRtspPacket out 1 unSize[%u] unDataSize[%u].", unSize, unDataSize);
        return 0;
    }

    if (NULL != m_pRtspMsg)
    {
        if (m_pRtspMsg->isRtspRespMsg(pData, unSize))
        {
            // 解析响应消息
            int nRet = m_pRtspMsg->decodeMessage(pData, unSize);
            // 错误码也要抛到上层，此处不能增加这个逻辑.
			/*if (RET_OK != nRet)
            {
                IVS_RUN_LOG_CRI("decode message fail.");
                return 0;
            }*/

            string strBuffer = "";
            strBuffer.append(pData, unSize);
            IVS_DBG_LOG("start decode rtsp message, len[%u]:\n%s", unSize, strBuffer.c_str());
            // 处理RTSP响应消息，带上解析结果
            (void)handleRtspRespMsg(nRet,pData, unSize);
			IVS_RUN_LOG_WAR("parseRtspPacket out 2.");
            return unSize;
        }
    }
    // 剩下的只可能是RTSP请求消息，暂时只处理ANNOUNCE请求
    if(0 == strncmp(pData, RTSP_ANNOUNCE_MSG, strlen(RTSP_ANNOUNCE_MSG)))
    {
        int nRet = handleRtspAnnounceMsg(pData, unSize);
        if (RET_OK != nRet)
        {
            // 处理失败，忽略
            IVS_RUN_LOG_CRI("hande rtsp announce message fail.");
            return unSize;
        }
    }
	IVS_RUN_LOG_WAR("parseRtspPacket out.");
    return unSize;
}

/***************************************************************************
 函 数 名 : handleRtspAnnounceMsg
 功能描述 : 解析和处理ANNOUNCE请求,需要考虑没有SDP的情况，直接回响应，不回调
 输入参数 : psRtspMsg           消息字符串
            nLen                消息长度
 输出参数 : 无
 返 回 值 : RET_OK-成功         RET_FAIL-失败
***************************************************************************/
int CRtspSession::handleRtspAnnounceMsg(const char* psRtspMsg, const unsigned int nLen)
{

    CRtspAnnounceMessage* pAnnounceMsg = NULL;
    try
    {
        pAnnounceMsg = new CRtspAnnounceMessage;
    }
    catch (...)
    {
        pAnnounceMsg = NULL;
    }

    if (NULL == pAnnounceMsg)
    {
        IVS_RUN_LOG_CRI("session ANNOUNCE msg response is invalid");
        return RET_FAIL;
    }

    string AnnounceResp = "";
    int nRet = pAnnounceMsg->decodeMessage(psRtspMsg, nLen);
    if (RET_OK != nRet)
    {
        pAnnounceMsg->setStatusCode(RTSP_CLIENT_BAD_REQUEST);
        (void) pAnnounceMsg->encodeMessage(AnnounceResp);
    }
    else
    {
        pAnnounceMsg->setStatusCode(RTSP_SUCCESS_OK);
        (void) pAnnounceMsg->encodeMessage(AnnounceResp);
    }

    // 发送响应成功且SDP字段不为空时，调用回调函数
    if (RET_OK == sendMessage(AnnounceResp.c_str(), AnnounceResp.length())
        && (RET_OK == nRet) && ("" != pAnnounceMsg->getRtspSdp()))
    {
        string strConntent = "";
        strConntent = pAnnounceMsg->getRtspSdp();
        string::size_type nPos = strConntent.find(RTSP_ANNOUNCE_CONTENT);
        if (string::npos == nPos)
        {
            // 调用回调函数，将SDP字段传出去
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this,
                RTSP_METHOD_ANNOUNCE,  
                RET_OK, 
                (void*)const_cast<char*>(pAnnounceMsg->getRtspSdp().c_str()),
                CRtspSessionManager::instance()->m_pUser);
        }
        else
        {
            string strValue = "";
            strValue = strConntent.substr(nPos + strlen(RTSP_ANNOUNCE_CONTENT));
            int nValue = ACE_OS::atoi(strValue.c_str());
            unsigned int unRet = RTSP_ERR_RET_SYS;
            // 正常结束
            if (RTSP_NORMAL_STOP == nValue)
            {
                unRet = RTSP_ERR_NORMAL_STOP;
            }
            // 异常结束
            else if(RTSP_ABNORMAL_STOP == nValue)
            {
                unRet = RTSP_ERR_ABNORMAL_STOP;
            }
            // 调用回调函数，将SDP字段传出去
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this,
                RTSP_METHOD_ANNOUNCE,  
                unRet, 
                (void*)const_cast<char*>(pAnnounceMsg->getRtspSdp().c_str()),
                CRtspSessionManager::instance()->m_pUser);
        }
    }
    delete pAnnounceMsg;
    return RET_OK;
}



/***************************************************************************
 函 数 名 : setStatus
 功能描述 : 设置连接状态
 输入参数 : status              更改后的连接状态
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::setStatus(unsigned int status)
{
    ACE_Guard<ACE_Thread_Mutex>  locker(m_StatusMutex);
    if (RTSP_SESSION_STATUS_DELETE < status)//lint !e685
    {
        IVS_RUN_LOG_CRI("set status from [%d] to [%u] fail.", getStatus(), status);
        return;
    }
    IVS_DBG_LOG("set status from [%d] to [%u].", getStatus(), status);
    m_enSessionStatus = (RTSP_SESSION_STATUS)status;
}


/***************************************************************************
 函 数 名 : getStatus
 功能描述 : 获取当前连接状态
 输入参数 : 无
 输出参数 : 无
 返 回 值 : status              更改后的连接状态
***************************************************************************/
RTSP_SESSION_STATUS CRtspSession::getStatus() const
{
    //ACE_Guard<ACE_Thread_Mutex>  locker(m_StatusMutex);
    return m_enSessionStatus;
}


/***************************************************************************
 函 数 名 : handleRtspRespMsg
 功能描述 : 处理RTSP响应消息
 输入参数 : nRet                消息解析结果
 输出参数 : 无
 返 回 值 : RET_OK-成功         RET_FAIL-失败
***************************************************************************/
int CRtspSession::handleRtspRespMsg(const int nRet,const char* psRtspMsg, const unsigned int nLen)
{
    if (NULL == m_pRtspMsg)
    {
        IVS_DBG_LOG("message handle is invalid.");
        return RET_FAIL;
    }

    IVS_RUN_LOG_INF("handle rtsp response message, CSeq[%u], ret[%d].", 
                    m_pRtspMsg->getCSeq(), nRet);

    switch(m_pRtspMsg->getMethodType())
    {
    case RTSP_METHOD_SETUP:     // setup消息响应
        handleRtspSetupMsg(nRet);
        break;
    case RTSP_METHOD_PLAY:      // play消息响应
        handleRtspPlayMsg(nRet);
        break;
    case RTSP_METHOD_PAUSE:     // pause消息响应
        handleRtspPauseMsg(nRet);
        break;
    case RTSP_METHOD_TEARDOWN:  // teardown消息响应
        handleRtspTeardownMsg(nRet);
        break;
    case RTSP_METHOD_OPTIONS:
        handleRtspOptionsMsg(nRet,psRtspMsg,nLen);
        break;
    default:                    // 非法状态
        return RET_FAIL;
    }
    return RET_OK;
}



/***************************************************************************
 函 数 名 : handleRtspSetupMsg
 功能描述 : 处理RTSP Setup响应，异步方式在此回调；同步方式通告主程序
 输入参数 : nRet                消息解析结果
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::handleRtspSetupMsg(const int nRet)
{
    // 取消定时器
    cancelRecvTimer();

    // 检查状态
    if ((RTSP_SESSION_STATUS_INIT == getStatus()) && (RET_OK == nRet))
    {
        setStatus(RTSP_SESSION_STATUS_READY);
        // 同步方式下，将结果通知主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = true;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 异步方式下，TCP媒体流连接不需要Peer写入地址
            CRtspSetupMessage* pSetupMsg = dynamic_cast<CRtspSetupMessage*>(m_pRtspMsg);
            if (NULL == pSetupMsg)
            {
                IVS_RUN_LOG_CRI("handle rtsp setup message response fail.");
                // 释放资源
                destroyRtspMsg();
                CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                      (unsigned int)RTSP_METHOD_SETUP,
                                                                      (unsigned int)RTSP_ERR_RET_SYS,
                                                                      NULL,
                                                                      CRtspSessionManager::instance()->m_pUser);
                return;
            }
            if (pSetupMsg->m_bTcpFlag)
            {
                // 释放资源
                destroyRtspMsg();
                CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                      (unsigned int)RTSP_METHOD_SETUP,
                                                                      (unsigned int)RET_OK,
                                                                      NULL,
                                                                      CRtspSessionManager::instance()->m_pUser);
            }
            // UDP连接方式下，传入Peer写入地址
            else
            {
                ACE_INET_Addr addr;
                addr.set((unsigned short)0, ntohl(pSetupMsg->getServerIp()));
                string strMediaAddr = "";
				const char* pszHostAddr = addr.get_host_addr();
				string strHostAddr = ((NULL == pszHostAddr) ? "" : pszHostAddr);
                strMediaAddr.append(strHostAddr.c_str());
                strMediaAddr += ":";
                stringstream buf;
                buf << pSetupMsg->getServerVideoPort();
                strMediaAddr += buf.str();
                strMediaAddr += ",";
                buf.str("");
                buf << pSetupMsg->getServerAudioPort();
                strMediaAddr += buf.str();
                strMediaAddr += ";";
                // 释放资源
                destroyRtspMsg();
                CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                      (unsigned int)RTSP_METHOD_SETUP,
                                                                      (unsigned int)RET_OK,
                                                                      (void*)const_cast<char*>(strMediaAddr.c_str()),
                                                                      CRtspSessionManager::instance()->m_pUser);
            }
            
        }
        return;
    }
    else
    {
        // 状态异常
        IVS_RUN_LOG_CRI("Setup messsage response is invalid.Session status[%u], response[%d].",
                        (unsigned int)getStatus(), nRet);
        // 同步方式下，将结果通告主程序
        if (m_bBlocked)
        {   
            m_bRecvRetFlag = false;
			m_iRetCode = nRet;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 释放资源
            destroyRtspMsg();
            // 异步方式下，调用回调函数
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                  (unsigned int)RTSP_METHOD_SETUP,
                                                                  (unsigned int)RTSP_ERR_STAUTS_ABNORMAL,
                                                                  NULL,
                                                                  CRtspSessionManager::instance()->m_pUser);
        }
        return;
    }

}


/***************************************************************************
 函 数 名 : handleRtspPlayMsg
 功能描述 : 处理RTSP Play响应消息，异步方式在此回调；同步方式通告主程序
 输入参数 : nRet                消息解析结果
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::handleRtspPlayMsg(const int nRet)
{
    // 取消定时器
    cancelRecvTimer();

    // 释放资源
    destroyRtspMsg();

    // 检查状态
    if ((RTSP_SESSION_STATUS_INIT != getStatus()) && (RET_OK == nRet))
    {
        setStatus(RTSP_SESSION_STATUS_PLAYING);
        // 同步方式下，通告主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = true;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 调用异常回调函数
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                  (unsigned int)RTSP_METHOD_PLAY,
                                                                  (unsigned int)RET_OK,
                                                                  NULL,
                                                                  CRtspSessionManager::instance()->m_pUser);
        }
    }
    else
    {
        IVS_RUN_LOG_CRI("session [%p] Play msg response is invalid. Session status[%u], response[%d].",
                        this, (unsigned int)getStatus(), nRet);
        // 同步方式下，通告主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = false;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 状态异常，调用回调函数，不做处理
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                  (unsigned int)RTSP_METHOD_PLAY,
                                                                  (unsigned int)RTSP_ERR_STAUTS_ABNORMAL,
                                                                  NULL,
                                                                  CRtspSessionManager::instance()->m_pUser);
            
        }        
    }

    return;
}


/***************************************************************************
 函 数 名 : handleRtspOptionsMsg
 功能描述 : 处理RTSP Options响应消息，记录当前时间
 输入参数 : nRet                消息解析结果
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::handleRtspOptionsMsg(int nRet,const char* psRtspMsg, const unsigned int nLen)
{
    // 取消定时器
    cancelRecvTimer();

    // 释放资源
    destroyRtspMsg();

    CRtspOptionsMessage* pOptionsMsg = NULL;
    try
    {
        pOptionsMsg = new CRtspOptionsMessage;
    }
    catch(...)
    {
        pOptionsMsg = NULL;
    }

    if (NULL == pOptionsMsg)
    {
        IVS_RUN_LOG_CRI("session OPTIONS msg response is invalid");
        return ;
    }
    string OptionResp = "";
    nRet += pOptionsMsg->decodeMessage(psRtspMsg, (int)nLen);

    // 检查状态
    if ((RTSP_SESSION_STATUS_INIT != getStatus()) && (RET_OK == nRet))
    {
        // 同步方式下，通告主程序
        if (m_bBlocked)
        {
            m_strTimeInfo = pOptionsMsg->m_strTimeinfo;
            m_bRecvRetFlag = true;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 调用异常回调函数
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                (unsigned int)RTSP_METHOD_OPTIONS,
                (unsigned int)RET_OK,
                (void*)pOptionsMsg->m_strTimeinfo.c_str(),          //lint !e1773
                CRtspSessionManager::instance()->m_pUser);
        }
    }
    else
    {
        IVS_RUN_LOG_CRI("session [%p] OPTIONS msg response is invalid. Session status[%u], response[%d].",
            this, (unsigned int)getStatus(), nRet);
        // 同步方式下，通告主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = false;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 状态异常，调用回调函数，不做处理
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                (unsigned int)RTSP_METHOD_OPTIONS,
                (unsigned int)RTSP_ERR_STAUTS_ABNORMAL,
                NULL,
                CRtspSessionManager::instance()->m_pUser);
        }
    }

    if (NULL != pOptionsMsg) //lint !e774
    {
        delete pOptionsMsg;
        pOptionsMsg = NULL;
    }
    return ;    //lint !e438
}


/***************************************************************************
 函 数 名 : handleRtspOptionsMsg
 功能描述 : 处理RTSP Pause响应消息
 输入参数 : nRet                消息解析结果
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::handleRtspPauseMsg(const int nRet)
{
    // 取消定时器
    cancelRecvTimer();

    // 释放资源
    destroyRtspMsg();

    // 如果READY状态，直接返回
    if (RTSP_SESSION_STATUS_READY == getStatus())
    {
        return;
    }

    // 正常状态
    if ((RTSP_SESSION_STATUS_PLAYING == getStatus()) && (RET_OK == nRet))
    {
        setStatus(RTSP_SESSION_STATUS_READY);
        // 同步方式下，将结果通知主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = true;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 异步方式下，调用异常回调函数
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                  (unsigned int)RTSP_METHOD_PAUSE,
                                                                  (unsigned int)RET_OK,
                                                                  NULL,
                                                                  CRtspSessionManager::instance()->m_pUser);
        }
    }
    else
    {
        IVS_RUN_LOG_CRI("Session [%p] Pause messsage response is invalid.Session status[%u], response[%d].",
                        this, (unsigned int)getStatus(), nRet);
        // 同步方式下，将结果通知主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = false;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 状态异常，调用回调函数，不做处理
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                  (unsigned int)RTSP_METHOD_PAUSE,
                                                                  (unsigned int)RTSP_ERR_STAUTS_ABNORMAL,
                                                                  NULL,
                                                                  CRtspSessionManager::instance()->m_pUser);
        }
    }
    return;
}


/***************************************************************************
 函 数 名 : handleRtspTeardownMsg
 功能描述 : 处理RTSP Teardown响应消息
 输入参数 : nRet                消息解析结果
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSession::handleRtspTeardownMsg(const int nRet)
{
    // INIT状态或响应消息解析错误，则直接返回
    if (RTSP_SESSION_STATUS_INIT == getStatus())
    {
        return;
    }

    (void)disconnectPeer();

    // 正常状态
    if ((RET_OK == nRet))
    {
        // 同步方式下，将结果通知主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = true;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 异步方式下，调用异常回调函数
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                  (unsigned int)RTSP_METHOD_TEARDOWN,
                                                                  (unsigned int)RET_OK,
                                                                  NULL,
                                                                  CRtspSessionManager::instance()->m_pUser);
        }
    }
    else
    {
        // 返回结果异常，保留连接和资源，等待下次发送Teradown消息
        IVS_RUN_LOG_CRI("Teardown messsage response is invalid.Session status[%u], response[%d].",
                        (unsigned int)getStatus(), nRet);
        // 同步方式下，将结果通知主程序
        if (m_bBlocked)
        {
            m_bRecvRetFlag = false;
            m_pRtspSemaphore->release();
        }
        else
        {
            // 状态异常，调用回调函数，不做处理
            CRtspSessionManager::instance()->m_fExceptionCallBack((int)this, 
                                                                  (unsigned int)RTSP_METHOD_TEARDOWN,
                                                                  (unsigned int)RTSP_ERR_STAUTS_ABNORMAL,
                                                                  NULL,
                                                                  CRtspSessionManager::instance()->m_pUser);
        }
    }
    return;
}


//********************************内部实现**********************//
/***************************************************************************
 函 数 名 : checkIsDisconnect
 功能描述 : 检查连接是否断开
 输入参数 : nErrNo              连接错误码
 输出参数 : 无
 返 回 值 : true-断开           false-未断开
***************************************************************************/
bool CRtspSession::checkIsDisconnect(int nErrNo) const
{
#ifndef WIN32
    if ((EAGAIN == nErrNo) 
        || (ETIME == nErrNo) 
        || (EWOULDBLOCK == nErrNo) 
        || (ETIMEDOUT == nErrNo) 
        || (EINTR == nErrNo))
#else
    if ((WSATRY_AGAIN == nErrNo) 
        || (WSAEWOULDBLOCK == nErrNo) 
        || (WSAETIMEDOUT == nErrNo) 
        || (WSAEINTR == nErrNo))

#endif
    {
        return false;
    }

    IVS_DBG_LOG("[%s:%d] is disconnected. error code [%d].",
				m_remoteAddr.get_host_addr(), m_remoteAddr.get_port_number(),
				nErrNo);
    return true;

}


/***************************************************************************
 函 数 名 : isRtspMessage
 功能描述 : 检查是否是RTSP消息
 输入参数 : psRtspMsg           消息字符串
            nLen                消息长度
 输出参数 : 无
 返 回 值 : true-是             false-否
***************************************************************************/
bool CRtspSession::isRtspMessage(const char* psRtspMsg,const unsigned int unLen)
{
    // 入参有效性检查
    if (NULL == psRtspMsg)
    {
		IVS_DBG_LOG("isRtspMessage failed, psRtspMsg = NULL .");
        return false;
    }

	if (0 == unLen)
	{
		IVS_DBG_LOG("isRtspMessage failed, unLen = 0.");
		return false;
	}

    unsigned int i = 0;
    for (; i < RTSP_METHOD_NUM; i++)
    {
        if (0 == strncmp(psRtspMsg, m_strRtspMethod[i], strlen(m_strRtspMethod[i])))
        {
            break;
        }
    }
    if (i >= RTSP_METHOD_NUM)
    {
        // 非RTSP消息返回非法头
		IVS_DBG_LOG("isRtspMessage failed, i[%u] >= RTSP_METHOD_NUM.", i);
        return false;
    }
    // TODO 确定RTSP消息边界
    return true;
} //lint !e1762

void CRtspSession::setDelFlag()
{
    m_bDelete = true;
}

bool CRtspSession::getDelFalg()
{
    return m_bDelete;
}

/*
 * SetupRtpTimer
 * 创建RTP 数据流检测定时器
 */
int CRtspSession::SetupRtpTimer()
{
	// 参数检查，避免定时器重复使用
	//ACE_Guard <ACE_Thread_Mutex> locker (m_Mutex);
	if (0 <= m_lDataTimer)
	{
		return RET_FAIL;
	}

	ACE_Time_Value tv(RTP_DATA_INTERVAL_MS, 0);
	m_lDataTimer = CRtspReactorManager::instance()->RegistTimer((ACE_Event_Handler*)this, m_unSessionIndex, tv, tv);
	if(-1 == m_lDataTimer)
	{
		return RET_FAIL;
	}
	IVS_DBG_LOG("register RTP receive timer success.");
	return RET_OK;

}

/*
 * KillRtpTimer
 * 删除RTP数据检测定时器
 */
void CRtspSession::KillRtpTimer()
{
	//ACE_Guard <ACE_Thread_Mutex> locker (m_Mutex);
	if (-1 != m_lDataTimer)
	{
		// 取消定时器
		(void)CRtspReactorManager::instance()->unRegistTimer(m_lDataTimer, m_unSessionIndex);
		IVS_DBG_LOG("rtsp session[%p] cancel receive timer success.", this);
		m_lDataTimer = -1;
	}
	return;
}

bool CRtspSession::CheckRtpTimeout()
{
	if (m_bDataRecved)
	{
		m_bDataRecved = false;
		m_uTmoCounter = 0;
		return false;
	}

	// 接收标识为 false
    m_uTmoCounter++;
	if (RTP_TIME_OUT_MAX <= m_uTmoCounter)
	{
		return true;
	}
	return false;
}

int CRtspSession::InitData()
{
	// 创建缓冲区
	if (NULL == m_pRecvBuffer)
	{
		try
		{
			m_pRecvBuffer = new Static_PreAssign_Buffer;
		}
		catch(...)
		{
			IVS_RUN_LOG_CRI("create m_pRecvBuffer fail.");
			return RET_FAIL;
		}
		m_pRecvBuffer->wr_ptr(0);
		m_pRecvBuffer->rd_ptr(0);
	}

	return RET_OK;
}
