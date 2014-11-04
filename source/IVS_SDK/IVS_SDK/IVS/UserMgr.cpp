#include "UserMgr.h"
#include "Cmd.h"
#include "NSSOperator.h"
#include "LoginXMLProcess.h"
#include "MediaXMLProcess.h"
#include "MotionDetection.h"
#include "IVS_Trace.h"
#include "nss_xml_msg.h"

CUserMgr::CUserMgr(void)
{
	m_UserInfoMgr.SetUserMgr(this);
	m_RealPlayMgr.SetUserMgr(this);
	m_RecordService.SetUserMgr(this);
	m_PlaybackMgr.SetUserMgr(this);
    m_DeviceMgr.SetUserMgr(this);
	m_LogMgr.SetUserMgr(this);
    m_loginObj.SetUserMgr(this);
	m_AlarmLevelMgr.SetUserMgr(this);
	m_AlarmHelpMgr.SetUserMgr(this);
    m_DeviceGroupMgr.SetUserMgr(this);
    m_RecordPlanMgr.SetUserMgr(this);
	m_DeviceParaConfig.SetUserMgr(this);
	m_RecordList.SetUserMgr(this);
	m_AlarmMgr.SetUserMgr(this);
	m_AlarmAreaMgr.SetUserMgr(this);
	m_UserGroupMgr.SetUserMgr(this);
    m_RoleMgr.SetUserMgr(this);
	m_AlarmLinkageMgr.SetUserMgr(this);
	m_DomainRouteMgr.SetUserMgr(this);
	m_PtzControl.SetUserMgr(this);
    m_RecordBookMark.SetUserMgr(this);
	m_RealPlayBookMark.SetUserMgr(this);
    m_UserDataMgr.SetUserMgr(this);
	m_MotionDetection.SetUserMgr(this);
	m_CloseLinkIDCount.SetUserMgr(this);
	m_Relink.SetUserMgr(this);
	m_TVWallMgr.SetUserMgr(this);
	m_RecordBackupMgr.SetUserMgr(this);
	m_CapturePicture.SetUserMgr(this);
	m_ClientNetworkMgr.SetUserMgr(this);
	m_DownLoadMgr.SetUserMgr(this);
    m_AudioMgr.SetUserMgr(this);
	m_TPMgr.SetUserMgr(this);
    m_ReportMgr.SetUserMgr(this);
    m_IntelligenceAnalysisMgr.SetUserMgr(this);
    m_UpgradeMgr.SetUserMgr(this);

	m_fnEventCallBack = NULL;
	m_pUserParam = NULL;
	m_pSingleSvrProxy = NULL;
    m_strDomainCode = LOCAL_DOMAIN_CODE_INIT;
	m_bAutoResume = false;
	m_bCloseLinkIDCount = false;
	m_bRelink = false;
	m_bReceiveOMUAlarm = 1;
	m_ResumeStartTime = 0;

    m_mapWaterMarkInfo.clear();
    m_pWaterMarkInfoMutex = VOS_CreateMutex();
}

CUserMgr::~CUserMgr(void)
{
    m_pUserParam = NULL;
    m_pSingleSvrProxy = NULL;

    if (NULL != m_pWaterMarkInfoMutex)
    {
        (void)VOS_DestroyMutex(m_pWaterMarkInfoMutex);
        m_pWaterMarkInfoMutex = NULL;
    }

	if (m_bAutoResume)
	{
		try
		{
			StopResumeTimer();
		}
		catch (...)
		{
            BP_RUN_LOG_ERR(IVS_FAIL,"Stop Resume Timer ","throw exception.");//lint !e1551
		}
	}
}

// 设置SDKInterface单实例对象;
void CUserMgr::SetSingleSvrProxy(void *pSingleSvrProxy)
{
	if(NULL != pSingleSvrProxy)
	{
		m_pSingleSvrProxy = pSingleSvrProxy;
	}
}

// 用户登录
IVS_INT32 CUserMgr::Login(const IVS_LOGIN_INFO* pLoginReqInfo)
{	
    IVS_DEBUG_TRACE("");
    if (NULL == pLoginReqInfo)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Login", "pLoginReqInfo = NULL");
        return IVS_PARA_INVALID;
    }
	int iRet = m_loginObj.Login(pLoginReqInfo);

	if (IVS_SUCCEED == iRet || IVS_SMU_USER_FIRST_LOGIN == iRet  || IVS_SMU_USER_PWD_NEED_MOD == iRet)
	{
		// 处理告警服务
		//m_alarmMgr.StartAlarmReporter();
		// 开启自动恢复定时器
		StartResumeTimer();

		//modify by zwx211831:通过GetDomainRoute遍历域，更新LinkID的Map
		IVS_UINT32 uiBufSize = sizeof(IVS_DOMAIN_ROUTE_LIST) + 127 * sizeof(IVS_DOMAIN_ROUTE);
		IVS_DOMAIN_ROUTE_LIST * pDomainRouteList = (IVS_DOMAIN_ROUTE_LIST *)(new IVS_CHAR[uiBufSize]);
		CHECK_POINTER(pDomainRouteList, IVS_OPERATE_MEMORY_ERROR);

		memset(pDomainRouteList, 0, uiBufSize);
		IVS_INT32 iReturn = GetDomainRouteMgr().GetDomainRoute(pDomainRouteList, uiBufSize);
		delete []pDomainRouteList;
		pDomainRouteList = NULL;
		if (IVS_SUCCEED != iReturn)
		{
			return iReturn;
		}
		//end
	}

	return iRet;
}


//用户单点登录;
IVS_INT32 CUserMgr::LoginByTicket(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx)
{
	IVS_DEBUG_TRACE("");
	if (NULL == pLoginReqInfoEx)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Login", "pLoginReqInfo = NULL");
		return IVS_PARA_INVALID;
	}
	int iRet = m_loginObj.LoginByTicket(pLoginReqInfoEx);

	if (IVS_SUCCEED == iRet || IVS_SMU_USER_FIRST_LOGIN == iRet  || IVS_SMU_USER_PWD_NEED_MOD == iRet)
	{
		// 开启自动恢复定时器;
		StartResumeTimer();

		//通过GetDomainRoute遍历域，更新LinkID的Map;
		IVS_UINT32 uiBufSize = sizeof(IVS_DOMAIN_ROUTE_LIST) + 127 * sizeof(IVS_DOMAIN_ROUTE);
		IVS_DOMAIN_ROUTE_LIST * pDomainRouteList = (IVS_DOMAIN_ROUTE_LIST *)(new IVS_CHAR[uiBufSize]);
		CHECK_POINTER(pDomainRouteList, IVS_OPERATE_MEMORY_ERROR);

		memset(pDomainRouteList, 0, uiBufSize);
		IVS_INT32 iReturn = GetDomainRouteMgr().GetDomainRoute(pDomainRouteList, uiBufSize);
		delete []pDomainRouteList;
		pDomainRouteList = NULL;
		if (IVS_SUCCEED != iReturn)
		{
			return iReturn;
		}
	}

	return iRet;
}

// 获取登录成功返回的loginId
char* CUserMgr::GetLoginId()
{
    return m_loginObj.GetLoginId();
}

// 获取UserID
IVS_UINT32 CUserMgr::GetUserID() const
{
    return m_loginObj.GetUserID();
}

// 连接建立成功通知用户
void CUserMgr::LinkOpenSuccessNotify(const std::string &strLinkId)
{
    IVS_DEBUG_TRACE("LinkId:%s", strLinkId.c_str());
    CLinkInfo oLinkInfoTemp;
    if (!CLinkRouteMgr::instance().FindLinkIDInfo(strLinkId, oLinkInfoTemp))
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Find linkID info from link route mgr ","failed");
        return;
    }

    //新建的连接，post信号量，状态置为连接中
    CLinkRouteMgr::instance().SetLinkIDStatus(strLinkId, LINKID_STATUS_OPENED);
}//lint !e1762 该函数不能设置为const，因为SetLinkIDStatus改变值.

// 发送NSS请求给登陆的SMU,连接建立成功通知用户
BP::CCmd* CUserMgr::BuildSMUCmd(unsigned short usReqID,const char* pData)
{
    CHECK_POINTER(pData, NULL);
    IVS_DEBUG_TRACE("ReqID:%d, Data:%s", (int)usReqID, pData);
    std::string strSmuLinkID = "";
    GetSMULinkID(strSmuLinkID);
    return CNSSOperator::instance().BuildSMUCmd(usReqID, pData, strSmuLinkID);
}

// 获取登陆到smu时的linkId
void CUserMgr::GetSMULinkID( std::string& strSmuLinkID)
{
    IVS_DEBUG_TRACE("SmuLinkID:%s", strSmuLinkID.c_str());

    //获取登陆SMU的linkID
    CLinkInfo oLinkInfoResult;
    bool bGetLinkRet = CLinkRouteMgr::instance().GetLinkIDInfoByDomainCode(m_iSessionID, NET_ELE_SMU_NSS, m_strDomainCode, oLinkInfoResult);
    if (!bGetLinkRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get LinkID Info By Domain Code ", "failed.");
        return;
    }

    //判断连接状态，TODO 改为枚举。2为连接 3为断开
    if (LINKID_STATUS_OPENED == oLinkInfoResult.GetLinkStatus())
    {
        strSmuLinkID = oLinkInfoResult.GetStrLinkId();
    }

    return;
}

// 获取指定域的smulinkId
IVS_INT32 CUserMgr::GetSMULinkID(std::string& strSmuLinkID, const std::string& strDomainCode)
{
	IVS_DEBUG_TRACE("DomainCode:%s", strDomainCode.c_str());

	//add by zwx211831增加对域编码有效性的判断
	IVS_INT32 iRet = CLinkRouteMgr::instance().DecideDomainCode(m_iSessionID, NET_ELE_SMU_NSS, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start Search Device failed", "DomainCode is invalid");
		return iRet;
	}

	//获取指定域SMU的linkID
	CLinkInfo oLinkInfoResult;
	bool bGetLinkRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(m_iSessionID, strDomainCode, oLinkInfoResult);
	if (!bGetLinkRet)
	{
		// 创建连接;
		std::string strGeneralSmuLinkID = "";
		iRet = m_loginObj.GeneralLoginSmu(strDomainCode, strGeneralSmuLinkID);
		if (IVS_SUCCEED != iRet && IVS_SMU_USER_FIRST_LOGIN != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get LinkID Info By Domain Code","LoginSmu failed");
			return iRet;
		}
		strSmuLinkID = strGeneralSmuLinkID;
		if (0 == strSmuLinkID.length())
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Get SMU LinkID failed", "LinkID is NULL");
			return IVS_FAIL;
		}
		return iRet;
	}

	//add by zwx211831	如果状态为可用但未连接，则连接
	if (LINKID_STATUS_ENABLED == oLinkInfoResult.GetLinkStatus())
	{
		ProcrssEnableLinkNode(oLinkInfoResult);
	}

	//判断连接状态，TODO 改为枚举。1为连接 2为断开
	if (LINKID_STATUS_OPENED == oLinkInfoResult.GetLinkStatus())
	{
		strSmuLinkID = oLinkInfoResult.GetStrLinkId();
	}

	if (0 == strSmuLinkID.length())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get SMU LinkID failed", "LinkID is NULL");
		return IVS_FAIL;
	}

	return iRet;
}

// 根据网元类型获取本域linkID(SMU和OMU使用)
IVS_INT32 CUserMgr::GetLocalDomainLinkID(IVS_INT32 iNetElementType, std::string& strLocalDomainLink)
{
    IVS_DEBUG_TRACE("iNetElementType:%d", iNetElementType);
    //获取登录域的linkID
    CLinkInfo oLinkInfoResult;
    bool bGetLinkRet = CLinkRouteMgr::instance().GetLinkIDInfoByDomainCode(m_iSessionID, iNetElementType, m_strDomainCode, oLinkInfoResult);
    if (!bGetLinkRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get LinkID Info By Domain Code ", "failed."); // 正常逻辑不会进入此分支，除非登录获取域路由失败
        return IVS_FAIL;
    }

    //判断连接状态
    if (LINKID_STATUS_OPENED != oLinkInfoResult.GetLinkStatus())
    {
        BP_RUN_LOG_ERR(IVS_NET_CONNECT_ERROR, "Get LocalDomain LinkID ", "LinkStatus isn't open"); // 正常逻辑不会进入此分支，除非登录获取域路由失败
        return IVS_NET_CONNECT_ERROR;
    }
    strLocalDomainLink = oLinkInfoResult.GetStrLinkId();
    BP_DBG_LOG("Get LocalDomain LinkID, strLocalDomainLink: %s", strLocalDomainLink.c_str());
    return IVS_SUCCEED;
}

// 用户注销
IVS_INT32 CUserMgr::Logout(bool bSendLogoutCmd)
{
	IVS_DEBUG_TRACE("SendLogoutCmd:%d",(int)bSendLogoutCmd);
	int iRet = IVS_FAIL;

	// 去掉定时器
	if (m_bAutoResume)
	{
		StopResumeTimer();
	}

	// 域路由清理
    m_DomainRouteMgr.ClearDomainRouteMap();
    m_DeviceMgr.Clear();
	iRet = m_loginObj.Logout(bSendLogoutCmd);
    
	// 将该用户的link路由从路由表中删除;
	CLinkRouteMgr::instance().DeleteLinkList(m_iSessionID);
    
    m_strDomainCode = LOCAL_DOMAIN_CODE_INIT;    // 放在最后清理，防止之前业务有使用m_strDomainCode
    m_iSessionID = SDK_SESSIONID_INIT;
    m_strTransID = "";
    m_strServerCode = "";
	return iRet;
}

// 获取SDK保存的用户信息，用于其它业务
CUserInfo* CUserMgr::GetUserInfoEx()
{
	return m_loginObj.GetUserInfo();
}

// 根据Rtsp句柄获取播放句柄
IVS_ULONG CUserMgr::GetPlayHandlebyRtspHandle(int iRtspHandle)
{
    IVS_DEBUG_TRACE("RtspHandle:%d", iRtspHandle);
    IVS_ULONG ulPlayHandle = m_RealPlayMgr.GetPlayHandlebyRtspHandle(iRtspHandle);
    if (0 != ulPlayHandle)
    {
        BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from real play mgr, playhandle=%u", ulPlayHandle);
        return ulPlayHandle;
    }

    ulPlayHandle = m_PlaybackMgr.GetPlayHandlebyRtspHandle(iRtspHandle);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from play back mgr, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}

	ulPlayHandle = m_DownLoadMgr.GetDownloadHandlebyRtspHandle(iRtspHandle);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from DownLoad mgr, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}

	ulPlayHandle = m_TPMgr.GetPlayHandlebyRtspHandle(iRtspHandle);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from tp mgr, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}
	ulPlayHandle = m_TVWallMgr.GetTVWallHandlebyRtspHandle((IVS_ULONG)iRtspHandle);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from TVWall mgr, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}

	ulPlayHandle = m_AudioMgr.GetTalkBackPlayHandleByRtspHandle((IVS_ULONG)iRtspHandle);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from Audio mgr TalkBack, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}

	ulPlayHandle = m_AudioMgr.GetBroadCastPlayHandleByRtspHandle((IVS_ULONG)iRtspHandle);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from Audio mgr BroadCast, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}

    //BP_DBG_LOG("Get Play Handle by Rtsp Handle succeed from play back mgr, playhandle=%u", ulPlayHandle);
    return ulPlayHandle;
}

// 根据播放通道号获取播放句柄
IVS_ULONG CUserMgr::GetPlayHandlebyPlayChannel(unsigned long ulPlayChannel)
{
    IVS_DEBUG_TRACE("PlayChannel:%lu", ulPlayChannel);
	IVS_ULONG ulPlayHandle = m_RealPlayMgr.GetPlayHandleByPlayChannel(ulPlayChannel);
    if (0 != ulPlayHandle)
    {
        BP_DBG_LOG("Get Play Handle by Play Channel succeed from real play mgr, playhandle=%u", ulPlayHandle);
        return ulPlayHandle;
    }

    ulPlayHandle = m_PlaybackMgr.GetPlayHandleByPlayChannel(ulPlayChannel);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Play Channel succeed from real play back mgr, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}

	ulPlayHandle = m_DownLoadMgr.GetDownloadHandleByPlayChannel(ulPlayChannel);
	if (0 != ulPlayHandle)
	{
		BP_DBG_LOG("Get Play Handle by Play Channel succeed from real DownLoad mgr, playhandle=%u", ulPlayHandle);
		return ulPlayHandle;
	}
	
    //BP_DBG_LOG("Get Play Handle by Play Channel succeed from play back mgr, playhandle=%u", ulPlayHandle);
    return ulPlayHandle;
}

// 根据NetSource的通道号获取播放句柄
IVS_ULONG CUserMgr::GetPlayHandlebyNet(unsigned long ulNetChannel)
{
    IVS_DEBUG_TRACE("NetChannel:%lu", ulNetChannel);
	IVS_ULONG ulPlayHandle = m_RealPlayMgr.GetPlayHandleByNetSourceChannel(ulNetChannel);
    if (0 != ulPlayHandle)
    {
        BP_DBG_LOG("Get Play Handle by Net Source Channel succeed from real play mgr, playhandle=%u", ulPlayHandle);
        return ulPlayHandle;
    }

    ulPlayHandle = m_PlaybackMgr.GetPlayHandleByNetSourceChannel(ulNetChannel);
    BP_DBG_LOG("Get Play Handle by Net Source Channel succeed from play back mgr, playhandle=%u", ulPlayHandle);

    ulPlayHandle = m_AudioMgr.GetTalkBackPlayHandleByNetHandle((IVS_ULONG)ulNetChannel);
    if (0 != ulPlayHandle)
    {
        BP_DBG_LOG("Get Play Handle by net Handle succeed from Audio mgr TalkBack, playhandle=%u", ulPlayHandle);
        return ulPlayHandle;
    }

    ulPlayHandle = m_AudioMgr.GetBroadCastPlayHandleByNetHandle((IVS_ULONG)ulNetChannel);
    if (0 != ulPlayHandle)
    {
        BP_DBG_LOG("Get Play Handle by net Handle succeed from Audio mgr BroadCast, playhandle=%u", ulPlayHandle);
        return ulPlayHandle;
    }

    return ulPlayHandle;
}

// 根据播放句柄获取服务类型
IVS_INT32 CUserMgr::GetServiceTypebyPlayHandle(IVS_ULONG ulPlayHandle, IVS_SERVICE_TYPE& enServiceType)
{
    IVS_DEBUG_TRACE("PlayHandle:%lu", ulPlayHandle);
    IVS_INT32 iRet = IVS_SUCCEED;
    CMediaBase* pRealPlay = m_RealPlayMgr.GetMediaBase(ulPlayHandle);
    if (NULL != pRealPlay)
    {
        enServiceType = SERVICE_TYPE_REALVIDEO;
        BP_DBG_LOG("Get Service Type by PlayHandle succeed, ServiceType[%d]",enServiceType);
        return iRet;
    }

    CPlayback* pPlayBack = m_PlaybackMgr.GetPlayBack(ulPlayHandle);
    if (NULL != pPlayBack)
    {
        enServiceType = pPlayBack->GetServiceType();
        BP_DBG_LOG("Get Service Type by PlayHandle succeed, ServiceType[%d]",enServiceType);
        return iRet;
    }

	// add by w00210470
	CDownload *pDownload = m_DownLoadMgr.GetDownLoad(ulPlayHandle);
	if (NULL != pDownload)
	{
		enServiceType = pDownload->GetServiceType();
		BP_DBG_LOG("Get Service Type by PlayHandle succeed, ServiceType[%d]", enServiceType);
        return IVS_SUCCEED;
	}

	CDecoderChannel* pChannel = m_TVWallMgr.GetChannel(ulPlayHandle);
	if (NULL != pChannel)
	{
		enServiceType = SERVICE_TYPE_REALVIDEO;
		BP_DBG_LOG("Get Service Type by PlayHandle succeed In TVWall, ServiceType[%d]",enServiceType);
		return iRet;
	}

	CAudioObj *pAudioObj = m_AudioMgr.GetAudioObj(ulPlayHandle);
	if (NULL != pAudioObj)
	{
		enServiceType = pAudioObj->GetServiceType();
		BP_DBG_LOG("Get Service Type by PlayHandle succeed, ServiceType[%d]", enServiceType);
		return IVS_SUCCEED;
	}
	
	bool bIsBroadCast = m_AudioMgr.IsBroadCast(ulPlayHandle);
	if (bIsBroadCast)
	{
		enServiceType = SERVICE_TYPE_AUDIO_BROADCAST;
		BP_DBG_LOG("Get Service Type by PlayHandle succeed, ServiceType[%d]", enServiceType);
		return IVS_SUCCEED;
	}

    BP_RUN_LOG_ERR(iRet, "Get Service Type by Play Handle", "Play handle error, ulPlayHandle = %lu", ulPlayHandle);
    return IVS_FAIL;
}


//解析SDP信息，获取码流是否采用加密的算法
IVS_INT32 CUserMgr::SetPlaySecretKeybyRtspHandle(int iRtspHandle,const char* pSdp)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;
	if (NULL == pSdp)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Get Secret Key ","SDP Info is NULL");
		return iRet;
	}
	//解析SDP消息
	bool bUseSecretKey = false;		//是否使用了视频加密
	MEDIA_INFO_SDP stMediaInfo;
	STORAGESECRET_INFO stStorageSecretInfo;
	MEDIA_KEY_INFO stMediaKeyInfo;
	memset(&stMediaInfo, 0x0, sizeof(MEDIA_INFO_SDP));
	memset(&stStorageSecretInfo,0x0,sizeof(STORAGESECRET_INFO));
	memset(&stMediaKeyInfo,0x0,sizeof(MEDIA_KEY_INFO));
	stMediaInfo.ManuType = 101;

	/*
	 * init payload type, add by w00210470
	 */
	stMediaKeyInfo.uPTAudio = PAY_LOAD_TYPE_PCMA;
	stMediaKeyInfo.uPTVideo = PAY_LOAD_TYPE_H264;
	
	stMediaKeyInfo.iAudioKeyType = HW_NO_DEC_TYPE;
	stMediaKeyInfo.iVideoKeyType = HW_NO_DEC_TYPE;
	stMediaKeyInfo.enVideoDecoderType = VIDEO_DEC_H264;//默认初始化视频类型
	stMediaKeyInfo.enAudioDecoderType = AUDIO_DEC_G711U;//默认初始化音频格式
	iRet = parseSdp(pSdp, &stMediaKeyInfo, &stMediaInfo, &stStorageSecretInfo);
	if(IVS_SUCCEED == iRet)
	{
		//目前解析SDP只会告知是否使用了加密算法，不存在密钥
		if ((HW_NO_DEC_TYPE != stMediaKeyInfo.iAudioKeyType && HW_NO_DEC_TYPE != stMediaKeyInfo.iVideoKeyType)&& 
			stMediaKeyInfo.iAudioKeyType == stMediaKeyInfo.iVideoKeyType)
		{
            BP_DBG_LOG("Need Encry. stMediaKeyInfo.iVideoKeyType : %d", stMediaKeyInfo.iVideoKeyType);
			bUseSecretKey = true;
		}
        else
        {
            BP_DBG_LOG("No Need Encry. iVideoKeyType:%d, iAudioKeyType:%d", 
                       stMediaKeyInfo.iVideoKeyType, stMediaKeyInfo.iAudioKeyType);
        }
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Parse Sdp Info","Parse Sdp Info Fail");
		return IVS_FAIL;
	}
	//获取当前播放类型，并设置加密信息
	IVS_ULONG ulMediaHandle = 0;
	CMediaBase* pMediaBase = NULL;
	unsigned int uiPort = 0;
	//通过RtspHandle获取MediaBase的类，尝试实况
	ulMediaHandle = m_RealPlayMgr.GetPlayHandlebyRtspHandle(iRtspHandle);
	if (0 != ulMediaHandle)
	{
		pMediaBase = m_RealPlayMgr.GetMediaBase(ulMediaHandle);
		uiPort = m_RealPlayMgr.GetPlayerChannelByHandle(ulMediaHandle);

        if (NULL == pMediaBase)
        {
            BP_RUN_LOG_ERR(IVS_FAIL,"Set Secret Key ","Set Secret Key Error,media Info is NULL");
            return IVS_FAIL;
        }

        //设置发送给前端设备的SessionInfo
        BP_DBG_LOG("SessionUrl:%s, SessionId:%s", stMediaInfo.SessionUrl, stMediaInfo.SessionId);
        pMediaBase->SetSessionInfo(stMediaInfo.SessionUrl, stMediaInfo.SessionId);
	}
	//回放
	ulMediaHandle = m_PlaybackMgr.GetPlayHandlebyRtspHandle(iRtspHandle);
	if (0 != ulMediaHandle)
	{
		pMediaBase = m_PlaybackMgr.GetMediaBase(ulMediaHandle);
		uiPort = m_PlaybackMgr.GetPlayerChannelByHandle(ulMediaHandle);
	}
	//下载
	ulMediaHandle = m_DownLoadMgr.GetDownloadHandlebyRtspHandle(iRtspHandle);
	if (0 != ulMediaHandle)
	{
		pMediaBase = m_DownLoadMgr.GetDownLoad(ulMediaHandle);
        if (NULL == pMediaBase)
        {
            BP_RUN_LOG_ERR(IVS_FAIL,"Set Secret Key ","download mgr,media base is NULL");
            return IVS_FAIL;
        }
		uiPort = pMediaBase->GetPlayerChannel();
	}

	if (NULL == pMediaBase)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Set Secret Key ","Set Secret Key Error,media Info is NULL");
		return IVS_FAIL;
	}

    /*
	* set rtp payload type, add by w00210470
	*/
	MEDIA_ATTR AttrAudio = {0};//lint !e522 !e10
	AttrAudio.rtpmap = stMediaKeyInfo.uPTAudio;//lint !e1013 !e10 !e63 !e734
	AttrAudio.type   = stMediaKeyInfo.enAudioDecoderType;//lint !e1013 !e10 !e63
	AttrAudio.samplerate = 8000;//lint !e1013 !e10 !e63
	MEDIA_ATTR AttrVideo = {0};//lint !e522 !e10
	AttrVideo.rtpmap = stMediaKeyInfo.uPTVideo;//lint !e1013 !e10 !e63 !e734
	AttrVideo.type   = stMediaKeyInfo.enVideoDecoderType;//lint !e1013 !e10 !e63
	AttrVideo.samplerate = 90000;//lint !e1013 !e10 !e63
	pMediaBase->SetMeidaAttribute(AttrVideo, AttrAudio);

	iRet = IVS_SUCCEED;
	//内部实现密钥的交换以及获取到明文密钥
	if(bUseSecretKey)
	{
		//BP_DBG_LOG("Need Secret Key.");
		iRet = pMediaBase->SetSecretKey(uiPort, stMediaKeyInfo);
        if (IVS_SUCCEED != iRet)
        {
            BP_DBG_LOG("Set Secret Key Is Fail.");
            pMediaBase->SetSecretFlag(false);
            return iRet;
        }
        BP_DBG_LOG("Set Secret Key Is Success.");
        pMediaBase->SetSecretFlag(true);
	}
    else
    {
        BP_DBG_LOG("No Need Secret Key.");
        iRet = pMediaBase->SetNoNeedSecretKey(uiPort, stMediaKeyInfo);
        if (IVS_SUCCEED != iRet)
        {
            BP_DBG_LOG("Set No Need Secret Key Is Fail.");
            pMediaBase->SetSecretFlag(false);
            return iRet;
        }
        BP_DBG_LOG("Set Secret Key Is Success.");
        pMediaBase->SetSecretFlag(true);
    }

    //如果密钥交换失败不能直接调用StopPlay，会造成崩溃
	//if(IVS_SUCCEED != iRet)
	//{
    //  BP_RUN_LOG_ERR(IVS_FAIL,"Set Secret Key ","Set Secret Key Fail");
	//	pMediaBase->StopPlay();		
	//}
	return iRet;
}

IVS_INT32 CUserMgr::GetSameSuperDomainCode(const std::string& strDomainCode1,const std::string& strDomainCode2,
																std::string& strSuperDomainCode)
{
	return m_DomainRouteMgr.GetSameSuperDomainCode(strDomainCode1,strDomainCode2,strSuperDomainCode);
}

bool CUserMgr::IsInDomainTree(const std::string& strDomainCode)
{
	return m_DomainRouteMgr.IsInDomainTree(strDomainCode);
}


// 向指定服务器发送命令
IVS_INT32 CUserMgr::SendCmd(CSendNssMsgInfo& sendNssMsgInfo,std::string &strNSSRsp,IVS_INT32& iNeedRedirect, uint32_t uiTimeout, IVS_BOOL bTimeout)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    // 如果域编码为空，直接发送，不考虑多级多域、重定向等
	std::string strDomainCode = sendNssMsgInfo.GetDomainCode();
	//std::string strCameraCode = sendNssMsgInfo.GetCameraCode();
	CLinkInfo oLinkIdInfo;
	/**************************************
	**如果域编码为空，表示当前不需要重定向服务
	***************************************/
	if (strDomainCode.empty())
	{
		if (!CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(m_iSessionID, m_strDomainCode, oLinkIdInfo))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"Send Cmd","strDomainCode is empty and GetSmuLinkIDInfoByDomainCode failed");
		    return IVS_FAIL;
		}
        // 判断连接状态，如果处于关闭状态，直接返回失败
        if (LINKID_STATUS_OPENED != oLinkIdInfo.GetLinkStatus())
        {
            BP_RUN_LOG_ERR(iRet,"Send Cmd","Link is closed");
            return IVS_NET_CONNECT_ERROR;
        }
		iRet = SendSynCmd(sendNssMsgInfo,oLinkIdInfo.GetStrLinkId(),strNSSRsp, uiTimeout, bTimeout);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Send Cmd","SendSynCmd failed");
		}
		return iRet;
	}
	/***************************************
	**以下服务需要代理，多域特性CameraCode可为空
	****************************************/

    //根据域编码查找目标域和代理域映射表，查找代理域
    std::string strSendDomainCode;
    bool bRet = m_DomainRouteMgr.FindProxyDomainCode(strDomainCode, strSendDomainCode);
    // 如果未找到代理域，使用目标域代替;
    if (!bRet)
    {
        strSendDomainCode = strDomainCode;
    }
    // 根据域编码查找LinkRouteMgr找SMU的LinkID
    bool bGetLinkRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(m_iSessionID, strSendDomainCode, oLinkIdInfo);// 此处调用查找LinkRouteMgr的函数； 
    if (!bGetLinkRet)
    {
        BP_RUN_LOG_ERR(bGetLinkRet, "send cmd,", "domain code:[%s] is not exist in route list cache.", strSendDomainCode.c_str());
    }

    if (LINKID_STATUS_ENABLED == oLinkIdInfo.GetLinkStatus())   // 如果处理成功，oLinkIdInfo会设置成 LINKID_STATUS_OPENED
    {
        iRet = ProcrssEnableLinkNode(oLinkIdInfo);
    }

    if (LINKID_STATUS_OPENED == oLinkIdInfo.GetLinkStatus())  // 可用的链接已存在，可以直接发送消息
    { 
        // 发送请求; 
		iRet = SendSynCmd(sendNssMsgInfo,oLinkIdInfo.GetStrLinkId(),strNSSRsp, uiTimeout, bTimeout);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Send Cmd","ON LinkStatus Send Cmd Error");
		}
		return iRet;
    }
    else
    {
        iRet = IVS_NET_CONNECT_ERROR;
    }

	iNeedRedirect = IVS_SUCCEED;
	return iRet;
}

IVS_INT32 CUserMgr::ProcrssEnableLinkNode(CLinkInfo& oLinkIdInfo)
{
    std::string strGeneralSmuLinkID;
    int iRet = m_loginObj.GeneralLoginSmu(oLinkIdInfo.GetStrDomainCode(), strGeneralSmuLinkID);
    if (IVS_SUCCEED == iRet || IVS_SMU_USER_FIRST_LOGIN == iRet)
    {
        // 成功登陆，设置RoutLink状态为Open
        oLinkIdInfo.SetLinkStatus(LINKID_STATUS_OPENED);
        oLinkIdInfo.SetStrLinkId(strGeneralSmuLinkID);
        CLinkRouteMgr::instance().UpdateLinkInfo(m_iSessionID, oLinkIdInfo);
    }
    else
    {
        // 登陆失败，放到重连链表中，并设置RouteLink状态为Closed
        oLinkIdInfo.SetLinkStatus(LINKID_STATUS_CLOSED);
        (void)CLinkRouteMgr::instance().UpdateLinkInfo(m_iSessionID, oLinkIdInfo);

        IVS_DOMAIN_ROUTE stDomainRouteInfo;
        memset(&stDomainRouteInfo, 0, sizeof(IVS_DOMAIN_ROUTE));
        iRet = GetDomainRouteMgr().GetDomainRoutebyCode(oLinkIdInfo.GetStrDomainCode(), stDomainRouteInfo);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(IVS_SDK_ERR_DOMAINCODE_NOT_EXIST, "General Login Smu", " failed");
            return IVS_SDK_ERR_DOMAINCODE_NOT_EXIST;
        }

        AddLinkIdToBusinessResumeList(strGeneralSmuLinkID, NET_ELE_SMU_NSS, stDomainRouteInfo.stIP.cIP, DEFAULT_SMU_PORT, TYPE_ADD_RELINK);

        iRet = IVS_NET_CONNECT_ERROR;
    }
    return iRet;
}

IVS_INT32 CUserMgr::SendRedirectServe(CSendNssMsgInfo& sendNssMsgInfo,std::string &strNSSRsp)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;
	// 如果域编码为空，直接发送，不考虑多级多域、重定向等
	std::string strDomainCode = sendNssMsgInfo.GetDomainCode();
	std::string strCameraCode = sendNssMsgInfo.GetCameraCode();

	//根据域编码查找目标域和代理域映射表，查找代理域
	std::string strSendDomainCode;
	bool bRet = m_DomainRouteMgr.FindProxyDomainCode(strDomainCode, strSendDomainCode);
	// 如果未找到代理域，使用目标域代替;
	if (!bRet)
	{
		strSendDomainCode = strDomainCode;
	}
	//以下过程准备进行重定向
	IVS_DOMAIN_ROUTE stDomainRoute;
	memset(&stDomainRoute, 0, sizeof(IVS_DOMAIN_ROUTE));
	//获取域路由信息
	iRet = m_DomainRouteMgr.GetDomainRoutebyCode(strSendDomainCode, stDomainRoute);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Send Cmd", "Get DoMain Code Error");
		return iRet;
	}
	// 如果该域是代理，直接返回失败;
	if (stDomainRoute.bIsAgent)
	{
		BP_RUN_LOG_ERR(iRet, "Send Cmd", "This Route is ProxyRoute");
		return IVS_FAIL;
	}
	// 根据请求ID查找请求映射表， 判断该请求是否需要重定向
	bRet = CToolsHelp::IsNeedReDirect(sendNssMsgInfo.GetReqID());
	if (!bRet)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Send Cmd", "Current ReqID with no need for ReDirect");
		return IVS_FAIL;
	}
	// 根据摄像机编码找到对应的NvrCode;
	if (strCameraCode.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Send Cmd","CameraCode Empty");
		return IVS_FAIL;
	}
	IVS_CAMERA_BRIEF_INFO stCameraBriefInfo;
	memset(&stCameraBriefInfo, 0, sizeof(IVS_CAMERA_BRIEF_INFO));
	//获取到摄像机对应的nvr code
	iRet = m_DeviceMgr.GetCameraBriefInfobyCode(strCameraCode, stCameraBriefInfo);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Send Cmd", "Can not find Camera Info,strCameraCode = %s", strCameraCode.c_str());
		return iRet;
	}

    IVS_CHAR szNvrCode[IVS_NVR_CODE_LEN + 1] = {0};
	if(!CToolsHelp::Memcpy(szNvrCode,IVS_NVR_CODE_LEN,stCameraBriefInfo.cNvrCode,IVS_NVR_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Send Cmd", "cNvrCode Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	std::string strNvrCode = szNvrCode;

	// 根据域编码和NVRCode查找LinkRouteMgr找SCU的LinkID
	//如果成功，说明已经发起送了服务
	iRet = SendCmdbyDomainNvrCode(strDomainCode, strNvrCode, sendNssMsgInfo, strNSSRsp);
	// 如果发送成功，说明SMU挂了，摄像机未发生迁移，还在登录时的那台NVR上;
	if (IVS_SUCCEED == iRet)
	{
		BP_DBG_LOG("Send Cmd by Domain Nvr Code succeed. smu going down, camera do not transfer to other nvr.");
		return iRet;
	}
	// TODO错误码为1、连接失败 2、发送失败 3、IPC不在该NVR上，再发起重定向流程;
// 	if (IVS_NET_CONNECT_ERROR == iRet || IVS_NET_RECV_TIMEOUT == iRet 
// 		|| IVS_SCU_REDIRECTION_ERROR == iRet || IVS_SCU_DEV_NOT_IN_WHITE_LIST == iRet)
// 	{
// 		//以下完成重定向的功能，即集群或者堆叠模式下的重定向
// 		iRet = SendCmdbyReDirect(stDomainRoute, sendNssMsgInfo, strNSSRsp);
// 	}
	// 只要失败情况下都都进行重定向,可能有重启SCU鉴权不通过的等情况发生，另外如果失败了，重定向可能还是再原来服务器上，这样
	// 可能发生再试一次的情况，这种情况也是被允许的 mod by z00193167
	//以下完成重定向的功能，即集群或者堆叠模式下的重定向
	iRet = SendCmdbyReDirect(stDomainRoute, sendNssMsgInfo, strNSSRsp);

	return iRet;
}

// 发送异步消息
IVS_INT32 CUserMgr::SendAsynMsg(const IVS_CHAR* pTargetDomainCode, 
                                                                IVS_UINT32 uiReqID, 
                                                                const std::string& strTransID,
                                                                CXml* pSendXm)
{
    CHECK_POINTER(pTargetDomainCode, IVS_PARA_INVALID);

    BP_RUN_LOG_INF("send asyn message", "target domain code:%s, reqid:%u", pTargetDomainCode, uiReqID);

    // 检测当前是否存在目标域的链接
    CLinkInfo linkInfo;
    bool bGetLinkRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(m_iSessionID, pTargetDomainCode, linkInfo);
    if (!bGetLinkRet)
    {
        // 只有外域，才存在CLinkRouteMgr中没记录的场景
        BP_RUN_LOG_INF("send asyn message", "fail to find domain code:%s in link route manager", pTargetDomainCode);
        //std::string strGeneralSmuLinkID;
        //int iCreateLinkRet = m_loginObj.GeneralLoginSmu(pTargetDomainCode, strGeneralSmuLinkID);
        //if (IVS_SUCCEED != iCreateLinkRet &&IVS_SMU_USER_FIRST_LOGIN != iCreateLinkRet)
        //{
        //    BP_RUN_LOG_ERR(iCreateLinkRet,"Send Cmd","LoginSmu failed");
        //    linkInfo.SetLinkStatus(LINKID_STATUS_CLOSED);
        //    IVS_DOMAIN_ROUTE stDomainRouteInfo;
        //    memset(&stDomainRouteInfo, 0, sizeof(IVS_DOMAIN_ROUTE));
        //    if (IVS_SUCCEED == GetDomainRouteMgr().GetDomainRoutebyCode(pTargetDomainCode, stDomainRouteInfo))
        //    {
        //        (void)AddLinkIdToBusinessResumeList(strGeneralSmuLinkID,NET_ELE_SMU_NSS,stDomainRouteInfo.stIP.cIP,DEFAULT_SMU_PORT,TYPE_ADD_RELINK);
        //    }
        //    return iCreateLinkRet;
        //}
        //linkInfo.SetLinkStatus(LINKID_STATUS_OPENED);
        //linkInfo.SetStrLinkId(strGeneralSmuLinkID);
    }

    if (LINKID_STATUS_ENABLED == linkInfo.GetLinkStatus())
    {
        (void)ProcrssEnableLinkNode(linkInfo);
    }

    // 如果处于连接状态，发起业务;
    if (LINKID_STATUS_OPENED != linkInfo.GetLinkStatus())
    {
        BP_RUN_LOG_ERR(IVS_NET_CONNECT_ERROR, "fail to send asyn message", "connect is closed");
        return IVS_NET_CONNECT_ERROR;
    }

    const IVS_CHAR* pXmlData = "";
    IVS_UINT32 uiLen = 0;
    if (NULL != pSendXm)
    {

        pXmlData = pSendXm->GetXMLStream(uiLen);
        if (NULL == pXmlData)
        {
            BP_RUN_LOG_ERR(IVS_PARA_INVALID, "send asyn message", "request xml is null");
            return IVS_PARA_INVALID;
        }
    }

    CNssXMLMsg nssXMLMsg(uiReqID, 0, strTransID.c_str(), 0);


    nssXMLMsg.SetXML(pXmlData, uiLen);

    ACE_Message_Block* pMsgBlock = NULL;
    HW_NEW(pMsgBlock,ACE_Message_Block); //lint !e774 !e831
    CHECK_POINTER(pMsgBlock, IVS_NULL_POINTER);
    IVS_INT32 iRet = nssXMLMsg.encode(pMsgBlock);
    if (IVS_SUCCEED != iRet && NULL != pMsgBlock)
    {
        (void)pMsgBlock->release();
        pMsgBlock = NULL;
        BP_RUN_LOG_ERR(iRet, "SendMsg", "Encode nssXMLMsg fail.");
        return iRet;
    }
    
    CMainCtrl* pMainCtrl = CMainCtrl::GetMainCtrl();
    CHECK_POINTER(pMainCtrl, IVS_NULL_POINTER);

    iRet = CDispatcher::instance().dispatcherOutWithoutTransaction(NET_ELE_SMU_NSS, linkInfo.GetStrLinkId().c_str(), strTransID.c_str(), uiReqID, pMsgBlock);
    if (IVS_SUCCEED != iRet && NULL != pMsgBlock)
    {
        BP_RUN_LOG_ERR(iRet, "SendMsg", "DispatcherMsgOut failed.");
        (void)pMsgBlock->release();
        pMsgBlock = NULL;
        return iRet;
    }
    return IVS_SUCCEED;
}

// 构造并发送Cmd，strLinkID指定发送的连接
IVS_INT32 CUserMgr::SendSynCmd(CSendNssMsgInfo& sendNssMsgInfo,const std::string& strLinkID, std::string& strNSSRsp, uint32_t uiTimeout, IVS_BOOL bTimeout)
{
    IVS_DEBUG_TRACE("");

    // 打印请求报文;
    BP_DBG_LOG("request info NetElemType:%d, ReqID:%x, LinkID:%s", 
                sendNssMsgInfo.GetNetElemType(), 
                sendNssMsgInfo.GetReqID(),
                strLinkID.c_str());

    std::string strReqData = sendNssMsgInfo.GetReqData();
    PrintLog(strReqData);

	CCmd *pCmd = NULL;
    // 构造Cmd，发送结构体时，会指定长度，其他NSS消息不需要
    if (0 != sendNssMsgInfo.GetReqLength() && NULL != sendNssMsgInfo.GetReqDataPointer())
    {
        pCmd = CNSSOperator::instance().BuildCmd(strLinkID, sendNssMsgInfo.GetNetElemType(), sendNssMsgInfo.GetReqID(), sendNssMsgInfo.GetReqDataPointer(), (int)sendNssMsgInfo.GetReqLength());
    }
    else
    {
        pCmd = CNSSOperator::instance().BuildCmd(strLinkID, sendNssMsgInfo.GetNetElemType(), sendNssMsgInfo.GetReqID(), sendNssMsgInfo.GetReqData().c_str());
    }
	CHECK_POINTER(pCmd,IVS_SDK_RET_BP_CMD_REQ_INVALID);

	//发送nss协议
	IVS_INT32 iRet = IVS_FAIL;
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd, uiTimeout, bTimeout);
	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);
	//const IVS_CHAR* pRsp转换为XML并转换为数据结构
	if (TYPE_NSS_NOXML != sendNssMsgInfo.GetNeedXml())
	{
		BP_DBG_LOG("CUserMgr::SendSynCmd TYPE_NSS_XML OK");
        // 函数本身返回值iRet 和 解析消息的返回码iRetCode 需分开 modify by xiongfeng 00192614 2012-12-30
		IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iRet);  
		//CHECK_POINTER(pRsp,iRetCode);
        if (NULL != pRsp)
        {
            strNSSRsp = pRsp;
            IVS_DELETE(pRsp,MUILI);

            // 打印响应报文;
            PrintLog(strNSSRsp);
        }
	}
	else
	{
		BP_DBG_LOG("CUserMgr::SendSynCmd TYPE_NSS_NOXML Ok");
		iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	}

	HW_DELETE(pCmdRsp);
	return iRet;
}//lint !e1746 !e1762

// 建立连接并且是发起业务
IVS_INT32 CUserMgr::BuildLinkAndDoBusiness(const std::string& strDomainCode, const std::string& strNvrCode,
                                           CSendNssMsgInfo& sendNssMsgInfo, std::string &strNSSRsp)
{
	IVS_DEBUG_TRACE("");
	if (strDomainCode.empty() || strNvrCode.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Create Link and Send Cmd ","DomainCode or NvrCode is empty");
		return IVS_FAIL;
	}
	IVS_INT32 iRet = IVS_FAIL;
	//想scu发起连接
	// 创建连接成功后将linkID插入LinkRouteMgr统一由LoginMgr操作
	std::string strLinkID;
	iRet = m_loginObj.LoginScu(strDomainCode,strNvrCode, strLinkID);
	//连接不成功
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Create Link and Send Cmd ","Build Link Error");
		return iRet;
	}
	//发起业务动作，调用接口
	CSendNssMsgInfo sendNssInfo;
	sendNssInfo.SetReqID(sendNssMsgInfo.GetReqID());
	sendNssInfo.SetNeedXml(sendNssMsgInfo.GetNeedXml());
	sendNssInfo.SetNetElemType(NET_ELE_SCU_NSS);
	sendNssInfo.SetDomainCode(strDomainCode);
	sendNssInfo.SetCameraCode(sendNssMsgInfo.GetCameraCode());
	sendNssInfo.SetReqData(sendNssMsgInfo.GetReqData());
	//int iRetCode = IVS_FAIL;
	iRet = SendSynCmd(sendNssInfo,strLinkID,strNSSRsp);
	return iRet;
}

// 根据域编码和NVRCode查找LinkRouteMgr找SCU的LinkID
IVS_INT32 CUserMgr::SendCmdbyDomainNvrCode(const std::string& strDomainCode, const std::string& strNvrCode,
										   CSendNssMsgInfo& sendNssMsgInfo,std::string &strNSSRsp) 
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    CLinkInfo oLinkIdInfo;
    //memset(&stLinkIdInfo, 0, sizeof(LINKID_INFO));
    bool bRet = false;
    //获取scu的link信息
    bRet = CLinkRouteMgr::instance().GetScuLinkByDomainCodeAndNVRCode(m_iSessionID, strDomainCode, strNvrCode,
                                                                      oLinkIdInfo);
    // 如果没有找到linkID,需要创建连接
    if (!bRet)
    {
		BP_RUN_LOG_INF("Send Cmd by NVR","Can not find Link Info");
		//创建连接，并发送业务
		iRet = BuildLinkAndDoBusiness(strDomainCode,strNvrCode,sendNssMsgInfo,strNSSRsp);
		return iRet;
    }
    // 如果找到了linkID，判断当前LinkID是否为连接状态
    if ( LINKID_STATUS_OPENED == oLinkIdInfo.GetLinkStatus() )//换成宏,
    {
        // 发送请求;发起业务请求
		CSendNssMsgInfo sendNssInfo;
		sendNssInfo.SetReqID(sendNssMsgInfo.GetReqID());
		sendNssInfo.SetNeedXml(sendNssMsgInfo.GetNeedXml());
		sendNssInfo.SetNetElemType(NET_ELE_SCU_NSS);
		sendNssInfo.SetDomainCode(strDomainCode);
		sendNssInfo.SetCameraCode(sendNssMsgInfo.GetCameraCode());
		sendNssInfo.SetReqData(sendNssMsgInfo.GetReqData());
		iRet = SendSynCmd(sendNssInfo,oLinkIdInfo.GetStrLinkId(),strNSSRsp);

	}
    return iRet;
}

// 发重定向请求
IVS_INT32 CUserMgr::SendCmdbyReDirect(const IVS_DOMAIN_ROUTE& stDomainRoute, CSendNssMsgInfo& sendNssMsgInfo,std::string &strNSSRsp)
{
	IVS_DEBUG_TRACE("");
    int iRet = IVS_FAIL;
    IVS_DOMAIN_TYPE enDomainType = static_cast<IVS_DOMAIN_TYPE>(stDomainRoute.uiDomainType);
    // 如果不是集群，直接返回失败
    if (DOMAIN_TYPE_CLUSTER != enDomainType)
    {
		BP_RUN_LOG_ERR(IVS_SDK_NOT_CLUSTER_MODE, "Send Cmd by ReDirect","Current Mode is STACK");
        return IVS_SDK_NOT_CLUSTER_MODE;
    }
	
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
	memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
	if(!CToolsHelp::Memcpy(szDomainCode,IVS_DOMAIN_CODE_LEN  + 1,stDomainRoute.cDomainCode,IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Send Cmd by ReDirect", "DomainCode Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    std::string strDomainCode = szDomainCode;

	IVS_DEVICE_BASIC_INFO stDeviceBasicInfo;
	memset(&stDeviceBasicInfo, 0, sizeof(IVS_DEVICE_BASIC_INFO));
    unsigned int uiNVRSize = m_DeviceMgr.GetNVRSize(strDomainCode);
	std::string strLinkID;
	std::string strNvrCode;

    for (unsigned int i = 0; i < uiNVRSize; i++)
    {
        iRet = m_DeviceMgr.GetFirstNVR(strDomainCode, stDeviceBasicInfo);
        if (IVS_SUCCEED != iRet)
        {
			BP_RUN_LOG_ERR(iRet, "Send Cmd by ReDirect","Get first NVR Error");
            return iRet;
        }
		//获取设备NVR Code
		strNvrCode = stDeviceBasicInfo.cCode;
		iRet = m_loginObj.LoginScu(strDomainCode,strNvrCode,strLinkID);
		//如果当前连接没有成功，继续尝试其它NVR；
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "Send Cmd by ReDirect","Login Scu Error");
            continue;
        }
		//生成获取摄像机定位指令XML
		CXml xml;
		iRet = CLoginXMLProcess::GetFineMainDevXML(sendNssMsgInfo.GetCameraCode(),xml);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Send Cmd by ReDirect","Build Cmd Error");
			break;
		}
		unsigned int uiLen = 0;
		const char* pszData = xml.GetXMLStream(uiLen);
		CHECK_POINTER(pszData, IVS_OPERATE_MEMORY_ERROR);
		char *pReqData = IVS_NEW(pReqData,uiLen);
		if (NULL == pReqData)
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Send Cmd by ReDirect","Alloc ReqData Error");
			break;
		}
		if(!CToolsHelp::Memcpy(pReqData,uiLen,pszData,uiLen))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Send Cmd by ReDirect", "ReqData Memcpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		//发起重定位指令 sendSysCmd();如果执行失败也continue 如果成功表示找到当期IPC在那个NVR下
		//创建摄像机定位消息指令
		CSendNssMsgInfo stSendNssInfo;
		stSendNssInfo.SetReqID(NSS_CLUSTER_FIND_MAIN_DEV_REQ);
		stSendNssInfo.SetNeedXml(TYPE_NSS_XML);
		stSendNssInfo.SetNetElemType(NET_ELE_SCU_NSS);
		stSendNssInfo.SetDomainCode(strDomainCode);
		stSendNssInfo.SetCameraCode(sendNssMsgInfo.GetCameraCode());
		stSendNssInfo.SetReqData(pReqData);
		iRet = SendSynCmd(stSendNssInfo,strLinkID,strNSSRsp);
		if (IVS_SUCCEED != iRet)
		{
			//当前定位失败
			BP_RUN_LOG_ERR(iRet, "Send Cmd by ReDirect","Find Camera Error");
			continue;
		}
		//发起重定位摄像机结束，并查找到摄像机信息
		//生成结果XML
		CXml retXml;
		if (!retXml.Parse(strNSSRsp.c_str()))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Send Cmd by ReDirect","Parse XML Error");
			continue;
		}
		char szNvrCode[IVS_NVR_CODE_LEN + 1];
		memset(szNvrCode,0,IVS_NVR_CODE_LEN + 1);
		iRet = CLoginXMLProcess::ParseNvrCodeXML(retXml,szNvrCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Send Cmd by ReDirect","Get NVRCode Error");
			break;
		}
		//获取到新的nvrCode,发起新的连接
		std::string strNewNvrCode = szNvrCode;
        iRet = BuildLinkAndDoBusiness(strDomainCode,strNewNvrCode,sendNssMsgInfo,strNSSRsp);
        if (IVS_SUCCEED == iRet )
        {
            IVS_CAMERA_BRIEF_INFO stCameraBriefInfo;
            memset(&stCameraBriefInfo, 0, sizeof(IVS_CAMERA_BRIEF_INFO));
            IVS_INT32 iRetCamera = m_DeviceMgr.GetCameraBriefInfobyCode(sendNssMsgInfo.GetCameraCode(), stCameraBriefInfo);
            if (IVS_SUCCEED == iRetCamera)
            {
                BP_RUN_LOG_INF("Send Cmd by ReDirect","Modify Camera Info");
                m_DeviceMgr.ModifyDevStatusByDomainCode(strDomainCode,sendNssMsgInfo.GetCameraCode(),stCameraBriefInfo.uiStatus, strNewNvrCode);
            }
        }
		//执行到这里表示无论是否发起业务成功都返回
		break;
	}
    return iRet;
}

IVS_INT32 CUserMgr::GetVideoNumAndResource(const IVS_CHAR* pCameraCode,IVS_BOOL* bReachLimit,
								 IVS_BOOL* bWhetherCited)
{
	IVS_INT32 iRet = IVS_FAIL;
	std::string strCameraCode;
	std::string strDomainCode;
	iRet = CToolsHelp::SplitCameraDomainCode(pCameraCode, strCameraCode, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get ReqNumber And ReqRSP XML", "SplitCameraCode failed, RetCode=%d.", iRet);
		return iRet;
	}
	CXml xmlReq;
	iRet = CMediaXMLProcess::VideoNumAndResourceGetXML(strCameraCode, xmlReq); //lint !e838
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get ReqNumber And ReqRSP XML", "get xml false");
		return IVS_FAIL;
	}
	unsigned int xmlLen  = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	BP_DBG_LOG("Get Video ReqNumber And ReqRSP xml = %s.", pReq);
	char cLocalDomainCode[IVS_DOMAIN_CODE_LEN + 1];
	memset(cLocalDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
	bool bRet = CToolsHelp::Memcpy(cLocalDomainCode, IVS_DOMAIN_CODE_LEN + 1, strDomainCode.c_str(),
		IVS_DOMAIN_CODE_LEN);
	if (false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get ReqNumber And ReqRSP XML", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_VIDEO_NUM_AND_RESOURCE_REQ);
	sendNssMsgInfo.SetReqData(pReq);
	sendNssMsgInfo.SetCameraCode(strCameraCode);
	sendNssMsgInfo.SetDomainCode(strDomainCode);
	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = SendCmd(sendNssMsgInfo, strpRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get URL", "SendCmd operation succeed,rspCode = %d", iRet);
		return iRet;
	}
	//解析数据
	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());
	IVS_INT32 iReachLimit = 0;
	IVS_INT32 iWhetherCited = 0;
	iRet = CMediaXMLProcess::ParseVideoNumAndResource(xmlRsp, iReachLimit, iWhetherCited);
	*bReachLimit = (IVS_BOOL)iReachLimit;
	*bWhetherCited = (IVS_BOOL)iWhetherCited;
	return iRet;
}//lint !e818 !e1762

// 启动自动恢复定时器
void CUserMgr::StartResumeTimer()
{
	IVS_INFO_TRACE("");
	m_ResumeTimerInst.init(100);
	m_ResumeTimerInst.run();
	m_bAutoResume = true;
	try
	{
		// 		if ( !m_AutoResume.IsStart() )
		// 		{
		//启动定时器 15S
		//m_AutoResume.SetStart();
		m_ResumeTimerInst.registerTimer(&m_AutoResume, NULL, 150, enRepeated);
		//	}
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "AutoResume registerTimer failed", "NA");
		return;
	}

	m_CloseLinkIDInst.init(100);
	m_CloseLinkIDInst.run();
	m_bCloseLinkIDCount = true;
	try
	{
		//每1秒检查一次
		m_CloseLinkIDInst.registerTimer(&m_CloseLinkIDCount, NULL, 10, enRepeated);
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "CloseLinkIDCount registerTimer failed", "NA");
		return;
	}

	m_RelinkInst.init(100);
	m_RelinkInst.run();
	m_bRelink = true;
	try
	{
		//每30秒检查一次
		m_RelinkInst.registerTimer(&m_Relink, NULL, 100, enRepeated);

	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Relink registerTimer failed", "NA");
		return;
	}
}

// 停止定时器
void CUserMgr::StopResumeTimer()
{
	m_AutoResume.clearCamerInfoList();
	m_bAutoResume = false;
	m_ResumeTimerInst.cancelTimer(&m_AutoResume);
	m_ResumeTimerInst.clearTimer();
	m_ResumeTimerInst.exit();
	
	if (m_bCloseLinkIDCount)
	{
		m_bCloseLinkIDCount = false;
		m_CloseLinkIDInst.cancelTimer(&m_CloseLinkIDCount);
		m_CloseLinkIDInst.clearTimer();
		m_CloseLinkIDInst.exit();
	}
	
	if (m_bRelink)
	{
		m_bRelink = false;
		m_RelinkInst.cancelTimer(&m_Relink);
		m_RelinkInst.clearTimer();
		m_RelinkInst.exit();
	}
}

// 增加LinkID到链表中
void CUserMgr::AddLinkIdToBusinessResumeList(const std::string& strLinkID,int iNetElemType,const std::string& strIP,int iPort,int iAddType)
{
	IVS_DEBUG_TRACE("iNetElemType: %d, iAddType: %d, strLinkID: %s, strIP: %s", 
		iNetElemType, iAddType, strLinkID.c_str(), strIP.c_str());
    // 登录的SMU连接进行重连操作
	//modify by zwx2118311, Date:20140618, 增加指针判空操作 
	CUserInfo *pUserInfo = m_loginObj.GetUserInfo();
	if (NULL == pUserInfo)
	{
		BP_RUN_LOG_WAR("GetServerIP Failed.", "pUserInfo is NULL");
		return;
	}
	std::string strSerIP = pUserInfo->GetServerIP();
	//end

    if (NET_ELE_SMU_NSS == iNetElemType)
    {
        if (TYPE_ADD_CLOSECOUNT == iAddType)
        {
            m_CloseLinkIDCount.AddLinkIdToBusinessList(strLinkID,iNetElemType,strIP,iPort);
        }
        else
        {
            // 删除连接，防止通信层重连
            if (!strLinkID.empty())
            {
                CNetFactory::Instance().DeleteClientLink(strLinkID, iNetElemType);
            }
            
			// 本域抛一个消息上去
			if (0 == strSerIP.compare(strIP))
			{
				// 抛一个保活失败消息,添加到重登录任务
				if(NULL!=m_fnEventCallBack)
				{
					m_fnEventCallBack(IVS_EVENT_KEEP_ALIVE_FAILED, NULL, 1, m_pUserParam);
					BP_RUN_LOG_INF("Keep Alive Failed CallBack End.", "SessionID: %d, IP: %s, Port: %d", m_iSessionID, strIP.c_str(), iPort);
				}
				else
				{
					BP_RUN_LOG_WAR("Keep Alive Failed CallBack Failed.", "m_fnEventCallBack is NULL");
				}
			}
            if (IVS_SUCCEED == m_Relink.AddLinkIdToBusinessList(strLinkID,iNetElemType,strIP,iPort))
            {
                BP_RUN_LOG_INF("resume change notify,", "add resume item, element type:[%d], target ip:[%s:%u].", iNetElemType, strIP.c_str(), iPort);
            }
        }
    }
    // 添加OMU连接重连任务
    else if (NET_ELE_OMU_NSS == iNetElemType)
    {
        if (!strLinkID.empty())
        {
            CNetFactory::Instance().DeleteClientLink(strLinkID, iNetElemType);
        }
        if (IVS_SUCCEED == m_Relink.AddLinkIdToBusinessList(strLinkID,iNetElemType,strIP,iPort))
        {
            BP_RUN_LOG_INF("resume change notify,", "add resume item, element type:[%d], target ip:[%s:%u].", iNetElemType, strIP.c_str(), iPort);
        }
    }

	return;
}

// 从链表中移除LinkID
//void CUserMgr::RemoveLinkIDfromResumeList(const std::string& strLinkID,int iNetElemType,const std::string& strIP,int iPort,int iAddType)
//{
//    std::string strSerIP = m_loginObj.GetUserInfo()->GetServerIP();
//    if (NET_ELE_SMU_NSS == iNetElemType && 0 == strSerIP.compare(strIP))
//    {
//	    if (TYPE_ADD_CLOSECOUNT == iAddType)
//	    {
//		    m_CloseLinkIDCount.RemoveLinkIdFromBusinessList(strLinkID);
//	    }
//	    else
//	    {
//		    m_Relink.RemoveLinkIdFromBusinessList(strLinkID);
//	    }
//    }
//}

// 循环打印日志，因BP打印的单条日志不能超过2048
void CUserMgr::PrintLog(const std::string& strLog) const
{
    // 使用BP打印请求报文和结束报文,BP的单条日志长度为2048，除去文件名、日期等信息，剩余1930左右的长度;
    static const unsigned int s_uiLogLen = 1930;
    std::string strLogData = strLog;
    while (strLogData.length() > s_uiLogLen)
    {
        std::string strOutputStr = strLogData.substr(0, s_uiLogLen);
        // 打印请求报文;
        BP_DBG_LOG("%s", strOutputStr.c_str());

        strLogData = strLogData.substr(s_uiLogLen, strLogData.length() - 1);
    }

    // 打印剩余的请求报文;
    BP_DBG_LOG("%s", strLogData.c_str());
}

// 通知缓冲区状态事件
IVS_INT32 CUserMgr::NotifyStreamBufferState(IVS_ULONG ulPlayHandle, IVS_INT32 iBufferState)
{
    IVS_DEBUG_TRACE("");
    IVS_SERVICE_TYPE enServiceType = SERVICE_TYPE_INVALID;
    IVS_INT32 iRet = GetServiceTypebyPlayHandle(ulPlayHandle, enServiceType);

    // 主要用于客户端流控，仅下载和回放业务涉及到缓存数据
    switch (enServiceType)
    {
    case SERVICE_TYPE_DOWNLOAD:
        break;
    case SERVICE_TYPE_PLAYBACK:
	case SERVICE_TYPE_BACKUPRECORD_PLAYBACK:
	case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK:
	case SERVICE_TYPE_PU_PLAYBACK:
        iRet = m_PlaybackMgr.NotifyStreamBufferState(ulPlayHandle, iBufferState);
        break;
    case SERVICE_TYPE_PU_DOWNLOAD:
        break;
    case SERVICE_TYPE_REALVIDEO:
    case SERVICE_TYPE_RECORD:
    case SERVICE_TYPE_AUDIO_CALL:
    case SERVICE_TYPE_AUDIO_BROADCAST:
    default:
        BP_DBG_LOG("Service Type %d, PlayHandle %d", enServiceType, ulPlayHandle);
        break;
    }
    return iRet;
}

IVS_VOID CUserMgr::StopNotifyBufEvent(IVS_ULONG ulPlayHandle)
{
	IVS_DEBUG_TRACE("");
	IVS_SERVICE_TYPE enServiceType = SERVICE_TYPE_INVALID;
	(void)GetServiceTypebyPlayHandle(ulPlayHandle, enServiceType);

	// 主要用于客户端流控，仅下载和回放业务涉及到缓存数据
	switch (enServiceType)
	{
	case SERVICE_TYPE_DOWNLOAD:
		break;
	case SERVICE_TYPE_PLAYBACK:
	case SERVICE_TYPE_BACKUPRECORD_PLAYBACK:
	case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK:
	case SERVICE_TYPE_PU_PLAYBACK:
		m_PlaybackMgr.StopNotifyBufEvent(ulPlayHandle);
		break;
	case SERVICE_TYPE_PU_DOWNLOAD:
		break;
	case SERVICE_TYPE_REALVIDEO:
	case SERVICE_TYPE_RECORD:
	case SERVICE_TYPE_AUDIO_CALL:
	case SERVICE_TYPE_AUDIO_BROADCAST:
	default:
		BP_DBG_LOG("Service Type %d, PlayHandle %d", enServiceType, ulPlayHandle);
		break;
	}
}

IVS_INT32 CUserMgr::RecollectCameraDomainCodes2Map(IVS_UINT32 uiCameraNum,const IVS_DEVICE_CODE* pCameraCodeList, DeviceCodeMap& deviceCodeMap)
{
	IVS_INT32 iRet = IVS_SUCCEED;
	deviceCodeMap.clear();
	//1、提取域编码，判断代理域
	for (IVS_UINT32 i = 0; i < uiCameraNum; i++)
	{
		IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
		IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
		iRet += CXmlProcess::ParseDevCode(pCameraCodeList->cDevCode, chDevCode, chDomaCode);

		std::string strDomainCode = chDomaCode;
		//根据域编码查找目标域和代理域映射表，查找代理域;
		std::string strSendDomainCode;
		bool bRet = m_DomainRouteMgr.FindProxyDomainCode(strDomainCode, strSendDomainCode);
		// 如果未找到代理域，使用目标域代替;
		if (!bRet)
		{
			strSendDomainCode = strDomainCode;
		}

		// 判断strSendDomainCode是否已经在map里;
		DeviceCodeMap::iterator iter = deviceCodeMap.find(strSendDomainCode);
		if (iter == deviceCodeMap.end())
		{
			DeviceCodeList deviceCodeList;

			deviceCodeList.push_back(pCameraCodeList);
			(void)deviceCodeMap.insert(std::make_pair(strSendDomainCode, deviceCodeList));
		}
		else
		{
			DeviceCodeList& deviceCodeList = static_cast<DeviceCodeList>(iter->second); //lint !e612 !e69
			deviceCodeList.push_back(pCameraCodeList);
			deviceCodeMap.erase(iter);
			(void)deviceCodeMap.insert(std::make_pair(strSendDomainCode, deviceCodeList));
		}

		pCameraCodeList++;
	}

	if (IVS_SUCCEED != iRet)
	{
		iRet = IVS_FAIL;
	}
	return iRet;
}

CCmd * CUserMgr::BuildCmd(CSendNssMsgInfo& sendNssMsgInfo)
{
	IVS_DEBUG_TRACE("");
	// IVS_INT32 iRet = IVS_FAIL;
	// 如果域编码为空，直接发送，不考虑多级多域、重定向等
	std::string strDomainCode = sendNssMsgInfo.GetDomainCode();
	//std::string strCameraCode = sendNssMsgInfo.GetCameraCode();
	CLinkInfo oLinkIdInfo;

	//根据域编码查找目标域和代理域映射表，查找代理域
	std::string strSendDomainCode;
	bool bRet = m_DomainRouteMgr.FindProxyDomainCode(strDomainCode, strSendDomainCode);
	// 如果未找到代理域，使用目标域代替;
	if (!bRet)
	{
		strSendDomainCode = strDomainCode;
	}
	// 根据域编码查找LinkRouteMgr找SMU的LinkID
	bool bGetLinkRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(m_iSessionID, strSendDomainCode, oLinkIdInfo);// 此处调用查找LinkRouteMgr的函数； 
	if (!bGetLinkRet)
	{
        BP_RUN_LOG_ERR(bGetLinkRet, "Build cmd,", "domain code:[%s] is not exist in route list cache.", strSendDomainCode.c_str());
		// 创建连接;
		//std::string strGeneralSmuLinkID;
		//int iCreateLinkRet = m_loginObj.GeneralLoginSmu(strSendDomainCode, strGeneralSmuLinkID);
		//// BP建立可能返回-1的错误码，导致后续发送超时，返回超时错误码；不用考虑第一次登录错误码
 	//	if (IVS_SUCCEED != iCreateLinkRet && IVS_SMU_USER_FIRST_LOGIN != iCreateLinkRet)
 	//	{
 	//		BP_RUN_LOG_ERR(iCreateLinkRet,"Build Cmd","LoginSmu failed");
  //          IVS_DOMAIN_ROUTE stDomainRouteInfo;
  //          memset(&stDomainRouteInfo, 0, sizeof(IVS_DOMAIN_ROUTE));
  //          if (IVS_SUCCEED == GetDomainRouteMgr().GetDomainRoutebyCode(strDomainCode, stDomainRouteInfo))
  //          {
  //              (void)AddLinkIdToBusinessResumeList(strGeneralSmuLinkID,NET_ELE_SMU_NSS,stDomainRouteInfo.stIP.cIP,stDomainRouteInfo.uiPort,TYPE_ADD_RELINK);
  //          }

 	//		return NULL;
 	//	}
		//// 发送请求;
		//if (0 != sendNssMsgInfo.GetReqLength() && NULL != sendNssMsgInfo.GetReqDataPointer())
		//{
		//	return CNSSOperator::instance().BuildCmd(oLinkIdInfo.GetStrLinkId(), sendNssMsgInfo.GetNetElemType(), sendNssMsgInfo.GetReqID(), sendNssMsgInfo.GetReqDataPointer(), (int)sendNssMsgInfo.GetReqLength());
		//}
		//else
		//{
		//	return CNSSOperator::instance().BuildCmd(oLinkIdInfo.GetStrLinkId(), sendNssMsgInfo.GetNetElemType(), sendNssMsgInfo.GetReqID(), sendNssMsgInfo.GetReqData().c_str());
		//}
	}

    if (LINKID_STATUS_ENABLED == oLinkIdInfo.GetLinkStatus())
    {
        (void)ProcrssEnableLinkNode(oLinkIdInfo);
    }

	if (LINKID_STATUS_OPENED == oLinkIdInfo.GetLinkStatus())  	// 如果处于连接状态，直接发起业务;
	{
		// 构造Cmd，发送结构体时，会指定长度，其他NSS消息不需要
		if (0 != sendNssMsgInfo.GetReqLength() && NULL != sendNssMsgInfo.GetReqDataPointer())
		{
			return CNSSOperator::instance().BuildCmd(oLinkIdInfo.GetStrLinkId(), sendNssMsgInfo.GetNetElemType(), sendNssMsgInfo.GetReqID(), sendNssMsgInfo.GetReqDataPointer(), (int)sendNssMsgInfo.GetReqLength());
		}
		else
		{
			return CNSSOperator::instance().BuildCmd(oLinkIdInfo.GetStrLinkId(), sendNssMsgInfo.GetNetElemType(), sendNssMsgInfo.GetReqID(), sendNssMsgInfo.GetReqData().c_str());
		}
	}

	BP_RUN_LOG_ERR(IVS_FAIL,"Build Cmd","Build Cmd Failed. link status close");
	return NULL;
}

// 设置水印数据信息
void CUserMgr::SetWaterMarkInfo(IVS_ULONG ulPlayHandle, ST_WATERMARK_ALARM_INFO& stWaterMarkAlarmInfo)
{
    BP_DBG_LOG("CUserMgr::SetWaterMarkInfo, ulPlayHandle:%lu.", ulPlayHandle);

    CLockGuard lock(m_pWaterMarkInfoMutex);
    MAP_WATERMARK_ALARM_INFO_ITOR itor = m_mapWaterMarkInfo.find(ulPlayHandle);
    if (itor != m_mapWaterMarkInfo.end())
    {
        m_mapWaterMarkInfo.erase(itor++);
    }

    (void)m_mapWaterMarkInfo.insert(std::make_pair(ulPlayHandle, stWaterMarkAlarmInfo));

    BP_DBG_LOG("CUserMgr::SetWaterMarkInfo, ulPlayHandle:%d, cCameraCode:%s, cStartTime:%s, ullAlarmEventID:%d",
               ulPlayHandle, stWaterMarkAlarmInfo.stWaterMarkInfo.cCameraCode, stWaterMarkAlarmInfo.stWaterMarkInfo.cStartTime, 
               stWaterMarkAlarmInfo.stWaterMarkInfo.ullAlarmEventID);
}

// 获取水印数据信息
void CUserMgr::GetWaterMarkInfo(IVS_ULONG ulPlayHandle, ST_WATERMARK_ALARM_INFO &stWaterMarkAlarmInfo)
{ 
    BP_DBG_LOG("CUserMgr::GetWaterMarkInfo, ulPlayHandle:%lu.", ulPlayHandle);

    CLockGuard lock(m_pWaterMarkInfoMutex);
    MAP_WATERMARK_ALARM_INFO_ITOR itor = m_mapWaterMarkInfo.find(ulPlayHandle);
    if (itor != m_mapWaterMarkInfo.end())
    {
        memcpy(&stWaterMarkAlarmInfo, &(itor->second)/*&(itor->second.stWaterMarkInfo)*/, sizeof(stWaterMarkAlarmInfo));//lint !e826
    }

    BP_DBG_LOG("CUserMgr::GetWaterMarkInfo, cCameraCode:%s, cStartTime:%s, ullAlarmEventID:%d",
        stWaterMarkAlarmInfo.stWaterMarkInfo.cCameraCode, stWaterMarkAlarmInfo.stWaterMarkInfo.cStartTime, 
        stWaterMarkAlarmInfo.stWaterMarkInfo.ullAlarmEventID);
}

void CUserMgr::RemoveWaterMarkInfoFromMap(IVS_ULONG ulPlayHandle)
{
    CLockGuard lock(m_pWaterMarkInfoMutex);
    MAP_WATERMARK_ALARM_INFO_ITOR itor = m_mapWaterMarkInfo.find(ulPlayHandle);
    if (itor != m_mapWaterMarkInfo.end())
    {
        m_mapWaterMarkInfo.erase(itor++);
    }
}


//设置是否接收OMU设备类告警
void CUserMgr::SetReceiveOMUAlarm(IVS_BOOL bReceive)
{
	IVS_DEBUG_TRACE("SetReceiveOMUAlarm bReceive:%d", bReceive);

	if (!bReceive)
	{
		m_bReceiveOMUAlarm = 0;
		BP_RUN_LOG_INF("SetReceiveOMUAlarm False", "bReceive: %d", bReceive);
	}
	else
	{
		m_bReceiveOMUAlarm = 1;
	}
}


//更新重连起始时间
void CUserMgr::UpdateResumeStartTime(const time_t &ltime)
{
	IVS_DEBUG_TRACE("UpdateSSOResumeStartTime, ltime: %d", ltime);

	if (m_ResumeStartTime <= (time_t)0 || (time_t)0 == ltime)
	{
		m_ResumeStartTime = ltime;
		BP_RUN_LOG_INF("Update SSO Resume Start Time", "SSOResumeStartTime is: %d", m_ResumeStartTime);
	}

	return;
}