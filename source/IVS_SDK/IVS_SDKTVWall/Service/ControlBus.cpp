#include "StdAfx.h"
#include "ControlBus.h"
#include "StreamMgr.h"
#include "bp_def.h"
#include "ToolsHelp.h"

//Default constructor
CControlBus::CControlBus()
{
    m_pStreamMgr = NULL; //StreamMgr对象指针

    m_bisFirstLogin = FALSE;    //是否初次登陆
    m_enLoginStatus = CUMW::LOGIN_STATUS_INIT;  //登陆状态为初始化

    m_ulBusinessId = 0;        //业务ID
	for(int i=0;i<THREAD_MAX_COUNT;i++)
	{
		m_ThreadPara[i].i = 0;
		m_ThreadPara[i].pThread = NULL;
	}
    m_pThreadMutex = VOS_CreateMutex();
    m_pBusinessIdMutex = VOS_CreateMutex();
}

//Destructor
CControlBus::~CControlBus()
{
    m_pStreamMgr = NULL; //StreamMgr对象指针

    (void)VOS_DestroyMutex(m_pThreadMutex);
	m_pThreadMutex = NULL;

    (void)VOS_DestroyMutex(m_pBusinessIdMutex);
    m_pBusinessIdMutex = NULL;
}

/**************************************************************************
* name       : InitControlBus
* description: 初始化ControlBus
* input      : pParaInfo   初始化参数
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::InitControlBus(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{
    //输入参数检查：
    //  1、pParaInfo指针非空
    //  2、ulStructLen必须等于IPI_INIT_CUMW结构体长度
    //  3、pParaInfo->pFunNotifyCallBack不能为空
	CHECK_POINTER(pFunNotifyCallBack, IVS_PARA_INVALID);
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);

	IPI_INIT_CUMW ParaInfo;
    IPI_INIT_CUMW* pParaInfo = &ParaInfo;
    pParaInfo->pFunNotifyCallBack = pFunNotifyCallBack;
    pParaInfo->pCallBackUserData = pUserData;

    //加上接口锁
    CUMW::CLock objLock(m_objInterfaceLock);

    //已经初始化
    if (m_enLoginStatus > CUMW::LOGIN_STATUS_INIT)
    {
        IVS_RUN_LOG_ERR( "CUMW Init Failed. Has Inited already.");
        return IVS_TVWALL_INIT_ALREADY;
    }

    //设置通知回调函数状态值
    m_objStatusNotifyCallBack.SetStatusValue(pParaInfo->pFunNotifyCallBack, pParaInfo->pCallBackUserData);

    //初始化ControlBus资源
    long lResult = InitControlBusResource();
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Init ControlBus Resource Failed On CUMW Init.");

        //释放ControlBus的资源
        ReleaseControlBusResource();

        return IVS_FAIL;
    }

    //保存初始化成功状态
    m_enLoginStatus = CUMW::LOGIN_STATUS_INITED;

    IVS_DBG_LOG("CUMW Init Success.");
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : InitControlBusResource
* description: 初始化ControlBus的资源
* input      : NA
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::InitControlBusResource()
{
    long lResult = IVS_SUCCEED;    //调用其它函数返回值

    IVS_DBG_LOG("Init ControlBus Resource Begin.");

    srand(GetTickCount()); //设置种子

    //初始化全局定时器
    lResult = InitNvsTimer();
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Init Global Timer Failed On Init ControlBus Resource.");
        return IVS_FAIL;
    }

    //初始化全局通信管理模块
    lResult = InitConnMgr();
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Init Global Connection Manager Failed On Init ControlBus Resource.");
        return IVS_FAIL;
    }

    //创建StreamMgr
    if (NULL == VOS_NEW((CStreamMgr * &)m_pStreamMgr))
    {
        IVS_RUN_LOG_ERR( "Create Stream Manager Failed On Init ControlBus Resource.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    //初始化StreamMgr
    lResult = m_pStreamMgr->InitStreamMgr(HandleNotifyCallBack, this);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Init Stream Manager Failed On Init ControlBus Resource.");
        return IVS_FAIL;
    }

    //启动事件处理线程
    for (int i = 0; i < THREAD_MAX_COUNT; i++)
    {
        m_ThreadPara[i].i = i;
        m_ThreadPara[i].pThread = this;
        lResult = m_objProcEventThread[i].StartThread(ProcEventThreadEntry, (void*)&m_ThreadPara[i], CUMW::EVENT_SYNC_QUEUE_NUM);
        if (IVS_SUCCEED != lResult)
        {
            IVS_RUN_LOG_ERR( "Start Process Event Thread Failed On Init ControlBus Resource. ThreadID = %#p. i = %d",
                    &m_objProcEventThread[i], i);
            return IVS_TVWALL_CREATE_THREAD_FAIL;
        }
    }

    //启动远程解码器管理
    lResult = m_pStreamMgr->StartRemoteDecoderMgr();
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Start Remote Decoder Mgr Failed On Init ControlBus Resource.");
        return IVS_TVWALL_CREATE_THREAD_FAIL;
    }

    IVS_DBG_LOG("Init ControlBus Resource Success.");
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : InitNvsTimer
* description: 初始化定时器
* input      : NA
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::InitNvsTimer() const
{
    IVS_DBG_LOG("Init Global Timer Begin.");

    //返回值
    IVS_INT32 lResult = IVS_SUCCEED;

    //获得定时器实例
    CNVSTimer& objNVSTimer = CStatusSingleton::Instance().GetNVSTimer();

    //初始化定时器
    lResult = objNVSTimer.init(CUMW::TIMER_INTERVAL);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Init Global Timer Failed.");
        return IVS_FAIL;
    }

    //启动定时器
    lResult = objNVSTimer.run();
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Start Global Timer Failed.");
        return IVS_FAIL;
    }

    IVS_DBG_LOG("Init Global Timer Success.");
    return lResult;
}

/**************************************************************************
* name       : InitConnMgr
* description: 初始化通信管理模块
* input      : NA
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::InitConnMgr() const
{
    IVS_DBG_LOG("Init Connection Manager Begin.");

    //返回值
    IVS_INT32 lResult = IVS_SUCCEED;

    //获得通信管理器实例
    CConnMgr& objConnMgr = CStatusSingleton::Instance().GetConnMgr();

    //初始化通信平台
    lResult = objConnMgr.init(CUMW::CONN_MGR_SELECT_PERIOD, VOS_FALSE, VOS_FALSE, VOS_TRUE); //(CUMW::CONN_MGR_SELECT_PERIOD, VOS_TRUE, VOS_TRUE, VOS_TRUE);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Init Connection Manager Failed.");
        return IVS_FAIL;
    }

    //运行通信平台
    lResult = objConnMgr.run();
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Start Connection Manager Failed.");
        return IVS_FAIL;
    }

    IVS_DBG_LOG("Init Connection Manager Success.");
    return lResult;
}

/**************************************************************************
* name       : ReleaseControlBus
* description: 释放ControlBus
* input      : NA
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::ReleaseControlBus()
{
    //加上接口锁
    CUMW::CLock objLock(m_objInterfaceLock);

    IVS_DBG_LOG("CUMW Release Begin.");

    //尚未初始化
    if (IVS_SUCCEED != CheckStatus(CUMW::LOGIN_STATUS_INITED))
    {
        IVS_RUN_LOG_ERR( "CUMW Release Failed. Has not Inited yet.");
        return IVS_TVWALL_INIT_NO;
    }

    //保存未初始化状态
    m_enLoginStatus = CUMW::LOGIN_STATUS_INIT;

    //释放ControlBus的资源
    ReleaseControlBusResource();

    IVS_DBG_LOG("CUMW Release Success.");

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : ReleaseControlBusResource
* description: 释放ControlBus的资源
* input      : NA
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
void CControlBus::ReleaseControlBusResource()
{
    IVS_DBG_LOG("Release ControlBus Resource Begin.");

    //退出处理事件线程
    for (int i = 0; i < THREAD_MAX_COUNT; i++)
    {
        m_objProcEventThread[i].StopThread();
    }

	//获得通信管理器实例
	CConnMgr& objConnMgr = CStatusSingleton::Instance().GetConnMgr();

	//停止通信管理器
	objConnMgr.exit();

    //释放StreamMgr
    if (NULL != m_pStreamMgr)
    {
        (void)m_pStreamMgr->StopRemoteDecoderMgr();

        (void)m_pStreamMgr->ReleaseStreamMgr();

        VOS_DELETE(m_pStreamMgr);
    }

    //获得定时器实例
    CNVSTimer& objNVSTimer = CStatusSingleton::Instance().GetNVSTimer();

    //停止定时器
    objNVSTimer.exit();

    //重置通知回调函数状态值
    m_objStatusNotifyCallBack.ResetStatus();

    IVS_DBG_LOG("Release ControlBus Resource Success.");
}

/**************************************************************************
* name       : HandleNotifyCallBack
* description: 处理通知回调
* input      : pstNotifyInfo   通知信息指针
               pUserData       用户数据指针
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 __stdcall CControlBus::HandleNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData)
{
    if (NULL == pstNotifyInfo)
    {
        IVS_RUN_LOG_ERR("ControlBus Handle Notify Call Back Failed. Notify Info is Emtpy.");
        return IVS_PARA_INVALID;
    }

    //参数指针安全转换
    CControlBus* pThis = VOS_CAST((CControlBus*)pUserData);
    if (NULL == pThis)
    {
        IVS_RUN_LOG_ERR( "ControlBus Handle Notify Call Back Failed. "
                "User Data(%#p) Can't Convert to ControlBus Object.", pUserData);
        return IVS_PARA_INVALID;
    }

    IVS_DBG_LOG("ControlBus Handle Notify Call Back Begin. NotifyType = 0x%04X.", pstNotifyInfo->ulNotifyType);
    long lResult = IVS_SUCCEED;

    switch (pstNotifyInfo->ulNotifyType)
    {
    case CUMW_NOTIFY_TYPE_ASYNC_RET:    //异步返回通知
    {
        lResult = pThis->HandleNotifyAsyncRet(*pstNotifyInfo);
        IVS_DBG_LOG("ControlBus Handle Notify Call Back End. Asynchronous Return Notify. Result = 0x%04X.",
                lResult);
        return lResult;    //处理完直接返回
        //break;
    }

    case CUMW_NOTIFY_TYPE_EVENT:
    {
        lResult = pThis->HandleNotifyEvent(*pstNotifyInfo);
        IVS_DBG_LOG("ControlBus Handle Notify Call Back End. Event Notify. Result = 0x%04X.", lResult);
        return lResult;

        //break;
    }

    case CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER:
    {
        CUMW_NOTIFY_REMOTE_DECODER_STATUS* pstInfo
            = (CUMW_NOTIFY_REMOTE_DECODER_STATUS*)pstNotifyInfo->pNotifyInfo; 

        long lIndex = 0;
        if (IVS_FAIL == pThis->GetIdleThread(lIndex))
        {
            IVS_RUN_LOG_ERR("ControlBus Handle Notify Call Back can't find idle thread. szDecoderID = %s.", pstInfo->szDecoderID);
            break;
        }

        memset(pThis->m_objProcEventThread[lIndex].m_szDecoderID, 0, CUMW_DECODER_ID_LEN + 1);
        if(!CToolsHelp::Memcpy(pThis->m_objProcEventThread[lIndex].m_szDecoderID, CUMW_DECODER_ID_LEN, pstInfo->szDecoderID, CUMW_DECODER_ID_LEN))
		{
			IVS_RUN_LOG_ERR("ControlBus Handle Notify Call Back Copy DecoderID failed");
			break;
		}

        IVS_DBG_LOG("ControlBus Handle Notify Call Back szDecoderID = %s. lIndex = %ld.", pstInfo->szDecoderID, lIndex);
        break;
    }

    case CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER:
    case CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT:
        {
            CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO* pstInfo
                = (CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO*)pstNotifyInfo->pNotifyInfo; 

            long lIndex = 0;
            IVS_INT32 lRet = pThis->FindThread(pstInfo->szDecoderID, lIndex);
            if (IVS_FAIL == lRet)
            {
                IVS_RUN_LOG_ERR("ControlBus Handle Notify Call Back can't find idle thread. szDecoderID = %s.", pstInfo->szDecoderID);
                break;
            }

            pThis->m_objProcEventThread[lIndex].m_bIdle =  true;
            memset(pThis->m_objProcEventThread[lIndex].m_szDecoderID, 0, CUMW_DECODER_ID_LEN + 1);

            IVS_DBG_LOG("ControlBus Handle Notify Call Back delete szDecoderID = %s. lIndex = %ld.", pstInfo->szDecoderID, lIndex);
            break;
        }
	case CUMW_NOTIFY_TYPE_START_DECODER:
		{//不需要处理
			break;
		}
	case CUMW_NOTIFY_TYPE_DECODER_ERROR:             //解码器通知错误
		{
			break;
		}
	case CUMW_NOTIFY_TYPE_AUDIO_CONTROL:             //播放/停止播放音频消息通知
		{
			break;
		}
	case CUMW_NOTIFY_TYPE_SET_VOLUME:                //音量调节消息通知
		{
			break;
		}
    default:
    {
		IVS_RUN_LOG_ERR("ControlBus Handle Notify Call Back unknown type = %lu.", pstNotifyInfo->ulNotifyType);
        break;
    }
    }

    //直接透传通知回调函数
    lResult = pThis->m_objStatusNotifyCallBack.NotifyCallBack(*pstNotifyInfo);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "ControlBus Handle Notify Call Back Failed. NotifyType = 0x%04X. UI Return Error(0x%04X).",
                pstNotifyInfo->ulNotifyType, lResult);
        return IVS_FAIL;
    }

    IVS_DBG_LOG("ControlBus Handle Notify Call Back Success. NotifyType = 0x%04X.", pstNotifyInfo->ulNotifyType);
    return IVS_SUCCEED;
}

IVS_INT32 CControlBus::GetIdleThread(long& lIndex)
{
    CLockGuard locker(m_pThreadMutex);

    IVS_DBG_LOG("Enter ControlBus GetIdleThread.");

    for (long i = 0; i < THREAD_MAX_COUNT; i++)
    {
        if (m_objProcEventThread[i].m_bIdle)
        {
            m_objProcEventThread[i].m_bIdle = false;
            lIndex = i;

            IVS_DBG_LOG("Out ControlBus GetIdleThread. lIndex = %ld", lIndex);

            return IVS_SUCCEED;
        }
    }

    IVS_RUN_LOG_ERR( "ControlBus GetIdleThread. can't find idle thread.");
    return IVS_FAIL;
}

IVS_INT32 CControlBus::FindThread(const char* pszDecoderID, long& lIndex)
{
    CLockGuard locker(m_pThreadMutex);

    IVS_DBG_LOG("Enter ControlBus FindThread.");

    for (long i = 0; i < THREAD_MAX_COUNT; i++)
    {
        if (0 == memcmp(m_objProcEventThread[i].m_szDecoderID, pszDecoderID, CUMW_DECODER_ID_LEN))
        {
            lIndex = i;

            IVS_DBG_LOG("Out ControlBus FindThread. szDecoderID = %s, lIndex = %ld", pszDecoderID, lIndex);

            return IVS_SUCCEED;
        }
    }

    IVS_RUN_LOG_ERR( "ControlBus FindThread can't find idle thread. szDecoderID = %s", pszDecoderID);
    return IVS_FAIL;
}

/**************************************************************************
* name       : HandleNotifyAsyncRet
* description: 处理异步返回通知
* input      : stNotifyInfo   通知信息
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleNotifyAsyncRet(CUMW_NOTIFY_INFO& stNotifyInfo)
{
    if (NULL == stNotifyInfo.pNotifyInfo)
    {
        IVS_RUN_LOG_ERR( "Handle Asynchronous Return Notify Failed. Notify Info is Emtpy.");
        return IVS_PARA_INVALID;
    }

    if (sizeof(CUMW_NOTIFY_ASYNC_RET) != stNotifyInfo.ulNotifyInfoLen)
    {
        IVS_RUN_LOG_ERR( "Handle Asynchronous Return Notify Failed. Notify Info Size(%lu) Must be %d.",
                stNotifyInfo.ulNotifyInfoLen, sizeof(CUMW_NOTIFY_ASYNC_RET));
        return IVS_PARA_INVALID;
    }

    //返回值
    long lResult = IVS_SUCCEED;

    CUMW_NOTIFY_ASYNC_RET& stAsyncRet = *(CUMW_NOTIFY_ASYNC_RET*)stNotifyInfo.pNotifyInfo;

    IVS_DBG_LOG(
            "[ AsyncSeq = %lu ] Handle Asynchronous Return Notify Begin. AsyncType = 0x%04X. RetCode = 0x%04X. BusinessID = %lu.",
            stAsyncRet.ulAsyncSeq, stAsyncRet.ulAsyncType, stAsyncRet.ulAsyncRetCode, stAsyncRet.ulBusinessID);

    if ((CUMW::ASYNC_TYPE_MEDIA_CHANNEL_OPEN != stAsyncRet.ulAsyncType)
        && (CUMW::ASYNC_TYPE_MEDIA_CHANNEL_CLOSE != stAsyncRet.ulAsyncType))
    {
        //直接透传通知回调函数
        lResult = m_objStatusNotifyCallBack.NotifyCallBack(stNotifyInfo);
    }

    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR(
                "[ AsyncSeq = %lu ] Handle Asynchronous Return Notify Failed. AsyncType = 0x%04X. Result = 0x%04X.",
                stAsyncRet.ulAsyncSeq, stAsyncRet.ulAsyncType, lResult);
        return lResult;
    }

    IVS_DBG_LOG("[ AsyncSeq = %lu ] Handle Asynchronous Return Notify Success. AsyncType = 0x%04X.",
            stAsyncRet.ulAsyncSeq, stAsyncRet.ulAsyncType);

    return lResult;
}

/**************************************************************************
* name       : ProcEventThreadEntry
* description: 处理事件线程入口
* input      : pPara   线程入口参数
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
unsigned long __stdcall CControlBus::ProcEventThreadEntry(void* pPara)
{
    //参数指针安全转换
    EVENT_THREAD_PARA* pThreadPara = VOS_CAST((EVENT_THREAD_PARA*)pPara);
    CControlBus* pThis = VOS_CAST((CControlBus*)pThreadPara->pThread);

    if (NULL == pThis)
    {
        IVS_RUN_LOG_ERR( "Control Bus Process Event Thread Parameter Invalidate. Parameter Address is %#p.", pPara);
        return IVS_PARA_INVALID;
    }

    //执行处理事件线程
    pThis->ProcEventThread(pThreadPara->i);

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : ProcEventThread
* description: 处理事件线程
* input      : NA
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
void CControlBus::ProcEventThread(int i)
{
    IVS_DBG_LOG("Enter Control Bus Process Event Thread.");

    ////m_mapIdToBusiness.clear();

    //返回值
    long lResult = IVS_SUCCEED;

    //事件信息指针
    CUMW::EVENT_INFO* pstEventInfo = VOS_NULL;

    while (m_objProcEventThread[i].m_bThreadRun)
    {
        //队列为空时会死等，直到获取一个事件。故若需退出该线程，需往队列插入EVENT_TYPE_EXIT_THREAD事件
        lResult = m_objProcEventThread[i].m_objEventSyncQueue.popFrontEv(pstEventInfo);

        IVS_DBG_LOG("ControlBus ProcEventThread. ThreadID is %d", i);

        //取事件失败，或者为空事件，则继续
        if ((VOS_OK != lResult) || (VOS_NULL == pstEventInfo))
        {
            continue;
        }

        //分别处理不同的事件
        switch ((long)pstEventInfo->enEventType)
        {
        case CUMW::EVENT_TYPE_CTRLBUS_BUSINESS: //媒体业务事件
        {
            lResult = HandleEventBusiness(pstEventInfo->pEventInfo, pstEventInfo->ulEventInfoLen);

            //释放事件信息
            VOS_DELETE((BUSINESS_ASYNC_INFO * &)pstEventInfo->pEventInfo);
            break;
        }

        case CUMW::EVENT_TYPE_EXIT_THREAD:    //线程退出
        {
            IVS_DBG_LOG("Receive Thread Exit Event. Control Bus Process Event Thread Will Exit.");
            m_objProcEventThread[i].m_bThreadRun = FALSE;

            //VOS_DELETE((char*&)pstEventInfo->pEventInfo, MULTI); //没有消息体
            break;
        }

        default:
        {
            IVS_RUN_LOG_ERR("Receive Unsupport Event Type(0x%04X) On Control Bus Process Event Thread.");

            //防止出现非法值
            pstEventInfo->enEventType = CUMW::EVENT_TYPE_INVALID;
            break;
        }
        }

        if (IVS_SUCCEED != lResult)
        {
            IVS_RUN_LOG_ERR( "Error(0x%04X) In Control Bus Process Event Thread. %s.",
                    lResult, CUMW::STR_ARR_EVENT_TYPE[pstEventInfo->enEventType]);
        }

        //释放事件
        VOS_DELETE(pstEventInfo);
    }

    //停止所有业务
    ////StopAllBusiness();

    //清空事件同步队列
    while (VOS_OK == m_objProcEventThread[i].m_objEventSyncQueue.popFrontEv(pstEventInfo, 0, QUEUE_MODE_NOWAIT))
    {    
        //begin: modify by liangyun at 2012.2.14.15:00
        if (CUMW::EVENT_TYPE_CTRLBUS_BUSINESS == (long)pstEventInfo->enEventType)
        {
            BUSINESS_ASYNC_INFO* pstAsyncInfo = (BUSINESS_ASYNC_INFO*)pstEventInfo->pEventInfo;
            if ((NULL != pstAsyncInfo) && (NULL != pstAsyncInfo->pParameter) )
            {
                CBusiness* pBusiness = (CBusiness*)pstAsyncInfo->pParameter;
                if (NULL != pBusiness)
                {
                    VOS_DELETE(pBusiness);
                }
            }
            
            if (NULL != pstAsyncInfo)
            {
                VOS_DELETE(pstAsyncInfo);
            }
        }
        //end: modify by liangyun at 2012.2.14.15:00

        //释放事件
        VOS_DELETE(pstEventInfo);
    }

    IVS_DBG_LOG("Exit  Control Bus Process Event Thread.");
}

/**************************************************************************
* name       : AddBuesinessEvent
* description: 添加业务事件
* input      : ulAsynSeq      异步序号
               enAsynType  异步类型
               ulBusinessId    业务ID
               pParameter  参数指针，取决于 异步类型
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::AddBuesinessEvent
(   IN unsigned long       ulAsynSeq,
    IN CUMW::ASYNC_TYPE    enAsynType,
    IN unsigned long       ulBusinessId,
    IN void*               pParameter)
{
	IVS_DBG_LOG("Add Event To Control Bus Synchronous Queue, BusinessID = %lu.", ulBusinessId);

    //构造函数中初始化
    BUSINESS_ASYNC_INFO* pstAsyncInfo = VOS_NEW(pstAsyncInfo);

    if (NULL == pstAsyncInfo)
    {
        IVS_RUN_LOG_ERR( "[ AsyncSeq = %lu ] Create Business Asynchronous Info Failed "
                "On Add Business Event To Control Bus Synchronous Queue.", ulAsynSeq);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    //设置业务异步信息
    pstAsyncInfo->ulAsyncSeq   = ulAsynSeq;
    pstAsyncInfo->enAsyncType  = enAsynType;
    pstAsyncInfo->ulBusinessId = ulBusinessId;
    pstAsyncInfo->pParameter = pParameter;

    CBusiness* pBusiness = (CBusiness*)pParameter;

    long lIndex = 0;
    IVS_INT32 lResult = FindThread(pBusiness->m_szDecoderId, lIndex);
    if (IVS_FAIL == lResult)
    {
        //释放临时内存
        VOS_DELETE(pstAsyncInfo);

        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Add Event To Control Bus Synchronous Queue Failed", ulAsynSeq);
        return lResult;
    }

    //往事件同步队列插入事件
    lResult = m_objProcEventThread[lIndex].AddEvent(CUMW::EVENT_TYPE_CTRLBUS_BUSINESS, pstAsyncInfo, sizeof(*pstAsyncInfo));
    if (IVS_SUCCEED != lResult)
    {
        //释放临时内存
        VOS_DELETE(pstAsyncInfo);

        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Add Event To Control Bus Synchronous Queue Failed", ulAsynSeq);
        return lResult;
    }

	IVS_DBG_LOG("Add Event To Control Bus Synchronous Queue Suc, BusinessID = %lu.", ulBusinessId);

    return lResult;
}

/**************************************************************************
* name       : AddGeneralAsynEvent
* description: 添加通用异步事件
* input      : ulAsynSeq      异步序号
               enAsynType  异步类型
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::AddGeneralAsynEvent
(   IN unsigned long    ulAsynSeq,
    IN CUMW::ASYNC_TYPE enAsynType)
{
    //构造函数中初始化
    /*CUMW::GENERAL_ASYN_INFO* pstAsynInfo = VOS_NEW(pstAsynInfo);

    if (NULL == pstAsynInfo)
    {
        IVS_RUN_LOG_ERR( "[ AsyncSeq = %lu ] Create General Asynchronous Info Failed "
                "On Add General Asynchronous Event To Control Bus Synchronous Queue.", ulAsynSeq);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    //设置异步信息
    pstAsynInfo->ulAsynSeq  = ulAsynSeq;
    pstAsynInfo->enAsynType = enAsynType;

    //往事件同步队列插入事件
    long lResult = m_objProcEventThread[0].AddEvent(CUMW::EVENT_TYPE_GENERAL_ASYN, pstAsynInfo, sizeof(*pstAsynInfo));
    if (IVS_SUCCEED != lResult)
    {
        //释放临时内存
        VOS_DELETE(pstAsynInfo);

        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Add Event To Control Bus Synchronous Queue Failed", ulAsynSeq);
        return lResult;
    }*/

    return IVS_SUCCEED;
}//lint !e1762

/**************************************************************************
* name       : HandleNotifyEvent
* description: 处理事件通知
* input      : stNotifyInfo   通知信息
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleNotifyEvent
(   IN CUMW_NOTIFY_INFO& stNotifyInfo)
{
    if (NULL == stNotifyInfo.pNotifyInfo)
    {
        IVS_RUN_LOG_ERR( "Handle Event Notify Failed. Notify Info is Emtpy.");
        return IVS_FAIL;
    }

    if (sizeof(CUMW_NOTIFY_EVENT) != stNotifyInfo.ulNotifyInfoLen)
    {
        IVS_RUN_LOG_ERR( "Handle Event Notify Failed. Notify Info Size(%lu) Must be %d.",
                stNotifyInfo.ulNotifyInfoLen, sizeof(CUMW_NOTIFY_EVENT));
        return IVS_FAIL;
    }

    long lResult = IVS_SUCCEED;

    CUMW_NOTIFY_EVENT& stNotifyEvent = *(CUMW_NOTIFY_EVENT*)stNotifyInfo.pNotifyInfo;

    IVS_DBG_LOG("Handle Event Notify Begin. EventType = 0x%04X.", stNotifyEvent.ulEventType);

    //默认直接透传通知回调函数
    lResult = m_objStatusNotifyCallBack.NotifyCallBack(stNotifyInfo);

    IVS_DBG_LOG("Handle Event Notify End. EventType = 0x%04X. Result = 0x%04X.",
            stNotifyEvent.ulEventType, lResult);

    return lResult;
}

/**************************************************************************
* name       : CheckStatus
* description: 检测是否达到达到预期状态
* input      : enExpectedStatus 预期状态
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::CheckStatus(CUMW::LOGIN_STATUS enExpectedStatus)
{
    //尚未达到当前状态
    if (m_enLoginStatus < enExpectedStatus)
    {
        IVS_RUN_LOG_ERR( "CUMW Check Status Failed. Current Status: %s. Expectd Status = %d.",
                CUMW::STR_LOGIN_STATUS[m_enLoginStatus], enExpectedStatus);
        return IVS_FAIL;
    }

    //StreamMgr指针不能为空
    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "CUMW Init Status is Abnormal. Stream Manager Address is NULL.");
        return IVS_TVWALL_MEMORY_NULL;
    }

    return IVS_SUCCEED;
} //lint !e1762 以免每个调用此函数的地方都需重复检查空指针

/**************************************************************************
* name       : AddBusinessCtrlEvent
* description: 添加业务控制事件
* input      : ulAsyncSeq                异步序号
               ulBusinessID              业务ID
               pParaInfo                 控制参数
               ulParaLength              控制参数长度
               enCtrlCode                控制码
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::AddBusinessCtrlEvent
(   IN const unsigned long            ulAsyncSeq,
    IN const unsigned long            ulBusinessID,
    IN const void*                    pParaInfo,
    IN const unsigned long            ulParaLength,
    IN const CUMW::BUSINESS_CTRL_CODE enCtrlCode)
{
    IVS_DBG_LOG("[ AysncSeq = %lu ][ BusinessID = %lu ] Add Business Ctrl Event Begin, Ctrl Code: %d.", ulAsyncSeq,
            ulBusinessID, enCtrlCode);

    unsigned long ulCtrlParaLen = sizeof(CUMW::BUSINESS_CTRL_PARA) + ulParaLength - 1;
    CUMW::BUSINESS_CTRL_PARA* pCtrlPara = (CUMW::BUSINESS_CTRL_PARA*)VOS_NEW((char* &)pCtrlPara, ulCtrlParaLen); //lint !e826 可变数组
    if (NULL == pCtrlPara)
    {
        IVS_RUN_LOG_ERR(
                "[ AysncSeq = %lu ][ BusinessID = %lu ] Create Business Control Param Failed On Add Business Ctrl.",
                ulAsyncSeq,
                ulBusinessID);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    memset((char*) pCtrlPara, 0, ulCtrlParaLen);

    //设置控制参数
    pCtrlPara->enCtrlCode = enCtrlCode;
    pCtrlPara->ulCtrlParaLen = ulParaLength;
    if(CToolsHelp::Memcpy(pCtrlPara->vCtrlPara, ulParaLength, pParaInfo, ulParaLength))    //lint !e419 可变数组
	{
		IVS_RUN_LOG_ERR("Add Business Ctrl Event Copy CtrlPara failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    //添加业务控制事件
    long lResult = AddBuesinessEvent(ulAsyncSeq, CUMW::ASYNC_TYPE_BUSINESS_CTRL, ulBusinessID, pCtrlPara);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ][ BusinessID = %lu ] Add Business Ctrl Event Failed On Add Business Ctrl.",
                ulAsyncSeq, ulBusinessID);
        VOS_DELETE(pCtrlPara, MULTI);
        return lResult;
    }

    IVS_DBG_LOG("[ AysncSeq = %lu ][ BusinessID = %lu ] Add Business Ctrl Event Success.", ulAsyncSeq, ulBusinessID);
    return lResult;
}

/**************************************************************************
* name       : StartBusiness
* description: 远程解码器开始解码
* input      : pParaInfo   参数
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::StartBusiness
(   IN CBusiness& objBusiness)
{
    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Start Business In Control Bus Failed. Stream Manager is Empty.");
        return IVS_TVWALL_MEMORY_NULL;
    }

    long lResult = IVS_SUCCEED;

    const unsigned long ulBusinessId = objBusiness.GetBusinessId();

    IVS_DBG_LOG("[ AsyncSeq = %lu ][ BusinessID = %lu ] Start Business In Control Bus Begin.",
            objBusiness.m_ulAsyncSeq, ulBusinessId);

    //初始化业务
    lResult = m_pStreamMgr->InitBusiness(objBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR(
                "[ AsyncSeq = %lu ][ BusinessID = %lu ] Init Business Failed On Start Business In Control Bus.",
                objBusiness.m_ulAsyncSeq, ulBusinessId);
        return lResult;
    }

    //必须先添加到映射表，StartBusiness中的回调会用到
    m_mapIdToBusiness[ulBusinessId] = &objBusiness;

    CBusiness* pSharedBusiness = NULL;
    lResult = GetBusinessById(objBusiness.m_ulSharedBusinessId, pSharedBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR(
                "[ AsyncSeq = %lu ][ BusinessID = %lu ] Get Shared Business Object Failed On Start Business In Control Bus.",
                objBusiness.m_ulAsyncSeq, ulBusinessId);
        return lResult;
    }

    //运行业务
    lResult = RunBusiness(objBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR(
                "[ AsyncSeq = %lu ][ BusinessID = %lu ] Start Business Failed On Start Business In Control Bus.",
                objBusiness.m_ulAsyncSeq, ulBusinessId);
        return lResult;
    }

    IVS_DBG_LOG("[ AsyncSeq = %lu ][ BusinessID = %lu ] Start Business In Control Bus Success.",
            objBusiness.m_ulAsyncSeq, ulBusinessId);
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : RunBusiness
* description: 运行业务
* input      : objBusiness     业务对象引用
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::RunBusiness
(   IN CBusiness& objBusiness)
{
    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Run Business In Control Bus Failed. Stream Manager is Empty.");
        return IVS_TVWALL_MEMORY_NULL;
    }

    long lResult = IVS_SUCCEED;

    const unsigned long ulBusinessId = objBusiness.GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Run Business In Control Bus Begin.", ulBusinessId);

    //开始业务
    lResult = m_pStreamMgr->StartBusiness(objBusiness);

    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Run Business Failed On Handle Start Business Event.", ulBusinessId);
        return lResult;
    }

    IVS_DBG_LOG("[ AsyncSeq = %lu ][ BusinessID = %lu ] Run Business In Control Bus Success.",
            objBusiness.m_ulAsyncSeq, ulBusinessId);
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : StopBusinessWhole
* description: 停止整个业务，包括发送停止信令到平台
* input      : objBusiness     待停止的业务对象引用
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::StopBusinessWhole
(   IN CBusiness& objBusiness)
{
    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Stop Business Whole In Control Bus Failed. Stream Manager is Empty.");
        return IVS_FAIL;
    }

    const unsigned long ulBusinessId = objBusiness.GetBusinessId();

    long lResult = IVS_SUCCEED;

    IVS_DBG_LOG("[ BusinessID = %lu ] Stop Business Whole In Control Bus Begin.", ulBusinessId);

    //删除业务对象本身
    CBusiness* pBusiness = &objBusiness;
    VOS_DELETE(pBusiness);

    IVS_DBG_LOG("[ BusinessID = %lu ] Stop Business Whole In Control Bus End. Result = 0x%04X.", ulBusinessId,
            lResult);
    return lResult;
}//lint !e1762

/**************************************************************************
* name       : HandleEventBusiness
* description: 处理媒体业务事件
* input      : pEventInfo      事件信息指针
               ulEventInfoLen  事件信息内容长度
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventBusiness
(   IN void*           pEventInfo,
    IN unsigned long   ulEventInfoLen)
{
    if (NULL == pEventInfo)
    {
        IVS_RUN_LOG_ERR( "Handle Business Event Failed. Event Info is Empty.");
        return IVS_PARA_INVALID;
    }

    if (sizeof(BUSINESS_ASYNC_INFO) != ulEventInfoLen)
    {
        IVS_RUN_LOG_ERR( "Handle Business Event Failed. Event Info Length(%ld) Must be %d.",
                ulEventInfoLen, sizeof(BUSINESS_ASYNC_INFO));
        return IVS_PARA_INVALID;
    }

    long lResult = IVS_SUCCEED;

    BUSINESS_ASYNC_INFO& stAsyncInfo = *(BUSINESS_ASYNC_INFO*)pEventInfo;

    IVS_DBG_LOG("[ AsyncSeq = %lu ][ BusinessID = %lu ] Handle Business Event Begin.",
            stAsyncInfo.ulAsyncSeq, stAsyncInfo.ulBusinessId);

    switch ((long)stAsyncInfo.enAsyncType)
    {
    case CUMW::ASYNC_TYPE_BUSINESS_START:
    {
        lResult = HandleEventBusinessStart(stAsyncInfo.pParameter, stAsyncInfo.ulAsyncSeq);
        break;
    }

    case CUMW::ASYNC_TYPE_BUSINESS_RUN:
    {
        lResult = HandleEventBusinessRun(stAsyncInfo.ulBusinessId);
        break;
    }

    case CUMW::ASYNC_TYPE_DECORDER_INIT:
    {
        lResult = HandleEventIniBusiness(stAsyncInfo);
        break;
    }

    case CUMW::ASYNC_TYPE_DECORDER_STOP:
    {
        lResult = HandleEventStopBusiness(stAsyncInfo);
        break;
    }
    case CUMW::ASYNC_TYPE_DECORDER_START:
    {
        lResult = HandleEventStartBusiness(stAsyncInfo);
        break;
    }
    case CUMW::ASYNC_TYPE_AUDIO_CONTROL:
    {
        lResult = HandleEventPlayAudio(stAsyncInfo);
        break;
    }
    case CUMW::ASYNC_TYPE_TUNEUP_VOLUME:
    {
        lResult = HandleEventSetVolume(stAsyncInfo);
        break;
    }
    default:
    {
        IVS_RUN_LOG_ERR(
                "[ AsyncSeq = %lu ][ BusinessID = %lu ] Handle Business Event Failed. AsyncType(0x%04X) Unsurpport.",
                stAsyncInfo.ulAsyncSeq, stAsyncInfo.ulBusinessId, stAsyncInfo.enAsyncType);
        return IVS_FAIL;

        //break;
    }
    }

    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AsyncSeq = %lu ][ BusinessID = %lu ] Handle Business Event Failed. ErrorCode = 0x%04X.",
                stAsyncInfo.ulAsyncSeq, stAsyncInfo.ulBusinessId, lResult);
        return lResult;
    }

    IVS_DBG_LOG("[ AsyncSeq = %lu ][ BusinessID = %lu ] Handle Business Event Success.",
            stAsyncInfo.ulAsyncSeq, stAsyncInfo.ulBusinessId);

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : HandleEventBusinessStart
* description: 处理媒体业务开始事件
* input      : pParameter  参数指针
               ulAsyncSeq  异步序号
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventBusinessStart
(   IN void*           pParameter,
    IN unsigned long   ulAsyncSeq)
{
    CBusiness* pBusiness = VOS_CAST((CBusiness*)pParameter);

    if (NULL == pBusiness)
    {
        IVS_RUN_LOG_ERR( "[ AsyncSeq = %lu ] Handle Start Business Event Failed. Business Object is Empty.",
                ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    long lResult = IVS_SUCCEED;

    IVS_DBG_LOG("[ AsyncSeq = %lu ] Handle Start Business Event Begin.", ulAsyncSeq);

    //保存异步序号
    pBusiness->m_ulAsyncSeq = ulAsyncSeq;

    lResult = StartBusiness(*pBusiness);

    //出错时才上报通知，正常时在StartBusiness中会有条件的上报
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AsyncSeq = %lu ] Start Business Failed On Handle Start Business Event. "
                "Will Notify Asynchorous Return, and Stop Business. ErrorCode = 0x%04X.", ulAsyncSeq, lResult);

        //上报异步返回通知
        (void)m_objStatusNotifyCallBack.NotifyBusinessAsynRet(ulAsyncSeq, lResult, pBusiness->GetBusinessId());

        //失败时停止业务，会释放pBusiness对象，后面不能再使用
        (void)StopBusinessWhole(*pBusiness);

        return lResult;
    }

    IVS_DBG_LOG("[ AsyncSeq = %lu ] Handle Start Business Event Success.", ulAsyncSeq);
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : HandleEventBusinessRun
* description: 处理媒体业务运行事件
* input      : ulBusinessId    业务ID
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventBusinessRun
(   IN unsigned long ulBusinessId)
{
    IVS_DBG_LOG("[ BusinessID = %lu ] Handle Run Business Event Begin.", ulBusinessId);

    long lResult = IVS_SUCCEED;

    CBusiness* pBusiness = NULL;
    lResult = GetBusinessById(ulBusinessId, pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Handle Run Business Event Failed. Business Object Not Exist.",
                ulBusinessId);
        return lResult;
    }

    //运行业务
    lResult = RunBusiness(*pBusiness);

    //出错时才上报通知，正常时在RunBusiness中会有条件的上报
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Run Business Failed On Handle Run Business Event. "
                "Will Notify Asynchorous Return, and Stop Business.", ulBusinessId);

        //上报异步返回通知
        (void)m_objStatusNotifyCallBack.NotifyBusinessAsynRet(pBusiness->m_ulAsyncSeq, lResult, ulBusinessId);

        //失败时停止业务
        (void)StopBusinessWhole(*pBusiness);

        //失败后继续
        //return lResult;
    }

    //BEGIN R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化
    //查找是否有共享业务
    MAP_SHARED_BUSINESS_ID::iterator iter = m_mapSharedBusinessId.find(ulBusinessId);
    if (m_mapSharedBusinessId.end() == iter)
    {
        IVS_DBG_LOG("[ BusinessID = %lu ] Handle Run Business Event Success. No Shared Business.", ulBusinessId);
        return IVS_SUCCEED;
    }

    VECTOR_BUSINESS_ID& vectorSharedId = iter->second;

    //遍历启动所有共享业务
    VECTOR_BUSINESS_ID::iterator iterShared = vectorSharedId.begin();
    for (; iterShared != vectorSharedId.end(); ++iterShared)
    {
        unsigned long& ulSharedBusinessId = *iterShared;
        CBusiness* pSharedBusiness = NULL;
        lResult = GetBusinessById(ulSharedBusinessId, pSharedBusiness);
        if (IVS_SUCCEED != lResult)
        {
            IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Handle Run Business Event Failed. Business Object Not Exist.",
                    ulSharedBusinessId);

            //失败后继续
            continue;
        }

        //复制服务器端的媒体信息
        pSharedBusiness->CopyServerMediaInfo(*pBusiness);

        //运行业务
        lResult = RunBusiness(*pSharedBusiness);

        //出错时才上报通知，正常时在RunBusiness中会有条件的上报
        if (IVS_SUCCEED != lResult)
        {
            IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Run Business Failed On Handle Run Business Event. "
                    "Will Notify Asynchorous Return, and Stop Business.", ulSharedBusinessId);

            //上报异步返回通知
            (void)m_objStatusNotifyCallBack.NotifyBusinessAsynRet(pSharedBusiness->m_ulAsyncSeq, lResult,
                                                                  ulSharedBusinessId);

            //失败时停止业务
            (void)StopBusinessWhole(*pSharedBusiness);

            //失败后继续
            //return lResult;
        }
        else
        {
            IVS_DBG_LOG(
                    "[ AsyncSeq = %lu ][ BusinessID = %lu ] Start Business In Control Bus In. Will Notify Stream Status : Arrive.",
                    pSharedBusiness->m_ulAsyncSeq, ulSharedBusinessId);
        }
    }

    //END   R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化

    IVS_DBG_LOG("[ BusinessID = %lu ] Handle Run Business Event Success.", ulBusinessId);
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : GetBusinessByID4Invite
* description: 获取媒体业务re-invite相关业务
* input      : ulBusinessID   业务ID
* output     : pBusiness      异步信息
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::GetBusinessById4Invite
(   IN unsigned long ulBusinessID,
    IO CBusiness*&   pBusiness)
{
    IVS_DBG_LOG(
            "[ BusinessID = %lu ] Get Business Object By BusinessID Begin On Handle Get Business By ID for Invite.",
            ulBusinessID);

    long lResult = GetBusinessById(ulBusinessID, pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        MAP_ID_TO_BUSINESS::iterator iter = m_mapIdToBusiness.begin();
        CBusiness* pTmpBusiness = NULL;
        for (; iter != m_mapIdToBusiness.end(); ++iter)
        {
            pTmpBusiness = iter->second;//lint !e64 
            if ((NULL != pTmpBusiness) && (pTmpBusiness->m_ulSharedBusinessId == ulBusinessID))
            {
                pBusiness = pTmpBusiness;
                IVS_DBG_LOG(
                        "[ BusinessID = %lu ] Get Business Object By Shared BusinessID Success On Handle Get Business By ID for Invite, BusinessID = %lu.",
                        ulBusinessID, pTmpBusiness->GetBusinessId());
                break;
            }
        }
    }

    if (NULL == pBusiness)
    {
        IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Get Business Object Failed On Handle Get Business By ID for Invite.",
                ulBusinessID);
        return IVS_FAIL;
    }

    IVS_DBG_LOG("[ BusinessID = %lu ] Get Business Object Success On Handle Get Business By ID for Invite.",
            ulBusinessID);
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : GetNewBusinessId
* description: 获取新的业务ID
* input      : objBusiness     待停止的业务对象引用
* output     : NA
* return     : 业务ID
* remark     : NA
**************************************************************************/
unsigned long CControlBus::GetNewBusinessId()
{
    (void)VOS_MutexLock(m_pBusinessIdMutex);

    if (0xFFFFFFFF == m_ulBusinessId)
    {
        m_ulBusinessId = 0;
    }

    unsigned long ulBusinessId = ++m_ulBusinessId;

    if (CUMW::INVALID_BUSINESS_ID == ulBusinessId)
    {
        ulBusinessId = ++m_ulBusinessId;
    }

    (void)VOS_MutexUnlock(m_pBusinessIdMutex);

    return ulBusinessId;
}

/**************************************************************************
* name       : HandleEventBusinessStop
* description: 根据业务ID获取业务对象
* input      : ulBusinessId    业务ID
* output     : pBusiness       返回的业务对象指针
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::GetBusinessById
(   IN unsigned long    ulBusinessId,
    OUT CBusiness*&     pBusiness)
{
    MAP_ID_TO_BUSINESS::iterator iter = m_mapIdToBusiness.find(ulBusinessId);

    if (m_mapIdToBusiness.end() == iter)
    {
        IVS_RUN_LOG_ERR("Business ID(%lu) isn't Exist On Get Business.", ulBusinessId);
        return IVS_TVWALL_BUSINESS_ID_NO_EXIST;
    }

    //获得业务对象指针
    pBusiness = iter->second;//lint !e64 

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : StopAllBusiness
* description: 停止所有业务，只能在处理事件线程中处理
* input      : NA
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
void CControlBus::StopAllBusiness()
{
    IVS_DBG_LOG("Stop All Business Begin.");

    //遍历停止所有业务
    MAP_ID_TO_BUSINESS::iterator iter = m_mapIdToBusiness.begin();
    while (m_mapIdToBusiness.end() != iter)
    {
        CBusiness* pBusiness = iter->second;//lint !e64 

        //必须先递增迭代器，停止业务时会删除当前迭代
        ++iter;

        if (NULL == pBusiness)
        {
            continue;
        }

        //停止业务
        (void)StopBusinessWhole(*pBusiness);
    }

    //清空map表
    m_mapIdToBusiness.clear();

    IVS_DBG_LOG("Stop All Business End.");
}

/**************************************************************************
* name       : IniBusiness
* description: 初始化解码器
* input      : pParaInfo   参数
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::IniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo )
{
	IVS_DBG_LOG("Ini Business Begin.");

    //加上接口锁
    //CUMW::CLock objLock(m_objInterfaceLock);

    //输入参数检查
    //1、pParaInfo指针非空
    if (NULL == pParaInfo)
    {
        IVS_RUN_LOG_ERR( "Parameter Invalidate. Parameter is Empty.");
        return IVS_PARA_INVALID;
    }

    //2、m_szDecoderID不能为空字符串
    if (0 == pParaInfo->szDecoderID[0])
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Parameter Invalidate. Decoder ID is Empty.", pParaInfo->ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    //3、协议字段必须为有效值
    if ((RDA_PROTOCOL_NSS_OVER_UDP > pParaInfo->ucTransportProtocol)
        || (RDA_PROTOCOL_RTP_OVER_TCP < pParaInfo->ucTransportProtocol))
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Parameter Invalidate. TransportProtocol is Invalidate.", pParaInfo->ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    //创建业务对象，构造函数里面初始化
    CBusiness* pBusiness = VOS_NEW(pBusiness);
    if (NULL == pBusiness)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Create Business Failed.", pParaInfo->ulAsyncSeq);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    pBusiness->m_ulAsyncSeq = pParaInfo->ulAsyncSeq;
    pBusiness->m_ulChannelId = pParaInfo->usChannelNo;
    (void)pBusiness->SetProtocolType(pParaInfo->ucTransportProtocol);
    (void)pBusiness->SetPlayType(CUMW_PLAY_TYPE_REMOTE);
    if(!CToolsHelp::Strncpy(pBusiness->m_szDecoderId, sizeof(pBusiness->m_szDecoderId), pParaInfo->szDecoderID, sizeof(pBusiness->m_szDecoderId) - 1))
	{
		IVS_RUN_LOG_ERR("Copy DecoderId error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //设置BusinessId
    const unsigned long ulBusinessId = GetNewBusinessId();
    pBusiness->SetBusinessId(ulBusinessId);

	IVS_DBG_LOG("Ini Business, BusinessID = %lu.", ulBusinessId);

    //添加开始业务事件
    long lResult = AddBuesinessEvent(pParaInfo->ulAsyncSeq, CUMW::ASYNC_TYPE_DECORDER_INIT, ulBusinessId, pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ][ BusinessID = %lu ] Add Business Event Failed.",
                pParaInfo->ulAsyncSeq, ulBusinessId);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    IVS_DBG_LOG("[ AysncSeq = %lu ][ BusinessID = %lu ] Ini Decoder Success.", pParaInfo->ulAsyncSeq, ulBusinessId);
    return IVS_SUCCEED;
}//lint !e818

/**************************************************************************
* name       : HandleEventIniBusiness
* description: 处理初始化解码器消息
* input      : objBusiness     业务对象引用
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventIniBusiness(BUSINESS_ASYNC_INFO& stAsyncInfo)
{
    long lResult = IVS_SUCCEED;

    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Init Business In Control Bus Failed. Stream Manager is Empty.");

        //上报异步返回通知
        lResult = m_objStatusNotifyCallBack.NotifyAsynRetNoInfo(stAsyncInfo.ulAsyncSeq, IVS_FAIL);

        //释放内存
        VOS_DELETE((CBusiness*&)stAsyncInfo.pParameter); //modify by liangyun at 2012.2.14.19:28

        return IVS_TVWALL_MEMORY_NULL;
    }

    CBusiness* pBusiness = (CBusiness*)stAsyncInfo.pParameter;
    const unsigned long ulBusinessId = pBusiness->GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Init Business In Control Bus Begin.", ulBusinessId);

    //初始化业务
    lResult = m_pStreamMgr->InitBusiness(*pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR(
                "[ AsyncSeq = %lu ][ BusinessID = %lu ] Init Business Failed In Control Bus.",
                pBusiness->m_ulAsyncSeq, ulBusinessId);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    //释放内存
    VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

    return lResult;
}

/**************************************************************************
* name       : StartBusiness
* description: 远程解码器开始解码
* input      : pParaInfo   参数
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo)
{
    //加上接口锁
    //CUMW::CLock objLock(m_objInterfaceLock);

    //输入参数检查
    //1、pParaInfo指针非空
    if (NULL == pParaInfo)
    {
        IVS_RUN_LOG_ERR( "Start Decoder Failed. Parameter Invalidate. Parameter is Empty.");
        return IVS_PARA_INVALID;
    }

    //2、m_szDecoderID不能为空字符串
    if (0 == pParaInfo->szDecoderID[0])
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Start Business Failed. Decoder ID Invalidate.", pParaInfo->ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    //3、协议
    if ((RDA_PROTOCOL_NSS_OVER_UDP > pParaInfo->stMediaNetInfo.ucTransportProtocol)
        || (RDA_PROTOCOL_RTP_OVER_TCP < pParaInfo->stMediaNetInfo.ucTransportProtocol))
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Start Business Failed. Parameter Invalidate. Protocol is error.", pParaInfo->ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    //创建业务对象，构造函数里面初始化
    CBusiness* pBusiness = VOS_NEW(pBusiness);
    if (NULL == pBusiness)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Create Business Failed On Start Business.", pParaInfo->ulAsyncSeq);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    pBusiness->m_ulAsyncSeq = pParaInfo->ulAsyncSeq;
    pBusiness->m_ulChannelId = pParaInfo->usChannelNo;
    (void)pBusiness->SetPlayType(CUMW_PLAY_TYPE_REMOTE_DIRECT_CONNECT);
    if(!CToolsHelp::Strncpy(pBusiness->m_szDecoderId, sizeof(pBusiness->m_szDecoderId), pParaInfo->szDecoderID, sizeof(pParaInfo->szDecoderID) - 1))
	{
		IVS_RUN_LOG_ERR("Copy DecoderId error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}

    if (RDA_PROTOCOL_RTP_OVER_TCP == pParaInfo->stMediaNetInfo.ucTransportProtocol)
    {
        (void)pBusiness->SetProtocolType(RDA_PROTOCOL_RTP_OVER_TCP);
        pBusiness->m_stServerPortInfo.usInterleavedPort = pParaInfo->stMediaNetInfo.usTcpInterleavedPort;
    }
    else
    {
        (void)pBusiness->SetProtocolType(RDA_PROTOCOL_RTP_OVER_UDP);
        pBusiness->m_stServerPortInfo.usVideoRtpPort  = pParaInfo->stMediaNetInfo.usUdpVideoRtpPort;
        pBusiness->m_stServerPortInfo.usVideoRtcpPort = pParaInfo->stMediaNetInfo.usUdpVideoRtcpPort;
        pBusiness->m_stServerPortInfo.usAudioRtpPort  = pParaInfo->stMediaNetInfo.usUdpAudioRtpPort;
        pBusiness->m_stServerPortInfo.usAudioRtcpPort = pParaInfo->stMediaNetInfo.usUdpAudioRtcpPort;
    }

    pBusiness->m_enVideoPt = (CUMW::PAY_LOAD_TYPE)pParaInfo->ucVideoPayloadType;
    pBusiness->m_enAudioPt = (CUMW::PAY_LOAD_TYPE)pParaInfo->ucAudioPayloadType;
    if(!CToolsHelp::Strncpy(pBusiness->m_szServerIp, sizeof(pBusiness->m_szServerIp), pParaInfo->stMediaNetInfo.szMediaIP, sizeof(pBusiness->m_szServerIp) - 1))
	{
		IVS_RUN_LOG_ERR("Copy ServerIp error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}
    if(!CToolsHelp::Strncpy(pBusiness->m_szSessionId, sizeof(pBusiness->m_szSessionId), pParaInfo->szSessionId, sizeof(pBusiness->m_szSessionId) - 1))
	{
		IVS_RUN_LOG_ERR("Copy SessionId error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}
    if(!CToolsHelp::Strncpy(pBusiness->m_szSessionUrl, sizeof(pBusiness->m_szSessionUrl), pParaInfo->szSessionUrl, sizeof(pBusiness->m_szSessionUrl) - 1))
	{
		IVS_RUN_LOG_ERR("Copy SessionUrl error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //设置BusinessId
    const unsigned long ulBusinessId = pParaInfo->ulBusinessID;
    pBusiness->SetBusinessId(ulBusinessId);

    //添加开始业务事件
    long lResult = AddBuesinessEvent(pParaInfo->ulAsyncSeq, CUMW::ASYNC_TYPE_DECORDER_START, 0, pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Add Business Event Failed On Start Business.",
                pParaInfo->ulAsyncSeq);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    IVS_DBG_LOG("[ AysncSeq = %lu ] Start Business Success.", pParaInfo->ulAsyncSeq);
    
    return IVS_SUCCEED;
}//lint !e818

/**************************************************************************
* name       : HandleEventStartDecoder
* description: 处理启动解码器消息
* input      : objBusiness     业务对象引用
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventStartBusiness(BUSINESS_ASYNC_INFO& stAsyncInfo)
{
    CBusiness* pCurBusiness = (CBusiness*)stAsyncInfo.pParameter;

    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Start Business In Control Bus Failed. Stream Manager is Empty.");

        //上报异步返回通知
        (void)m_objStatusNotifyCallBack.NotifyAsynRetNoInfo(stAsyncInfo.ulAsyncSeq, IVS_FAIL);

        //释放内存
        VOS_DELETE(pCurBusiness); //modify by liangyun at 2012.2.14.19:28

        return IVS_TVWALL_MEMORY_NULL;
    }

    //开始业务
    long lResult = m_pStreamMgr->StartBusiness(*pCurBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Start Business Failed On Handle Start Business Event.",
                pCurBusiness->GetBusinessId());

        //释放内存
        VOS_DELETE(pCurBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    IVS_DBG_LOG("[ AsyncSeq = %lu ][ BusinessID = %lu ] Start Business In Control Bus Success.",
            pCurBusiness->m_ulAsyncSeq, pCurBusiness->GetBusinessId());

    //释放内存
    VOS_DELETE(pCurBusiness); //modify by liangyun at 2012.2.14.19:28

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : StopBusiness
* description: 停止解码
* input      : objBusiness     业务对象引用
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo)
{
    //加上接口锁
    //CUMW::CLock objLock(m_objInterfaceLock);

    //输入参数检查
    //1、pParaInfo指针非空
    if (NULL == pParaInfo)
    {
        IVS_RUN_LOG_ERR( "Parameter Invalidate. Parameter is Empty.");
        return IVS_PARA_INVALID;
    }

    //2、m_szDecoderID不能为空字符串
    if (0 == pParaInfo->szDecoderID[0])
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Parameter Invalidate. Decoder ID is Empty.", pParaInfo->ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    //创建业务对象，构造函数里面初始化
    CBusiness* pBusiness = VOS_NEW(pBusiness);
    if (NULL == pBusiness)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Create Business Failed.", pParaInfo->ulAsyncSeq);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    pBusiness->m_ulAsyncSeq = pParaInfo->ulAsyncSeq;
    pBusiness->m_ulChannelId = pParaInfo->usChannelNo;
    (void)pBusiness->SetPlayType(CUMW_PLAY_TYPE_REMOTE_DIRECT_CONNECT);
    if(!CToolsHelp::Strncpy(pBusiness->m_szDecoderId, sizeof(pBusiness->m_szDecoderId), pParaInfo->szDecoderID, sizeof(pParaInfo->szDecoderID) - 1))
	{
		IVS_RUN_LOG_ERR("Copy DecoderId error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}
    //设置BusinessId
    const unsigned long ulBusinessId = pParaInfo->ulBusinessID;
    pBusiness->SetBusinessId(ulBusinessId);

    //添加停止业务事件
    long lResult = AddBuesinessEvent(pParaInfo->ulAsyncSeq, CUMW::ASYNC_TYPE_DECORDER_STOP, ulBusinessId, pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ][ BusinessID = %lu ] Add Business Event Failed.",
                pParaInfo->ulAsyncSeq, ulBusinessId);

        //释放内存 
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    IVS_DBG_LOG("[ AysncSeq = %lu ][ BusinessID = %lu ] Stop Business Success.", pParaInfo->ulAsyncSeq, ulBusinessId);
    return IVS_SUCCEED;
}//lint !e818

/**************************************************************************
* name       : HandleEventStopBusiness
* description: 处理停止解码的消息
* input      : objBusiness     业务对象引用
* output     : NA
* return     : long 错误码
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventStopBusiness(BUSINESS_ASYNC_INFO& stAsyncInfo)
{
    long lResult = IVS_SUCCEED;

    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Stop Business In Control Bus Failed. Stream Manager is Empty.");

        //释放内存
        VOS_DELETE((CBusiness*&)stAsyncInfo.pParameter); //modify by liangyun at 2012.2.14.19:28

        return IVS_TVWALL_MEMORY_NULL;
    }

    CBusiness* pBusiness = (CBusiness*)stAsyncInfo.pParameter;
    const unsigned long ulBusinessId = pBusiness->GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Stop Business In Control Bus Begin.", ulBusinessId);

    //初始化业务
    lResult = m_pStreamMgr->StopBusiness(*pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR(
                "[ AsyncSeq = %lu ][ BusinessID = %lu ] Stop Business Failed In Control Bus.",
                pBusiness->m_ulAsyncSeq, ulBusinessId);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    //释放内存
    VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

    return lResult;
}

/**************************************************************************
* name       : PlayAudio
* description: 播放/停止播放声音
* input      : pParaInfo 初始化参数
* output     : void
* return     : RET_OK-成功     RET_FAIL-失败
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::PlayAudio(IPI_AUDIO_CONTROL_REQ *pParaInfo)
{
    /*
    //加上接口锁
    //CUMW::CLock objLock(m_objInterfaceLock);

    //输入参数检查
    //1、pParaInfo指针非空
    if (NULL == pParaInfo)
    {
        IVS_RUN_LOG_ERR( "Parameter Invalidate. Parameter is Empty.");
        return IVS_PARA_INVALID;
    }

    //2、m_szDecoderID不能为空字符串
    if (0 == pParaInfo->szDecoderID[0])
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Parameter Invalidate. Camera ID is Empty.", pParaInfo->ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    //创建业务对象，构造函数里面初始化
    CBusiness* pBusiness = VOS_NEW(pBusiness);
    if (NULL == pBusiness)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Create Business Failed.", pParaInfo->ulAsyncSeq);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    pBusiness->m_ulAsyncSeq = pParaInfo->ulAsyncSeq;
    pBusiness->m_ulChannelId = pParaInfo->usChannelNo;
    (void)pBusiness->SetPlayType(CUMW_PLAY_TYPE_REMOTE_DIRECT_CONNECT);
    if(!CToolsHelp::Strncpy(pBusiness->m_szDecoderId, sizeof(pBusiness->m_szDecoderId), (const char *)pParaInfo->szDecoderID, sizeof(pParaInfo->szDecoderID) - 1))
	{
		IVS_RUN_LOG_ERR("Copy DecoderId error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //设置BusinessId
    const unsigned long ulBusinessId = pParaInfo->ulBusinessID;
    pBusiness->SetBusinessId(ulBusinessId);

    //设置控制信息
    pBusiness->m_cAudioCmd = pParaInfo->cAudioCmd;

    //添加停止业务事件
    long lResult = AddBuesinessEvent(pParaInfo->ulAsyncSeq, CUMW::ASYNC_TYPE_AUDIO_CONTROL, ulBusinessId, pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ][ BusinessID = %lu ] Add Business Event Failed.",
                pParaInfo->ulAsyncSeq, ulBusinessId);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    IVS_DBG_LOG("[ AysncSeq = %lu ][ BusinessID = %lu ] Play Audio Success.", pParaInfo->ulAsyncSeq, ulBusinessId);
    return IVS_SUCCEED;
    */
    return IVS_SYS_NO_SUPPORT;
}//lint !e818 !e1762

/**************************************************************************
* name       : PlayAudio
* description: 处理播放/停止播放声音信息
* input      : pParaInfo 初始化参数
* output     : void
* return     : RET_OK-成功     RET_FAIL-失败
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventPlayAudio(BUSINESS_ASYNC_INFO& stAsyncInfo)//lint !e830
{
    /*
    long lResult = IVS_SUCCEED;

    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Play Audio In Control Bus Failed. Stream Manager is Empty.");

        //释放内存
        VOS_DELETE((CBusiness*&)stAsyncInfo.pParameter); //modify by liangyun at 2012.2.14.19:28

        return IVS_TVWALL_MEMORY_NULL;
    }

    CBusiness* pBusiness = (CBusiness*)stAsyncInfo.pParameter;

    unsigned long ulCtrlParaLen = sizeof(CUMW::BUSINESS_CTRL_PARA) + sizeof(IPI_CTRL_LOCAL_AUDIO) - 1;
    CUMW::BUSINESS_CTRL_PARA* pCtrlPara = (CUMW::BUSINESS_CTRL_PARA*)VOS_NEW((char* &)pCtrlPara, ulCtrlParaLen); //lint !e826 可变数组
    if (NULL == pCtrlPara)
    {
        IVS_RUN_LOG_ERR(
                "[ AysncSeq = %lu ][ BusinessID = %lu ] Create Business Control Param Failed On Add Business Ctrl.",
                stAsyncInfo.ulAsyncSeq, stAsyncInfo.ulBusinessId);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return IVS_ALLOC_MEMORY_ERROR;
    }

    memset((char*) pCtrlPara, 0, ulCtrlParaLen);

    //设置控制参数
    pCtrlPara->enCtrlCode = BUSINESS_CTRL_CODE_E_LOCAL_AUDIO;
    pCtrlPara->ulCtrlParaLen = sizeof(IPI_CTRL_LOCAL_AUDIO);

    //设置控制参数
    IPI_CTRL_LOCAL_AUDIO *pParaInfo = VOS_NEW(pParaInfo);
    if (NULL == pParaInfo)
    {
        IVS_RUN_LOG_ERR( "Set Control Param Failed ");

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return IVS_ALLOC_MEMORY_ERROR;
    }

    pParaInfo->ulAsyncSeq   = stAsyncInfo.ulAsyncSeq;
    pParaInfo->ulAudioType  = CUMW_AUDIO_TYPE_OUT;
    pParaInfo->ulCtrlCode = pBusiness->m_cAudioCmd;
    pParaInfo->ulVolume = 0;

    if(!CToolsHelp::Memcpy(pCtrlPara->vCtrlPara, sizeof(IPI_CTRL_LOCAL_AUDIO), pParaInfo, sizeof(IPI_CTRL_LOCAL_AUDIO)))    //lint !e419 可变数
	{
		IVS_RUN_LOG_ERR("Play Audio In Control Bus Copy CtrlPara failed");
        VOS_DELETE(pBusiness);
    VOS_DELETE((IPI_CTRL_LOCAL_AUDIO*&)pParaInfo);

		return IVS_ALLOC_MEMORY_ERROR;
	}
    const unsigned long ulBusinessId = pBusiness->GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Play Audio In Control Bus Begin.", ulBusinessId);

    //初始化业务
    lResult = m_pStreamMgr->ControlBusiness(*pBusiness, *pCtrlPara);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AsyncSeq = %lu ][ BusinessID = %lu ] Play Audio Failed On Start Business In Control Bus.",
                pBusiness->m_ulAsyncSeq, ulBusinessId);
    }

    //释放内存
    VOS_DELETE((char*&)pCtrlPara);
    VOS_DELETE((IPI_CTRL_LOCAL_AUDIO*&)pParaInfo);
    VOS_DELETE((CBusiness*&)stAsyncInfo.pParameter);

    return lResult;
    */
    return IVS_SYS_NO_SUPPORT;
}//lint !e1762 !e1764

/**************************************************************************
* name       : SetVolume
* description: 设置音量
* input      : pParaInfo 初始化参数
* output     : void
* return     : RET_OK-成功     RET_FAIL-失败
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::SetVolume(IPI_TUNEUP_VOLUME_REQ *pParaInfo)
{
    /*
    //加上接口锁
    //CUMW::CLock objLock(m_objInterfaceLock);

    //输入参数检查
    //1、pParaInfo指针非空
    if (NULL == pParaInfo)
    {
        IVS_RUN_LOG_ERR( "Parameter Invalidate. Parameter is Empty.");
        return IVS_PARA_INVALID;
    }

    //2、m_szDecoderID不能为空字符串
    if (0 == pParaInfo->szDecoderID[0])
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Parameter Invalidate. Camera ID is Empty.", pParaInfo->ulAsyncSeq);
        return IVS_PARA_INVALID;
    }

    //创建业务对象，构造函数里面初始化
    CBusiness* pBusiness = VOS_NEW(pBusiness);
    if (NULL == pBusiness)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ] Create Business Failed.", pParaInfo->ulAsyncSeq);
        return IVS_ALLOC_MEMORY_ERROR;
    }

    pBusiness->m_ulAsyncSeq = pParaInfo->ulAsyncSeq;
    pBusiness->m_ulChannelId = pParaInfo->usChannelNo;
    (void)pBusiness->SetPlayType(CUMW_PLAY_TYPE_REMOTE_DIRECT_CONNECT);
    if(!CToolsHelp::Strncpy(pBusiness->m_szDecoderId, sizeof(pBusiness->m_szDecoderId), (const char *)pParaInfo->szDecoderID, sizeof(pParaInfo->szDecoderID) - 1))
	{
		IVS_RUN_LOG_ERR("Copy DecoderId error.");
        VOS_DELETE(pBusiness);
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //设置BusinessId
    const unsigned long ulBusinessId = pParaInfo->ulBusinessID;
    pBusiness->SetBusinessId(ulBusinessId);

    //设置控制信息，音量调节
    pBusiness->m_cAudioCmd = CUMW_AUDIO_CTRL_CODE_SET_VOLUME;
    pBusiness->m_cVolume = pParaInfo->cVolume;

    //添加停止业务事件
    long lResult = AddBuesinessEvent(pParaInfo->ulAsyncSeq, CUMW::ASYNC_TYPE_TUNEUP_VOLUME, ulBusinessId, pBusiness);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AysncSeq = %lu ][ BusinessID = %lu ] Add Business Event Failed.",
                pParaInfo->ulAsyncSeq, ulBusinessId);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return lResult;
    }

    IVS_DBG_LOG("[ AysncSeq = %lu ][ BusinessID = %lu ] Ini Decoder Success.", pParaInfo->ulAsyncSeq, ulBusinessId);
    return IVS_SUCCEED;
    */
    return IVS_SYS_NO_SUPPORT;
}//lint !e818 !e1762

/**************************************************************************
* name       : SetVolume
* description: 处理设置音量信息
* input      : pParaInfo 初始化参数
* output     : void
* return     : RET_OK-成功     RET_FAIL-失败
* remark     : NA
**************************************************************************/
IVS_INT32 CControlBus::HandleEventSetVolume(BUSINESS_ASYNC_INFO& stAsyncInfo)//lint !e830
{
    /*
    long lResult = IVS_SUCCEED;

    if (NULL == m_pStreamMgr)
    {
        IVS_RUN_LOG_ERR( "Set Volume In Control Bus Failed. Stream Manager is Empty.");

        //释放内存
        VOS_DELETE((CBusiness*&)stAsyncInfo.pParameter); //modify by liangyun at 2012.2.14.19:28

        return IVS_TVWALL_MEMORY_NULL;
    }

    CBusiness* pBusiness = (CBusiness*)stAsyncInfo.pParameter;

    unsigned long ulCtrlParaLen = sizeof(CUMW::BUSINESS_CTRL_PARA) + sizeof(IPI_CTRL_LOCAL_AUDIO) - 1;
    CUMW::BUSINESS_CTRL_PARA* pCtrlPara = (CUMW::BUSINESS_CTRL_PARA*)VOS_NEW((char* &)pCtrlPara, ulCtrlParaLen); //lint !e826 可变数组
    if (NULL == pCtrlPara)
    {
        IVS_RUN_LOG_ERR(
                "[ AysncSeq = %lu ][ BusinessID = %lu ] Create Business Control Param Failed On Add Business Ctrl.",
                stAsyncInfo.ulAsyncSeq, stAsyncInfo.ulBusinessId);

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return IVS_ALLOC_MEMORY_ERROR;
    }

    memset((char*) pCtrlPara, 0, ulCtrlParaLen);

    //设置控制参数
    pCtrlPara->enCtrlCode = BUSINESS_CTRL_CODE_E_LOCAL_AUDIO;
    pCtrlPara->ulCtrlParaLen = sizeof(IPI_CTRL_LOCAL_AUDIO);

    //设置控制参数
    IPI_CTRL_LOCAL_AUDIO *pParaInfo = VOS_NEW(pParaInfo);
    if (NULL == pParaInfo)
    {
        IVS_RUN_LOG_ERR( "Set Control Param Failed ");

        //释放内存
        VOS_DELETE(pBusiness); //modify by liangyun at 2012.2.14.19:28

        return IVS_ALLOC_MEMORY_ERROR;
    }

    pParaInfo->ulAsyncSeq   = stAsyncInfo.ulAsyncSeq;
    pParaInfo->ulAudioType  = CUMW_AUDIO_TYPE_OUT;
    pParaInfo->ulCtrlCode = pBusiness->m_cAudioCmd;
    pParaInfo->ulVolume = pBusiness->m_cVolume;

    if(!CToolsHelp::Memcpy(pCtrlPara->vCtrlPara, sizeof(IPI_CTRL_LOCAL_AUDIO), pParaInfo, sizeof(IPI_CTRL_LOCAL_AUDIO)))    //lint !e419 可变数组
	{
		IVS_RUN_LOG_ERR("Set Volume In Control Bus Copy CtrlPara failed");
        VOS_DELETE(pBusiness);
    VOS_DELETE((IPI_CTRL_LOCAL_AUDIO*&)pParaInfo);

		return IVS_ALLOC_MEMORY_ERROR;
	}
    const unsigned long ulBusinessId = pBusiness->GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Play Audio In Control Bus Begin.", ulBusinessId);

    //初始化业务
    lResult = m_pStreamMgr->ControlBusiness(*pBusiness, *pCtrlPara);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ AsyncSeq = %lu ][ BusinessID = %lu ] Set Volume Failed On Start Business In Control Bus.",
                pBusiness->m_ulAsyncSeq, ulBusinessId);
    }

    //释放内存
    VOS_DELETE((char*&)pCtrlPara);
    VOS_DELETE((IPI_CTRL_LOCAL_AUDIO*&)pParaInfo);
    VOS_DELETE((CBusiness*&)stAsyncInfo.pParameter);

    return lResult;
    */
    return IVS_SYS_NO_SUPPORT;
}//lint !e1762 !e1764
