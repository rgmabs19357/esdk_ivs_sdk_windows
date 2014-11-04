/********************************************************************
 filename    :    StreamEventMgr.h
 author      :    s00191067
 created     :    2013/03/12
 description :    媒体流反馈控制管理类，根据播放库中解码模块缓存情况，控制与服务器间的收发速度;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/03/12 初始版本
*********************************************************************/
#ifndef STREAM_EVENT_MGR
#define STREAM_EVENT_MGR

#include "vos.h"
#include "BaseJob.h"
#include "SDKDef.h"
#include "IVSPlayerDataType.h"

struct STREAM_EVENT_DATA
{
    int iSessionID;             
    unsigned long ulPlayHandle;
    int iBufferState;
};

class _BaseThread;
class CStreamEventMgr : public BaseJob
{
public:
    virtual ~CStreamEventMgr(void);

    static CStreamEventMgr& instance()
    {
        static CStreamEventMgr s_eventStreamMgr;
        return s_eventStreamMgr;
    }

    int Init();

    void UnInit();

    virtual void MainLoop();

    virtual void kill();

    int PostEvent(int iSessionID, unsigned long ulPlayHandle, int iBufferState);

    PLAYER_EVENT_CALLBACK GetPlayerEventCallBack() const;
private:
    CStreamEventMgr(void);
    STREAM_EVENT_DATA* PopEvent();
    void DoEventMsg(STREAM_EVENT_DATA*& pstEvent);

private:
    _BaseThread     *m_pEventThread;    //事件异常回调;
    std::queue<STREAM_EVENT_DATA*>      m_eventQueue;
    VOS_Mutex*                          m_pMutexEventQueue;       // 队列锁;
    bool                                m_bRun;                   // 线程运行标志; 
};
#endif //STREAM_EVENT_MGR
