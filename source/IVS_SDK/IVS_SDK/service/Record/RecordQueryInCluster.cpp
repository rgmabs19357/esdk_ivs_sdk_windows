#include "RecordQueryInCluster.h"
#include "ToolHelp.h"
#include "IVS_Trace.h"

#define FORMAT_TIME(t, buf)                                   \
	do {                                                      \
struct tm _tm;                                         \
	localtime_s(&_tm, &(t));                                  \
	_snprintf(buf, 14, "%04d%02d%02d%02d%02d%02d",         \
	_tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, \
	_tm.tm_hour, _tm.tm_min, _tm.tm_sec);            \
	} while (0)



#define TIME_INTERVAL_HOUR 6
#define TIME_SPAN_INTERVAL (3600*TIME_INTERVAL_HOUR) //分段查询时时间间隔
CRecordQueryInCluster::CRecordQueryInCluster()
	:m_uiFromIndex(0)
	,m_uiToIndex(0)
	,m_uiFromIndexReMove(0)
	,m_uiToIndexReMove(0)
	,m_StarQueryTime(0)
	,m_EndQueryTime(0)
    ,uiRealNum(0)
	
{
	m_pUserMgr = NULL;
}
CRecordQueryInCluster::~CRecordQueryInCluster()
{
    m_pUserMgr = NULL;
}

void CRecordQueryInCluster::SetUserMgr( CUserMgr *pUserMgr)
{
	m_pUserMgr = pUserMgr;
}
void CRecordQueryInCluster::SetIndexRange(const IVS_INDEX_RANGE* pIndexRange)
{
	if (NULL != pIndexRange)
	{
		m_uiFromIndex = pIndexRange->uiFromIndex;
		m_uiToIndex =  pIndexRange->uiToIndex;
	}
}

IVS_INT32 CRecordQueryInCluster::GetTimeSpan(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_TIME_SPAN& stTimeSpan)
{
	CHECK_POINTER(pUnifiedQuery,IVS_OPERATE_MEMORY_ERROR);
	IVS_UINT32 iFieldNum = static_cast <IVS_UINT32>(pUnifiedQuery->iFieldNum);
	for (IVS_UINT32 j = 0; j < iFieldNum; j++)
	{
	    if (QUERY_OPERATION_START_TIME == pUnifiedQuery->stQueryField[j].eFieID)
	    {
			(void)CToolsHelp::Memcpy(stTimeSpan.cStart, IVS_TIME_LEN, &pUnifiedQuery->stQueryField[j].cValue,IVS_TIME_LEN);//lint !e545
			m_StarQueryTime = str2time(stTimeSpan.cStart);
		}
	    if (QUERY_OPERATION_END_TIME == pUnifiedQuery->stQueryField[j].eFieID)
	    {
			(void)CToolsHelp::Memcpy(stTimeSpan.cEnd, IVS_TIME_LEN, &pUnifiedQuery->stQueryField[j].cValue,IVS_TIME_LEN); //lint !e545
			 m_EndQueryTime = str2time(stTimeSpan.cEnd);
	    }
	}
	return IVS_SUCCEED;
}

//修改查询的区间，默认开始为1
IVS_INT32 CRecordQueryInCluster::UpDateQueryIndex(std::string& strQueryXml)
{
	unsigned int uiFromIndex = strQueryXml.find("<FromIndex>");
	unsigned int uiEndIndex = strQueryXml.find("</FromIndex>");
	strQueryXml.replace(uiFromIndex + strlen("<FromIndex>") ,uiEndIndex - (uiFromIndex + strlen("<FromIndex>")),"1");
	return IVS_SUCCEED;
}//lint !e1762


IVS_INT32 CRecordQueryInCluster::GetRecordInCluster(CSendNssMsgInfo& sendNssMsgInfo,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,std::string& strSendXml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUnifiedQuery,IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	IVS_TIME_SPAN timeSpan;
	memset(&timeSpan,0,sizeof(IVS_TIME_SPAN));
	iRet = GetTimeSpan(pUnifiedQuery,timeSpan);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Time Span ","Get Time Span Error");
		return iRet;
	}
	SetIndexRange(&pUnifiedQuery->stIndex);
	//生成获取摄像机轨迹的XML
	CXml xml;
	iRet = CRecordXMLProcess::GetCamerTraceXML(&timeSpan,sendNssMsgInfo.GetCameraCode(),xml);
	if (IVS_SUCCEED != iRet)//lint !e774 上面放开后这个去掉
	{
		BP_RUN_LOG_ERR(iRet, "Send Cmd by ReDirect","Build Cmd Error");
		return iRet;
	}
	unsigned int uiLen = 0;
	const char* pszData = xml.GetXMLStream(uiLen);
	CHECK_POINTER(pszData, IVS_OPERATE_MEMORY_ERROR);
	char *pReqData = IVS_NEW(pReqData,uiLen);
	if (NULL == pReqData)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Send Cmd by ReDirect","Alloc ReqData Error");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	if(!CToolsHelp::Memcpy(pReqData,uiLen,pszData,uiLen))
	{
		IVS_DELETE(pReqData, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Send Cmd by ReDirect", "ReqData Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	m_strCamerCode = sendNssMsgInfo.GetCameraCode();
	//创建查找摄像机轨迹消息类
	CSendNssMsgInfo stSendNssInfo;
	stSendNssInfo.SetReqID(NSS_CLUSTER_GET_CAMERA_TRACE_REQ);
	stSendNssInfo.SetNeedXml(TYPE_NSS_XML);
	stSendNssInfo.SetNetElemType(NET_ELE_SCU_NSS);
	stSendNssInfo.SetCameraCode(sendNssMsgInfo.GetCameraCode());
	stSendNssInfo.SetReqData(pReqData);
	std::string strSendDomainCode;
	std::string strDomainCode = sendNssMsgInfo.GetDomainCode();
	bool bRet = m_pUserMgr->GetDomainRouteMgr().FindProxyDomainCode(strDomainCode, strSendDomainCode);
	// 如果未找到代理域，使用目标域代替;
	if (!bRet)
	{
		strSendDomainCode = strDomainCode;
	}
	//设置最终发送的域
	stSendNssInfo.SetDomainCode(strSendDomainCode);
	iRet = GetCamerTraceInfo(stSendNssInfo);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pReqData, MUILI);
		BP_RUN_LOG_ERR(iRet, "Get Camer Trace Info", "Get Camer Trace Info Error");
		return iRet;
	}

	//首先清空要返回的结果
	strSendXml.clear();
	iRet = SendRecordListInfo(sendNssMsgInfo,strSendXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Record List Info", "Get Record List Info Error");
	}
	IVS_DELETE(pReqData, MUILI);
	return iRet;
}

IVS_INT32 CRecordQueryInCluster::GetCamerTraceInfo(CSendNssMsgInfo& sendNssMsgInfo)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;
	if (NULL == m_pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get Camer Trace Info","User Info is NULL");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	std::string strpRsp;
	sendNssMsgInfo.SetReqID(NSS_CLUSTER_GET_CAMERA_TRACE_REQ);
	//向SCU发送获取摄像机轨迹信息,获取摄像机历史轨迹的请求消息，也就是根据时间段查找摄像机曾经出现在那些NVR上
	iRet = m_pUserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);//lint !e1013 !e1055 !e746 !e526 !e628
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Camer Trace Info","Get Camer Trace Info Error");
		return iRet;
	}
	//解析数据
	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());
	//解析查询请求XML
	if (IVS_SUCCEED != CRecordXMLProcess::ParseCamerTraceInfo(xmlRsp,m_nvrList))
	{
		BP_RUN_LOG_ERR(iRet,"Get Camer Trace Info","Parse Camer Trace Info Error");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

IVS_INT32 CRecordQueryInCluster::ParseRecordSegmentInfo(CXml& xml)
{
	IVS_DEBUG_TRACE("");
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML","no 'Content' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	if (!xml.FindElem("TimeSegList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "no 'ResultList' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	std::string strSegIndex;
	std::string strRecordNum;

	TIME_SPAN_ALL_NUM_MAP_ITER iIterAllNum;
	TIME_SPAN_ALL_NUM_MAP_ITER iIterAllNumEnd = m_timeSpanAllNumMap.end();

	if (xml.FindElemEx("Content/TimeSegList/TimeSegInfo"))
	{
		do 
		{
			xml.IntoElem();
            const IVS_CHAR* pSeqIndex = xml.GetElemValue();
            if (NULL != pSeqIndex)
            {
                strSegIndex = pSeqIndex;
            }
			(void)xml.NextElem();
            const IVS_CHAR* pRecordNum = xml.GetElemValue();
            if (NULL != pRecordNum)
            {
                strRecordNum = pRecordNum;
            }

			xml.OutOfElem();
			int segIndex = CToolHelp::StrToInt(strSegIndex.c_str());
			int vedionum = CToolHelp::StrToInt(strRecordNum.c_str());
			time_t timeKey =m_StarQueryTime+(time_t)((segIndex-1)*TIME_SPAN_INTERVAL);//lint !e647

			iIterAllNum = m_timeSpanAllNumMap.find(timeKey);
			//累加所有录像记录到总表中
			if (iIterAllNum == iIterAllNumEnd)
			{
				(void)m_timeSpanAllNumMap.insert(std::make_pair(timeKey,vedionum));
			}
			else
			{
				//如果存在相同主键的值，则对里面的录像个数进行累加
				iIterAllNum->second += vedionum;//lint !e64 !e128
			}
		} while (xml.NextElem());            
	}
	return IVS_SUCCEED;
}

IVS_INT32 CRecordQueryInCluster::GetRecordSegment(CSendNssMsgInfo& sendNssMsgInfo,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	//生成获取摄像机轨迹的XML
	CXml xml;
	std::string strDomainCode = sendNssMsgInfo.GetDomainCode();
	iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery,xml,strDomainCode.c_str());
	if (IVS_SUCCEED != iRet)//lint !e774 上面放开后这个去掉
	{
		BP_RUN_LOG_ERR(iRet, "Send Cmd by ReDirect","Build Cmd Error");
		return iRet;
	}
	unsigned int uiLen = 0;
	const char* pszData = xml.GetXMLStream(uiLen);
	CHECK_POINTER(pszData, IVS_OPERATE_MEMORY_ERROR);
	//生成消息体
	CSendNssMsgInfo stSendNssInfo;
	stSendNssInfo.SetReqID(NSS_STAT_RECORD_SEGMENT_REQ);
	stSendNssInfo.SetNeedXml(TYPE_NSS_XML);
	stSendNssInfo.SetNetElemType(NET_ELE_SCU_NSS);
	stSendNssInfo.SetDomainCode(sendNssMsgInfo.GetDomainCode());
	stSendNssInfo.SetCameraCode(sendNssMsgInfo.GetCameraCode());
	stSendNssInfo.SetReqData(pszData);
	std::string strReqData = stSendNssInfo.GetReqData();
	(void)UpDateQueryIndex(strReqData);
	//准备发送指令并解析
	std::string strLinkID;
	std::string strNSSRsp;

	NVR_LIST_ITER beginIter = m_nvrList.begin();
	NVR_LIST_ITER endIter = m_nvrList.end();
	CLinkInfo oLinkIdInfo;
	bool bRet = false;
	for (;beginIter != endIter; beginIter++ )
	{
		bRet = CLinkRouteMgr::instance().GetScuLinkByDomainCodeAndNVRCode(m_pUserMgr->GetSessionID(),strDomainCode,*beginIter,oLinkIdInfo);
		if (bRet && LINKID_STATUS_OPENED == oLinkIdInfo.GetLinkStatus())
		{
			strLinkID = oLinkIdInfo.GetStrLinkId();
			iRet = IVS_SUCCEED;
		}
		else
		{
			iRet = m_pUserMgr->GetLoginMgr().LoginScu(strDomainCode,*beginIter,strLinkID);
		}
		if (IVS_SUCCEED != iRet)
		{
			continue;
		}
		strNSSRsp.clear();
		iRet = m_pUserMgr->SendSynCmd(stSendNssInfo,strLinkID,strNSSRsp);//lint !e1013 !e1055 !e746 !e526 !e628
		if (IVS_SUCCEED != iRet)
		{
			continue;
		}
		//解析数据
		CXml xmlRsp;
		(void)xmlRsp.Parse(strNSSRsp.c_str());
		iRet = ParseRecordSegmentInfo(xmlRsp);
	}
	return IVS_SUCCEED;
}

IVS_INT32 CRecordQueryInCluster::SendRecordListInfo(CSendNssMsgInfo& sendNssMsgInfo,std::string& strSendXml)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;
	std::string strCmdXML = sendNssMsgInfo.GetReqData();
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SCU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_LIST_REQ);

	//准备发送指令并解析
	std::string strLinkID;
	std::string strNSSRsp;
	if (NULL == m_pUserMgr)
	{
		return IVS_OPERATE_MEMORY_ERROR;
	}
	std::string strDomainCode = sendNssMsgInfo.GetDomainCode();
	NVR_LIST_ITER iterNvrBegin = m_nvrList.begin();
	NVR_LIST_ITER iterNvrEnd = m_nvrList.end();
	CLinkInfo oLinkIdInfo;
	bool bRet = false;
	for (;iterNvrBegin != iterNvrEnd ;)
	{
		(void)AssembleDomainCodeXml(strCmdXML,*iterNvrBegin);
		(void)UpDateQueryIndex(strCmdXML);
		sendNssMsgInfo.SetReqData(strCmdXML);

		bRet = CLinkRouteMgr::instance().GetScuLinkByDomainCodeAndNVRCode(m_pUserMgr->GetSessionID(),strDomainCode,*iterNvrBegin,oLinkIdInfo);
		if (bRet && LINKID_STATUS_OPENED == oLinkIdInfo.GetLinkStatus())
		{
			strLinkID = oLinkIdInfo.GetStrLinkId();
			iRet = IVS_SUCCEED;
		}
		else
		{
			iRet = m_pUserMgr->GetLoginMgr().LoginScu(strDomainCode,*iterNvrBegin,strLinkID);
		}
		if (IVS_SUCCEED != iRet)
		{
			m_nvrList.erase(iterNvrBegin++);
			continue;
		}
		strNSSRsp.clear();
		iRet = m_pUserMgr->SendSynCmd(sendNssMsgInfo,strLinkID,strNSSRsp);//lint !e1013
		m_nvrList.erase(iterNvrBegin++);
		if (IVS_SUCCEED != iRet)
		{
			continue;
		}
		(void)GetRecordListRSP(strNSSRsp,strSendXml);
	}
	if (!strNSSRsp.empty())
	{
		return IVS_SUCCEED;
	}
	//如果未查询到任何数据，返回为查询到数据的XML
	strSendXml = AssembleRspXml();
	return IVS_SUCCEED;
}

int CRecordQueryInCluster::GetRecordListRSP(const std::string& strXml,std::string& strSendXml)
{
	IVS_DEBUG_TRACE("");
	unsigned int uiBegin = strXml.find("<RecordDataList>");
	uiBegin += strlen("<RecordDataList>"); //lint !e737
	unsigned int uiEnd   = strXml.find("</RecordDataList>");
	std::string strXmlRecord = strXml.substr(uiBegin,uiEnd-uiBegin);
	m_strRspXml.append(strXmlRecord.c_str());
	//若m_nvrList已经为空，将组装后的消息发给
	if(!m_nvrList.empty())
	{
		return IVS_SUCCEED;
	}
	m_strRspXml.append("</RecordDataList></Content>");
	//将收集到来自各个scu的录像信息排序，删除重复段，并组装消息发给sdk
	(void)SortAndSetRecordInfo(m_strRspXml);
	//(void)DeleteOverlapTimeSpan();
	strSendXml = AssembleRspXml();
	return IVS_SUCCEED;
}


int CRecordQueryInCluster::SortAndSetRecordInfo(const std::string& strRecordInfo)
{
	IVS_DEBUG_TRACE("");
	std::string strRecordInfoBack = strRecordInfo;
	unsigned int uiBeginIndex = 0;
	unsigned int uiEndIndex = 0;
	uiBeginIndex = strRecordInfoBack.find("<RecordDataInfo>");
	uiEndIndex   = strRecordInfoBack.find("</RecordDataInfo>");
	while(uiBeginIndex != std::string::npos 
		&& uiEndIndex != std::string::npos )
	{
		unsigned int sublen = uiEndIndex + strlen("</RecordDataInfo>") - uiBeginIndex;
		std::string strRecordInfoTmp = strRecordInfoBack.substr(uiBeginIndex,sublen);
		std::string strKeyValue;
		(void)GetTimeFromRecordInfo(strRecordInfoTmp,strKeyValue);
		(void)m_ReacordInfoStrMap.insert(std::make_pair(strKeyValue,strRecordInfoTmp));
		uiBeginIndex = strRecordInfoBack.find("<RecordDataInfo>",uiBeginIndex+1);
		uiEndIndex   = strRecordInfoBack.find("</RecordDataInfo>",uiEndIndex+1);
	}
	return IVS_SUCCEED;
}

//主键采用时间+NVRCode+录像类型的方式给出
IVS_INT32 CRecordQueryInCluster::GetTimeFromRecordInfo(const std::string& strRecordInfo,std::string& strKeyValue)
{
	unsigned int uiBeginIndex = strRecordInfo.find("<StartTime>");
	unsigned int uiEndIndex   = strRecordInfo.find("</StartTime>");
	uiBeginIndex += strlen("<StartTime>");
	string strStartTime = strRecordInfo.substr(uiBeginIndex,uiEndIndex-uiBeginIndex);

	uiBeginIndex = strRecordInfo.find("<NVRCode>");
	uiEndIndex   = strRecordInfo.find("</NVRCode>");
	uiBeginIndex += strlen("<NVRCode>");
	string strNvrCode = strRecordInfo.substr(uiBeginIndex,uiEndIndex-uiBeginIndex);

	uiBeginIndex = strRecordInfo.find("<RecordType>");
	uiEndIndex   = strRecordInfo.find("</RecordType>");
	uiBeginIndex += strlen("<RecordType>");
	string strRecordType = strRecordInfo.substr(uiBeginIndex,uiEndIndex-uiBeginIndex);
	do 
	{
		//如果为计划录像
		if (0 == strRecordType.compare("010"))
		{
			strRecordType = "001";
			break;
		}
		//如果为告警录像
		if (0 == strRecordType.compare("001"))
		{
			strRecordType = "010";
			break;
		}
	} while (false); //lint !e717

	//生成Map的主键
	strKeyValue.append(strStartTime);
	strKeyValue.append(strNvrCode);
	strKeyValue.append(strRecordType);
	return IVS_SUCCEED;

}//lint !e1762


IVS_INT32 CRecordQueryInCluster::DeleteOverlapTimeSpan()
{
	//通过map的插入，可以将录像文件按照开始时间从小到大排序，然后我们
	//删除那些重复段
	REACORD_INFO_MAP_ITER ilter = m_ReacordInfoMap.begin();
	while(ilter != m_ReacordInfoMap.end())
	{
		CRecordData* pCRecordInfo = &(ilter->second);
		if (NULL == pCRecordInfo)//lint !e774
		{
			ilter++;
			continue;
		}
		//保存当前处理recordinfo
		REACORD_INFO_MAP_ITER tmpIlter = ilter;
		ilter++;
		if( ilter == m_ReacordInfoMap.end() )
		{
			//删除重复段结束
			break;
		}
		CRecordData* pCRecordInfoNext = &(ilter->second);
		if(pCRecordInfoNext->m_start > pCRecordInfo->m_end )
		{
			//这两段没有重复段
			continue;
		}
		if( pCRecordInfoNext->m_start <= pCRecordInfo->m_end)
		{
			if( pCRecordInfoNext->m_end <= pCRecordInfo->m_end )
			{
				m_ReacordInfoMap.erase(ilter);
			}
			else
			{
				pCRecordInfoNext->m_start = pCRecordInfo->m_end + 1;
				(void)SetTimeToRecordInfo(pCRecordInfoNext->m_strRecordInfo,pCRecordInfoNext->m_start,pCRecordInfoNext->m_end);
			}
		}
		//指向刚刚处理过的recordinfo的下一个recordinfo元素
		ilter = tmpIlter++;
	}
	return IVS_SUCCEED;
}


std::string CRecordQueryInCluster::AssembleRspXml()
{
	std::string strRspXml = "";
	//组装xml消息
	strRspXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><Content><PageInfo>";
	strRspXml.append("<RealNum>");
	strRspXml.append(CToolHelp::IntToStr(m_ReacordInfoStrMap.size()));//lint !e713
	strRspXml.append("</RealNum>");
	strRspXml.append("<FromIndex>");
	strRspXml.append(CToolHelp::IntToStr(m_uiFromIndex));//lint !e713
	strRspXml.append("</FromIndex>");
	strRspXml.append("<ToIndex>");
	strRspXml.append(CToolHelp::IntToStr(m_uiToIndex));//lint !e713
	strRspXml.append("</ToIndex>");
	strRspXml.append("</PageInfo>");

	strRspXml.append("<CameraCode>");
	strRspXml.append(m_strCamerCode.c_str());
	strRspXml.append("</CameraCode>");

	strRspXml.append("<RecordDataList>");
	//从map中读取数据 并组装xml字符串
	REACORD_INFO_STR_MAP_ITER ilter = m_ReacordInfoStrMap.begin();
	REACORD_INFO_STR_MAP_ITER iEndIter = m_ReacordInfoStrMap.end();
	unsigned int uiFromIndex = 1;

	while(ilter != iEndIter)
	{
		if (uiFromIndex >= m_uiFromIndex)
		{
			strRspXml.append(ilter->second);//lint !e1025
			ilter++;
		}
		if(uiFromIndex == m_uiToIndex)
		{
			break;
		}
		uiFromIndex++;
	}
	strRspXml.append("</RecordDataList></Content>");
	return strRspXml;//lint !e1036
}

IVS_INT32 CRecordQueryInCluster::SetTimeToRecordInfo(std::string& strRecordInfo,time_t& start,time_t& end)
{
	unsigned int uiBeginIndex = strRecordInfo.find("<StartTime>");
	unsigned int uiEndIndex   = strRecordInfo.find("</StartTime>");
	uiBeginIndex      += strlen("<StartTime>");
	char cStartTime[256];
	memset(cStartTime,0,sizeof(cStartTime));
	//string strYmdhmsStart = time2str(StartTime,256,start+TIMEADD);
	 FORMAT_TIME(start,cStartTime);
	std::string strYmdhmsStart = cStartTime;// FormatTime(StartTime,256,start,"YYYYMMDDhhmmss");
	strRecordInfo.replace(uiBeginIndex,uiEndIndex-uiBeginIndex,strYmdhmsStart);
	uiBeginIndex = strRecordInfo.find("<EndTime>");
	uiEndIndex   = strRecordInfo.find("</EndTime>");
	uiBeginIndex      += strlen("<EndTime>");
	char cEndTime[256];
	memset(cEndTime,0,sizeof(cEndTime));
	//string strYmdhmsEnd = time2str(EndTime,256,end+TIMEADD);
	FORMAT_TIME(end,cEndTime);
	std::string strYmdhmsEnd = cEndTime;//FormatTime(EndTime,256,end,"YYYYMMDDhhmmss");
	strRecordInfo.replace(uiBeginIndex,uiEndIndex-uiBeginIndex,strYmdhmsEnd);
	return IVS_SUCCEED;
}//lint !e1762 !e1764


int CRecordQueryInCluster::AssembleTimeSpanXml(std::string& strXml)
{
	unsigned int fromTimeIndex = strXml.find("FromTime");
	unsigned int fromTimeValue1 = strXml.find("<Value>",fromTimeIndex);
	unsigned int fromTimeValue2 = strXml.find("</Value>",fromTimeIndex);
	fromTimeValue1 += strlen("<Value>");

	char StartTime[256];
	memset(StartTime,0,sizeof(StartTime));
	FORMAT_TIME(m_StarQueryTime,StartTime);
	std::string strYmdhmsStart = StartTime;
	strXml.replace(fromTimeValue1,fromTimeValue2-fromTimeValue1,strYmdhmsStart);

	unsigned int toTimeIndex = strXml.find("ToTime");
	unsigned int toTimeValue1 = strXml.find("<Value>",toTimeIndex);
	unsigned int toTimeValue2 = strXml.find("</Value>",toTimeIndex);
	toTimeValue1 += strlen("<Value>");

	char EndTime[256];
	memset(EndTime,0,sizeof(EndTime));
	FORMAT_TIME(m_EndQueryTime,EndTime);
	std::string strYmdhmsEnd = EndTime;
	strXml.replace(toTimeValue1,toTimeValue2-toTimeValue1,strYmdhmsEnd);
	return IVS_SUCCEED;
}//lint !e1762

int CRecordQueryInCluster::AssembleDomainCodeXml(std::string& strXml,std::string& strDoaminCode)
{
	unsigned int fromDomainIndex = strXml.find("<DomainCode>");
	unsigned int fromDomainEndIndex = strXml.find("</DomainCode>");
	strXml.replace(fromDomainIndex + strlen("<DomainCode>") ,fromDomainEndIndex - (fromDomainIndex + strlen("<DomainCode>")),strDoaminCode);
	return IVS_SUCCEED;
}//lint !e1764 !e1762
