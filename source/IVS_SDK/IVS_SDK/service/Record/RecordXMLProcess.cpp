#include "RecordXMLProcess.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"
#include "MediaBase.h"


IVS_INT32 CRecordXMLProcess::GetRecordSegmentXML(const IVS_TIME_SPAN* pTimeSpan,const std::string& strCamerCode,IVS_INT32 iHoursPerSeg,CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pTimeSpan,IVS_OPERATE_MEMORY_ERROR);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCamerCode.c_str());

	(void)xml.AddElem("StartTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pTimeSpan->cStart);

	(void)xml.AddElem("EndTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pTimeSpan->cEnd);

	(void)xml.AddElem("HoursPerSeg");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iHoursPerSeg)).c_str());

	xml.OutOfElem();
	return IVS_SUCCEED;
}

IVS_INT32 CRecordXMLProcess::GetCamerTraceXML(const IVS_TIME_SPAN* pTimeSpan,const std::string& strCamerCode,CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pTimeSpan,IVS_OPERATE_MEMORY_ERROR);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("CameraInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCamerCode.c_str());
	xml.OutOfElem();
	(void)xml.AddElem("FromTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pTimeSpan->cStart);
	(void)xml.AddElem("ToTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pTimeSpan->cEnd);
	xml.OutOfElem();
	return IVS_SUCCEED;
}

IVS_INT32 CRecordXMLProcess::ParseCamerTraceInfo(CXml& xml,std::list<std::string>& nvrList)
{
	
	IVS_DEBUG_TRACE("");
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML","no 'Content' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	if (!xml.FindElem("NVRList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "no 'ResultList' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	std::string strNvr;
	const char*AttriValue = NULL;   //临时存储单个节点值
	char szNvrCode[IVS_NVR_CODE_LEN+1];
	if (xml.FindElemEx("Content/NVRList/NVRInfo"))
	{
		do 
		{
			xml.IntoElem();
			memset(szNvrCode,0,IVS_NVR_CODE_LEN+1);
			GET_ELEM_VALUE_CHAR("NVRCode",AttriValue,szNvrCode,IVS_NVR_CODE_LEN,xml);
			strNvr = szNvrCode ;
			nvrList.push_back(strNvr);
			xml.OutOfElem();
		} while (xml.NextElem());            

	}
	return IVS_SUCCEED;
}

//增加录像空间
IVS_INT32 CRecordXMLProcess::AddRecordSpaceGetXML(const IVS_RECORD_POLICY_TIME* pRecordSpace,const IVS_CHAR* pCameraCode,CXml &xml) //lint !e1762
{
	CHECK_POINTER(pRecordSpace, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("NodeCode");
	(void)xml.IntoElem();
	(void)xml.AddElem("StoreType");
	(void)xml.AddElem("StoreSpace");
	(void)xml.AddElem("GlobalFullPolicy");

	return IVS_SUCCEED;
}
//查询告警类型列表拼接xml
IVS_INT32 CRecordXMLProcess::GetRecordSpaceGetXML(const IVS_RECORD_POLICY_TIME* pRecordSpaceInfo,CXml &xml)
{
    CHECK_POINTER(pRecordSpaceInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("NodeList");
	(void)xml.IntoElem();
    (void)xml.AddChildElem("NodeCode");
	(void)xml.IntoElem();
    //(void)xml.SetElemValue(pRecordSpaceInfo->cCode);
	xml.OutOfElem();

	return IVS_SUCCEED;
}
//查询录像策略拼接xml
IVS_INT32 CRecordXMLProcess::GetRecordPolicyGetXML(CXml &xml,const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode)
{
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pCameraCode);
	(void)xml.AddElem("DomainCode");
	(void)xml.SetElemValue(pDomainCode);
	xml.OutOfElem();

	return IVS_SUCCEED;
}
//查询录像计划的请求XML
IVS_INT32 CRecordXMLProcess::GetRecordPlanParseRSP(IVS_RECORD_PLAN* pRecordPlan, CXml& xml)
{
	CHECK_POINTER(pRecordPlan,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElemEx("Content/Plan"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "ErrorCode =%d",IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}

	// RecordMethod
	if (!xml.FindElemEx("Content/Plan/RecordMethod"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "ErrorCode =%d", IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}
	const char* pszRecordMethod = xml.GetElemValue();
	if (NULL == pszRecordMethod)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "ErrorCode =%d", IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}
	pRecordPlan->uiRecordMethod = static_cast<IVS_UINT32>(atoi(pszRecordMethod));

	// IsEnable
	if (!xml.FindElemEx("Content/Plan/IsEnable"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "ErrorCode =%d", IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}
	const char* pszIsEnable = xml.GetElemValue();
	if (NULL == pszIsEnable)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "ErrorCode =%d", IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}
	pRecordPlan->bEnableRecordPlan = static_cast<IVS_BOOL>(atoi(pszIsEnable));

	// PlanType
	if (!xml.FindElemEx("Content/Plan/PlanType"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "ErrorCode =%d", IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}
	const char* pszPlanType = xml.GetElemValue();
	if (NULL == pszPlanType)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "ErrorCode =%d", IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}
	pRecordPlan->uiRecordPlanType = static_cast<IVS_UINT32>(atoi(pszPlanType));

	// PlanList
	//if (!xml.FindElemEx("Content/Plan/PlanList"))
	//{
	// BP_RUN_LOG_ERR(IVS_XML_INVALID,"CRecordXMLProcess::GetRecordPlanParseRSP", "ErrorCode =%d", IVS_XML_INVALID);
	// return IVS_XML_INVALID;
	//}
	//(void)xml.IntoElem();
	IVS_INT32 iPlanInfoNum = 0;
	IVS_INT32 iTimeInfoNum = 0;

	const char* szElemValue = NULL;
	if (xml.FindElemEx("Content/Plan/PlanList"))
	{
		do 
		{
			xml.IntoElem();
			//解析PlanInfo
			(void)xml.IntoElem();
			memset(&pRecordPlan->stPlanInfo[iPlanInfoNum],0, sizeof(IVS_PLAN_INFO));
			GET_ELEM_VALUE_NUM_FOR_UINT("DayType",szElemValue,pRecordPlan->stPlanInfo[iPlanInfoNum].uiDayType,xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("TimeInfoNum",szElemValue,pRecordPlan->stPlanInfo[iPlanInfoNum].uiTimeSpanInfoNum,xml);

			if (xml.FindElem("TimeList"))
			{
				xml.IntoElem();
			}
			if(xml.FindElem("TimeInfo") && pRecordPlan->stPlanInfo[iPlanInfoNum].uiTimeSpanInfoNum > 0)
			{
				do{ 
					(void)xml.IntoElem();
					GET_ELEM_VALUE_CHAR("StartTime",szElemValue,  pRecordPlan->stPlanInfo[iPlanInfoNum].stTimeSpanInfo[iTimeInfoNum].cStart,IVS_TIME_LEN,xml);
					GET_ELEM_VALUE_CHAR("EndTime",szElemValue,  pRecordPlan->stPlanInfo[iPlanInfoNum].stTimeSpanInfo[iTimeInfoNum].cEnd,IVS_TIME_LEN,xml);
					iTimeInfoNum++;
					xml.OutOfElem();
				}while(xml.NextElem());
			}

			xml.OutOfElem();
			xml.OutOfElem();
			iPlanInfoNum++;
			iTimeInfoNum = 0;
		} while(xml.NextElem());
	}
	pRecordPlan->uiPlanInfoNum = static_cast<IVS_UINT32>(iPlanInfoNum);
	xml.OutOfElem();

	return IVS_SUCCEED;
}
//设置录像策略拼接xml
IVS_INT32 CRecordXMLProcess::SetRecordPolicyGetXML(const IVS_RECORD_POLICY_TIME* pRecordPolicy ,const IVS_CHAR* pCameraCode,
	const IVS_CHAR* pDomainCode,CXml &xml)
{
	CHECK_POINTER(pRecordPolicy,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pCameraCode);
	(void)xml.AddElem("DomainCode");
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("RecordMode");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiRecordMode)).c_str()); 

	(void)xml.AddElem("AllocMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(0).c_str()); 

	(void)xml.AddElem("Time");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiTime)).c_str()); 

	(void)xml.AddElem("FrameExtractRecordTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiFrameExtractRecordTime)).c_str()); 

	(void)xml.AddElem("AlarmTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiAlarmTime)).c_str()); 

	(void)xml.AddElem("KeyframeTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiKeyframeTime)).c_str()); 

	(void)xml.AddElem("BeforeAlarm");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiBeforeAlarm)).c_str()); 

	(void)xml.AddElem("AfterAlarm");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiAfterAlarm)).c_str()); 

	(void)xml.AddElem("PlanStreamID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiPlanStreamType)).c_str()); 

	(void)xml.AddElem("AlarmStreamID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiAlarmStreamType)).c_str()); 

	(void)xml.AddElem("AlarmRecordTTL");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiAlarmRecordTTL)).c_str()); 

	(void)xml.AddElem("ManualRecordTTL");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiManualRecordTTL)).c_str()); 

	(void)xml.AddElem("PreRecord");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiPreRecord)).c_str()); 

	(void)xml.AddElem("PreRecordTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiPreRecordTime)).c_str()); 

	(void)xml.AddElem("AssociatedAudio");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->bAssociatedAudio)).c_str()); 

	return IVS_SUCCEED;
}
//增加录像空间
IVS_INT32 CRecordXMLProcess::AddRecordSpacePraseXML(CXml xml,const IVS_RECORD_POLICY_TIME* pRecordSpaceInfo)
{
	CHECK_POINTER(pRecordSpaceInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

    std::string strMsgType;
    //IVS_INT32 iRet = CXmlCommOperation::PraseXmlCommHead(xml,strMsgType);
    //if(IVS_SUCCEED != iRet)
    //{
    //    //IVS_LOG(LOG_ERR,"PraseXmlCommHead error in QueryVoiceDevRspXML");
    //    return iRet;
    //}

    //const IVS_CHAR* szElemValue = NULL;
    if (xml.FindElem("Content"))
    {
        (void)xml.IntoElem();
        if (xml.FindElem("NodeList"))
        {
            (void)xml.IntoElem();
            if (xml.FindElem("NodeInfo"))
            {
                (void)xml.IntoElem();
                /*GET_ELEM_VALUE_NUM("NodeCode",szElemValue, pRecordSpaceInfo.iNodeCode,xml);
                GET_ELEM_VALUE_NUM("NodeIP",szElemValue, pRecordSpaceInfo.iNodeIp,xml);
                GET_ELEM_VALUE_NUM("TotalSpace",szElemValue, pRecordSpaceInfo.iTotalSpace,xml);
                GET_ELEM_VALUE_NUM("RecordSpace",szElemValue, pRecordSpaceInfo.iRecordSpace,xml);
                GET_ELEM_VALUE_NUM("RecordUsedSpace",szElemValue, pRecordSpaceInfo.iRecordUsedSpace,xml);
                GET_ELEM_VALUE_NUM("RecordFullPolicy",szElemValue, pRecordSpaceInfo.iRecordFullPolicy,xml);
                GET_ELEM_VALUE_NUM("BackupSpace",szElemValue, pRecordSpaceInfo.iBackupSpace,xml);
                GET_ELEM_VALUE_NUM("BackupUsedSpace",szElemValue, pRecordSpaceInfo.iBackupUsedSpace,xml);
                GET_ELEM_VALUE_NUM("BackupFullPolicy",szElemValue, pRecordSpaceInfo.iBackupFullPolicy,xml);
                GET_ELEM_VALUE_NUM("MaxRecordWays",szElemValue, pRecordSpaceInfo.iMaxRecordWays,xml);
                GET_ELEM_VALUE_NUM("MaxReplayWays",szElemValue, pRecordSpaceInfo.iMaxReplayWays,xml);
                GET_ELEM_VALUE_NUM("UsedRecordWays",szElemValue, pRecordSpaceInfo.iUsedRecordWays,xml);
                GET_ELEM_VALUE_NUM("UsedReplayWays",szElemValue, pRecordSpaceInfo.iUsedReplayWays,xml);*/
            }
            xml.OutOfElem();
        }
        xml.OutOfElem();
    }
    
	return IVS_SUCCEED;
}

//录像检索的响应解析XML
IVS_INT32 CRecordXMLProcess::ParseGetRecordList(CXml& xml,IVS_RECORD_INFO_LIST* pRecordInfoList,IVS_RECORD_DATA_PLACE* pRecordData,IVS_UINT32 uiBufferSize,
	                                            IVS_UINT32& uiRecordNum,IVS_UINT32 uiFromIndex,IVS_UINT32 uiToIndex)
{
	CHECK_POINTER(pRecordInfoList, IVS_OPERATE_MEMORY_ERROR); 
	//CHECK_POINTER(pRecordData,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	const IVS_UINT32 ulRecordTypeLength = 5;
	int iRet = IVS_FAIL;
	if (!xml.FindElemEx("Content"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content/PageInfo) is fail");
		return iRet;
	}

	if (!xml.FindElemEx("Content/PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content/PageInfo) is fail");
		return IVS_XML_INVALID;
	}   

	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM("FromIndex", pTemp, pRecordInfoList->stIndexRange.uiFromIndex, xml);//lint !e732
	GET_ELEM_VALUE_NUM("ToIndex", pTemp, pRecordInfoList->stIndexRange.uiToIndex, xml);//lint !e732
	GET_ELEM_VALUE_NUM("RealNum",pTemp, pRecordInfoList->uiTotal, xml);//lint !e734 !e732

	IVS_UINT32 uiCurrentNum = 0;
	const char* szElemValue = NULL;
	bool bRet = false;
	IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1];
	memset(cCameraCode,0,IVS_DEV_CODE_LEN+1);
	if (xml.FindElemEx("Content/CameraCode"))
	{
		GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,cCameraCode,IVS_DEV_CODE_LEN,xml);
		//设置摄像机Code和记录条数
		if (NULL != pRecordData)
		{
			bRet = CToolsHelp::Memcpy(&(pRecordData->cCameraCode), IVS_DEV_CODE_LEN, &cCameraCode, IVS_DEV_CODE_LEN); //lint !e545
			pRecordData->uiTotal = pRecordInfoList->uiTotal;
		}
	}

	if (0 == pRecordInfoList->uiTotal)
	{
		return IVS_SUCCEED;
	}

	if (xml.FindElemEx("Content/RecordDataList/RecordDataInfo"))
	{
		do
		{
			IVS_RECORD_INFO sRecordInfo;
			memset(&sRecordInfo, 0, sizeof(IVS_RECORD_INFO));
			RECORD_DATA_PLACE stRecordDataPlace;
			memset(&stRecordDataPlace,0,sizeof(RECORD_DATA_PLACE));

			bRet = false;
			(void)xml.IntoElem();
			GET_ELEM_VALUE_CHAR("FileName",szElemValue,  sRecordInfo.cRecordFileName,IVS_FILE_NAME_LEN,xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,  sRecordInfo.uiRecordMethod,xml);

			GET_ELEM_VALUE_CHAR("NVRCode",szElemValue,stRecordDataPlace.cNVRCode,IVS_NVR_CODE_LEN,xml);
			
			GET_ELEM_VALUE_CHAR("NVRCode",szElemValue,  sRecordInfo.cNvrCode,IVS_NVR_CODE_LEN,xml);
			GET_ELEM_VALUE_CHAR("MBUDomain",szElemValue,  sRecordInfo.cMBUDomain,IVS_DOMAIN_CODE_LEN,xml);

			//录像类型需要转换下
			IVS_CHAR cRecordStatus[ulRecordTypeLength+1];
			memset(cRecordStatus,0,ulRecordTypeLength+1);
			GET_ELEM_VALUE_CHAR("RecordType",pTemp,cRecordStatus,ulRecordTypeLength,xml);
			IVS_UINT32 ulRecordType = CXmlProcess::TransBinStrToDec(cRecordStatus);
			sRecordInfo.uiRecordType = ulRecordType;
			
			GET_ELEM_VALUE_CHAR("AlarmType",szElemValue,  sRecordInfo.cAlarmType,IVS_ALARM_TYPE_LEN,xml);

			GET_ELEM_VALUE_NUM_FOR_UINT("IsFullRecord",szElemValue,  sRecordInfo.uiFrameExtractionTimes,xml);
			GET_ELEM_VALUE_CHAR("StartTime",szElemValue,  sRecordInfo.stTime.cStart,IVS_TIME_LEN,xml);
			GET_ELEM_VALUE_CHAR("EndTime",szElemValue,  sRecordInfo.stTime.cEnd,IVS_TIME_LEN,xml);
			stRecordDataPlace.iStartTime = (IVS_INT32)CMediaBase::UTCStr2Time(sRecordInfo.stTime.cStart, IVS_DATE_TIME_FORMAT);
			stRecordDataPlace.iEndTime = (IVS_INT32)CMediaBase::UTCStr2Time(sRecordInfo.stTime.cEnd, IVS_DATE_TIME_FORMAT);
	
			if (xml.FindElem("RecordBookmark"))
			{
				(void)xml.IntoElem();
				GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkID",szElemValue,  sRecordInfo.stBookmarkInfo.uiBookmarkID,xml);
				GET_ELEM_VALUE_CHAR("BookmarkName",szElemValue,  sRecordInfo.stBookmarkInfo.cBookmarkName,IVS_BOOKMARK_NAME_LEN,xml);
				GET_ELEM_VALUE_CHAR("BookmarkTime",szElemValue,  sRecordInfo.stBookmarkInfo.cBookmarkTime,IVS_TIME_LEN,xml);
				GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkCreatorID",szElemValue,  sRecordInfo.stBookmarkInfo.uiBookmarkCreatorID,xml);
				GET_ELEM_VALUE_CHAR("BookmarkCreatorName",szElemValue,  sRecordInfo.stBookmarkInfo.cBookmarkCreatorName,IVS_TIME_LEN,xml);
				GET_ELEM_VALUE_CHAR("BookmarkCreatorDomain",szElemValue,  sRecordInfo.stBookmarkInfo.cUserDomain,IVS_DOMAIN_CODE_LEN,xml);
				bRet = CToolsHelp::Memcpy(sRecordInfo.stBookmarkInfo.cCameraCode,IVS_DEV_CODE_LEN,cCameraCode,IVS_DEV_CODE_LEN);
				if(false == bRet)
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				//memcpy(sRecordInfo.stBookmarkInfo.cCameraName,cCameraName,IVS_NAME_LEN);
			    xml.OutOfElem();
			}

			if (xml.FindElem("LockInfo"))
			{
				(void)xml.IntoElem();
				GET_ELEM_VALUE_NUM_FOR_UINT("LockID",szElemValue,  sRecordInfo.stLockInfo.uiLockID,xml);
				GET_ELEM_VALUE_CHAR("LockDesc",szElemValue,  sRecordInfo.stLockInfo.cLockDesc,IVS_RECORD_LOCK_DESC_LEN,xml);
				GET_ELEM_VALUE_CHAR("OperatorName",szElemValue,  sRecordInfo.stLockInfo.cOperatorName,IVS_NAME_LEN,xml);
				GET_ELEM_VALUE_CHAR("LockTime",szElemValue,  sRecordInfo.stLockInfo.cLockTime,IVS_TIME_LEN,xml);
				GET_ELEM_VALUE_CHAR("LockStartTime",szElemValue,  sRecordInfo.stLockInfo.stLockTimeSpan.cStart,IVS_TIME_LEN,xml);
				GET_ELEM_VALUE_CHAR("LockEndTime",szElemValue,  sRecordInfo.stLockInfo.stLockTimeSpan.cEnd,IVS_TIME_LEN,xml);
			    xml.OutOfElem();
			}

			if (xml.FindElem("PresetInfo"))
			{
				(void)xml.IntoElem();
				GET_ELEM_VALUE_NUM_FOR_UINT("PresetID",szElemValue,  sRecordInfo.stPtzPresetInfo.uiID,xml);
				GET_ELEM_VALUE_CHAR("PresetName",szElemValue,  sRecordInfo.stPtzPresetInfo.cName,IVS_RECORD_PTZ_PRESET_NAME_LEN,xml);
			    xml.OutOfElem();
			}
			xml.OutOfElem();

			//添加尺寸校验
			IVS_UINT32 uiSize = sizeof(IVS_RECORD_INFO_LIST) + static_cast<IVS_UINT32>(uiRecordNum-1) * sizeof(IVS_RECORD_INFO);
			if (uiSize >= uiBufferSize)
			{
				break;
			}

			uiCurrentNum++;
			if((uiFromIndex == 0 && uiToIndex == 0 && uiRecordNum < pRecordInfoList->stIndexRange.uiToIndex)|| (/*uiCurrentNum>=uiFromIndex && */uiCurrentNum<=uiToIndex))
			{
				bRet = CToolsHelp::Memcpy(&(pRecordInfoList->stRecordInfo[uiRecordNum]), sizeof(IVS_RECORD_INFO), &sRecordInfo, sizeof(IVS_RECORD_INFO));
				if(false == bRet)
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				if (NULL != pRecordData)
				{
					bRet = CToolsHelp::Memcpy(&(pRecordData->stRecordData[uiRecordNum]), sizeof(RECORD_DATA_PLACE), &stRecordDataPlace, sizeof(RECORD_DATA_PLACE));
					if(false == bRet)
					{
						BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
						return IVS_ALLOC_MEMORY_ERROR;
					}
				}
				uiRecordNum++;
			}	
		}while(xml.NextElem());
	}

	return IVS_SUCCEED;
}

//添加/修改录像计划的请求XML
IVS_INT32 CRecordXMLProcess::AddOrModRecordPlanGetXML(const std::string& strSendDomainCode,DeviceCodeList& devList,const IVS_RECORD_PLAN* pRecordPlan, CXml &xml)
{
	CHECK_POINTER(pRecordPlan,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");//   ---- content
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strSendDomainCode.c_str());
	(void)xml.AddElem("CameraList");

	DeviceCodeListIpIter devListIter = devList.begin();
	DeviceCodeListIpIter devListIterEnd = devList.end();
	IVS_UINT32 i=0;
	for (; devListIter != devListIterEnd; devListIter++)
	{
		const IVS_DEVICE_CODE* pdevCode = *devListIter;
		CHECK_POINTER(pdevCode, IVS_PARA_INVALID);

		(IVS_VOID)CXmlProcess::ParseDevCode(pdevCode->cDevCode, chDevCode, chDomaCode);		
		if (i == 0)
		{
			(void)xml.AddChildElem("CameraCode");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(chDevCode);
		}else
		{      
			(void)xml.AddElem("CameraCode");
			(void)xml.SetElemValue(chDevCode);
		}
		i++;
	}
	xml.OutOfElem();

	(void)xml.AddElem("Plan");
	(void)xml.AddChildElem("RecordMethod");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->uiRecordMethod)).c_str()); 
	(void)xml.AddElem("PlanType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->uiRecordPlanType)).c_str()); 
	(void)xml.AddElem("IsEnable");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pRecordPlan->bEnableRecordPlan).c_str()); 
	if(pRecordPlan->uiRecordPlanType != PLAN_ALLDAY)
	{
		(void)xml.AddElem("PlanList");
		for (i=0; i<pRecordPlan->uiPlanInfoNum; i++)
		{
			if (0 == pRecordPlan->stPlanInfo[i].uiTimeSpanInfoNum || IVS_MAX_TIMESPAN_NUM < pRecordPlan->stPlanInfo[i].uiTimeSpanInfoNum)
			{
				BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Add Or Mod Record Plan Get XML", "Invalid TimeSpanInfoNum");
				return IVS_PARA_INVALID;
			}

			IVS_CHAR cStartTime[IVS_TIME_LEN+1] = {0};
			IVS_CHAR cEndTime[IVS_TIME_LEN+1] = {0}; 	
			if (i == 0)
			{        
				(void)xml.AddChildElem("PlanInfo");   
				(void)xml.IntoElem();   
			}
			else
			{
				(void)xml.AddElem("PlanInfo"); 
			}
			(void)xml.AddChildElem("TimeInfoNum");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->stPlanInfo[i].uiTimeSpanInfoNum)).c_str()); 
			(void)xml.AddElem("DayType"); 
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->stPlanInfo[i].uiDayType)).c_str());
			(void)xml.AddElem("TimeList"); 
			for (IVS_UINT32 j=0; j<pRecordPlan->stPlanInfo[i].uiTimeSpanInfoNum; j++)
			{
				if (j == 0)
				{
					(void)xml.AddChildElem("TimeInfo");
					(void)xml.IntoElem();
				}
				else{
					(void)xml.AddElem("TimeInfo");
				}
				if (!CToolsHelp::Memcpy(cStartTime, IVS_TIME_LEN,pRecordPlan->stPlanInfo[i].stTimeSpanInfo[j].cStart, IVS_TIME_LEN))
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Or Mod Record Plan Get XML", "Memcpy cStartTime failed");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				if (!CToolsHelp::Memcpy(cEndTime, IVS_TIME_LEN, pRecordPlan->stPlanInfo[i].stTimeSpanInfo[j].cEnd, IVS_TIME_LEN))
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Or Mod Record Plan Get XML", "Memcpy cEndTime failed");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				(void)xml.AddChildElem("StartTime");
				(void)xml.IntoElem();
				(void)xml.SetElemValue(cStartTime);
				(void)xml.AddElem("EndTime");
				(void)xml.SetElemValue(cEndTime); 
				xml.OutOfElem();
			}  
			xml.OutOfElem();
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();
	return IVS_SUCCEED;
}

IVS_INT32 CRecordXMLProcess::AddOrModExDomainRecordPlanGetXML(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
    const IVS_RECORD_PLAN* pRecordPlan, CXml &xml)
{
    CHECK_POINTER(pDeviceList,IVS_PARA_INVALID);
    CHECK_POINTER(pRecordPlan,IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("Add Or Mod ExDomain Record Plan Get XML.");

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
    (IVS_VOID)CXmlProcess::ParseDevCode(pDeviceList->cDevCode, chDevCode, chDomaCode);
	std::string strStartTime = "StartTime";
	std::string strEndTime = "EndTime";
	std::string strExDomainCode = "ExDomainCode";

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");//   ---- content
    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(chDomaCode);
    (void)xml.AddElem("CameraList");
    for (IVS_UINT32 i=0; i<uiDeviceNum; i++)
    {
		if (0 == i)
		{            
			(void)xml.AddChildElem("CameraInfo");
			(void)xml.IntoElem();
		}
		else
		{
			(void)xml.AddElem("CameraInfo");
		}

        memset(chDevCode,0,IVS_DEV_CODE_LEN+1);
        memset(chDomaCode,0,IVS_DOMAIN_CODE_LEN+1);
        (IVS_VOID)CXmlProcess::ParseDevCode(pDeviceList->cDevCode, chDevCode, chDomaCode);
        (void)xml.AddChildElem("CameraCode");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(chDevCode);
        (void)xml.AddElem(strExDomainCode.c_str());
        (void)xml.SetElemValue(chDomaCode);
        xml.OutOfElem();
        pDeviceList++;
    }
    xml.OutOfElem();

    (void)xml.AddElem("Plan");
    (void)xml.AddChildElem("RecordMethod");
    (void)xml.IntoElem();//------------5
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->uiRecordMethod)).c_str()); 
    (void)xml.AddElem("IsEnable");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRecordPlan->bEnableRecordPlan).c_str()); 
    (void)xml.AddElem("PlanType");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->uiRecordPlanType)).c_str()); 
    if(pRecordPlan->uiRecordPlanType !=2)
    {
        (void)xml.AddElem("PlanList");

        for (IVS_UINT32 i=0; i<pRecordPlan->uiPlanInfoNum; i++)
        {
            if (i == 0)
            {        
                (void)xml.AddChildElem("PlanInfo");   
                (void)xml.IntoElem();// ------------4    
            }
            else
            {
                (void)xml.AddElem("PlanInfo"); 
            }

            (void)xml.AddChildElem("DayType");
            (void)xml.IntoElem();//  -------------3
            (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->stPlanInfo[i].uiDayType)).c_str());
            (void)xml.AddElem("TimeInfoNum"); 
            (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->stPlanInfo[i].uiTimeSpanInfoNum)).c_str()); 
            (void)xml.AddElem("TimeList"); 

            IVS_CHAR cStartTime[IVS_TIME_LEN+1] = {0};
            IVS_CHAR cEndTime[IVS_TIME_LEN+1] = {0}; 	
            for (IVS_UINT32 j=0; j<pRecordPlan->stPlanInfo[i].uiTimeSpanInfoNum; j++)
            {
                if (j == 0)
                {
                    (void)xml.AddChildElem("TimeInfo");
                    (void)xml.IntoElem();// ---------------------- 2
                }
                else{
                    (void)xml.AddElem("TimeInfo");
                }
                if (!CToolsHelp::Memcpy(cStartTime, IVS_TIME_LEN,pRecordPlan->stPlanInfo[i].stTimeSpanInfo[j].cStart, IVS_TIME_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Or Mod ExDomain Record Plan Get XML", "Memcpy cStartTime failed");
                    return IVS_ALLOC_MEMORY_ERROR;
                }
                if (!CToolsHelp::Memcpy(cEndTime, IVS_TIME_LEN, pRecordPlan->stPlanInfo[i].stTimeSpanInfo[j].cEnd, IVS_TIME_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Or Mod ExDomain Record Plan Get XML", "Memcpy cEndTime failed");
                    return IVS_ALLOC_MEMORY_ERROR;
                }
                (void)xml.AddChildElem(strStartTime.c_str());
                (void)xml.IntoElem();//-------------------------1
                (void)xml.SetElemValue(cStartTime);

                (void)xml.AddElem(strEndTime.c_str());
                (void)xml.SetElemValue(cEndTime); 

                xml.OutOfElem();//-------------------------1
            }  
            xml.OutOfElem();// ---------------------2
            xml.OutOfElem();//----------------3
        }
        xml.OutOfElem();//----------------4
    }
    xml.OutOfElem();//---------------5
    return IVS_SUCCEED;
}

//删除录像计划的请求XML
IVS_INT32 CRecordXMLProcess::DelRecordPlanGetXML(const std::string& strSendDomainCode,DeviceCodeList& devList,
	IVS_UINT32 uiRecordMethod, CXml &xml)
{
	IVS_DEBUG_TRACE("");

	IVS_CHAR chCameraCode[IVS_DEV_CODE_LEN+1];
	IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN+1];

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");//   ---- content
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strSendDomainCode.c_str());
	(void)xml.AddElem("CameraList");

	DeviceCodeListIpIter devListIter = devList.begin();
	DeviceCodeListIpIter devListIterEnd = devList.end();
	IVS_UINT32 j=0;
	for (; devListIter != devListIterEnd; devListIter++)
	{
		const IVS_DEVICE_CODE* pdevCode = *devListIter;
		(IVS_VOID)CXmlProcess::ParseDevCode(pdevCode->cDevCode, chCameraCode, chDomainCode);		
		if (j == 0)
		{
			(void)xml.AddChildElem("CameraCode");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(chCameraCode);
		}else
		{      
			(void)xml.AddElem("CameraCode");
			(void)xml.SetElemValue(chCameraCode);
		}
		j++;
	}
	xml.OutOfElem();

	(void)xml.AddElem("Plan");
	(void)xml.AddChildElem("RecordMethod");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRecordMethod)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//删除录像计划的请求XML
IVS_INT32 CRecordXMLProcess::DelExDomainRecordPlanXML(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
    IVS_UINT32 uiRecordMethod, CXml &xml)
{
    CHECK_POINTER(pDeviceList,IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("Del ExDomain Record Plan XML.");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pDeviceList->cDevCode, chDevCode, chDomaCode);

    (void)xml.SetElemValue(chDomaCode);
    (void)xml.AddElem("CameraList");
    for (IVS_UINT32 i=0; i<uiDeviceNum; i++)
    {     
        memset(chDevCode,0,IVS_DEV_CODE_LEN+1);
        memset(chDomaCode,0,IVS_DOMAIN_CODE_LEN+1);
        (IVS_VOID)CXmlProcess::ParseDevCode(pDeviceList->cDevCode, chDevCode, chDomaCode);
        if (0 == i)
        {            
            (void)xml.AddChildElem("CameraInfo");
            (void)xml.IntoElem();
        }
        else
        {
            (void)xml.AddElem("CameraInfo");
        }

        (void)xml.AddChildElem("CameraCode");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(chDevCode);
        //(void)xml.OutOfElem();
        (void)xml.AddElem("ExDomainCode");
        //(void)xml.IntoElem();
        (void)xml.SetElemValue(chDomaCode);
        xml.OutOfElem();

        pDeviceList++;
        /*if (i == 0)
        {
            (void)xml.AddChildElem("CameraCode");
            (void)xml.IntoElem();
            (void)xml.SetElemValue(chDevCode);
        }else
        {      
            (void)xml.AddElem("CameraCode");
            (void)xml.SetElemValue(chDevCode);
        }*/
    }
    xml.OutOfElem();

    (void)xml.AddElem("Plan");
    (void)xml.AddChildElem("RecordMethod");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRecordMethod)).c_str());
    xml.OutOfElem();

    return IVS_SUCCEED;
}

//查询录像计划的请求XML
IVS_INT32 CRecordXMLProcess::GetRecordPlanGetXML(const char* pCameraCode,IVS_UINT32 uiRecordMethod, CXml &xml)
{
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("RecordMethod");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRecordMethod)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//添加/修改/删除录像计划的响应XML
IVS_INT32 CRecordXMLProcess::RecordPlanParseRSP(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize, CXml& xml,IVS_UINT32& uiRecordNum)
{
	CHECK_POINTER(pResultList,IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceList,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML","no 'Content' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	if (!xml.FindElem("ResultList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "no 'ResultList' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	
	if (xml.FindElemEx("Content/ResultList/ResultInfo"))
	{
		do 
		{
			xml.IntoElem();

			if (!xml.FindElem("CameraCode"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "no 'CameraCode' elemenet");
				return IVS_XML_INVALID;
			}
			const char* cpszCameraCode = xml.GetElemValue();
			if (NULL == cpszCameraCode)
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "cpszCameraCode NULL");
				return IVS_XML_INVALID;  
			}

			IVS_CHAR* CameraCode = CRecordXMLProcess::GetCameraDomainCodeByCameraCode(uiDeviceNum,pDeviceList,cpszCameraCode);
			bool bRet = CToolsHelp::Strncpy(pResultList->stInfo[uiRecordNum].cCode,IVS_DEV_CODE_LEN,CameraCode,strlen(CameraCode));
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Strncpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!xml.FindElem("ResultCode"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "no 'ResultCode' elemenet");
				return IVS_XML_INVALID;
			}
			const char* cpszResultCode = xml.GetElemValue();
			if (NULL == cpszResultCode)
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "cpszResultCode NULL");
				return IVS_XML_INVALID;  
			}
			pResultList->stInfo[uiRecordNum].iResult = atoi(cpszResultCode);

			xml.OutOfElem();
			uiRecordNum ++;
		} while (xml.NextElem());            

	}
	pResultList->uiNum = uiRecordNum;
	IVS_INT32 iRet = IVS_SUCCEED;
	if (pResultList->uiNum * sizeof(IVS_RESULT_INFO) + sizeof(IVS_UINT32) > uiResultBufferSize)
	{
		iRet = IVS_OPERATE_MEMORY_ERROR;
		// 返回的结果大于输入的cameraid的个数,异常
		BP_RUN_LOG_ERR(iRet,"Parse Result XML","return cameracode Result length > iResultListLen");
		return iRet;
	}
	return IVS_SUCCEED;
}
//获得摄像机编号中的域编号
IVS_CHAR* CRecordXMLProcess::GetCameraDomainCodeByCameraCode(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,const IVS_CHAR CameraCode[64])
{
	CHECK_POINTER(pDeviceList,NULL);

	for (IVS_UINT32 i=0; i<uiDeviceNum; i++)
	{
		IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
		IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
		(IVS_VOID)CXmlProcess::ParseDevCode(pDeviceList[i].cDevCode, chDevCode, chDomaCode);
		std::string devCodeStr(chDevCode);
		std::string cameraCodeStr(CameraCode);
		if(cameraCodeStr == devCodeStr)
		{
			return (char*)pDeviceList[i].cDevCode;
		}
	}
	return (IVS_CHAR*)CameraCode;
}
//获取录像策略的响应XML
 IVS_INT32 CRecordXMLProcess::GetRecordPolicyParseRSP(IVS_RECORD_POLICY_TIME* pRecordPolicy, CXml& xml)
 {
	 CHECK_POINTER(pRecordPolicy, IVS_OPERATE_MEMORY_ERROR); 
	 IVS_DEBUG_TRACE("");
	 int iRet = IVS_FAIL;
	 if (!xml.FindElemEx("Content"))
	 {
		 iRet = IVS_XML_INVALID;
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content/PageInfo) is fail");
		 return iRet;
	 }
	 xml.IntoElem();

	 const char* pTemp = NULL;

	 GET_ELEM_VALUE_NUM_FOR_UINT("RecordMode", pTemp, pRecordPolicy->uiRecordMode, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("Time", pTemp, pRecordPolicy->uiTime, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("FrameExtractRecordTime",pTemp, pRecordPolicy->uiFrameExtractRecordTime, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("AlarmTime", pTemp, pRecordPolicy->uiAlarmTime, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("KeyframeTime", pTemp, pRecordPolicy->uiKeyframeTime, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("BeforeAlarm",pTemp, pRecordPolicy->uiBeforeAlarm, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("AfterAlarm", pTemp, pRecordPolicy->uiAfterAlarm, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("PlanStreamID", pTemp, pRecordPolicy->uiPlanStreamType, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("AlarmStreamID",pTemp, pRecordPolicy->uiAlarmStreamType, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("AlarmRecordTTL", pTemp, pRecordPolicy->uiAlarmRecordTTL, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("ManualRecordTTL", pTemp, pRecordPolicy->uiManualRecordTTL, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("PreRecord",pTemp, pRecordPolicy->uiPreRecord, xml);
	 GET_ELEM_VALUE_NUM_FOR_UINT("PreRecordTime", pTemp, pRecordPolicy->uiPreRecordTime, xml);
	 GET_ELEM_VALUE_NUM("AssociatedAudio", pTemp, pRecordPolicy->bAssociatedAudio, xml);
	 
	 return IVS_SUCCEED;
 }

//录像检索的请求XML
IVS_INT32 CRecordXMLProcess::GetRecordListReqXML(const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode,IVS_INT32 iRecordMethod,const IVS_TIME_SPAN* pTimeSpan,
	const IVS_INDEX_RANGE* pIndexRange,CXml& xml)
{
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTimeSpan,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pIndexRange,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("PageInfo");	
	(void)xml.AddChildElem("FromIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiFromIndex)).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiToIndex)).c_str());

	(void)xml.AddElem("QueryCond");	
	(void)xml.AddChildElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("RecordMethod");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str());
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();

	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("FromTime");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue(pTimeSpan->cStart);
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();
	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("ToTime");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue(pTimeSpan->cEnd);
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();
	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("CameraCode");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue(pCameraCode);
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();
	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("RecordType");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue("1111");
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();

	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("QueryType");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();
	xml.OutOfElem();

	(void)xml.AddElem("QueryCond");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("OrderCond");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("OrderField");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("FromTime");
	(void)xml.AddElem("Order");
	(void)xml.SetElemValue("DOWN");
	xml.OutOfElem();
	xml.OutOfElem();

	xml.OutOfElem();

	return IVS_SUCCEED;
}
//查询录像状态的请求XML
IVS_INT32 CRecordXMLProcess::GetRecordStatusReqXML(const IVS_CHAR* pCameraCode,IVS_UINT32 iRecordMethod,CXml& xml)
{
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
	memset(chDevCode,0,IVS_DEV_CODE_LEN+1);
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	memset(chDomaCode,0,IVS_DOMAIN_CODE_LEN+1);
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iRecordMethod)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}
//查询录像状态的响应解析XML
IVS_INT32 CRecordXMLProcess::ParseRecordStatus(CXml& xml,IVS_UINT32* ulStatus)
{
	CHECK_POINTER(ulStatus,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "ErrorCode =%d",IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}

	const char* pTemp = NULL;
	const IVS_UINT32 iRecordStatusLength = 4;

	IVS_CHAR cRecordStatus[iRecordStatusLength+1];
	memset(cRecordStatus,0,iRecordStatusLength+1);

	GET_ELEM_VALUE_CHAR("RecordState",pTemp,cRecordStatus,iRecordStatusLength,xml);
	
	IVS_UINT32 ulRecordStatus = CXmlProcess::TransBinStrToDec(cRecordStatus);
	*ulStatus = ulRecordStatus;

	return IVS_SUCCEED;
}
//开始远程录像回放的请求XML
IVS_INT32 CRecordXMLProcess::StartRemoteRecordReqXML(const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 iRecordType,IVS_UINT32 iRecordTime,CXml& xml)
{
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("UserDomain");
	(void)xml.SetElemValue(pUserDomainCode);
	(void)xml.AddElem("UserID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserId)).c_str());
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iRecordType)).c_str());
	(void)xml.AddElem("RecordType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(1).c_str());
	(void)xml.AddElem("RecordTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iRecordTime)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}
//停止远程录像回放的请求XML
IVS_INT32 CRecordXMLProcess::StopRemoteRecordReqXML(const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 iRecordType,CXml& xml)
{ 
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	// 摄像机编号去掉域编号处理
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iRecordType)).c_str());
	(void)xml.AddElem("UserDomain");
	(void)xml.SetElemValue(pUserDomainCode);
	(void)xml.AddElem("UserID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserId)).c_str());
	(void)xml.AddElem("RecordType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(1).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}
//添加录像书签的请求XML
IVS_INT32 CRecordXMLProcess::AddBookmarkGetXML(const IVS_INT32 iRecordMethod,const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* cUserName,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
	const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime, CXml &xml)
{
	CHECK_POINTER(cUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNVRCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBookmarkName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBookmarkTime, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomainCode);

	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		(void)xml.SetElemValue(pDomainCode);
	}
	else
	{
		(void)xml.SetElemValue(chDomainCode);
	}
	(void)xml.AddElem("NVRCode");
	(void)xml.SetElemValue(pNVRCode); 
	(void)xml.AddElem("RecordBookmark");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode); 
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 
	(void)xml.AddElem("BookmarkName");
	(void)xml.SetElemValue(pBookmarkName); 
	(void)xml.AddElem("BookmarkTime");
	(void)xml.SetElemValue(pBookmarkTime); 
	(void)xml.AddElem("BookmarkCreatorDomain");
	(void)xml.SetElemValue(pUserDomainCode); 
	(void)xml.AddElem("BookmarkCreatorID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserId)).c_str()); 
	(void)xml.AddElem("BookmarkCreatorName");
	(void)xml.SetElemValue(cUserName); 

	xml.OutOfElem();
	return IVS_SUCCEED;
}
//修改录像书签的请求XML
IVS_INT32 CRecordXMLProcess::ModifyBookmarkGetXML(const IVS_INT32 iRecordMethod,const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
	const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName, CXml &xml)
{
	CHECK_POINTER(cUserName, IVS_OPERATE_MEMORY_ERROR); 
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNVRCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewBookmarkName, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strRecordMethod = "RecordMethod";
	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode); 
	(void)xml.AddElem("NVRCode");
	(void)xml.SetElemValue(pNVRCode); 
	(void)xml.AddElem("DomainCode");
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		(void)xml.SetElemValue(pDomainCode);
	}
	else
	{
		(void)xml.SetElemValue(chDomaCode);
	}
	(void)xml.AddElem(strRecordMethod.c_str());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 
	(void)xml.AddElem("RecordBookmark");
	(void)xml.AddChildElem("BookmarkID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiBookmarkID)).c_str()); 
	(void)xml.AddElem("BookmarkName");
	(void)xml.SetElemValue(pNewBookmarkName); 
	(void)xml.AddElem("BookmarkCreatorDomain");
	(void)xml.SetElemValue(pUserDomainCode); 
	(void)xml.AddElem("BookmarkCreatorID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserId)).c_str()); 
	(void)xml.AddElem("BookmarkCreatorName");
	(void)xml.SetElemValue(cUserName); 

	xml.OutOfElem();
	return IVS_SUCCEED;
}
//删除录像书签的请求XML
IVS_INT32 CRecordXMLProcess::DeleteBookmarkGetXML(const IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNVRCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	(void)xml.AddChildElem("NVRCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pNVRCode); 
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode); 

	(void)xml.AddElem("DomainCode");
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		(void)xml.SetElemValue(pDomainCode);
	}
	else
	{
		(void)xml.SetElemValue(chDomaCode);
	}

	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 
	(void)xml.AddElem("RecordBookmark");
	(void)xml.AddChildElem("BookmarkID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiBookmarkID)).c_str()); 

	xml.OutOfElem();
	return IVS_SUCCEED;
}
//解析查询平台书签的响应XML
IVS_INT32 CRecordXMLProcess::ParseGetBookmarkList(CXml& xml,IVS_RECORD_BOOKMARK_INFO_LIST* pRecordBookmarkInfoList,const std::string& /*strDomainCode*/,IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pRecordBookmarkInfoList, IVS_OPERATE_MEMORY_ERROR); 
	IVS_DEBUG_TRACE("");

	int iRet = IVS_FAIL;
	if (!xml.FindElemEx("Content"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "xml.FindElem(Content) is fail");
		return iRet;
	}

	if (!xml.FindElemEx("Content/PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Request XML", "xml.FindElem(Content/PageInfo) is fail");
		return IVS_XML_INVALID;
	}   

	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM("FromIndex", pTemp, pRecordBookmarkInfoList->stIndexRange.uiFromIndex, xml);//lint !e732
	GET_ELEM_VALUE_NUM("ToIndex", pTemp, pRecordBookmarkInfoList->stIndexRange.uiToIndex, xml);//lint !e732
	GET_ELEM_VALUE_NUM("RealNum",pTemp, pRecordBookmarkInfoList->uiTotal, xml);//lint !e734 !e732

	IVS_INT32 iRealNum = 0;
	const char* szElemValue = NULL;

	if (0 == pRecordBookmarkInfoList->uiTotal)
	{
		return IVS_SUCCEED;
	}

	if (xml.FindElemEx("Content/RecordBookmarkList/RecordBookmark"))
	{
		do
		{
			IVS_RECORD_BOOKMARK_INFO stRecordBookmarkInfo;
			memset(&stRecordBookmarkInfo, 0, sizeof(IVS_RECORD_BOOKMARK_INFO));

			(void)xml.IntoElem();
			GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkID",szElemValue,  stRecordBookmarkInfo.uiBookmarkID,xml);
			GET_ELEM_VALUE_CHAR("NVRCode",szElemValue,  stRecordBookmarkInfo.cNvrCode,IVS_NVR_CODE_LEN,xml);
			GET_ELEM_VALUE_CHAR("MBUDomain",szElemValue,  stRecordBookmarkInfo.cMBUDomain,IVS_DOMAIN_CODE_LEN,xml);
			GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,  stRecordBookmarkInfo.cCameraCode,IVS_DEV_CODE_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkName",szElemValue,  stRecordBookmarkInfo.cBookmarkName,IVS_BOOKMARK_NAME_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkTime",szElemValue,  stRecordBookmarkInfo.cBookmarkTime,IVS_TIME_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkCreatorName",szElemValue,  stRecordBookmarkInfo.cBookmarkCreatorName,IVS_NAME_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkCreatorDomain",szElemValue,  stRecordBookmarkInfo.cUserDomain,IVS_DOMAIN_CODE_LEN,xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkCreatorID",szElemValue,stRecordBookmarkInfo.uiBookmarkCreatorID,xml);
			xml.OutOfElem();

			bool bRet = CToolsHelp::Memcpy(&(pRecordBookmarkInfoList->stBookmarkInfo[iRealNum]), sizeof(IVS_RECORD_BOOKMARK_INFO), &stRecordBookmarkInfo, sizeof(IVS_RECORD_BOOKMARK_INFO));
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			iRealNum++;
		}while(xml.NextElem());
	}
	//添加尺寸校验

    IVS_UINT32 unItemSize =(IVS_UINT32)((iRealNum-1 > 0) ? (iRealNum-1) : 0);

	IVS_UINT32 uiSize = sizeof(IVS_RECORD_BOOKMARK_INFO_LIST) + unItemSize * sizeof(IVS_RECORD_BOOKMARK_INFO);
	if (uiSize>uiBufferSize)
	{
		return IVS_OPERATE_MEMORY_ERROR;
	}

	return IVS_SUCCEED;
}

//解析查询备份服务器书签的响应XML
IVS_INT32 CRecordXMLProcess::ParseGetBookmarkListForBackup(CXml& xml,IVS_RECORD_BOOKMARK_INFO_LIST* pRecordBookmarkInfoList,const std::string& /*strDomainCode*/,IVS_UINT32 uiBufferSize,
	 IVS_UINT32& uiRecordNum,IVS_UINT32 uiFromIndex,IVS_UINT32 uiToIndex)
{
	CHECK_POINTER(pRecordBookmarkInfoList, IVS_OPERATE_MEMORY_ERROR); 
	IVS_DEBUG_TRACE("");

	int iRet = IVS_FAIL;
	if (!xml.FindElemEx("Content"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Get Bookmark List For Backup:Get Request XML", "xml.FindElem(Content) is fail");
		return iRet;
	}

	if (!xml.FindElemEx("Content/PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Get Bookmark List For Backup:Get Request XML", "xml.FindElem(Content/PageInfo) is fail");
		return IVS_XML_INVALID;
	}   

	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM("RealNum",pTemp, pRecordBookmarkInfoList->uiTotal, xml);//lint !e734 !e732
	GET_ELEM_VALUE_NUM("FromIndex", pTemp, pRecordBookmarkInfoList->stIndexRange.uiFromIndex, xml);//lint !e732
	GET_ELEM_VALUE_NUM("ToIndex", pTemp, pRecordBookmarkInfoList->stIndexRange.uiToIndex, xml);//lint !e732


	IVS_UINT32 uiCurrentNum = 0;
	const char* szElemValue = NULL;

	if (0 == pRecordBookmarkInfoList->uiTotal)
	{
		return IVS_SUCCEED;
	}

	if (xml.FindElemEx("Content/RecordBookmarkList/RecordBookmark"))
	{
		do
		{
			IVS_RECORD_BOOKMARK_INFO stRecordBookmarkInfo;
			memset(&stRecordBookmarkInfo, 0, sizeof(IVS_RECORD_BOOKMARK_INFO));
			(void)xml.IntoElem();
			GET_ELEM_VALUE_CHAR("NVRCode",szElemValue,  stRecordBookmarkInfo.cNvrCode,IVS_NVR_CODE_LEN,xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkID",szElemValue,  stRecordBookmarkInfo.uiBookmarkID,xml);
			GET_ELEM_VALUE_CHAR("MBUDomain",szElemValue,  stRecordBookmarkInfo.cMBUDomain,IVS_DOMAIN_CODE_LEN,xml);
			GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,  stRecordBookmarkInfo.cCameraCode,IVS_DEV_CODE_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkTime",szElemValue,  stRecordBookmarkInfo.cBookmarkTime,IVS_TIME_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkName",szElemValue,  stRecordBookmarkInfo.cBookmarkName,IVS_BOOKMARK_NAME_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkCreatorName",szElemValue,  stRecordBookmarkInfo.cBookmarkCreatorName,IVS_NAME_LEN,xml);
			GET_ELEM_VALUE_CHAR("BookmarkCreatorDomain",szElemValue, stRecordBookmarkInfo.cUserDomain,IVS_DOMAIN_CODE_LEN,xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkCreatorID",szElemValue,stRecordBookmarkInfo.uiBookmarkCreatorID,xml);
			xml.OutOfElem();

			//添加尺寸校验
			IVS_UINT32 uiSize = sizeof(IVS_RECORD_BOOKMARK_INFO_LIST) + static_cast<IVS_UINT32>(uiRecordNum-1) * sizeof(IVS_RECORD_BOOKMARK_INFO);
			if (uiSize>uiBufferSize)
			{
				break;
			}

			uiCurrentNum++;
			if((uiFromIndex == 0 && uiToIndex == 0)|| (uiCurrentNum>=uiFromIndex && uiCurrentNum<=uiToIndex))
			{
				bool bRet = CToolsHelp::Memcpy(&(pRecordBookmarkInfoList->stBookmarkInfo[uiRecordNum]), sizeof(IVS_RECORD_BOOKMARK_INFO), &stRecordBookmarkInfo, sizeof(IVS_RECORD_BOOKMARK_INFO));
				if(false == bRet)
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Get Bookmark List For Backup:Parse Result XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				uiRecordNum++;
			}	
		}while(xml.NextElem());
	}

	return IVS_SUCCEED;
}


//手动录像状态的报文解析XML
IVS_INT32 CRecordXMLProcess::ParseManualRecordStateXML(CXml& xml,IVS_MANUAL_RECORD_STATUS_NOTIFY* pManualRecordStatusNotify)
{
	CHECK_POINTER(pManualRecordStatusNotify,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "ErrorCode =%d",IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const char* szElemValue = NULL;
	char cCameraCode[IVS_DEV_CODE_LEN+1]={0};
	char cDomainCode[IVS_DEV_CODE_LEN+1]={0};
	GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,cCameraCode,IVS_DEV_CODE_LEN,xml);
	GET_ELEM_VALUE_CHAR("DomainCode",szElemValue,cDomainCode,IVS_DEV_CODE_LEN,xml);
	std::string strBigCode;
	strBigCode.append(cCameraCode).append("#").append(cDomainCode);
	//pManualRecordStatusNotify->cCameraCode = strBigCode.c_str();
	bool bRet = CToolsHelp::Memcpy(pManualRecordStatusNotify->cCameraCode,IVS_DEV_CODE_LEN, strBigCode.c_str(), strlen(strBigCode.c_str()));
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}


	GET_ELEM_VALUE_NUM_FOR_UINT("UserID",szElemValue,  pManualRecordStatusNotify->uiUserID,xml);
	//pManualRecordStatusNotify->uiUserID = ntohl(pManualRecordStatusNotify->uiUserID);
	GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,  pManualRecordStatusNotify->uiRecordMethod,xml);
	//pManualRecordStatusNotify->uiRecordMethod = ntohl(pManualRecordStatusNotify->uiRecordMethod);
	GET_ELEM_VALUE_NUM_FOR_UINT("ManualRecordState",szElemValue,  pManualRecordStatusNotify->uiManualRecordState,xml);
	//pManualRecordStatusNotify->uiManualRecordState = ntohl(pManualRecordStatusNotify->uiManualRecordState);
	GET_ELEM_VALUE_NUM_FOR_UINT("NotifInfo",szElemValue,  pManualRecordStatusNotify->uiNotifInfo,xml);
	//pManualRecordStatusNotify->uiNotifInfo = ntohl(pManualRecordStatusNotify->uiNotifInfo);
	return IVS_SUCCEED;
}

//锁定录像的请求XML
IVS_INT32 CRecordXMLProcess::LockRecordReqXML(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo, CXml &xml)
{
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pRecordLockInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	std::string strDomainCode = "DomainCode";
	std::string strNVRCode = "NVRCode";
	std::string strRecordMethod = "RecordMethod";

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem(strDomainCode.c_str());
	(void)xml.IntoElem();
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		(void)xml.SetElemValue(pDomainCode);
		(void)xml.AddElem(strNVRCode.c_str());
		(void)xml.SetElemValue(pNVRCode);
		(void)xml.AddElem("CameraDomain");
		(void)xml.SetElemValue(chDomaCode);
	}
	else
	{
		(void)xml.SetElemValue(chDomaCode);
	}
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem(strRecordMethod.c_str());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 

	IVS_CHAR cLockTime[IVS_TIME_LEN+1] = {0};
	IVS_CHAR cStartLockTime[IVS_TIME_LEN+1] = {0}; 
	IVS_CHAR cEndLockTime[IVS_TIME_LEN+1] = {0}; 
	IVS_CHAR cLockDesc[IVS_RECORD_LOCK_DESC_LEN+1] = {0};
	IVS_CHAR cOperatorName[IVS_NAME_LEN+1] = {0};
	if (!CToolsHelp::Memcpy(cLockTime, IVS_TIME_LEN,pRecordLockInfo->cLockTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Lock Record Req XML", "Memcpy cLockTime failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if (!CToolsHelp::Memcpy(cStartLockTime, IVS_TIME_LEN, pRecordLockInfo->stLockTimeSpan.cStart, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Lock Record Req XML", "Memcpy cStartLockTime failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if (!CToolsHelp::Memcpy(cEndLockTime, IVS_TIME_LEN,pRecordLockInfo->stLockTimeSpan.cEnd, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Lock Record Req XML", "Memcpy cEndLockTime failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if (!CToolsHelp::Memcpy(cLockDesc, IVS_RECORD_LOCK_DESC_LEN, pRecordLockInfo->cLockDesc, IVS_RECORD_LOCK_DESC_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Lock Record Req XML", "Memcpy cLockDesc failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if (!CToolsHelp::Memcpy(cOperatorName, IVS_NAME_LEN,pRecordLockInfo->cOperatorName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Lock Record Req XML", "Memcpy cOperatorName failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	(void)xml.AddElem("LockInfo");
	(void)xml.AddChildElem("LockTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(cLockTime); 
	(void)xml.AddElem("LockStartTime");
	(void)xml.SetElemValue(cStartLockTime); 
	(void)xml.AddElem("LockEndTime");
	(void)xml.SetElemValue(cEndLockTime); 
	(void)xml.AddElem("LockDesc");
	(void)xml.SetElemValue(cLockDesc); 
	(void)xml.AddElem("OperatorID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordLockInfo->uiOperatorID)).c_str()); 
	(void)xml.AddElem("OperatorName");
	(void)xml.SetElemValue(cOperatorName); 
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//修改录像锁定信息的请求XML
IVS_INT32 CRecordXMLProcess::ModLockRecordReqXML(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo, CXml &xml)
{
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pRecordLockInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1]= {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		(void)xml.SetElemValue(pDomainCode);
		(void)xml.AddElem("NVRCode");
		(void)xml.SetElemValue(pNVRCode);
		(void)xml.AddElem("CameraDomain");
		(void)xml.SetElemValue(chDomaCode);
	}
	else
	{
		(void)xml.SetElemValue(chDomaCode);
	}
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 

	
	IVS_CHAR cOperatorName[IVS_NAME_LEN+1] = {0};
	if (!CToolsHelp::Memcpy(cOperatorName, IVS_NAME_LEN,pRecordLockInfo->cOperatorName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Mod Lock Record Req XML", "Memcpy cOperatorName failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_CHAR cLockDesc[IVS_RECORD_LOCK_DESC_LEN+1] = {0};
	if (!CToolsHelp::Memcpy(cLockDesc, IVS_RECORD_LOCK_DESC_LEN, pRecordLockInfo->cLockDesc, IVS_RECORD_LOCK_DESC_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Mod Lock Record Req XML", "Memcpy cLockDesc failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	(void)xml.AddElem("LockInfo");
	(void)xml.AddChildElem("LockID");
	(void)xml.IntoElem(); 
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordLockInfo->uiLockID)).c_str()); 
	(void)xml.AddElem("LockDesc");
	(void)xml.SetElemValue(cLockDesc); 
	(void)xml.AddElem("OperatorID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordLockInfo->uiOperatorID)).c_str()); 
	(void)xml.AddElem("OperatorName");
	(void)xml.SetElemValue(cOperatorName); 
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//解锁录像的请求XML
IVS_INT32 CRecordXMLProcess::UnLockRecordReqXML(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo, CXml &xml)
{
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pRecordLockInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	std::string strNVRCode = "NVRCode";
	std::string strCameraDomain = "CameraDomain";

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		(void)xml.SetElemValue(pDomainCode);
		(void)xml.AddElem(strNVRCode.c_str());
		(void)xml.SetElemValue(pNVRCode);
		(void)xml.AddElem(strCameraDomain.c_str());
		(void)xml.SetElemValue(chDomaCode);
	}
	else
	{
		(void)xml.SetElemValue(chDomaCode);
	}
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 

	IVS_CHAR cUnlockStartTime[IVS_TIME_LEN+1] = {0}; 
	if (!CToolsHelp::Memcpy(cUnlockStartTime, IVS_TIME_LEN, pRecordLockInfo->stLockTimeSpan.cStart, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "UnLock Record Req XML", "Memcpy cUnlockStartTime failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_CHAR cUnlockEndTime[IVS_TIME_LEN+1] = {0}; 
	if (!CToolsHelp::Memcpy(cUnlockEndTime, IVS_TIME_LEN,pRecordLockInfo->stLockTimeSpan.cEnd, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "UnLock Record Req XML", "Memcpy cUnlockEndTime failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_CHAR cOperatorName[IVS_NAME_LEN+1] = {0};
	if (!CToolsHelp::Memcpy(cOperatorName, IVS_NAME_LEN,pRecordLockInfo->cOperatorName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "UnLock Record Req XML", "Memcpy cOperatorName failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	(void)xml.AddElem("UnlockInfo");
	(void)xml.AddChildElem("LockID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordLockInfo->uiLockID)).c_str()); 
	(void)xml.AddElem("UnlockStartTime");
	(void)xml.SetElemValue(cUnlockStartTime); 
	(void)xml.AddElem("UnlockEndTime");
	(void)xml.SetElemValue(cUnlockEndTime); 
	(void)xml.AddElem("OperatorID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordLockInfo->uiOperatorID)).c_str()); 
	(void)xml.AddElem("OperatorName");
	(void)xml.SetElemValue(cOperatorName); 
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

IVS_VOID CRecordXMLProcess::GetCameraPlanInfoGetXML(IVS_UINT32 uiPlanType, const IVS_CHAR* pDomainCode, CXml &xml)
{
    IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);
    (void)xml.AddElem("PlanType");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiPlanType))).c_str());
    xml.OutOfElem();
}

IVS_INT32 CRecordXMLProcess::GetCameraPlanInfoParseXML(CXml &xml, PLAN_INFO_VEC &cameraPlanInfoList)
{
    IVS_DEBUG_TRACE("");

    const IVS_CHAR* szElemValue = NULL;

    if (!xml.FindElemEx("Content/CameraList"))
    {
        return IVS_XML_INVALID;
    }

    if (xml.FindElemEx("Content/CameraList/CameraInfo"))
    {
        do
        {
            (void)xml.IntoElem();
            IVS_CAMERA_PLAN_SET_INFO stcCameraPlan;
            memset(&stcCameraPlan, 0, sizeof(IVS_CAMERA_PLAN_SET_INFO));

            GET_ELEM_VALUE_CHAR("CameraCode", szElemValue, stcCameraPlan.cCameraCode, IVS_DEV_CODE_LEN, xml);  
            GET_ELEM_VALUE_NUM("SetPlan", szElemValue, stcCameraPlan.bSetPlan, xml);
            stcCameraPlan.iResult = IVS_SUCCEED;
            xml.OutOfElem();
            cameraPlanInfoList.push_back(stcCameraPlan);
        }while(xml.NextElem());
    }

    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_VOID CRecordXMLProcess::GetCameraPlanInfoGetXml(const std::map<std::string, PLAN_INFO_VEC> &planMap, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");        
    (void)xml.AddChildElem("CameraList");
    (void)xml.IntoElem();

    const IVS_CAMERA_PLAN_SET_INFO* pPlanInfo;
    std::string strDomainCode;
    std::map<std::string, PLAN_INFO_VEC>::const_iterator planMapIter;
    const PLAN_INFO_VEC * vec_planList;
    IVS_UINT32 iSize = 0;
    for (planMapIter = planMap.begin(); planMapIter != planMap.end(); planMapIter++)
    {
        strDomainCode = planMapIter->first;
        vec_planList = &planMapIter->second;

        IVS_UINT32 iNum = vec_planList->size();
        for (IVS_UINT32 i = 0; i < iNum; i++)
        {
            pPlanInfo = &vec_planList->at(i);
            if(iSize > 0)
            {
                (void)xml.AddElem("CameraInfo");
            }
            else
            {
                (void)xml.AddChildElem("CameraInfo");
            }

            (void)xml.IntoElem();
            (void)xml.AddChildElem("CameraCode");
            (void)xml.IntoElem();
            IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
            if (!CToolsHelp::Memcpy(cCameraCode, IVS_DEV_CODE_LEN, pPlanInfo->cCameraCode, IVS_DEV_CODE_LEN))
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pPlanInfo->cCameraCode", "Memcpy failed");
            }
            std::string strBigCode;
            strBigCode.append(cCameraCode).append("#").append(strDomainCode);
            (void)xml.SetElemValue(strBigCode.c_str());

            (void)xml.AddElem("SetPlan");
            (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pPlanInfo->bSetPlan)).c_str());
            xml.OutOfElem();
            iSize++;
        }
    }

    xml.OutOfElem();
}
