/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RTSP_CLIENT_API.cpp
  作    者 : y00182336
  功能描述 : RTSP客户端通讯库
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/
#include "rtsp_client_api.h"
#include "RtspRetCode.h"
#include "Rtsp_Log_Manager.h"
#include "RtspReactorManager.h"
#include "RtspSessionManager.h"
#include "RtspSetupMessage.h"
#include "RtspPlayMessage.h"
#include "RtspPauseMessage.h"
#include "RtspTeardownMessage.h"
#include "RtspOptionsMessage.h"
//#include "windows.h"

// 定义头次启动标志位
static bool g_bIsFirst = true;         //lint !e956

// 为java提供__stdcall方式下的别名
#ifdef WIN32
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SetLog=_RTSP_CLIENT_SetLog@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_Init=_RTSP_CLIENT_Init@8")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_GetHandle=_RTSP_CLIENT_GetHandle@8")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SetExceptionCallBack=_RTSP_CLIENT_SetExceptionCallBack@8")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SetDataCallBack=_RTSP_CLIENT_SetDataCallBack@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendSetupMsg=_RTSP_CLIENT_SendSetupMsg@28")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendPlayMsg=_RTSP_CLIENT_SendPlayMsg@24")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendPauseMsg=_RTSP_CLIENT_SendPauseMsg@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendTeardownMsg=_RTSP_CLIENT_SendTeardownMsg@12")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_GetStatus=_RTSP_CLIENT_GetStatus@4")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_FreeHandle=_RTSP_CLIENT_FreeHandle@4")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_Cleanup=_RTSP_CLIENT_Cleanup@0")
#pragma comment(linker, "/EXPORT:RTSP_CLIENT_SendOptionMsg=_RTSP_CLIENT_SendOptionMsg@20")
#endif

/***************************************************************************
 函 数 名 : RTSP_CLIENT_SetLog
 功能描述 : 通讯库日志线程初始化，若不调用初始化，日志不打印
 输入参数 : enLevel        日志等级
            bDebugflag     打印DEBUG消息标志
			pszPath       日志文件路径
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SetLog(const unsigned int unLevel, const int iDebugflag, 
                               const char* pszPath)
{ //lint !e1784
    if (unLevel > RTSP_LOG_LEVEL_DEBUG)
    {
        IVS_RUN_LOG_CRI("set rtsp client log fail, Level[%u], debug switch[%d].", 
                        unLevel, iDebugflag); //lint !e730
        return RTSP_ERR_PARAMETER;
    }

    if (NULL != pszPath)
    {
        if (NSS_LOG_FILE_PATH_LEN < strlen(pszPath))
        {
            return RTSP_ERR_PARAMETER;
        }
        strncpy(CRtsp_Log_Manager::instance().m_strLogRootPath, pszPath, strlen(pszPath));
    }

    // 配置日志
    CRtsp_Log_Manager::instance().m_RefreshLevel = (RTSP_LOG_LEVEL_E) unLevel;
    CRtsp_Log_Manager::instance().m_bDebugFlag = iDebugflag ? true : false;
    IVS_DBG_LOG("init log manager success.");
    return RET_OK;
}



/***************************************************************************
 函 数 名 : RTSP_CLIENT_Init
 功能描述 : 通讯库初始化
 输入参数 : 无
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_Init(unsigned int unLevel, const char* pszPath)
{ //lint !e1784
    // 避免重复启动
    if (!g_bIsFirst)
    {
        return RET_OK;
    }
    g_bIsFirst = false;
//#ifdef WIN32
    ACE::init();
//#endif
    // 入参日志路径为空，取默认路径
    std::string strLogPath = RTSP_CLIENT_LOG_PATH;
    if (NULL != pszPath && 0 != strlen(pszPath))
    {
        strLogPath = pszPath;
        for(string::size_type pos(0); pos!=string::npos; pos += strlen("/")) 
        {
            if((pos = strLogPath.find("\\",pos)) != string::npos)    
                strLogPath.replace(pos,strlen("\\"),"/"); 
            else 
                break;    
        }    
    }
    // 打开日志
    int nRet = CRtsp_Log_Manager::instance().initLog(RTSP_CLIENT_SERVICE_NAME, 
                                                     strLogPath.c_str(), unLevel);
    if (RET_OK != nRet)
    {
        return RET_FAIL;
    }

    // 打开反应器管理
    nRet = CRtspReactorManager::instance()->openManager();
    if (RET_OK != nRet)
    {
        (void)CRtsp_Log_Manager::instance().endLog();
        return RET_FAIL;
    }

    // 创建连接管理
    nRet= CRtspSessionManager::instance()->init();
    if (RET_OK != nRet)
    {
        (void)CRtspReactorManager::instance()->closeManager();
        (void)CRtsp_Log_Manager::instance().endLog();
        return RET_FAIL;
    }

    IVS_DBG_LOG("init rtsp client success.");
    return RET_OK;
}



/***************************************************************************
 函 数 名 : RTSP_CLIENT_GetHandle
 功能描述 : 获取连接句柄
 输入参数 : bBlocked        同步/异步方式
 输出参数 : nHandle         创建连接句柄
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_GetHandle(const bool bBlocked, 
                                  int *nHandle)
{ //lint !e1784
    if (NULL == nHandle)
    {
        IVS_RUN_LOG_CRI("handle is NULL.");
        return RET_FAIL;
    }
    CRtspSession* pSession = CRtspSessionManager::instance()->createRtspSession(bBlocked);
    if (NULL == pSession)
    {
        IVS_RUN_LOG_CRI("get session handle fail.");
        return RET_FAIL;
    }
    *nHandle = (int)pSession;
    
    IVS_DBG_LOG("get session handle success, mode[%d], handle[%p].", bBlocked, pSession); //lint !e730
    return RET_OK;
}


/***************************************************************************
 函 数 名 : RTSP_CLIENT_SetExceptionCallBack
 功能描述 : 设置回调函数
 输入参数 : cbExceptionCallBack          回调函数
            pUser                        用户参数，回调函数中返回
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SetExceptionCallBack(fExceptionCallBack cbExceptionCallBack, void* pUser)
{ //lint !e1784
    // 参数检查
    if (NULL == cbExceptionCallBack)
    {
        IVS_RUN_LOG_CRI("set Exception Call Back function fail, parameter abnormal.");
        return RTSP_ERR_PARAMETER;
    }

    CRtspSessionManager::instance()->m_fExceptionCallBack = cbExceptionCallBack;
    CRtspSessionManager::instance()->m_pUser = pUser;
    IVS_DBG_LOG("set Exception Call Back function success.");
    return RET_OK;
}


/***************************************************************************
 函 数 名 : RTSP_CLIENT_SetDataCallBack
 功能描述 : 设置媒体流回调函数
 输入参数 :  nHandle                    连接句柄
            cbDataCallBack              媒体流回调函数
            pUser                       用户参数，回调函数中返回
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SetDataCallBack(int nHandle, fDataCallBack cbDataCallBack, void* pUser)
{ //lint !e1784
    // 入参有效性检查
    if(0 == nHandle)
    {
        IVS_RUN_LOG_CRI("parameter invalid. 0 == nHandle");
        return RTSP_ERR_PARAMETER;
    }

	if(NULL == cbDataCallBack)
	{
		IVS_RUN_LOG_CRI("set data Call Back for NULL.");
	}

    // 检查连接是否存在
    CRtspSession* pSession = (CRtspSession*) (nHandle);
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("set data Call Back function fail, session not exist, handle[%p].", pSession);
        return RTSP_ERR_NOT_EXIST;
    }

    // 加锁
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    pSession->m_fDataCallBack = cbDataCallBack;
    pSession->m_pUser = pUser;
    IVS_DBG_LOG("set data Call Back function success, handle[%p].", pSession);
    return RET_OK;
}


static int sendSetupMessage(const bool bTcpFlag, CRtspSession* pSession, 
                           const char *sURL,
                           const INET_ADDR* pstServerAddr,
                           const MEDIA_ADDR* pstLocalMediaAddr)
{
    // 媒体流通过TCP方式传输，用回调函数传递出去
    if ((bTcpFlag) && (NULL == pSession->m_fDataCallBack))
    {
        IVS_RUN_LOG_CRI("data call back funtion not exist.");
        return RTSP_ERR_PARAMETER;
    }

    // 检查连接状态
    if (RTSP_SESSION_STATUS_INIT != pSession->getStatus())
    {
        IVS_RUN_LOG_CRI("send rtsp setup message fail, invalid status[%d], handle[%p].", 
                        pSession->getStatus(), pSession);
        return RTSP_ERR_STAUTS_ABNORMAL;
    }

    // 创建SETUP消息类，在收到响应或者超时释放
    CRtspSetupMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspSetupMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create rtsp setup requst message fail.");
        return RET_FAIL;
    }

    // 保存连接方式
    pRtspMsg->m_bTcpFlag = bTcpFlag;
    // 设置URL
    std::string strRtspUrl = "";
    strRtspUrl.append(sURL);
    pRtspMsg->setRtspUrl(strRtspUrl);
    // 设置传输类型
    if (bTcpFlag)
    {
        // Setup消息，记录TCP类型传输媒体流，硬解需要
        pRtspMsg->setTransType(TRANS_TYPE_TCP);
        if (NULL != pstLocalMediaAddr)
        {
            pRtspMsg->setDestinationIp(pstLocalMediaAddr->unMediaIp);       //lint !e613
            pRtspMsg->setClientPort(pstLocalMediaAddr->unMediaVideoPort, 
                                    pstLocalMediaAddr->unMediaAudioPort);   //lint !e613
        }
    }
    else
    {
        pRtspMsg->setTransType(TRANS_TYPE_UDP);
        // 设置媒体流本地接收地址和端口
        pRtspMsg->setDestinationIp(pstLocalMediaAddr->unMediaIp);       //lint !e613
        pRtspMsg->setClientPort(pstLocalMediaAddr->unMediaVideoPort, 
                                pstLocalMediaAddr->unMediaAudioPort);   //lint !e613

    }


    // 组装RTSP SETUP消息
    std::string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        return RET_FAIL;
    }

    // 建立连接
    ACE_INET_Addr remoteAddr(pstServerAddr->unAddrPort, pstServerAddr->unAddrIp);

    nRet = pSession->connectPeer(remoteAddr);
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("connect peer fail, remote address[%s:%d], handle[%p].", 
                        remoteAddr.get_host_addr(),
                        remoteAddr.get_port_number(),
                        pSession);
        (void)pSession->disconnectPeer();
        return RTSP_ERR_DISCONNECT;
    }
    IVS_RUN_LOG_INF("connect peer success, remote address [%s:%d], handle[%p].", 
                    remoteAddr.get_host_addr(),
                    remoteAddr.get_port_number(),
                    pSession);

    // 发送RTSP SETUP消息
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
		IVS_RUN_LOG_INF("sendSetupMessage failed, remote address [%s:%d], handle[%p] nRet[%d].", 
						remoteAddr.get_host_addr(),
						remoteAddr.get_port_number(),
						pSession, nRet);
        return nRet;
    }

	IVS_RUN_LOG_INF("sendSetupMessage succeed, remote address [%s:%d], handle[%p].", 
					remoteAddr.get_host_addr(),
					remoteAddr.get_port_number(),
					pSession);
    return RET_OK;
}


static int handleSynchSetup(CRtspSession* pSession, const unsigned int unTimeout,
                            const bool bTcpFlag, MEDIA_ADDR *pstPeerMediaAddr)
{
    // 等待信号触发，否则阻塞在此
    ACE_Time_Value wait = ACE_OS::gettimeofday();
    wait.sec (wait.sec () + unTimeout);

    int nRet = pSession->m_pRtspSemaphore->acquire(&wait);
    // 接收超时，直接返回
    if(0 != nRet)
    {
        ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex);
        IVS_RUN_LOG_ERR("receive rtsp setup response out of time, handle[%p].", pSession);
        pSession->destroyRtspMsg();
        return RTSP_ERR_TIMEOUT;
    }

	//IVS_RUN_LOG_ERR("----- handleSynchSetup 1, handle[%p].", pSession);

    // 解析错误，直接返回
    if (!pSession->m_bRecvRetFlag)
    {
		ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex);
		int iRet = pSession->m_iRetCode;
        pSession->destroyRtspMsg();
		IVS_RUN_LOG_ERR("receive rtsp setup response decode error, handle[%p], RetCode[%d].", pSession, iRet);
        return iRet;
    }

	//IVS_RUN_LOG_ERR("----- handleSynchSetup 2, handle[%p].", pSession);

    // UDP传输方式下，获取媒体流发送地址
    //if (false == bTcpFlag)
    CRtspSetupMessage* pRtspMsg = dynamic_cast<CRtspSetupMessage*>(pSession->m_pRtspMsg);
    if (NULL == pRtspMsg)
    {
		IVS_RUN_LOG_ERR("receive rtsp setup response msg is null, handle[%p].", pSession);
        return RET_FAIL;
    }
    if (0 != pRtspMsg->getServerIp())
    {
        pstPeerMediaAddr->unMediaIp        = pRtspMsg->getServerIp();           //lint !e613
        pstPeerMediaAddr->unMediaVideoPort = pRtspMsg->getServerVideoPort();    //lint !e613
        pstPeerMediaAddr->unMediaAudioPort = pRtspMsg->getServerAudioPort();    //lint !e613
    }
 
	//IVS_RUN_LOG_ERR("----- handleSynchSetup 3, handle[%p].", pSession);

    // 释放消息
    pSession->destroyRtspMsg();
    // 解析成功，更改状态为ready
    pSession->setStatus(RTSP_SESSION_STATUS_READY);
    IVS_RUN_LOG_INF("send setup message success, handle[%p].", pSession);
    return RET_OK;
}

/***************************************************************************
 函 数 名 : RTSP_CLIENT_SendSetupMsg
 功能描述 : 发送RTSP SETUP请求
 输入参数 :  nHandle                     连接句柄
            pstServerAddr               RTSP服务器地址
            sURL                        SETUP消息封装中的URL
            unTimeout                   SETUP消息超时时间（单位：S）
            bTcpFlag                    连接是否为TCP方式
            pstLocalMediaAddr           同步模式下，媒体流本地接收地址；异步模式下为空
 输出参数 : pstPeerMediaAddr            同步模式下，媒体流发送端地址；异步模式下为空
 返 回 值 : RET_OK-成功     其他-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendSetupMsg(int nHandle , const INET_ADDR* pstServerAddr,
                                    const char *sURL, unsigned int unTimeout, bool bTcpFlag,
                                    const MEDIA_ADDR* pstLocalMediaAddr, MEDIA_ADDR *pstPeerMediaAddr )
{ //lint !e1784
    // 参数检查
    if ((0 == nHandle) || (NULL == pstServerAddr)
        || (0 == unTimeout) || (NULL == sURL) 
        || (NULL == CRtspSessionManager::instance()->m_fExceptionCallBack))
    {
        IVS_RUN_LOG_ERR("send rtsp setup message fail, parameter invalid.");
        return RTSP_ERR_PARAMETER;
    }

    // UDP方式，检查媒体地址
    if ((!bTcpFlag) && ((NULL == pstLocalMediaAddr) || (NULL == pstPeerMediaAddr)))
    {
        IVS_RUN_LOG_ERR("udp channel must have media addr.");
        return RTSP_ERR_PARAMETER;
    }

    // 检查连接是否存在
    CRtspSession* pSession = (CRtspSession*) (nHandle);    
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_ERR("send rtsp setup message fail, session not exist, handle[%p].", pSession);
        return RTSP_ERR_NOT_EXIST;
    }

	// add by w00210470
	pSession->EnableRtpTimeoutCheck(NULL == pstLocalMediaAddr);

    // 加锁
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    int nRet = sendSetupMessage(bTcpFlag, pSession, sURL, pstServerAddr, pstLocalMediaAddr);
    if (RET_OK != nRet)
    {
		IVS_RUN_LOG_ERR("sendSetupMessage fail 2, handle[%p].", pSession);
        return nRet;
    }

    // 若是同步方式，等待信号量
    if (pSession->m_bBlocked)
    {
        nRet = handleSynchSetup(pSession, unTimeout, bTcpFlag, pstPeerMediaAddr);
        if (RET_OK != nRet)
        {
            IVS_RUN_LOG_INF("send setup message fail, synch mode handle[%p], RetCode[%d].", pSession, nRet);
        }
        return nRet;
    }
    else
    {
        // 异步方式下，注册超时定时器，在接收到响应消息或超时，取消定时器
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            return RET_FAIL;
        }
    }

    IVS_RUN_LOG_INF("send setup message success, handle[%p].", pSession);
    return RET_OK;
}


/***************************************************************************
 函 数 名 : RTSP_CLIENT_SendPlayMsg
 功能描述 : 发送RTSP PLAY请求
 输入参数 :  nHandle                    连接句柄
            sURL                        PLAY消息封装中的URL
            dScale                      回放/下载播放速率
            stRange                     回放/下载时间段
            unTimeout                   PLAY消息超时时间（单位：S）
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendPlayMsg(int nHandle, const char *sURL, double dScale, 
                                    const MEDIA_RANGE_S* stRange, unsigned int unTimeout)
{ //lint !e1784
    // 入参有效性检查
    if ((0 == nHandle) || (0 == unTimeout) || (NULL == sURL))
    {
        IVS_RUN_LOG_CRI("send rtsp play message fail, parameter is invalid.");
        return RTSP_ERR_PARAMETER;
    }

    // 检查连接是否存在
    CRtspSession* pSession = (CRtspSession*) (nHandle);    
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("send rtsp play message fail, session not exist, handle[%p].", pSession);
        return RTSP_ERR_NOT_EXIST;
    }

    // 加锁
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    // 检查连接状态
    if (RTSP_SESSION_STATUS_INIT == pSession->getStatus())
    {
        IVS_RUN_LOG_CRI("send rtsp play message fail, invalid status[%d], handle[%p].",
                        pSession->getStatus(),
                        pSession);
        return RTSP_ERR_STAUTS_ABNORMAL;
    }

    // 创建PLAY消息类，使用完后删除
    CRtspPlayMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspPlayMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        return RET_FAIL;
    }

    // 设置RTSP URL
    std::string strRtspUrl = "";
    strRtspUrl.append(sURL);
    pRtspMsg->setRtspUrl(strRtspUrl);
    pRtspMsg->setRang(stRange);
    pRtspMsg->setScale(dScale);

    // RTSP PLAY消息封装
    string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        return RET_FAIL;
    }

    // 发送RTSP PLAY消息
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        return nRet;
    }
    // 若是同步方式
    if (pSession->m_bBlocked)
    {
        // 等待信号触发，否则阻塞在此
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec (wait.sec () + unTimeout);
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (0 != nRet)
        {
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            return RTSP_ERR_TIMEOUT;
        }
        // 解析失败，直接返回
        if (!pSession->m_bRecvRetFlag)
        {
            return RET_FAIL;
        }
		// 返回成功，启动RTP数据检测
		pSession->SetupRtpTimer(); 
    }
    else
    {
        // 异步方式下，注册超时定时器
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            return RET_FAIL;
        }
    }


    IVS_DBG_LOG("send play message success, handle[%p].", pSession);
    return RET_OK;
}

/***************************************************************************
 函 数 名 : RTSP_CLIENT_SendOptionMsg
 功能描述 : 发送RTSP OPTIONS请求
 输入参数 :  nHandle                    连接句柄
            sURL                        OPTIONS消息封装中的URL
            unTimeout                   OPTIONS消息超时时间（单位：S）
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendOptionMsg(int nHandle,const char *sURL,unsigned int unTimeout, char *strMsg, int &iLength)
{
    if ((0 == nHandle) || (NULL == sURL) || (0 == unTimeout))
    {
        IVS_RUN_LOG_CRI("send teardown message fail, parameter is invalid.");
        return RTSP_ERR_PARAMETER;
    }

    // 检查连接是否存在
    CRtspSession* pSession = (CRtspSession*)nHandle;
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("handle is not a valid Session.");
        return RTSP_ERR_NOT_EXIST;
    }

    // 加锁
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);

    // 组装option消息
    CRtspOptionsMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspOptionsMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create option message failed.");
        return RET_FAIL;
    }

    std::string strRtspMsg = "";
    pRtspMsg->setRtspUrl(sURL);
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        IVS_RUN_LOG_CRI("encode pause message fail.");
        return RET_FAIL;
    }

    // 发送RTSP OPTION消息
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("send pause message fail.");
        return RET_FAIL;
    }

    // 如果是同步模式，等待收信结果
    if (pSession->m_bBlocked)
    {
        // 设置Pause消息的超时时间
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec(wait.sec() + unTimeout);
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (-1 == nRet)
        {
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Sync mode:recv pause response timeout.");
            return RTSP_ERR_TIMEOUT;
        }
        else if (0 != nRet)
        {
            IVS_RUN_LOG_CRI("Sync mode:recv pause response fail.");
            return RET_FAIL;
        }
        if (!pSession->m_bRecvRetFlag)
        {
            return RET_FAIL;
        }
        else
        {
            if (NULL == strMsg)
            {
                return RET_FAIL;
            }
            iLength = (int)pSession->m_strTimeInfo.length();
            memcpy(strMsg,pSession->m_strTimeInfo.c_str(),(unsigned int)iLength);
        }
    }
    else
    {   //异步模式，注册超时定时器
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Async mode:set pause recv timer failed.");
            return RET_FAIL;
        }
    }
    return RET_OK;
}
/***************************************************************************
 函 数 名 : RTSP_CLIENT_SendPauseMsg
 功能描述 : 发送RTSP PAUSE请求
 输入参数 :  nHandle                     连接句柄
             sURL                        PAUSE消息封装中的URL
             unTimeout                   PAUSE消息超时时间（单位：S）
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendPauseMsg(int nHandle, const char *sURL, unsigned int unTimeout)
{ //lint !e1784
    // 入参有效性检查
    if ((0 == nHandle) || (NULL == sURL) || (0 == unTimeout))
    {
        IVS_RUN_LOG_CRI("send pause message fail, parameter invalid.");
        return RTSP_ERR_PARAMETER;
    }

    // 检查连接是否存在
    CRtspSession* pSession = (CRtspSession*)nHandle;
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("handle is not a valid Session.");
        return RTSP_ERR_NOT_EXIST;
    }

    // 加锁
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    // 检查会话状态，如果已经是READY状态，直接返回
    if (RTSP_SESSION_STATUS_READY == pSession->getStatus())
    {
        return RET_OK;
    } 
    else if (RTSP_SESSION_STATUS_PLAYING != pSession->getStatus())
    {
        IVS_RUN_LOG_CRI("send rtsp pause message fail, invalid status[%d], handle[%p].", 
                        pSession->getStatus(),
                        pSession);
        // 如果不是PLAYING状态，返回错误
        return RTSP_ERR_STAUTS_ABNORMAL;
    }

    // 组装pause消息
    CRtspPauseMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspPauseMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create pause message failed.");
        return RET_FAIL;
    }

    pRtspMsg->setRtspUrl(sURL);
    std::string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        IVS_RUN_LOG_CRI("encode pause message fail.");
        return RET_FAIL;
    }

    // 发送RTSP Pause消息
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("send pause message fail.");
        return RET_FAIL;
    }

    // 如果是同步模式，等待收信结果
    if (pSession->m_bBlocked)
    {
        // 设置Pause消息的超时时间
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec(wait.sec() + unTimeout);
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (-1 == nRet)
        {
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Sync mode:recv pause response timeout.");
            return RTSP_ERR_TIMEOUT;
        }
        else if (0 != nRet)
        {
            IVS_RUN_LOG_CRI("Sync mode:recv pause response fail.");
            return RET_FAIL;
        }
        if (!pSession->m_bRecvRetFlag)
        {
            return RET_FAIL;
        }
    }
    else
    {   //异步模式，注册超时定时器
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("Async mode:set pause recv timer failed.");
            return RET_FAIL;
        }
    }
    return RET_OK;
}


/***************************************************************************
 函 数 名 : RTSP_CLIENT_SendTeardownMsg
 功能描述 : 发送RTSP TEARDOWN请求
 输入参数 :  nHandle                    连接句柄
            sURL                        TEARDOWN消息封装中的URL
            unTimeout                   PAUSE消息超时时间（单位：S）
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_SendTeardownMsg(int nHandle, const char *sURL, unsigned int unTimeout)
{ //lint !e1784
    // 入参检查
    if ((0 == nHandle) || (NULL == sURL) || (0 == unTimeout))
    {
        IVS_RUN_LOG_CRI("send teardown message fail, parameter is invalid.");
        return RTSP_ERR_PARAMETER;
    }

    // 连接查找
    CRtspSession* pSession = (CRtspSession*)nHandle;
    // 检查连接是否存在
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("handle is not a valid Session.");
        return RTSP_ERR_NOT_EXIST;
    }

    // 加锁
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    // 如果是INIT状态，不作处理，直接返回成功
    if (RTSP_SESSION_STATUS_INIT == pSession->getStatus())
    {
        IVS_RUN_LOG_INF("session[%p] status has been init already, don't need to teardown.", nHandle);
        return RET_OK;
    }

    // 组装Teardown消息
    CRtspTeardownMessage* pRtspMsg = NULL;
    pRtspMsg = pSession->createRtspMessage((CRtspTeardownMessage*)NULL); //lint !e838
    if (NULL == pRtspMsg)
    {
        IVS_RUN_LOG_CRI("create teardown message fail.");
        return RET_FAIL;
    }

    pRtspMsg->setRtspUrl(sURL);
    std::string strRtspMsg = "";
    int nRet = pRtspMsg->encodeMessage(strRtspMsg);
    if (RET_OK != nRet)
    {
        pSession->destroyRtspMsg();
        IVS_RUN_LOG_CRI("encode teardown message fail.");
        return RET_FAIL;
    }
    // 发送消息
    nRet = pSession->sendMessage(strRtspMsg.c_str(), strRtspMsg.length());
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("send teardown message fail.");
        return RET_FAIL;
    }

    // 同步模式接收消息
    if (pSession->m_bBlocked)
    {
        // 设置Teardown消息的超时时间
        ACE_Time_Value wait = ACE_OS::gettimeofday();
        wait.sec(wait.sec() + unTimeout);
        nRet = pSession->m_pRtspSemaphore->acquire(&wait);
        if (-1 == nRet)
        {
            // 是不是应该加锁
            ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_Mutex); //lint !e578
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("sync mode recv teardown response timeout, handle[%p].", pSession);

            return RTSP_ERR_TIMEOUT;
        }
        if (!pSession->m_bRecvRetFlag)
        {
            return RET_FAIL;
        }
    }
    else
    {
        // 异步模式，设置定时器
        nRet = pSession->setRecvTimer(unTimeout);
        if (RET_OK != nRet)
        {
            pSession->destroyRtspMsg();
            IVS_RUN_LOG_CRI("async mode set teardown recv timer fail.");
            return RET_FAIL;
        }
    }

    return RET_OK;
}



/***************************************************************************
 函 数 名 : RTSP_CLIENT_GetStatus
 功能描述 : 获取连接当前状态
 输入参数 : nHandle                     连接句柄
 输出参数 : 无
 返 回 值 : RTSP_SESSION_STATUS -连接状态
***************************************************************************/
RTSP_SESSION_STATUS RTSPAPI RTSP_CLIENT_GetStatus(int nHandle)
{ //lint !e1784
    // 入参检查
    if (0 == nHandle)
    {
        return RTSP_SESSION_STATUS_ABNORMAL;
    }

    // 检查连接是否存在
    CRtspSession* pSession = (CRtspSession*) (nHandle);
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("rtsp session not exist.");
        return RTSP_SESSION_STATUS_ABNORMAL;
    }

    return pSession->getStatus();
}


/***************************************************************************
 函 数 名 : RTSP_CLIENT_FreeHandle
 功能描述 : 释放连接句柄
 输入参数 : nHandle                     连接句柄
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_FreeHandle(int nHandle)
{ //lint !e1784
    // 入参有效性检查
    if (0 == nHandle)
    {
        return RTSP_ERR_PARAMETER;
    }

    // 检查连接是否存在
    CRtspSession* pSession = (CRtspSession*) (nHandle);
    if (!CRtspSessionManager::instance()->findSession(pSession))
    {
        IVS_RUN_LOG_CRI("rtsp session not exist.");
        return RTSP_ERR_NOT_EXIST;
    }
 
	/* 
	 *  connect 失败的 rtsp session 不会被注册到reactor的链表中
	 *  在这里由调用者删除
	 */
	if (pSession->IsConnectFail())
	{
		CRtspSessionManager::instance()->deleteSession(pSession);
		delete pSession;
		return RET_OK;
	}

    // 释放连接
	IVS_RUN_LOG_CRI("begin Set Status abnormal state.");
    ACE_Guard <ACE_Thread_Mutex> locker (pSession->m_SessionMutex);
    pSession->setStatus(RTSP_SESSION_STATUS_ABNORMAL);
	IVS_RUN_LOG_CRI("end Set Status abnormal state.");
    return RET_OK;
}


/***************************************************************************
 函 数 名 : RTSP_CLIENT_Cleanup
 功能描述 : 释放通讯库
 输入参数 : 无
 输出参数 : 无
 返 回 值 : RET_OK-成功     RET_FAIL-失败
***************************************************************************/
int RTSPAPI RTSP_CLIENT_Cleanup()
{ //lint !e1784
    if (g_bIsFirst)
    {
        return RET_OK;
    }
    g_bIsFirst = true;

    // 先关闭RTSP连接管理器
    CRtspSessionManager::instance()->closeManager();

    // 关闭反应器管理器
    if (RET_OK != CRtspReactorManager::instance()->closeManager())
    {
        return RET_FAIL;
    }

    // 关闭日志
    (void)CRtsp_Log_Manager::instance().endLog();
//
#ifndef WIN32
//    IVS_DBG_LOG("ACE fini");
    ACE::fini();
#endif

    return RET_OK;
}


