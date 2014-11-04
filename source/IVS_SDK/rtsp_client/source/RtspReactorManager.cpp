//lint -e429
/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspReactorManager.cpp
  作    者 : y00182336
  功能描述 : RTSP Client反应器管理
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include "RtspReactorManager.h"
#include "rtsp_client_datatype.h"



CRtspReactorManager::CRtspReactorManager()
{
    m_bRunFlag      = false;
    m_pReactor      = NULL;

    m_ThreadNum     = 0;
}

CRtspReactorManager::~CRtspReactorManager()
{
    m_bRunFlag    = false;
    if (NULL != m_pReactor)
    {
        try
        {
            for (unsigned int i = 0; i < RTSP_THREAD_NUM; i++)
            {
                if (NULL != m_pReactor[i])
                {
                    delete m_pReactor[i];
                    m_pReactor[i]  = NULL;
                }
            }
            delete[] m_pReactor;
            m_pReactor = NULL;
        }
        catch(...)
        {

        }

    }
    m_ThreadNum     = 0;
}


/***************************************************************************
 函 数 名 : openManager
 功能描述 : 打开反应器管理线程
 输入参数 : ThreadNum            反应器线程个数
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::openManager( )
{
    int nRet = createResources();
    if (RET_OK != nRet)
    {
        return RET_FAIL;
    }

    // 标志位置位
    m_bRunFlag = true;
    nRet = createServiceThreads();
    if (RET_OK != nRet)
    {
        return RET_FAIL;
    }
    IVS_RUN_LOG_INF("open reactor manager success.");
    return RET_OK;
}


/***************************************************************************
 函 数 名 : closeManager
 功能描述 : 关闭反应器管理线程
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::closeManager()
{
    IVS_RUN_LOG_INF("start close reactor manager.");
    m_bRunFlag  = false;
    m_ThreadNum = 0;
    (void)wait();
    this->thr_mgr_ = 0;

    if (NULL != m_pReactor)
    {
        try
        {
            for (unsigned int i = 0; i < RTSP_THREAD_NUM; i++)
            {
                if (NULL != m_pReactor[i])
                {
                    delete m_pReactor[i];
                    m_pReactor[i]  = NULL;
                }
            }
            delete[] m_pReactor;
            m_pReactor = NULL;
        }
        catch(...)            
        {

        }
    }
    return RET_OK;
}


/***************************************************************************
 函 数 名 : svc
 功能描述 : 反应器线程函数
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::svc()
{
    IVS_RUN_LOG_INF("reactor manager thread start.");
    unsigned int unThreadIndex = getThreadIndex();
    if ((NULL == m_pReactor) || (NULL == m_pReactor[unThreadIndex]))
    {
        IVS_RUN_LOG_CRI("reactor manager thread exit, resource not created.");
        return RET_FAIL;
    }

    (void)m_pReactor[unThreadIndex]->owner(ACE_OS::thr_self());

    (void)m_pReactor[unThreadIndex]->restart(false);
    
    ACE_Time_Value delaytime;
    while (m_bRunFlag)
    {
        {
            ACE_Guard<ACE_Thread_Mutex> locker(m_FreeSessionListMutex);
            RtspSessionIter itor;
            RtspSessionIter itorend = m_FreeSessionList.end();
            for (itor = m_FreeSessionList.begin(); itor != itorend;)
            {
                CRtspSession *pSession = *itor;
                if (NULL == pSession)
                {
                    itor = m_FreeSessionList.erase(itor);
                    continue;
                }

				//IVS_RUN_LOG_INF("CRtspReactorManager::svc Session[%lu] falg[%d].", pSession, pSession->getDelFalg());
                if (pSession->getDelFalg())
                {
					//IVS_RUN_LOG_INF("CRtspReactorManager::svc Session[%lu] is delete falg.", pSession);
                    itor = m_FreeSessionList.erase(itor);
                    
					// 去除注册
					//unRegistHandle(pSession, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);

					IVS_RUN_LOG_INF("CRtspReactorManager::svc delete Session[%p].", pSession);
					delete pSession;
					pSession = NULL;
                    continue;
                }
                ++itor;
            }

        }
        delaytime.set((time_t)0, RTSP_MAX_TIME_PER_THREAD);
        if (m_pReactor[unThreadIndex]->handle_events(&delaytime) < 0)
        {
            continue;
        }
    }

    IVS_RUN_LOG_INF("reactor manager thread exit.");
    return RET_OK;
}


/***************************************************************************
 函 数 名 : RegistHandle
 功能描述 : 注册接收事件
 输入参数 : pHandle                事件回调类
            mask                事件触发类型
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::RegistHandle(CRtspSession *pHandle, unsigned long mask)
{
	IVS_RUN_LOG_CRI("in RegistHandle.");

    if ((NULL == pHandle) || (NULL == m_pReactor))
    {
		IVS_RUN_LOG_CRI("CRtspReactorManager::RegistHandle pHandle = NULL or NULL == m_pReactor.");
        return RET_FAIL;
    }
    unsigned int unThreadIndex = pHandle->m_unSessionIndex % RTSP_THREAD_NUM;
    if (NULL == m_pReactor[unThreadIndex])
    {
		IVS_RUN_LOG_CRI("CRtspReactorManager::RegistHandle m_pReactor[%u] = NULL.", unThreadIndex);
        return RET_FAIL;
    }

    int nRet = m_pReactor[unThreadIndex]->register_handler(pHandle->get_handle(),
                                                         (ACE_Event_Handler *)pHandle,
                                                         mask);
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("register handler fail.");
        return RET_FAIL;
    }

    ACE_Guard<ACE_Thread_Mutex> locker(m_FreeSessionListMutex);
    m_FreeSessionList.push_back(pHandle);
	IVS_RUN_LOG_INF("CRtspReactorManager::svc push Session[%lu] to list.", pHandle);

    return RET_OK;
}


/***************************************************************************
 函 数 名 : unRegistHandle
 功能描述 : 取消接收事件
 输入参数 : pHandle                事件回调类
            mask                事件触发类型
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::unRegistHandle(const ACE_Event_Handler *pHandle, unsigned long mask) const
{
	IVS_RUN_LOG_INF("CRtspReactorManager::unRegistHandle Session[%lu] in.", pHandle);
    if ((NULL == pHandle) || (NULL == m_pReactor))
    {
        return RET_FAIL;
    }

    // 取得当前负载连接个数最轻的反应器
    ACE_Reactor *pReactor = pHandle->reactor();
    if (NULL == pReactor)
    {
        return RET_FAIL;
    }

    return pReactor->remove_handler(pHandle->get_handle(), mask);
}


/***************************************************************************
 函 数 名 : RegistTimer
 功能描述 : 注册定时器
 输入参数 : pHandle              事件应答句柄
           delta                超时时间
           interval             超时时间
 输出参数 : 无       
 返 回 值 : timer_id - 成功       -1 - 失败
***************************************************************************/
long CRtspReactorManager::RegistTimer(ACE_Event_Handler *pHandle, unsigned int unIndex,
                                      const ACE_Time_Value &delta,
                                      const ACE_Time_Value &interval) const
{
    unsigned int unThreadIndex = unIndex % RTSP_THREAD_NUM;
    if ((NULL == pHandle) || (NULL == m_pReactor) || (NULL == m_pReactor[unThreadIndex]))
    {
        return RET_FAIL;
    }

    return m_pReactor[unThreadIndex]->schedule_timer(pHandle, 0, delta, interval);
}


/***************************************************************************
 函 数 名 : unRegistTimer
 功能描述 : 取消接收定时器
 输入参数 : timer_id            定时器Id
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::unRegistTimer(long timer_id, unsigned int unIndex) const
{
    unsigned int unThreadIndex = unIndex % RTSP_THREAD_NUM;
    if ((-1 == timer_id) || (NULL == m_pReactor) || (NULL == m_pReactor[unThreadIndex]))
    {
        return RET_FAIL;
    }

    int nRet = m_pReactor[unThreadIndex]->cancel_timer(timer_id);
    if (0 == nRet)
    {
        IVS_RUN_LOG_CRI("cancel timer fail, timerId[%ld].", timer_id);
        return RET_OK;
    }
    return RET_FAIL;
}


// 获取线程号
unsigned int CRtspReactorManager::getThreadIndex()
{
    ACE_Guard <ACE_Thread_Mutex> locker(m_ThreadNumMutex);
    return m_ThreadNum++;
}


//*********************************内部实现********************************//

/***************************************************************************
 函 数 名 : createResources
 功能描述 : 创建反应器资源
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::createResources()
{
    try
    {
        m_pReactor = new ACE_Reactor*[RTSP_THREAD_NUM];
    }
    catch(...)
    {
        return RET_FAIL;
    }
    for(unsigned int i = 0; i < RTSP_THREAD_NUM; i++)
    {
        ACE_Reactor_Impl *pImpl = NULL;

        try
        {
#ifdef WIN32
            pImpl = new ACE_Select_Reactor(RTSP_MAX_EPOLL_SIZE, true);
#else
            pImpl = new ACE_Dev_Poll_Reactor(RTSP_MAX_EPOLL_SIZE, true);
#endif
	    }
	    catch(...)
	    {
	        return RET_FAIL;
	    }

        try
        {
            m_pReactor[i] = new ACE_Reactor(pImpl, true);
        }
        catch(...)
        {
            delete m_pReactor[i];
            IVS_RUN_LOG_CRI("Create debug reactor fail.");
            return RET_FAIL;
        }
    }
    return RET_OK; //lint !e429
}//lint !e429

/***************************************************************************
 函 数 名 : createResources
 功能描述 : 创建反应器线程
 输入参数 : 无            
 输出参数 : 无       
 返 回 值 : RET_OK - 成功       RET_FAIL - 失败
***************************************************************************/
int CRtspReactorManager::createServiceThreads()
{
    size_t* pStackSize = NULL;
    pStackSize = new (size_t[RTSP_THREAD_NUM]);//lint !e838
    for(unsigned int i = 0; i < RTSP_THREAD_NUM; i++)
    {
        pStackSize[i] = DEFAULT_THREAD_STACK_SIZE;
    }
    m_ThreadNum = 0;
    int nRet = activate(THR_NEW_LWP  | THR_JOINABLE, //lint !e835
                        (int)RTSP_THREAD_NUM,
                        0,
                        ACE_DEFAULT_THREAD_PRIORITY,
                        -1,
                        0,
                        0,
                        0,
                        pStackSize,
                        0 );

    delete[] pStackSize;
    if (-1 == nRet)
    {
        IVS_RUN_LOG_CRI("create media connect manager threads fail, retcode[%d].", errno);
        return RET_FAIL;
    }
    IVS_RUN_LOG_INF("create reactor manager thred success.");
    return RET_OK;
}

//lint +e429

