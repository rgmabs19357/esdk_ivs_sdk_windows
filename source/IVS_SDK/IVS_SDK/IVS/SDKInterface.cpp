#include "SDKInterface.h"
#include "bp_log.h"
#include "bp_environments.h"
#include "IService.h"
#include "Thread.h"
#include "HandleSignal.h"
#include "SDKMainCtrl.h"
#include "bp_net_factory.h"
#include "nss_mt.h"
#include "CapabilityMgr.h"
#include "rtsp_client_api.h"
#include "IVS_NetSource.h"
#include "IVS_Player.h"
#include "EventCallBackJob.h"
#include "SDKDef.h"
#include "SDKConfig.h"
#include "IVS_Trace.h"
#include "StreamEventMgr.h"

#define MAX_PORT 65535
#define MIN_PORT 1
#define MAX_IP_VALUE 255
#define IP_SPLIT_LEN 4
CSDKInterface::CSDKInterface(void)
	: m_bInit(false)
	, m_bBplogInit(true)
{
	m_pMutexArrayLock = VOS_CreateMutex();
	m_pMutexMapLock   = VOS_CreateMutex();
	m_pMutexLoginLock = VOS_CreateMutex();
	m_pMutexLoginByTktLock = VOS_CreateMutex();
    m_pRetriveStatusMapLock = VOS_CreateMutex();
	memset(m_SessionIDFlagArray, SDK_SESSION_NOTUSE, sizeof(m_SessionIDFlagArray));
}

CSDKInterface::~CSDKInterface(void)
{
	try  
    {
		if (m_bInit)
		{
			(void)Cleanup();
		}

        if (NULL != m_pMutexArrayLock)
        {
            VOS_DestroyMutex(m_pMutexArrayLock);
            m_pMutexArrayLock = NULL;
        }
    	
        if (NULL != m_pMutexMapLock)
        {
            VOS_DestroyMutex(m_pMutexMapLock);
            m_pMutexMapLock = NULL;
        }

		if (NULL != m_pMutexLoginLock)
		{
			VOS_DestroyMutex(m_pMutexLoginLock);
			m_pMutexLoginLock = NULL;
		}

		if (NULL != m_pMutexLoginByTktLock)
		{
			VOS_DestroyMutex(m_pMutexLoginByTktLock);
			m_pMutexLoginByTktLock = NULL;
		}

        if (NULL != m_pRetriveStatusMapLock)
        {
            VOS_DestroyMutex(m_pRetriveStatusMapLock);
            m_pRetriveStatusMapLock = NULL;
        }
	}
	catch (...)
	{
		// 不能打印日志,BP已卸载;
	}
}

// 初始化BP
IVS_INT32 CSDKInterface::BPInit()
{
	InitExceptionFilter();     
	ACE::init();
	CSDKMainctrl *pMainService = IVS_NEW(pMainService);   
	if (NULL == pMainService)                            
	{ 
        m_bBplogInit = false;
		return IVS_FAIL;// BP未初始化，不加日志
	}                                               
	pMainService->SetParam(NULL, NULL);
    // 初始化日志目录，m_strLogRootPath为空表示未设置路径，以配置文件为准
    const char* pLogPath = NULL;
    std::string strLogPath = CSDKConfig::instance().GetLogPath();
    if (!strLogPath.empty())
    {
        strLogPath.append(LOG_MODULE_NAME_SDK);
        pLogPath = strLogPath.c_str();
    }
	int iRet = pMainService->InitLog(pLogPath); 
	if (IVS_SUCCEED != iRet) 
	{
        m_bBplogInit = false;
		return iRet;// BP未初始化，不加日志 
	}
	BP_RUN_LOG_INF("init log success","NA");
	CHandleSignal::Init();                        
	CThread* pMainThread = IVS_NEW(pMainThread);              
	if (NULL == pMainThread)                      
	{                                               
		IVS_DELETE(pMainService) ;
		return IVS_FAIL;// BP未初始化，不加日志                             
	}                                               
	CThread::SetMainThread(pMainThread);            
	pMainThread->open("main", true);               
	CThreadMgr::instance().AddThread(pMainThread);  
	pMainService->Init(); 
	CMainCtrl::SetMainCtrl(pMainService);           
	pMainThread->AddService(pMainService);  

	// 这里注册能力，告警等能力需要通过这个注册
	// 告警信息通知
	CCapabilityMgr::instance().RegCapability(NSS_ALARM_NOTIFY_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());       //lint !e1013 !e1055  !e746 
	// 告警信息状态通知
	CCapabilityMgr::instance().RegCapability(NSS_ALARM_STATUS_NOTIFY_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID()); //lint !e1013
	// 联动动作执行、停止通知
	CCapabilityMgr::instance().RegCapability(NSS_DO_LINKAGE_ACTION_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
    CCapabilityMgr::instance().RegCapability(NSS_STOP_LINKAGE_ACTION_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
    // 通知客户端用户下线
    CCapabilityMgr::instance().RegCapability(NSS_USER_OFFLINE_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
	// 智能分析
	CCapabilityMgr::instance().RegCapability(NSS_BA_LEARNING_SCHEDULE_REPORT_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
	CCapabilityMgr::instance().RegCapability(NSS_FR_REPORT_SCHEDULE_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
	CCapabilityMgr::instance().RegCapability(NSS_PUSH_ALARM_LOCUS_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
	CCapabilityMgr::instance().RegCapability(NSS_ADDPLAN_SCHEDULE_REPORT_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
    // OMU告警
	CCapabilityMgr::instance().RegCapability(NSS_OMU_ALARM_CU_REPORT_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
	// 手动录像状态上报
	CCapabilityMgr::instance().RegCapability(NSS_NOTIF_MANUAL_RECORD_STATE_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
	// 搜索设备上报
	CCapabilityMgr::instance().RegCapability(NSS_DISCOVERED_DEVICE_NOTIFY_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
    // 模式轨迹录制结束通知
	CCapabilityMgr::instance().RegCapability(NSS_MODE_CRUISE_RECORD_OVER_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID());  //lint !e1013
	
    // 查询子设备列表通知
    CCapabilityMgr::instance().RegCapability(NSS_GET_USER_CHANNEL_LIST_RSP, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID()); //lint !e1013

	// 客户端关闭视频业务通知
	CCapabilityMgr::instance().RegCapability(NSS_SHUTDOWN_STREAM_NOTIFIY_REQ, pMainService->GetServiceID(), CThread::GetMainThread()->GetThreadID()); //lint !e1013


	return IVS_SUCCEED;
}

// 初始化
IVS_INT32 CSDKInterface::Init()
{
    int iRet = IVS_FAIL;
    do 
    {
		std::string strDir;
		TCHAR pBuf[MAX_PATH] = {'\0'};
		GetModuleFileName(::GetModuleHandle("IVS_SDK.dll"), pBuf, MAX_PATH-sizeof(TCHAR));
		char* cTemp = (char*)pBuf;
		std::string strTemp = cTemp;
		strDir = strTemp.substr(0,strTemp.rfind('\\')+1);
		set_home_directory(strDir);//lint !e1036
        if (IVS_SUCCEED != BPInit())
        {
            BP_RUN_LOG_ERR(IVS_FAIL,"SDK Interface Init Failed.", "Reason:BPInit Failed.");
            iRet = IVS_FAIL;
            break;
        }

        // 导入SDK配置文件失败，使用默认值，不直接返回失败；默认值在CSDKConfig构造函数中初始化
        int iLoadConfig = CSDKConfig::instance().LoadConfigFile();
        if (IVS_SUCCEED != iLoadConfig)
        {
            BP_RUN_LOG_ERR(IVS_FAIL,"LoadConfigFile Failed", "Client will use default config, iLoadConfig = %d",iLoadConfig);
        }
        const char* pLogPath = NULL;
        std::string strLogRootPath = CSDKConfig::instance().GetLogPath();
        if (!strLogRootPath.empty())
        {
            strLogRootPath.append(LOG_MODULE_NAME_RTSP);
            pLogPath = strLogRootPath.c_str();
        }
        // 初始化rtsp_client库;
        unsigned int uiLogLevel = CSDKConfig::instance().GetRtspLogLevel();
        iRet = RTSP_CLIENT_Init(uiLogLevel, pLogPath);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(IVS_SDK_RET_INIT_RTSPCLIENT_ERROR,"Init rtsp client ", "failed.");
            iRet = IVS_SDK_RET_INIT_RTSPCLIENT_ERROR;
            (void)BPCleanup();
            return iRet;
        }
        strLogRootPath = CSDKConfig::instance().GetLogPath();
        pLogPath = NULL;
        if (!strLogRootPath.empty())
        {
            strLogRootPath.append(LOG_SPLIT_CHAR);
            pLogPath = strLogRootPath.c_str();
        }
        // 初始化NetSource库;
        iRet = IVS_NETSOURCE_Init(pLogPath);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(IVS_PLAYER_RET_INIT_NETSOURCE_ERROR,"Init net source ", "failed.");
            iRet = IVS_PLAYER_RET_INIT_NETSOURCE_ERROR;
            (void)BPCleanup();
            RTSP_CLIENT_Cleanup();
            return iRet;
        }
        strLogRootPath = CSDKConfig::instance().GetLogPath();
        pLogPath = NULL;
        if (!strLogRootPath.empty())
        {
            strLogRootPath.append(LOG_SPLIT_CHAR);
            pLogPath = strLogRootPath.c_str();
        }
        // 初始化IVS_Player播放库;
        iRet = IVS_PLAY_Init(pLogPath);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(IVS_PLAYER_RET_INIT_PLAY_ERROR,"Init play ", "failed.");
            iRet = IVS_PLAYER_RET_INIT_PLAY_ERROR;
            (void)BPCleanup();
            RTSP_CLIENT_Cleanup();
            IVS_NETSOURCE_UnInit();
            return iRet;
        }

        iRet = m_eventMgr.Init();
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet,"Init event mgr ", "failed.");
            (void)BPCleanup();
            RTSP_CLIENT_Cleanup();
            IVS_NETSOURCE_UnInit();
            IVS_PLAY_UnInit();
            return iRet;
        }

        iRet = CStreamEventMgr::instance().Init();
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet,"Init stream event mgr ", "failed.");
            m_eventMgr.UnInit();
            (void)BPCleanup();
            RTSP_CLIENT_Cleanup();
            IVS_NETSOURCE_UnInit();
            IVS_PLAY_UnInit();
            return iRet;
        }

    } while (false);

	
	return iRet;
}

IVS_INT32 CSDKInterface::BPCleanup()
{
	CMainCtrl* pMainCtrl = CMainCtrl::GetMainCtrl();
	if (NULL != pMainCtrl)                          
	{                                               
		pMainCtrl->UnInit();                        
		//delete pMainCtrl;                           
	}    
	//// 关掉所有线程，包括主线程（主线程也在这个里面）
	//CThreadMgr::instance().close(); 

 //   //初始化日志不成功，m_bBplogInit被置为false，不用清理日志;
 //   if (m_bBplogInit)
 //   {
 //       IVS_LogClean();
 //   }
	ACE::fini();    

	return IVS_SUCCEED;
}//lint !e1762

// 清理
IVS_INT32 CSDKInterface::Cleanup()
{
	IVS_DEBUG_TRACE("");
    m_bInit = false;

    CStreamEventMgr::instance().UnInit();

    // 停止事件管理;
    m_eventMgr.UnInit();

	// 删除本地用户;
	//(void)DeleteLocalUserObj();
	int iKey = IVS_SUCCEED;

	int iSessionIDMapSize = m_mapSessionIDToSingleSvrProxy.size();//lint !e713
	MAP_SESSEIONID_USERMGR_ITER svrProxyIter = m_mapSessionIDToSingleSvrProxy.begin();
	while (iSessionIDMapSize>0)
	{
		svrProxyIter = m_mapSessionIDToSingleSvrProxy.begin();

		//每次logout都会删除map中的当前元素,因此每次都指向开始元素;
		iKey = svrProxyIter->first;
		(void)Logout(iKey);

		//删除之后更新size值;
		iSessionIDMapSize = m_mapSessionIDToSingleSvrProxy.size();//lint !e713
	}

	m_mapSessionIDToSingleSvrProxy.clear();

	//停止回调线程;
	//if (NULL != m_pEventCallBackThread)
	//{
	//    (void)m_pEventCallBackThread->Stop();
	//    delete m_pEventCallBackThread;
	//    m_pEventCallBackThread = NULL;
	//}

	//释放协议播放库
	IVS_INT32 iRet = IVS_FAIL;// = IVS_MLIB_Cleanup();
	
    // cleanup bp by z00193167   modify by x00192614，BPCleanup需在用户注销之后，否则注销请求发送不出去
    iRet = BPCleanup();
	if (IVS_SUCCEED != iRet)
    {
        // BP库已经释放，不能打印日志
        //BP_RUN_LOG_ERR(iRet,"BPCleanup ", "failed");
        return iRet;
    }

    iRet = RTSP_CLIENT_Cleanup();
    IVS_NETSOURCE_UnInit();
    IVS_PLAY_UnInit();
	return iRet;
}

IVS_INT32 CSDKInterface::SetEventCallBack( IVS_INT32 iSessionID, EventCallBack fEventCallBack /*= NULL*/, void *pUserData /*= NULL*/ )
{
	if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID, "Set Event CallBack", "Param error, SessionID out of range.");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}

	CUserMgr *pUserMgr = GetUserMgr(iSessionID);
	if (NULL == pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID, "Set Event CallBack", "Get UserMgr by id failed.");
		return IVS_PARA_INVALID;
	}
	pUserMgr->SetEventCallBack(fEventCallBack, pUserData);

	return m_eventMgr.SetEventCallBack(iSessionID, fEventCallBack, pUserData);
}

// 获取会话ID
int CSDKInterface::GetSessoinID()
{
    (void)VOS_MutexLock(m_pMutexArrayLock);
	for (int iSessionID = SDK_CLIENT_STARTNO; iSessionID < SDK_CLIENT_TOTAL; iSessionID++)
	{
		if (SDK_SESSION_NOTUSE == m_SessionIDFlagArray[iSessionID])
		{
			m_SessionIDFlagArray[iSessionID] = SDK_SESSION_INUSE;
			(void)VOS_MutexUnlock(m_pMutexArrayLock);
			return iSessionID;
		}
	}
    (void)VOS_MutexUnlock(m_pMutexArrayLock);
	return SDK_SESSIONID_INIT;
}

// 释放SessionID
IVS_INT32 CSDKInterface::FreeSessoinID(int iSessionID)
{
	if((SDK_CLIENT_STARTNO <= iSessionID) && (iSessionID < SDK_CLIENT_TOTAL))
	{
		(void)VOS_MutexLock(m_pMutexArrayLock);

		m_SessionIDFlagArray[iSessionID] = SDK_SESSION_NOTUSE;
		m_mapSessionIDToSingleSvrProxy.erase(iSessionID);
		(void)VOS_MutexUnlock(m_pMutexArrayLock);

		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

// 插入SessionID到map中
void CSDKInterface::AddSessoinID_SingleSvrProxyToMap(int iSessionID, CUserMgr *pSingleSvrProxy)
{
	if ((SDK_CLIENT_STARTNO <= iSessionID) && (iSessionID < SDK_CLIENT_TOTAL))
	{
		(void)VOS_MutexLock(m_pMutexMapLock);
		m_mapSessionIDToSingleSvrProxy[iSessionID] = pSingleSvrProxy;
		(void)VOS_MutexUnlock(m_pMutexMapLock);
	}
}

// 根据SessionID获取用户对象
CUserMgr *CSDKInterface::GetUserMgr(int iSessionID)
{
	if (SDK_CLIENT_STARTNO > iSessionID || SDK_CLIENT_TOTAL <= iSessionID)
	{
		return NULL;
	}
	
	CUserMgr *pUserMgr = NULL;
	(void)VOS_MutexLock(m_pMutexMapLock);
    MAP_SESSEIONID_USERMGR_ITER iter = m_mapSessionIDToSingleSvrProxy.find(iSessionID);
	if (iter != m_mapSessionIDToSingleSvrProxy.end())
	{
		pUserMgr = iter->second;
	}
	(void)VOS_MutexUnlock(m_pMutexMapLock);
	return pUserMgr;
}

//add by w00206574 start
//检查字符串是否为数字串
IVS_BOOL CheckIsDigit(IVS_CHAR* pDigitChar)
{
	// 参数检验;
	CHECK_POINTER(pDigitChar, IVS_IP_INVALID);
	IVS_INT32 iLen = strlen(pDigitChar);//字符串长度
    if (1 != iLen && '0' == *pDigitChar )//长度非1的字符串首字母不能为0
	{
		return false;
	}

	for (IVS_INT32 index =0;index<iLen;index++)
	{
		if (false == isdigit(*(pDigitChar+index)))
		{
			return false;
		}
	}
	return true;
}
//校验IP和PORT是否合法
IVS_BOOL CheckIpAndPort(const IVS_LOGIN_INFO* pLoginReqInfo)
{
	// 参数检验;
	CHECK_POINTER(pLoginReqInfo, IVS_IP_INVALID);
	try
	{
		IVS_CHAR pPort[IVS_DOMAIN_LEN] = {0};
		sprintf(pPort, "%d", pLoginReqInfo->uiPort);
		struct addrinfo *pResult =NULL;

		if (MIN_PORT > pLoginReqInfo->uiPort || pLoginReqInfo->uiPort> MAX_PORT)//端口不合法
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ","pLoginReqInfo->uiPort is Error");
			return IVS_PORT_INVALID;
		}
		else if (0 == pLoginReqInfo->stIP.uiIPType)//ipv4
		{
			IVS_INT32 iCount = 0;    
			const IVS_CHAR *pSplitChar = ".";    
			IVS_CHAR cTempIP[IVS_IP_LEN +1]={0};    
			IVS_INT32 iIPAddr[IP_SPLIT_LEN] = {0};   
			strcpy(cTempIP,pLoginReqInfo->stIP.cIP);
			for(IVS_UINT32  index = 0; index < strlen(cTempIP); index++)    
			{        
				if(cTempIP[index] == '.')        
				{            
					iCount++;                     
				}        
			}     
			if(iCount != IP_SPLIT_LEN - 1)    
			{       
				return IVS_IP_INVALID;   
			} 

			IVS_CHAR* cTemp = strtok(cTempIP, pSplitChar);
			for (IVS_INT32 index = 0;index < IP_SPLIT_LEN;index++)
			{
				if (NULL == cTemp|| *cTemp==' ' || !CheckIsDigit(cTemp))
				{
					return IVS_IP_INVALID;
				}
				iIPAddr[index] = atoi(cTemp);
				cTemp = strtok(NULL, pSplitChar);
			}
			
			if ((iIPAddr[0] >= 0&&iIPAddr[0] <= MAX_IP_VALUE)&&(iIPAddr[1] >= 0&&iIPAddr[1] <= MAX_IP_VALUE)&&(iIPAddr[2] >= 0&&iIPAddr[2] <= MAX_IP_VALUE)&&(iIPAddr[3] >= 0&&iIPAddr[3] <= MAX_IP_VALUE))   
			{           
				return IVS_SUCCEED;            
			}   
			else    
			{       
				return IVS_IP_INVALID;    
			}
		}
		else if (0 != getaddrinfo(pLoginReqInfo->stIP.cIP,pPort,NULL,&pResult))//ip不合法
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ","stIP.cIP is Error");
			return IVS_IP_INVALID;		
		}
	}
	catch (...)
	{
		return IVS_IP_INVALID;
	}
	return IVS_SUCCEED;
}

//add by w00206574 end

// 用户登录
IVS_INT32 CSDKInterface::Login(const IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId)//lint !e830
{
    // 参数检验;
    CHECK_POINTER(pLoginReqInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pSessionId, IVS_OPERATE_MEMORY_ERROR);

    //IP类型校验 
    if ((IP_V6 != pLoginReqInfo->stIP.uiIPType) && (IP_V4 != pLoginReqInfo->stIP.uiIPType))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ", "stIP.iIPType is Error");
        return IVS_PARA_INVALID;
    }

	//用户名、IP、机器名校验
    if ('\0' == pLoginReqInfo->cUserName[0] || '\0' == pLoginReqInfo->stIP.cIP[0]) //lint !e774    
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ", "Param is Error");
        return IVS_PARA_INVALID;
    }
    //客户端类型校验
    if ((CLIENT_PC > pLoginReqInfo->uiClientType) || (CLIENT_TV_WALL < pLoginReqInfo->uiClientType)) //lint !e568 !e685    
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ","iClientType is Error");
        return IVS_PARA_INVALID;
    }
	//add by wangguangyan start
	IVS_INT32 iCheckIpPort = CheckIpAndPort(pLoginReqInfo);
	if (IVS_SUCCEED != iCheckIpPort)
	{
		return iCheckIpPort;
	}
	//add by wangguangyan end

	//add by zwx211831, Date:20140703, 对登录接口进行加锁，确保同步登录，防止对smu的冲击
	CLockGuard oReLinklock(m_pMutexLoginLock);

    // 删除相同用户
    (void)RemoveUser(pLoginReqInfo->cUserName, pLoginReqInfo->stIP.cIP, "");

    int iSessionID = GetSessoinID();
    if(SDK_SESSIONID_INIT == iSessionID)  
    {
        // 已达最大用户数;
        BP_RUN_LOG_WAR("Login ", "Over max user count, User's iSessionID = %d", iSessionID);
        return IVS_SDK_ERR_ERR_MAX_USERNUM;
    }

    //构造内部结构体，
    IVS_LOGIN_INFO stLoginReqInfo;
	if(!CToolsHelp::Memcpy(&stLoginReqInfo,sizeof(IVS_LOGIN_INFO),pLoginReqInfo,sizeof(IVS_LOGIN_INFO)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Login", "stLoginReqInfo Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    stLoginReqInfo.stIP.cIP[IVS_IP_LEN - 1] = '\0'; //IP实际长度不会达到64，强制char数组末尾加'\0'
    stLoginReqInfo.uiLoginType = LOGIN_BASIC;
    CToolsHelp::GetHostName(stLoginReqInfo.cMachineName);

    CUserMgr& userMgr = m_UserMgrArray[iSessionID];//lint !e838
    userMgr.SetSingleSvrProxy(this);
    userMgr.SetSessionID(iSessionID);
    AddSessoinID_SingleSvrProxyToMap(iSessionID, &userMgr);

    int iRet = userMgr.Login(&stLoginReqInfo);

	// 清理内存，保证内存中不会有密码的存在
	memset(&stLoginReqInfo,0,sizeof(IVS_LOGIN_INFO));

    // 首次登录，密码过期均视作登录成功，由调用方根据返回码限制用户修改密码
    if(IVS_SUCCEED != iRet && IVS_SMU_USER_FIRST_LOGIN != iRet && IVS_SMU_USER_PWD_NEED_MOD != iRet)
    {
        (void)FreeSessoinID(iSessionID);
        userMgr.SetSessionID(SDK_SESSIONID_INIT);
        BP_RUN_LOG_ERR(iRet, "Login", "Login is failed");
        return iRet;
    }

    *pSessionId = iSessionID;  //保存sessionId

    // 为每一个用户对象设置回调，函数指针相同但用户数据为sessionID;
    (void)RTSP_CLIENT_SetExceptionCallBack(m_eventMgr.GetRtspExceptionCallBack(), (void*)iSessionID);
    IVS_NETSOURCE_SetExceptionCallBack(m_eventMgr.GetNetExceptionCallBack(), (void*)iSessionID);
    IVS_PLAY_SetExceptionCallBack(m_eventMgr.GetPlayerExceptionCallBack(), (void*)iSessionID);   
    IVS_PLAY_SetEventCallBack(CStreamEventMgr::instance().GetPlayerEventCallBack(), (void*)iSessionID);
    return iRet;
}


//用户单点登录;
IVS_INT32 CSDKInterface::LoginByTicket(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, IVS_INT32* pSessionId)
{
	// 参数检验;
	CHECK_POINTER(pLoginReqInfoEx, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSessionId, IVS_OPERATE_MEMORY_ERROR);

	//IP类型校验;
	if ((IP_V6 != pLoginReqInfoEx->stIP.uiIPType) && (IP_V4 != pLoginReqInfoEx->stIP.uiIPType))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login By Ticket ", "stIP.iIPType is Error");
		return IVS_PARA_INVALID;
	}

	//TicketID、IP校验
	if ('\0' == pLoginReqInfoEx->stIP.cIP[0] || '\0' == pLoginReqInfoEx->cTicketID[0]) //lint !e774    
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login By Ticket, Param is Error. IP: %s, TicketID: %s", 
			pLoginReqInfoEx->stIP.cIP, pLoginReqInfoEx->cTicketID);
		return IVS_PARA_INVALID;
	}

	//客户端类型校验;
	if ((CLIENT_PC > pLoginReqInfoEx->uiClientType) || (CLIENT_TV_WALL < pLoginReqInfoEx->uiClientType)) //lint !e568 !e685    
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login By Ticket ","iClientType is Error");
		return IVS_PARA_INVALID;
	}

	//add by zwx211831, Date:20140703, 对登录接口进行加锁，确保同步登录，防止对smu的冲击
	CLockGuard oReLinklock(m_pMutexLoginByTktLock);

	// 删除相同用户;
	(void)RemoveUserByTicketID(pLoginReqInfoEx->cTicketID, pLoginReqInfoEx->stIP.cIP, "");

	int iSessionID = GetSessoinID();
	if(SDK_SESSIONID_INIT == iSessionID)  
	{
		// 已达最大用户数;
		BP_RUN_LOG_WAR("Login ", "Over max user count, User's iSessionID = %d", iSessionID);
		return IVS_SDK_ERR_ERR_MAX_USERNUM;
	}

	//构造内部结构体;
	IVS_LOGIN_INFO_EX stLoginReqInfoEx;
	if(!CToolsHelp::Memcpy(&stLoginReqInfoEx,sizeof(IVS_LOGIN_INFO_EX),pLoginReqInfoEx,sizeof(IVS_LOGIN_INFO_EX)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Login By Ticket", "stLoginReqInfo Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	stLoginReqInfoEx.stIP.cIP[IVS_IP_LEN - 1] = '\0'; //IP实际长度不会达到64，强制char数组末尾加'\0'
	stLoginReqInfoEx.uiLoginType = LOGIN_BASIC;
	CToolsHelp::GetHostName(stLoginReqInfoEx.cMachineName);

	CUserMgr& userMgr = m_UserMgrArray[iSessionID];//lint !e838
	userMgr.SetSingleSvrProxy(this);
	userMgr.SetSessionID(iSessionID);
	AddSessoinID_SingleSvrProxyToMap(iSessionID, &userMgr);

	int iRet = userMgr.LoginByTicket(&stLoginReqInfoEx);

	// 首次登录，密码过期均视作登录成功，由调用方根据返回码限制用户修改密码;
	if(IVS_SUCCEED != iRet && IVS_SMU_USER_FIRST_LOGIN != iRet && IVS_SMU_USER_PWD_NEED_MOD != iRet)
	{
		(void)FreeSessoinID(iSessionID);
		userMgr.SetSessionID(SDK_SESSIONID_INIT);
		BP_RUN_LOG_ERR(iRet, "Login By Ticket", "Login is failed");
		return iRet;
	}

	*pSessionId = iSessionID;  //保存sessionId

	// 为每一个用户对象设置回调，函数指针相同但用户数据为sessionID;
	(void)RTSP_CLIENT_SetExceptionCallBack(m_eventMgr.GetRtspExceptionCallBack(), (void*)iSessionID);
	IVS_NETSOURCE_SetExceptionCallBack(m_eventMgr.GetNetExceptionCallBack(), (void*)iSessionID);
	IVS_PLAY_SetExceptionCallBack(m_eventMgr.GetPlayerExceptionCallBack(), (void*)iSessionID);   
	IVS_PLAY_SetEventCallBack(CStreamEventMgr::instance().GetPlayerEventCallBack(), (void*)iSessionID);
	return iRet;
}


// window用户登录
IVS_INT32 CSDKInterface::WinUserLogin(const IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId)//lint !e830
{
    // 参数检验;
	CHECK_POINTER(pLoginReqInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSessionId, IVS_OPERATE_MEMORY_ERROR);

	//IP类型校验 
	if ((IP_V6 != pLoginReqInfo->stIP.uiIPType) && (IP_V4 != pLoginReqInfo->stIP.uiIPType))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ", "stIP.iIPType is Error");
		return IVS_PARA_INVALID;
	}

	//用户名、IP、机器名校验
    if (0 == strlen(pLoginReqInfo->cUserName) || 0 == strlen(pLoginReqInfo->stIP.cIP))
	// if ((NULL == pLoginReqInfo->cUserName) || (NULL == pLoginReqInfo->stIP.cIP)) //lint !e774    
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ", "Param is Error");
		return IVS_PARA_INVALID;
	}

	//客户端类型校验
	if ((CLIENT_PC > pLoginReqInfo->uiClientType) || (CLIENT_TV_WALL < pLoginReqInfo->uiClientType)) //lint !e568 !e685    
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Login ", "iClientType is Error");
		return IVS_PARA_INVALID;
	}

	// 删除相同用户，需根据用户名，IP和域名
	(void)RemoveUser(pLoginReqInfo->cUserName, pLoginReqInfo->stIP.cIP, pLoginReqInfo->cDomainName);

	int iSessionID = GetSessoinID();
	if(SDK_SESSIONID_INIT == iSessionID)  
	{
		// 已达最大用户数;
		BP_RUN_LOG_WAR("Login ", "User's number is the max");
		return IVS_SDK_ERR_ERR_MAX_USERNUM;
	}

	//构造内部结构体，
	IVS_LOGIN_INFO stLoginReqInfo;
	memset(&stLoginReqInfo, 0, sizeof(stLoginReqInfo));
	if(!CToolsHelp::Memcpy(&stLoginReqInfo,sizeof(IVS_LOGIN_INFO),pLoginReqInfo,sizeof(IVS_LOGIN_INFO)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Login", "stLoginReqInfo Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    stLoginReqInfo.stIP.cIP[IVS_IP_LEN - 1] = '\0'; //IP实际长度不会达到64，强制char数组末尾加'\0'
	CToolsHelp::GetHostName(stLoginReqInfo.cMachineName);

	CUserMgr& userMgr = m_UserMgrArray[iSessionID];//lint !e838
	userMgr.SetSingleSvrProxy(this);
	userMgr.SetSessionID(iSessionID);
    AddSessoinID_SingleSvrProxyToMap(iSessionID, &userMgr);

	int iRet = userMgr.Login(&stLoginReqInfo);
	if(IVS_SUCCEED != iRet)
	{
		(void)FreeSessoinID(iSessionID);
        userMgr.SetSessionID(SDK_SESSIONID_INIT);
		BP_RUN_LOG_ERR(iRet, "Login ", "Login is failed");
		return iRet;
	}

	*pSessionId = iSessionID;  //保存sessionId

    // 为每一个用户对象设置回调，函数指针相同但用户数据为sessionID;
    (void)RTSP_CLIENT_SetExceptionCallBack(m_eventMgr.GetRtspExceptionCallBack(), (void*)iSessionID);
    IVS_NETSOURCE_SetExceptionCallBack(m_eventMgr.GetNetExceptionCallBack(), (void*)iSessionID);
    IVS_PLAY_SetExceptionCallBack(m_eventMgr.GetPlayerExceptionCallBack(), (void*)iSessionID);    
    IVS_PLAY_SetEventCallBack(CStreamEventMgr::instance().GetPlayerEventCallBack(), (void*)iSessionID);
    return iRet;
}

// 根据loginID查询 SessionID
IVS_INT32 CSDKInterface::GetSessionIDByLoginID(const string &strLoginID)
{
	IVS_DEBUG_TRACE("LoginID:%s", strLoginID.c_str());
	// 查询已登陆的会话，获取对应的iSessionID
	int iSessionID = SDK_SESSIONID_INIT;
	if (strLoginID.empty())
	{
		BP_RUN_LOG_INF("Get Session ID By Login ID", "Leave strLoginID is empty");
		return iSessionID;
	}
	// TODO strLoginID为密文时，需解密比较
	CUserMgr *pSingleSvrProxy = NULL;
	(void)VOS_MutexLock(m_pMutexMapLock);
	
	string strLoginIDTemp;
	MAP_SESSEIONID_USERMGR_ITER iter = m_mapSessionIDToSingleSvrProxy.begin();
	MAP_SESSEIONID_USERMGR_ITER iterEnd = m_mapSessionIDToSingleSvrProxy.end();
	for (;iter != iterEnd; iter++)
	{
		pSingleSvrProxy = dynamic_cast<CUserMgr*>(iter->second);  //lint !e611
	    if (NULL == pSingleSvrProxy)
	    {
			continue;
	    }
		strLoginIDTemp = pSingleSvrProxy->GetLoginId();
		if (0 == strLoginIDTemp.compare(strLoginID))
		{
			iSessionID = iter->first;
			break;
		}
	}
	(void)VOS_MutexUnlock(m_pMutexMapLock);
	return iSessionID;
}

// 根据Rtsp句柄获取CUserMgr
CUserMgr*  CSDKInterface::GetUserMgrbyRtspHandle(const long iRtspHandle)
{
	IVS_DEBUG_TRACE("RtspHandle:%d", iRtspHandle);
	IVS_ULONG ulPlayHandle = 0;
	CUserMgr *pUserMgr = NULL;

	VOS_MutexLock(m_pMutexMapLock);

	MAP_SESSEIONID_USERMGR_ITER iter = m_mapSessionIDToSingleSvrProxy.begin();
	MAP_SESSEIONID_USERMGR_ITER iterEnd = m_mapSessionIDToSingleSvrProxy.end();
	for (;iter != iterEnd; iter++)
	{
		pUserMgr = iter->second;
		if (NULL == pUserMgr)
		{
			continue;
		}
		ulPlayHandle = pUserMgr->GetPlayHandlebyRtspHandle(iRtspHandle);
		if (0 < ulPlayHandle)
		{
			break;
		}
	}
	if (iterEnd == iter)
	{
		pUserMgr =NULL;
	}
	VOS_MutexUnlock(m_pMutexMapLock);
	return pUserMgr;
}

// 用户注销
int CSDKInterface::Logout(int iSessionID, bool iSendLogoutCmd)
{
    IVS_DEBUG_TRACE("SessionID:%d, SendLogoutCmd:%d", iSessionID, (int)iSendLogoutCmd);
	if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID, "Logout ", "SessionID is Illegal");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}

	// 获得用户对象
	CUserMgr *pUserMgr = GetUserMgr(iSessionID);
	if(NULL == pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID, "Logout ", "Get UserMgr failed");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}

	//add by zwx211831, Date:20140618, 取消已注册的事件回调函数
	pUserMgr->SetEventCallBack(NULL, NULL);
	(void)m_eventMgr.SetEventCallBack(iSessionID, NULL, NULL);
	//end

	(void)FreeSessoinID(iSessionID);

	//注销用户前停止对讲
	//if(m_bIsVoiceTalkbackOpened)
	//{
	//	(void)pSingleSvrProxy->StopVoiceTalkback(m_voiceTalkbackHandle);
	//	m_bIsVoiceTalkbackOpened = false;
	//	m_voiceTalkbackHandle = -1;
	//}

	pUserMgr->GetRealPlayMgr().StopAllRealPlay();

	//pSingleSvrProxy->StopAllRemotePlayBack();


	//MediaLocalPlayback *pMediaLocalPlayback = MediaLocalPlayback::GetInstance();

	//CHECK_POINTER(pMediaLocalPlayback, SDK_ERR_NOT_ENOUGH_MEMERY);

	//pMediaLocalPlayback->StopAllLocalVedioPlay();

	return  pUserMgr->Logout(iSendLogoutCmd);
}

// 删除用户
int CSDKInterface::RemoveUser(const string &strUserName, const string &strIP, const string &strDomainName)
{
	IVS_DEBUG_TRACE("UserName:%s IP:%s DomainName:%s", strUserName.c_str(), strIP.c_str(), strDomainName.c_str());

	int iSessionID = SDK_SESSIONID_INIT;
	(void)VOS_MutexLock(m_pMutexMapLock);
	MAP_SESSEIONID_USERMGR_ITER svrProxyIter = m_mapSessionIDToSingleSvrProxy.begin();
	for (; svrProxyIter != m_mapSessionIDToSingleSvrProxy.end(); svrProxyIter++)
	{
        CUserMgr *pUserMgr = svrProxyIter->second;
        if (NULL == pUserMgr)
        {
            continue;
        }

		CUserInfo *pUserInfo = pUserMgr->GetUserInfoEx();
        if (NULL == pUserInfo)
        {
            continue;
        }
		
		if (strDomainName.empty())
		{
			// 只检验用户名和IP
			if ((strUserName == pUserInfo->GetLoginName())
				&&(strIP == pUserInfo->GetServerIP())
				)
			{
				iSessionID = pUserInfo->GetSdkSessionID();
				break;
			}
		}
		else
		{
			// 检验用户名，IP和域名
			if ((strUserName == pUserInfo->GetLoginName())
				&&(strIP == pUserInfo->GetServerIP())
				&& (strDomainName == pUserInfo->GetDomainName()))
			{
				iSessionID = pUserInfo->GetSdkSessionID();
				break;
			}
		}
	}
	(void)VOS_MutexUnlock(m_pMutexMapLock);

	if (SDK_SESSIONID_INIT != iSessionID)
	{
		return Logout(iSessionID);
	}
	return IVS_FAIL;
}

//登出已单点登录的用户;
IVS_INT32 CSDKInterface::RemoveUserByTicketID(const string &strTicketID, const string &strIP, const string &strDomainName)
{
	IVS_DEBUG_TRACE("TicketID:%s IP:%s DomainName:%s", strTicketID.c_str(), strIP.c_str(), strDomainName.c_str());

	int iSessionID = SDK_SESSIONID_INIT;
	VOS_MutexLock(m_pMutexMapLock);
	MAP_SESSEIONID_USERMGR_ITER svrProxyIter = m_mapSessionIDToSingleSvrProxy.begin();
	for (; svrProxyIter != m_mapSessionIDToSingleSvrProxy.end(); svrProxyIter++)
	{
		CUserMgr *pUserMgr = svrProxyIter->second;
		if (NULL == pUserMgr)
		{
			continue;
		}

		CUserInfo *pUserInfo = pUserMgr->GetUserInfoEx();
		if (NULL == pUserInfo)
		{
			continue;
		}

		if (strDomainName.empty())
		{
			// 只检验用户名和IP;
			if ((strTicketID == pUserInfo->GetTicketID())
				&&(strIP == pUserInfo->GetServerIP())
				)
			{
				iSessionID = pUserInfo->GetSdkSessionID();
				break;
			}
		}
		else
		{
			// 检验用户名，IP和域名;
			if ((strTicketID == pUserInfo->GetTicketID())
				&&(strIP == pUserInfo->GetServerIP())
				&& (strDomainName == pUserInfo->GetDomainName()))
			{
				iSessionID = pUserInfo->GetSdkSessionID();
				break;
			}
		}
	}
	VOS_MutexUnlock(m_pMutexMapLock);

	if (SDK_SESSIONID_INIT != iSessionID)
	{
		return Logout(iSessionID);
	}
	return IVS_FAIL;
}

//释放SDK申请的内存
IVS_INT32 CSDKInterface::ReleaseBuffer(IVS_CHAR *pBuffer) const
{
    CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("pBuffer: %08x", pBuffer);
	IVS_DELETE(pBuffer, MULTI);
	return IVS_SUCCEED;
}


void CSDKInterface::AddTransIDChannelListRetriveStatusMap(std::string& strTransID, IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus)
{

    (void)VOS_MutexLock(m_pRetriveStatusMapLock);
    m_ChannelListRetriveStatusMap.insert(std::make_pair(strTransID, channelListRetriveStatus));
    (void)VOS_MutexUnlock(m_pRetriveStatusMapLock);

}

void CSDKInterface::RemoveChannelListRetriveStatusByTransID(std::string& strTransID)
{
    (void)VOS_MutexLock(m_pRetriveStatusMapLock);
    (void)m_ChannelListRetriveStatusMap.erase(strTransID);
    (void)VOS_MutexUnlock(m_pRetriveStatusMapLock);
}

IVS_INT32 CSDKInterface::GetChannelListRetriveStatusByTransID(std::string& strTransID, IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus)
{
    (void)VOS_MutexLock(m_pRetriveStatusMapLock);
    std::map<std::string, IVS_CHANNLELIST_RETRIVE_STATUS_INFO>::iterator itor = m_ChannelListRetriveStatusMap.find(strTransID);
    if (m_ChannelListRetriveStatusMap.end() == itor)
    {
        (void)VOS_MutexUnlock(m_pRetriveStatusMapLock);
        return IVS_FAIL;
    }
    channelListRetriveStatus = itor->second;
    (void)VOS_MutexUnlock(m_pRetriveStatusMapLock);
    return IVS_SUCCEED;
}