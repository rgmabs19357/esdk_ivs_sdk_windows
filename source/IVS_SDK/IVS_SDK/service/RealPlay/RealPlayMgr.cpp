#include "RealPlayMgr.h"
#include "UserMgr.h"
#include "bp_log.h"
#include "IVS_Trace.h"

CRealPlayMgr::CRealPlayMgr(void)
{
	m_pUserMgr = NULL;
	m_pHanleRealPlayMutex = VOS_CreateMutex();
}


CRealPlayMgr::~CRealPlayMgr(void)
{
	m_pUserMgr = NULL;
	if (NULL != m_pHanleRealPlayMutex)
	{
		(void)VOS_DestroyMutex(m_pHanleRealPlayMutex);
		m_pHanleRealPlayMutex = NULL;
	}
}//lint !e1740


void CRealPlayMgr::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr = pUserMgr;
}

//获取空闲的realplay对象;
CRealPlay *CRealPlayMgr::GetFreeRealPlay(IVS_ULONG& ulHandle)
{
	IVS_DEBUG_TRACE("");
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.begin();
    HandleRealPlayMapIter realplayIterEnd = m_HanleRealPlayMap.end();

	//查找空闲对象;
	for (; realplayIter != realplayIterEnd; realplayIter++)
	{
        pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second);//lint !e611
		if (NULL != pRealPlay)
		{
			if (pRealPlay->GetStatus() == MEDIA_STATUS_FREE)
			{
				break;
			}
		}
		pRealPlay = NULL;
	}

	if (NULL != pRealPlay)
	{
		m_HanleRealPlayMap.erase(realplayIter);
	}
	else
	{
		pRealPlay = IVS_NEW((CRealPlay*&)pRealPlay);
		if (NULL == pRealPlay)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Free RealPlay", "create real play error");
			(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
			return NULL;
		}
	}

	//初始化realplay对象;
	pRealPlay->SetUserMgr(m_pUserMgr);
	pRealPlay->SetStatus(MEDIA_STATUS_BUSY);

	//插入列表;
    ulHandle = (unsigned long)pRealPlay;
	(void)m_HanleRealPlayMap.insert(std::make_pair(ulHandle, pRealPlay));

	pRealPlay->SetHandle(ulHandle);

	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return pRealPlay;
}


CRealPlay *CRealPlayMgr::GetFreeRealPlayEx(IVS_ULONG& ulHandle)
{
	CRealPlay *pRealPlay = NULL;
	pRealPlay = IVS_NEW((CRealPlay*&)pRealPlay);
	if (NULL == pRealPlay)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Free RealPlayEx", "create real play error");
		return NULL;
	}

	BP_DBG_LOG("Get Free RealPlayEx", "CreateHandle %u", (unsigned long)pRealPlay);
	
	ulHandle = (unsigned long)pRealPlay;
	pRealPlay->SetHandle(ulHandle);
	pRealPlay->SetUserMgr(m_pUserMgr);

	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	(void)m_HanleRealPlayMap.insert(std::make_pair(ulHandle, pRealPlay));
	(void)pRealPlay->GetRef();
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	return pRealPlay;
}


//开始实况
IVS_INT32 CRealPlayMgr::StartRealPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, HWND hWnd, IVS_ULONG* pHandle)
{
	IVS_DEBUG_TRACE("");

    if (NULL == pRealplayPara || NULL == pHandle)
    {
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Real Play", "Para Invalid");
        return IVS_PARA_INVALID;
    }

	IVS_INT32 iRet = IVS_FAIL;
	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
    
	//初始化realplay对象;
	CRealPlay *pRealPlay = GetFreeRealPlayEx(ulHandle);
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StartRealPlay(pRealplayPara, pCameraCode, hWnd);
		if (IVS_SUCCEED != iRet)
		{
			FreeRealPlayEx(ulHandle);
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

//根据句柄获取realplay;
CMediaBase *CRealPlayMgr::GetMediaBase(IVS_ULONG ulHandle)
{
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CMediaBase *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.begin();
    HandleRealPlayMapIter realplayIterEnd = m_HanleRealPlayMap.end();

	//查找空闲对象;
	for (; realplayIter != realplayIterEnd; realplayIter++)
	{
		if (realplayIter->first == ulHandle)
		{
            pRealPlay = realplayIter->second;
			break;
		}
	}

	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return pRealPlay;
}
//停止实况
IVS_INT32 CRealPlayMgr::StopRealPlay(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;

	if (NULL != m_pUserMgr)
	{
		m_pUserMgr->GetAutoResume().RemoveStopPlayHandle(ulHandle);
	}

	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StopRealPlay();
    	FreeRealPlayEx(ulHandle);
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}

	return iRet;
}

//添加播放窗口
IVS_INT32 CRealPlayMgr::AddPlayWindow(IVS_ULONG ulHandle, HWND hWnd)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->AddPlayWindow(hWnd);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}
//删除播放窗口
IVS_INT32 CRealPlayMgr::DelPlayWindow(IVS_ULONG ulHandle, HWND hWnd)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->DelPlayWindow(hWnd);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//增加局部图像播放窗口
IVS_INT32 CRealPlayMgr::AddPlayWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->AddPlayWindowPartial(hWnd, pPartial);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//更新局部放大源矩形
IVS_INT32 CRealPlayMgr::UpdateWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->UpdateWindowPartial(hWnd, pPartial);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//获取通道号(如果失败返回-1);
IVS_UINT32 CRealPlayMgr::GetPlayerChannelByHandle(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
	IVS_UINT32 uiChannel = SDK_INVALID_HANDLE;

	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter;

	//查找空闲对象;
	for (realplayIter = m_HanleRealPlayMap.begin(); realplayIter != m_HanleRealPlayMap.end(); realplayIter++)
	{
		if (realplayIter->first == ulHandle)
		{
            pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second);//lint !e611
			if (NULL != pRealPlay)
			{
				uiChannel = pRealPlay->GetPlayerChannel();
			}
			break;
		}
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	return uiChannel;
}

//根据句柄获取CameraCode;
void CRealPlayMgr::GetCameraCodeByHandle(IVS_ULONG ulHandle, std::string& strCameraCode)
{
	//char *pCameraCode = NULL;
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.find(ulHandle);
	if (realplayIter != m_HanleRealPlayMap.end())
	{
		pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second); //lint !e611
		// mod by z00193167 未释放锁
		if (NULL == pRealPlay)
		{
			(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
			return;
		}
		strCameraCode = pRealPlay->GetCameraCode();
		(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
		return;
	}

	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
}

//检查窗口是否存在;
IVS_ULONG CRealPlayMgr::GetHandleByHwnd(HWND hWnd)
{
    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.begin();
    HandleRealPlayMapIter realplayIterEnd = m_HanleRealPlayMap.end();

	//查找空闲对象;
	for (;realplayIter != realplayIterEnd; realplayIter++)
	{
        pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second);//lint !e611
		if (NULL != pRealPlay && pRealPlay->IsExistHWnd(hWnd))
		{
			ulHandle = realplayIter->first;
			break;
		}
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	return ulHandle;
}//lint !e818

//获取通道号和窗口句柄;
//IVS_INT32 CRealPlayMgr::GetPortHwndByHandle(IVS_INT32 iHandle, IVS_INT32 &iPort, HWND &hWnd)
//{
//	IVS_INT32 iIsFind = SDK_ERR_FINDVIDEOHANDLE_FAILED;
//	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
//	CRealPlay *pRealPlay = NULL;
//	HandleRealPlayMapIter realplayIter;
//
//	//查找空闲对象;
//	for (realplayIter = m_HanleRealPlayMap.begin(); realplayIter != m_HanleRealPlayMap.end(); realplayIter++)
//	{
//		if (realplayIter->first == iHandle)
//		{
//			pRealPlay = realplayIter->second;
//			if (NULL != pRealPlay)
//			{
//				iPort = pRealPlay->GetPort();
//				hWnd = pRealPlay->GetHWnd();
//				iIsFind = SDK_CLIENT_SUCC;
//			}
//			break;
//		}
//	}
//	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
//
//	return iIsFind;
//}

//处理MP的流异常回调;
//void CRealPlayMgr::DealException(IVS_INT32 iPort, IVS_INT32 iMsgType, void* pHandle)
//{
//	IVS_INT32 iHandle = GetHandleByPort(iPort);
//	if (SDK_INVALID_HANDLE != iHandle)
//	{
//		if (NULL == m_pUserMgr)
//		{
//			BP_RUN_LOG_ERR("DealException", "deal exception error, user obj is null");
//			return;
//		}
//
//		EventCallBack fnCallBack = m_pUserMgr->GetEventCallBack();
//		if (NULL == fnCallBack)
//		{
//			BP_RUN_LOG_ERR("DealException", "deal exception error, fn call back is null");
//			return;
//		}
//
//		SOURCE_EXCEPTION_DATA* pExceptionData = (SOURCE_EXCEPTION_DATA*)pHandle;
//		if (NULL == pExceptionData)
//		{
//			BP_RUN_LOG_ERR("DealException", "deal exception error, exception data is null");
//			return;
//		}
//
//#ifdef WIN32
//		//m_pUserMgr->StopSerialCapture( iPort );
//#endif
//
//		STREAM_EXCEPTION_INFO stuExceptionInfo;
//		memset(&stuExceptionInfo, 0, sizeof(STREAM_EXCEPTION_INFO));
//		stuExceptionInfo.iSessionID = m_pUserMgr->GetSessionID();
//		stuExceptionInfo.iHandle = iHandle;
//		stuExceptionInfo.iExceptionCode = (int)pExceptionData->iExceptionCode;
//		fnCallBack(EVENT_REALPLAY_FAILED, &stuExceptionInfo, sizeof(STREAM_EXCEPTION_INFO), m_pUserMgr->GetEventUserData());
//
//		BP_RUN_LOG_INF("DealException", "deal real play exception, exception code is %d", stuExceptionInfo.iExceptionCode);
//	}
//}

//开始实况(裸码流);
IVS_INT32 CRealPlayMgr::StartRealPlayCBRaw(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackRaw fRealPlayCallBackRaw, void* pUserData, IVS_ULONG* pHandle )
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);
	IVS_INT32 iRet = IVS_FAIL;

	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
	CRealPlay *pRealPlay = GetFreeRealPlay(ulHandle);
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StartRealPlayCBRaw(pRealplayPara, pCameraCode, fRealPlayCallBackRaw, pUserData);
		if (IVS_SUCCEED != iRet)
		{
			FreeRealPlay(ulHandle);
			//pRealPlay->RetSet();
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

//开始实况(YUV流);
IVS_INT32 CRealPlayMgr::StartRealPlayCBFrame(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData, IVS_ULONG* pHandle )
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);

	IVS_INT32 iRet = IVS_FAIL;
	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
	CRealPlay *pRealPlay = GetFreeRealPlay(ulHandle);
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StartRealPlayCBFrame(pRealplayPara, pCameraCode, fRealPlayCallBackFrame, pUserData);
		if (IVS_SUCCEED != iRet)
		{
			FreeRealPlay(ulHandle);
			//pRealPlay->RetSet();
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

//关闭所有实况;
void CRealPlayMgr::StopAllRealPlay()
{
	IVS_DEBUG_TRACE("");
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);

	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter realplayIterEnd = m_HanleRealPlayMap.end();

	//查找空闲对象;
	for (;realplayIter != realplayIterEnd; realplayIter++)
	{
		pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second);//lint !e611
		if (NULL != pRealPlay)
		{
			try
			{
				if (pRealPlay->GetStatus() == MEDIA_STATUS_BUSY)
				{
					(void)pRealPlay->StopRealPlay();
				}
			}
			catch(...)
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "StopAllRealPlay", "Delete realplay[%lu] failed.", pRealPlay);
			}

			try
			{
				IVS_DELETE(pRealPlay);
			}
			catch(...)
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "StopAllRealPlay", "Delete realplay[%lu] failed.", pRealPlay);
			}
			
		}
		pRealPlay = NULL;
	}
	m_HanleRealPlayMap.clear();
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
}

//获取实况句柄
IVS_ULONG CRealPlayMgr::GetPlayHandlebyRtspHandle(int iRtspHandle)
{
	IVS_ULONG ulPlayHandle = 0;
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		CRealPlay* pRealPlay = dynamic_cast<CRealPlay*>(iter->second);//lint !e611
		if (NULL == pRealPlay)
		{
			continue;
		}
		int iRtspHandleTemp = pRealPlay->GetRtspHandle();
		if (iRtspHandleTemp == iRtspHandle)
		{
			ulPlayHandle = iter->first;
			break;
		}
	}
	
	return ulPlayHandle;
}


IVS_ULONG CRealPlayMgr::GetPlayHandlebyRtspHandleEx(int iRtspHandle)
{
	IVS_ULONG ulPlayHandle = 0;

    (void)VOS_MutexLock(m_pHanleRealPlayMutex);
	
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		CRealPlay* pRealPlay = iter->second;
		if (NULL == pRealPlay)
		{
			continue;
		}

		if (iRtspHandle == pRealPlay->GetRtspHandle())
		{
			ulPlayHandle = iter->first;
			(void)pRealPlay->GetRef();
			break;
		}
	}

	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return ulPlayHandle;
}



//获取实况句柄
IVS_ULONG CRealPlayMgr::GetPlayHandleByNetSourceChannel(unsigned long ulNetSourceChannel)
{
    IVS_ULONG ulPlayHandle = 0;
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		CRealPlay* pRealPlay = dynamic_cast<CRealPlay*>(iter->second);//lint !e611
		if (NULL == pRealPlay)
		{
			continue;
		}
		unsigned long ulNetSourceHandleTemp = pRealPlay->GetNetSourceHandle();
		if (ulNetSourceHandleTemp == ulNetSourceChannel)
		{
			ulPlayHandle = iter->first;
			break;
		}
	}

	return ulPlayHandle;
}
//根据播放信道获取播放句柄
IVS_ULONG CRealPlayMgr::GetPlayHandleByPlayChannel(unsigned long ulPlayChannel)
{
	IVS_ULONG ulPlayHandle = 0;
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		CRealPlay* pRealPlay = dynamic_cast<CRealPlay*>(iter->second);//lint !e611
		if (NULL == pRealPlay)
		{
			continue;
		}
		unsigned long ulNetSourceHandleTemp = pRealPlay->GetPlayerChannel();
		if (ulNetSourceHandleTemp == ulPlayChannel)
		{
			ulPlayHandle = iter->first;
			break;
		}
	}

	return ulPlayHandle;
}

// 播放随路语音
IVS_INT32 CRealPlayMgr::PlayAudioSound(unsigned int uiPlayHandle)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->PlaySound();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//停止播放随路语音
IVS_INT32 CRealPlayMgr::StopAudioSound(unsigned int uiPlayHandle)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->StopSound();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//设置音量
IVS_INT32 CRealPlayMgr::SetVolume(unsigned int uiPlayHandle, unsigned int uiVolumeValue)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->SetVolume(uiVolumeValue);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//获取音量
IVS_INT32 CRealPlayMgr::GetVolume(unsigned int uiPlayHandle, unsigned int* puiVolumeValue)
{	
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->GetVolume(puiVolumeValue);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

void CRealPlayMgr::FreeRealPlay(IVS_ULONG ulHandle)
{
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.find(ulHandle);
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	if (iter != iterEnd)
	{
		CRealPlay* pRealPlay = dynamic_cast<CRealPlay*>(iter->second);//lint !e611
		if (NULL != pRealPlay)
		{
			IVS_DELETE(pRealPlay);
		}
		m_HanleRealPlayMap.erase(iter++);
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
}


void CRealPlayMgr::FreeRealPlayEx(IVS_ULONG ulHandle)
{
    BP_DBG_LOG("Free Real PlayEx", "Free Handle %u", ulHandle);

	CRealPlay *pRealPlay = NULL;
	
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.find(ulHandle);
	if (iter != m_HanleRealPlayMap.end())
	{
		pRealPlay = iter->second;
		m_HanleRealPlayMap.erase(iter++);
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	if (NULL == pRealPlay)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "FreeRealPlayEx", "pRealPlay = NULL");
		return;
	}

	if (0 == pRealPlay->PutRef())
	{
		BP_DBG_LOG("Free Real PlayEx", "Delete Handle %u", ulHandle);
	    IVS_DELETE(pRealPlay);
	}
}


CRealPlay *CRealPlayMgr::GetRealPlay(IVS_ULONG ulHandle)
{
	CRealPlay *pRealPlay = NULL;
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.find(ulHandle);
	if (iter != m_HanleRealPlayMap.end())
	{
		pRealPlay = iter->second;
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return pRealPlay;
}



CRealPlay *CRealPlayMgr::GetRealPlayEx(IVS_ULONG ulHandle)
{
	CRealPlay *pRealPlay = NULL;
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.find(ulHandle);
	if (iter != m_HanleRealPlayMap.end())
	{
		pRealPlay = iter->second;
		pRealPlay->GetRef();
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return pRealPlay;
}

void CRealPlayMgr::ReleaseRealPlay(CRealPlay * pRealPlay)
{
	if (pRealPlay && 0 == pRealPlay->PutRef())
	{
		IVS_DELETE(pRealPlay);
	}
}//lint !e1762 暂不列为const





