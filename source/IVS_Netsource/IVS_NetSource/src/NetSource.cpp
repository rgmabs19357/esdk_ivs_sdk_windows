#include "NetSource.h"
#include "ChannelInfo.h"
#include "IVSCommon.h"
#include "ivs_log.h"
#include "_BaseThread.h"
#include "Connection.h"

CNetSource::CNetSource(void)
    : m_pChannelInfo(NULL)
	, m_pSourceThread(NULL)
    , m_pRecvBuf(NULL)
    , m_ulRecvBufLen(0)
    , m_ulLastSendNatTick(0)
	, m_ulLastCheckTimeoutTick(0)
    , m_bRun(false)
{
   
}

CNetSource::~CNetSource(void)
{
    Release();
    m_ulLastSendNatTick = 0;
	m_ulLastCheckTimeoutTick = 0;
    m_bRun = false;
}

// 工作主循环接口定义，里面完成工作循环逻辑;
void CNetSource::MainLoop()
{
	while (true)//lint !e716
	{
		// 加锁;
		{
			/*
			 * 仅仅对状态量的判断不需要加锁，modify by w00210470
			 */
			//CAutoLock lock(m_RunFlagMutex);
			if (!m_bRun)
			{
				IVS_LOG(IVS_LOG_DEBUG, "Main loop", "m_bRun is false, thread will be exit.");
				break;
			}
		}

		// 穿越;
		(void)CheckNatSend();

		// 收流;
		ProcessRecevData();
	}
}

// 停止工作循环接口定义，将工作停下来;
void CNetSource::kill()
{
	//停止线程;
	//CAutoLock lock(m_RunFlagMutex);
	m_bRun = false;
}

int CNetSource::Init(CChannelInfo* pChannelInfo)
{
	(void)IVS_NEW(m_pRecvBuf, RTP_MEDIA_BUF_LEN);
	if (NULL != m_pRecvBuf)
	{
		memset(m_pRecvBuf, 0, RTP_MEDIA_BUF_LEN);
	}
	else
	{
		IVS_LOG(IVS_LOG_ERR, "Init", "Alloc recveive buffer memory failed.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    if (NULL == pChannelInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "Init", "Input param error, pChannelInfo is NULL");
        return IVS_PARA_INVALID;
    }
    m_pChannelInfo = pChannelInfo;
    return IVS_SUCCEED;
}

void CNetSource::Release()
{
    try
    {
		IVS_DELETE(m_pSourceThread);

        m_pChannelInfo = NULL;
        IVS_DELETE(m_pRecvBuf, MUILI);//lint !e1551
        m_pRecvBuf = NULL; 
        m_ulRecvBufLen = 0;
    }
    catch(...)
    {
    }
}

void CNetSource::ProcMediaPacket()
{
    if (NULL == m_pRecvBuf)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcMediaPacket", "Source process packet error, receive buffer is null.");  
        return;
    }
    
    NET_DATA_CALLBACK cbDataCallBack = NULL;  //网络数据回调函数;
    void* pUserData = NULL;       //用户数据;
    m_pChannelInfo->GetDataCallBack(cbDataCallBack, pUserData);
    if (NULL == cbDataCallBack)
    {
        IVS_LOG(IVS_LOG_ERR, "ProcMediaPacket", "data call back pointer is null.");  
        return;
    }

    cbDataCallBack(m_pRecvBuf, m_ulRecvBufLen, pUserData);
    return;		
}

//处理收流异常上报;
void CNetSource::DoExceptionCallBack(int iMsgType, void*  pParam)
{
    if (NULL == m_pChannelInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "DoExceptionCallBack", "Channel info is NULL.");  
        return;
    }
    NET_EXCEPTION_CALLBACK cbExceptionCallBack = NULL;
    m_pChannelInfo->GetExceptionCallBack(cbExceptionCallBack);

    if (NULL != cbExceptionCallBack)
    { 
        cbExceptionCallBack(0, iMsgType, pParam, NULL); // 子模块不需要关注通道号;
    }
}

int CNetSource::CheckTimeOut()
{
    if (NULL == m_pChannelInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "CheckTimeOut", "Channel info is NULL.");  
        return IVS_FAIL;
    }

	unsigned long ulInterval = GetTickCount() - m_ulLastCheckTimeoutTick;
	IVS_LOG(IVS_LOG_DEBUG, "CheckTimeOut", "ulInterval[%lu].", ulInterval); 

    //判断超时是否大于30秒;
    if (ulInterval >= RECV_TOTAL_TIMEOUT_TIME)
    {
        //媒体流超时异常上报;
        unsigned int ulChannel = m_pChannelInfo->GetChannelNo();
        unsigned int* pChannelNo = &ulChannel;
        DoExceptionCallBack(IVS_PLAYER_RET_RECV_DATA_TIMEOUT, (void*)pChannelNo);

        m_ulLastCheckTimeoutTick = GetTickCount();
    }

    return IVS_SUCCEED;
}

// 检查是否满足NAT穿越条件,每隔30s向服务器发送nat消息;
int CNetSource::CheckNatSend()
{
	if (m_ulLastSendNatTick == 0)
	{
		m_ulLastSendNatTick = GetTickCount();
		return IVS_SUCCEED;
	}

	unsigned long ulInterval = GetTickCount() - m_ulLastSendNatTick;
	//判断超时是否大于20秒;
	if (ulInterval < SEND_NAT_INTERVAL)
	{
		return IVS_FAIL;
	}

	m_ulLastSendNatTick = GetTickCount();
	return IVS_SUCCEED;
}

// 启动收流;
int CNetSource::Start()
{
	IVS_LOG(IVS_LOG_DEBUG, "Start", "Start thread.");  
	int iRet = IVS_SUCCEED;
	// 创建、启动收流线程;
	try
	{
		CAutoLock lock(m_RunFlagMutex);
		m_bRun = true;
		m_pSourceThread = new _BaseThread(this, 0);
	}
	catch(...)
	{
		m_pSourceThread = NULL;
		iRet = IVS_ALLOC_MEMORY_ERROR;
		IVS_LOG(iRet, "Start", "Create net source thread error");
	}

    // 启动收流时，更新收流超时、NAT穿越的初始计数时间;
    m_ulLastCheckTimeoutTick = GetTickCount();
    //m_ulLastSendNatTick = GetTickCount();
    return iRet;
}

// 停止收流;
void CNetSource::Stop()
{
	IVS_LOG(IVS_LOG_DEBUG, "Stop", "stop thread.");  
	
	try
	{
		IVS_DELETE(m_pSourceThread);
	}
	catch (...)
	{
		IVS_LOG(IVS_LOG_ERR, "Stop", "delete source thread throw exception.");  
	}
	
    return;
}
