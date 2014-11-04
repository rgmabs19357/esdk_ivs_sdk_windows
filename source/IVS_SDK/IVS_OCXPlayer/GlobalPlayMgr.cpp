#include "stdafx.h"
#include "GlobalPlayMgr.h"
#include "IVS_OCXPlayerCtrl.h"
#include "VideoPane.h"
#include "bp_log.h"
#include "OCXRecordXMLProcess.h"
#include "OCXPlayBackXMLProcess.h"
#include "OCXAlarmMgrXMLProcess.h"
#include "SDKDef.h"
#include "SyncPlayBackMgr.h"

#define INVALID_PLAY_HANDLE ((unsigned long)NULL)

//lint -e831 831错误不需要清楚
// 按钮状态
CPlayObject::CPlayObject(unsigned long handle, bool bRealStream)
    : m_ref(0),m_bRealStream(bRealStream),m_uStreamType(0),m_ulHandle(handle)
{
    m_PanelState.bRecording = false;
    m_PanelState.PlayState = VIDEO_PLAY_STATE_NOT_PLAYING;
    m_PanelState.Volume = 50;
    memset(m_CamID, 0, sizeof(m_CamID));
}

CPlayObject::~CPlayObject()
{
    //do clean
}

// SetCamID
// 设置摄像头ID字符串
void CPlayObject::SetCamID(const TCHAR *szCamID)
{
    if ((NULL == szCamID) || ('\0' == szCamID[0]))
    {
        BP_RUN_LOG_INF("Set Camera ID", "CamID Empty");
        return;
    }

    (void)CToolsHelp::Strncpy(m_CamID, IVS_DEV_CODE_LEN + 1, szCamID, IVS_DEV_CODE_LEN);
}

// InsertPanel
// 插入播放panel
void CPlayObject::InsertPanel(CVideoPane *pPanel)
{
    PANEL_ITER iter = m_Panels.begin();
    PANEL_ITER end = m_Panels.end();

    // 检查是否重复
    for (; iter != end; iter++)
    {
        if (*iter == pPanel)
        {
            BP_RUN_LOG_INF("PlayObject InsertPanel", "Conflict Panel");
            ASSERT(0);
            return; //lint !e527
        }
    }

    m_Panels.push_back(pPanel);
    m_ref++;
}

//DeletePanel
//删除播放窗口
void CPlayObject::DeletePanel(CVideoPane *pPanel)
{
    PANEL_ITER iter = m_Panels.begin();
    PANEL_ITER end = m_Panels.end();

    for (; iter != end; iter++)
    {
        if (*iter == pPanel)
        {
            (void)m_Panels.erase(iter);
            m_ref--;
            return;
        }
    }

    //BP_RUN_LOG_INF("PlayObject DeletePanel", "No Panel found");
    //ASSERT(0);
} //lint !e818

//CGlobalPlayMgr
CGlobalPlayMgr::CGlobalPlayMgr()
{
    m_bStreamShare = true;
    m_bInited   = false;
    m_ScaleType = 0;
}

// ~CGlobalPlayMgr
CGlobalPlayMgr::~CGlobalPlayMgr()
{
    // 进程结束，存在为关闭的播放句柄，需要注意
    try
    {
        ASSERT(!m_RealStreamList.size());
        ASSERT(!m_HandleMap.size());

        PLAY_HANDLE_ITER iter = m_HandleMap.begin();
        PLAY_HANDLE_ITER end = m_HandleMap.end();
        for (; iter != end; iter++)
        {
            CPlayObject *pObject = iter->second;
            ASSERT(NULL != pObject);
            if (NULL != pObject) //lint !e774
            {
                delete pObject;
            }
        }

        m_bInited = false;
    }
    catch (...)
    {}
}

int CGlobalPlayMgr::Init()
{
    m_bInited = true;
    return IVS_SUCCEED;
}

void CGlobalPlayMgr::CleanUp()const
{
    // do nothing
}

// RegPlayHandle
// 注册播放句柄
int CGlobalPlayMgr::RegRealPlayHandle(unsigned long Handle,
                                      CVideoPane *pPanel, const TCHAR *szCam, unsigned int StreamType)
{
    ASSERT(0 != Handle && NULL != pPanel && NULL != szCam);//lint !e831
    if ((0 == Handle) || (NULL == pPanel) || (NULL == szCam))  //lint !e774 !e831
    {
        BP_RUN_LOG_INF("Reg RealPlay Handle", "Param Invalid");
        return 0; // 返回引用参数0
    }

    BP_RUN_LOG_INF("Reg RealPlay Handle",
                   "Handle:%08x Panel:%08x WndID:%d, CamID:%s StreamType:%u",
                   Handle, pPanel, pPanel->GetPaneID(), szCam, StreamType);

    CAutoLock lock(m_Mutex);

    CPlayObject *pPlayObj = NULL;
    PLAY_HANDLE_ITER iter = m_HandleMap.find(Handle);
    if (iter != m_HandleMap.end())
    {
        pPlayObj = iter->second;
        ASSERT(pPlayObj);
    }

    if (NULL == pPlayObj)
    {
        try
        {
            pPlayObj = new CPlayObject(Handle, true);
        }
        catch (...)
        {
            BP_RUN_LOG_INF("Reg PlayHandle", "No Mem");
            return 0;
        }
    }

    pPlayObj->SetStreamType(StreamType);
    pPlayObj->SetCamID(szCam);
    pPlayObj->InsertPanel(pPanel);
    pPlayObj->m_PanelState.PlayState = VIDEO_PLAY_STATE_REALTIME;

    (void)m_HandleMap.insert(std::make_pair(Handle, pPlayObj));

    if ((NULL != szCam) && (pPlayObj->GetRef() == 1)) //lint !e774 !e831 该摄像头的第一路视频流
    {
        CAM_REAL_STREAM Cam;
        Cam.playHandle = Handle;
        Cam.StreamType = StreamType;
        (void)CToolsHelp::Strncpy(Cam.CamID, IVS_DEV_CODE_LEN + 1, szCam, IVS_DEV_CODE_LEN);
        m_RealStreamList.push_back(Cam);
    }

    return pPlayObj->GetRef();
}

// RegPlayBackHandle
// 注册回放句柄
int CGlobalPlayMgr::RegPlayBackHandle(unsigned long Handle,
                                      CVideoPane *pPanel, const TCHAR *szCam)
{
    ASSERT(0 != Handle && NULL != pPanel && NULL != szCam);//lint !e831
    if ((0 == Handle) || (NULL == pPanel) || (NULL == szCam))  //lint !e774 !e831
    {
        BP_RUN_LOG_INF("Reg Play Back Handle", "Param Invalid");
        return 0; // 返回引用参数0
    }

    BP_RUN_LOG_INF("Reg Play Back Handle", "Handle:%08x Panel:%08x WndID:%lu CamID:%s",
                   Handle, pPanel, pPanel->GetPaneID(), szCam);

    CAutoLock lock(m_Mutex);
    CPlayObject *pPlayObj = NULL;
    try
    {
        pPlayObj = new CPlayObject(Handle, false);
    }
    catch (...)
    {
        BP_RUN_LOG_INF("Reg Play Back Handle", "No Mem");
        return 0;
    }

    pPlayObj->SetCamID(szCam); // for debug
    pPlayObj->InsertPanel(pPanel);

	PLAY_HANDLE_ITER HandleIter = m_HandleMap.find(Handle);
	if (HandleIter != m_HandleMap.end())
	{
		CPlayObject *p = HandleIter->second;
		if (NULL != p)
		{
			delete p;
			p = NULL;
		}
		m_HandleMap.erase(HandleIter);
		BP_RUN_LOG_INF("Reg Play Back Handle", "Del exist handle");
	}

    (void)m_HandleMap.insert(std::make_pair(Handle, pPlayObj));
   	
	return pPlayObj->GetRef();  //lint !e429 插入链表中
}

// UnRegPlayHandle
// 去注册播放句柄
int CGlobalPlayMgr::UnRegPlayHandle(unsigned long Handle, CVideoPane *pPanel)
{
    CAutoLock lock(m_Mutex);

    CPlayObject *pPlayObj = NULL;
    PLAY_HANDLE_ITER HandleIter = m_HandleMap.find(Handle);

    if (HandleIter != m_HandleMap.end())
    {
        pPlayObj = HandleIter->second;
        ASSERT(pPlayObj);
    }

    ASSERT(NULL != pPlayObj);
    if (NULL == pPlayObj)  //lint !e774
    {
        BP_RUN_LOG_INF("UnReg Play Handle", "Invalid Handle Input");
        return 0;  // 返回引用计数为0
    }

    pPlayObj->DeletePanel(pPanel);
    if (!pPlayObj->GetRef()) // 引用计数为0
    {
        if (pPlayObj->m_bRealStream) // 处理最后一路实况
        {
            REAL_CAM_ITER iter = m_RealStreamList.begin();
            REAL_CAM_ITER end = m_RealStreamList.end();
            for (; iter != end; iter++)
            {
                CAM_REAL_STREAM &Cam = *iter;
                if (Cam.playHandle == Handle)
                {
                    (void)m_RealStreamList.erase(iter++);
                    break;
                }
            }
        }

        (void)m_HandleMap.erase(HandleIter);
        delete (pPlayObj);
        return 0;
    }

    return pPlayObj->GetRef();
}

// TryRealStreamShare
// 尝试复用流
unsigned long CGlobalPlayMgr::TryRealStreamShare(const TCHAR *szCamID,
                                                 unsigned int StreamType, CVideoPane *pPanel)
{
    if (!m_bStreamShare)
    {
        return INVALID_PLAY_HANDLE;
    }

    ASSERT(szCamID && szCamID[0]);//lint !e831
    if ((NULL == szCamID) || ('\0' == szCamID[0])) //lint !e774 !e831
    {
        BP_RUN_LOG_INF("Try RealStream Share", "Invalid CameraID");
        return INVALID_PLAY_HANDLE; // Invalid handle
    }

    BP_RUN_LOG_INF("Try RealStream Share",
                   "Try To Share RealStream: OCX:%08x Panel:%08x, WinID:%08x",
                   pPanel->GetOCXPlayerCtrl(), pPanel, pPanel->GetPaneID());

    CAutoLock lock(m_Mutex);

    unsigned long Handle = INVALID_PLAY_HANDLE;

    REAL_CAM_ITER iter = m_RealStreamList.begin();
    REAL_CAM_ITER end = m_RealStreamList.end();

    //REAL_CAM_ITER iter   = m_RealStreamMap.find(szCamID);
    for (; iter != end; iter++)
    {
        CAM_REAL_STREAM &cam = *iter;
        if (!strcmp(szCamID, cam.CamID))
        {
            if (cam.StreamType == StreamType)
            {
                Handle = cam.playHandle;
                break;
            }

            if (STREAM_TYPE_UNSPECIFIED == StreamType)
            {
                Handle = cam.playHandle;
                if (STREAM_TYPE_MAIN == cam.StreamType)
                {
                    break;
                }
            }
        }
    }

    if (INVALID_PLAY_HANDLE != Handle)
    {
        CPlayObject *pPlayObj = NULL;
        PLAY_HANDLE_ITER HandleIter = m_HandleMap.find(Handle);
        ASSERT(HandleIter != m_HandleMap.end());
        if (HandleIter != m_HandleMap.end())
        {
            pPlayObj = HandleIter->second;
            ASSERT(pPlayObj);
        }

        if (NULL == pPlayObj)
        {
            BP_RUN_LOG_INF("Try RealStream Share",
                           "No PlayObject with PlayHandle %08x", Handle);
            return INVALID_PLAY_HANDLE; // invalid handle
        }

        pPlayObj->InsertPanel(pPanel);

        // sync button state FIX me!!!

        CTranVideoRealtimeBar *pBar = pPanel->GetVideoRealtimeBar();
        if (NULL != pBar)
        {
            pBar->SetRecordBtnState(pPlayObj->m_PanelState.bRecording);
        }

        pPanel->SetIsLocalRecord(pPlayObj->m_PanelState.bRecording);
    }

    return Handle;
}

int CGlobalPlayMgr::RegDownLoadHandle(unsigned long Handle)
{
    CAutoLock lock(m_Mutex);
    DOWNLOAD_ITER iter = m_DownLoads.begin();
    DOWNLOAD_ITER end = m_DownLoads.end();

    for (; iter != end; iter++)
    {
        if (Handle == *iter)
        {
            break;
        }
    }
	//lint !e830
    ASSERT(iter == end);
    m_DownLoads.push_back(Handle);

    return 1; // ref = 1
}

void CGlobalPlayMgr::UnRegDownLoadHandle(unsigned long Handle)
{
    CAutoLock lock(m_Mutex);
    DOWNLOAD_ITER iter = m_DownLoads.begin();
    DOWNLOAD_ITER end = m_DownLoads.end();

    for (; iter != end; iter++)
    {
        if (Handle == *iter)
        {
            m_DownLoads.erase(iter);
            return;
        }
    }

    ASSERT(0);
}

/*------------------------------------------------------------------------------------------*/

void CGlobalPlayMgr::PostWaterAlarmEvent(CIVS_OCXPlayerCtrl *pOcxCtrl, long EventID, IVS_ALARM_WATERMARK_INFO *pInfo)const//lint !e830
{
    CXml xml;
    //IVS_INT32 iRet = IVS_FAIL;

    switch (EventID)
    {
    case IVS_EVENT_REPORT_WATERMARK_TIP:
        if (IVS_SUCCEED == COCXAlarmMgrXMLProcess::GetWaterTIPReportInfoXML(pInfo, xml))
        {
            unsigned int uiLength = 0;
            const TCHAR* pszXML = xml.GetXMLStream(uiLength);
            pOcxCtrl->IVS_OCX_Windows_Event(IVS_EVENT_REPORT_WATERMARK_TIP, (LPCTSTR)pszXML);
        }

        break;
    case IVS_EVENT_REPORT_WATERMARK_ALARM:
        if (IVS_SUCCEED == COCXAlarmMgrXMLProcess::GetWaterAlarmReportInfoXML(pInfo, xml))
        {
            unsigned int uiLength = 0;
            const TCHAR* pszXML = xml.GetXMLStream(uiLength);
            BP_RUN_LOG_INF("PostWaterAlarmEvent", "pszXML : %s.", pszXML);
            pOcxCtrl->IVS_OCX_Windows_Event(IVS_EVENT_REPORT_WATERMARK_ALARM, (LPCTSTR)pszXML);
        }

        break;
    default:
        break;
    }
}//lint !e818

void CGlobalPlayMgr::PostRecordSplitEvent(CIVS_OCXPlayerCtrl *pOcxCtrl,
                                          LONG EventID, IVS_LOCAL_RECORD_SUCCESS_INFO *pInfo, unsigned long ulHandle)const
{
    CXml xml;

    if (IVS_SUCCEED == COCXRecordXMLProcess::LocalRecordBuildXML(pInfo, ulHandle, xml))
    {
        unsigned int uiLength = 0;
        const TCHAR* pszXML = xml.GetXMLStream(uiLength);
        pOcxCtrl->IVS_OCX_Windows_Event((LONG)EventID, (LPCTSTR)pszXML);
        pOcxCtrl->IVS_OCX_Event((LONG)EventID, (LPCTSTR)pszXML);
    }
}

void CGlobalPlayMgr::PostRecordGenericEvent(CIVS_OCXPlayerCtrl *pOcxCtrl,
                                            long EventID, int code, int PanelID)const
{
    CXml xml;

    if (IVS_SUCCEED == COCXRecordXMLProcess::LocalRecordBuidlXMLGeneric(code, PanelID, xml))
    {
        unsigned int uiLength = 0;
        const TCHAR* pszXML = xml.GetXMLStream(uiLength);
        pOcxCtrl->IVS_OCX_Windows_Event((LONG)EventID, (LPCTSTR)pszXML);
    }
}

void CGlobalPlayMgr::PostDownLoadEvent(CIVS_OCXPlayerCtrl *pOcxCtrl, long EventID, unsigned long ulHandle, int Code)const
{
    CXml xml;

    if (IVS_SUCCEED == COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, Code, xml)) //lint !e732
    {
        unsigned int uiLength = 0;
        const TCHAR* pszXML = xml.GetXMLStream(uiLength);
        std::string strXml;
		if (NULL != pszXML)
		{
			strXml = pszXML;
		}
		pOcxCtrl->IVS_OCX_Windows_Event((LONG)EventID, (LPCTSTR)pszXML);
        pOcxCtrl->IVS_OCX_Event((LONG)EventID, (LPCTSTR)pszXML);

		BP_DBG_LOG("PostDownLoadEvent EventID[%d] xml[%s] OCX Address[%x]", EventID, strXml.c_str(), pOcxCtrl);
    }
}

// HandleLocalRecordException
// 处理本地录像异常事件上报
void CGlobalPlayMgr::HandleLocalRecordException(unsigned long Handle, int EventID,
                                                void *EventData, size_t DataSize, void *pUserData)
{
    CAutoLock __lock(m_Mutex);

    switch (EventID)
    {
    case IVS_EVENT_LOCAL_RECORD_SUCCESS:
    {
        IVS_LOCAL_RECORD_SUCCESS_INFO *pInfo = (IVS_LOCAL_RECORD_SUCCESS_INFO *)EventData;
        unsigned long __handle = *(unsigned long *)(pInfo + 1);

        // 查找panel对应的ocx
        CPlayObject *playObj = GetPlayObject(__handle);
        ASSERT(NULL != playObj && playObj->m_Panels.size());//lint !e831
        if ((NULL == playObj) || (0 == playObj->m_Panels.size()))    //lint !e774 !e831
        {
            return;
        }


		CVideoPane *pPanel = playObj->m_Panels.front();
		CIVS_OCXPlayerCtrl *pOcxCtrl = pPanel->GetOCXPlayerCtrl();
		ASSERT (NULL != pOcxCtrl);
		if (NULL == pOcxCtrl) //lint !e774
		{
			// log
			return;
		}
    	PostRecordSplitEvent(pOcxCtrl, (LONG)EventID, pInfo, __handle);
       
    }
        break;

    case IVS_EVENT_LOCAL_RECORD_EGENERIC:
    {
        unsigned long *pCode   = (unsigned long *)EventData;
        unsigned long __handle = *(unsigned long *)(pCode + 1);

        // 查找panel对应的ocx
        CPlayObject *playObj = GetPlayObject(__handle);
        ASSERT(NULL != playObj && playObj->m_Panels.size());//lint !e831
        if ((NULL == playObj) || (0 == playObj->m_Panels.size()))    //lint !e774 !e831
        {
            return;
        }

        // 遍历每个panel，设置panel状态
        PANEL_ITER iter = playObj->m_Panels.begin();
        PANEL_ITER end = playObj->m_Panels.end();
        for (; iter != end; iter++)
        {
            CVideoPane *pPanel = *iter;
            CIVS_OCXPlayerCtrl *pOcxCtrl = pPanel->GetOCXPlayerCtrl();

            if ((EventID == IVS_EVENT_LOCAL_RECORD_EGENERIC) && (*pCode != 2))
            {
                IVS_SDK_StopLocalRecord(CIVS_OCXPlayerCtrl::m_iSDKSessionID, pPanel->GetPlayHandle());
                CTranVideoRealtimeBar *pBar = pPanel->GetVideoRealtimeBar();
                if (NULL != pBar)
                {
                    pBar->SetRecordBtnState(false);
                }

                pPanel->SetIsLocalRecord(false);
            }
			
			playObj->m_PanelState.bRecording = false;

            PostRecordGenericEvent(pOcxCtrl, EventID, (int)*pCode, (int)pPanel->GetPaneID());
        }
    }
        break;

    default:
        break;
    }
} //lint !e818 !e715 pUserData暂不使用

void CGlobalPlayMgr::HandleRealPlayException(unsigned long Handle,
                                             int EventID, void *EventData, size_t DataSize, void *pUserData)
{
    CAutoLock __lock(m_Mutex);

    switch (EventID)
    {
    case IVS_EVENT_REALPLAY_FAILED:
    {
        STREAM_EXCEPTION_INFO* exceptionInfo = (STREAM_EXCEPTION_INFO*)(EventData);
        if (NULL == exceptionInfo)
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "exceptionInfo is null", "NA");
            return;
        }

        CPlayObject *playObj = GetPlayObject(exceptionInfo->ulHandle);
        ASSERT(NULL != playObj && playObj->m_Panels.size());//lint !e831
        if ((NULL == playObj) || (0 == playObj->m_Panels.size()))    //lint !e774 !e831
        {
            return;
        }
		playObj->m_PanelState.PlayState = VIDEO_PLAY_STATE_RESTARTING;

        // 遍历每个panel，设置panel状态
        PANEL_ITER iter = playObj->m_Panels.begin();
        PANEL_ITER end = playObj->m_Panels.end();
        for (; iter != end; iter++)
        {
            CVideoPane *pPanel = *iter;
            //pPanel->SetPlayState(VIDEO_PLAY_STATE_RESTARTING);
            if ((IVS_PLAYER_RET_RTSP_DISCONNECT == exceptionInfo->iExceptionCode)
                   || (IVS_PLAYER_RET_RECV_DATA_TIMEOUT == exceptionInfo->iExceptionCode)
				   || (IVS_PLAYER_RET_RTSP_ABNORMAL_STOP == exceptionInfo->iExceptionCode))
            {
				// 被占用成局部放大状态或者即时回放，不显示重连字样
				if (BOTTOMBAR_MAGNIFY_OPEN == pPanel->GetDstMagnifyState()
					|| pPanel->GetShowInstantStatus())
				{
					pPanel->SetResumeState(false);
				}
				else // 显示重连字样
				{
					pPanel->SetPlayState(VIDEO_PLAY_STATE_RESTARTING);
					pPanel->SetResumeState(true);
					pPanel->Invalidate(TRUE);
				}
            }
        }
    }
        break;

    case IVS_EVENT_RESUME_REALPLAY_OK:
    {
        CPlayObject *playObj = GetPlayObject((unsigned long)EventData);
        ASSERT(NULL != playObj && playObj->m_Panels.size());//lint !e831
        if ((NULL == playObj) || (0 == playObj->m_Panels.size()))   //lint !e774 !e831
        {
            return;
        }

        // 遍历每个panel，设置panel状态
        PANEL_ITER iter = playObj->m_Panels.begin();
        PANEL_ITER end = playObj->m_Panels.end();
        for (; iter != end; iter++)
        {
            CVideoPane *pPanel = *iter;
			pPanel->PostMessage(WM_RESUME_REALPLAY_OK_MSG, (WPARAM)0, (LPARAM)0);
        }

        playObj->m_PanelState.PlayState = VIDEO_PLAY_STATE_REALTIME;
    }
        break;

    default:
        break;
    }
} //lint !e818 !e715  pUserData 暂不使用

void CGlobalPlayMgr::PostPlayBackEvent(bool bLocal, CIVS_OCXPlayerCtrl *pOcxCtrl, int Exceptioncode,
                                       unsigned long WindID)const                                                              //lint !e830
{
    if (!bLocal)
    {
        PostMessage(pOcxCtrl->GetSafeHwnd(), WM_EVENT_REMOTE_PLAYBACK_CALLBACK, (WPARAM)WindID, (LPARAM)Exceptioncode);
    }
    else
    {
        PostMessage(pOcxCtrl->GetSafeHwnd(), WM_EVENT_LOCAL_PLAYBACK_CALLBACK, (WPARAM)WindID, 0);
    }
} //lint !e818

/*
 * HandlePlayBackException
 * 处理回放异常
 */
void CGlobalPlayMgr::HandlePlayBackException(unsigned long Handle, int EventID,
                                             void *EventData, size_t DataSize, void *pUserData) //lint !e830
{
    // 根据handle查找窗口
    STREAM_EXCEPTION_INFO* exceptionInfo = (STREAM_EXCEPTION_INFO*)(EventData);

    if (NULL == exceptionInfo)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "exceptionInfo is null", "NA");
        return;
    }

    CPlayObject *playObj = GetPlayObject(exceptionInfo->ulHandle);

    //ASSERT(NULL != playObj && playObj->m_Panels.size());
    if ((NULL == playObj) || (0 == playObj->m_Panels.size()))  //lint !e774
    {
        return;
    }

    PANEL_ITER iter = playObj->m_Panels.begin();
    CVideoPane *pPanel = *iter;
    CIVS_OCXPlayerCtrl *pCtrl = NULL;

    ASSERT(NULL != pPanel);
    if (NULL != pPanel) //lint !e774
    {
        if (pPanel->IsSyncPlayBack())
        {
 			CSyncPlayBackMgr &SyncMgr = CSyncPlayBackMgr::Instance();
			SyncMgr.SetLastError(pPanel, exceptionInfo->iExceptionCode);

			pPanel->SetPlayState(VIDEO_PLAY_STATE_NOT_PLAYING);
            //pPanel->ResetBottomButton();

			/*
			 * DTS2013052503198 
			 * 增加异常停止事件上报
			 */
			if (IVS_PLAYER_RET_RTSP_NORMAL_STOP != exceptionInfo->iExceptionCode)
			{
				if (pCtrl = pPanel->GetOCXPlayerCtrl())//lint !e720
				{
					pCtrl->PostMessage(WM_EVENT_REMOTE_PLAYBACK_CALLBACK,
						(WPARAM)pPanel->GetPaneID(), (LPARAM)(exceptionInfo->iExceptionCode));
				}
			}

			pPanel->PostMessage(WM_SYNC_PLAYBACK_MSG, (WPARAM)FALSE, (LPARAM)0);
            //pPanel->Invalidate(TRUE);

			//pPanel->SetPlayHandle(0);

        }
		else if (TYPE_WND_INSTANTBACKPLAY == pPanel->GetTypeWnd())
		{
		    /*
			 * DTS2013123004459
			 * 即时录像回放上报录像播放结束事件
			 */
			pPanel->PostMessage(WM_TIMERBAR_PLAYBACK_STOP, (WPARAM)(exceptionInfo->ulHandle),
				(LPARAM)(exceptionInfo->iExceptionCode));
		}
        else
        {
            if (pCtrl = pPanel->GetOCXPlayerCtrl()) //lint !e820 !e720
            {
                if (IVS_EVENT_LOCAL_PLAYBACK_FAILED != EventID)
                {
                    PostPlayBackEvent(false, pCtrl, exceptionInfo->iExceptionCode, pPanel->GetPaneID());
                }
                else
                {
                    PostPlayBackEvent(true, pCtrl, exceptionInfo->iExceptionCode, pPanel->GetPaneID());
                }
            }
        }
    }
} //lint !e818 !e715

/*
 * HandleDownLoadEvent
 * 处理下载事件上报
 */
void CGlobalPlayMgr::HandleDownLoadEvent(unsigned long Handle, int EventID,
                                         void *EventData, size_t DataSize, void *pUserData) //lint !e830
{
    CAutoLock __lock(m_Mutex);
	switch (EventID)//lint !e830
	{
	case IVS_EVENT_DOWNLOAD_SUCCESS:
		{
			IVS_LOCAL_RECORD_SUCCESS_INFO *pInfo = (IVS_LOCAL_RECORD_SUCCESS_INFO *)EventData;
			unsigned long __handle = *(unsigned long *)(pInfo + 1);
			if (NULL != CIVS_OCXPlayerCtrl::m_pOCX)
			{
				PostRecordSplitEvent(CIVS_OCXPlayerCtrl::m_pOCX, (LONG)EventID, pInfo, __handle);
			}
		}
		break;

	case IVS_EVENT_DOWNLOAD_FAILED:
		{
			unsigned long Code = 0;
			unsigned long __handle = 0;

			// 错误码需要重新定义，暂时转测使用 FIX ME!!!!
			if (DataSize == sizeof(STREAM_EXCEPTION_INFO))
			{
				STREAM_EXCEPTION_INFO* exceptionInfo = (STREAM_EXCEPTION_INFO*)(EventData);
				__handle = exceptionInfo->ulHandle;
				Code = exceptionInfo->iExceptionCode;        //lint !e732 !e830
			}
			else
			{
				unsigned long *pCode = (unsigned long *)EventData;
				Code = *pCode;
				__handle = *(unsigned long *)(pCode + 1);
				if (1 == Code)//lint !e818
				{
					Code = IVS_PLAYER_RET_RTSP_NORMAL_STOP;
				}
			}

			// 停止前端下载与平台下载统一接口
			if (Code != 2)
			{
				// (void)IVS_SDK_StopPlatformDownload(CIVS_OCXPlayerCtrl::m_iSDKSessionID, __handle);
			}

			if (NULL != CIVS_OCXPlayerCtrl::m_pOCX)
			{
				PostDownLoadEvent(CIVS_OCXPlayerCtrl::m_pOCX, IVS_EVENT_DOWNLOAD_FAILED, __handle, Code);    //lint !e713
			}
		}
		break;

	default:
		break;
	}
} //lint !e818 !e715
//处理水印告警上抛函数
void CGlobalPlayMgr::HandleWaterAlarmEvent(unsigned long Handle, int EventID, void *EventData, size_t DataSize,
                                           void *pUserData)//lint !e830
{
    CAutoLock __lock(m_Mutex);
	//获取水印告警结构体，即水印信息
    IVS_ALARM_WATERMARK_INFO *pInfo = (IVS_ALARM_WATERMARK_INFO *)EventData;
	//获取播放句柄
    unsigned long __handle = *(unsigned long *)(pInfo + 1);

    // 通过句柄信息查找panel对应的ocx
    CPlayObject *playObj = GetPlayObject(__handle);

    ASSERT(NULL != playObj && playObj->m_Panels.size());//lint !e831
    if ((NULL == playObj) || (0 == playObj->m_Panels.size()))        //lint !e774 !e831
    {
        return;
    }

    CVideoPane *pPanel = playObj->m_Panels.front();
	//得到对应的OCX信息
    CIVS_OCXPlayerCtrl *pOcxCtrl = pPanel->GetOCXPlayerCtrl();
    ASSERT (NULL != pOcxCtrl);
    if (NULL == pOcxCtrl)         //lint !e774
    {
        return;
    }

    BP_DBG_LOG("HandleWaterAlarmEvent,EventID:%d, pInfo->cCameraCode:%s, pInfo->cStartTime:%s, pInfo->cEndTime:%s, pInfo->ullAlarmEventID:%d", 
               EventID, pInfo->cCameraCode, pInfo->cStartTime, pInfo->cEndTime, pInfo->ullAlarmEventID);
	//上抛水印告警信息
    PostWaterAlarmEvent(pOcxCtrl, (LONG)EventID, pInfo);
}//lint !e818 !e715

// HandleEvent
// 处理事件
int CGlobalPlayMgr::HandleEvent(unsigned long Handle, int EventID, void *EventData, size_t DataSize, void *pUserData)const
{
    return 0;
} //lint !e818 !e715 pUserData 暂不使用

// DispatchEvent
// 分发事件
int CGlobalPlayMgr::DispatchEvent(unsigned long Handle, long EventID, void *data)const
{
    return 0;
} //lint !e818 !e715 未实现

// Instance
// 获取单例对象引用
CGlobalPlayMgr &CGlobalPlayMgr::Instance()
{
    static CGlobalPlayMgr Mgr;

    return Mgr;
}

// DumpPlayMsg
// 输出当前播放句柄信息
void CGlobalPlayMgr::DumpPlayMsg(std::string &strBuf)
{
#define BUF_SIZE 128
    PLAY_HANDLE_ITER iter = m_HandleMap.begin();
    PLAY_HANDLE_ITER end = m_HandleMap.end();
    for (; iter != end; iter++)
    {
        TCHAR Line[BUF_SIZE + 1] = {0};
        CPlayObject *pObject = iter->second;
        ASSERT(NULL != pObject);

        if (!CToolsHelp::Snprintf(Line, BUF_SIZE, "ref:%u, bReal:%u, Handle:%08x, m_strCamID:%s\n",
                                  pObject->m_ref, pObject->m_bRealStream, pObject->m_ulHandle, pObject->m_CamID))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS_OCX::", "GlobalPlay Mgr Dump Play Msg.Failed");
        }

        Line[BUF_SIZE] = '\0';
        strBuf += Line;

        PANEL_ITER PanelIter = pObject->m_Panels.begin();
        PANEL_ITER Panelend = pObject->m_Panels.end();
        for (; PanelIter != Panelend; PanelIter++)
        {
            memset(Line, 0, sizeof(Line));
            CVideoPane *pPanel = *PanelIter;
            ASSERT(NULL != pPanel);

            if (!CToolsHelp::Snprintf(Line, BUF_SIZE, "\tPanel:%08x OcxPtr:%08x WndID:%u\n",
                                      pPanel, pPanel->GetOCXPlayerCtrl(), pPanel->GetPaneID()))
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS_OCX::", "GlobalPlay Mgr Dump Play Msg.Failed");
            }

            Line[BUF_SIZE] = '\0';
            strBuf += Line;
        }
    }

#undef BUF_SIZE
}

// SetDisplayScale
// 设置满窗口显示
void CGlobalPlayMgr::SetDisplayScale(int Scale)const
{
    //TODO 遍历所有播放句柄，设置其播放比例
}//lint !e715

//GetPlayObject
//根据播放句柄获取playObject
CPlayObject *CGlobalPlayMgr::GetPlayObject(unsigned long Handle)
{
    PLAY_HANDLE_ITER HandleIter = m_HandleMap.find(Handle);

    if (HandleIter != m_HandleMap.end())
    {
        return HandleIter->second;
    }

    return NULL;
}

void CGlobalPlayMgr::SyncButtonState(unsigned long Handle, int btn, long value)
{
    CPlayObject *playObj = GetPlayObject(Handle);

    ASSERT(NULL != playObj && playObj->m_Panels.size());//lint !e831
    if ((NULL == playObj) || (0 == playObj->m_Panels.size())) //lint !e774 !e831
    {
        return;
    }

    switch (btn)
    {
    case BTN_LOCAL_RECORD:
    {
        PANEL_ITER iter = playObj->m_Panels.begin();
        PANEL_ITER end = playObj->m_Panels.end();
        for (; iter != end; iter++)
        {
            CVideoPane *pPanel = *iter;
            ASSERT(NULL != pPanel);
            if (NULL != pPanel)      //lint !e774
            {
                CTranVideoRealtimeBar* pToolBar = pPanel->GetVideoRealtimeBar();
                if (NULL != pToolBar)
                {
                    pToolBar->SetRecordBtnState(value != 0);
                }

                pPanel->SetIsLocalRecord(value != 0);
                CTranVideoTitleBar* pTitleBar = pPanel->GetVideoTitleBar();
                if (NULL != pTitleBar)
                {
                    pTitleBar->Invalidate(TRUE);
                }
            }
        }

        playObj->m_PanelState.bRecording = (value != 0);
    }
        break;

    default:
        break;
    }
}

unsigned long CGlobalPlayMgr::GetPlayState(unsigned long Handle)
{
    CPlayObject *pObject = GetPlayObject(Handle);

    if (NULL != pObject)
    {
        return pObject->GetPalyState();
    }

    return VIDEO_PLAY_STATE_NOT_PLAYING;
}

CGlobalSoundMgr::CGlobalSoundMgr()
{
	m_pSoundingPanel = NULL;
}

CGlobalSoundMgr::~CGlobalSoundMgr()
{
	m_pSoundingPanel = NULL;
}

CGlobalSoundMgr &CGlobalSoundMgr::Instance()
{
	static CGlobalSoundMgr Mgr;
	return Mgr;
}

void CGlobalSoundMgr::SetSoundingPanel(CVideoPane *pPanel)
{
    BP_RUN_LOG_INF("Set Sounding Panel", 
		"m_pSoundingPanel = %08x pPanel = %08x", m_pSoundingPanel, pPanel);

	if (NULL == m_pSoundingPanel)
	{
		m_pSoundingPanel = pPanel;
		return;
	} else {
		CTranVideoRealtimeBar *pBar = m_pSoundingPanel->GetVideoRealtimeBar();
		if (NULL == pBar)
		{
			return;
		}
		if (BOTTOMBAR_SOUND_OPEN == m_pSoundingPanel->GetRealtimeSoundState())
		{
			pBar->StopSound();
		}

		m_pSoundingPanel = pPanel;
	}
	return;
}


