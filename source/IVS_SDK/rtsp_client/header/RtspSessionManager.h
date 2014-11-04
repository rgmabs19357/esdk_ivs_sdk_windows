/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspSessionManager.h
  作    者 : y00182336
  功能描述 : RTSP客户端通讯库
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#ifndef __RTSPSESSIONMANAGER_H_
#define __RTSPSESSIONMANAGER_H_

#include <list>
#include "ace_header.h"
#include "RtspSession.h"
#include "rtsp_client_datatype.h"
using namespace std;

// 定义RTSP连接队列
typedef std::list<CRtspSession*>        RtspSessionList;
typedef std::list<CRtspSession*>::iterator       RtspSessionIter;

class CRtspSessionManager : public ACE_Event_Handler
{
public:
    virtual ~CRtspSessionManager();
    
    static CRtspSessionManager* instance()
    {
        static CRtspSessionManager SessionManager;
        return &SessionManager;
    };
    
    //初始化，注册反应器
    int init();

    // 关闭连接管理器
    void closeManager();

    // 创建连接
    CRtspSession* createRtspSession(const bool bBlocked);

    // 释放连接
    //int releaseRtspSession(CRtspSession* pSession);

    // 定时器回调函数，用于定期发送Options消息
    int handle_timeout(const ACE_Time_Value &tv, const void *arg);
    
    // 检查连接是否存在
    bool findSession(const CRtspSession *pSession);

	bool deleteSession(const CRtspSession *pSession);
private:
    CRtspSessionManager();
    
    unsigned int getSessionIndex();

    // 检查队列中是否存在连接
    bool isSessionInList(const CRtspSession *pSession, 
                            RtspSessionList &SessionList) const;

    // 检查连接是否超时
    void checkSession();

    // 连接入队列
    int pushRtspSession(CRtspSession* pSession);


public:

    fExceptionCallBack          m_fExceptionCallBack;           // 异常回调函数
    void*                       m_pUser;                        // 用户参数

private:
    ACE_Thread_Mutex            m_unSessionIndexMutex;          // 连接号线程锁
    unsigned int                m_unSessionIndex;               // 连接号

    ACE_Thread_Mutex            m_RtspSessionListMutex;         // 线程锁
    RtspSessionList             m_RtspSessionList;              // 所有RTSP连接

    long                        m_lTimerId;                     // 定时器ID

};

#endif //__RTSPSESSIONMANAGER_H_
