#include "StdAfx.h"
#include "CumwStatusClasses.h"
#include "RemoteDecoderMgr.h"

CStatusConfig::CStatusConfig() : m_ulLogLM(CUMW::DEFAULT_VALUE_LOG_LM)
								, m_usRemoteDecoderListenPort(CUMW::DEFAULT_VALUE_REMOTE_DECODER_LISTEN_PORT)
{
}

long CStatusConfig::InitConfig(const string& strConfigFileFullName)
{
    //保存配置文件名称
    m_strConfigFileFullName = strConfigFileFullName;

    //重新加载配置项
    return ReloadConfig();
}

long CStatusConfig::ReloadConfig()
{
    long lResult = IVS_SUCCEED;

    //获取配置项的值
    GetConfigValue();

    return lResult;
}

long CStatusConfig::SaveConifg()
{
    //设置所有配置项
    SetConfigValue();
	return IVS_SUCCEED;
}

void CStatusConfig::GetConfigValue()
{
    //日志级别
    SetLogLM(CUMW::DEFAULT_VALUE_LOG_LM);

    //远程解码器监听端口
    SetRemoteDecoderListenPort(CUMW::DEFAULT_VALUE_REMOTE_DECODER_LISTEN_PORT);
}

void CStatusConfig::SetConfigValue()
{

}

unsigned long CStatusConfig::GetLogLM() const
{
    return m_ulLogLM;
}

void CStatusConfig::SetLogLM(const unsigned long ulLogLM)
{
    //值合法性检查
    if (IVSLOG_DEBUG >= ulLogLM)
    {
        //保存值
        m_ulLogLM = ulLogLM;
    }
}

unsigned short CStatusConfig::GetRemoteDecoderListenPort() const
{
    return m_usRemoteDecoderListenPort;
}

void CStatusConfig::SetRemoteDecoderListenPort
(   IN const unsigned short usListenPort)
{
    if (CUMW::PORT_MIN < usListenPort)
    {
        m_usRemoteDecoderListenPort = usListenPort;
    }
	else
	{
		IVS_RUN_LOG_ERR( "Listen Port Value should be larger than %ld.", CUMW::PORT_MIN);
	}
}

CStatusSingleton::CStatusSingleton()
{
    memset(m_szLocalIp, 0, sizeof(m_szLocalIp));
};

const char* CStatusSingleton::GetLocalIp()
{
    //已经获取过，直接返回
    if (0 != m_szLocalIp[0])
    {
        return m_szLocalIp;
    }

    //获取本机IP
    char szHostName[CUMW_PATH_LEN] = {0};

    //获取主机名
    int nResult = gethostname(szHostName, sizeof(szHostName));
    if (SOCKET_ERROR == nResult)
    {
        IVS_RUN_LOG_ERR( "gethostname Failed On Get Local Address. ErrorCode = %ld.", GetLastError());
        return m_szLocalIp;
    }

    //获取本机地址
    sockaddr_in stAddr = {0};
    struct hostent* pstHostent = NULL;
    pstHostent = gethostbyname(szHostName);   //系统函数
    if (pstHostent)
    {
        stAddr.sin_addr.s_addr = *(unsigned long*)(void*)pstHostent->h_addr;
    }
	else
	{
		IVS_RUN_LOG_ERR( "gethostbyname Failed On Get Local Address. ErrorCode = %ld.", GetLastError());
		return m_szLocalIp;
	}

	char* pstrAdd = inet_ntoa(stAddr.sin_addr);
	if (pstrAdd)
	{
		if(!CToolsHelp::Strncpy(m_szLocalIp, sizeof(m_szLocalIp), pstrAdd, strlen(pstrAdd)))
		{
			IVS_RUN_LOG_ERR("GetLocalIp Copy LocalIp failed");
		}
	}
	else
	{
		IVS_RUN_LOG_ERR( "inet_ntoa Failed On Get Local Address. ErrorCode = %ld.", GetLastError());
		return m_szLocalIp;
	}
	
    IVS_DBG_LOG("Local IP: %.15s.", m_szLocalIp);

    return m_szLocalIp;
}

void CStatusSingleton::SetLocalIp(const char* pszIp)
{
    if(!CToolsHelp::Strncpy(m_szLocalIp, sizeof(m_szLocalIp), pszIp, sizeof(m_szLocalIp) - 1))
	{
		IVS_RUN_LOG_ERR("SetLocalIp copy LocalIp failed");
	}
}

CStatusNotifyCallBack::CStatusNotifyCallBack()
{
    //重置状态
    ResetStatus();
}

void CStatusNotifyCallBack::ResetStatus()
{
    m_pFunCallBack = NULL; //回调函数指针
    m_pUserData = NULL; //用户数据指针
}

void CStatusNotifyCallBack::SetStatusValue(PCUMW_CALLBACK_NOTIFY pFunCallBack, void* pUserData)
{
    if (NULL != pFunCallBack) //为null不赋值
    {
        m_pFunCallBack = pFunCallBack; //回调函数指针
    }

    if (NULL != pUserData)
    {
        m_pUserData = pUserData;    //用户数据指针
    }
}

long CStatusNotifyCallBack::NotifyCallBack(CUMW_NOTIFY_INFO& stInfo)
{
    //指针非空检查
    if (NULL == m_pFunCallBack)
    {
        IVS_RUN_LOG_ERR( "NotifyCallBack Point is NULL");
        return IVS_TVWALL_MEMORY_NULL;
    }

    //调用回调函数
    long lResult = m_pFunCallBack(&stInfo, m_pUserData);

    return lResult;
}

long CStatusNotifyCallBack::NotifyAsynRet
(   const CUMW_NOTIFY_ASYNC_RET& stNotifyInfo)
{
    CUMW_NOTIFY_INFO stInfo = {0};

    stInfo.ulNotifyType = CUMW_NOTIFY_TYPE_ASYNC_RET; //返回类型为异步类型
    stInfo.pNotifyInfo = (void*)&stNotifyInfo;
    stInfo.ulNotifyInfoLen = sizeof(stNotifyInfo);

    long lResult = NotifyCallBack(stInfo);

    return lResult;
}

long CStatusNotifyCallBack::NotifyAsynRetNoInfo
(   IN unsigned long ulAsynSeq,
    IN long          lAsynRetCode,
    IN unsigned long ulBusinessId)
{
    CUMW_NOTIFY_ASYNC_RET stAsynRet = {0};

    stAsynRet.ulAsyncSeq = ulAsynSeq;
    stAsynRet.ulAsyncRetCode = (unsigned long)lAsynRetCode;
    stAsynRet.ulAsyncType  = CUMW_ASYNC_TYPE_NO_RET_INFO;
    stAsynRet.ulBusinessID = ulBusinessId;

    long lResult = NotifyAsynRet(stAsynRet);

    return lResult;
}

long CStatusNotifyCallBack::NotifyBusinessAsynRet
(   IN unsigned long ulAsyncSeq,
    IN long          lAsyncRetCode,
    IN unsigned long ulBusinessID)
{
    CUMW_NOTIFY_ASYNC_RET stSyncRet = {0};

    stSyncRet.ulAsyncSeq = ulAsyncSeq;
    stSyncRet.ulAsyncRetCode = (unsigned long)lAsyncRetCode;
    stSyncRet.ulAsyncType  = CUMW_ASYNC_TYPE_START_MEDIA_BUSINESS;
    stSyncRet.ulBusinessID = ulBusinessID;

    long lResult = NotifyAsynRet(stSyncRet);

    return lResult;
}

long CStatusNotifyCallBack::NotifyEvent
(   IN unsigned long ulEventType,
    IN unsigned long ulEventPara)
{
    //生成事件通知结构体
    CUMW_NOTIFY_EVENT stEvent = {0};

    stEvent.ulEventType = ulEventType;
    stEvent.ulEventPara = ulEventPara;

    //生成事件通知信息结构体
    CUMW_NOTIFY_INFO stInfo = {0};
    stInfo.ulNotifyType = CUMW_NOTIFY_TYPE_EVENT;
    stInfo.pNotifyInfo = &stEvent;
    stInfo.ulNotifyInfoLen = sizeof(stEvent);

    long lResult = NotifyCallBack(stInfo);

    return lResult;
}

long CStatusNotifyCallBack::NotifyDecoderDisConnect
(   IN CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO& stNotifyInfo)
{
    CUMW_NOTIFY_INFO stInfo = {0};

    stInfo.ulNotifyType = CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT;
    stInfo.pNotifyInfo = (void*)&stNotifyInfo;
    stInfo.ulNotifyInfoLen = sizeof(stNotifyInfo);

    long lResult = NotifyCallBack(stInfo);

    return lResult;
}

long CStatusNotifyCallBack::NotifyDecoderRegister
(   IN CUMW_NOTIFY_REMOTE_DECODER_STATUS& stNotifyInfo)
{
    CUMW_NOTIFY_INFO stInfo = {0};

    stInfo.ulNotifyType = CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER;
    stInfo.pNotifyInfo = (void*)&stNotifyInfo;
    stInfo.ulNotifyInfoLen = sizeof(stNotifyInfo);

    long lResult = NotifyCallBack(stInfo);

    return lResult;
}

long CStatusNotifyCallBack::NotifyDecoderUnRegister
(   IN CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO& stNotifyInfo)
{
    CUMW_NOTIFY_INFO stInfo = {0};

    stInfo.ulNotifyType = CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER;
    stInfo.pNotifyInfo = (void*)&stNotifyInfo;
    stInfo.ulNotifyInfoLen = sizeof(stNotifyInfo);

    long lResult = NotifyCallBack(stInfo);

    return lResult;
}

CStatusTrigger::CStatusTrigger
(
    IN CUMW::TRIGGER_TYPE enTriggerType
)
{
    m_enTriggerType = enTriggerType;
}

void CStatusTrigger::onTrigger
(   IN void*           pArg,
    IN ULONGLONG       ullScales,
    IN TriggerStyle    enStyle)
{
    switch ((unsigned long)m_enTriggerType)
    {
    case CUMW::TRIGGER_TYPE_DECODER_CONN_TIME_OUT:
    {
        HandleTriggerDecoderConnTimeOut(pArg);
        break;
    }

    default:
    {
		IVS_DBG_LOG("Decoder Conn not TimeOut");
        break;
    }
    }

    //下述参数不需处理
    ullScales = ullScales;
    enStyle = enStyle;
}

void CStatusTrigger::HandleTriggerDecoderConnTimeOut
(   IN void* pArg) const
{
    CDecoderConnHandle* pConnHandle = VOS_CAST((CDecoderConnHandle*)pArg);

    if (NULL == pConnHandle)
    {
        IVS_RUN_LOG_ERR(
                "Parameter(%#p) Can't Convert To Decoder Connection Handle Object On Handlet Decoder Connection Time Out Tringger.",
                pArg);
        return;
    }

    IVS_RUN_LOG_ERR("Handlet Decoder Connection Time Out Tringger. Remote Decoder(IP:%.15s) Connect Time Out.",
            inet_ntoa((in_addr &)pConnHandle->m_peerAddr.m_lIpAddr));

    //处理超时
    pConnHandle->HandleTimeout();
}

CStatusThread::CStatusThread()
{
    m_hThread = NULL;     //处理事件线程句柄
    m_bThreadRun  = FALSE;      //处理事件线程是否运行
    m_ulQueueSize = 0;        //队列大小
    m_bIdle = true;
    memset(m_szDecoderID, 0, CUMW_DECODER_ID_LEN);
}

CStatusThread::~CStatusThread()
{
    m_hThread = NULL;   //处理事件线程句柄
}

long CStatusThread::StartThread
(   IN CUMW::PTHREAD_FUN   pThreadFun,
    IN void*               pThreadEntryPara,
    IN unsigned long       ulQueueSize)
{
    IVS_DBG_LOG("[ ThreadID = %#p ] Start Thread Begin. QueueSize = %u.", this, ulQueueSize);

    m_ulQueueSize = ulQueueSize;

    if (0 < m_ulQueueSize)
    {
        //初始化事件同步队列
        long lResult = m_objEventSyncQueue.init((long)ulQueueSize);
        if (IVS_SUCCEED != lResult)
        {
            IVS_RUN_LOG_ERR( "[ ThreadID = %#p ] Init Event Synchronous Queue Failed On Start Thread.", this);
            return IVS_FAIL;
        }
    }

    //创建线程前，先置为线程运行状态
    m_bThreadRun = TRUE;

    //创建处理事件线程
    m_hThread = CreateThread(NULL, 0, pThreadFun, pThreadEntryPara, 0, NULL);
    if (NULL == m_hThread)
    {
        m_bThreadRun = FALSE;
        IVS_RUN_LOG_ERR( "[ ThreadID = %#p ] Create Thread Failed On Start Thread. ErrorCode = %ld.", this,
                GetLastError());
        return IVS_TVWALL_CREATE_THREAD_FAIL;
    }

    IVS_DBG_LOG("[ ThreadID = %#p ] Start Thread Success. QueueSize = %lu.", this, ulQueueSize);
    return IVS_SUCCEED;
}

void CStatusThread::StopThread()
{
    IVS_DBG_LOG("[ ThreadID = %#p ] Stop Thread Begin.", this);

    //退出处理事件线程
    //m_bThreadRun = FALSE;

    if (NULL != m_hThread)
    {
        //if (0 < m_ulQueueSize)
        {
            //往事件同步队列插入退出线程事件，不需处理错误
            (void)AddEvent(CUMW::EVENT_TYPE_EXIT_THREAD);

            //IVS_RUN_LOG_INF("[ ThreadID = %#p ] Add Stop Thread Info.", this);
        }

        //最多等待30秒，如尚未退出，则程序逻辑有问题
        if (WAIT_OBJECT_0 != WaitForSingleObject(m_hThread, CUMW::WAIT_FOR_THREAD_EXIT_TIME))
        {
            IVS_RUN_LOG_ERR( "[ ThreadID = %#p ] Thread Hasn't Exit. There May be some Problem.", this);
        }

        //释放线程句柄
        (void)CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    IVS_DBG_LOG("[ ThreadID = %#p ] Stop Thread End.", this);
}

long CStatusThread::AddEvent
(   IN CUMW::EVENT_TYPE    enEventType,
    IN void*               pEventInfo,
    IN unsigned long       ulEventInfoLen)
{
    if (0 == m_ulQueueSize)
    {
        IVS_RUN_LOG_ERR( "[ ThreadID = %#p ] Add Event Failed. QueueSize = 0.", this);
        return IVS_FAIL;
    }

    IVS_DBG_LOG("[ ThreadID = %#p ] Add Event Begin. %s.", this, CUMW::STR_ARR_EVENT_TYPE[enEventType]);

    long lResult = IVS_SUCCEED;

    //创建事件
    CUMW::EVENT_INFO* pstEventInfo = VOS_NEW(pstEventInfo);

    //创建失败
    if (NULL == pstEventInfo)
    {
        //退出线程时，必须往事件同步队列插入一个事件，此时使用空事件
        if (CUMW::EVENT_TYPE_EXIT_THREAD == enEventType)
        {
            //往事件同步队列插入一个空事件
            lResult = m_objEventSyncQueue.pushBackEv(NULL);
            if (IVS_SUCCEED != lResult)
            {
                IVS_RUN_LOG_ERR( "[ ThreadID = %#p ] Push Event Failed On Add Event. %s.", this,
                        CUMW::STR_ARR_EVENT_TYPE[enEventType]);
                return IVS_FAIL;
            }

            return IVS_SUCCEED;
        }

        //其他情况，返回失败
        IVS_RUN_LOG_ERR( "[ ThreadID = %#p ] Create Event Failed On Add Event. %s.", this,
                CUMW::STR_ARR_EVENT_TYPE[enEventType]);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    //复制事件信息
    pstEventInfo->enEventType = enEventType;
    pstEventInfo->pEventInfo = pEventInfo;
    pstEventInfo->ulEventInfoLen = ulEventInfoLen;

    //往事件同步队列插入事件
    lResult = m_objEventSyncQueue.pushBackEv(pstEventInfo);
    if (IVS_SUCCEED != lResult)
    {
        //释放临时内存
        VOS_DELETE(pstEventInfo);

        IVS_RUN_LOG_ERR( "[ ThreadID = %#p ] Push Event Failed On Add Event. %s.", this,
                CUMW::STR_ARR_EVENT_TYPE[enEventType]);
        return IVS_FAIL;
    }

    IVS_DBG_LOG("[ ThreadID = %#p ] Add Event Success. %s.", this, CUMW::STR_ARR_EVENT_TYPE[enEventType]);
    return IVS_SUCCEED;
}
