#include "stdafx.h"
#include "OCXCapturePictureXMLProcess.h"
#include "ToolsHelp.h"
#include "OCXXmlProcess.h"
#include "SDKDef.h"
#include "IVS_Trace.h"
OCXCapturePictureXMLProcess::OCXCapturePictureXMLProcess()
{
}

OCXCapturePictureXMLProcess::~OCXCapturePictureXMLProcess()
{
}


//设置平台抓拍计划:将请求XML转换为结构体
IVS_INT32 OCXCapturePictureXMLProcess::SetSnapshotPlanGetXML(IVS_CHAR* pCameraCode,IVS_SNAPSHOT_PLAN* pSnapShotPlan, CXml &xml)
{ 
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pSnapShotPlan,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElemEx("Content/SnapshotPlanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Set Snapshot Plan Get XML", "fail", "no 'SnapshotPlanInfo' elemenet");
		return IVS_XML_INVALID;
	}      
	(void)xml.IntoElem();
	const char* szElemValue = NULL;

	// 摄像机编码
	if (!xml.FindElem("CameraCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Set Snapshot Plan Get XML", "faill", "no 'CameraCode' elemenet");
		return IVS_XML_INVALID;
	}
	GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,pCameraCode,IVS_DEV_CODE_LEN,xml);
	// 是否启用 
	if (!xml.FindElem("IsEnable"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Set Snapshot Plan Get XML", "faill", "no 'IsEnable' elemenet");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* cpszIsEnable = xml.GetElemValue();
	if (NULL == cpszIsEnable)
	{
		return IVS_XML_INVALID;
	}
	pSnapShotPlan->bEnable = static_cast<IVS_BOOL>(atoi(cpszIsEnable));

	// 计划方式(周计划 日计划 全程录像)
	if (!xml.FindElem("PlanType"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Set Snapshot Plan Get XML", "faill", "no 'PlanType' elemenet");
		return IVS_XML_INVALID;
	}
	GET_ELEM_VALUE_NUM_FOR_UINT("PlanType",szElemValue,pSnapShotPlan->uiPlanType,xml);

	IVS_UINT32 iPlanInfoNum = 0;
	IVS_INT32 iTimeInfoNum = 0;
	if (xml.FindElemEx("Content/SnapshotPlanInfo/PlanList"))
	{
		do 
		{
			(void)xml.IntoElem();
			//解析PlanInfo
			(void)xml.IntoElem();
			memset(&pSnapShotPlan->stPlanInfo[iPlanInfoNum],0, sizeof(IVS_SNAPSHOT_PLAN_INFO));
			GET_ELEM_VALUE_NUM_FOR_UINT("DayType",szElemValue,pSnapShotPlan->stPlanInfo[iPlanInfoNum].uiDayType,xml);

			if (xml.FindElem("TimeList"))
			{
				xml.IntoElem();
			}
			if(xml.FindElem("TimeInfo"))
			{
				do{ 
					(void)xml.IntoElem();
					GET_ELEM_VALUE_CHAR("StartTime",szElemValue,  pSnapShotPlan->stPlanInfo[iPlanInfoNum].stTimeSpanInfo[iTimeInfoNum].cStart,IVS_TIME_LEN,xml);
					GET_ELEM_VALUE_CHAR("EndTime",szElemValue,  pSnapShotPlan->stPlanInfo[iPlanInfoNum].stTimeSpanInfo[iTimeInfoNum].cEnd,IVS_TIME_LEN,xml);
					GET_ELEM_VALUE_NUM_FOR_UINT("SnapInterval",szElemValue,pSnapShotPlan->stPlanInfo[iPlanInfoNum].stTimeSpanInfo[iTimeInfoNum].SnapInterval,xml);
					iTimeInfoNum++;
					xml.OutOfElem();
				}while(xml.NextElem());
			}
			pSnapShotPlan->stPlanInfo[iPlanInfoNum].uiTimeSpanInfoNum = static_cast<IVS_UINT32>(iTimeInfoNum);
			xml.OutOfElem();
			xml.OutOfElem();
			iPlanInfoNum++;
			iTimeInfoNum = 0;
		} while(xml.NextElem());
	}
	pSnapShotPlan->uiPlanInfoNum = iPlanInfoNum;
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//查询平台抓拍计划:将响应结构体转换为结果XML
IVS_INT32 OCXCapturePictureXMLProcess::GetSnapshotPlanGetXML(IVS_INT32 iRet,const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml &xml)
{ 
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pSnapShotPlan, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iRet)).c_str());
	(void)xml.AddElem("SnapshotPlanInfo");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pCameraCode);
	(void)xml.AddElem("IsEnable");   
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSnapShotPlan->bEnable)).c_str());
	(void)xml.AddElem("PlanType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSnapShotPlan->uiPlanType)).c_str());

	(void)xml.AddElem("PlanList");
	IVS_CHAR cStartTime[IVS_TIME_LEN+1] = {0};
	IVS_CHAR cEndTime[IVS_TIME_LEN+1] = {0}; 
	for (IVS_UINT32 i=0; i<pSnapShotPlan->uiPlanInfoNum; i++)
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
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSnapShotPlan->stPlanInfo[i].uiDayType)).c_str());
		(void)xml.AddElem("TimeList");    
		for (IVS_UINT32 j=0; j<pSnapShotPlan->stPlanInfo[i].uiTimeSpanInfoNum; j++)
		{
			if (j == 0)
			{
				(void)xml.AddChildElem("TimeInfo");
				(void)xml.IntoElem();// ---------------------- 2
			}
			else{
				(void)xml.AddElem("TimeInfo");
			}
			if (!CToolsHelp::Memcpy(cStartTime, IVS_TIME_LEN, pSnapShotPlan->stPlanInfo[i].stTimeSpanInfo[j].cStart, IVS_TIME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snapshot Plan Get XML", "Memcpy cStartTime failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(cEndTime, IVS_TIME_LEN, pSnapShotPlan->stPlanInfo[i].stTimeSpanInfo[j].cEnd, IVS_TIME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snapshot Plan Get XML", "Memcpy cEndTime failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.AddChildElem("StartTime");
			(void)xml.IntoElem();//-------------------------1
			(void)xml.SetElemValue(cStartTime);
			(void)xml.AddElem("EndTime");
			(void)xml.SetElemValue(cEndTime); 
			(void)xml.AddElem("SnapInterval");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSnapShotPlan->stPlanInfo[i].stTimeSpanInfo[j].SnapInterval)).c_str()); 
			xml.OutOfElem();//-------------------------1
		}  
		xml.OutOfElem();// ---------------------2
		xml.OutOfElem();//----------------3
	}
	xml.OutOfElem();//----------------4
	xml.OutOfElem();

	return IVS_SUCCEED;
}


// 解析抓拍图片查询参数xml
IVS_INT32 OCXCapturePictureXMLProcess::GetSnapshotListParseXml(CXml &xml, IVS_QUERY_SNAPSHOT_PARAM *pQueryParam, const IVS_CHAR* pCameraCode)
{
	CHECK_POINTER(pQueryParam,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	const char* szElemValue = NULL;
	const char* pTemp = NULL;
	if (!xml.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content/PageInfo) is fail");
		return IVS_XML_INVALID;
	}
	GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,  pCameraCode,IVS_DEV_CODE_LEN, xml);

	if (!xml.FindElemEx("Content/PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content/PageInfo) is fail");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", pTemp, pQueryParam->stIndexRange.uiFromIndex, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", pTemp, pQueryParam->stIndexRange.uiToIndex, xml);

	if (!xml.FindElemEx("Content/TimeInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content/PageInfo) is fail");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	GET_ELEM_VALUE_CHAR("FromTime",szElemValue,  pQueryParam->stTimeSpan.cStart, IVS_TIME_LEN, xml);
	GET_ELEM_VALUE_CHAR("ToTime",szElemValue,  pQueryParam->stTimeSpan.cEnd, IVS_TIME_LEN, xml);
	xml.OutOfElem();
	GET_ELEM_VALUE_NUM_FOR_UINT("SnapType", pTemp, pQueryParam->uiSnapType, xml); // plint !e

	return IVS_SUCCEED;
}

// 拼装抓拍图片告警信息xml
IVS_INT32 OCXCapturePictureXMLProcess::GetSnapshotListGetXml(CXml &xml, const IVS_SNAPSHOT_INFO_LIST *pSnapshotList, IVS_UINT32 iTotalNum)
{
	CHECK_POINTER(pSnapshotList,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("PageInfo");
	(void)xml.AddChildElem("RealNum");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pSnapshotList->uiTotal).c_str());
	(void)xml.AddElem("FromIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pSnapshotList->stIndexRange.uiFromIndex).c_str());

	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pSnapshotList->stIndexRange.uiToIndex).c_str());
	xml.OutOfElem();

	(void)xml.AddElem("SnapshotList");
	IVS_UINT32 uIndex = 0;
	if ((pSnapshotList->uiTotal) > iTotalNum)
	{
		uIndex = iTotalNum;
	}
	else
	{
		uIndex = pSnapshotList->uiTotal;
	}
	for (IVS_UINT32 i = 0; i < uIndex; i++)
	{
		IVS_SNAPSHOT_INFO cSnapshotInfo = pSnapshotList->stSnapshotInfo[i];
		i == 0 ? xml.AddChildElem("SnapInfo") : xml.AddElem("SnapInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("PictureID");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)cSnapshotInfo.uiPictureID).c_str());

		(void)xml.AddElem("PictureName");
		IVS_CHAR cPictureName[IVS_FILE_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cPictureName, IVS_FILE_NAME_LEN, cSnapshotInfo.cPictureName, IVS_FILE_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snapshot List GetXml", "Memcpy pPictureName failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cPictureName);

		//直接用CU的大CameraCode
		(void)xml.AddElem("CameraCode");
		IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
 		if (!CToolsHelp::Memcpy(cCameraCode, IVS_DEV_CODE_LEN, cSnapshotInfo.cCameraCode, IVS_DEV_CODE_LEN))
 		{
 			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snapshot List GetXml", "Memcpy cCameraCode failed");
 			return IVS_ALLOC_MEMORY_ERROR;
 		}
		(void)xml.SetElemValue(cCameraCode);

		(void)xml.AddElem("SnapTime");
		IVS_CHAR cSnapTime[IVS_TIME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cSnapTime, IVS_TIME_LEN, cSnapshotInfo.cSnapTime, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snapshot List GetXml", "Memcpy cSnapTime failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cSnapTime);

		(void)xml.AddElem("SnapType");
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)cSnapshotInfo.uiSnapType).c_str());

		(void)xml.AddElem("PictureSize");
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)cSnapshotInfo.uiPictureSize).c_str());

		(void)xml.AddElem("PreviewUrl");
		IVS_CHAR cPreviewUrl[IVS_URL_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cPreviewUrl, IVS_URL_LEN, cSnapshotInfo.cPreviewUrl, IVS_URL_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snapshot List GetXml", "Memcpy cPreviewUrl failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cPreviewUrl);

		(void)xml.AddElem("PictureUrl");
		IVS_CHAR cPictureUrl[IVS_URL_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cPictureUrl, IVS_URL_LEN, cSnapshotInfo.cPictureUrl, IVS_URL_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snapshot List GetXml", "Memcpy cPictureUrl failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cPictureUrl);
		xml.OutOfElem();
	}

	xml.GetRootPos();

	return IVS_SUCCEED;
}


/*********************** 本地抓拍参数 *********************** 
<CaptureConfig>
	<CapturePath>C:\IVS\LocalPictureFile</CapturePath>
	<CaptureDownloadPath>C:\IVS\DownloadPictureFile</CaptureDownloadPath>
	<SnapshotMode>0</SnapshotMode>
	<SnapshotCount>1</SnapshotCount>
	<SnapshotInterval>1</SnapshotInterval>
	<SnapshotFormat>0</SnapshotFormat>
	<NameRule>1</NameRule>
</CaptureConfig>
**************************************************************/
// 解析本地抓拍参数客户端配置xml
IVS_INT32 OCXCapturePictureXMLProcess::LocalCaptureParseClientConfig(LPCTSTR pXmlStr, IVS_LOCAL_CAPTURE_PARAM &LocalCaptuParam)
{
	IVS_DEBUG_TRACE("Local Capture Config %s", pXmlStr);
	(void)CToolsHelp::Strncpy(LocalCaptuParam.cCapturePath, sizeof(LocalCaptuParam.cCapturePath) - 1, CAPTURE_DEFAULT_PATH, sizeof(CAPTURE_DEFAULT_PATH));  
	LocalCaptuParam.uiSnapshotMode = CAPTURE_DEFAULT_MODE;
	LocalCaptuParam.uiSnapshotCount = CAPTURE_DEFAULT_COUNT;			
	LocalCaptuParam.uiSnapshotInterval = CAPTURE_DEFAULT_INTERVAL;		
	LocalCaptuParam.uiSnapshotFormat = CAPTURE_DEFAULT_FORMAT;		
	LocalCaptuParam.uiNameRule = CAPTURE_DEFAULT_NAME_RULE;		
	if (NULL == pXmlStr || '\0' == pXmlStr[0])
	{
		return IVS_PARA_INVALID;
	}

	CXml xml;
	if (!xml.Parse(pXmlStr))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Local Capture Parse Client Config", "parse xml error, use default");
		return IVS_FAIL;
	}

	if (!xml.FindElemEx("CaptureConfig"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Local Capture Parse Client Config fail", "no 'CaptureConfig' elemenet");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	const char *value = NULL;
	bool  bSuccess = true;
	bool  bRet = false;

	bSuccess = (xml.FindElem("CapturePath")) && (NULL != (value = xml.GetElemValue()));
	if (bSuccess)
	{
		bRet = CToolsHelp::Strncpy(LocalCaptuParam.cCapturePath, sizeof(LocalCaptuParam.cCapturePath) - 1, value, strlen(value));  
	} else {
		bRet = CToolsHelp::Strncpy(LocalCaptuParam.cCapturePath, sizeof(LocalCaptuParam.cCapturePath) - 1, CAPTURE_DEFAULT_PATH, sizeof(CAPTURE_DEFAULT_PATH));
	}
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Local Capture Parse Client Config", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	bSuccess = (xml.FindElem("SnapshotMode")) && (NULL != (value = xml.GetElemValue()));
	LocalCaptuParam.uiSnapshotMode = bSuccess ? atoi(value) : CAPTURE_DEFAULT_MODE;

	bSuccess = (xml.FindElem("SnapshotCount")) &&(NULL != (value = xml.GetElemValue()));
	LocalCaptuParam.uiSnapshotCount = bSuccess ? (IVS_UINT32)atoi(value) : CAPTURE_DEFAULT_COUNT;

	bSuccess = (xml.FindElem("SnapshotInterval")) &&(NULL != (value = xml.GetElemValue()));
	LocalCaptuParam.uiSnapshotInterval = bSuccess ? (IVS_UINT32)atoi(value) : CAPTURE_DEFAULT_INTERVAL;
	
	bSuccess = (xml.FindElem("SnapshotFormat")) &&(NULL != (value = xml.GetElemValue()));
	LocalCaptuParam.uiSnapshotFormat = bSuccess ? atoi(value) : CAPTURE_DEFAULT_FORMAT;
	
	bSuccess = (xml.FindElem("NameRule")) && (NULL != (value = xml.GetElemValue()));
	LocalCaptuParam.uiNameRule = bSuccess ? atoi(value) : CAPTURE_DEFAULT_NAME_RULE;

	return IVS_SUCCEED;
}

#define CHECK_SUCCESS(s)\
	do { if(!(s)) return IVS_FAIL;} while(0)

/*********************** 本地抓拍返回给CU *********************** 
<Content>
	<WndID>1</WndID>
	<EventID>10000</EventID>
	<FileName>C:\IVS\LocalPictureFile\xxxxxxx.jpg</FileName>
</Content>
**************************************************************/
// 拼装客户端抓拍通用返回xml
IVS_INT32 OCXCapturePictureXMLProcess::LocalCaptureBuildXMLGeneric(LONG iEventCode, ULONG iWndID, const IVS_CHAR* pFileName, CXml &xml)
{
// 	CHECK_POINTER(pFileName, IVS_OPERATE_MEMORY_ERROR); // 可以为空
	IVS_DEBUG_TRACE("");
	char buf[IVS_FILE_NAME_LEN + 1] = {0};

	CHECK_SUCCESS(xml.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xml.AddElem("Content"));
	CHECK_SUCCESS(xml.AddChildElem("WndID"));
	xml.IntoElem();
	(void)_itoa_s((int)iWndID, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));
	
	CHECK_SUCCESS(xml.AddElem("EventID"));
	(void)_itoa_s(iEventCode, buf, 10);
	CHECK_SUCCESS(xml.SetElemValue(buf));
	
	if (NULL != pFileName)
	{
		CHECK_SUCCESS(xml.AddElem("FileName"));
		CHECK_SUCCESS(xml.SetElemValue(pFileName));
	}

	xml.OutOfElem();
	return IVS_SUCCEED;
}
