#include "EventCallBackJob.h"

#include "IVSCommon.h"
#include "ivs_error.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "LockGuard.h"

CEventCallBackJob::CEventCallBackJob(void)
    : m_bRun(true)
    , m_pEventMgr(NULL)
{
    m_pMutexEventQueue = VOS_CreateMutex();
}

CEventCallBackJob::~CEventCallBackJob(void)
{
    m_bRun = false;
    m_pEventMgr = NULL;
    
    try
    {
        if (NULL != m_pMutexEventQueue)
        {
            VOS_DestroyMutex(m_pMutexEventQueue);
            m_pMutexEventQueue = NULL;
        }
    }
    catch(...)
    {}//lint !e1775
}

void CEventCallBackJob::MainLoop()
{
    while (m_bRun)
    {
        EVENT_CALL_BACK_DATA* pstEvent = PopEvent();
        // 队列中无数据sleep100毫秒;
        if (NULL == pstEvent)
        {
            Sleep(100);
        }
        else
        {
            if (NULL == m_pEventMgr)
            {
                // 此处不能打日志;
                return;
            }

            (void)m_pEventMgr->OnEventCallBack(pstEvent->iSessionID, pstEvent->iEventType, pstEvent->pEventData, pstEvent->iBufSize);
            delete [] pstEvent->pEventData;
            pstEvent->pEventData = NULL;
            IVS_DELETE(pstEvent);
        }
    }
}

void CEventCallBackJob::kill()
{
    //停止线程;
    m_bRun = false;
}

int CEventCallBackJob::PostEvent(int iSessionID, int iEventType, const void* pEventData, int iBufSize)
{
    CLockGuard lock(m_pMutexEventQueue);
    //构造事件对象
    EVENT_CALL_BACK_DATA* pstEventData = IVS_NEW(pstEventData);
    if (NULL == pstEventData)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "event data null", "NA");
        return IVS_FAIL;
    }

    memset(pstEventData, 0, sizeof(EVENT_CALL_BACK_DATA));
    pstEventData->iSessionID = iSessionID;
    pstEventData->iEventType = iEventType;

	IVS_CHAR* pEvent = IVS_NEW((IVS_CHAR* &)pEvent, (unsigned long)iBufSize);
	if (!CToolsHelp::Memcpy(pEvent, (unsigned int)iBufSize, pEventData, (unsigned int)iBufSize))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memcpy fail", "NA");
		IVS_DELETE(pstEventData);
		IVS_DELETE(pEvent, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
    pstEventData->pEventData = (void*)pEvent;
    pstEventData->iBufSize = iBufSize;

    //插入队列前先检查下当前的长度,如果已达最大值，那删除第一个;
    if (m_eventQueue.size() >= MAX_EVENT_QUEUE_LEN)
    {
        EVENT_CALL_BACK_DATA* pstEventDataFront = m_eventQueue.front();
        IVS_DELETE(pstEventDataFront->pEventData, MUILI);
        IVS_DELETE(pstEventDataFront);
        m_eventQueue.pop();
        BP_DBG_LOG("event queue over max size[%u]", m_eventQueue.size());
    }

    //插入队列
    m_eventQueue.push(pstEventData);
    return IVS_SUCCEED;
}

EVENT_CALL_BACK_DATA* CEventCallBackJob::PopEvent()
{
    CLockGuard lock(m_pMutexEventQueue);
    if (0 == m_eventQueue.size())
    {
        return NULL;
    }

    EVENT_CALL_BACK_DATA* pstEventData = m_eventQueue.front();
    //delete stEventData1.pEventData;
    m_eventQueue.pop();
    return pstEventData;
}
