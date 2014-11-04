/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspReactorManager.h
  作    者 : y00182336
  功能描述 : RTSP Client反应器管理
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/


#ifndef __RTSPREACTORMANAGER_H_
#define __RTSPREACTORMANAGER_H_

#include "ace_header.h"
#include "RtspSessionManager.h"

#include <list>
using namespace std;


/// 定义线程数量
#define RTSP_THREAD_NUM                     (5)

/// 定义EPOLL中可管理的FD大小
#ifndef WIN32
#define RTSP_MAX_EPOLL_SIZE                 (10000)
#else
#define RTSP_MAX_EPOLL_SIZE                 (1024)
#endif

/// 定义每个线程最大处理时间，500ms
#define RTSP_MAX_TIME_PER_THREAD            (500 * 1000)

/// 定义线程堆栈大小
#define DEFAULT_THREAD_STACK_SIZE           (128 * 1024)


class CRtspReactorManager : public ACE_Task<ACE_MT_SYNCH>
{
public:
    virtual ~CRtspReactorManager();

    static CRtspReactorManager* instance()
    {
        static CRtspReactorManager ReactorManager;
        return &ReactorManager;
    }

    // 打开管理器
    int openManager();

    // 关闭管理器
    int closeManager();

    // 线程回调
    int svc();
    
    //注册接收事件
    int RegistHandle(CRtspSession *pHandle, unsigned long mask);
    
    //取消接收事件
    int unRegistHandle(const ACE_Event_Handler *pHandle, unsigned long mask) const;

    //注册定时器
    long RegistTimer(ACE_Event_Handler *pHandle, unsigned int unIndex,
                     const ACE_Time_Value &delta,
                     const ACE_Time_Value &interval) const;

    //取消定时器
    int unRegistTimer(long timer_id, unsigned int unIndex) const;

private:
    CRtspReactorManager();
    
    unsigned int getThreadIndex();

    // 创建资源
    int createResources();

    // 创建线程
    int createServiceThreads();

private:
    bool                        m_bRunFlag;                     /// 线程运行标志

    unsigned int                m_ThreadNum;                    /// 线程号
    ACE_Thread_Mutex            m_ThreadNumMutex;              /// 线程号锁

    ACE_Reactor**               m_pReactor;                     /// 反应器
    ACE_Thread_Mutex            m_FreeSessionListMutex;         // 线程锁
    RtspSessionList             m_FreeSessionList;              // 所有RTSP连接
};

#endif /*__RTSPREACTORMANAGER_H_*/

