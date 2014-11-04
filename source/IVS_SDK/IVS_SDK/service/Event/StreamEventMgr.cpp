#include "StreamEventMgr.h"
#include "_BaseThread.h"
#include "IVSCommon.h"
#include "LockGuard.h"
#include "SDKInterface.h"
#include "IVS_Player.h"
#include "SDKConfig.h"
//lint -e526
extern CSDKInterface *g_pNvs;//lint !e526

static void __SDK_CALL PlayerEventCallback(IVS_ULONG  ulChannel,
                                            IVS_INT32  iMsgType,
                                            IVS_VOID*  pParam,
                                            IVS_VOID*  pUser)
{
    BP_DBG_LOG("PlayerEventCallback, Channel:%d, MsgType:%d.",ulChannel, iMsgType);
    int iSessionID = reinterpret_cast <int>(pUser);
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    if (NULL == pUserMgr)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "PlayerEventCallback", "Can not get usermgr by sessionid:%d", iSessionID);
        return;
    }

    unsigned long playHandle = pUserMgr->GetPlayHandlebyPlayChannel(ulChannel);
    if (0 == playHandle)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "PlayerEventCallback", "Get playhandle by rtsphandle failed, rtsp handle:%d", playHandle);
        return;
    }

    switch (iMsgType)
    {
    case IVS_EVENT_PLAYER_BUFFER:
        {
            int iFullFlag = *((int*)pParam);
            (void)CStreamEventMgr::instance().PostEvent(iSessionID, playHandle, iFullFlag);
            BP_DBG_LOG("PlayerEventCallback, IVS_EVENT_PLAYER_BUFFER by sessionid:%d, rtsp handle:%d, iFullFlag:%d", iSessionID, playHandle, iFullFlag);
        }
        break;
    default:
        break;
    }
}//lint !e818

CStreamEventMgr::CStreamEventMgr(void)
    : m_pEventThread(NULL)
    , m_pMutexEventQueue(NULL)
    , m_bRun(false)
{
    m_pMutexEventQueue = VOS_CreateMutex();
}

CStreamEventMgr::~CStreamEventMgr(void)
{
    try
    {
        UnInit();

        if (NULL != m_pMutexEventQueue)
        {
            VOS_DestroyMutex(m_pMutexEventQueue);
            m_pMutexEventQueue = NULL;
            m_pEventThread = NULL;
        }
    }
    catch(...)
    {}//lint !e1775    
}//lint !e732 !e1740

int CStreamEventMgr::Init()
{
    IVS_INT32 iRet = IVS_FAIL;
    
    try
    {
        // 设置媒体缓冲区的上下限百分比
        IVS_UINT32 uiLowerLimit = 0;
        IVS_UINT32 uiUpperLimit = 0;
        CSDKConfig::instance().GetMediaBufferLimit(uiLowerLimit, uiUpperLimit);

		BP_RUN_LOG_INF("CStreamEventMgr::Init", "Set MediaBfferLimit Low[%u] Up[%u]", uiLowerLimit, uiUpperLimit);
        IVS_PLAY_SetMediaBufferLimit(uiLowerLimit, uiUpperLimit);

        //开启事件回调线程;
        m_pEventThread = new _BaseThread((BaseJob* )this, 0);
        m_bRun = true;
        iRet = IVS_SUCCEED;
    }
    catch (...)
    {
        BP_RUN_LOG_ERR(IVS_PLAYER_RET_ALLOC_MEM_ERROR, "new thread", "NA");
        m_pEventThread = NULL;
    }
    return iRet;
}

void CStreamEventMgr::UnInit()
{
    m_bRun = false;
    IVS_DELETE(m_pEventThread);
}

void CStreamEventMgr::MainLoop()
{
    while (m_bRun)
    {
        STREAM_EVENT_DATA* pstEvent = PopEvent();
        // 队列中无数据sleep100毫秒;
        if (NULL == pstEvent)
        {
            Sleep(100);
            continue;
        }
        
        DoEventMsg(pstEvent);
    }
}

void CStreamEventMgr::kill()
{
    //停止线程;
    m_bRun = false;
}

int CStreamEventMgr::PostEvent(int iSessionID, unsigned long ulPlayHandle, int iBufferState)
{
    CLockGuard lock(m_pMutexEventQueue);
    //构造事件对象
    STREAM_EVENT_DATA* pstEventData = IVS_NEW(pstEventData);
    if (NULL == pstEventData)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "event data null", "NA");
        return IVS_FAIL;
    }

    memset(pstEventData, 0, sizeof(STREAM_EVENT_DATA));
    pstEventData->iSessionID = iSessionID;
    pstEventData->ulPlayHandle = ulPlayHandle;
    pstEventData->iBufferState = iBufferState;

    //插入队列前先检查下当前的长度,如果已达最大值，那删除第一个;
    if (m_eventQueue.size() >= MAX_EVENT_QUEUE_LEN)
    {
        STREAM_EVENT_DATA* pstEventDataFront = m_eventQueue.front();
        IVS_DELETE(pstEventDataFront);
        m_eventQueue.pop();
        BP_DBG_LOG("event queue over max size[%u]", m_eventQueue.size());
    }

    //插入队列
    m_eventQueue.push(pstEventData);
    return IVS_SUCCEED;
}

STREAM_EVENT_DATA* CStreamEventMgr::PopEvent()
{
    CLockGuard lock(m_pMutexEventQueue);
    if (0 == m_eventQueue.size())
    {
        return NULL;
    }

    STREAM_EVENT_DATA* pstEventData = m_eventQueue.front();
    m_eventQueue.pop();
    return pstEventData;
}

void CStreamEventMgr::DoEventMsg(STREAM_EVENT_DATA*& pstEvent)
{
    if (NULL == pstEvent)
    {
        return;
    }

    CHECK_POINTER_VOID(g_pNvs);
    int iSessionID = pstEvent->iSessionID;
    CUserMgr* pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    if (NULL == pUserMgr)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "DoEventMsg", "Can not get usermgr by sessionid:%d", iSessionID);
        return;
    }

    (void)pUserMgr->NotifyStreamBufferState(pstEvent->ulPlayHandle, pstEvent->iBufferState);
    IVS_DELETE(pstEvent);
}//lint !e1762 pstEvent在函数内释放，不能声明为const

PLAYER_EVENT_CALLBACK CStreamEventMgr::GetPlayerEventCallBack() const
{
    return PlayerEventCallback;
}
