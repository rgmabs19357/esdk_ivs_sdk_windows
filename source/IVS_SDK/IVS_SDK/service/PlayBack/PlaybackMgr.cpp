#include "PlaybackMgr.h"
#include <time.h>
#include <stdio.h>
#include "LockGuard.h"
#include "UserMgr.h"
#include "MediaBase.h"
#include "IVS_Trace.h"

CPlaybackMgr::CPlaybackMgr(void)
{
    m_pUserMgr = NULL;
    m_pHandlePlayBackMutex = VOS_CreateMutex();
}

CPlaybackMgr::~CPlaybackMgr(void)
{
	m_pUserMgr = NULL;
	try
	{
		HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
		while(m_HanlePlayBackMap.end() != iter)
		{
			CPlayback* pPlayback = dynamic_cast<CPlayback*>(iter->second);//lint !e611
			if (NULL != pPlayback)
			{
				IVS_DELETE(pPlayback);
			}
			++iter;
		}
		m_HanlePlayBackMap.clear();

        if (NULL != m_pHandlePlayBackMutex)
        {
            (void)VOS_DestroyMutex(m_pHandlePlayBackMutex);
            m_pHandlePlayBackMutex = NULL;
        }
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Playback Mgr", "Catch Exception.");
	}
}

void CPlaybackMgr::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr = pUserMgr;
}

IVS_INT32 CPlaybackMgr::StartPlatformPlayBackCB(const IVS_CHAR* pCameraCode,
												const IVS_PLAYBACK_PARAM* pPlaybackParam,
												PlayBackCallBackRaw fPlayBackCallBackRaw,
												void* pUserData, 
												IVS_ULONG* pHandle)
{
	IVS_DEBUG_TRACE("");

	if (NULL == pCameraCode || NULL == pPlaybackParam)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back CB", "CheckParam failed.");
		return IVS_PARA_INVALID;
	}

	TIME_SPAN_INTERNAL stTimeSpanInter;
	memset(&stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
	IVS_INT32 iRet = CheckParam(pCameraCode, &pPlaybackParam->stTimeSpan, pPlaybackParam->uiProtocolType, pPlaybackParam->fSpeed, stTimeSpanInter);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back CB", "CheckParam failed.");
		return IVS_PARA_INVALID;
	}

	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

	//初始化对象;
	CPlayback *pPlayBack = GetFreePlayback(ulHandle);
	if (NULL != pPlayBack)
	{
		iRet = pPlayBack->StartPlatformPlayBackCB(NULL,NULL,pCameraCode, pPlaybackParam->uiProtocolType, stTimeSpanInter, fPlayBackCallBackRaw, pUserData, pPlaybackParam->fSpeed);
		if (IVS_SUCCEED != iRet)
		{
			pPlayBack->RetSet();
		}
		else
		{
			*pHandle = ulHandle;
		}
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}

	return iRet;
}

IVS_INT32 CPlaybackMgr::StartPlatformPlayBackCBFrame(const IVS_CHAR* pCameraCode,
													 const IVS_PLAYBACK_PARAM* pPlaybackParam,
													 PlayBackCallBackFrame fPlayBackCallBackFrame,
													 void* pUserData, 
													 IVS_ULONG* pHandle)
{
	IVS_DEBUG_TRACE("");

	if (NULL == pCameraCode || NULL == pPlaybackParam || NULL == pHandle)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back CB", "CheckParam failed.");
		return IVS_PARA_INVALID;
	}

	TIME_SPAN_INTERNAL stTimeSpanInter;
	memset(&stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
	IVS_INT32 iRet = CheckParam(pCameraCode, &pPlaybackParam->stTimeSpan, pPlaybackParam->uiProtocolType, pPlaybackParam->fSpeed, stTimeSpanInter);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back CB", "CheckParam failed.");
		return IVS_PARA_INVALID;
	}

	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

	//初始化对象;
	CPlayback *pPlayBack = GetFreePlayback(ulHandle);
	if (NULL != pPlayBack)
	{
		iRet = pPlayBack->StartPlatformPlayBackCBFrame(NULL,NULL,pCameraCode, pPlaybackParam->uiProtocolType, stTimeSpanInter, fPlayBackCallBackFrame, pUserData, pPlaybackParam->fSpeed);
		if (IVS_SUCCEED != iRet)
		{
			pPlayBack->RetSet();
		}
		else
		{
			*pHandle = ulHandle;
		}
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}

	return iRet;
}

//开始平台录像回放
IVS_INT32 CPlaybackMgr::StartPlatformPlayBack(const IVS_CHAR* pCameraCode,
                                              const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                              HWND hWnd,
                                              IVS_ULONG* pHandle)
{
	IVS_DEBUG_TRACE("");

    if (NULL == pCameraCode || NULL == pPlaybackParam)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    TIME_SPAN_INTERNAL stTimeSpanInter;
    memset(&stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
	IVS_INT32 iRet = CheckParam(pCameraCode, &pPlaybackParam->stTimeSpan, pPlaybackParam->uiProtocolType, pPlaybackParam->fSpeed, stTimeSpanInter);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
   
	//如果已经存在，那要关闭
    ulHandle = GetHandleByHwnd(hWnd);//lint !e838
    if (SDK_INVALID_HANDLE != ulHandle)
    {
        BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED,"Start Platform Play Back", "the hwnd is busy, stop the hwnd first");
        (void)StopPlatformPlayBack(ulHandle);
    }

    //初始化对象;
    CPlayback *pPlayBack = GetFreePlayback(ulHandle);
    if (NULL != pPlayBack)
    {
        //TO DO 每一路设置一个异常回调

        iRet = pPlayBack->StartPlatformOrBackupPlayBack(NULL,NULL,pCameraCode, pPlaybackParam->uiProtocolType, stTimeSpanInter, hWnd, pPlaybackParam->fSpeed);
        if (IVS_SUCCEED != iRet)
        {
            pPlayBack->RetSet();
        }
        else
        {
            *pHandle = ulHandle;
        }
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

	return iRet;
}//lint !e818 //不需要

//检验回放参数是否合法
IVS_INT32 CPlaybackMgr::CheckParam(const IVS_CHAR* pCameraCode,const IVS_TIME_SPAN* pTimeSpan,IVS_UINT32 uiProtocolType, IVS_FLOAT fSpeed, TIME_SPAN_INTERNAL& stTimeSpanInter)
{
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTimeSpan,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;

    stTimeSpanInter.iStartTime = (IVS_INT32)CMediaBase::UTCStr2Time(pTimeSpan->cStart, IVS_DATE_TIME_FORMAT);
    stTimeSpanInter.iEndTime = (IVS_INT32)CMediaBase::UTCStr2Time(pTimeSpan->cEnd, IVS_DATE_TIME_FORMAT);
	if( stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime )
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform Video Play: param time span  is invalidate","failed");
		return IVS_PARA_INVALID;
	}

	if (PROTOCOL_RTP_OVER_UDP == uiProtocolType)
	{
		//UDP
	}
	else if(PROTOCOL_RTP_OVER_TCP == uiProtocolType)
	{
		//TCP
	}
	else
	{
		iRet = IVS_PARA_INVALID;
		return iRet;
	}

    // 检查播放倍速是否合法;
    bool bRet = CToolsHelp::IsSpeedVaild(fSpeed);
    if (!bRet)
    {
        iRet = IVS_PARA_INVALID;
        return iRet;
    }

	return IVS_SUCCEED;
}//lint !e1762
//根据句柄获取空闲对象
CPlayback* CPlaybackMgr::GetFreePlayback(IVS_ULONG& ulHandle)
{
    CLockGuard lock(m_pHandlePlayBackMutex);
	CPlayback *pPlayback = NULL;
	HandlePlayBackMapIter playbackIter = m_HanlePlayBackMap.begin();
	HandlePlayBackMapIter playbackIterEnd = m_HanlePlayBackMap.end();

	//查找空闲对象;
	for (; playbackIter != playbackIterEnd; playbackIter++)
	{
		pPlayback = dynamic_cast<CPlayback*>(playbackIter->second);//lint !e611
		if (NULL != pPlayback)
		{
			if (pPlayback->GetStatus() == MEDIA_STATUS_FREE)
			{
				break;
			}
		}
		pPlayback = NULL;
	}

	if (NULL != pPlayback)
	{
		m_HanlePlayBackMap.erase(playbackIter);
	}
	else
	{
		pPlayback = IVS_NEW((CPlayback*&)pPlayback);
		if (NULL == pPlayback)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Free RealPlay", "create real play error");
			return NULL;
		}
	}

	//初始化realplay对象;
	pPlayback->SetUserMgr(m_pUserMgr);
	pPlayback->SetStatus(MEDIA_STATUS_BUSY);

	//插入列表;
	ulHandle = reinterpret_cast<unsigned long>(pPlayback);
	(void)m_HanlePlayBackMap.insert(std::make_pair(ulHandle, pPlayback));

	pPlayback->SetHandle(ulHandle);
	return pPlayback;
}

//根据窗体句柄获得句柄号;
IVS_ULONG CPlaybackMgr::GetHandleByHwnd(HWND hWnd)
{
    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
    CLockGuard lock(m_pHandlePlayBackMutex);

    CPlayback* pPlayBack = NULL;
    HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
    HandlePlayBackMapIter iterEnd = m_HanlePlayBackMap.end();

    //查找空闲对象;
    for (;iter != iterEnd; iter++)
    {
        pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
        if (NULL != pPlayBack && pPlayBack->IsExistHWnd(hWnd))
        {
            ulHandle = iter->first;
            break;
        }
    }
    return ulHandle;
}//lint !e818
//停止平台录像回放
IVS_INT32 CPlaybackMgr::StopPlatformPlayBack(IVS_ULONG ulHandle)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayBack = GetPlayBack(ulHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->StopPlatformPlayBack();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//开始容灾录像回放
IVS_INT32 CPlaybackMgr::StartDisasterRecoveryPlayBack(const IVS_CHAR* pCameraCode,
    const IVS_PLAYBACK_PARAM* pPlaybackParam,
    HWND hWnd,
    IVS_ULONG* pHandle)
{
    IVS_DEBUG_TRACE("");//lint !e830

    if (NULL == pCameraCode || NULL == pPlaybackParam)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Disaster Recovery Play Back", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    TIME_SPAN_INTERNAL stTimeSpanInter;
    memset(&stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
    IVS_INT32 iRet = CheckParam(pCameraCode, &pPlaybackParam->stTimeSpan, pPlaybackParam->uiProtocolType, pPlaybackParam->fSpeed, stTimeSpanInter);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Disaster Recovery Play Back", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

    //如果已经存在，那要关闭
    ulHandle = GetHandleByHwnd(hWnd);//lint !e838
    if (SDK_INVALID_HANDLE != ulHandle)
    {
        BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED,"Start Disaster Recovery Play Back", "the hwnd is busy, stop the hwnd first");
        (void)StopDisasterRecoveryPlayBack(ulHandle);
    }

    // TODO
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    if (NULL != this->m_pUserMgr)
    {
        // 获取域路由，记录日志
        CDomainRouteMgr& domainRouteMgr = m_pUserMgr->GetDomainRouteMgr();

        string strLocalDomainCode;
        domainRouteMgr.GetLocalDomainCode(strLocalDomainCode);

        if (!CToolsHelp::Memcpy(chDomaCode,IVS_DOMAIN_CODE_LEN,strLocalDomainCode.c_str(),strLocalDomainCode.length()))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Query Record List", "memcpy failed");
            return IVS_ALLOC_MEMORY_ERROR;
        }
        // memset(chDomaCode,'\0',IVS_DOMAIN_CODE_LEN + 1);
        BP_DBG_LOG(" record method disaster recovery set domain is local[%s]. ",strLocalDomainCode.c_str());
    }

    //初始化对象;
    CPlayback *pPlayBack = GetFreePlayback(ulHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->StartPlatformOrBackupPlayBack(chDomaCode,NULL,pCameraCode, pPlaybackParam->uiProtocolType, stTimeSpanInter, hWnd, pPlaybackParam->fSpeed,
            SERVICE_TYPE_DISASTEBACKUP_PLAYBACK);
        if (IVS_SUCCEED != iRet)
        {
            pPlayBack->RetSet();
        }
        else
        {
            *pHandle = ulHandle;
        }
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}//lint !e818 //不需要

//停止容灾录像回放
IVS_INT32 CPlaybackMgr::StopDisasterRecoveryPlayBack(IVS_ULONG ulHandle)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayBack = GetPlayBack(ulHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->StopPlatformPlayBack();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

/*
 * PlayBackFrameStepForward
 * 平台录像单步播放前进
 */
IVS_INT32 CPlaybackMgr::PlayBackFrameStepForward(IVS_ULONG ulPlayHandle)
{
	IVS_DEBUG_TRACE("");
    CPlayback *pPlayBack = GetPlayBack(ulPlayHandle);
	if (NULL != pPlayBack)
	{
		return pPlayBack->StepForward();
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

/*
 * PlayBackFrameStepBackWard
 * 平台录像单步播放后退
 */
IVS_INT32 CPlaybackMgr::PlayBackFrameStepBackward(IVS_ULONG ulPlayHandle)
{
	IVS_DEBUG_TRACE("");
	CPlayback *pPlayBack = GetPlayBack(ulPlayHandle);
	if (NULL != pPlayBack)
	{
		return pPlayBack->StepBackward();
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

// 暂停回放
IVS_INT32 CPlaybackMgr::PlayBackPause(IVS_ULONG ulHandle)
{	
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayBack = GetPlayBack(ulHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->PlayBackPause();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

// 恢复回放
IVS_INT32 CPlaybackMgr::PlayBackResume(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayBack = GetPlayBack(ulHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->PlayBackResume();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

IVS_INT32 CPlaybackMgr::GetPlayBackStatus(IVS_ULONG ulPlayHandle, IVS_BOOL& bPause)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SUCCEED;

	CPlayback *pPlayBack = GetPlayBack(ulPlayHandle);
	if (NULL != pPlayBack)
	{
		bPause = pPlayBack->IsPaused() ? 0 : 1;//0为暂停，1为播放;
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}

	return iRet;
}

IVS_INT32 CPlaybackMgr::SetPlayBackSpeed(IVS_ULONG ulHandle, float fSpeed)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;

	CPlayback *pPlayBack = GetPlayBack(ulHandle);
	if (NULL != pPlayBack)
	{
		iRet = pPlayBack->SetPlayBackSpeed(fSpeed);
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	return iRet;
}

IVS_INT32 CPlaybackMgr::GetPlayBackSpeed(IVS_ULONG ulHandle, float& fSpeed)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SUCCEED;

	CPlayback *pPlayBack = GetPlayBack(ulHandle);
	if (NULL != pPlayBack)
	{
		fSpeed = pPlayBack->GetPlayBackSpeed();
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	return iRet;
}

/*
 * GetCurrentFrameTick
 * 获取当前播放帧时间戳
 */
IVS_INT32 CPlaybackMgr::GetCurrentFrameTick(IVS_ULONG ulPlayHandle, IVS_UINT64* pTick)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pTick, IVS_PARA_INVALID);

	unsigned long long tick = 0;
	CPlayback *pPlayBack = GetPlayBack(ulPlayHandle);
	if (NULL != pPlayBack && IVS_SUCCEED == pPlayBack->GetCurrentFrameTick(tick))
	{
		*pTick = tick;
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}


// 获取回放时间
IVS_INT32 CPlaybackMgr::GetPlayBackTime(IVS_ULONG ulPlayHandle, IVS_PLAYBACK_TIME* pPlayBackTime)
{
	IVS_DEBUG_TRACE("");
    
    if (NULL == pPlayBackTime)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get PlayBack Time", "Param error pPlayBackTime = NULL");
        return IVS_PARA_INVALID;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CPlayback *pPlayBack = GetPlayBack(ulPlayHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->GetPlayBackTime(*pPlayBackTime);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//设置录像回放播放时间
IVS_INT32 CPlaybackMgr::SetPlayBackTime(IVS_ULONG ulHandle ,IVS_UINT32 uiTime)
{
	IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;
    CPlayback *pPlayBack = GetPlayBack(ulHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->SetPlayBackTime(uiTime);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//根据句柄获取对象;
CPlayback* CPlaybackMgr::GetPlayBack(IVS_ULONG ulHandle)
{
    CLockGuard lock(m_pHandlePlayBackMutex);
    CPlayback *pPlayBack = NULL;
    HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
    HandlePlayBackMapIter iterEnd = m_HanlePlayBackMap.end();

    //查找空闲对象;
    for (; iter != iterEnd; iter++)
    {
        if (iter->first == ulHandle)
        {
            pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
            break;
        }
    }

    return pPlayBack;
}
//根据Rtsp句柄获取播放句柄;
IVS_ULONG CPlaybackMgr::GetPlayHandlebyRtspHandle(int iRtspHandle)
{
	CLockGuard lock(m_pHandlePlayBackMutex);
    IVS_ULONG ulPlayHandle = 0;
    HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
    HandlePlayBackMapIter iterEnd = m_HanlePlayBackMap.end();
    for (; iter != iterEnd; iter++)
    {
        CPlayback* pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
        if (NULL == pPlayBack)
        {
            continue;
        }
        int iRtspHandleTemp = pPlayBack->GetRtspHandle();
        if (iRtspHandleTemp == iRtspHandle)
        {
            ulPlayHandle = iter->first;
            break;
        }
    }

    return ulPlayHandle;
}
//获取播放句柄
IVS_ULONG CPlaybackMgr::GetPlayHandleByNetSourceChannel(unsigned long ulNetSourceChannel)
{
	CLockGuard lock(m_pHandlePlayBackMutex);
    IVS_ULONG ulPlayHandle = 0;
    HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
    HandlePlayBackMapIter iterEnd = m_HanlePlayBackMap.end();
    for (; iter != iterEnd; iter++)
    {
        CPlayback* pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
        if (NULL == pPlayBack)
        {
            continue;
        }
        unsigned long ulNetSourceHandleTemp = pPlayBack->GetNetSourceHandle();
        if (ulNetSourceHandleTemp == ulNetSourceChannel)
        {
            ulPlayHandle = iter->first;
            break;
        }
    }

    return ulPlayHandle;
}
//根据播放信道号获取播放句柄
IVS_ULONG CPlaybackMgr::GetPlayHandleByPlayChannel(unsigned long ulPlayChannel)
{
	CLockGuard lock(m_pHandlePlayBackMutex);
    IVS_ULONG ulPlayHandle = 0;
    HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
    HandlePlayBackMapIter iterEnd = m_HanlePlayBackMap.end();
    for (; iter != iterEnd; iter++)
    {
        CPlayback* pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
        if (NULL == pPlayBack)
        {
            continue;
        }
        unsigned long ulNetSourceHandleTemp = pPlayBack->GetPlayerChannel();
        if (ulNetSourceHandleTemp == ulPlayChannel)
        {
            ulPlayHandle = iter->first;
            break;
        }
    }

    return ulPlayHandle;
}

// 播放随路语音
IVS_INT32 CPlaybackMgr::PlayAudioSound(unsigned int uiPlayHandle)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayback = dynamic_cast<CPlayback *>(GetPlayBack(uiPlayHandle));
    if (NULL != pPlayback)
    {
        iRet = pPlayback->PlaySound();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//停止播放随路语音
IVS_INT32 CPlaybackMgr::StopAudioSound(unsigned int uiPlayHandle)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayback = dynamic_cast<CPlayback *>(GetPlayBack(uiPlayHandle));
    if (NULL != pPlayback)
    {
        iRet = pPlayback->StopSound();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//设置音量
IVS_INT32 CPlaybackMgr::SetVolume(unsigned int uiPlayHandle, unsigned int uiVolumeValue)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayback = dynamic_cast<CPlayback *>(GetPlayBack(uiPlayHandle));
    if (NULL != pPlayback)
    {
        iRet = pPlayback->SetVolume(uiVolumeValue);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//获取音量
IVS_INT32 CPlaybackMgr::GetVolume(unsigned int uiPlayHandle, unsigned int* puiVolumeValue)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayback = dynamic_cast<CPlayback *>(GetPlayBack(uiPlayHandle));
    if (NULL != pPlayback)
    {
        iRet = pPlayback->GetVolume(puiVolumeValue);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//根据句柄获取meidiaBase对象指针;
CMediaBase *CPlaybackMgr::GetMediaBase(IVS_ULONG ulHandle)
{
	CLockGuard lock(m_pHandlePlayBackMutex);
	HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
	HandlePlayBackMapIter iterEnd = m_HanlePlayBackMap.end();
	for (; iter != iterEnd; iter++)
	{
		CPlayback* pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
		if (NULL == pPlayBack)
		{
			continue;
		}

		if (pPlayBack->GetHandle() == ulHandle)
		{
			return pPlayBack;
		}
	}
	return NULL;
}
//根据句柄获取播放信道
unsigned int CPlaybackMgr::GetPlayerChannelByHandle(unsigned long ulHandle)
{
	CLockGuard lock(m_pHandlePlayBackMutex);
	HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
	HandlePlayBackMapIter iterEnd = m_HanlePlayBackMap.end();
	for (; iter != iterEnd; iter++)
	{
		CPlayback* pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
		if (NULL == pPlayBack)
		{
			continue;
		}
		if (pPlayBack->GetHandle() == ulHandle)
		{
			return pPlayBack->GetPlayerChannel();
		}
	}
	return 0;
}

/******************************************************************
function : StartPUPlayBack
description: 开始前端录像回放;
input : pPlaybackParam,回放参数；pCameraCode，摄像机编号；hWnd，窗体句柄；pHandle，句柄
output : NA
return : 成功-0；失败-1
*******************************************************************/
IVS_INT32 CPlaybackMgr::StartPUPlayBack(const IVS_CHAR* pCameraCode,
                            const IVS_PLAYBACK_PARAM* pPlaybackParam,
                            HWND hWnd,
                            IVS_ULONG* pHandle)
{
    if (NULL == pCameraCode || NULL == pPlaybackParam)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    TIME_SPAN_INTERNAL stTimeSpanInter;
    memset(&stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
    IVS_INT32 iRet = CheckParam(pCameraCode, &pPlaybackParam->stTimeSpan, pPlaybackParam->uiProtocolType, pPlaybackParam->fSpeed, stTimeSpanInter);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform Play Back", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    IVS_RECORD_INFO stRecordInfo;
    memset(&stRecordInfo, 0, sizeof(stRecordInfo));
    iRet = GetPUPlayBackFileName(pCameraCode,
                             &pPlaybackParam->stTimeSpan,
                             stRecordInfo);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform Play Back",
                       "Get PU playBack fileName failed with %d.", iRet);
        return iRet;
    }

    BP_DBG_LOG("Get PU playBack fileName %s", stRecordInfo.cRecordFileName);

    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

    //如果已经存在，那要关闭
    ulHandle = GetHandleByHwnd(hWnd);//lint !e838
    if (SDK_INVALID_HANDLE != ulHandle)
    {
        BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED,"Start Platform Play Back", "the hwnd is busy, stop the hwnd first");
        (void)StopPUPlayBack(ulHandle);
    }

    //初始化对象;
    CPlayback *pPlayBack = GetFreePlayback(ulHandle);
    if (NULL != pPlayBack)
    {
        //TO DO 每一路设置一个异常回调

        iRet = pPlayBack->StartPUPlayBack(pCameraCode,
                                          pPlaybackParam->uiProtocolType,
                                          stTimeSpanInter,
                                          hWnd,
                                          pPlaybackParam->fSpeed,
                                          stRecordInfo.cRecordFileName);
        if (IVS_SUCCEED != iRet)
        {
            pPlayBack->RetSet();
        }
        else
        {
            *pHandle = ulHandle;
        }
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

/******************************************************************
function : StopPUPlayBack
description: 停止前端录像回放;
input : ulHandle,句柄
output : NA
return : 成功-0；失败-1
*******************************************************************/
IVS_INT32 CPlaybackMgr::StopPUPlayBack(IVS_ULONG ulHandle)
{
    IVS_INT32 iRet = IVS_FAIL;

    CPlayback *pPlayBack = GetPlayBack(ulHandle);
    if (NULL != pPlayBack)
    {
        iRet = pPlayBack->StopPUPlayBack();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

IVS_INT32 CPlaybackMgr::GetPUPlayBackFileName(const char* pCameraCode,
                                              const IVS_TIME_SPAN* pTimeSpan,
                                              IVS_RECORD_INFO &stRecordInfo)
{
    if (NULL == pCameraCode || NULL == pTimeSpan)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PU PlayBack FileName", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pUserMgr)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PU PlayBack FileName", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    memset(&stRecordInfo,0,sizeof(stRecordInfo));

    IVS_INDEX_RANGE stIndexRange;
    stIndexRange.uiFromIndex = 1;
    stIndexRange.uiToIndex = 1;
    int iSize = sizeof(IVS_RECORD_INFO_LIST) + (stIndexRange.uiToIndex-stIndexRange.uiFromIndex) * sizeof(IVS_RECORD_INFO);   //lint !e737 !e713

	char *pBuff = IVS_NEW((char* &)pBuff, (unsigned long)iSize);
	if (NULL == pBuff)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
    memset(pBuff, 0, iSize);   //lint !e732

	CRecordList& recordList = m_pUserMgr->GetRecordList();
    int iRet = recordList.GetRecordList(pCameraCode, 
                                        RECORD_TYPE_PU,
                                        pTimeSpan,
                                        &stIndexRange,
                                        (IVS_RECORD_INFO_LIST*)pBuff,
                                        (IVS_UINT32)iSize);   //lint !e826


    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get PU PlayBack FileName", "Get Record List failed.");
    }
	else
	{
		IVS_RECORD_INFO_LIST *pRecordList = (IVS_RECORD_INFO_LIST*)pBuff;   //lint !e826
		(void)CToolsHelp::Memcpy(&stRecordInfo, sizeof(stRecordInfo),
			pRecordList->stRecordInfo, sizeof(IVS_RECORD_INFO));
	}

	IVS_DELETE(pBuff, MUILI);
    return iRet;
}

IVS_INT32 CPlaybackMgr::NotifyStreamBufferState(IVS_ULONG ulPlayHandle, IVS_INT32 iBufferState)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    CPlayback *pPlayback = dynamic_cast<CPlayback *>(GetPlayBack(ulPlayHandle));
    if (NULL != pPlayback)
    {
        BP_DBG_LOG("CPlaybackMgr::NotifyStreamBufferState, ulPlayHandle:%lu, iBufferState:%d.", ulPlayHandle, iBufferState);
        iRet = pPlayback->NotifyStreamBufferState(iBufferState);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
        BP_RUN_LOG_ERR(iRet, "Notify Stream Buffer State", "Get play back by playhandle[%lu] failed.", ulPlayHandle);
    }
    return iRet;
}

IVS_VOID CPlaybackMgr::StopNotifyBufEvent(IVS_ULONG ulPlayHandle)
{
	IVS_DEBUG_TRACE("");
	CPlayback *pPlayback = dynamic_cast<CPlayback *>(GetPlayBack(ulPlayHandle));
	if (NULL != pPlayback)
	{
		pPlayback->StopNotifyBufEvent();
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED, "StopNotifyBufEvent", "Get play back by playhandle[%lu] failed.", ulPlayHandle);
	}
}

//停止备份录像回放
IVS_INT32 CPlaybackMgr::StopBackupPlayBack(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;

	CPlayback *pPlayBack = GetPlayBack(ulHandle);
	if (NULL != pPlayBack)
	{
		iRet = pPlayBack->StopBackupPlayBack();
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
		BP_RUN_LOG_ERR(iRet,"StopBackupPlayBack", "");
	}

	return iRet;
}

//开始备份录像回放
IVS_INT32 CPlaybackMgr::StartBackupPlayBack(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,
	const IVS_PLAYBACK_PARAM* pPlaybackParam,
	HWND hWnd,
	IVS_ULONG* pHandle)
{
	IVS_DEBUG_TRACE("");

	if (NULL == pNVRCode ||NULL == pCameraCode || NULL == pPlaybackParam)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Backup Play Back", "CheckParam failed.");
		return IVS_PARA_INVALID;
	}

	TIME_SPAN_INTERNAL stTimeSpanInter;
	memset(&stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
	IVS_INT32 iRet = CheckParam(pCameraCode, &pPlaybackParam->stTimeSpan, pPlaybackParam->uiProtocolType, pPlaybackParam->fSpeed, stTimeSpanInter);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Backup Play Back", "CheckParam failed.");
		return IVS_PARA_INVALID;
	}

	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

	//如果已经存在，那要关闭
	ulHandle = GetHandleByHwnd(hWnd);//lint !e838
	if (SDK_INVALID_HANDLE != ulHandle)
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED,"Start Backup Play Back", "the hwnd is busy, stop the hwnd first");
		(void)StopBackupPlayBack(ulHandle);
	}

	//初始化对象;
	CPlayback *pPlayBack = GetFreePlayback(ulHandle);
	if (NULL != pPlayBack)
	{
		//TO DO 每一路设置一个异常回调

		iRet = pPlayBack->StartPlatformOrBackupPlayBack(pDomainCode,pNVRCode,pCameraCode, pPlaybackParam->uiProtocolType, stTimeSpanInter, hWnd, pPlaybackParam->fSpeed);
		if (IVS_SUCCEED != iRet)
		{
			pPlayBack->RetSet();
		}
		else
		{
			*pHandle = ulHandle;
		}
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}

	return iRet;
}//lint !e818 //不需要

//添加播放窗口
IVS_INT32 CPlaybackMgr::AddPlayWindow(IVS_ULONG ulHandle, HWND hWnd)
{
	CPlayback *pPlayBack = dynamic_cast<CPlayback *>(GetMediaBase(ulHandle));
	if (NULL != pPlayBack)
	{
		return pPlayBack->AddPlayWindow(hWnd);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}
//删除播放窗口
IVS_INT32 CPlaybackMgr::DelPlayWindow(IVS_ULONG ulHandle, HWND hWnd)
{
	CPlayback *pPlayBack = dynamic_cast<CPlayback *>(GetMediaBase(ulHandle));
	if (NULL != pPlayBack)
	{
		return pPlayBack->DelPlayWindow(hWnd);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//增加局部图像播放窗口
IVS_INT32 CPlaybackMgr::AddPlayWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial)
{
	CPlayback *pPlayBack = dynamic_cast<CPlayback *>(GetMediaBase(ulHandle));
	if (NULL != pPlayBack)
	{
		return pPlayBack->AddPlayWindowPartial(hWnd, pPartial);
	}

	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//更新局部放大源矩形
IVS_INT32 CPlaybackMgr::UpdateWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial)
{
	CPlayback *pPlayBack = dynamic_cast<CPlayback *>(GetMediaBase(ulHandle));
	if (NULL != pPlayBack)
	{
		return pPlayBack->UpdateWindowPartial(hWnd, pPartial);
	}

	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

// 启动本地回放
IVS_INT32 CPlaybackMgr::StartLocalPlayBack(const IVS_CHAR* pFileName, 
	          const IVS_LOCAL_PLAYBACK_PARAM* pPlaybackParam, HWND hWnd, IVS_ULONG* pHandle)
{
	IVS_INT32 iRet = IVS_SUCCEED;
	unsigned long ulHandle = 0;
	CPlayback *pPlayBack = GetFreePlayback(ulHandle);
	if (NULL != pPlayBack)
	{
        iRet = pPlayBack->StartLocalPlayBack(pFileName, pPlaybackParam, hWnd);
		if (IVS_SUCCEED != iRet)
		{
			pPlayBack->RetSet();
		}
		else
		{
			*pHandle = ulHandle;
		}
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}

	return iRet;
}

// 停止本地回放
IVS_INT32 CPlaybackMgr::StopLocalPlayBack(IVS_ULONG ulPlayHandle)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;

	CPlayback *pPlayBack = GetPlayBack(ulPlayHandle);
	if (NULL != pPlayBack)
	{
		iRet = pPlayBack->StopLocalPlayBack();
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	return iRet;
}

//停止所有回放
void CPlaybackMgr::StopAllPlayBack()
{
	IVS_DEBUG_TRACE("");
	(void)VOS_MutexLock(m_pHandlePlayBackMutex);

	HandlePlayBackMapIter iter = m_HanlePlayBackMap.begin();
	HandlePlayBackMapIter iter_end = m_HanlePlayBackMap.end();
	try
	{
		for(; iter!=iter_end; iter++)
		{
			CPlayback *pPlayBack = dynamic_cast<CPlayback*>(iter->second);//lint !e611
			switch(pPlayBack->GetServiceType())
			{
			case SERVICE_TYPE_PLAYBACK:
				/*本地与平台回放无法区分，这里只调用停止平台回放，此时本地回放实际也会被停止*/
				pPlayBack->StopPlatformPlayBack();
				break;
			case SERVICE_TYPE_BACKUPRECORD_PLAYBACK:
				pPlayBack->StopBackupPlayBack();
				break;
			case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK:
				pPlayBack->StopPlatformPlayBack();
				break;
			case SERVICE_TYPE_PU_PLAYBACK:
				pPlayBack->StopPUPlayBack();
				break;
			default:
				break;
			}
			IVS_DELETE(pPlayBack);
		}
	}
	catch (...)
	{
	}

	m_HanlePlayBackMap.clear();
	(void)VOS_MutexUnlock(m_pHandlePlayBackMutex);
}


// 设置视频质量
IVS_INT32 CPlaybackMgr::SetVideoEffect(IVS_ULONG ulPlayHandle, IVS_LONG lBrightValue, 
	IVS_LONG lContrastValue, IVS_LONG lSaturationValue, 
	IVS_LONG lHueValue)
{
	IVS_INFO_TRACE("Playhandle: %lu, BrightValue: %d, ContrastValue: %d, SaturationValue: %d, HueValue: %d",
		ulPlayHandle, lBrightValue, lContrastValue, lSaturationValue, lHueValue);

	if (0 > lBrightValue || 255 < lBrightValue || 0 > lContrastValue || 255 < lContrastValue || 
		0 > lSaturationValue || 255 < lSaturationValue || 0 > lHueValue || 255 < lHueValue)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Set Video Effect", "Parameter is Invalid");
		return IVS_PARA_INVALID;
	}

	IVS_INT32 iRet = IVS_FAIL;
	CPlayback *pPlayBack = GetPlayBack(ulPlayHandle);
	if (NULL != pPlayBack && (MEDIA_STATUS_BUSY == pPlayBack->GetStatus()))
	{
		iRet = pPlayBack->SetVideoEffect(lBrightValue, lContrastValue, lSaturationValue, lHueValue);
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
		BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED, "Set Video Effect", "Playhandle is Invalid");
	}

	return iRet;
}