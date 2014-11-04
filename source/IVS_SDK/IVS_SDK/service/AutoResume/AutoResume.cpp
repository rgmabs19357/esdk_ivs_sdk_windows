#include "AutoResume.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
 CAutoResume::CAutoResume(void)
 {
	m_pstAutoResumeMutex = VOS_CreateMutex();
 }
 
 CAutoResume::~CAutoResume(void)
 {
	try
	{
		clearCamerInfoList();
		if (NULL != m_pstAutoResumeMutex)
		{
			(void)VOS_DestroyMutex(m_pstAutoResumeMutex);
			m_pstAutoResumeMutex = NULL;
		} 
	}
	catch (...)
	{
		;
	}
 }
 
 // 自动恢复回调函数
void CAutoResume::onTrigger( void* /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*style*/ )
{
    IVS_DEBUG_TRACE("");
	
	CAMERA_PLAY_INFO_LIST Failed;
    while (true) //lint !e716
	{
		(void)VOS_MutexLock(m_pstAutoResumeMutex);
		if (m_CamerPlayInfoList.empty())
		{
			(void)VOS_MutexUnlock(m_pstAutoResumeMutex);
			break;
		}
		
		CAMERA_PLAY_INFO* pCameraPlayInfo = m_CamerPlayInfoList.front();
		m_CamerPlayInfoList.pop_front();
		(void)VOS_MutexUnlock(m_pstAutoResumeMutex);
     
		if (NULL == pCameraPlayInfo)
		{
			continue;
		}
 		
		CUserMgr* userMgr = (CUserMgr*)(pCameraPlayInfo->pUserData);
		if (NULL == userMgr)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "userMgr is null", "NA");
			IVS_DELETE(pCameraPlayInfo);
			continue;
		}
		
		CRealPlay *pRealPlay = (CRealPlay *)(pCameraPlayInfo->pPlayObj);
		if (NULL == pRealPlay)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "PlayObj is null", "NA");
			IVS_DELETE(pCameraPlayInfo);
			continue;
		}

		IVS_INT32 iRet = pRealPlay->ReStartRealPlay(&(pCameraPlayInfo->stMediaPara), pCameraPlayInfo->cCameraCode);
		BP_RUN_LOG_INF("ReStartRealPlay","iRet = %d", iRet);
		
		if (IVS_SUCCEED == iRet)
		{
			BP_RUN_LOG_INF("restart success","playhandle = %d", pCameraPlayInfo->uPlayHandle);
			EventCallBack fnCallBack = userMgr->GetEventCallBack();
			if (NULL != fnCallBack)
			{
				fnCallBack(IVS_EVENT_RESUME_REALPLAY_OK, (IVS_VOID*)pCameraPlayInfo->uPlayHandle, 1, userMgr->GetEventUserData());
			}
			IVS_DELETE(pCameraPlayInfo);
    		userMgr->GetRealPlayMgr().ReleaseRealPlay(pRealPlay);
		}
		else if (IVS_PARA_INVALID == iRet)
		{
			BP_RUN_LOG_INF("restart Fail","playhandle = %d, already stopped", pCameraPlayInfo->uPlayHandle);
			IVS_DELETE(pCameraPlayInfo);
			userMgr->GetRealPlayMgr().ReleaseRealPlay(pRealPlay);
		}
		else
		{
			BP_RUN_LOG_INF("restart Fail", "playhandle = %d", pCameraPlayInfo->uPlayHandle);
			Failed.push_back(pCameraPlayInfo);
		}
	}

	CAMERA_PLAY_INFO_LIST_ITER iter = Failed.begin();
    CAMERA_PLAY_INFO_LIST_ITER  end = Failed.end();
    (void)VOS_MutexLock(m_pstAutoResumeMutex);
    for (; iter != end; iter++)
    {
	    m_CamerPlayInfoList.push_back(*iter);
    }
    (void)VOS_MutexUnlock(m_pstAutoResumeMutex);
}
 
 // 清除重连列表
 void CAutoResume::clearCamerInfoList()
 {
 	BP_RUN_LOG_INF("clearCamerInfoList","Start");
 	CLockGuard lockGuard(m_pstAutoResumeMutex);
 	CAMERA_PLAY_INFO_LIST_ITER Listiter = m_CamerPlayInfoList.begin();
 	while (m_CamerPlayInfoList.end() != Listiter)
 	{
 		CAMERA_PLAY_INFO* lpCameraInfo= *Listiter;
		IVS_DELETE(lpCameraInfo);
 		++Listiter;
 	}
 	m_CamerPlayInfoList.clear();

 	BP_RUN_LOG_INF("clearCamerInfoList","Leave");
 }

// 增加一个列表到已有的列表中,如果已经存在，先删除存在的
bool CAutoResume::AddCameraPlayInfo2List(const CAMERA_PLAY_INFO& cameraPlayInfo)
{
	// 查找是否有相同的handle的信息存在，如果存在就移除
	RemoveStopPlayHandle(cameraPlayInfo.uPlayHandle);
	CLockGuard lockGuard(m_pstAutoResumeMutex);
	// 加入到列表中
	CAMERA_PLAY_INFO* pCameraPlayInfo = IVS_NEW(pCameraPlayInfo);
	if (!CToolsHelp::Memcpy(pCameraPlayInfo, sizeof(CAMERA_PLAY_INFO), &cameraPlayInfo, sizeof(CAMERA_PLAY_INFO)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
		return false;
	}

	BP_RUN_LOG_INF("AddCameraPlayInfo2List success", "palyhandle = %d", cameraPlayInfo.uPlayHandle);
	m_CamerPlayInfoList.push_back(pCameraPlayInfo);

	return true;//lint !e429
}

void CAutoResume::RemoveStopPlayHandle(IVS_ULONG ulHandle)
{
	CLockGuard lockGuard(m_pstAutoResumeMutex);
	CAMERA_PLAY_INFO_LIST_ITER iter = m_CamerPlayInfoList.begin();
	for (;iter != m_CamerPlayInfoList.end();)
	{
	   	CAMERA_PLAY_INFO* pCameraPlayInfoWnd = *iter;
		if (NULL == pCameraPlayInfoWnd)
		{
			BP_RUN_LOG_INF("pCameraPlayInfoWnd is null","NA");
			m_CamerPlayInfoList.erase(iter++);
			continue;
		}
	   	if (pCameraPlayInfoWnd->uPlayHandle == ulHandle)
	   	{
			BP_RUN_LOG_INF("CAutoResume::RemoveStopPlayHandle", "palyhandle = %d", ulHandle);
			IVS_DELETE(pCameraPlayInfoWnd);
			m_CamerPlayInfoList.erase(iter++);
			break;
	   	}
		iter++;
	}
}
