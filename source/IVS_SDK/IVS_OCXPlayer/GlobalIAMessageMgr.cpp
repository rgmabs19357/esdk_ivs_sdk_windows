#include "stdafx.h"
#include "GlobalIAMessageMgr.h"
#include "IVS_OCXPlayerCtrl.h"
#include "VideoPane.h"
#include "OCXIAXMLProcess.h"
#include "SDKConfig.h"
#include "IVS_Trace.h"
#include "ivs_xml.h"
#include <process.h>
#include <time.h>

#define IA_VIDEO_DELAY_TIME_MIN 20
#define IA_VIDEO_DELAY_TIME_MAX 2000
/************************************************************************/
/*                            CIAObject                                 */
/************************************************************************/

CIAObject::CIAObject(IVS_ULONG	ulRuleType, IVS_ULONG ulQueryType)
	:m_ulRuleType(ulRuleType)
	,m_ulQueryType(ulQueryType)
{
	memset(m_CameraID, 0, sizeof(m_CameraID));
}

CIAObject::~CIAObject()
{

}

IVS_INT32 CIAObject::InsertPanel(CVideoPane *pPanel)
{
	ASSERT(pPanel);
	if (NULL == pPanel) //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Add Panel", "Panel is NULL");
		return IVS_FAIL;
	}
	PANEL_ITER iterPanel = m_VideoPaneList.begin();
	PANEL_ITER iterPanelEnd = m_VideoPaneList.end();
	while (iterPanel != iterPanelEnd)
	{
		CVideoPane* pVideoPanel = *iterPanel++;
		ASSERT(pVideoPanel != pPanel);
		if (pVideoPanel == pPanel)
		{
			BP_RUN_LOG_INF("Add Panel", "Panel Exists");
			return IVS_SUCCEED;
		}
	}
	m_VideoPaneList.push_back(pPanel);
	return IVS_SUCCEED;
}

IVS_INT32 CIAObject::DeletePanel(const CVideoPane *pPanel)
{
	ASSERT(pPanel);
	if (NULL == pPanel) //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Delete Panel", "Panel is NULL");
		return IVS_FAIL;
	}
	PANEL_ITER iterPanel = m_VideoPaneList.begin();
	PANEL_ITER iterPanelEnd = m_VideoPaneList.end();
	while (iterPanel != iterPanelEnd)
	{
		CVideoPane* pVideoPanel = *iterPanel;
		if (pVideoPanel == pPanel)
		{
			// Panel不能释放
			m_VideoPaneList.erase(iterPanel);
			break;
		}
        iterPanel++;
	}
	return IVS_SUCCEED;
}

IVS_INT32 CIAObject::SetCameraID(const TCHAR *cCamID)
{
	if (NULL == cCamID || '\0' == cCamID[0])
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Set Camera ID", "CameraID Empty");
		return IVS_PARA_INVALID;
	}
	(void)CToolsHelp::Strncpy(m_CameraID, IVS_DEV_CODE_LEN + 1, 
								cCamID, IVS_DEV_CODE_LEN);
	return IVS_SUCCEED;
}

IVS_INT32 CIAObject::GetPanelNum()
{
	IVS_INT32 iCount = 0;
	PANEL_ITER iterPanel = m_VideoPaneList.begin();
	PANEL_ITER iterPanelEnd = m_VideoPaneList.end();
	while (iterPanel != iterPanelEnd)
	{
		iterPanel++;
		iCount++;
	}
	return iCount;
}


/************************************************************************/
/*                           CRecordPanel                               */
/************************************************************************/

CRecordPanel::CRecordPanel(IVS_INT32 iSessionID, 
						   const IA_REQ_PARAM &ReqParam, 
						   CVideoPane *pVideoPane)
{
	m_iSessionID = iSessionID;
	m_pVideoPane = pVideoPane;
    memset(&m_ReqParam, 0, sizeof(IA_REQ_PARAM));
	(void)CToolsHelp::Memcpy(&m_ReqParam, sizeof(IA_REQ_PARAM), 
							 &ReqParam, sizeof(IA_REQ_PARAM));
	memset(m_cTransID, 0, sizeof(m_cTransID));
	
}

CRecordPanel::~CRecordPanel()
{
	m_pVideoPane = NULL;
}

IVS_INT32 CRecordPanel::SetTransID(const IVS_CHAR *cTransID)
{
	if (NULL == cTransID || '\0' == cTransID[0])
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Set TransID", "TransID Empty");
		return IVS_PARA_INVALID;
	}
	(void)CToolsHelp::Strncpy(m_cTransID, TRANS_ID_LENTH + 1, 
		cTransID, TRANS_ID_LENTH);
	return IVS_SUCCEED;
}


/************************************************************************/
/*                        CGlobalIAMessageMgr                           */
/************************************************************************/

CGlobalIAMessageMgr::CGlobalIAMessageMgr()
{
	m_bInited	= true;
	m_bThreadRun = false;
	m_ulRequestID = IA_INVALID_REQUEST_ID;
	m_ThreadHandle = NULL;
	memset(&m_IAReqParam, 0, sizeof(m_IAReqParam));
}

CGlobalIAMessageMgr::~CGlobalIAMessageMgr()
{
	m_bThreadRun = false;

	if (NULL != m_ThreadHandle)
	{
		WaitForSingleObject(m_ThreadHandle, INFINITE);
		CloseHandle(m_ThreadHandle);
		m_ThreadHandle = NULL;
	}

	try {
		ASSERT(!m_IAMessageMap.size());

		IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.begin();
		IAMESSAGE_MAP_ITER iterMapEnd  = m_IAMessageMap.end();
		for ( ; iterMap != iterMapEnd; iterMap++)
		{
			CIAObject *pIAObject = iterMap->second;
			ASSERT(NULL != pIAObject);
			if (NULL != pIAObject) //lint !e774
			{
// 				delete pIAObject;
				IVS_DELETE(pIAObject);
			}
		}
		m_bInited  = false;
	}
	catch(...)	{	}

	try {
		ASSERT(!m_RecordPanelList.size());

		RECORD_PANEL_LIST_ITER iterRecord = m_RecordPanelList.begin();
		RECORD_PANEL_LIST_ITER iterRecordEnd  = m_RecordPanelList.end();
		while (iterRecord != iterRecordEnd)
		{
			CRecordPanel *pRecordPanel = *iterRecord++;
			ASSERT(NULL != pRecordPanel);
			if (NULL != pRecordPanel) //lint !e774
			{
// 				delete pRecordPanel;
				IVS_DELETE(pRecordPanel);
			}
		}
		m_bInited  = false;
	}
	catch(...)	{	}

}

CGlobalIAMessageMgr &CGlobalIAMessageMgr::Instance()
{
	static CGlobalIAMessageMgr Mgr;
	return Mgr;
}


// 开始轨迹叠加
IVS_ULONG CGlobalIAMessageMgr::StartOverlay(IVS_INT32 iSessionID, 
											CVideoPane *pVideoPane, 
											IVS_ULONG ulRuleType)
{
	IVS_DEBUG_TRACE("SessionID: %d, VideoPane Addr: %x, RuleType: %lu", iSessionID, pVideoPane, ulRuleType);
	ASSERT(pVideoPane);
	if (NULL == pVideoPane)//lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start IA Overlay", "Panel is NULL");
		return IA_INVALID_REQUEST_ID;
	}
	
	// 获取查询基本参数
	IA_REQ_PARAM IAReqParam;
	memset(&IAReqParam, 0, sizeof(IAReqParam));
	IVS_INT32 iRet = IVS_FAIL;
	iRet = GetReqParam(pVideoPane, ulRuleType, IAReqParam);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start IA Overlay", "Set ReqParam Failed");
		return IA_INVALID_REQUEST_ID;
	}

	IVS_ULONG ulRequestID = IA_INVALID_REQUEST_ID;
	if (IA_QUERY_TYPE_REALTIME == IAReqParam.ulQueryType)// 实况
	{
		ulRequestID = StartRealtimeOverlay(pVideoPane, iSessionID, IAReqParam);
	}
	else// 录像
	{
		ulRequestID = StartRecordOverlay(pVideoPane, iSessionID, IAReqParam);
	}

	return ulRequestID;
}

// 停止轨迹叠加
IVS_INT32 CGlobalIAMessageMgr::StopOverlay(IVS_INT32 iSessionID, 
											const CVideoPane *pVideoPane, 
											IVS_ULONG &ulRequestID)
{
	IVS_DEBUG_TRACE("VideoPane Address: %x, RequestID: %lu", pVideoPane, ulRequestID);
	ASSERT(pVideoPane);
	if (NULL == pVideoPane)//lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Stop IA Overlay", "Panel is NULL");
		return IVS_PARA_INVALID;
	}
	
	DeletePanelInRecordList(iSessionID, pVideoPane);
	DeletePanelInMap(iSessionID, ulRequestID, pVideoPane);

    // modify by wanglei 00165153:2013.6.15, 关闭智能分析轨迹叠加后，需要关心响应的视频播放时延
    (void)IVS_SDK_SetDelayFrameNum(iSessionID, pVideoPane->GetPlayHandle(), 0);

	ulRequestID = IA_INVALID_REQUEST_ID;
	return IVS_SUCCEED;
}

// 智能分析轨迹上报回调
IVS_INT32 CGlobalIAMessageMgr::IAPushAlarmLocus(const IVS_CHAR *pIAMessage)
{
	IVS_DEBUG_TRACE("");

	IA_LAYOUT_MESSAGE* pIALayoutMessage = (IA_LAYOUT_MESSAGE*)IVS_NEW((CHAR* &)pIALayoutMessage, sizeof(IA_LAYOUT_MESSAGE));//lint !e826
	IA_REQ_PARAM	  IAReqParameters;
	memset(pIALayoutMessage, 0, sizeof(IA_LAYOUT_MESSAGE));
	memset(&IAReqParameters, 0, sizeof(IAReqParameters));
	IVS_INT32 iRet = CIAXMLProcess::TrackInfoContentPraseXML(pIAMessage, 
								*pIALayoutMessage, IAReqParameters);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IA Push Alarm Locus", 
						"Prase Track Info XML Failed");
		IVS_DELETE(pIALayoutMessage, MUILI);
		return iRet;
	}
	iRet = InsertTrackInfo(IAReqParameters, *pIALayoutMessage);
	IVS_DELETE(pIALayoutMessage, MUILI);
	return iRet;
}


// 插入轨迹信息
IVS_INT32 CGlobalIAMessageMgr::InsertTrackInfo(const IA_REQ_PARAM &IAReqParameters, 
												const IA_LAYOUT_MESSAGE &IALayoutMessage)
{
	IVS_DEBUG_TRACE("RequestID: %lu, QueryType: %lu, FrameID: %I64u", 
		IAReqParameters.ulRequestID, IAReqParameters.ulQueryType, IALayoutMessage.ulFrameID);
	CAutoLock lock(m_MutexMap);
	CIAObject *pIAObject = NULL;

	if (IA_QUERY_TYPE_REALTIME == IAReqParameters.ulQueryType)
	{
		IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.begin();
		IAMESSAGE_MAP_ITER iterMapEnd = m_IAMessageMap.end();
		for (; iterMap != iterMapEnd; iterMap++) 
		{
			CIAObject *ptmpIAObject = iterMap->second;
			ASSERT(ptmpIAObject);
			if (NULL == ptmpIAObject)//lint !e774
			{
				continue;
			}
			if ((IAReqParameters.ulRuleType == ptmpIAObject->m_ulRuleType) 
				&& (IAReqParameters.ulQueryType == ptmpIAObject->m_ulQueryType) 
				&& (0 == strncmp(IAReqParameters.CameraID, ptmpIAObject->m_CameraID, strlen(IAReqParameters.CameraID))))
			{
				pIAObject = ptmpIAObject;
			}
		}
		if (NULL == pIAObject)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Insert Track Info", "RealPlay Panel Not Found");
			return IVS_FAIL;
		}
	} 
	else 
	{
		// 根据RequestID查找节点
		IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.find(IAReqParameters.ulRequestID);
		if (m_IAMessageMap.end() == iterMap)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Insert Track Info", "Record Panel Not Found");
			return IVS_FAIL;
		}
		pIAObject = iterMap->second;
		ASSERT(pIAObject);
		if (NULL == pIAObject) //lint !e774
		{
 			BP_RUN_LOG_INF("Insert Track Info", "IAObject NULL");
			return IVS_FAIL;
		}
	}

	PANEL_ITER IterPanel = pIAObject->m_VideoPaneList.begin();
	PANEL_ITER IterPanelEnd = pIAObject->m_VideoPaneList.end();
	while (IterPanel != IterPanelEnd)
	{
		CVideoPane *pVideoPane = *IterPanel++;
		ASSERT(pVideoPane);
		if (NULL == pVideoPane) //lint !e774
		{
// 			PaneIter = pIAObject->m_VideoPaneList.erase(PaneIter);
// 			BP_RUN_LOG_INF("Insert Track Info", "Delete NULL VideoPane");
			continue;
		}

		IVS_INT32 iRet = pVideoPane->InsertIALayoutMessage(IALayoutMessage);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Insert Track Info", "Insert Failed");
			return iRet;
		}
	}
	return IVS_SUCCEED;
}



IVS_ULONG CGlobalIAMessageMgr::AllocRequestID()
{
	return ++m_ulRequestID;
}


// 设置基础查询参数：QueryType、CameraID
IVS_INT32 CGlobalIAMessageMgr::GetReqParam(const CVideoPane *pVideoPane, 
											IVS_ULONG ulRuleType,
											IA_REQ_PARAM &IAReqParam)const
{
	ASSERT(pVideoPane);
	if (NULL == pVideoPane) //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Request Param", "Panel is NULL");
		return IVS_PARA_INVALID;
	}

	// 获取当前播放状态（实况 或 录像）
	unsigned long ulPlayState = pVideoPane->GetPlayState();
	switch (ulPlayState)
	{
	case VIDEO_PLAY_STATE_REALTIME:
		IAReqParam.ulQueryType = IA_QUERY_TYPE_REALTIME;
		break;
	case VIDEO_PLAY_STATE_VOD:
		IAReqParam.ulQueryType = IA_QUERY_TYPE_RECORD;
		break;
	default:
		ASSERT(0);
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Request Param", 
						"Play State Invalid");//lint !e527
		return IVS_FAIL;
	}

	// 获取当前CameraID
	const TCHAR *pCameraID = pVideoPane->GetCameraID();
	if (NULL == pCameraID || '\0' == pCameraID[0] 
	|| !CToolsHelp::Strncpy(IAReqParam.CameraID, IVS_DEV_CODE_LEN + 1, 
		pCameraID, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Request Param", "Get Camera ID Failed");
		return IVS_FAIL;
	}

//     if (NULL == pCUCode || '\0' == pCUCode[0] 
//     || !CToolsHelp::Strncpy(IAReqParam.CUCode, 65, pCUCode, 64))
//     {
//         BP_RUN_LOG_ERR(IVS_FAIL, "Get Request Param", "Get CUCode Failed");
//         return IVS_FAIL;
//     }
// 

	// 设置RuleType
	IAReqParam.ulRuleType = ulRuleType;
	
	return IVS_SUCCEED;
}


IVS_ULONG CGlobalIAMessageMgr::StartRealtimeOverlay(CVideoPane *pVideoPane, 
													IVS_INT32 iSessionID, 
													IA_REQ_PARAM &IAReqParam) 
{
	IVS_ULONG ulRequestID = TryRealStreamShare(pVideoPane, IAReqParam);
	if (IA_INVALID_REQUEST_ID != ulRequestID)
	{
		return ulRequestID;
	}

	IAReqParam.ulRequestID = AllocRequestID();// 设置RequestID
	IVS_INT32 iResult = IVS_FAIL;

	// 发送请求轨迹XML
	iResult = SendMessageReqXML(iSessionID, IAReqParam, NULL);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(iResult, "Start Realtime Overlay", 
						"Send Req Message Failed");
		return IA_INVALID_REQUEST_ID;
	}
	// 实况延时设置
	IVS_MEDIA_INFO strMediaInfo;
	memset(&strMediaInfo,0,sizeof(IVS_MEDIA_INFO));
	iResult = IVS_SDK_GetMediaInfo(iSessionID,pVideoPane->GetPlayHandle(), &strMediaInfo);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(iResult, "Start Realtime Overlay", 
			"Get Media Info from SDK Failed");
		return IA_INVALID_REQUEST_ID;
	}

    //获取配置延时秒数，从配置文件获取前端延时时间
    IVS_UINT32 uiIAVideoDelayTime = 0;
    if (4 == IAReqParam.ulRuleType)
    {
        //获取前端延时时间设置
        uiIAVideoDelayTime = CSDKConfig::instance().GetPUIAVideoDelayValue();
        uiIAVideoDelayTime = min(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MAX);
        uiIAVideoDelayTime = max(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MIN);
        BP_RUN_LOG_INF("Start Realtime Overlay", "PU Video Delay Time: %u", uiIAVideoDelayTime);
    }
    else
    {
        //获取平台延时时间设置
        uiIAVideoDelayTime = CSDKConfig::instance().GetPlatformIAVideoDelayValue();
        uiIAVideoDelayTime = min(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MAX);
        uiIAVideoDelayTime = max(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MIN);
        BP_RUN_LOG_INF("Start Realtime Overlay", "Platform Video Delay Time: %u", uiIAVideoDelayTime);
    }

	// 获取配置延时秒数，计算延时帧数
	//IVS_UINT32 uiIAVideoDelayTime = CSDKConfig::instance().GetIAVideoDelayValue();
	//uiIAVideoDelayTime = min(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MAX);
	//uiIAVideoDelayTime = max(uiIAVideoDelayTime, IA_VIDEO_DELAY_TIME_MIN);
	//BP_RUN_LOG_INF("Start Realtime Overlay", "Video Delay Time: %u", uiIAVideoDelayTime);

	IVS_UINT32 uiIAVideoDelayFrame = (strMediaInfo.uiVideoFrameRate 
										* uiIAVideoDelayTime) / 1000;
#if 0// 待zhouqiming修改MP完成
	uiIAVideoDelayFrame = 19;
#endif	
	BP_RUN_LOG_INF("Start Realtime Overlay", "Video Delay Frame: %u", uiIAVideoDelayFrame);
	(void)IVS_SDK_SetDelayFrameNum(iSessionID, pVideoPane->GetPlayHandle(), uiIAVideoDelayFrame);

	// 将Panel关系组插入到Map中，用于插入轨迹信息
	iResult = InsertIAObject(pVideoPane, IAReqParam);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(iResult, "Start Realtime Overlay", 
						"Insert IAObject Failed");
		return IA_INVALID_REQUEST_ID;
	}
	return IAReqParam.ulRequestID;
}

IVS_ULONG CGlobalIAMessageMgr::StartRecordOverlay(CVideoPane *pVideoPane, 
												  IVS_INT32 iSessionID, 
												  IA_REQ_PARAM &IAReqParam) 
{
	IAReqParam.ulRequestID = AllocRequestID();// 设置RequestID

	// 获取录像当前时间
	IVS_PLAYBACK_TIME stPlaybackTime;
	memset(&stPlaybackTime, 0, sizeof(IVS_PLAYBACK_TIME));
	IVS_INT32 iResult = IVS_SDK_GetPlayBackTime(iSessionID, 
				(IVS_ULONG)pVideoPane->GetPlayHandle(), &stPlaybackTime);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(iResult, "Start Record Overlay", 
						"Get Play Back Time Failed");
		return IA_INVALID_REQUEST_ID;
	}
	(void)CToolsHelp::Strncpy(IAReqParam.StartTime, sizeof(IAReqParam.StartTime), 
		stPlaybackTime.cPlayBackTime, sizeof(stPlaybackTime.cPlayBackTime));

	// 起线程跟踪录像轨迹数
	if (NULL == m_ThreadHandle)
	{	
		m_bThreadRun = true;
		m_ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, 
			this, 0, NULL);
		ASSERT(m_ThreadHandle);
		if (NULL == m_ThreadHandle)//lint !e774
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Start Record Overlay", 
				"Begin Thread Failed");
			m_bThreadRun = false;
			return IA_INVALID_REQUEST_ID;
		}
	}

	// 将Panel关系组插入到Map中，用于插入轨迹信息
	iResult = InsertIAObject(pVideoPane, IAReqParam);
	if (IVS_SUCCEED != iResult)
	{
		BP_RUN_LOG_ERR(iResult, "Start Record Overlay", 
						"Insert IAObject Failed");
		return IA_INVALID_REQUEST_ID;
	}

	// 将Panel插入到录像列表
	CRecordPanel *pRecordPanel = NULL;
	try	{
		pRecordPanel = new CRecordPanel(iSessionID, IAReqParam, pVideoPane);
	} 
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Start Record Overlay", 
						"Create RecordPanel Failed");
		return IA_INVALID_REQUEST_ID;
	}

	CAutoLock lock(m_MutexList);
	m_RecordPanelList.push_back(pRecordPanel);

	return IAReqParam.ulRequestID;//lint !e429
}//lint !e429


// 尝试流复用
IVS_ULONG CGlobalIAMessageMgr::TryRealStreamShare(CVideoPane *pVideoPane, 
												  const IA_REQ_PARAM &IAReqParam)
{
	ASSERT(pVideoPane);
	if (NULL == pVideoPane) //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Find IA Object", "VideoPane is NULL");
		return IA_INVALID_REQUEST_ID;
	}
	CAutoLock lock(m_MutexMap);

	IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.begin();
	IAMESSAGE_MAP_ITER iterMapEnd = m_IAMessageMap.end();
	while (iterMap != iterMapEnd) 
	{
		CIAObject *pIAObject = iterMap->second;
		ASSERT(pIAObject);
		if (NULL == pIAObject)//lint !e774
		{
			iterMap++;
			continue;
		}
		if ((IAReqParam.ulRuleType == pIAObject->m_ulRuleType) 
			&& (IAReqParam.ulQueryType == pIAObject->m_ulQueryType) 
			&& (0 == strcmp(IAReqParam.CameraID, pIAObject->m_CameraID)))
		{
			(void)pIAObject->InsertPanel(pVideoPane);
			return iterMap->first;
		}
		iterMap++;
	}
	return IA_INVALID_REQUEST_ID;
}

// 发送请求轨迹XML消息
IVS_INT32 CGlobalIAMessageMgr::SendMessageReqXML(IVS_INT32 iSessionID, IA_REQ_PARAM &IAReqParam, IVS_CHAR *pTransID)const
{
	CXml xml;
	if (IVS_SUCCEED != CIAXMLProcess::GetAnalyzeResultBuildXML(IAReqParam, xml))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Send Message Req XML", "Build XML Failed");
		return IVS_FAIL;
	}

	// send cmd
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR *ReqXml = xml.GetXMLStream(uiLen);
	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_FAIL;
	if (NULL == pTransID)
	{
		iRet = IVS_SDK_IA(iSessionID, NSS_GET_ANALYZE_RESULT_REQ, ReqXml, &pRspXml);
	} 
    else 
    {
		iRet = IVS_SDK_IA_EX(iSessionID, NSS_GET_ANALYZE_RESULT_REQ, pTransID, TRANS_ID_LENTH + 1, ReqXml, &pRspXml);
	}
	if ((IVS_SUCCEED == iRet) && (NULL != pRspXml))
	{
		IVS_ULONG ulResultCode = IVS_FAIL;
		iRet = CIAXMLProcess::GetAnalyzeResultPraseXML(pRspXml, ulResultCode, IAReqParam);
	}
	IVS_SDK_ReleaseBuffer(pRspXml);

	return iRet;
}

// 发送停止请求轨迹XML消息
IVS_INT32 CGlobalIAMessageMgr::SendMessageStopXML(IVS_INT32 iSessionID, 
													IVS_ULONG ulRequestID, 
													IVS_ULONG ulRuleType, 
													IVS_ULONG ulQueryType, 
													const IVS_CHAR *CameraID, IVS_CHAR *pTransID)const
{
	if (NULL == CameraID) 
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Send Message Stop XML", "CameraID is NULL");
		return IVS_PARA_INVALID;
	}
	CXml xml;
	if (IVS_SUCCEED != CIAXMLProcess::StopAnalyzeResultBuildXML(ulRequestID, ulRuleType, ulQueryType, CameraID, xml))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Send Message Stop XML", "Build XML Failed");
		return IVS_FAIL;
	}

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR *ReqXml = xml.GetXMLStream(uiLen);
	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_FAIL;
	if (NULL == pTransID)
	{
		iRet = IVS_SDK_IA(iSessionID, NSS_STOP_PUSH_ANALYZE_RESULT_REQ, ReqXml, &pRspXml);
	} else {
		iRet = IVS_SDK_IA_EX(iSessionID, NSS_STOP_PUSH_ANALYZE_RESULT_REQ, pTransID, TRANS_ID_LENTH + 1, ReqXml, &pRspXml);
	}
	if ((IVS_SUCCEED == iRet) && (NULL != pRspXml))
	{
// 		IVS_ULONG ulResultCode = IVS_FAIL;
// 		iRet = CIAXMLProcess::StopAnalyzeResultPraseXML(pRspXml, ulResultCode);
	}
	IVS_SDK_ReleaseBuffer(pRspXml);

	return iRet;

}

// 插入新的IAObject到map中
IVS_INT32 CGlobalIAMessageMgr::InsertIAObject(CVideoPane *pVideoPane, IA_REQ_PARAM &IAReqParam)
{
	CAutoLock lock(m_MutexMap);
	IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.find(IAReqParam.ulRequestID);
	if (m_IAMessageMap.end() != iterMap)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Insert IA Object", "RequestID Exists");
		return IVS_FAIL;
	}

	CIAObject *pNewIAObject = NULL;
	try	{
		pNewIAObject = new CIAObject(IAReqParam.ulRuleType, IAReqParam.ulQueryType);
	} 
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Insert IA Object", 
						"Create IAObject Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	(void)pNewIAObject->SetCameraID(IAReqParam.CameraID);
	(void)pNewIAObject->InsertPanel(pVideoPane);
	(void)m_IAMessageMap.insert(std::make_pair(IAReqParam.ulRequestID, pNewIAObject));

	return IVS_SUCCEED;
}//lint !e429

void CGlobalIAMessageMgr::DeletePanelInMap(IVS_INT32 iSessionID, IVS_ULONG ulRequestID, const CVideoPane *pVideoPane) 
{
	ASSERT(pVideoPane);
	if (NULL == pVideoPane)//lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Delete Panel In Map", "Panel is NULL");
		return;
	}

	CAutoLock lock(m_MutexMap);

	// 根据RequestID查找节点
	IAMESSAGE_MAP_ITER iterMap = m_IAMessageMap.find(ulRequestID);
	if (m_IAMessageMap.end() == iterMap)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Delete Panel In Map", "Panel Not Found");
		return;
	}

	CIAObject *pIAObject = iterMap->second;
	ASSERT(pIAObject);
	if (NULL == pIAObject) //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Delete Panel In Map", "IAObject is NULL");
		return;
	}
	if (IA_QUERY_TYPE_REALTIME == pIAObject->m_ulQueryType 
		&& 1 == pIAObject->GetPanelNum() 
		&& IVS_SUCCEED != SendMessageStopXML(iSessionID, 0, pIAObject->m_ulRuleType, 
						pIAObject->m_ulQueryType, pIAObject->m_CameraID, NULL))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Delete Panel In Map", "Send Stop Push Failed");
	}

	// 删除map节点中PanelList中对应Panel指针
	(void)pIAObject->DeletePanel(pVideoPane);
	// 若仅存在一个Panel指针,则删除整个map节点
	if (pIAObject->CheckPanelListNull())
	{
// 		delete pIAObject;
		IVS_DELETE(pIAObject);
		iterMap = m_IAMessageMap.erase(iterMap);
	}
}

void CGlobalIAMessageMgr::DeletePanelInRecordList(IVS_INT32 iSessionID, const CVideoPane *pVideoPane) 
{
	ASSERT(pVideoPane);
	if (NULL == pVideoPane)//lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Delete Panel In Record List", "Panel is NULL");
		return;
	}

	CAutoLock lock(m_MutexList);

	// 删除录像列表中对应的Panel
	RECORD_PANEL_LIST_ITER iterRecord = m_RecordPanelList.begin();
	RECORD_PANEL_LIST_ITER iterRecordEnd = m_RecordPanelList.end();
	while (iterRecord != iterRecordEnd)
	{
		CRecordPanel *pPanel = *iterRecord;
		if (NULL == pPanel)
		{
			iterRecord++;
			continue;
		}
		if (pPanel->m_pVideoPane == pVideoPane)
		{
			if (IVS_SUCCEED != SendMessageStopXML(iSessionID, pPanel->m_ReqParam.ulRequestID, 
								pPanel->m_ReqParam.ulRuleType, pPanel->m_ReqParam.ulQueryType, 
								pPanel->m_ReqParam.CameraID, pPanel->m_cTransID))
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "Delete Panel In Record List", "Send Stop Push Failed");
			}
			IVS_DELETE(pPanel);
// 			try
// 			{
// 				delete pPanel;
// 			}
// 			catch (...)
// 			{
// 				BP_RUN_LOG_ERR(IVS_FAIL, "Delete Panel In Record List", 
// 					"Delete RecordPanel Failed");
// 				return;
// 			}
			iterRecord = m_RecordPanelList.erase(iterRecord);
			break;
		}
		iterRecord++;
	}
}

void CGlobalIAMessageMgr::CalculateStopTime(IA_REQ_PARAM &IAReqParam)
{
	if ('\0' == IAReqParam.StartTime[0])
	{
		return;
	}
	tm stStartInt;
	memset(&stStartInt, 0, sizeof(tm));
	TM stStartArr;
	memset(&stStartArr, 0, sizeof(TM));

	memcpy(stStartArr.Year, &IAReqParam.StartTime[0], 4);
	stStartInt.tm_year = atoi(stStartArr.Year) - 1900;
	memcpy(stStartArr.Month, &IAReqParam.StartTime[4], 2);
	stStartInt.tm_mon = atoi(stStartArr.Month) - 1;
	memcpy(stStartArr.Day, &IAReqParam.StartTime[6], 2);
	stStartInt.tm_mday = atoi(stStartArr.Day);
	memcpy(stStartArr.Hour, &IAReqParam.StartTime[8], 2);
	stStartInt.tm_hour = atoi(stStartArr.Hour);
	memcpy(stStartArr.Min, &IAReqParam.StartTime[10], 2);
	stStartInt.tm_min = atoi(stStartArr.Min);
	memcpy(stStartArr.Sec, &IAReqParam.StartTime[12], 2);
	stStartInt.tm_sec = atoi(stStartArr.Sec);

	stStartInt.tm_min = stStartInt.tm_min + 1;
	if ((time_t)-1 == mktime(&stStartInt))
	{
		return;
	}

	(void)_snprintf_s(IAReqParam.StopTime, IVS_TIME_LEN, "%04d%02d%02d%02d%02d%02d", 
				stStartInt.tm_year + 1900, stStartInt.tm_mon + 1, 
				stStartInt.tm_mday, stStartInt.tm_hour, 
				stStartInt.tm_min, stStartInt.tm_sec);

}


IVS_UINT32 __stdcall CGlobalIAMessageMgr::ThreadProc(void *pParameter)
{
	ASSERT(pParameter);
	if (NULL == pParameter)//lint !e774
	{
		_endthreadex(0);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Thread Process", "Parameter Point NULL");
		return IVS_PARA_INVALID;
	}
	CGlobalIAMessageMgr *pIAMessageMgr = (CGlobalIAMessageMgr *)pParameter;

	while (pIAMessageMgr->m_bThreadRun)
	{
		// 录像Panel列表, 判断是否需要发轨迹请求消息
		//CAutoLock lock(pIAMessageMgr->m_Mutex);
		pIAMessageMgr->m_MutexList.Lock();
		RECORD_PANEL_LIST_ITER iterRecord = pIAMessageMgr->m_RecordPanelList.begin();
		RECORD_PANEL_LIST_ITER iterRecordEnd = pIAMessageMgr->m_RecordPanelList.end();

		while (iterRecord != iterRecordEnd)
		{
			CRecordPanel *pPanel = *iterRecord++;
			if (NULL == pPanel)
			{
				continue;
			}
			CVideoPane *pVideoPane = pPanel->m_pVideoPane;
			if (NULL == pVideoPane)
			{
				continue;
			}

			if (TRACK_MESSAGE_NUM_MIN > pVideoPane->GetCurrentElementNum())
			{
				// 计算 pPanel->m_ReqParam.StopTime 一分钟后的结束时间
				CalculateStopTime(pPanel->m_ReqParam);
				IVS_INT32 iResult = pIAMessageMgr->SendMessageReqXML(pPanel->m_iSessionID, pPanel->m_ReqParam, pPanel->m_cTransID);
				if (IVS_SUCCEED != iResult)
				{
					BP_RUN_LOG_ERR(iResult, "Thread Process", "Send Message Failed");
				}
			}
		}
		pIAMessageMgr->m_MutexList.UnLock();

		// 等待
		Sleep(TREAD_WAIT_MSECS);
	}

	_endthreadex(0);
	return 0;
}

