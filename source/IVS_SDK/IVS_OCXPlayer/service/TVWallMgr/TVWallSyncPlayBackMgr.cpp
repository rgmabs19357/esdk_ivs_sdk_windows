#include <stdafx.h>
#include "ToolsHelp.h"
#include "TVWallSyncPlayBackMgr.h"
#include "VideoPane.h"
#include "IVS_SDKTVWall.h"
#include "IVS_SDK.h"
#include "IVSCommon.h"

CTVWallSyncTask::CTVWallSyncTask(ULONG ulTVwallID,
	IVS_INT32 uiSessionID,
	CVideoPane* pVideoPane,
	const IVS_CHAR *pCamID, 
	const IVS_PLAYBACK_PARAM *pPlaybackParam, 
	const IVS_TVWALL_PARAM* pTVWallParam,
	const REC_TIME_SPAN& timeSpan)
{
	m_TVwallID = ulTVwallID;	
	m_SessionID = uiSessionID;
	memset(m_CameraID,0,IVS_DEV_CODE_LEN+1);
	(void)CToolsHelp::Strncpy(m_CameraID,IVS_DEV_CODE_LEN,pCamID,strlen(pCamID));
	(void)CToolsHelp::Memcpy((void*)&m_PlayBackParam,sizeof(IVS_PLAYBACK_PARAM),(void*)pPlaybackParam,sizeof(IVS_PLAYBACK_PARAM));
	(void)CToolsHelp::Memcpy((void*)&m_TVWallParam,sizeof(IVS_TVWALL_PARAM),(void*)pTVWallParam,sizeof(IVS_TVWALL_PARAM));
	m_llStartTime = (unsigned long long)(timeSpan.Start*1000);
	m_llEndTime = (unsigned long long)(timeSpan.End*1000);
	m_bIsHasOnTVWall = false;
	m_llCurTime = 0;
	m_ulCurWndID = (pVideoPane != NULL)?(pVideoPane->GetPaneID()):0;
	m_pVideoPane = pVideoPane;
}
CTVWallSyncTask::~CTVWallSyncTask()
{
	m_TVwallID = 0;
	m_SessionID = -1;
	m_llStartTime = 0;
	m_llEndTime = 0;
	m_llCurTime = 0;
	m_bIsHasOnTVWall = false;
	m_pVideoPane = NULL;
	memset(m_CameraID,0,IVS_DEV_CODE_LEN+1);
}
IVS_INT32 CTVWallSyncTask::StartTVWallPlayBack()
{
	if(NULL == m_pVideoPane)
	{
		return IVS_FAIL;
	}
	if(!m_bIsHasOnTVWall)
	{
		m_bIsHasOnTVWall = true;
		m_pVideoPane->SetTVWallStatus(TVWALL_STATUS_START);

        BP_DBG_LOG("CTVWallSyncTask::StartTVWallPlayBack: m_CameraID = %s, uiChannel = %d, WndID = %d, uiProtocolType = %d.",
            m_CameraID, m_TVWallParam.uiChannel, GetCurWndID(), m_PlayBackParam.uiProtocolType);

		IVS_INT32 iRet = IVS_SDK_StartPlatformPlayBackTVWall(m_SessionID, m_CameraID, &m_PlayBackParam, &m_TVWallParam);
		//记录每一次上墙的日志
		if(IVS_SUCCEED == iRet)
		{
			BP_RUN_LOG_INF("Start TVWall SyncPlayBack Task Success.","");
			m_pVideoPane->SetTVWallStatus(TVWALL_STATUS_PLAY);
		}
		else
		{
			m_pVideoPane->SetTVWallStatus(TVWALL_STATUS_NONE);
			BP_RUN_LOG_ERR(iRet, "Start TVWall SyncPlayBack Task Failed.", "Reason:SDK Start TVWallPlayBack Failed.");
		}
		return iRet;
	}
	return IVS_SUCCEED;
	 
}
IVS_INT32 CTVWallSyncTask::StopTVWallPlayBack()
{
	if(NULL == m_pVideoPane)
	{
		return IVS_FAIL;
	}
	if(m_bIsHasOnTVWall)
	{
		m_bIsHasOnTVWall = false;
		IVS_INT32 iRet = IVS_SDK_StopPlatformPlayBackTVWall(m_SessionID, &m_TVWallParam);
        if (IVS_SDK_NOT_PLAY_TVWALL == iRet)
        {
            iRet = IVS_SUCCEED;
        }

		if(IVS_SUCCEED == iRet)
		{
			BP_RUN_LOG_INF("Stop TVWall SyncPlayBack Task Success.","");
		}
		else
		{
			BP_RUN_LOG_ERR(iRet, "Stop TVWall SyncPlayBack Task Failed.", "Reason:SDK Stop TVWallPlayBack Failed.");
		}
		m_pVideoPane->SetTVWallStatus(TVWALL_STATUS_NONE);
		return iRet;
	}
	return IVS_SUCCEED;	
}



CTVWallSyncPlayBackMgr::CTVWallSyncPlayBackMgr()
{
	m_bStart = false;
	m_iSessionID = -1;
	m_hThread    = NULL;
}
CTVWallSyncPlayBackMgr::~CTVWallSyncPlayBackMgr()
{
	if(m_bStart)
	{
		(void)StopTVwallSyncPlayBack();//lint !e1551
	}
}//lint !e1740

CTVWallSyncPlayBackMgr& CTVWallSyncPlayBackMgr::Instance()
{
	static CTVWallSyncPlayBackMgr Mgr;
	return Mgr;
}
IVS_INT32 CTVWallSyncPlayBackMgr::InsertTask(ULONG ulTVwallID, IVS_INT32 uiSessionID,CVideoPane* pVideoPane,const char *pCamID, const IVS_TVWALL_PARAM* pTVWallParam,const IVS_PLAYBACK_PARAM *pPlaybackParam,const REC_TIME_SPAN& timeSpan)
{
	CAutoLock __lock(m_TVWallMutex);
	if(NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Insert TVWallSyncTask Failed.","Reason:pVideoPane is NULL.");
		return IVS_FAIL;
	}
	CTVWallSyncTask* task = new CTVWallSyncTask(ulTVwallID,uiSessionID,pVideoPane,pCamID,pPlaybackParam,pTVWallParam,timeSpan);
	m_Task.push_back(task);
	(void)m_VideoTimeMap.insert(std::make_pair(pVideoPane,0));
	m_iSessionID = uiSessionID;
	BP_RUN_LOG_INF("Insert TVWallSyncTask Success.","PaneID:%lu,CamID:%s,Start:%llu,End:%llu.",pVideoPane->GetPaneID(),pCamID,(unsigned long long)timeSpan.Start*1000,(unsigned long long)timeSpan.End*1000);
	return IVS_SUCCEED;
}
IVS_INT32 CTVWallSyncPlayBackMgr::StartTVwallSyncPlayBack()
{
	if(0 == m_Task.size())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Start TVwallSyncPlayBack Failed.","Reason:SyncTask size is 0.")
		return IVS_FAIL;
	}
	//启动线程
	if(!m_bStart)
	{
		m_bStart = true;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, TVwallSyncScheduleProc, this, 0, NULL);			
	}

	return IVS_SUCCEED;
}
IVS_INT32 CTVWallSyncPlayBackMgr::StopTVwallSyncPlayBack()
{	
	if(!m_bStart)
	{
		BP_RUN_LOG_INF("Stop TVWall SyncPlayBack End.","INF:TVWallSyncPlayBack no run.");
		return IVS_SUCCEED;
	}
	BP_RUN_LOG_INF("Stop TVWall SyncPlayBack Started.","");
	m_bStart = false; 
	if(NULL != m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
	}
	m_hThread  = NULL;

	TVWALL_SYNC_TASK_ITER task = m_Task.begin();
	TVWALL_SYNC_TASK_ITER taskEnd = m_Task.end();
	for(;task!=taskEnd;)
	{
		CTVWallSyncTask* tvwalltask = *task;
		(void)tvwalltask->StopTVWallPlayBack();
		m_Task.erase(task++);
		IVS_DELETE(tvwalltask);
	}
	m_Task.clear();
	BP_RUN_LOG_INF("Stop TVWall SyncPlayBack Finished.","");
	return IVS_SUCCEED;
}

IVS_INT32 CTVWallSyncPlayBackMgr::StopTVWallPlayBackByWndID(ULONG ulWndID)
{
	BP_RUN_LOG_INF("Stop TVWall SyncPlayBack By WndID Started.","Wnd = %lu",ulWndID);
	CAutoLock __lock(m_TVWallMutex);
	if(!m_bStart)
	{
		BP_RUN_LOG_INF("Stop TVWall SyncPlayBack By WndID End.","Reason:thread no run.");
		return IVS_SUCCEED;
	}

	TVWALL_SYNC_TASK_ITER task = m_Task.begin();
	TVWALL_SYNC_TASK_ITER taskEnd = m_Task.end();
	for(;task!=taskEnd;)
	{
		CTVWallSyncTask* pTVWallTask = *task;
		if(pTVWallTask->GetOnTVwallStatus() && ulWndID == pTVWallTask->GetCurWndID())
		{
			(void)pTVWallTask->StopTVWallPlayBack();
			m_Task.erase(task++);
			IVS_DELETE(pTVWallTask);
			continue;
		}		
		task++;
	}
	BP_RUN_LOG_INF("Stop TVWall SyncPlayBack By WndID End.","");
	return IVS_SUCCEED;
}
unsigned long long CTVWallSyncPlayBackMgr::GetCurentTime(const CVideoPane *pVideoPane)
{
	unsigned long long tick = 0;
	if(NULL == pVideoPane)
	{
		return 0;
	}
	(void)IVS_SDK_GetCurrentFrameTick(m_iSessionID,pVideoPane->GetPlayHandle(),&tick);
	return tick;
}

//回访同步上墙调度线程
unsigned int _stdcall CTVWallSyncPlayBackMgr::TVwallSyncScheduleProc(LPVOID lpParameter)
{
	CTVWallSyncPlayBackMgr* pTVWallSyncPBMrg = (CTVWallSyncPlayBackMgr*)(lpParameter);
	if(NULL == pTVWallSyncPBMrg)
	{
		_endthreadex(0);
		return 1;
	}
	while(pTVWallSyncPBMrg->m_bStart)
	{
        if (true) //lint !e506 !e774
        {
		    CAutoLock __lock(pTVWallSyncPBMrg->m_TVWallMutex);
		    //更新所有任务时间
		    VIDEOPANE_TIME_MAP_ITER videoTimetask = pTVWallSyncPBMrg->m_VideoTimeMap.begin();
		    VIDEOPANE_TIME_MAP_ITER videoTimetaskEnd = pTVWallSyncPBMrg->m_VideoTimeMap.end();
		    for(;videoTimetask!=videoTimetaskEnd;videoTimetask++)
		    {			
			    CVideoPane* pVideoPane = videoTimetask->first;			
			    if(NULL == pVideoPane)
			    {
				    continue;
			    }
			    unsigned long long tick =  pTVWallSyncPBMrg->GetCurentTime(pVideoPane);			
			    pTVWallSyncPBMrg->m_VideoTimeMap[pVideoPane] = tick;
			    BP_RUN_LOG_INF("GetCurentTime End.","WndID:%lu,CurTime:%llu.",pVideoPane->GetPaneID(),tick);			
		    }

		    //获取当前时间
		    TVWALL_SYNC_TASK_ITER task = pTVWallSyncPBMrg->m_Task.begin();
		    TVWALL_SYNC_TASK_ITER taskEnd =  pTVWallSyncPBMrg->m_Task.end();
		    for(;task!=taskEnd;)
		    {
			    CTVWallSyncTask* pONTVWallTask = *task;
			    if(NULL == pONTVWallTask)
			    {
				    task++;
				    continue;
			    }

			    //获取当前播放窗口的时间
			    unsigned long long curtime = 0;
			    VIDEOPANE_TIME_MAP_ITER iter = pTVWallSyncPBMrg->m_VideoTimeMap.find(pONTVWallTask->GetVideoPane());
			    if(iter != videoTimetaskEnd)
			    {
				    curtime = (unsigned long long)(iter->second);				
			    }

			    //比较当前任务时间与本任务的开始和结束时间
			    unsigned long long ullStartTime = pONTVWallTask->GetStartTime();
			    unsigned long long ullEndTime = pONTVWallTask->GetEndTime();
			    if(curtime >= ullStartTime && curtime < ullEndTime)
			    {
				    //上墙
				    IVS_INT32 iRet = pONTVWallTask->StartTVWallPlayBack();
				    if(IVS_SUCCEED != iRet)
				    {
					    //上墙失败，删除任务
					    pTVWallSyncPBMrg->m_Task.erase(task++);
					    IVS_DELETE(pONTVWallTask);
					    continue;
				    }
			    }
			    else if(curtime >= ullEndTime || curtime == 0)
			    {				
				    if(pONTVWallTask->GetOnTVwallStatus())//已上墙才能触发下墙
				    {
					    //下墙
					    (void)pONTVWallTask->StopTVWallPlayBack();
					    //下墙后，删除task;
					    pTVWallSyncPBMrg->m_Task.erase(task++);
					    IVS_DELETE(pONTVWallTask);
					    continue;
				    }				
			    }
			    task++;
		    }

		    if(pTVWallSyncPBMrg->m_Task.empty())
		    {
			    break;
		    }
        } //防止死锁，把sleep放在锁的外面

		Sleep(500);//延时500ms，获取时间当前任务的时间片，以防日志打印过多
	}
	pTVWallSyncPBMrg->m_bStart = false;
	_endthreadex(0);
	return 0;
}

IVS_INT32 CTVWallSyncPlayBackMgr::PauseTVWallSyncPlayBack()
{
    if(!m_bStart)
    {
        BP_RUN_LOG_INF("Pause TVWall SyncPlayBack End.","INF:TVWallSyncPlayBack no run.");
        return IVS_SUCCEED;
    }
    BP_RUN_LOG_INF("Pause TVWall SyncPlayBack Started.","");

    CAutoLock __lock(m_TVWallMutex);

    TVWALL_SYNC_TASK_ITER task = m_Task.begin();
    TVWALL_SYNC_TASK_ITER taskEnd = m_Task.end();
    for(;task!=taskEnd;task++)
    {
        CTVWallSyncTask* tvwalltask = *task;
        if (NULL == tvwalltask)
        {
            continue;
        }

        BP_DBG_LOG("Pause TVWall SyncPlayBack: CurWndID[%d]", tvwalltask->GetCurWndID());

        int iRet = IVS_SDK_PlayBackPauseTVWall(tvwalltask->GetSessionID(), &tvwalltask->m_TVWallParam);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "Pause TVWall SyncPlayBack", "Failed.");
        }
    }

    BP_RUN_LOG_INF("Resume TVWall SyncPlayBack Finished.","");
    return IVS_SUCCEED;
}

IVS_INT32 CTVWallSyncPlayBackMgr::ResumeTVWallSyncPlayBack()
{
    if(!m_bStart)
    {
        BP_RUN_LOG_INF("Resume TVWall SyncPlayBack End.","INF:TVWallSyncPlayBack no run.");
        return IVS_SUCCEED;
    }
    BP_RUN_LOG_INF("Resume TVWall SyncPlayBack Started.","");

    CAutoLock __lock(m_TVWallMutex);

    TVWALL_SYNC_TASK_ITER task = m_Task.begin();
    TVWALL_SYNC_TASK_ITER taskEnd = m_Task.end();
    for(;task!=taskEnd;task++)
    {
        CTVWallSyncTask* tvwalltask = *task;
        if (NULL == tvwalltask)
        {
            continue;
        }

        BP_DBG_LOG("Resume TVWall SyncPlayBack: CurWndID[%d]", tvwalltask->GetCurWndID());

        int iRet = IVS_SDK_PlayBackResumeTVWall(tvwalltask->GetSessionID(), &tvwalltask->m_TVWallParam);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "Resume TVWall SyncPlayBack", "Failed.");
        }
    }

    BP_RUN_LOG_INF("Resume TVWall SyncPlayBack Finished.","");
    return IVS_SUCCEED;
}

bool CTVWallSyncPlayBackMgr::IsTVWallSyncPlayBack(ULONG ulWndID)
{
    if(!m_bStart)
    {
        BP_RUN_LOG_INF("Is TVWall SyncPlayBack End.","INF:TVWallSyncPlayBack no run.");
        return false;
    }
    BP_RUN_LOG_INF("Is TVWall SyncPlayBack Started.","");

    CAutoLock __lock(m_TVWallMutex);

    TVWALL_SYNC_TASK_ITER task = m_Task.begin();
    TVWALL_SYNC_TASK_ITER taskEnd = m_Task.end();
    for(;task!=taskEnd;task++)
    {
        CTVWallSyncTask* tvwalltask = *task;
        if (NULL == tvwalltask)
        {
            continue;
        }

        BP_DBG_LOG("Is TVWall SyncPlayBack: CurWndID[%d]", tvwalltask->GetCurWndID());

        if (tvwalltask->GetCurWndID() == ulWndID)
        {
            BP_RUN_LOG_INF("Is TVWall SyncPlayBack Finished.","");
            return true;
        }
    }

    BP_RUN_LOG_INF("Is TVWall SyncPlayBack Finished.","");
    return false;
}
