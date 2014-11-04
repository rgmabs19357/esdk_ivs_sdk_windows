#include "OCXPtzControlXMLProcess.h"
#include "ivs_error.h"
#include "ToolsHelp.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"


// 新增预置位拼装xml
IVS_INT32 COCXPtzControlXMLProcess::AddPTZPresetGetXML(std::string& strCameraCode, IVS_UINT32 uiPresetIndex, IVS_INT32 iRet, CString& strResult)
{
	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("PresetPTZ");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Index");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiPresetIndex).c_str());
	xml.OutOfElem();
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	const char* pXml = xml.GetXMLStream(iLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;
}//lint !e1764 !e715

// 查询预置位列表
IVS_INT32 COCXPtzControlXMLProcess::GetPTZPresetListGetXML(std::string& strCameraCode, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 uiPTZPresetNum, IVS_INT32 iRet, CString& strResult)
{
	CHECK_POINTER(pPTZPresetList, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCameraCode.c_str());
	(void)xml.AddElem("PresetPTZList");
	(void)xml.IntoElem();
	if (uiPTZPresetNum == 0)
	{

		IVS_UINT32 uiLen = 0;
		const char* pXml = xml.GetXMLStream(uiLen);
		CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
		strResult = pXml;
		return IVS_SUCCEED;
	}
	for (unsigned int i=0; i < uiPTZPresetNum; i++)
	{
		IVS_PTZ_PRESET& stPTZPreset = *(pPTZPresetList + i);
		i==0?(void)xml.AddChildElem("PresetPTZ"):(void)xml.AddElem("PresetPTZ");	
		(void)xml.IntoElem();
		(void)xml.AddChildElem("Index");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stPTZPreset.uiPresetIndex).c_str());
		(void)xml.AddElem("Name");
		(void)xml.IntoElem();
		IVS_CHAR szTemp[IVS_PRESET_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szTemp, IVS_PRESET_NAME_LEN + 1, stPTZPreset.cPresetName, IVS_PRESET_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pRsp to pResultXml fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szTemp);
		xml.OutOfElem();
	}
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	const char* pXml = xml.GetXMLStream(iLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;
}//lint !e1764

// 设置看守位解析xml
IVS_INT32 COCXPtzControlXMLProcess::SetGuardPosParseXML(std::string& strGuardPosInfo, std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo)
{
	CHECK_POINTER(pGuardPosInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strGuardPosInfo.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/GuardPositionPara/CameraCode"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CameraCode", "NA");
		return iRet;
	}
	const char* pCameraCode = xml.GetElemValue();
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	strCameraCode = pCameraCode;
	
	if (!xml.FindElemEx("Content/GuardPositionPara/Enabled"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find GuardPositionEnabled", "NA");
		return iRet;
	}
	const char* pbEnableGuardPosition = xml.GetElemValue();
	CHECK_POINTER(pbEnableGuardPosition, IVS_XML_INVALID);
	pGuardPosInfo->bEnableGuardPosition = atoi(pbEnableGuardPosition);
	if (!xml.FindElemEx("Content/GuardPositionPara/WaitTime"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find WaitTime", "NA");
		return iRet;
	}
	const char* pWaitTime = xml.GetElemValue();
	CHECK_POINTER(pWaitTime, IVS_XML_INVALID);
	pGuardPosInfo->uiWaitTime = static_cast<IVS_UINT32>(atoi(pWaitTime));

	if (!xml.FindElemEx("Content/GuardPositionPara/PresetIndex"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find PresetIndex", "NA");
		return iRet;
	}
	const char* pPresetIndex = xml.GetElemValue();
	CHECK_POINTER(pPresetIndex, IVS_XML_INVALID);
	pGuardPosInfo->uiPresetIndex = static_cast<IVS_UINT32>(atoi(pPresetIndex));
	return IVS_SUCCEED;
}//lint !e1764

// 删除看守位解析xml
IVS_INT32 COCXPtzControlXMLProcess::DelGuardPosParseXML(std::string& strGuardPosInfo, std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo)
{
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strGuardPosInfo.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/GuardPositionPara/CameraCode"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CameraCode", "NA");
		return iRet;
	}
	const char* pCameraCode = xml.GetElemValue();
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	strCameraCode = pCameraCode;

	if (!xml.FindElemEx("Content/GuardPositionPara/PresetIndex"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find PresetIndex", "NA");
		return iRet;
	}
	const char* pPresetIndex = xml.GetElemValue();
	CHECK_POINTER(pPresetIndex, IVS_XML_INVALID);
	pGuardPosInfo->uiPresetIndex = static_cast<IVS_UINT32>(atoi(pPresetIndex));
	return IVS_SUCCEED;
}//lint !e1764

// 获取看守位拼装xml
IVS_INT32 COCXPtzControlXMLProcess::GetGuardPosGetXML(std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo, IVS_INT32 iRet, CString& strResult)
{
	CHECK_POINTER(pGuardPosInfo,IVS_PARA_INVALID);

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("GuardPositionPara");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCameraCode.c_str());
	(void)xml.AddElem("Enabled");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pGuardPosInfo->bEnableGuardPosition)).c_str());
	(void)xml.AddElem("WaitTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pGuardPosInfo->uiWaitTime)).c_str());
	(void)xml.AddElem("PresetIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pGuardPosInfo->uiPresetIndex)).c_str());
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	const char* pXml = xml.GetXMLStream(iLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;
}//lint !e1764 !e818

// 增加巡航轨迹解析xml
IVS_INT32 COCXPtzControlXMLProcess::AddCruiseTrackParseXML(std::string& strCuriseTrackInfo, std::string& strCameraCode, IVS_CRUISE_TRACK* pCuriseTrackInfo)
{
	CHECK_POINTER(pCuriseTrackInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strCuriseTrackInfo.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/CruiseTrack/CameraCode"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CameraCode", "NA");
		return iRet;
	}
	const char* pCameraCode = xml.GetElemValue();
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	strCameraCode = pCameraCode;

	if (!xml.FindElemEx("Content/CruiseTrack/Name"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find Name", "NA");
		return iRet;
	}

	const char* pCameraName = xml.GetElemValue();
	CHECK_POINTER(pCameraName, IVS_XML_INVALID);
	(void)CToolsHelp::Memcpy(pCuriseTrackInfo->cTrackName, IVS_CURISE_TRACK_NAME_LEN, pCameraName, strlen(pCameraName));

	if (!xml.FindElemEx("Content/CruiseTrack/CruiseType"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruiseType", "NA");
		return iRet;
	}
	const char* pCruiseType = xml.GetElemValue();
	CHECK_POINTER(pCruiseType, IVS_XML_INVALID);
	pCuriseTrackInfo->uiCruiseType = static_cast<IVS_UINT32>(atoi(pCruiseType));

	if (!xml.FindElemEx("Content/CruiseTrack/CruisePointList"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruisePointList", "NA");
		return iRet;
	}
	
	int i = 0;

	do 
	{
		if (!xml.FindElemValue("CruisePoint"))
		{
			break;
		}

		IVS_CRUISE_TRACK_POINT& stCuriseTrackPoint = pCuriseTrackInfo->stTrackPoint[i];
		(void)xml.IntoElem();
		if (xml.FindElem("PresetIndex"))
		{
			const char* pPresetIndex = xml.GetElemValue();
			if (NULL != pPresetIndex)
			{
				stCuriseTrackPoint.uiPresetIndex = static_cast<IVS_UINT32>(atoi(pPresetIndex));
			}
		}

		if (xml.FindElem("DwellTime"))
		{
			const char* pDwellTime = xml.GetElemValue();
			if (NULL != pDwellTime)
			{
				stCuriseTrackPoint.uiDwellTime = static_cast<IVS_UINT32>(atoi(pDwellTime));
			}
		}

		if (xml.FindElem("Speed"))
		{
			const char* pSpeed = xml.GetElemValue();
			if (NULL != pSpeed)
			{
				stCuriseTrackPoint.uiSpeed = static_cast<IVS_UINT32>(atoi(pSpeed));
			}
		}

		xml.OutOfElem();
		i++;
	} while (xml.NextElem() && i < IVS_MAX_CURISE_TRACK_POINT);

	pCuriseTrackInfo->uiTrackPointNum = static_cast<IVS_UINT32>(i);
	return IVS_SUCCEED;
}//lint !e1764

// 增加巡航轨迹拼装xml 
IVS_INT32 COCXPtzControlXMLProcess::AddCruiseTrackGetXML(IVS_UINT32 uiTrackIndex, IVS_INT32 iRet, CString& strResult)
{
	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("CruisePoint");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CruiseNO");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiTrackIndex).c_str());
	xml.OutOfElem();
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	const char* pXml = xml.GetXMLStream(iLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;
}

// 修改巡航轨迹解析xml
IVS_INT32 COCXPtzControlXMLProcess::ModCruiseTrackParseXML(std::string& strCuriseTrackInfo, std::string& strCameraCode, IVS_CRUISE_TRACK* pCuriseTrackInfo)
{
	CHECK_POINTER(pCuriseTrackInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = AddCruiseTrackParseXML(strCuriseTrackInfo, strCameraCode, pCuriseTrackInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "mod cruise track parse xml", " add cruise track parse xml failed")
		return iRet;
	}

	CXml xml;
	if (!xml.Parse(strCuriseTrackInfo.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/CruiseTrack/CruiseNO"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruiseNO", "NA");
		return iRet;
	}
	const char* pCruiseNO = xml.GetElemValue();
	CHECK_POINTER(pCruiseNO, IVS_XML_INVALID);
	pCuriseTrackInfo->uiCruiseNO = static_cast<IVS_UINT32>(atoi(pCruiseNO));

	return IVS_SUCCEED;
}

// 查询巡航轨迹拼装xml
IVS_INT32 COCXPtzControlXMLProcess::GetCruiseTrackGetXML(std::string& strCameraCode, IVS_CRUISE_TRACK* pCruiseTrackInfo, IVS_INT32 iRet, CString& strResult)//lint !e830
{
	CHECK_POINTER(pCruiseTrackInfo, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("CruiseTrack");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCameraCode.c_str());
	(void)xml.AddElem("CruiseNO");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pCruiseTrackInfo->uiCruiseNO).c_str());
	(void)xml.AddElem("Name");
	(void)xml.IntoElem();
	IVS_CHAR szTemp[IVS_CURISE_TRACK_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szTemp, IVS_CURISE_TRACK_NAME_LEN + 1, pCruiseTrackInfo->cTrackName, IVS_CURISE_TRACK_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy szTemp to cTrackName fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szTemp);
	(void)xml.AddElem("CruiseType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pCruiseTrackInfo->uiCruiseType).c_str());
	(void)xml.AddElem("CruisePointList");
	(void)xml.IntoElem();
	
	for (unsigned int i=0; i < pCruiseTrackInfo->uiTrackPointNum; i++)
	{
		const IVS_CRUISE_TRACK_POINT& stCruiseTrackPoint = pCruiseTrackInfo->stTrackPoint[i];
		i==0?(void)xml.AddChildElem("CruisePoint"):(void)xml.AddElem("CruisePoint");	
		(void)xml.IntoElem();
		(void)xml.AddChildElem("PresetIndex");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiPresetIndex).c_str());
		(void)xml.AddElem("DwellTime");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiDwellTime).c_str());
		(void)xml.AddElem("Speed");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiSpeed).c_str());
		xml.OutOfElem();
	}
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	const char* pXml = xml.GetXMLStream(iLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;
}//lint !e1764 !e826 !e818

// 查询巡航轨迹信息列表拼装xml
IVS_INT32 COCXPtzControlXMLProcess::GetCruiseTrackListGetXML(std::string&strCameraCode, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 uiCruiseTrackNum, IVS_INT32 iRet, CString& strResult)
{
	CHECK_POINTER(pCruiseTrackList, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCameraCode.c_str());
	(void)xml.AddElem("CruiseTrackList");
	(void)xml.IntoElem();
	
	for (unsigned int i=0; i < uiCruiseTrackNum; i++)
	{
	 	IVS_CRUISE_TRACK& stCruiseTrack = *(pCruiseTrackList + i);
	 	i==0?(void)xml.AddChildElem("CruiseTrack"):(void)xml.AddElem("CruiseTrack");	
	 	(void)xml.IntoElem();
	 	(void)xml.AddChildElem("CruiseNO");
	 	(void)xml.IntoElem();
	 	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrack.uiCruiseNO).c_str());
	 	(void)xml.AddElem("Name");
	 	(void)xml.IntoElem();
	 	IVS_CHAR szTemp[IVS_CURISE_TRACK_NAME_LEN + 1] = {0};
	 	if (!CToolsHelp::Memcpy(szTemp, IVS_CURISE_TRACK_NAME_LEN + 1, stCruiseTrack.cTrackName, IVS_CURISE_TRACK_NAME_LEN))
	 	{
	 		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pRsp to pResultXml fail", "NA");
	 		return IVS_ALLOC_MEMORY_ERROR;
	 	}
	 	(void)xml.SetElemValue(szTemp);
		(void)xml.AddElem("CruiseType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrack.uiCruiseType).c_str());

	 	xml.OutOfElem();
	}
	xml.OutOfElem();
	 
	IVS_UINT32 iLen = 0;
	//把xml生成string
	const char* pXml = xml.GetXMLStream(iLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;
}//lint !e1764
// 设置巡航计划
IVS_INT32 COCXPtzControlXMLProcess::SetCruisePlanParseXML(std::string& strCruisePlan, std::string& strCameraCode, IVS_CRUISE_PLAN* pCruisePlan)
{
	CHECK_POINTER(pCruisePlan, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strCruisePlan.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/CruisePlanInfo/CameraCode"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CameraCode", "NA");
		return iRet;
	}

    const char* pCameraCodeTemp = xml.GetElemValue();
    if (NULL != pCameraCodeTemp)
    {
        strCameraCode = pCameraCodeTemp;
    }

	

	if (!xml.FindElemEx("Content/CruisePlanInfo/IsEnable"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find Name", "NA");
		return iRet;
	}
	const char* pEnable = xml.GetElemValue();
	CHECK_POINTER(pEnable, IVS_XML_INVALID);
	pCruisePlan->bEnable = static_cast<IVS_UINT32>(atoi(pEnable));//lint !e713
	
	if (!xml.FindElemEx("Content/CruisePlanInfo/PlanType"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruiseType", "NA");
		return iRet;
	}
	const char* pPlanType = xml.GetElemValue();
	CHECK_POINTER(pPlanType, IVS_XML_INVALID);
	pCruisePlan->uiPlanType = static_cast<IVS_UINT32>(atoi(pPlanType));

	if (!xml.FindElemEx("Content/CruisePlanInfo/PlanList"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruisePointList", "NA");
		return iRet;
	}

	int i = 0;
	do 
	{
		if (!xml.FindElemValue("PlanInfo"))
		{
			break;
		}

		IVS_CRUISE_PLAN_INFO& stCruisePlanInfo = pCruisePlan->stPlanInfo[i];
		(void)xml.IntoElem();
		if (xml.FindElem("DayType"))
		{
			const char* pDayType = xml.GetElemValue();
			if (NULL != pDayType)
			{
				stCruisePlanInfo.uiDayType = static_cast<IVS_UINT32>(atoi(pDayType));
			}
		}

		if (xml.FindElem("TimeInfoNum"))
		{
			const char* pTimeSpanInfoNum = xml.GetElemValue();
			if (NULL != pTimeSpanInfoNum)
			{
				stCruisePlanInfo.uiTimeSpanInfoNum = static_cast<IVS_UINT32>(atoi(pTimeSpanInfoNum));
			}
		}

		if (!xml.FindElem("TimeList"))
		{
			iRet = IVS_XML_INVALID;
			BP_RUN_LOG_ERR(iRet, "can not find TimeList", "NA");
			return iRet;
		}
		(void)xml.IntoElem();
		int j = 0;
		do 
		{
			if (!xml.FindElemValue("TimeInfo"))
			{
				iRet = IVS_XML_INVALID;
				BP_RUN_LOG_ERR(iRet, "can not find TimeInfo", "NA");
				return iRet;
			}
			(void)xml.IntoElem();
			IVS_CRUISE_TIME_SPAN& stCuriseTimeSpanInfo = stCruisePlanInfo.stTimeSpanInfo[j];
			if (xml.FindElem("StartTime"))
			{

                const char* pStartTimeTemp = xml.GetElemValue();
                if (NULL != pStartTimeTemp)
                {
                    (void)CToolsHelp::Memcpy(stCuriseTimeSpanInfo.cStart, IVS_TIME_LEN, pStartTimeTemp, strlen(pStartTimeTemp));
                }
				
			}
			//(void)xml.IntoElem();
			if (xml.FindElem("EndTime"))
			{
                const char* pEndTimeTemp = xml.GetElemValue();
                if (NULL != pEndTimeTemp)
                {
                    (void)CToolsHelp::Memcpy(stCuriseTimeSpanInfo.cEnd, IVS_TIME_LEN, pEndTimeTemp, strlen(pEndTimeTemp));
                }
				
			}
			//(void)xml.IntoElem();
			if (!xml.FindElem("TrackInfo"))
			{
				iRet = IVS_XML_INVALID;
				BP_RUN_LOG_ERR(iRet, "can not find TrackInfo", "NA");
				return iRet;
			}
			(void)xml.IntoElem();
			if (xml.FindElem("TrackType"))
			{
				const char* pCruiseType = xml.GetElemValue();
				if (NULL != pCruiseType)
				{
					stCuriseTimeSpanInfo.uiCruiseType = static_cast<IVS_UINT32>(atoi(pCruiseType));
				}
			}
			if (xml.FindElem("TrackIndex"))
			{
				const char* pCruiseNO = xml.GetElemValue();
				if (NULL != pCruiseNO)
				{
					stCuriseTimeSpanInfo.uiCruiseNO = static_cast<IVS_UINT32>(atoi(pCruiseNO));
				}
			}
			xml.OutOfElem();
			xml.OutOfElem();
			j++;
		} while (xml.NextElem() && j < IVS_MAX_TIMESPAN_NUM);
		stCruisePlanInfo.uiTimeSpanInfoNum = static_cast<IVS_UINT32>(j);

		xml.OutOfElem();
		xml.OutOfElem();
		i++;
	} while (xml.NextElem() && i < IVS_MAX_PLAN_INFO_NUM);
	pCruisePlan->uiPlanInfoNum = static_cast<IVS_UINT32>(i);

	return IVS_SUCCEED;
}//lint !e1764

// 获取巡航计划拼装xml
IVS_INT32 COCXPtzControlXMLProcess::GetCruisePlanGetXML(std::string& strCameraCode, IVS_CRUISE_PLAN* pCruisePlan, IVS_INT32 iRet, CString& strResult)
{
	CHECK_POINTER(pCruisePlan, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("CruisePlanInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCameraCode.c_str());
	(void)xml.AddElem("IsEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pCruisePlan->bEnable).c_str());
	(void)xml.AddElem("PlanType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pCruisePlan->uiPlanType).c_str());
	(void)xml.AddElem("PlanList");
	(void)xml.IntoElem();
	
	for (unsigned int i=0; i < pCruisePlan->uiPlanInfoNum; i++)
	{
		IVS_CRUISE_PLAN_INFO& stCruisePlanInfo = pCruisePlan->stPlanInfo[i];
		i==0?(void)xml.AddChildElem("PlanInfo"):(void)xml.AddElem("PlanInfo");	
		(void)xml.IntoElem();
		(void)xml.AddChildElem("DayType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruisePlanInfo.uiDayType).c_str());
		(void)xml.AddElem("TimeInfoNum");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruisePlanInfo.uiTimeSpanInfoNum).c_str());
		(void)xml.AddElem("TimeList");
		(void)xml.IntoElem();
		for (unsigned int j = 0; j < stCruisePlanInfo.uiTimeSpanInfoNum; j++)
		{
			IVS_CRUISE_TIME_SPAN& stCruiseTimeInfo = stCruisePlanInfo.stTimeSpanInfo[j];
			j==0?(void)xml.AddChildElem("TimeInfo"):(void)xml.AddElem("TimeInfo");	
			(void)xml.IntoElem();
			(void)xml.AddChildElem("StartTime");
			(void)xml.IntoElem();
			IVS_CHAR szTemp[IVS_TIME_LEN + 1] = {0};
			(void)CToolsHelp::Memcpy(szTemp, IVS_TIME_LEN + 1, stCruiseTimeInfo.cStart, IVS_TIME_LEN);
			(void)xml.SetElemValue(szTemp);
			(void)xml.AddElem("EndTime");
			(void)xml.IntoElem();
			(void)CToolsHelp::Memcpy(szTemp, IVS_TIME_LEN + 1, stCruiseTimeInfo.cEnd, IVS_TIME_LEN);
			(void)xml.SetElemValue(szTemp);
			(void)xml.AddElem("TrackInfo");
			(void)xml.IntoElem();
			(void)xml.AddChildElem("TrackType");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTimeInfo.uiCruiseType).c_str());
			(void)xml.AddElem("TrackIndex");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTimeInfo.uiCruiseNO).c_str());
			xml.OutOfElem();
			xml.OutOfElem();
		}
		xml.OutOfElem();
		xml.OutOfElem();
	}
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	const char* pXml = xml.GetXMLStream(iLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;
}//lint !e1764


// PTZ控制拼装xml
IVS_INT32 COCXPtzControlXMLProcess::GetPTZControlRspXML(IVS_PTZ_CONTROL_INFO& stPtzControlInfo, IVS_INT32 iRet, CString& strResult)
{

    CXml xml;
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
    (void)xml.AddElem("LockStatus");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(stPtzControlInfo.uiLockStatus)).c_str());
    //if (1 == stPtzControlInfo.uiLockStatus) // 锁定
    //{
    (void)xml.AddElem("LockUserInfo");
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(stPtzControlInfo.uiUserID)).c_str());
 
    (void)xml.AddElem("UserName");
    IVS_CHAR szUserName[IVS_NAME_LEN + 1] = {0};
    (void)CToolsHelp::Memcpy(szUserName, IVS_NAME_LEN + 1, stPtzControlInfo.cUserName, IVS_NAME_LEN);
    (void)xml.SetElemValue(szUserName);

    (void)xml.AddElem("LoginIP");
    IVS_CHAR szLoginIP[IVS_IP_LEN + 1] = {0};
    (void)CToolsHelp::Memcpy(szLoginIP, IVS_IP_LEN + 1, stPtzControlInfo.stIP.cIP, IVS_IP_LEN);
    (void)xml.SetElemValue(szLoginIP);

    (void)xml.AddElem("ReleaseTimeRemain");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(stPtzControlInfo.uiReleaseTimeRemain)).c_str());

    xml.OutOfElem();
   // }

    IVS_UINT32 iLen = 0;
    const char* pXml = xml.GetXMLStream(iLen);
    CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
    strResult = pXml;
    return IVS_SUCCEED;
}