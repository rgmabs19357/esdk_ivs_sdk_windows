/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspSessionManager.cpp
  作    者 : y00182336
  功能描述 : RTSP客户端通讯库
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include "Rtsp_Log.h"
#include "RtspRetCode.h"
#include "RtspSessionManager.h"
#include "RtspReactorManager.h"
#include "rtsp_client_datatype.h"
using namespace std;

/***************************************************************************
 函 数 名 : CRtspSessionManager
 功能描述 : 构造函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSessionManager::CRtspSessionManager()
{
    m_fExceptionCallBack = NULL;
    m_pUser              = NULL;
    m_unSessionIndex     = 0;
    m_lTimerId           = -1;
}


/***************************************************************************
 函 数 名 : ~CRtspSessionManager
 功能描述 : 析构函数
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
CRtspSessionManager::~CRtspSessionManager()
{
    m_fExceptionCallBack = NULL;
    m_pUser              = NULL;
}


// 定时器回调函数，用于定期发送检查连接状态
int CRtspSessionManager::handle_timeout(const ACE_Time_Value &tv, const void *arg)
{
    ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
    if (m_RtspSessionList.empty())
    {
		//IVS_RUN_LOG_CRI("handle_timeout failed. list is empty.");
        return 0;
    }

    // 检查连接状态消息
    RtspSessionIter iter = m_RtspSessionList.begin();
    CRtspSession *pSession = NULL;
    while (iter != m_RtspSessionList.end())
    {
        pSession = *iter;
        RtspSessionIter pIter = iter++;
        if (NULL == pSession)
        {
            // 从List中移除session并释放
            m_RtspSessionList.erase(pIter);
            continue;
        }

        if (RTSP_SESSION_STATUS_ABNORMAL== pSession->getStatus())
        {
			// 清除异常连接
            (void)pSession->disconnectPeer();
            pSession->setStatus(RTSP_SESSION_STATUS_DELETE);
            continue;
        }

        if (RTSP_SESSION_STATUS_DELETE == pSession->getStatus())
        {
			IVS_RUN_LOG_INF("start release rtsp session [%p]", pSession);
            // 从List中移除session并释放
            m_RtspSessionList.erase(pIter);
            pSession->setDelFlag();
        }
    }
    return 0;
}

// 初始化连接管理器
int CRtspSessionManager::init()
{
    ACE_Time_Value tv(1, 0); //lint !e747
    if (-1 != m_lTimerId)
    {
        CRtspReactorManager::instance()->unRegistTimer(m_lTimerId, 0);
        m_lTimerId = -1;
    }
    m_lTimerId = CRtspReactorManager::instance()->RegistTimer((ACE_Event_Handler *)this, 0, tv, tv);
    if (-1 == m_lTimerId)
    {
        IVS_RUN_LOG_CRI("start check timer fail.");
        return RET_FAIL;
    }
    return RET_OK;
}

/***************************************************************************
 函 数 名 : closeManager
 功能描述 : 关闭连接管理器
 输入参数 : 无
 输出参数 : 无
 返 回 值 : 无
***************************************************************************/
void CRtspSessionManager::closeManager()
{
    ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
    // 关闭所有连接，所有连接出队列
    if (m_RtspSessionList.empty())
    {
        IVS_RUN_LOG_INF("Session list already empty.");
        return;
    }
    RtspSessionIter iter = m_RtspSessionList.begin();
    CRtspSession *pSession = NULL;
    // modified by y00182336 关闭通讯库时，中断
    while(iter != m_RtspSessionList.end())
    {
        pSession = *iter;
        RtspSessionIter pIter = iter++;
        if (NULL == pSession)
        {
            // 从List中移除session并释放
            m_RtspSessionList.erase(pIter);
            continue;
        }
        else
        {
            // 清除异常连接
            (void)pSession->disconnectPeer();
            IVS_RUN_LOG_INF("start release rtsp session [%p]", pSession);
            // 从List中移除session并释放
            m_RtspSessionList.erase(pIter);
            //ACE_Guard <ACE_Thread_Mutex> lockerSession (pSession->m_Mutex);
            pSession->setDelFlag();
        }
    }
    // end modified by y00182336

    if (-1 != m_lTimerId)
    {
        CRtspReactorManager::instance()->unRegistTimer(m_lTimerId, 0);
        m_lTimerId = -1;
    }

    return;
}



/***************************************************************************
 函 数 名 : createRtspSession
 功能描述 : 创建RTSP连接
 输入参数 : bBlocked                同步/异步标志位（true：同步；false：异步）
 输出参数 : 无
 返 回 值 : CRtspSession*           NULL-失败
***************************************************************************/
CRtspSession* CRtspSessionManager::createRtspSession(const bool bBlocked)
{
    CRtspSession* RtspSession = NULL;
    try
    {
        RtspSession = new CRtspSession;
    }
    catch(...)
    {
		IVS_RUN_LOG_CRI("new session throw exception.");
        return NULL;
    }

	int nRet = RtspSession->InitData();
	if (RET_OK != nRet)
	{
		delete RtspSession;
		RtspSession = NULL;
		IVS_RUN_LOG_CRI("init rtsp session data fail.");
		return NULL;
	}

	IVS_RUN_LOG_CRI("createRtspSession this = %08x m_pRecvBuffer = %08x", RtspSession, RtspSession->m_pRecvBuffer);

    // 设置同步标志位
    RtspSession->m_bBlocked = bBlocked;
    RtspSession->m_unSessionIndex = getSessionIndex();

    // 连接入队列
    nRet = pushRtspSession(RtspSession);
    if (RET_OK != nRet)
    {
        delete RtspSession;
        RtspSession = NULL;
        IVS_RUN_LOG_CRI("push rtsp session fail.");
    }
    return RtspSession;
}


/***************************************************************************
 函 数 名 : findSession
 功能描述 : 查找连接
 输入参数 : pSession                被查找的连接
 输出参数 : 无
 返 回 值 : true-成功               false-失败
***************************************************************************/
bool CRtspSessionManager::findSession(const CRtspSession *pSession)
{
    if (NULL == pSession)
    {
        return false;
    }
    
    // 在所有创建的连接队列中查找
    ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
    return isSessionInList(pSession, m_RtspSessionList);
}

bool CRtspSessionManager::deleteSession(const CRtspSession *pSession)
{
	if (NULL == pSession)
	{
		return false;
	}

	bool bRet = false;
	// 在所有创建的连接队列中查找
	ACE_Guard<ACE_Thread_Mutex> locker(m_RtspSessionListMutex);
	RtspSessionIter iter;
	for (iter = m_RtspSessionList.begin(); iter != m_RtspSessionList.end(); ++iter)
	{
		if (pSession == *iter)
		{
			m_RtspSessionList.erase(iter);
			bRet = true;
			break;
		}
	}
	return bRet;
}

//*************************内部实现**************************//

/***************************************************************************
 函 数 名 : pushRtspSession
 功能描述 : RtspSession放入队列中
 输入参数 : pSession                被操作的连接
 输出参数 : 无
 返 回 值 : RET_OK-成功             NULL-失败
***************************************************************************/
int CRtspSessionManager::pushRtspSession(CRtspSession* pSession)
{
    try
    {
        ACE_Guard<ACE_Thread_Mutex>  locker(m_RtspSessionListMutex);
        m_RtspSessionList.push_back(pSession);
    }
    catch (...)
    {
        return RET_FAIL;
    }
    return RET_OK;
}



/***************************************************************************
 函 数 名 : isSessionInList
 功能描述 : 检查连接是否存在
 输入参数 : pSession                被操作的连接
            SessionList             被操作的队列
 输出参数 : 无
 返 回 值 : true-存在               false-不存在
***************************************************************************/
bool CRtspSessionManager::isSessionInList(const CRtspSession *pSession, 
                                          RtspSessionList &SessionList) const
{
    RtspSessionIter iter;
    for (iter = SessionList.begin(); iter != SessionList.end(); ++iter)
    {
        if (pSession == *iter)
        {
            return true;
        }
    }
    return false;
}


unsigned int CRtspSessionManager::getSessionIndex()
{
    ACE_Guard<ACE_Thread_Mutex> locker (m_unSessionIndexMutex);
    return m_unSessionIndex++;
}

