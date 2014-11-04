#include "PostEventXML.h"
#include "SDKDef.h"
#include "IVS_Trace.h"
#include "ToolsHelp.h"

PostEventXML::PostEventXML(void)
{
	m_mapRealtimeToolBarBtnName.clear();
	m_mapAnalysisToolBarBtnName.clear();

	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_SNAPSHOT, "IVS_TOOLBAR_SNAPSHOT"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_LOCAL_RECORD, "IVS_TOOLBAR_LOCAL_RECORD"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_BOOKMARK, "IVS_TOOLBAR_BOOKMARK"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_ZOOM, "IVS_TOOLBAR_ZOOM"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_PLAY_RECORD, "IVS_TOOLBAR_PLAY_RECORD"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_PLAY_SOUND, "IVS_TOOLBAR_PLAY_SOUND"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_TALKBACK, "IVS_TOOLBAR_TALKBACK"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_VIDEO_TVW, "IVS_TOOLBAR_VIDEO_TVW"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_ALARM_WIN, "IVS_TOOLBAR_ALARM_WIN"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_PTZ, "IVS_TOOLBAR_PTZ"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_IA, "IVS_TOOLBAR_IA"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_OPEN_MAP, "IVS_TOOLBAR_OPEN_MAP"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_WINDOW_MAIN, "IVS_TOOLBAR_WINDOW_MAIN"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_PLAY_QUALITY, "IVS_TOOLBAR_PLAY_QUALITY"));
    (void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_PTZ3D, "IVS_TOOLBAR_PTZ3D"));
	(void)m_mapRealtimeToolBarBtnName.insert(std::make_pair(IVS_TOOLBAR_EXIT_PLAY_RECORD, "IVS_TOOLBAR_EXIT_PLAY_RECORD"));

	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_DRAW_RECT, "IVS_IA_TOOLBAR_DRAW_RECT"));
	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_DRAW_POLYGON, "IVS_IA_TOOLBAR_DRAW_POLYGON"));
	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_ZOOM_IN, "IVS_IA_TOOLBAR_ZOOM_IN"));
	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_ZOOM_OUT, "IVS_IA_TOOLBAR_ZOOM_OUT"));
	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_DRAW_MOVE, "IVS_IA_TOOLBAR_DRAW_MOVE"));
	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_DISPLAY_MODE, "IVS_IA_TOOLBAR_DISPLAY_MODE"));
	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_PLAY, "IVS_IA_TOOLBAR_PLAY"));
	(void)m_mapAnalysisToolBarBtnName.insert(std::make_pair(IVS_IA_TOOLBAR_DELETE, "IVS_IA_TOOLBAR_DELETE"));
}


PostEventXML::~PostEventXML(void)
{
}

IVS_VOID PostEventXML::PostTalkbackEvent(CString &xmlToCU, ULONG ulResult)
{
    IVS_CHAR str[TEMP_BUFFER_SIZE] = {0};
    xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    xmlToCU += "<Content>";
    xmlToCU += "<ResultCode>";
    (void)itoa((int)ulResult, str, 10);
    xmlToCU += str;
    xmlToCU += "</ResultCode>";
    xmlToCU +="</Content>";
}//lint !e1762

// 获得当前激活窗体更改消息报文
IVS_VOID PostEventXML::GetActiveWndChangeEventMsgXML( CString &xmlToCU, ULONG ulWinID )
{
    IVS_CHAR str[TEMP_BUFFER_SIZE] = {0};

    xmlToCU  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    xmlToCU += "<Content>";

    xmlToCU += "<WndID>";
    (void)itoa((int)ulWinID, str, 10);
    xmlToCU +=str;
    xmlToCU += "</WndID>";

    xmlToCU +="</Content>";
}//lint !e1762

// 获得交换窗体事件消息报文
IVS_VOID PostEventXML::GetExchangeWndEventMsgXML( CString &xmlToCU, ULONG ulSrcWnd, ULONG ulDstWnd )
{
    IVS_CHAR str[TEMP_BUFFER_SIZE] = {0};
    xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    xmlToCU += "<Content>";

    xmlToCU += "<SrcWndID>";
    (void)itoa((int)ulSrcWnd, str, 10);
    xmlToCU +=str;
    xmlToCU += "</SrcWndID>";

    xmlToCU += "<DstWndID>";
    (void)itoa((int)ulDstWnd, str, 10);
    xmlToCU +=str;
    xmlToCU += "</DstWndID>";

    xmlToCU +="</Content>";
}//lint !e1762

// 获取鼠标点击事件的消息报文
IVS_VOID PostEventXML::GetMouseClickEventXML( CString &xmlToCU, ULONG ulWinID, ULONG ulX, ULONG ulY )
{
    IVS_CHAR str[TEMP_BUFFER_SIZE];

    xmlToCU= "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

    xmlToCU += "<Content>";

    xmlToCU += "<WndID>";
    (void)itoa((int)ulWinID, str, 10);
    xmlToCU +=str;
    xmlToCU += "</WndID>";

    xmlToCU += "<X>";
    (void)itoa((int)ulX, str, 10);
    xmlToCU +=str;
    xmlToCU += "</X>";

    xmlToCU += "<Y>";
    (void)itoa((int)ulY, str, 10);
    xmlToCU +=str;
    xmlToCU += "</Y>";

    xmlToCU +="</Content>";
}//lint !e1762

// 获取停止点击的消息报文
IVS_VOID PostEventXML::GetStopButtonClickMsgXML(CString &xmlToCU, ULONG ulWinID)
{
    xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

    IVS_CHAR str[TEMP_BUFFER_SIZE];
    xmlToCU += "<Content>";

    xmlToCU += "<WndID>";
    (IVS_VOID)itoa((IVS_INT32)ulWinID, str, 10);
    xmlToCU += str;
    xmlToCU += "</WndID>";

    xmlToCU +="</Content>";
}//lint !e1762

// 获取云镜参数报文
IVS_LONG PostEventXML::GetPTZConfig(const std::string &strXML,IVS_LONG *lResultCode , IVS_UINT32 *iSpeed)
{
	IVS_DEBUG_TRACE("");
	if (NULL == lResultCode || NULL == iSpeed)
	{
		return IVS_OPERATE_MEMORY_ERROR;
	}
	TiXmlDocument * pXMLDocument = nullptr;
	try
	{
		pXMLDocument = new TiXmlDocument();
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "PostEventXML::GetPTZConfig", "new TiXmlDocument failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

    pXMLDocument->Parse(strXML.c_str());
   // TiXmlNode * pNode = pXMLDocument->RootElement()->FirstChild(_T("ResultCode"));
	TiXmlElement *pTiXmlElement = pXMLDocument->RootElement();
	if (nullptr == pTiXmlElement)
	{
		delete pXMLDocument;
		pXMLDocument = nullptr;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "RootElement parameter is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	TiXmlNode * pNode = pTiXmlElement->FirstChild(_T("ResultCode"));
    if (nullptr == pNode)
    {
		delete pXMLDocument;
		pXMLDocument = nullptr;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "FirstChild(ResultCode) parameter is null");
        return IVS_OPERATE_MEMORY_ERROR;
    }
   // const IVS_CHAR * pResultCode =  pNode->FirstChild()->Value();
	TiXmlNode* pFirstChild = pNode->FirstChild();
	if (nullptr == pFirstChild)
	{
		delete pXMLDocument;
		pXMLDocument = nullptr;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "Node FirstChild(ResultCode) parameter is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	const IVS_CHAR * pResultCode = pFirstChild->Value();
	if (NULL == pResultCode)
	{
		delete pXMLDocument;
		pXMLDocument = nullptr;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "FirstChild(ResultCode) Value parameter is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}
    *lResultCode = atol(pResultCode);
   // pNode = pXMLDocument->RootElement()->FirstChild(_T("PTZConfig"));
	pNode = pTiXmlElement->FirstChild(_T("PTZConfig"));
    if (nullptr == pNode)
    {
		delete pXMLDocument;
		pXMLDocument = nullptr;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "FirstChild（PTZConfig）parameter is null");
		return IVS_OPERATE_MEMORY_ERROR;
    }
    pNode = pNode->FirstChild(_T("Speed"));
    if (nullptr == pNode)
    {
		delete pXMLDocument;
		pXMLDocument = nullptr;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "FirstChild(Speed) parameter is null");
		return IVS_OPERATE_MEMORY_ERROR;
    }
    //const IVS_CHAR * pSpeed = pNode->FirstChild()->Value();
	pFirstChild = pNode->FirstChild();
	if (nullptr == pFirstChild)
	{
		delete pXMLDocument;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "Node FirstChild（Speed）parameter is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	const IVS_CHAR * pSpeed = pFirstChild->Value();
	if (NULL == pSpeed)
	{
		delete pXMLDocument;
		pXMLDocument = nullptr;
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get PTZ Config", "FirstChild(Speed) Value parameter is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}
    *iSpeed = static_cast<unsigned int>(atoi(pSpeed));
    delete pXMLDocument;
    pXMLDocument = nullptr;
    return IVS_SUCCEED;
}//lint !e1762
IVS_VOID PostEventXML::GetOcxEventMsgStrXML( std::string &xmlToCU, LPCTSTR pMsg, LPCTSTR pMsgSeq, ULONG ulResultCode, ULONG ulWndID )
{
	xmlToCU = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?><MESSAGE><VERSION>2.0</VERSION><CV_HEADER>");

	xmlToCU += _T("<MSG_TYPE>");
	xmlToCU += pMsg;
	xmlToCU += _T("</MSG_TYPE>");

	TCHAR str[TEMP_BUFFER_SIZE] = {0x0};
	xmlToCU += _T("<MSG_SEQ>");
	xmlToCU += pMsgSeq;
	xmlToCU += _T("</MSG_SEQ></CV_HEADER><CV_CONTENT>");

	xmlToCU += _T("<RESULT_CODE>");
#ifdef UNICODE
	(void)_itow((int)ulResultCode, str, 10);//lint !e1055 !e746
#else
	(void)_itoa((int)ulResultCode, str, 10);//lint !e1055 !e746
#endif

	xmlToCU +=str;
	xmlToCU += _T("</RESULT_CODE>");

	xmlToCU += _T("<HANDLE>");
#ifdef UNICODE
	(void)_itow((int)ulWndID, str, 10);//lint !e1055 !e746
#else
	(void)_itoa((int)ulWndID, str, 10);//lint !e1055 !e746
#endif
	xmlToCU +=str;
	xmlToCU += _T("</HANDLE>");

	xmlToCU +=_T("</CV_CONTENT></MESSAGE>");
}//lint !e1762

PostEventXML& PostEventXML::Instance()
{
	static PostEventXML PostEventXMLIm;
	return PostEventXMLIm;
}

IVS_VOID PostEventXML::GetBtnName( std::string &strBtnName ,ULONG ulBarType,ULONGLONG ulBtnTtype )
{
	std::map<ULONGLONG,std::string>::const_iterator iter;
	if (ulBarType == 1)
	{
		iter = m_mapAnalysisToolBarBtnName.find(ulBtnTtype);
	} else if (ulBarType == 2){
		iter = m_mapRealtimeToolBarBtnName.find(ulBtnTtype);
	}

	strBtnName.clear();
	strBtnName.append(static_cast<std::string>(iter->second).c_str());//lint !e69
}

//拼装OCX返回事件的XML
IVS_VOID PostEventXML::GetToolBarClickEventMsg(std::string &xml,const std::string &strEvent,ULONG ulWndID,const std::string &strStatus)
{
	xml.clear();
	xml.append(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
	xml.append(_T("<Content>"));
	xml.append(_T("<ToolbarButton>"));
	xml.append(strEvent.c_str());
	xml.append(_T("</ToolbarButton>"));
	xml.append(_T("<WndID>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(ulWndID)));
	xml.append(_T("</WndID>"));
	if (!strStatus.empty())
	{
		xml.append(_T("<ButtonStatus>"));
		xml.append(strStatus.c_str());
		xml.append(_T("</ButtonStatus>"));
	}
	xml.append(_T("</Content>"));
}//lint !e1762


IVS_VOID PostEventXML::GetInstantReturnEventMsg(std::string &xml,ULONG ulWndID)
{
	xml.clear();
	xml.append(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
	xml.append(_T("<CV_CONTENT>"));
	xml.append(_T("<WndID>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(ulWndID)));
	xml.append(_T("</WndID>"));
	xml.append(_T("</CV_CONTENT>"));
}//lint !e1762

//拼装OCX返回事件的XML
IVS_VOID PostEventXML::GetToolBarEMapClickEventMsg(std::string &xml,const std::string &strEvent,ULONG ulWndID,CRect& rect)
{
	xml.clear();
	xml.append(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
	xml.append(_T("<Content>"));
	xml.append(_T("<ToolbarButton>"));
	xml.append(strEvent.c_str());
	xml.append(_T("</ToolbarButton>"));
	xml.append(_T("<WndID>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(ulWndID)));
	xml.append(_T("</WndID>"));
	xml.append(_T("<Rect>"));
	xml.append(_T("<Left>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(rect.left)));
	xml.append(_T("</Left>"));
	xml.append(_T("<Top>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(rect.top)));
	xml.append(_T("</Top>"));
	xml.append(_T("<Width>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(rect.Width())));
	xml.append(_T("</Width>"));
	xml.append(_T("<Height>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(rect.Height())));
	xml.append(_T("</Height>"));
	xml.append(_T("</Rect>"));
	xml.append(_T("</Content>"));
}//lint !e1762

//拼装OCX返回事件的XML
IVS_VOID PostEventXML::GetLadderLayoutEventMsg(std::string &xml,ULONG ulWndNum)
{
	xml.clear();
	xml.append(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
	xml.append(_T("<Content>"));
	xml.append(_T("<MsgType>EVENT_LADDER_LAYOUT_WINDOW</MsgType>"));
	xml.append(_T("<WndNum>"));
	xml.append(CToolsHelp::Int2Str(static_cast<int>(ulWndNum)));
	xml.append(_T("</WndNum>"));
	xml.append(_T("</Content>"));
}//lint !e1762

IVS_VOID PostEventXML::GetVolumeEventMsg(CString &xmlToCU, ULONG ulWndID, ULONG ulEventType, ULONG ulVolumeValue) const
{
	IVS_CHAR str[TEMP_BUFFER_SIZE] = {0};

	xmlToCU  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xmlToCU += "<Content>";

	xmlToCU += "<WndID>";
	(void)_itoa_s((int)ulWndID, str, 10);
	xmlToCU +=str;
	xmlToCU += "</WndID>";

	xmlToCU += "<EventType>";
	(void)_itoa_s((int)ulEventType, str, 10);
	xmlToCU +=str;
	xmlToCU += "</EventType>";

	xmlToCU += "<VolumeValue>";
	(void)_ultoa_s(ulVolumeValue, str, 10);
	xmlToCU +=str;
	xmlToCU += "</VolumeValue>";

	xmlToCU +="</Content>";

}

IVS_VOID PostEventXML::GetFullSCreenXML(std::string &xmlToCU, ULONG ulResultCode, ULONG ulWndID )
{
	xmlToCU.clear();
	xmlToCU.append(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
	xmlToCU.append(_T("<Content>"));
	xmlToCU.append(_T("<ResultCode>"));
	xmlToCU.append(CToolsHelp::Int2Str(static_cast<int>(ulResultCode)));
	xmlToCU.append(_T("</ResultCode>"));
	xmlToCU.append(_T("<Handle>"));
	xmlToCU.append(CToolsHelp::Int2Str(static_cast<int>(ulWndID)));
	xmlToCU.append(_T("</Handle>"));
	xmlToCU.append(_T("</Content>"));
}//lint !e1762
