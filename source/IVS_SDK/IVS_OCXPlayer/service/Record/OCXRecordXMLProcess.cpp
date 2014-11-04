#include "stdafx.h"
#include "OCXRecordXMLProcess.h"
#include "ToolsHelp.h"
#include "OCXXmlProcess.h"
#include "SDKDef.h"
#include "IVS_Trace.h"
COCXRecordXMLProcess::COCXRecordXMLProcess()
{
}

COCXRecordXMLProcess::~COCXRecordXMLProcess()
{
}

//设置录像策略响应XML
IVS_INT32 COCXRecordXMLProcess::RecordPolicyPraseXML(IVS_RECORD_POLICY_TIME* pRecordPolicy, IVS_CHAR *cCameraCode,CXml& xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pRecordPolicy, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(cCameraCode, IVS_OPERATE_MEMORY_ERROR);
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Record Policy Prase XML fail", "no 'Content' elemenet");
		return IVS_XML_INVALID;
	}

	if (!xml.FindElemEx("Content/CameraCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Record Policy Prase XML fail", "no 'CameraCode' elemenet");
		return IVS_XML_INVALID;
	}
	const char* szElemValue = xml.GetElemValue(); 
	CHECK_POINTER(szElemValue, IVS_XML_INVALID);
	bool bRet = CToolsHelp::Strncpy(cCameraCode, IVS_DEV_CODE_LEN, szElemValue, strlen(szElemValue));  
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Record Policy Prase XML", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

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
	GET_ELEM_VALUE_NUM_FOR_UINT("AlarmRecordTTL", pTemp, pRecordPolicy->uiAlarmRecordTTL, xml)
	GET_ELEM_VALUE_NUM_FOR_UINT("ManualRecordTTL", pTemp, pRecordPolicy->uiManualRecordTTL, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("PreRecord",pTemp, pRecordPolicy->uiPreRecord, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("PreRecordTime", pTemp, pRecordPolicy->uiPreRecordTime, xml);
	GET_ELEM_VALUE_NUM("AssociatedAudio", pTemp, pRecordPolicy->bAssociatedAudio, xml);

	return IVS_SUCCEED;

}
//录像策略解析响应消息XML
IVS_INT32 COCXRecordXMLProcess::GetRecordPolicyGetXML(const IVS_RECORD_POLICY_TIME* pRecordPolicy ,LPCTSTR pCameraCode,CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pRecordPolicy, IVS_PARA_INVALID);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pCameraCode);

	(void)xml.AddElem("RecordMode");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPolicy->uiRecordMode)).c_str()); 

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
//查询录像列表请求xml
IVS_INT32 COCXRecordXMLProcess::GetRecordListGetXML(const IVS_CHAR* pCameraCode,IVS_RECORD_INFO_LIST *pRecordInfoList,IVS_INT32 iRetCord,CXml &xml)
{
	CHECK_POINTER(pRecordInfoList,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCord).c_str());

	(void)xml.AddElem("PageInfo");
	(void)xml.AddChildElem("RealNum");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordInfoList->uiTotal)).c_str());
	(void)xml.AddElem("FromIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordInfoList->stIndexRange.uiFromIndex)).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordInfoList->stIndexRange.uiToIndex)).c_str());
	xml.OutOfElem();

	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pCameraCode);

	if(pRecordInfoList->uiTotal>0)
	{
		(void)xml.AddElem("RecordDataList");
		IVS_INT32 iToIndex = static_cast<IVS_INT32>(pRecordInfoList->stIndexRange.uiToIndex);
		IVS_INT32 iFromIndex = static_cast<IVS_INT32>(pRecordInfoList->stIndexRange.uiFromIndex);
		int iRealNum = static_cast<IVS_INT32>(pRecordInfoList->uiTotal);
		int iRequestTotal = (iToIndex - iFromIndex) + 1;
		if (iRequestTotal < iRealNum && iToIndex < iRealNum )
		{
			iRealNum = iRequestTotal;
		}
		else
		{
			iRealNum = (iRealNum - iFromIndex) +1;
		}
		for (int iIndex = 0;iIndex<iRealNum;iIndex++)
		{
			IVS_RECORD_INFO &stRecordInfo = pRecordInfoList->stRecordInfo[iIndex];
			iIndex == 0 ? (void)xml.AddChildElem("RecordDataInfo"):(void)xml.AddElem("RecordDataInfo");
			(void)(xml.IntoElem());

			(void)xml.AddChildElem("FileName");
			(void)(xml.IntoElem());

			IVS_CHAR cRecordFileName[IVS_FILE_NAME_LEN+1];
			memset(cRecordFileName,0,IVS_FILE_NAME_LEN+1);
			bool bRet = CToolsHelp::Memcpy(cRecordFileName,IVS_FILE_NAME_LEN,stRecordInfo.cRecordFileName,IVS_FILE_NAME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cRecordFileName);

			(void)xml.AddElem("RecordMethod");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRecordInfo.uiRecordMethod)).c_str());
			//这里需要转化下

			std::string str = COCXXmlProcess::TransDecToBinStr(stRecordInfo.uiRecordType);
			int iStatus = COCXXmlProcess::TransStrToInt(str);
			CString strTempRecordType;
			strTempRecordType.Format("%03d",iStatus);

			(void)xml.AddElem("NVRCode");
			IVS_CHAR cNvrCode[IVS_NVR_CODE_LEN+1];
			memset(cNvrCode,0,IVS_NVR_CODE_LEN+1);
			bRet = CToolsHelp::Memcpy(cNvrCode,IVS_NVR_CODE_LEN,stRecordInfo.cNvrCode,IVS_NVR_CODE_LEN);
			if(!bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cNvrCode);

			(void)xml.AddElem("MBUDomain");
			IVS_CHAR cMBUDomain[IVS_DOMAIN_CODE_LEN+1];
			memset(cMBUDomain,0,IVS_DOMAIN_CODE_LEN+1);
			bRet = CToolsHelp::Memcpy(cMBUDomain,IVS_DOMAIN_CODE_LEN,stRecordInfo.cMBUDomain,IVS_DOMAIN_CODE_LEN);
			if(!bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cMBUDomain);

			(void)xml.AddElem("RecordType");
			(void)xml.SetElemValue(strTempRecordType);

			IVS_CHAR cAlarmType[IVS_ALARM_TYPE_LEN+1];
			memset(cAlarmType,0,IVS_ALARM_TYPE_LEN+1);
			bRet = CToolsHelp::Memcpy(cAlarmType,IVS_ALARM_TYPE_LEN,stRecordInfo.cAlarmType,IVS_ALARM_TYPE_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.AddElem("AlarmType");
			(void)xml.SetElemValue(cAlarmType);
			(void)xml.AddElem("IsFullRecord");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRecordInfo.uiFrameExtractionTimes)).c_str());
			(void)xml.AddElem("StartTime");
			IVS_CHAR cStartTime[IVS_TIME_LEN+1];
			memset(cStartTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cStartTime,IVS_TIME_LEN,stRecordInfo.stTime.cStart,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cStartTime);

			(void)xml.AddElem("EndTime");
			IVS_CHAR cEndTime[IVS_TIME_LEN+1];
			memset(cEndTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cEndTime,IVS_TIME_LEN,stRecordInfo.stTime.cEnd,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cEndTime);

			(void)xml.AddElem("RecordBookmark");
			(void)xml.AddChildElem("BookmarkID");
			(void)(xml.IntoElem());
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRecordInfo.stBookmarkInfo.uiBookmarkID)).c_str());

			(void)xml.AddElem("BookmarkName");
			IVS_CHAR cBookInfoName[IVS_BOOKMARK_NAME_LEN+1];
			memset(cBookInfoName,0,IVS_BOOKMARK_NAME_LEN+1);
			bRet = CToolsHelp::Memcpy(cBookInfoName,IVS_BOOKMARK_NAME_LEN,stRecordInfo.stBookmarkInfo.cBookmarkName,IVS_BOOKMARK_NAME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cBookInfoName);
			(void)xml.AddElem("BookmarkTime");
			IVS_CHAR cBookInfoTime[IVS_TIME_LEN+1];
			memset(cBookInfoTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cBookInfoTime,IVS_TIME_LEN,stRecordInfo.stBookmarkInfo.cBookmarkTime,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cBookInfoTime);
			(void)xml.AddElem("BookmarkCreatorDomain");
			IVS_CHAR cUserDomain[IVS_DOMAIN_CODE_LEN+1];
			memset(cUserDomain,0,IVS_DOMAIN_CODE_LEN+1);
			bRet = CToolsHelp::Memcpy(cUserDomain,IVS_DOMAIN_CODE_LEN,stRecordInfo.stBookmarkInfo.cUserDomain,IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cUserDomain);
			(void)xml.AddElem("BookmarkCreatorID");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRecordInfo.stBookmarkInfo.uiBookmarkCreatorID)).c_str());
			(void)xml.AddElem("BookmarkCreatorName");
			IVS_CHAR cBookInfoCreatorName[IVS_NAME_LEN+1];
			memset(cBookInfoCreatorName,0,IVS_NAME_LEN+1);
			bRet = CToolsHelp::Memcpy(cBookInfoCreatorName,IVS_NAME_LEN,stRecordInfo.stBookmarkInfo.cBookmarkCreatorName,IVS_NAME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cBookInfoCreatorName);
			xml.OutOfElem();

			(void)xml.AddElem("LockInfo");
			(void)xml.AddChildElem("LockID");
			(void)(xml.IntoElem());
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRecordInfo.stLockInfo.uiLockID)).c_str());
			(void)xml.AddElem("OperatorName");
			IVS_CHAR cLockOperatorName[IVS_NAME_LEN+1];
			memset(cLockOperatorName,0,IVS_NAME_LEN+1);
			bRet = CToolsHelp::Memcpy(cLockOperatorName,IVS_NAME_LEN,stRecordInfo.stLockInfo.cOperatorName,IVS_NAME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cLockOperatorName);
			(void)xml.AddElem("LockDesc");
			IVS_CHAR cLockDesc[IVS_RECORD_LOCK_DESC_LEN+1];
			memset(cLockDesc,0,IVS_RECORD_LOCK_DESC_LEN+1);
			bRet = CToolsHelp::Memcpy(cLockDesc,IVS_RECORD_LOCK_DESC_LEN,stRecordInfo.stLockInfo.cLockDesc,IVS_RECORD_LOCK_DESC_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cLockDesc);
			(void)xml.AddElem("LockTime");
			IVS_CHAR cLockTime[IVS_TIME_LEN+1];
			memset(cLockTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cLockTime,IVS_TIME_LEN,stRecordInfo.stLockInfo.cLockTime,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cLockTime);
			(void)xml.AddElem("LockStartTime");
			IVS_CHAR cLockStartTime[IVS_TIME_LEN+1];
			memset(cLockStartTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cLockStartTime,IVS_TIME_LEN,stRecordInfo.stLockInfo.stLockTimeSpan.cStart,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cLockStartTime);
			(void)xml.AddElem("LockEndTime");
			IVS_CHAR cLockEndTime[IVS_TIME_LEN+1];
			memset(cLockEndTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cLockEndTime,IVS_TIME_LEN,stRecordInfo.stLockInfo.stLockTimeSpan.cEnd,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cLockEndTime);
			xml.OutOfElem();

			(void)xml.AddElem("PresetInfo");
			(void)xml.AddChildElem("PresetID");
			(void)(xml.IntoElem());
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRecordInfo.stPtzPresetInfo.uiID)).c_str());
			(void)xml.AddElem("PresetName");
			IVS_CHAR cPresetName[IVS_RECORD_PTZ_PRESET_NAME_LEN+1];
			memset(cPresetName,0,IVS_RECORD_PTZ_PRESET_NAME_LEN+1);
			bRet = CToolsHelp::Memcpy(cPresetName,IVS_RECORD_PTZ_PRESET_NAME_LEN,stRecordInfo.stPtzPresetInfo.cName,IVS_RECORD_PTZ_PRESET_NAME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cPresetName);
			xml.OutOfElem();
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}


	xml.OutOfElem();

	return IVS_SUCCEED;
}
//获取录像查询条件中的CameraCode
IVS_INT32 COCXRecordXMLProcess::GetQueryUnifiedCameraCode(CXml &xml,IVS_CHAR* pCameraCode)
{
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	const char* szElemValue = NULL;
	IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1];
	memset(cCameraCode,0,IVS_DEV_CODE_LEN+1);
	if (xml.FindElemEx("Content/PageInfo/QueryCond/QueryField"))
	{
		do 
		{
			(void)xml.IntoElem();

			GET_ELEM_VALUE_CHAR("Field",szElemValue,cCameraCode,IVS_DEV_CODE_LEN,xml);
			if (QUERY_CAMERA_CODE==QUERYFILED.GetTypeID(cCameraCode))
			{
				GET_ELEM_VALUE_CHAR("Value",szElemValue,pCameraCode,IVS_DEV_CODE_LEN,xml);
				iRet = IVS_SUCCEED;
				break;
			}

			xml.OutOfElem();
		} while (xml.NextElem());
	}
	
	return iRet;
}

//添加/修改录像计划:将请求XML转换为结构体
 IVS_INT32 COCXRecordXMLProcess::AddOrModRecordPlanGetXML(IVS_UINT32& uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, IVS_RECORD_PLAN *pRecordPlan, CXml &xml)
 { 
	 IVS_DEBUG_TRACE("");
	 CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	 CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);

	 if (!xml.FindElemEx("Content/CameraList"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Add Or Mod Record Plan Get XML", "fail", "no 'CameraList' elemenet");
		 return IVS_XML_INVALID;
	 }      

	 const char* szElemValue = NULL;

	 if (xml.FindElemEx("Content/CameraList/CameraCode"))
	 { 
		 do 
		 {   //解析CameraCode
			 szElemValue = xml.GetElemValue(); 
			 CHECK_POINTER(szElemValue, IVS_XML_INVALID);
			 IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1];
			 memset(cCameraCode,0,IVS_DEV_CODE_LEN+1);
			 bool bRet = CToolsHelp::Strncpy(cCameraCode, IVS_DEV_CODE_LEN, szElemValue, strlen(szElemValue));  
			 if(false == bRet)
			 {
				 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Add Or Mod Record Plan Get XML", "Strncpy error.");
				 return IVS_ALLOC_MEMORY_ERROR;
			 }
			 bRet = CToolsHelp::Memcpy((void*)pDeviceList[uiDeviceNum].cDevCode,IVS_DEV_CODE_LEN,cCameraCode,IVS_DEV_CODE_LEN);	 
			 if(false == bRet)
			 {
				 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Add Or Mod Record Plan Get XML", "Memcpy error.");
				 return IVS_ALLOC_MEMORY_ERROR;
			 }
			 uiDeviceNum++;
		 } while (xml.NextElem());
	 }

	 xml.OutOfElem();
	 if (!xml.FindElem("Plan"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Add Or Mod Record Plan Get XML", "faill", "no 'Plan' elemenet");
		 return IVS_XML_INVALID;
	 }
	 xml.IntoElem();

	 // 平台录像计划
	 if (!xml.FindElem("RecordMethod"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Add Or Mod Record Plan Get XML", "faill", "no 'RecordMethod' elemenet");
		 return IVS_XML_INVALID;
	 }
	 GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,pRecordPlan->uiRecordMethod,xml);
	 // 是否启用 
	 if (!xml.FindElem("IsEnable"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Add Or Mod Record Plan Get XML", "faill", "no 'IsEnable' elemenet");
		 return IVS_XML_INVALID;
	 }
	 const IVS_CHAR* cpszIsEnable = xml.GetElemValue();
	 CHECK_POINTER(cpszIsEnable,IVS_XML_INVALID);
	 pRecordPlan->bEnableRecordPlan = static_cast<IVS_BOOL>(atoi(cpszIsEnable));

	 // 计划方式(周计划 日计划 全程录像)
	 if (!xml.FindElem("PlanType"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Add Or Mod Record Plan Get XML", "faill", "no 'PlanType' elemenet");
		 return IVS_XML_INVALID;
	 }
	 GET_ELEM_VALUE_NUM_FOR_UINT("PlanType",szElemValue,pRecordPlan->uiRecordPlanType,xml);

	 IVS_UINT32 iPlanInfoNum = 0;
	 IVS_INT32 iTimeInfoNum = 0;
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
	 pRecordPlan->uiPlanInfoNum = iPlanInfoNum;
	 xml.OutOfElem();

	 return IVS_SUCCEED;
 }

 //删除录像计划:将请求XML转换为结构体
 IVS_INT32 COCXRecordXMLProcess::DelRecordPlanGetXML(IVS_UINT32& uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, IVS_UINT32 &uiRecordMethod, CXml &xml)
 {
	 CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	 IVS_DEBUG_TRACE("");
	 if (!xml.FindElemEx("Content"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Del Record Plan Get XML", "fail", "no 'Content' elemenet");
		 return IVS_XML_INVALID;
	 }
	 if (!xml.FindElemEx("Content/CameraList"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Del Record Plan Get XML", "fail", "no 'CameraList' elemenet");
		 return IVS_XML_INVALID;
	 }

	 const char* szElemValue = NULL;
	 if (xml.FindElemEx("Content/CameraList/CameraCode"))
	 { 
		 bool bRet = false;
		 do 
		 {//解析CameraCode
			 szElemValue = xml.GetElemValue(); 
			 IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1];
			 memset(cCameraCode,0,IVS_DEV_CODE_LEN+1);
			 if (NULL != szElemValue)                                                            
			 {        
				 bRet = CToolsHelp::Strncpy(cCameraCode, IVS_DEV_CODE_LEN, szElemValue, strlen(szElemValue)); 
				 if(false == bRet)
				 {
					 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Del Record Plan Get XML", "Strncpy error.");
					 return IVS_ALLOC_MEMORY_ERROR;
				 }
			 }
			 bRet = CToolsHelp::Memcpy((void*)pDeviceList[uiDeviceNum].cDevCode,IVS_DEV_CODE_LEN,cCameraCode,IVS_DEV_CODE_LEN);	 
			 if(false == bRet)
			 {
				 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Del Record Plan Get XML", "Memcpy error.");
				 return IVS_ALLOC_MEMORY_ERROR;
			 }
			 uiDeviceNum++;
		 } while (xml.NextElem());
	 }
	 xml.OutOfElem();
	 if (!xml.FindElemEx("Content/Plan/RecordMethod"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Del Record Plan Get XML", "fail", "no 'CameraList' elemenet");
		 return IVS_XML_INVALID;
	 }  

	 // 平台录像计划信息
	 GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,uiRecordMethod,xml);

	 return IVS_SUCCEED;
 }//lint !e550

 //查询录像计划:将请求XML转换为结构体
 IVS_INT32 COCXRecordXMLProcess::GetRecordPlanGetXML(const char* pCameraCode, IVS_UINT32 &uiRecordMethod, CXml &xml)
 {
	 IVS_DEBUG_TRACE("");
	 if (!xml.FindElemEx("Content"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Record Plan Get XML", "fail", "no 'Content' elemenet");
		 return IVS_XML_INVALID;
	 }
	 if (!xml.FindElemEx("Content/CameraCode"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Record Plan Get XML", "fail", "no 'CameraCode' elemenet");
		 return IVS_XML_INVALID;
	 }
	 const char* szElemValue = NULL;
	 GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,pCameraCode,IVS_DEV_CODE_LEN,xml);
	 if (!xml.FindElemEx("Content/RecordMethod"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Record Plan Get XML", "fail", "no 'RecordMethod' elemenet");
		 return IVS_XML_INVALID;
	 }
	 // 平台录像计划信息
	 GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,uiRecordMethod,xml);
	 xml.OutOfElem();

	 return IVS_SUCCEED;
 }//lint !e550

 //添加/修改/删除录像计划:将结果结构体转换为响应XML
  IVS_INT32 COCXRecordXMLProcess::RecordPlanGetRspXML(IVS_INT32 iRet,const IVS_RESULT_LIST* resultList, CXml &xml)
  {
      CHECK_POINTER(resultList,IVS_PARA_INVALID);
	  IVS_DEBUG_TRACE("");

      (void)xml.AddDeclaration("1.0","UTF-8","");
      (void)xml.AddElem("Content");
	  (void)xml.AddChildElem("ResultCode");
	  (void)xml.IntoElem();
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	  //xml.OutOfElem();
      (void)xml.AddElem("ResultList");
      for (IVS_UINT32 i=0; i<resultList->uiNum; i++)
      {
		  if (i == 0)
		  {        
			  (void)xml.AddChildElem("ResultInfo");   
			  (void)xml.IntoElem();   
		  }
		  else
		  {
			  (void)xml.AddElem("ResultInfo"); 
		  }

		  (void)xml.AddChildElem("CameraCode");
		  (void)xml.IntoElem();
		  (void)xml.SetElemValue(resultList->stInfo[i].cCode);
		  (void)xml.AddElem("ResultCode"); 
		  (void)xml.SetElemValue(CToolsHelp::Int2Str(resultList->stInfo[i].iResult).c_str());
		  xml.OutOfElem();
      }
      xml.OutOfElem();
     // xml.OutOfElem();
      return IVS_SUCCEED;
  }

 //添加/修改/删除录像计划:将结果结构体转换为响应XML(用于异常情况返回)
  void COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(IVS_INT32 iRet,IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, CXml &xml)
  {
	  CHECK_POINTER_VOID(pDeviceList);
	  IVS_DEBUG_TRACE("");

	  (void)xml.AddDeclaration("1.0","UTF-8","");
	  (void)xml.AddElem("Content");
	  (void)xml.AddChildElem("ResultCode");
	  (void)xml.IntoElem();
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(IVS_FAIL).c_str());
	  //xml.OutOfElem();
	  (void)xml.AddElem("ResultList");
      for (IVS_UINT32 i=0; i<uiDeviceNum; i++)
      {
		  if (i == 0)
		  {        
			  (void)xml.AddChildElem("ResultInfo");   
			  (void)xml.IntoElem();   
		  }
		  else
		  {
			  (void)xml.AddElem("ResultInfo"); 
		  }

		  (void)xml.AddChildElem("CameraCode");
		  (void)xml.IntoElem();
		  IVS_CHAR cCode[IVS_DEV_CODE_LEN+1];
		  memset(cCode,0,IVS_DEV_CODE_LEN+1);
		  bool bRet = CToolsHelp::Memcpy(cCode,IVS_DEV_CODE_LEN+1,pDeviceList[i].cDevCode,IVS_DEV_CODE_LEN); 
		  if(false == bRet)
		  {
			  BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Record Plan Get Rsp XML By pDeviceList", "Memcpy error.");
			  //return IVS_ALLOC_MEMORY_ERROR;
		  }
		  (void)xml.SetElemValue(cCode);
		  (void)xml.AddElem("ResultCode"); 
		  (void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
		  xml.OutOfElem();
      }
      xml.OutOfElem();
     
      
  }

 //查询录像计划:将结果结构体转换为响应XML
  IVS_INT32 COCXRecordXMLProcess::GetRecordPlanGetRspXML(IVS_INT32 iRet,const char* pCameraCode,const IVS_RECORD_PLAN* pRecordPlan, CXml &xml)
  {
	  CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	  CHECK_POINTER(pRecordPlan,IVS_PARA_INVALID);
	  IVS_DEBUG_TRACE("");

	  (void)xml.AddDeclaration("1.0","UTF-8","");
	  (void)xml.AddElem("Content");
	  (void)xml.AddChildElem("ResultCode");
	  (void)xml.IntoElem();
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	  //xml.OutOfElem();

	  (void)xml.AddElem("Plan");
	  (void)xml.AddChildElem("CameraCode");
	  (void)xml.IntoElem();//------------5
	  (void)xml.SetElemValue(pCameraCode); 
	  (void)xml.AddElem("RecordMethod");
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->uiRecordMethod)).c_str()); 
	  (void)xml.AddElem("IsEnable");
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(pRecordPlan->bEnableRecordPlan).c_str()); 
	  (void)xml.AddElem("PlanType");
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordPlan->uiRecordPlanType)).c_str()); 
	  IVS_CHAR cStartTime[IVS_TIME_LEN+1] = {0};
	  IVS_CHAR cEndTime[IVS_TIME_LEN+1] = {0}; 
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
				  if (!CToolsHelp::Memcpy(cStartTime, IVS_TIME_LEN, pRecordPlan->stPlanInfo[i].stTimeSpanInfo[j].cStart, IVS_TIME_LEN))
				  {
					  BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record Plan Get Rsp XML", "Memcpy cStartTime failed");
					  return IVS_ALLOC_MEMORY_ERROR;
				  }
				  if (!CToolsHelp::Memcpy(cEndTime, IVS_TIME_LEN,pRecordPlan->stPlanInfo[i].stTimeSpanInfo[j].cEnd, IVS_TIME_LEN))
				  {
					  BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record Plan Get Rsp XML", "Memcpy cEndTime failed");
					  return IVS_ALLOC_MEMORY_ERROR;
				  }
				  (void)xml.AddChildElem("StartTime");
				  (void)xml.IntoElem();//-------------------------1
				  (void)xml.SetElemValue(cStartTime);
				  (void)xml.AddElem("EndTime");
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
 //录像状态查询
  IVS_INT32 COCXRecordXMLProcess::GetRecordStatusRspXML(IVS_INT32 iRet,const char* pCameraCode,IVS_UINT32 ulRecordMethod,IVS_UINT32 ulRecordStatus, CXml &xml)
  {
	  CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	  IVS_DEBUG_TRACE("");

	  (void)xml.AddDeclaration("1.0","UTF-8","");
	  (void)xml.AddElem("Content");
	  (void)xml.AddChildElem("ResultCode");
	  (void)xml.IntoElem();
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

	  (void)xml.AddElem("CameraCode");
	  (void)xml.SetElemValue(pCameraCode);
	  (void)xml.AddElem("RecordMethod");
	  (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(ulRecordMethod)).c_str());
	  (void)xml.AddElem("RecordState");

	  std::string str = COCXXmlProcess::TransDecToBinStr(ulRecordStatus);
	  int iStatus = COCXXmlProcess::TransStrToInt(str);
	  CString strTempStatus;
	  strTempStatus.Format("%03d",iStatus);
	  (void)xml.SetElemValue(strTempStatus);
	  xml.OutOfElem();

	  return IVS_SUCCEED;
  }
 //解析本地录像配置参数XML
IVS_INT32 COCXRecordXMLProcess::LocalRecordParseXML(IVS_LOCAL_RECORD_PARAM &LocalParam, LPCTSTR pXmlStr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pXmlStr, IVS_OPERATE_MEMORY_ERROR);
	
	CXml xml;
	if (!xml.Parse(pXmlStr))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Local Record Parse XML", "parse xml error, use default");	//lint !e40
    	LocalParam.uiNameRule = RULE_NAME_NUM_TIME;
		bool bRet = CToolsHelp::Strncpy(LocalParam.cSavePath, sizeof(LocalParam.cSavePath) - 1, RECORD_DIRECTORY, sizeof(RECORD_DIRECTORY));  
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Local Record Parse XML", "Strncpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
    	LocalParam.uiRecordFormat  = IVS_FILE_MP4;
		LocalParam.uiRecordTime    = RECORD_TIME_MAX;
		LocalParam.uiSplitterType  = RECORD_SPLITE_TIME;
		LocalParam.uiSplitterValue = RECORD_TIME_SPLIT;
		LocalParam.uiDiskWarningValue = 1024;
		LocalParam.uiStopRecordValue  = 500;
    	return IVS_SUCCEED;
	}
		
	if (!xml.FindElemEx("Content/RecordParam"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Local Record Parse XML fail", "no 'Content/RecordParam' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
  
	const char *value = NULL;
	bool  bSuccess = true;

	bSuccess = (xml.FindElem("RecordFormat")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiRecordFormat = bSuccess ? atoi(value) : IVS_FILE_MP4;

	bSuccess = (xml.FindElem("SplitterType")) &&(NULL != (value = xml.GetElemValue()));
	LocalParam.uiSplitterType = bSuccess ? atoi(value) : RECORD_SPLITE_TIME;

	bSuccess = (xml.FindElem("SplitterValue")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiSplitterValue = bSuccess ? (unsigned long)atoi(value) :
		    LocalParam.uiSplitterValue == RECORD_SPLITE_TIME ? 
                           RECORD_TIME_SPLIT : RECORD_SIZE_SPLIT;

	bSuccess = (xml.FindElem("DiskWarningValue")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiDiskWarningValue = bSuccess ? (unsigned long)atoi(value) : RECORD_DISK_WARN;

	bSuccess = (xml.FindElem("StopRecordValue")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiStopRecordValue = bSuccess ? (unsigned long)atoi(value) : RECORD_DISK_FULL;

	bSuccess = (xml.FindElem("RecordTime")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiRecordTime = bSuccess ? (unsigned long)atoi(value) : RECORD_TIME_MAX;

	bSuccess = (xml.FindElem("NameRule")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiNameRule = bSuccess ? atoi(value) : RULE_NAME_NUM_TIME;

	bSuccess = (xml.FindElem("SavePath")) && (NULL != (value = xml.GetElemValue()));
	bool bRet = false;
	if (bSuccess)
	{
		bRet = CToolsHelp::Strncpy(LocalParam.cSavePath, sizeof(LocalParam.cSavePath) - 1, value, strlen(value));  
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Local Record Parse XML", "Strncpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}
	else
	{
		bRet = CToolsHelp::Strncpy(LocalParam.cSavePath, sizeof(LocalParam.cSavePath) - 1, RECORD_DIRECTORY, sizeof(RECORD_DIRECTORY));  
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Local Record Parse XML", "Strncpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}

	return IVS_SUCCEED;
}

/*
Content	Element	M	
    CameraCode	Element	M	摄像机编码
    FileName	Element	M	文件名
    FilePath	Element	M	文件存放路径
	StartTime	Element	M	平台录像开始时间
	EndTime		Element	M	平台录像结束时间
	RecordType	Element	M	录像类型
							0：本地录像
							1：平台下载录像
	IsEncrypt	Element	M	录像是否加密
							0：不加密
							1：加密
*/


#define CHECK_SUCCESS(s)\
	do { if(!(s)) return IVS_FAIL;} while(0)
//本地录像创建xml
IVS_INT32 COCXRecordXMLProcess::LocalRecordBuildXML(IVS_LOCAL_RECORD_SUCCESS_INFO *pInfo, unsigned long ulHandle, CXml &xml)//lint !e830
{
	IVS_DEBUG_TRACE("");

	CHECK_POINTER(pInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_SUCCESS(xml.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xml.AddElem("Content"));

	CHECK_SUCCESS(xml.AddChildElem("CameraCode"));
	xml.IntoElem();
	CHECK_SUCCESS(xml.SetElemValue(pInfo->cCameraCode));
	
	CHECK_SUCCESS(xml.AddElem("FileName"));
	CHECK_SUCCESS(xml.SetElemValue(pInfo->cFileName));

	CHECK_SUCCESS(xml.AddElem("FilePath"));
	CHECK_SUCCESS(xml.SetElemValue(pInfo->cFilePath));

	CHECK_SUCCESS(xml.AddElem("OperationTime"));
	CHECK_SUCCESS(xml.SetElemValue(pInfo->cOperationTime));

	CHECK_SUCCESS(xml.AddElem("StartTime"));
	CHECK_SUCCESS(xml.SetElemValue(pInfo->stRecordTime.cStart));

	CHECK_SUCCESS(xml.AddElem("EndTime"));
	CHECK_SUCCESS(xml.SetElemValue(pInfo->stRecordTime.cEnd));
 
	CHECK_SUCCESS(xml.AddElem("RecordType"));
	CHECK_SUCCESS(xml.SetElemValue(pInfo->uiRecordType ? "1" : "0"));

	CHECK_SUCCESS(xml.AddElem("IsEncrypt"));
	CHECK_SUCCESS(xml.SetElemValue(pInfo->bIsEncrypt? "1" : "0"));

	char buf[64] = {0};
	itoa(ulHandle, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.AddElem("Handle"));
	CHECK_SUCCESS(xml.SetElemValue(buf));
	xml.OutOfElem();
	return IVS_SUCCEED;
}//lint !e818

//本地录像请求xml
IVS_INT32 COCXRecordXMLProcess::LocalRecordBuidlXMLGeneric(int code, int wndID, CXml &xml)
{
	IVS_DEBUG_TRACE("");

	CHECK_SUCCESS(xml.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xml.AddElem("Content"));
	CHECK_SUCCESS(xml.AddChildElem("WndID"));
	
	xml.IntoElem();
	
	char buf[64] = {0};
	(void)_itoa_s(wndID, buf, 10);
	CHECK_SUCCESS(xml.SetElemValue(buf));
	CHECK_SUCCESS(xml.AddElem("EventID"));
	(void)_itoa_s(code, buf, 10);
	CHECK_SUCCESS(xml.SetElemValue(buf));
	return IVS_SUCCEED;
}

/*
<RecordLocalParam>
<RecordPath>D:\1080p Record</RecordPath>
<DownLoadPath>C:\IVS\LocalDownLoadFile</DownLoadPath>
<ManualRecordTime>300</ManualRecordTime>
<FileSeparateType>1</FileSeparateType>
<FileSeparateValue>3000</FileSeparateValue>
<MinDiskCapacity>1233</MinDiskCapacity>
<WarningDiskCapacity>1000</WarningDiskCapacity>
<VedioType>1</VedioType>
<NameRule>2</NameRule>
<DownloadSpeed>0</DownloadSpeed>
</RecordLocalParam>
 *
 */

/*
 * 读取CU配置，暂时转测使用
 *
 */
//读取CU配置XML 转测使用
IVS_INT32 COCXRecordXMLProcess::LocalRecordParseClientConfig(IVS_LOCAL_RECORD_PARAM &LocalParam, LPCTSTR pXmlStr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pXmlStr, IVS_OPERATE_MEMORY_ERROR);
	CXml xml;
	bool bRet = false;
	if (!xml.Parse(pXmlStr))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Local Record Parse Client Config", "parse xml error, use default");	//lint !e40
		LocalParam.uiNameRule = RULE_NAME_NUM_TIME;
		bRet = CToolsHelp::Strncpy(LocalParam.cSavePath, sizeof(LocalParam.cSavePath) - 1, RECORD_DIRECTORY, sizeof(RECORD_DIRECTORY));  
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Local Record Parse Client Config", "Strncpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		LocalParam.uiRecordFormat  = IVS_FILE_MP4;
		LocalParam.uiRecordTime    = RECORD_TIME_MAX;
		LocalParam.uiSplitterType  = RECORD_SPLITE_TIME;
		LocalParam.uiSplitterValue = RECORD_TIME_SPLIT;
		LocalParam.uiDiskWarningValue = 1024;
		LocalParam.uiStopRecordValue  = 500;
		return IVS_SUCCEED;
	}

	if (!xml.FindElemEx("RecordLocalParam"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Local Record Parse Client Config fail", "no 'RecordLocalParam' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	const char *value = NULL;
	bool  bSuccess = true;

	bSuccess = (xml.FindElem("VedioType")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiRecordFormat = bSuccess ? atoi(value) : IVS_FILE_MP4;

	bSuccess = (xml.FindElem("FileSeparateType")) &&(NULL != (value = xml.GetElemValue()));
	LocalParam.uiSplitterType = bSuccess ? atoi(value) : RECORD_SPLITE_TIME;

	bSuccess = (xml.FindElem("FileSeparateValue")) && (NULL != (value = xml.GetElemValue()));

	if (bSuccess)
	{
		LocalParam.uiSplitterValue = (IVS_UINT32)atoi(value); 
	}
	else
	{
		LocalParam.uiSplitterValue = (LocalParam.uiSplitterType == RECORD_SPLITE_TIME) ? 
                 RECORD_TIME_SPLIT : RECORD_SIZE_SPLIT;
	}
 
	bSuccess = (xml.FindElem("ManualRecordTime")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiRecordTime = bSuccess ? (unsigned long)atoi(value) : RECORD_TIME_MAX;

	bSuccess = (xml.FindElem("NameRule")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiNameRule = bSuccess ? atoi(value) : RULE_NAME_NUM_TIME;

	bSuccess = (xml.FindElem("WarningDiskCapacity")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiDiskWarningValue = bSuccess ? (unsigned long)atoi(value) : RECORD_DISK_WARN;

	bSuccess = (xml.FindElem("MinDiskCapacity")) && (NULL != (value = xml.GetElemValue()));
	LocalParam.uiStopRecordValue = bSuccess ? (unsigned long)atoi(value) : RECORD_DISK_FULL;

	bSuccess = (xml.FindElem("RecordPath")) && (NULL != (value = xml.GetElemValue()));
	if (bSuccess)
	{
		bRet = CToolsHelp::Strncpy(LocalParam.cSavePath, sizeof(LocalParam.cSavePath) - 1, value, strlen(value));  
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Local Record Parse Client Config", "Strncpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}
	else
	{
		bRet = CToolsHelp::Strncpy(LocalParam.cSavePath, sizeof(LocalParam.cSavePath) - 1, RECORD_DIRECTORY, sizeof(RECORD_DIRECTORY));  
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Local Record Parse Client Config", "Strncpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}

	return IVS_SUCCEED;
}
//录像书签列表
IVS_INT32 COCXRecordXMLProcess::GetRecordBookmarkListGetXML(const IVS_CHAR* pCameraCode,IVS_RECORD_BOOKMARK_INFO_LIST *pRecordBookmarkInfoList,IVS_INT32 iRetCode,CXml &xml)
{
	if (NULL == pRecordBookmarkInfoList)
	{
		return IVS_PARA_INVALID;
	}
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCode).c_str());

	(void)xml.AddElem("PageInfo");
	(void)xml.AddChildElem("RealNum");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordBookmarkInfoList->uiTotal)).c_str());
	(void)xml.AddElem("FromIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordBookmarkInfoList->stIndexRange.uiFromIndex)).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRecordBookmarkInfoList->stIndexRange.uiToIndex)).c_str());
	xml.OutOfElem();

	if(pRecordBookmarkInfoList->uiTotal>0)
	{
		IVS_INT32 iToIndex = static_cast<IVS_INT32>(pRecordBookmarkInfoList->stIndexRange.uiToIndex);
		IVS_INT32 iFromIndex = static_cast<IVS_INT32>(pRecordBookmarkInfoList->stIndexRange.uiFromIndex);
		int iRealNum = static_cast<IVS_INT32>(pRecordBookmarkInfoList->uiTotal);
		int iRequestTotal = (iToIndex - iFromIndex) + 1;
		if (iRequestTotal < iRealNum && iToIndex < iRealNum )
		{
			iRealNum = iRequestTotal;
		}
		else
		{
			iRealNum = (iRealNum - iFromIndex) +1;
		}
		(void)xml.AddElem("RecordBookmarkList");
		for (int iIndex = 0;iIndex<iRealNum;iIndex++)
		{
			IVS_RECORD_BOOKMARK_INFO &stBookmarkInfo = pRecordBookmarkInfoList->stBookmarkInfo[iIndex];
			if(0 == iIndex)
			{
				(void)xml.AddChildElem("RecordBookmark");
				(void)(xml.IntoElem());
			}
			else
			{
				(void)xml.AddElem("RecordBookmark");
			}
			(void)xml.AddChildElem("BookmarkID");
			(void)(xml.IntoElem());
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stBookmarkInfo.uiBookmarkID)).c_str());

			(void)xml.AddElem("NVRCode");
			IVS_CHAR cNvrCode[IVS_NVR_CODE_LEN+1];
			memset(cNvrCode,0,IVS_NVR_CODE_LEN+1);
			bool bRet = CToolsHelp::Memcpy(cNvrCode,IVS_NVR_CODE_LEN,stBookmarkInfo.cNvrCode,IVS_NVR_CODE_LEN);
			if(!bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record Bookmark List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cNvrCode);

			(void)xml.AddElem("MBUDomain");
			IVS_CHAR cMBUDomain[IVS_DOMAIN_CODE_LEN+1];
			memset(cMBUDomain,0,IVS_DOMAIN_CODE_LEN+1);
			bRet = CToolsHelp::Memcpy(cMBUDomain,IVS_DOMAIN_CODE_LEN,stBookmarkInfo.cMBUDomain,IVS_DOMAIN_CODE_LEN);
			if(!bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record Bookmark List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cMBUDomain);

			(void)xml.AddElem("CameraCode");
			(void)xml.SetElemValue(pCameraCode);
			(void)xml.AddElem("BookmarkName");
			IVS_CHAR cBookName[IVS_BOOKMARK_NAME_LEN+1];
			memset(cBookName,0,IVS_BOOKMARK_NAME_LEN+1);
			bRet = CToolsHelp::Memcpy(cBookName,IVS_BOOKMARK_NAME_LEN,stBookmarkInfo.cBookmarkName,IVS_BOOKMARK_NAME_LEN);
			if(!bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record Bookmark List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cBookName);
			(void)xml.AddElem("BookmarkTime");
			IVS_CHAR cBookTime[IVS_TIME_LEN+1];
			memset(cBookTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cBookTime,IVS_TIME_LEN,stBookmarkInfo.cBookmarkTime,IVS_TIME_LEN);
			if(!bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record Bookmark List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cBookTime);
			(void)xml.AddElem("BookmarkCreatorDomain");
			IVS_CHAR cUserDomain[IVS_DOMAIN_CODE_LEN+1];
			memset(cUserDomain,0,IVS_DOMAIN_CODE_LEN+1);
			bRet = CToolsHelp::Memcpy(cUserDomain,IVS_DOMAIN_CODE_LEN,stBookmarkInfo.cUserDomain,IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record Bookmark List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cUserDomain);
			(void)xml.AddElem("BookmarkCreatorName");
			IVS_CHAR cBookCreatorName[IVS_NAME_LEN+1];
			memset(cBookCreatorName,0,IVS_NAME_LEN+1);
			bRet = CToolsHelp::Memcpy(cBookCreatorName,IVS_NAME_LEN,stBookmarkInfo.cBookmarkCreatorName,IVS_NAME_LEN);
			if(!bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Record Bookmark List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cBookCreatorName);

			(void)xml.AddElem("BookmarkCreatorID");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stBookmarkInfo.uiBookmarkCreatorID)).c_str());
			
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 手动录像状态上报拼接XML
IVS_INT32 COCXRecordXMLProcess::ManualRecordStateGetXML(const IVS_MANUAL_RECORD_STATUS_NOTIFY* pManualRecordStatusNotify, CXml& xml)
{
	CHECK_POINTER(pManualRecordStatusNotify,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pManualRecordStatusNotify->cCameraCode);
	(void)xml.AddElem("UserID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pManualRecordStatusNotify->uiUserID)).c_str());
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pManualRecordStatusNotify->uiRecordMethod)).c_str());
	(void)xml.AddElem("ManualRecordState");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pManualRecordStatusNotify->uiManualRecordState)).c_str());
	(void)xml.AddElem("NotifInfo");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pManualRecordStatusNotify->uiNotifInfo)).c_str());

	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;
}

/*
 * RecordDownLoadParseXml
 */
IVS_INT32 COCXRecordXMLProcess::RecordDownLoadParseXml(IVS_DOWNLOAD_PARAM &DLParam, LPCTSTR pXmlStr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pXmlStr, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	if (!xml.Parse(pXmlStr))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Record DownLoad Parse Xml", "parse xml error");	//lint !e40
		return IVS_XML_INVALID;
	}

	if (!xml.FindElemEx("Content/DownloadParam"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Record DownLoad Parse Xml", "no 'Content/DownloadParam' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	const char *value = NULL;
    
	CHECK_SUCCESS(xml.FindElem("RecordFormat") && NULL != (value = xml.GetElemValue()));
    DLParam.uiRecordFormat = (unsigned int)atoi(value);

	CHECK_SUCCESS(xml.FindElem("SplitterType") && NULL != (value = xml.GetElemValue()));
	DLParam.uiSplitterType = (unsigned int)atoi(value);

	CHECK_SUCCESS(xml.FindElem("SplitterValue") && NULL != (value = xml.GetElemValue()));
	DLParam.uiSplitterValue = (unsigned int)atoi(value);

	CHECK_SUCCESS(xml.FindElem("DiskWarningValue") && NULL != (value = xml.GetElemValue()));
	DLParam.uiDiskWarningValue = (unsigned int)atoi(value);

	CHECK_SUCCESS(xml.FindElem("StopRecordValue") && NULL != (value = xml.GetElemValue()));
	DLParam.uiStopRecordValue = (unsigned int)atoi(value);

	CHECK_SUCCESS(xml.FindElem("NameRule") && NULL != (value = xml.GetElemValue()));
	DLParam.uiNameRule = (unsigned int)atoi(value);

	DLParam.bEncryptRecord = 0;
	CHECK_SUCCESS(xml.FindElem("EncryptRecord") && NULL != (value = xml.GetElemValue()));
	DLParam.bEncryptRecord = atoi(value);

    if (DLParam.bEncryptRecord)
	{
		CHECK_SUCCESS(xml.FindElem("RecordPWD") && NULL != (value = xml.GetElemValue()));
		(void)CToolsHelp::Strncpy(DLParam.cRecordPWD, sizeof(DLParam.cRecordPWD) - 1, value, strlen(value));  
	}

	CHECK_SUCCESS(xml.FindElem("SavePath") && NULL != (value = xml.GetElemValue()));
	(void)CToolsHelp::Strncpy(DLParam.cSavePath, sizeof(DLParam.cSavePath) - 1, value, strlen(value));  

	CHECK_SUCCESS(xml.FindElem("DownloadSpeed") && NULL != (value = xml.GetElemValue()));
	DLParam.uiDownloadSpeed = (unsigned int)atoi(value);
    if (xml.FindElem("SaveFileName") && NULL != (value = xml.GetElemValue()))
	{
		(void)CToolsHelp::Strncpy(DLParam.cFileName, sizeof(DLParam.cFileName) - 1, value, strlen(value));  
	}
	return IVS_SUCCEED;
}


/*
 * RecordDownLoadBuildXml
 */
IVS_INT32 COCXRecordXMLProcess::RecordDownLoadBuildXml(unsigned long ulHandle, unsigned int uiCode, CXml &xml)
{
	IVS_DEBUG_TRACE("");

	CHECK_SUCCESS(xml.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xml.AddElem("Content"));
	CHECK_SUCCESS(xml.AddChildElem("ResultCode"));
	xml.IntoElem();

	char buf[64] = {0};
	(void)_itoa_s(uiCode, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("DownloadHandle"));
	(void)_itoa_s(ulHandle, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));
	return IVS_SUCCEED;
}

// 构造下载信息XML
IVS_INT32 COCXRecordXMLProcess::RecordDownLoadInfoBuildXml(int ResultCode, IVS_DOWNLOAD_INFO& DLInfo, CXml &xml)
{
	IVS_DEBUG_TRACE("");

	CHECK_SUCCESS(xml.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xml.AddElem("Content"));
	CHECK_SUCCESS(xml.AddChildElem("ResultCode"));
	xml.IntoElem();

	char buf[64] = {0};
	(void)_itoa_s(ResultCode, buf, 10);
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("DownloadInfo"));

	CHECK_SUCCESS(xml.AddChildElem("Status"));
	xml.IntoElem();
	(void)_itoa_s(DLInfo.uiDownloadStatus, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("Speed"));
	(void)_itoa_s(DLInfo.uiDownloadSpeed, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("Size"));
	(void)_itoa_s(DLInfo.uiDownloadSize, buf, 10);//lint !e713 !e712 !e747
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("TimeLeft"));
	(void)_itoa_s(DLInfo.uiTimeLeft, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("Progress"));
	(void)_itoa_s(DLInfo.uiProgress, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));

	return IVS_SUCCEED;
}

// 录像锁定解析xml
IVS_INT32 COCXRecordXMLProcess::LockRecordGetXML(const std::string& strLockInfo, std::string& strDomainCode,std::string& strNVRCode, std::string& strCameraCode, IVS_RECORD_LOCK_INFO* pRecordLockInfo)
{
	CHECK_POINTER(pRecordLockInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strLockInfo.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "Lock Record Get XML:parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/CameraCode"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "Lock Record Get XML:can not find CameraCode", "NA");
		return iRet;
	}
	const char* pCameraCode = xml.GetElemValue();
	CHECK_POINTER(pCameraCode,IVS_XML_INVALID);
	strCameraCode = pCameraCode;

	if (xml.FindElemEx("Content/NVRCode"))
	{
		const char* pNVRCode = xml.GetElemValue();
		CHECK_POINTER(pNVRCode,IVS_XML_INVALID);
		strNVRCode = pNVRCode;
	}
	if (xml.FindElemEx("Content/DomainCode"))
	{
		const char* pDomainCode = xml.GetElemValue();
		CHECK_POINTER(pDomainCode,IVS_XML_INVALID);
		strDomainCode = pDomainCode;
	}

	if (!xml.FindElemEx("Content/LockInfo"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find Content/LockInfo", "NA");
		return iRet;
	}
	xml.IntoElem();
	const char* szElemValue = NULL;
	GET_ELEM_VALUE_CHAR("LockTime",szElemValue,  pRecordLockInfo->cLockTime,IVS_TIME_LEN,xml);
	GET_ELEM_VALUE_CHAR("LockStartTime",szElemValue, pRecordLockInfo->stLockTimeSpan.cStart,IVS_TIME_LEN,xml);
	GET_ELEM_VALUE_CHAR("LockEndTime",szElemValue,  pRecordLockInfo->stLockTimeSpan.cEnd,IVS_TIME_LEN,xml);
	GET_ELEM_VALUE_CHAR("LockDesc",szElemValue,  pRecordLockInfo->cLockDesc,IVS_RECORD_LOCK_DESC_LEN,xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("OperatorID",szElemValue,pRecordLockInfo->uiOperatorID,xml);
	GET_ELEM_VALUE_CHAR("OperatorName",szElemValue,  pRecordLockInfo->cOperatorName,IVS_NAME_LEN,xml);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 录像解锁解析xml
IVS_INT32 COCXRecordXMLProcess::UnLockRecordGetXML(const std::string& strUnLockInfo,std::string& strDomainCode,std::string& strNVRCode, std::string& strCameraCode, IVS_RECORD_LOCK_INFO* pRecordLockInfo)
{
	CHECK_POINTER(pRecordLockInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strUnLockInfo.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "UnLock Record Get XML:parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/CameraCode"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "UnLock Record Get XML:can not find CameraCode", "NA");
		return iRet;
	}
	const char* pCameraCode = xml.GetElemValue();
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	strCameraCode = pCameraCode;

	if (xml.FindElemEx("Content/NVRCode"))
	{
		const char* pNVRCode = xml.GetElemValue();
		CHECK_POINTER(pNVRCode,IVS_XML_INVALID);
		strNVRCode = pNVRCode;
	}
	if (xml.FindElemEx("Content/DomainCode"))
	{
		const char* pDomainCode = xml.GetElemValue();
		CHECK_POINTER(pDomainCode,IVS_XML_INVALID);
		strDomainCode = pDomainCode;
	}

	if (!xml.FindElemEx("Content/UnlockInfo"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find Content/UnlockInfo", "NA");
		return iRet;
	}
	xml.IntoElem();
	const char* szElemValue = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("LockID",szElemValue,pRecordLockInfo->uiLockID,xml);
	GET_ELEM_VALUE_CHAR("UnlockStartTime",szElemValue, pRecordLockInfo->stLockTimeSpan.cStart,IVS_TIME_LEN,xml);
	GET_ELEM_VALUE_CHAR("UnlockEndTime",szElemValue,  pRecordLockInfo->stLockTimeSpan.cEnd,IVS_TIME_LEN,xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("OperatorID",szElemValue,pRecordLockInfo->uiOperatorID,xml);
	GET_ELEM_VALUE_CHAR("OperatorName",szElemValue,  pRecordLockInfo->cOperatorName,IVS_NAME_LEN,xml);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 修改录像锁定信息解析xml
IVS_INT32 COCXRecordXMLProcess::ModLockRecordGetXML(const std::string& strLockInfo,std::string& strDomainCode, std::string& strNVRCode, std::string& strCameraCode, IVS_RECORD_LOCK_INFO* pRecordLockInfo)
{
	CHECK_POINTER(pRecordLockInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strLockInfo.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "Mod Lock Record Get XML:parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/CameraCode"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "Mod Lock Record Get XML:can not find CameraCode", "NA");
		return iRet;
	}
	const char* pCameraCode = xml.GetElemValue();
	CHECK_POINTER(pCameraCode,IVS_XML_INVALID);
	strCameraCode = pCameraCode;

	if (xml.FindElemEx("Content/DomainCode"))
	{
		const char* pDomainCode = xml.GetElemValue();
		CHECK_POINTER(pDomainCode,IVS_XML_INVALID);
		strDomainCode = pDomainCode;
	}
	if (xml.FindElemEx("Content/NVRCode"))
	{
		const char* pNVRCode = xml.GetElemValue();
		CHECK_POINTER(pNVRCode,IVS_XML_INVALID);
		strNVRCode = pNVRCode;
	}

	if (!xml.FindElemEx("Content/LockInfo"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find Content/LockInfo", "NA");
		return iRet;
	}
	xml.IntoElem();
	const char* szElemValue = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("LockID",szElemValue,pRecordLockInfo->uiLockID,xml);
	GET_ELEM_VALUE_CHAR("LockDesc",szElemValue,  pRecordLockInfo->cLockDesc,IVS_RECORD_LOCK_DESC_LEN,xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("OperatorID",szElemValue,pRecordLockInfo->uiOperatorID,xml);
	GET_ELEM_VALUE_CHAR("OperatorName",szElemValue,  pRecordLockInfo->cOperatorName,IVS_NAME_LEN,xml);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}


//添加书签生成结果XML
IVS_INT32 COCXRecordXMLProcess::AddBookmarkParseXML(IVS_UINT32 uiBookmarkID,IVS_INT32 iRetCode,CXml &xml)
{
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCode).c_str());
	(void)xml.AddElem("BookmarkID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiBookmarkID)).c_str());
	xml.OutOfElem();
	return IVS_SUCCEED;
}

IVS_INT32 COCXRecordXMLProcess::GetCameraPlanInfoParseXML(const IVS_CHAR* pReqXml, IVS_UINT32& uiPlanType)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);

    CXml xml;
    if (!xml.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "CXml Parse", "xml parsing error");
        return IVS_XML_INVALID;
    }

    const char* szElemValue = NULL;
    /*if(!xml.FindElemEx("Content"))
    {
    BP_RUN_LOG_INF("Get Camera Plan Info ParseXML", "Can't find 'Content'");
    return IVS_XML_INVALID;
    }
    GET_ELEM_VALUE_NUM_FOR_UINT("PlanType", szElemValue, uiPlanType, xml);*/

    if (xml.FindElemEx("Content/PlanType"))
    {
        GET_ELEM_VALUE_NUM_FOR_UINT("PlanType", szElemValue, uiPlanType, xml);
    }
    else
    {
        BP_RUN_LOG_INF("Get Camera Plan Info ParseXML", "Can't find 'Content/PlanType'");
        return IVS_XML_INVALID;
    }
    
    return IVS_SUCCEED;
}

