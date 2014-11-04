#include "CapturePictureXMLProcess.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"

// 拼装平台手动抓拍请求的xml
IVS_INT32 CapturePictureXMLProcess::PlatformSnapshotGetGenericXML(const IVS_CHAR* pCameraCode,CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN +1 ] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	(void)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装设置平台抓拍计划请求的xml
IVS_INT32 CapturePictureXMLProcess::SetSnapshotPlanGetXML(const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pSnapShotPlan, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("SnapshotPlanInfo");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("IsEnable");   
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSnapShotPlan->bEnable)).c_str());
	(void)xml.AddElem("PlanType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSnapShotPlan->uiPlanType)).c_str());

	(void)xml.AddElem("PlanList");
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
		IVS_CHAR cStartTime[IVS_TIME_LEN+1] = {0};
		IVS_CHAR cEndTime[IVS_TIME_LEN+1] = {0}; 
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
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Set Snapshot Plan Get XML", "Memcpy cStartTime failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(cEndTime, IVS_TIME_LEN, pSnapShotPlan->stPlanInfo[i].stTimeSpanInfo[j].cEnd, IVS_TIME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Set Snapshot Plan Get XML", "Memcpy cEndTime failed");
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

//解析查询平台抓拍计划响应的xml
IVS_INT32 CapturePictureXMLProcess::ParseGetSnapshotPlan(IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml& xml)
{
	CHECK_POINTER(pSnapShotPlan, IVS_PARA_INVALID); 
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/SnapshotPlanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Set Snapshot Plan Get XML", "fail", "no 'SnapshotPlanInfo' elemenet");
		return IVS_XML_INVALID;
	}      
	(void)xml.IntoElem();
	const char* szElemValue = NULL;

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


// 拼装抓拍图片查询参数xml
IVS_INT32 CapturePictureXMLProcess::GetSnapshotListGetXML(const IVS_CHAR* pCameraCode, const IVS_QUERY_SNAPSHOT_PARAM* pQueryParam, CXml &xml)
{
	CHECK_POINTER(pQueryParam, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_CHAR cDevCode[IVS_DEV_CODE_LEN+1]={0};
	IVS_CHAR cDomaCode[IVS_DOMAIN_CODE_LEN+1]={0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, cDevCode, cDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(cDomaCode);
	(void)xml.AddElem("CameraList");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(cDevCode);
	
	xml.OutOfElem();

	(void)xml.AddElem("PageInfo");
	(void)xml.AddChildElem("FromIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pQueryParam->stIndexRange.uiFromIndex).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pQueryParam->stIndexRange.uiToIndex).c_str());

	xml.OutOfElem();
    
	(void)xml.AddElem("TimeInfo");
	(void)xml.AddChildElem("FromTime");
	(void)xml.IntoElem();
	IVS_CHAR szStart[IVS_TIME_LEN + 1] = {0};
	IVS_CHAR szEnd[IVS_TIME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szStart, IVS_TIME_LEN, pQueryParam->stTimeSpan.cStart, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snap shot List Get XML", "Memcpy cStart failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szStart);

	(void)xml.AddElem("ToTime");
	if (!CToolsHelp::Memcpy(szEnd, IVS_TIME_LEN, pQueryParam->stTimeSpan.cEnd, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snap shot List Get XML", "Memcpy cEnd failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szEnd);
	xml.OutOfElem();
	(void)xml.AddElem("SnapType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pQueryParam->uiSnapType).c_str());

	xml.GetRootPos();
	return IVS_SUCCEED;
}

// 解析抓拍图片列表信息xml
IVS_INT32 CapturePictureXMLProcess::GetSnapshotListParseXML(CXml& xml, IVS_SNAPSHOT_INFO_LIST* pSnapshotList, IVS_UINT32 uiBufSize, const IVS_CHAR* pDomainCode)
{
	CHECK_POINTER(pSnapshotList, IVS_PARA_INVALID); 
	IVS_DEBUG_TRACE("");
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
	IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_NUM("FromIndex", pTemp, iTemp, xml);
	pSnapshotList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iTemp);
	iTemp = 0;
	GET_ELEM_VALUE_NUM("ToIndex", pTemp, iTemp, xml);
	pSnapshotList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iTemp);
	iTemp = 0;
	GET_ELEM_VALUE_NUM("RealNum",pTemp, iTemp, xml);
	pSnapshotList->uiTotal = static_cast<IVS_UINT32>(iTemp);
	iTemp = 0;

	IVS_UINT32 iRealNum = 0;
	IVS_UINT32 iTotalNum =  pSnapshotList->uiTotal;
	const char* szElemValue = NULL;

	if (0 == pSnapshotList->uiTotal)
	{
		return IVS_SUCCEED;
	}

	if (xml.FindElemEx("Content/SnapshotList/SnapInfo"))
	{
		do 
		{
			IVS_SNAPSHOT_INFO sSnapshoutInfo;
			IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
			memset(chDevCode, 0, IVS_DEV_CODE_LEN+1);
			memset(&sSnapshoutInfo, 0, sizeof(IVS_SNAPSHOT_INFO));
			(void)xml.IntoElem();

			GET_ELEM_VALUE_NUM("PictureID", pTemp, iTemp, xml);
			sSnapshoutInfo.uiPictureID = static_cast<IVS_UINT32>(iTemp);
			iTemp = 0;
			GET_ELEM_VALUE_CHAR("PictureName",szElemValue,  sSnapshoutInfo.cPictureName,IVS_FILE_NAME_LEN, xml);
		    GET_ELEM_VALUE_CHAR("CameraCode",szElemValue, chDevCode, IVS_DEV_CODE_LEN,xml);
			std::string strCameraCode = chDevCode;
			strCameraCode.append("#").append(pDomainCode);
			if (!CToolsHelp::Memcpy(sSnapshoutInfo.cCameraCode, IVS_DEV_CODE_LEN, strCameraCode.c_str(), strCameraCode.length()))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Snap shot List Get XML", "Memcpy cStart failed");
			}
			GET_ELEM_VALUE_CHAR("SnapTime",szElemValue,  sSnapshoutInfo.cSnapTime,IVS_TIME_LEN,xml);
			GET_ELEM_VALUE_NUM("SnapType", pTemp, iTemp, xml);
			sSnapshoutInfo.uiSnapType = static_cast<IVS_UINT32>(iTemp);
			iTemp = 0;
			GET_ELEM_VALUE_NUM("PictureSize", pTemp, iTemp, xml);
			sSnapshoutInfo.uiPictureSize = static_cast<IVS_UINT32>(iTemp);
			iTemp = 0;
			GET_ELEM_VALUE_CHAR("PreviewUrl",szElemValue,  sSnapshoutInfo.cPreviewUrl,IVS_URL_LEN,xml);
			GET_ELEM_VALUE_CHAR("PictureUrl",szElemValue,  sSnapshoutInfo.cPictureUrl,IVS_URL_LEN,xml);
		
			if(!CToolsHelp::Memcpy(&(pSnapshotList->stSnapshotInfo[iRealNum]), sizeof(IVS_SNAPSHOT_INFO), &sSnapshoutInfo, sizeof(IVS_SNAPSHOT_INFO)))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			iRealNum++;
			xml.OutOfElem();
		} 
		while (xml.NextElem() && iTotalNum > iRealNum);
	}
	//pSnapshotList->uiTotal = iRealNum;

    IVS_UINT32 unItemSize =(IVS_UINT32)((iRealNum-1 > 0) ? (iRealNum-1) : 0);

	IVS_UINT32 uiSize = sizeof(IVS_SNAPSHOT_INFO_LIST) + unItemSize * sizeof(IVS_SNAPSHOT_INFO);
	if (uiSize>uiBufSize)
	{
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return IVS_SUCCEED;
}

// 删除平台抓拍计划
IVS_INT32 CapturePictureXMLProcess::DelSnapshotPlanGetXml(const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode,CXml& xml)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pCameraCode);

	return IVS_SUCCEED;
}

// 拼装删除平台抓拍图片请求的xml
IVS_INT32 CapturePictureXMLProcess::DeleteSnapshotGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	// 摄像机编号去掉域编号处理
	IVS_CHAR cDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR cDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(void)CXmlProcess::ParseDevCode(pCameraCode, cDevCode, cDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(cDomaCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(cDevCode);
	(void)xml.AddElem("PictureID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiPictureID)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}
