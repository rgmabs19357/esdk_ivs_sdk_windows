#include "OCXAlarmMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "IVSCommon.h"
#include "IVS_Trace.h"





COCXAlarmMgrXMLProcess::COCXAlarmMgrXMLProcess()
{
}

COCXAlarmMgrXMLProcess::~COCXAlarmMgrXMLProcess()
{
}
//获取水印告警确认XML
IVS_INT32 COCXAlarmMgrXMLProcess::GetWatermarkAlarmConfirmInfo(const IVS_CHAR * pConfirmInfo,IVS_WATERMARK_ALARM_CONFIRM* pstConfirmInfo)
{
	CHECK_POINTER(pConfirmInfo,IVS_PARA_INVALID);
	CHECK_POINTER(pstConfirmInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	//IVS_INT32 iRet = IVS_FAIL;
	CXml xmlRsp;
	if (!xmlRsp.Parse(pConfirmInfo))
	{
		return IVS_FAIL;
	}
	if (!xmlRsp.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse XML to List",
			"no 'Content' elemenet");
		return IVS_FAIL;
	}

	const IVS_CHAR *pSzElement = NULL;
	//设备编码
	GET_ELEM_VALUE_CHAR("CameraCode", pSzElement, pstConfirmInfo->cCameraCode, IVS_DEV_CODE_LEN, xmlRsp);

	GET_ELEM_VALUE_NUM("AlarmEventID", pSzElement, pstConfirmInfo->ullAlarmEventID, xmlRsp);//lint !e732

	GET_ELEM_VALUE_CHAR("ConfirmInfo", pSzElement, pstConfirmInfo->cConfirmInfo, IVS_ALARM_DESCRIPTION_LEN, xmlRsp);

	GET_ELEM_VALUE_CHAR("ConfirmTime", pSzElement, pstConfirmInfo->cConfirmTime, IVS_TIME_LEN, xmlRsp);

	return IVS_SUCCEED;
}


//获取水印告警撤销XML
IVS_INT32 COCXAlarmMgrXMLProcess::GetWatermarkAlarmCancelInfo(const IVS_CHAR * pCancelInfo,IVS_WATERMARK_ALARM_CANCEL* pstCancelInfo)
{
	CHECK_POINTER(pCancelInfo,IVS_PARA_INVALID);
	CHECK_POINTER(pstCancelInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	//IVS_INT32 iRet = IVS_FAIL;
	CXml xmlRsp;
	if (!xmlRsp.Parse(pCancelInfo))
	{
		return IVS_FAIL;
	}
	if (!xmlRsp.FindElemEx("Content"))//lint e!830
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse XML to List",
			"no 'Content' elemenet");
		return IVS_FAIL;
	}

	const IVS_CHAR *pSzElement = NULL;
	//设备编码
	GET_ELEM_VALUE_CHAR("CameraCode", pSzElement, pstCancelInfo->cCameraCode, IVS_DEV_CODE_LEN, xmlRsp);

	GET_ELEM_VALUE_NUM("AlarmEventID", pSzElement, pstCancelInfo->ullAlarmEventID, xmlRsp);//lint !e732

	GET_ELEM_VALUE_CHAR("CancelInfo", pSzElement, pstCancelInfo->cCancelInfo, IVS_ALARM_DESCRIPTION_LEN, xmlRsp);

	GET_ELEM_VALUE_CHAR("CancelTime", pSzElement, pstCancelInfo->cCancelTime, IVS_TIME_LEN, xmlRsp);

	return IVS_SUCCEED;
}
//查询水印告警列表请求XML

IVS_INT32 COCXAlarmMgrXMLProcess::GetWatermarkAlarmListGetXML(IVS_WATERMARK_ALARM_INFO_LIST *pWatermarkAlarmList,IVS_INT32 iRetCord,CXml &xml)
{
	CHECK_POINTER(pWatermarkAlarmList,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCord).c_str());

	(void)xml.AddElem("PageInfo");
	(void)xml.AddChildElem("RealNum");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pWatermarkAlarmList->uiTotal)).c_str());
	(void)xml.AddElem("FromIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pWatermarkAlarmList->stIndexRange.uiFromIndex)).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pWatermarkAlarmList->stIndexRange.uiToIndex)).c_str());
	xml.OutOfElem();
	if (pWatermarkAlarmList->uiTotal > 0)
	{
		(void)xml.AddElem("WatermarkAlarmList");
		int iRealNum = static_cast<int>(pWatermarkAlarmList->uiTotal);//lint !e713

		int iTotal =(int)(pWatermarkAlarmList->stIndexRange.uiToIndex - pWatermarkAlarmList->stIndexRange.uiFromIndex) + 1;
		if (iTotal < iRealNum)
		{
			iRealNum = iTotal;
		}
		for (int iIndex = 0;iIndex < iRealNum;iIndex++)
		{
			IVS_WATERMARK_ALARM_INFO &stWaterAlarmInfo = pWatermarkAlarmList->stWatermarkAlarmInfo[iIndex];
			iIndex == 0 ? (void)xml.AddChildElem("WatermarkAlarmInfo"):(void)xml.AddElem("WatermarkAlarmInfo");
			(void)(xml.IntoElem());

			(void)xml.AddChildElem("AlarmEventID");
			(void)(xml.IntoElem());
			(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stWaterAlarmInfo.ullAlarmEventID).c_str()); //lint !e712 !e747

			(void)xml.AddElem("CheckoutOperator");
			(void)xml.SetElemValue(stWaterAlarmInfo.cCheckoutOperator);

			(void)xml.AddElem("AlarmType");
			(void)xml.SetElemValue(stWaterAlarmInfo.cAlarmType);

			(void)xml.AddElem("CameraCode");
			(void)xml.SetElemValue(stWaterAlarmInfo.cCameraCode);

			(void)xml.AddElem("OccurStartTime");
			(void)xml.SetElemValue(stWaterAlarmInfo.cOccurStartTime);

			(void)xml.AddElem("OccurStopTime");
			(void)xml.SetElemValue(stWaterAlarmInfo.cOccurStopTime);

			(void)xml.AddElem("WatermarkAlarmStatus");
			(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stWaterAlarmInfo.uiWatermarkAlarmStatus).c_str());

			(void)xml.AddElem("ConfirmOperator");
			(void)xml.SetElemValue(stWaterAlarmInfo.cConfirmOperator);

			(void)xml.AddElem("ConfirmTime");
			(void)xml.SetElemValue(stWaterAlarmInfo.cConfirmTime);

			(void)xml.AddElem("ConfirmInfo");
			(void)xml.SetElemValue(stWaterAlarmInfo.cConfirmInfo);
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();
	return IVS_SUCCEED;
}

//获取水印告警提示
IVS_INT32 COCXAlarmMgrXMLProcess::GetWaterTIPReportInfoXML(const IVS_ALARM_WATERMARK_INFO * pWaterAlarmInfo, CXml &xml)
{
	CHECK_POINTER(pWaterAlarmInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pWaterAlarmInfo->cCameraCode);

	(void)xml.AddElem("StartTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pWaterAlarmInfo->cStartTime);

	xml.OutOfElem();
	return IVS_SUCCEED;
}

//获取水印篡改告警
IVS_INT32 COCXAlarmMgrXMLProcess::GetWaterAlarmReportInfoXML(const IVS_ALARM_WATERMARK_INFO * pWaterAlarmInfo, CXml &xml)
{
	CHECK_POINTER(pWaterAlarmInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pWaterAlarmInfo->cCameraCode);

	(void)xml.AddElem("AlarmEventID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pWaterAlarmInfo->ullAlarmEventID).c_str());//lint !e712 !e747

	(void)xml.AddElem("StartTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pWaterAlarmInfo->cStartTime);
	
	(void)xml.AddElem("StopTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pWaterAlarmInfo->cEndTime);

	xml.OutOfElem();
	return IVS_SUCCEED;
}


// 告警级别通用解析XML
IVS_INT32 COCXAlarmMgrXMLProcess::AlarmLevelPraseXML(IVS_ALARM_LEVEL *pAlarmLevel, CXml &xml)
{	
	CHECK_POINTER(pAlarmLevel,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "fail is no 'Content' elemenet", "NA");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	if (!xml.FindElem("AlarmLevel"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "fail is no 'AlarmLevel' elemenet", "NA");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelID", pTemp, pAlarmLevel->uiAlarmLevelId, xml);
	GET_ELEM_VALUE_CHAR("AlarmLevelName", pTemp, pAlarmLevel->cName,IVS_NAME_LEN, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelValue", pTemp, pAlarmLevel->uiValue, xml);
	GET_ELEM_VALUE_CHAR("AlarmLevelColor",pTemp, pAlarmLevel->cColor,IVS_ALARM_LEVEL_COLOR_LEN,xml);
	GET_ELEM_VALUE_CHAR("AlarmLevelDesc", pTemp, pAlarmLevel->cDescription,IVS_ALARM_DESCRIPTION_LEN, xml);

	xml.OutOfElem();

	xml.OutOfElem();

	return IVS_SUCCEED;

}

// 告警帮助通用解析XML
IVS_INT32 COCXAlarmMgrXMLProcess::HelpInfoPraseXML(IVS_HELP_INFO *helpInfo, CXml &xml, IVS_CHAR* pDomainCode)
{
	CHECK_POINTER(helpInfo,IVS_PARA_INVALID);
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "fail is no 'Content' elemenet", "NA");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	if (!xml.FindElem("DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}
	const char* pD = xml.GetElemValue();
	if (NULL != pD)
	{
		(void)CToolsHelp::Memcpy(pDomainCode, IVS_DOMAIN_CODE_LEN, pD, strlen(pD));
	}
	
	if (!xml.FindElem("HelpInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"HelpInfo\") faill ", "NA");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("SourceID", pTemp, helpInfo->uiSourceId, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("SourceType", pTemp, helpInfo->uiSourceType, xml);
	GET_ELEM_VALUE_CHAR("HelpInfo", pTemp, helpInfo->cHelpInfo,IVS_MAX_HELP_LEN, xml);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}//lint !e550

// 查询告警帮助拼装XML
IVS_INT32 COCXAlarmMgrXMLProcess::QueryHelpInfoGetXML(CString &cstrRet,const IVS_HELP_INFO *pHelpInfo, IVS_INT32 iRet, CXml &xml)
{
	CHECK_POINTER(pHelpInfo,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR pointer[POINTER_MAX_NUM] = {0};

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("HelpInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("SourceType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pHelpInfo->uiSourceType)).c_str());

	(void)xml.AddElem("SourceID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pHelpInfo->uiSourceId)).c_str());

	(void)xml.AddElem("SourceName");
	(void)xml.IntoElem();
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pHelpInfo->cSourceName, IVS_ALARM_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pHelpInfo->cSourceName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("HelpInfo");
	(void)xml.IntoElem();
	IVS_CHAR* pszHelpInfo = IVS_NEW(pszHelpInfo, IVS_MAX_HELP_LEN + 1);
	if (!CToolsHelp::Memcpy(pszHelpInfo, IVS_MAX_HELP_LEN, pHelpInfo->cHelpInfo, IVS_MAX_HELP_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pHelpInfo->cHelpInfo failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pszHelpInfo);
	IVS_DELETE(pszHelpInfo, MUILI);
	(void)xml.AddElem("UserName");
	(void)xml.IntoElem();
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pHelpInfo->cUserName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy  pHelpInfo->cUserName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("UpdateTime");
	(void)xml.IntoElem();
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pHelpInfo->cUpdateTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pHelpInfo->cUpdateTime failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	xml.OutOfElem();
	(void)xml.AddElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	cstrRet = xml.GetXMLStream(iLen);
	return IVS_SUCCEED;
}

IVS_INT32 COCXAlarmMgrXMLProcess::QueryHelpInfoTextGetXML(CString &cstrRet, const char *pHelpInfoText, IVS_INT32 iRet, CXml &xml)
{
    CHECK_POINTER(pHelpInfoText,IVS_PARA_INVALID);
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
    (void)xml.AddElem("HelpInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("HelpInfo");
    (void)xml.IntoElem();
    xml.SetElemValue(pHelpInfoText);

    unsigned int uiLen = 0;
    const char* pResult = xml.GetXMLStream(uiLen);
    CHECK_POINTER(pResult, IVS_FAIL)
    cstrRet = pResult;
    return IVS_SUCCEED;

}

// 查询告警级别列表拼接XML
IVS_INT32 COCXAlarmMgrXMLProcess::GetAlarmLevelListGetXML(CString &cstrRet, const IVS_UINT32* pNum ,const IVS_ALARM_LEVEL *pAlarmLevel,IVS_INT32 iRet,CXml &xml) 
{
	CHECK_POINTER(pAlarmLevel,IVS_PARA_INVALID);
	CHECK_POINTER(pNum,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR pointer[POINTER_MAX_NUM] = {0};

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmLevelList");
	(void)xml.IntoElem();
	IVS_UINT32 i = 0;
	do 
	{
		i == 0?(void)xml.AddChildElem("AlarmLevel"):(void)xml.AddElem("AlarmLevel");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("AlarmLevelID");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pAlarmLevel[i].uiAlarmLevelId)).c_str());

		(void)xml.AddElem("AlarmLevelName");
		(void)xml.IntoElem();
		if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmLevel[i].cName, IVS_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel[i].cName failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(pointer);
		memset(pointer, 0, POINTER_MAX_NUM);

		(void)xml.AddElem("AlarmLevelValue");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pAlarmLevel[i].uiValue)).c_str());

		(void)xml.AddElem("AlarmLevelColor");
		(void)xml.IntoElem();
		if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmLevel[i].cColor, ALARM_COLOR_NUM))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel[i].cColor failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(pointer);
		memset(pointer, 0, POINTER_MAX_NUM);

		IVS_CHAR szDescription[IVS_ALARM_DESCRIPTION_LEN + 1];
		memset(szDescription, 0, IVS_ALARM_DESCRIPTION_LEN + 1);
		(void)xml.AddElem("AlarmLevelDesc");
		(void)xml.IntoElem();
		if (!CToolsHelp::Memcpy(szDescription, IVS_ALARM_DESCRIPTION_LEN, pAlarmLevel[i].cDescription, IVS_ALARM_DESCRIPTION_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel[i].cDescription failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szDescription);

		(void)xml.AddElem("IsUserDefined");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pAlarmLevel[i].bIsUserDefined).c_str());
		i++;
		xml.OutOfElem();
	} while (i < *pNum);

	xml.OutOfElem();
	(void)xml.AddElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	cstrRet = xml.GetXMLStream(iLen);
	return IVS_SUCCEED;
}

// 查询告警级别信息+错误码
IVS_INT32 COCXAlarmMgrXMLProcess::GetAlarmLevel(CString &cstrRet,const IVS_ALARM_LEVEL *pAlarmLevel,IVS_INT32 iRet,CXml &xml)
{
	CHECK_POINTER(pAlarmLevel,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	char pointer[POINTER_MAX_NUM];
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("AlarmLevel");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmLevelID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmLevel->uiAlarmLevelId)).c_str());

	(void)xml.AddElem("AlarmLevelName");
	(void)xml.IntoElem();
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmLevel->cName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel->cName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("AlarmLevelValue");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmLevel->uiValue)).c_str());

	(void)xml.AddElem("AlarmLevelColor");
	(void)xml.IntoElem();
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmLevel->cColor, ALARM_COLOR_NUM))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel->cColor failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("AlarmLevelDesc");
	(void)xml.IntoElem();
	char szDescription[IVS_ALARM_DESCRIPTION_LEN+1];
	memset(szDescription, 0, IVS_ALARM_DESCRIPTION_LEN+1);
	if (!CToolsHelp::Memcpy(szDescription, IVS_ALARM_DESCRIPTION_LEN, pAlarmLevel->cDescription, IVS_ALARM_DESCRIPTION_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel->cDescription failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szDescription);

	(void)xml.AddElem("IsUserDefined");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pAlarmLevel->bIsUserDefined).c_str());

	xml.OutOfElem();
	(void)xml.AddElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	cstrRet = xml.GetXMLStream(iLen);

	return IVS_SUCCEED;
}

// 告警类型列表拼接XML
IVS_INT32 COCXAlarmMgrXMLProcess::GetAlarmTypeListGetXml(IVS_ALARM_TYPE_LIST* pAlarmTypeList,CXml& xmlRes, unsigned int bufSize)
{
	CHECK_POINTER(pAlarmTypeList,IVS_FAIL);
	IVS_DEBUG_TRACE("");

	(void)xmlRes.AddDeclaration("1.0","UTF-8","");
	(void)xmlRes.AddElem("Content");
	(void)xmlRes.IntoElem();
	(void)xmlRes.AddChildElem("PageInfo");
	(void)xmlRes.IntoElem();
	(void)xmlRes.AddChildElem("RealNum");
	(void)xmlRes.IntoElem();
	(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmTypeList->iTotal)).c_str());
	(void)xmlRes.AddElem("FromIndex");
	(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmTypeList->stIndexRange.uiFromIndex)).c_str());
	(void)xmlRes.AddElem("ToIndex");
	(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmTypeList->stIndexRange.uiToIndex)).c_str());
	xmlRes.OutOfElem();
	(void)xmlRes.AddElem("AlarmTypeList");
	unsigned int iCount = (pAlarmTypeList->stIndexRange.uiToIndex - pAlarmTypeList->stIndexRange.uiFromIndex) + 1;
	int iNeedSize  = static_cast<IVS_INT32>(sizeof(IVS_ALARM_TYPE_LIST) + (iCount -1)*sizeof(IVS_ALARM_TYPE));
	if (iNeedSize > (int)bufSize)
	{
		BP_RUN_LOG_INF("count error,memory leak","NA");
		(void)xmlRes.AddElem("ResultCode");
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(IVS_PARA_INVALID).c_str());
		xmlRes.OutOfElem();
		return IVS_SUCCEED;
	}
	if (pAlarmTypeList->iTotal == 0)
	{
		iCount = 0;
	}
	if (pAlarmTypeList->iTotal < pAlarmTypeList->stIndexRange.uiToIndex)
	{
		iCount = (pAlarmTypeList->iTotal - pAlarmTypeList->stIndexRange.uiFromIndex) + 1;
	}
	char pointer[POINTER_MAX_NUM] = {0};
	memset(pointer, 0, POINTER_MAX_NUM);
	for (unsigned int i = 0; i < iCount; i++)
	{
		IVS_ALARM_TYPE &iResInfo = pAlarmTypeList->stSubscribeInfo[i];
		i == 0 ? xmlRes.AddChildElem("AlarmTypeInfo") : xmlRes.AddElem("AlarmTypeInfo");
		(void)xmlRes.IntoElem();
		(void)xmlRes.AddChildElem("AlarmTypeID");
		(void)xmlRes.IntoElem();
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iResInfo.uiID)).c_str());
		(void)xmlRes.AddElem("AlarmType");
		if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, iResInfo.cType, IVS_ALARM_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy iResInfo.cType failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.SetElemValue(pointer);
		memset(pointer, 0, POINTER_MAX_NUM);
		(void)xmlRes.AddElem("AlarmTypeName");
		if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, iResInfo.cName, IVS_ALARM_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy iResInfo.cName failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.SetElemValue(pointer);
		memset(pointer, 0, POINTER_MAX_NUM);
		(void)xmlRes.AddElem("AlarmInType");
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iResInfo.uiType)).c_str());
		(void)xmlRes.AddElem("AlarmCategory");
		if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, iResInfo.cCategory, IVS_ALARM_TYPE_CATEGORY_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy iResInfo.cCategory failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.SetElemValue(pointer);
		memset(pointer, 0, POINTER_MAX_NUM);
		(void)xmlRes.AddElem("AlarmLevelID");
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iResInfo.stAlarmLevel.uiAlarmLevelId)).c_str());
		(void)xmlRes.AddElem("AlarmLevelValue");
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iResInfo.stAlarmLevel.uiValue)).c_str());
		(void)xmlRes.AddElem("AlarmLevelName");
		IVS_CHAR cAlarmLevelName[IVS_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cAlarmLevelName, IVS_NAME_LEN, iResInfo.stAlarmLevel.cName, IVS_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy iResInfo.stAlarmLevel.cName failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.SetElemValue(cAlarmLevelName);
		memset(pointer, 0, POINTER_MAX_NUM);
		(void)xmlRes.AddElem("IsUserDefined");
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(iResInfo.stAlarmLevel.bIsUserDefined).c_str());
		xmlRes.OutOfElem();
	}
	if (iCount != 0)
	{
		xmlRes.OutOfElem();
	}

	(void)xmlRes.AddElem("ResultCode");
	(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(IVS_SUCCEED).c_str());
	xmlRes.OutOfElem();
	return IVS_SUCCEED;
}

// 新增告警级别拼接错误码XML
IVS_VOID COCXAlarmMgrXMLProcess::GetAddAlarmErrString( CString &strResult,IVS_UINT32 iNum, IVS_INT32 iRet)
{
	IVS_DEBUG_TRACE("");
	CXml xmlRsp;

	(void)xmlRsp.AddDeclaration("1.0","UTF-8","");

	(void)xmlRsp.AddElem("Content");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.AddChildElem("AlarmLevelID");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iNum)).c_str());

	(void)xmlRsp.AddElem("ResultCode");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	xmlRsp.OutOfElem();
	IVS_UINT32 iLen = 0;
	strResult = xmlRsp.GetXMLStream(iLen);

}

// 
//设置告警类型级别解析XML
IVS_INT32 COCXAlarmMgrXMLProcess::SetAlarmTypeLevelGetXml(CXml &xml, IVS_ALARM_TYPE_LIST* pAlarmTypeList)
{
	CHECK_POINTER(pAlarmTypeList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/AlarmTypeList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/AlarmTypeList) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElem("AlarmTypeInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"AlarmTypeInfo\") fail", "NA");
		return IVS_XML_INVALID;
	}

	int iSize = 0;
	const char* szElemValue = NULL;
	do{
		(void)xml.IntoElem();
		IVS_ALARM_TYPE &stAlarmType = pAlarmTypeList->stSubscribeInfo[iSize];
		GET_ELEM_VALUE_NUM("AlarmTypeID", szElemValue, stAlarmType.uiID, xml);//lint !e732
		GET_ELEM_VALUE_NUM("AlarmLevelID", szElemValue, stAlarmType.stAlarmLevel.uiAlarmLevelId, xml);//lint !e732
		xml.OutOfElem();
		iSize++;
	}while (xml.NextElem());
	return IVS_SUCCEED;
}

// 告警上报拼接XML
IVS_INT32 COCXAlarmMgrXMLProcess::AlarmReportGetXML(const IVS_ALARM_NOTIFY* pAlarmNotify, CXml& xml)
{
	CHECK_POINTER(pAlarmNotify,IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	char pointer[POINTER_MAX_NUM] = {0};
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("AlarmNotify");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmEventID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmNotify->ullAlarmEventID)).c_str());

	(void)xml.AddElem("AlarmInCode");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cAlarmInCode, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy strDomainCode.c_str() to pAlarmNotifyTmp->cDevDomainCode fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("DevDomainCode");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cDevDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy strDomainCode.c_str() to pAlarmNotifyTmp->cDevDomainCode fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("AlarmInType");
	std::string strAlarmInType;
	if (ALARM_IN_CAMERA == pAlarmNotify->uiAlarmInType)
	{
		strAlarmInType = "01";
	}
	else if (ALARM_IN_DI == pAlarmNotify->uiAlarmInType)
	{
		strAlarmInType = "02";
	}
	else
	{
		strAlarmInType = "09";
	}
	(void)xml.SetElemValue(strAlarmInType.c_str());
	
	(void)xml.AddElem("AlarmInName");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cAlarmInName, IVS_ALARM_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cAlarmInName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	
	(void)xml.AddElem("AlarmLevelValue");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmNotify->uiAlarmLevelValue)).c_str());
	
	(void)xml.AddElem("AlarmLevelName");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cAlarmLevelName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cAlarmLevelName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("AlarmLevelColor");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cAlarmLevelColor failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("AlarmType");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cAlarmType, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cAlarmType failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("AlarmTypeName");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cAlarmTypeName, IVS_ALARM_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cAlarmTypeName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("AlarmCategory");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cAlarmCategory, IVS_ALARM_CATEGORY_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cAlarmCategory failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("OccurTime");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cOccurTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cOccurTime failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddElem("OccurNumber");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmNotify->uiOccurNumber)).c_str());

	(void)xml.AddElem("AlarmStatus");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmNotify->uiAlarmStatus)).c_str());
	
	(void)xml.AddElem("IsCommission");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pAlarmNotify->bIsCommission).c_str());

	(void)xml.AddElem("PreviewUrl");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmNotify->cPreviewUrl, IVS_URL_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cPreviewUrl failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("ExistsRecord");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pAlarmNotify->bExistsRecord).c_str());

    (void)xml.AddElem("NVRCode");
	IVS_CHAR szNvrCode[IVS_NVR_CODE_LEN +1] = {0};
	if (!CToolsHelp::Memcpy(szNvrCode, IVS_NVR_CODE_LEN, pAlarmNotify->cNvrCode, IVS_NVR_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cNvrCode to szNvrCode fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szNvrCode);
	
	(void)xml.AddElem("AlarmDesc");
	IVS_CHAR cAlarmDesc[IVS_ALARM_DESCRIPTION_LEN + 1];
	memset(cAlarmDesc, 0, IVS_ALARM_DESCRIPTION_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, pAlarmNotify->cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cAlarmDesc failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmDesc);

	(void)xml.AddElem("ExtParam");
	IVS_CHAR cExParam[IVS_ALARM_EX_PARAM_LEN + 1];
	memset(cExParam, 0, IVS_ALARM_EX_PARAM_LEN + 1);
	if (!CToolsHelp::Memcpy(cExParam, IVS_ALARM_EX_PARAM_LEN, pAlarmNotify->cExtParam, IVS_ALARM_EX_PARAM_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmNotify->cExtParam failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValueCDATA(cExParam);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}


// 通用获取返回码
IVS_INT32 COCXAlarmMgrXMLProcess::GeneralGetString(CString &strResult, const IVS_CHAR* pRspXml, IVS_INT32 iRet)
{
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xml;

	if (!xml.Parse(pRspXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pRspXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"Content\") fail", "NA");
		return IVS_XML_INVALID;
	}

	if (xml.IntoElem())
	{
		(void)xml.AddElem("ResultCode");
	}
	else
	{
		(void)xml.AddChildElem("ResultCode");
	}
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	strResult = xml.GetXMLStream(iLen);
	return IVS_SUCCEED;
}

IVS_INT32 COCXAlarmMgrXMLProcess::ParseAlarmCommissionXML(const IVS_CHAR* pReqXml, IVS_CHAR* pAlarmInCode, IVS_ALARM_COMMISSION*& pAlarmCommission)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	CHECK_POINTER(pAlarmCommission, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xml;
	if (!xml.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pReqXml) is fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"Content\") is fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();

	const IVS_CHAR* szElemValue = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("AlarmEventID", szElemValue, pAlarmCommission->ullAlarmEventID, xml);
	GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, pAlarmInCode, IVS_ALARM_CODE_LEN, xml);
	if (!xml.FindElem("UserInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"UserIDList\") is fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();

	if (!xml.FindElem("UserInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"UserID\") is fail", "NA");
		return IVS_XML_INVALID;
	}
	IVS_UINT32 ui = 0;
	for(ui = 0; ; ui++)
	{
		(void)xml.IntoElem();
		IVS_USER_ID_INFO& stUserInfo = pAlarmCommission->stUserInfo[ui];
		GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, stUserInfo.uiUserID, xml);
		GET_ELEM_VALUE_CHAR("UserDomainCode", szElemValue, stUserInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, xml);
		xml.OutOfElem();
		if (!xml.NextElem())
		{
			break;
		}
	}
	xml.OutOfElem();
	return IVS_SUCCEED;
}

// 查询告警历史信息列表
IVS_INT32 COCXAlarmMgrXMLProcess::GetAlarmListGetXML(CString &strResult, IVS_ALARM_EVENT_LIST* pResAlarmEventList, IVS_INT32 &iRet, CXml &xml)
{
	if (NULL == pResAlarmEventList)
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID,"pResAlarmEventList == NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_SMU_XML_INVALID); 
		return IVS_SMU_XML_INVALID;
	}
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());	

	(void)xml.AddElem("PageInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("RealNum");
	(void)xml.IntoElem();

	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pResAlarmEventList->iTotal)).c_str());	
	(void)xml.AddElem("FromIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pResAlarmEventList->stIndexRange).uiFromIndex)).c_str());	

	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pResAlarmEventList->stIndexRange).uiToIndex)).c_str());	
	xml.OutOfElem();

	(void)xml.AddElem("AlarmList");

	IVS_UINT32  i = 0;
	IVS_UINT32 uiFromIndex = pResAlarmEventList->stIndexRange.uiFromIndex;
	IVS_UINT32 uiToIndex = pResAlarmEventList->stIndexRange.uiToIndex;
	IVS_UINT32 uiTotal = pResAlarmEventList->iTotal;
	IVS_UINT32 uiRestltNum = 0;
	// 判断实际返回来的数据有多少条
	if (uiTotal <= uiToIndex )
	{
		uiRestltNum = (uiTotal - uiFromIndex) + 1;
	}
	else
	{
		uiRestltNum = (uiToIndex - uiFromIndex) + 1;	
	}
	while(i < uiRestltNum)
	{
		IVS_ALARM_EVENT &pAlarmEvent = pResAlarmEventList->stSubscribeInfo[i];
		i == 0?(void)xml.AddChildElem("AlarmInfo"):(void)xml.AddElem("AlarmInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("AlarmEventID");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent.stAlarmNotify.ullAlarmEventID)).c_str());

		(void)xml.AddElem("AlarmInCode");
		IVS_CHAR cAlarmInCode[IVS_ALARM_CODE_LEN + 1];
		memset(cAlarmInCode, 0, IVS_ALARM_CODE_LEN + 1);
		(void)CToolsHelp::Memcpy(cAlarmInCode, IVS_ALARM_CODE_LEN, pAlarmEvent.stAlarmNotify.cAlarmInCode, IVS_ALARM_CODE_LEN);
		IVS_CHAR cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1];
		memset(cDevDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
		(void)CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pAlarmEvent.stAlarmNotify.cDevDomainCode, IVS_DOMAIN_CODE_LEN);
		std::string strAlarmInCode = cAlarmInCode;
		strAlarmInCode.append("#").append(cDevDomainCode);
		(void)xml.SetElemValue(strAlarmInCode.c_str());

		(void)xml.AddElem("AlarmInType");
		std::string strAlarmInType;
		if (ALARM_IN_CAMERA == pAlarmEvent.stAlarmNotify.uiAlarmInType)
		{
			strAlarmInType = "01";
		}
		else if (ALARM_IN_DI == pAlarmEvent.stAlarmNotify.uiAlarmInType)
		{
			strAlarmInType = "02";
		}
		else
		{
			strAlarmInType = "09";
		}
		(void)xml.SetElemValue(strAlarmInType.c_str());

		(void)xml.AddElem("AlarmInName");
		IVS_CHAR cAlarmInName[IVS_ALARM_NAME_LEN + 1];
		memset(cAlarmInName, 0, IVS_ALARM_NAME_LEN + 1);
		(void)CToolsHelp::Memcpy(cAlarmInName, IVS_ALARM_NAME_LEN, pAlarmEvent.stAlarmNotify.cAlarmInName, IVS_ALARM_NAME_LEN);
		(void)xml.SetElemValue(cAlarmInName);

		(void)xml.AddElem("AlarmLevelValue");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent.stAlarmNotify.uiAlarmLevelValue)).c_str());

		(void)xml.AddElem("AlarmLevelName");
		IVS_CHAR cAlarmLevelName[IVS_NAME_LEN + 1];
		memset(cAlarmLevelName, 0, IVS_NAME_LEN + 1);
		(void)CToolsHelp::Memcpy(cAlarmLevelName, IVS_NAME_LEN, pAlarmEvent.stAlarmNotify.cAlarmLevelName, IVS_NAME_LEN);
		(void)xml.SetElemValue(cAlarmLevelName);

		(void)xml.AddElem("AlarmLevelColor");
		IVS_CHAR cAlarmLevelColor[IVS_ALARM_LEVEL_COLOR_LEN + 1];
		memset(cAlarmLevelColor, 0, IVS_ALARM_LEVEL_COLOR_LEN + 1);
		(void)CToolsHelp::Memcpy(cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN, pAlarmEvent.stAlarmNotify.cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN);

		(void)xml.SetElemValue(cAlarmLevelColor);

		(void)xml.AddElem("AlarmType");
		IVS_CHAR cAlarmType[IVS_ALARM_CODE_LEN + 1];
		memset(cAlarmType, 0, IVS_ALARM_CODE_LEN + 1);
		(void)CToolsHelp::Memcpy(cAlarmType, IVS_ALARM_CODE_LEN, pAlarmEvent.stAlarmNotify.cAlarmType, IVS_ALARM_CODE_LEN);
		(void)xml.SetElemValue(cAlarmType);

		(void)xml.AddElem("AlarmCategory");
		IVS_CHAR cAlarmCategory[IVS_ALARM_CATEGORY_LEN + 1];
		memset(cAlarmCategory, 0, IVS_ALARM_CATEGORY_LEN + 1);
		(void)CToolsHelp::Memcpy(cAlarmCategory, IVS_ALARM_CATEGORY_LEN, pAlarmEvent.stAlarmNotify.cAlarmCategory, IVS_ALARM_CATEGORY_LEN);
		(void)xml.SetElemValue(cAlarmCategory);

		(void)xml.AddElem("OccurTime");
		IVS_CHAR cOccurTime[IVS_TIME_LEN + 1];
		memset(cOccurTime, 0, IVS_TIME_LEN + 1);
		(void)CToolsHelp::Memcpy(cOccurTime, IVS_TIME_LEN, pAlarmEvent.stAlarmNotify.cOccurTime, IVS_TIME_LEN);
		(void)xml.SetElemValue(cOccurTime);

		(void)xml.AddElem("OccurNumber");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent.stAlarmNotify.uiOccurNumber)).c_str());

		(void)xml.AddElem("AlarmStatus");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent.stAlarmNotify.uiAlarmStatus)).c_str());

		(void)xml.AddElem("Operator");
		IVS_CHAR cOperator[IVS_NAME_LEN + 1];
		memset(cOperator, 0, IVS_NAME_LEN + 1);
		(void)CToolsHelp::Memcpy(cOperator, IVS_NAME_LEN, pAlarmEvent.stOperateInfo.cOperatorName, IVS_NAME_LEN);
		(void)xml.SetElemValue(cOperator);

		(void)xml.AddElem("OperateInfo");
		IVS_CHAR cOperateInfo[IVS_ALARM_DESCRIPTION_LEN + 1];
		memset(cOperateInfo, 0, IVS_ALARM_DESCRIPTION_LEN + 1);
		(void)CToolsHelp::Memcpy(cOperateInfo, IVS_ALARM_DESCRIPTION_LEN, pAlarmEvent.stOperateInfo.cOperateInfo, IVS_ALARM_DESCRIPTION_LEN);
		(void)xml.SetElemValue(cOperateInfo);

		(void)xml.AddElem("OperateTime");
		IVS_CHAR cOperateTime[IVS_TIME_LEN + 1];
		memset(cOperateTime, 0, IVS_TIME_LEN + 1);
		(void)CToolsHelp::Memcpy(cOperateTime, IVS_TIME_LEN, pAlarmEvent.stOperateInfo.cOperateTime, IVS_TIME_LEN);
		(void)xml.SetElemValue(cOperateTime);

		(void)xml.AddElem("AlarmDesc");
		IVS_CHAR cAlarmDesc[IVS_ALARM_DESCRIPTION_LEN + 1];
		memset(cAlarmDesc, 0, IVS_ALARM_DESCRIPTION_LEN + 1);
		(void)CToolsHelp::Memcpy(cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, pAlarmEvent.stAlarmNotify.cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN);
		(void)xml.SetElemValue(cAlarmDesc);

		(void)xml.AddElem("ExistsRecord");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pAlarmEvent.stAlarmNotify.bExistsRecord).c_str());

		(void)xml.AddElem("PreviewUrl");
		IVS_CHAR cPrwviewRecord[IVS_URL_LEN + 1];
		memset(cPrwviewRecord, 0, IVS_URL_LEN + 1);
		(void)CToolsHelp::Memcpy(cPrwviewRecord, IVS_URL_LEN, pAlarmEvent.stAlarmNotify.cPreviewUrl, IVS_URL_LEN);
		(void)xml.SetElemValue(cPrwviewRecord);
	
		xml.OutOfElem();
		i++;
	}

	xml.OutOfElem();
	xml.OutOfElem();
	IVS_UINT32 xmlLen = 0;
	strResult = xml.GetXMLStream(xmlLen);

	return IVS_SUCCEED;
}


// 查询告警信息事件拼接XML
IVS_INT32 COCXAlarmMgrXMLProcess::GetAlarmInfoGetXML(CString &strResult, const IVS_ALARM_EVENT* pAlarmEvent)
{
	if (NULL == pAlarmEvent)
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "pAlarmEvent == NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_SMU_XML_INVALID); 
		return IVS_SMU_XML_INVALID;
	}
	IVS_DEBUG_TRACE("");

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("AlarmInfo");
	(void)xml.IntoElem();
	
	(void)xml.AddChildElem("AlarmEventID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.ullAlarmEventID)).c_str());

	(void)xml.AddElem("AlarmInCode");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmInCode[IVS_ALARM_CODE_LEN + 1];
	memset(cAlarmInCode, 0x0, IVS_ALARM_CODE_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmInCode, IVS_ALARM_CODE_LEN, pAlarmEvent->stAlarmNotify.cAlarmInCode, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmInCode failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_CHAR cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1];
	memset(cDevDomainCode, 0x0, IVS_DOMAIN_CODE_LEN + 1);
	if (!CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pAlarmEvent->stAlarmNotify.cDevDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cDevDomainCode failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	std::string strAlarmInCode = cAlarmInCode;
	strAlarmInCode.append("#").append(cDevDomainCode);
	(void)xml.SetElemValue(strAlarmInCode.c_str());
			
	(void)xml.AddElem("AlarmInType");
	std::string strAlarmInType;
	if (ALARM_IN_CAMERA == pAlarmEvent->stAlarmNotify.uiAlarmInType)
	{
		strAlarmInType = "01";
	}
	else if (ALARM_IN_DI == pAlarmEvent->stAlarmNotify.uiAlarmInType)
	{
		strAlarmInType = "02";
	}
	else
	{
		strAlarmInType = "09";
	}
	(void)xml.SetElemValue(strAlarmInType.c_str());

	(void)xml.AddElem("AlarmInName");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmInName[IVS_ALARM_NAME_LEN + 1];
	memset(cAlarmInName, 0x0, IVS_ALARM_NAME_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmInName, IVS_ALARM_NAME_LEN, pAlarmEvent->stAlarmNotify.cAlarmInName, IVS_ALARM_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmInName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmInName);

	(void)xml.AddElem("AlarmLevelValue");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.uiAlarmLevelValue)).c_str());

	(void)xml.AddElem("AlarmLevelName");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmLevelName[IVS_NAME_LEN + 1];
	memset(cAlarmLevelName, 0x0, IVS_NAME_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmLevelName, IVS_NAME_LEN, pAlarmEvent->stAlarmNotify.cAlarmLevelName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmLevelName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmLevelName);

	(void)xml.AddElem("AlarmLevelColor");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmLevelColor[IVS_ALARM_LEVEL_COLOR_LEN + 1];
	memset(cAlarmLevelColor, 0x0, IVS_ALARM_LEVEL_COLOR_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN, pAlarmEvent->stAlarmNotify.cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmLevelColor failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmLevelColor);

	(void)xml.AddElem("AlarmType");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmType[IVS_ALARM_CODE_LEN + 1];
	memset(cAlarmType, 0x0, IVS_ALARM_CODE_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmType, IVS_ALARM_CODE_LEN, pAlarmEvent->stAlarmNotify.cAlarmType, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmType failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmType);

	(void)xml.AddElem("AlarmCategory");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmCategory[IVS_ALARM_CATEGORY_LEN + 1];
	memset(cAlarmCategory, 0x0, IVS_ALARM_CATEGORY_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmCategory, IVS_ALARM_CATEGORY_LEN, pAlarmEvent->stAlarmNotify.cAlarmCategory, IVS_ALARM_CATEGORY_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmCategory failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmCategory);

	(void)xml.AddElem("OccurTime");
	(void)xml.IntoElem();
	IVS_CHAR cOccurTime[IVS_TIME_LEN + 1];
	memset(cOccurTime, 0x0, IVS_TIME_LEN + 1);
	if (!CToolsHelp::Memcpy(cOccurTime, IVS_TIME_LEN, pAlarmEvent->stAlarmNotify.cOccurTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cOccurTime failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOccurTime);

	(void)xml.AddElem("OccurNumber");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.uiOccurNumber)).c_str());

	(void)xml.AddElem("AlarmStatus");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.uiAlarmStatus)).c_str());

	(void)xml.AddElem("Operator");
	(void)xml.IntoElem();
	IVS_CHAR cOperator[IVS_NAME_LEN + 1];
	memset(cOperator, 0x0, IVS_NAME_LEN + 1);
	if (!CToolsHelp::Memcpy(cOperator, IVS_NAME_LEN, pAlarmEvent->stOperateInfo.cOperatorName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stOperateInfo.cOperatorName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOperator);

	(void)xml.AddElem("OperateInfo");
	(void)xml.IntoElem();
	IVS_CHAR cOperateInfo[IVS_ALARM_DESCRIPTION_LEN + 1];
	memset(cOperateInfo, 0x0, IVS_ALARM_DESCRIPTION_LEN + 1);
	if (!CToolsHelp::Memcpy(cOperateInfo, IVS_ALARM_DESCRIPTION_LEN, pAlarmEvent->stOperateInfo.cOperateInfo, IVS_ALARM_DESCRIPTION_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stOperateInfo.cOperateInfo failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOperateInfo);

	(void)xml.AddElem("OperateTime");
	(void)xml.IntoElem();
	IVS_CHAR cOperateTime[IVS_TIME_LEN + 1];
	memset(cOperateTime, 0x0, IVS_TIME_LEN + 1);
	if (!CToolsHelp::Memcpy(cOperateTime, IVS_TIME_LEN, pAlarmEvent->stOperateInfo.cOperateTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stOperateInfo.cOperateTime failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOperateTime);

	(void)xml.AddElem("AlarmDesc");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmDesc[IVS_ALARM_DESCRIPTION_LEN + 1];
	memset(cAlarmDesc, 0x0, IVS_ALARM_DESCRIPTION_LEN +1);
	if (!CToolsHelp::Memcpy(cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, pAlarmEvent->stAlarmNotify.cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmDesc failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmDesc);

	(void)xml.AddElem("ExistsRecord");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.bExistsRecord)).c_str());

	(void)xml.AddElem("PreviewUrl");
	(void)xml.IntoElem();
	IVS_CHAR cPreviewUrl[IVS_URL_LEN + 1];
	memset(cPreviewUrl, 0x0, IVS_URL_LEN + 1);
	if (!CToolsHelp::Memcpy(cPreviewUrl, IVS_URL_LEN, pAlarmEvent->stAlarmNotify.cPreviewUrl, IVS_URL_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cPreviewUrl failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cPreviewUrl);
	
	xml.OutOfElem();

	xml.OutOfElem();

	IVS_UINT32 iLen = 0;
	//把xml生成string
	strResult = xml.GetXMLStream(iLen);

	return IVS_SUCCEED;

}

// 告警状态上报
IVS_INT32 COCXAlarmMgrXMLProcess::GetAlarmStatusGetXML(const IVS_ALARM_STATUS_NOTIFY* pAlarmStatusNotify, CXml& xml)
{
	CHECK_POINTER(pAlarmStatusNotify, IVS_OPERATE_MEMORY_ERROR);
	//IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmStatusNotify");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmEventID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmStatusNotify->ullAlarmEventID)).c_str());

	(void)xml.AddElem("AlarmInCode");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmInCode[IVS_ALARM_CODE_LEN + 1];
	memset(cAlarmInCode, 0x0, IVS_ALARM_CODE_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmInCode, IVS_ALARM_CODE_LEN, pAlarmStatusNotify->cAlarmInCode, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmStatusNotify->cAlarmInCode failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmInCode);

	(void)xml.AddElem("DevDomainCode");
	(void)xml.IntoElem();
	IVS_CHAR cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pAlarmStatusNotify->cDevDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmStatusNotify->cDevDomainCode failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevDomainCode);

	(void)xml.AddElem("AlarmType");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmType[IVS_ALARM_CODE_LEN + 1];
	memset(cAlarmType, 0x0, IVS_ALARM_CODE_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmType, IVS_ALARM_CODE_LEN, pAlarmStatusNotify->cAlarmType, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmStatusNotify->cAlaryType failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmType);

	(void)xml.AddElem("AlarmStatus");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmStatusNotify->uiAlarmStatus)).c_str());

	(void)xml.AddElem("OccurNumber");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmStatusNotify->uiOccurNumber)).c_str());


	(void)xml.AddElem("AlarmOperateInfo");
	(void)xml.IntoElem();


	(void)xml.AddChildElem("OperatorID");
	(void)xml.IntoElem();
	if (0 == pAlarmStatusNotify->stOperateInfo.uiOperatorID)
	{
		xml.SetElemValue("");
	}
	else
	{
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmStatusNotify->stOperateInfo.uiOperatorID)).c_str());
	}


	(void)xml.AddElem("OperatorName");
	(void)xml.IntoElem();
	IVS_CHAR cOperatorName[IVS_NAME_LEN + 1];
	memset(cOperatorName, 0x0, IVS_NAME_LEN + 1);
	if (!CToolsHelp::Memcpy(cOperatorName, IVS_NAME_LEN, pAlarmStatusNotify->stOperateInfo.cOperatorName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmStatusNotify->stOperateInfo.cOperatorName failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOperatorName);

	(void)xml.AddElem("OperateTime");
	(void)xml.IntoElem();
	IVS_CHAR cOperateTime[IVS_TIME_LEN + 1];
	memset(cOperateTime, 0x0, IVS_TIME_LEN + 1);
	if (!CToolsHelp::Memcpy(cOperateTime, IVS_TIME_LEN, pAlarmStatusNotify->stOperateInfo.cOperateTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmStatusNotify->stOperateInfo.cOperateTime failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOperateTime);

	(void)xml.AddElem("Reserve");
	(void)xml.IntoElem();
	IVS_CHAR cReserve[ALARM_RESERVER_NUM + 1];
	memset(cReserve, 0x0, ALARM_RESERVER_NUM + 1);
	if (!CToolsHelp::Memcpy(cReserve, ALARM_RESERVER_NUM, pAlarmStatusNotify->stOperateInfo.cReserver, ALARM_RESERVER_NUM))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmStatusNotify->stOperateInfo.cReserver failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cReserve);

	(void)xml.AddElem("OperateInfo");
	(void)xml.IntoElem();
	IVS_CHAR cOperateInfo[IVS_ALARM_DESCRIPTION_LEN + 1];
	memset(cOperateInfo, 0x0, IVS_ALARM_DESCRIPTION_LEN + 1);
	if (!CToolsHelp::Memcpy(cOperateInfo, IVS_ALARM_DESCRIPTION_LEN, pAlarmStatusNotify->stOperateInfo.cOperateInfo, IVS_ALARM_DESCRIPTION_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmStatusNotify->stOperateInfo.cOperateInfo failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOperateInfo);

	xml.OutOfElem();	
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 告警确认解析XML
IVS_INT32 COCXAlarmMgrXMLProcess::AlarmConfirmParseXML(IVS_UINT64* ullAlarmEventID, IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo, IVS_CHAR* pAlarmInCode, CXml &xml)
{
	CHECK_POINTER(ullAlarmEventID, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmOperateInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml can not findElem(Content)", "NA");
		return IVS_XML_INVALID;
	}

	(void)xml.IntoElem();
	const char* szElemValue = NULL;
	IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_NUM("AlarmEventID", szElemValue, iTemp, xml);
	*ullAlarmEventID = static_cast<IVS_UINT64>(iTemp); //lint !e571
	GET_ELEM_VALUE_CHAR("OperateInfo", szElemValue, pAlarmOperateInfo->cOperateInfo, IVS_ALARM_DESCRIPTION_LEN, xml);
	GET_ELEM_VALUE_CHAR("OperateTime", szElemValue, pAlarmOperateInfo->cOperateTime, IVS_TIME_LEN, xml);
	GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, pAlarmInCode, IVS_ALARM_CODE_LEN, xml);
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析告警信息上报xml
IVS_INT32 COCXAlarmMgrXMLProcess::ParseAlarmReportXML(const IVS_CHAR* pReqXml, IVS_ALARM_REPORT* pAlarmReport)
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmReport, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xml;
	if (!xml.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pReqXml) is error", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElemEx("Content/AlarmReport"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content) is error", "NA");
		return IVS_XML_INVALID;
	}

	const IVS_CHAR* szElemValue = NULL;
	IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, pAlarmReport->cAlarmInCode, IVS_ALARM_CODE_LEN, xml);
	GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, pAlarmReport->cDevDomainCode, IVS_DOMAIN_CODE_LEN, xml);
	GET_ELEM_VALUE_CHAR("AlarmType", szElemValue, pAlarmReport->cAlarmType, IVS_ALARM_TYPE_LEN, xml);
	GET_ELEM_VALUE_CHAR("OccurTime", szElemValue, pAlarmReport->cOccurTime, IVS_TIME_LEN, xml);
	GET_ELEM_VALUE_CHAR("ReportTime", szElemValue, pAlarmReport->cReportTime, IVS_TIME_LEN, xml);
	GET_ELEM_VALUE_NUM("Action", szElemValue, iTemp, xml);
	pAlarmReport->bAction = static_cast<IVS_BOOL>(iTemp);
	GET_ELEM_VALUE_CHAR("AlarmID", szElemValue, pAlarmReport->cAlarmID, IVS_ALARM_REPORT_ID_LEN, xml);
	GET_ELEM_VALUE_NUM("OccurNumber", szElemValue, iTemp, xml);
	pAlarmReport->uiOccurNumber = static_cast<IVS_UINT32>(iTemp);
	GET_ELEM_VALUE_CHAR("NVRCode", szElemValue, pAlarmReport->cNvrCode, IVS_NVR_CODE_LEN, xml);
	GET_ELEM_VALUE_CHAR("AlarmDesc", szElemValue, pAlarmReport->cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, xml);
	GET_ELEM_VALUE_CHAR("ExtParam", szElemValue, pAlarmReport->cExtParam, IVS_ALARM_EX_PARAM_LEN, xml);

	return IVS_SUCCEED;
}
#define STEPINDEX 256
// 获取拥有指定设备权限的用户列表拼接XML
IVS_INT32 COCXAlarmMgrXMLProcess::GetDeviceUserList(std::list<IVS_DEVICE_USER_INFO_LIST*>& listDevUserInfoList, CString &strResult, IVS_UINT32 uiFromIndex, IVS_UINT32 uiToIndex, CXml &xml)
{
	IVS_DEBUG_TRACE("");
	if (listDevUserInfoList.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get DeviceUserList","FAILED");
		return IVS_FAIL;
	}
	std::list<IVS_DEVICE_USER_INFO_LIST*>::iterator iterDevUserList = listDevUserInfoList.begin();
	IVS_DEVICE_USER_INFO_LIST* pDevUserInfo = (IVS_DEVICE_USER_INFO_LIST*)(*iterDevUserList);
    IVS_UINT32 uiTotal = pDevUserInfo->uiTotal;
	IVS_UINT32 xmlLen = 0;

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("PageInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("RealNum");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiTotal)).c_str());

	(void)xml.AddElem("FromIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiFromIndex)).c_str());

	(void)xml.AddElem("ToIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiToIndex)).c_str());
	xml.OutOfElem();

	(void)xml.AddElem("UserList");
	(void)xml.IntoElem();
	// 如果返回的结果总数是0，直接返回成功，不再继续拼装xml
	if (0 == uiTotal)
	{
		xml.OutOfElem();
		strResult = xml.GetXMLStream(xmlLen);

		return IVS_SUCCEED;
	}
	IVS_UINT32 uiCircleNum = 0;// 判断读取次数
	IVS_INT32 iFlag = 0; // 作为拼装xml时的标志位，判断是使用AddChildElem OR AddElem；
	while(iterDevUserList != listDevUserInfoList.end())
	{
		pDevUserInfo = (IVS_DEVICE_USER_INFO_LIST*)(*iterDevUserList);
		while(uiCircleNum < STEPINDEX)
		{
			IVS_DEVICE_USER_INFO &stDevUserInfo = pDevUserInfo->stDeviceUserInfo[uiCircleNum];  //lint !e662
			// 如果说下一条结构体中没有数据，那么对应的UserID为0,LoginName为空，不在继续作为有效值拼装xml，循环跳出，继续寻找list中的下一个元素。
			if (0 == stDevUserInfo.uiUserID &&  '\0' == stDevUserInfo.cLoginName[0]) //lint !e774 
			{
				BP_RUN_LOG_INF("no more info, break and next", "NA");
				break;
			}
			iFlag == 0?(void)xml.AddChildElem("UserInfo"):(void)xml.AddElem("UserInfo");
			(void)xml.IntoElem();
			(void)xml.AddChildElem("UserID");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stDevUserInfo.uiUserID)).c_str());

			(void)xml.AddElem("LoginName");
			(void)xml.IntoElem();
			IVS_CHAR cLoginName[IVS_NAME_LEN + 1];
			memset(cLoginName, 0x0, IVS_NAME_LEN + 1);
			if (!CToolsHelp::Memcpy(cLoginName, IVS_NAME_LEN, stDevUserInfo.cLoginName, IVS_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevUserInfo.cLoginName failed", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cLoginName);

			(void)xml.AddElem("UserName");
			(void)xml.IntoElem();
			IVS_CHAR cUserName[IVS_NAME_LEN + 1];
			memset(cUserName, 0x0, IVS_NAME_LEN + 1);
			if (!CToolsHelp::Memcpy(cUserName, IVS_NAME_LEN, stDevUserInfo.cUserName, IVS_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevUserInfo.cUserName failed", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cUserName);

			(void)xml.AddElem("UserGroupID");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stDevUserInfo.uiGroupID)).c_str());

			xml.OutOfElem();

			uiCircleNum++;
			iFlag++;
		}
		iterDevUserList++;
		uiCircleNum = 0;
	}
	xml.OutOfElem();

	xml.OutOfElem();
	strResult = xml.GetXMLStream(xmlLen);

	return IVS_SUCCEED;
}

// 将Resultcode拼在报文的前面
IVS_INT32 COCXAlarmMgrXMLProcess::AddResultCodeUp(const char* pRsqXml, CString &strResult)
{
	CHECK_POINTER(pRsqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xml;
	// 构造ResultCode的节点
	TiXmlElement* pResultCodeElem = NULL;
	try
	{
		pResultCodeElem = new TiXmlElement("ResultCode");

	}
	catch(...)
	{
		BP_RUN_LOG_INF("new TiXmlElement(LoginInfo) is error", "NA");
		pResultCodeElem = NULL;
		return IVS_OPERATE_MEMORY_ERROR;
	}

	TiXmlText* pResultCodeText = NULL;
	try
	{
		pResultCodeText = new TiXmlText("0");
	}
	catch(...)
	{
		BP_RUN_LOG_INF("new TiXmlText(pLoginID) is error", "NA");
		IVS_DELETE(pResultCodeElem);
		pResultCodeElem = NULL;
		pResultCodeText = NULL;
		return IVS_OPERATE_MEMORY_ERROR;
	}
	pResultCodeElem->LinkEndChild(pResultCodeText);

	// 加载XML
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	if (NULL == pXmlDoc)
	{
		BP_RUN_LOG_INF("NULL == pXmlDoc", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429

	}
	//CHECK_POINTER(pXmlDoc, IVS_FAIL);
	(void)(pXmlDoc->Parse(pRsqXml, NULL, TIXML_ENCODING_LEGACY));
	if (0 != pXmlDoc->Error())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "parse xml failed", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}
	// 查找content节点，如果根节点就是content就不要找了，下面那个方法有缺陷
	TiXmlElement* pRootElem = pXmlDoc->RootElement();
	if (NULL == pRootElem)
	{
		BP_RUN_LOG_INF("NULL == pRootElem", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}
	TiXmlElement* pContent = NULL;
	if (strcmp(pRootElem->Value(), "Content") == 0)
	{
		pContent = pXmlDoc->RootElement();
	}
	else
	{
		pContent = CToolsHelp::FindElementByPath(pXmlDoc->RootElement(),"Content");
	}

	// 如果没有子节点 直接返回错
	TiXmlElement* pContentFirstChild = pContent->FirstChildElement();
	if (NULL == pContentFirstChild)
	{
		BP_RUN_LOG_INF("NULL == pContentFirstChild", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}
	// 在找到的第一个节点之前插入ResultCode
	if (NULL == pContent->InsertBeforeChild(pContentFirstChild, *pResultCodeElem))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "InsertBeforeChild error", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}
	// 将拼装好的xml打到Cxml对象里面，保持外部一致
	TiXmlPrinter xmlPrinter;
	xmlPrinter.SetStreamPrinting();		

	if (!pXmlDoc->Accept(&xmlPrinter))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "print error", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}

	(void)xml.Parse(xmlPrinter.CStr());

	IVS_DELETE(pXmlDoc);
	IVS_DELETE(pResultCodeElem);

	IVS_UINT32 uiLen = 0;
	strResult = xml.GetXMLStream(uiLen);

	return IVS_SUCCEED;//lint !e429
}

// 得到告警授权请求xml中UserID的个数
IVS_INT32 COCXAlarmMgrXMLProcess::GetUserNum(const char* pReqXml, IVS_INT32& iNum)
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xml;
	if (!xml.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pReqXml)", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElemEx("Content/UserInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/UserIDList)", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElem("UserInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(UserID)", "NA");
		return IVS_XML_INVALID;
	}
	do 
	{
		iNum++;
	} while (xml.NextElem());

	return IVS_SUCCEED;
}

// 查询OMU告警事件列表拼装响应xml
IVS_INT32 COCXAlarmMgrXMLProcess::GetDevAlarmListGetXML(CXml& xml, IVS_DEVICE_ALARM_EVENT_LIST* pDevAlarmList, IVS_INT32& iRet)
{
	CHECK_POINTER(pDevAlarmList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();


	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

	(void)xml.AddElem("PageInfo");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("RealNum");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevAlarmList->iTotal)).c_str());

	(void)xml.AddElem("FromIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevAlarmList->stIndexRange.uiFromIndex)).c_str());

	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevAlarmList->stIndexRange.uiToIndex)).c_str());
	xml.OutOfElem();

	(void)xml.AddElem("AlarmList");

	IVS_UINT32 uiNum = 0;
	if (pDevAlarmList->iTotal > pDevAlarmList->stIndexRange.uiToIndex)
	{
		uiNum = (pDevAlarmList->stIndexRange.uiToIndex - pDevAlarmList->stIndexRange.uiFromIndex) + 1;
	}
	else
	{
		uiNum = (pDevAlarmList->iTotal - pDevAlarmList->stIndexRange.uiFromIndex) + 1;
	}
	IVS_UINT32 ui = 0;
	while(ui < uiNum)
	{
		IVS_DEVICE_ALARM_NOTIFY& stDevAlarmNotify = pDevAlarmList->stDeviceAlarmNotify[ui];

		ui == 0?(void)xml.AddChildElem("AlarmInfo"):(void)xml.AddElem("AlarmInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("AlarmID");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stDevAlarmNotify.ullAlarmEventID)).c_str());

		(void)xml.AddElem("AlarmInCode");
		IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szAlarmInCode, IVS_ALARM_CODE_LEN, stDevAlarmNotify.cAlarmInCode, IVS_ALARM_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cAlarmInCode to szAlarmInCode fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szAlarmInCode);

		(void)xml.AddElem("AlarmInType");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stDevAlarmNotify.uiAlarmInType)).c_str());

		(void)xml.AddElem("AlarmInName");
		IVS_CHAR szAlarmInName[IVS_ALARM_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szAlarmInName, IVS_ALARM_NAME_LEN, stDevAlarmNotify.cAlarmInName, IVS_ALARM_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cAlarmInName to szAlarmInName fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szAlarmInName);

		(void)xml.AddElem("NodeType");
		IVS_CHAR szNodeType[IVS_NODE_TYPE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szNodeType, IVS_NODE_TYPE_LEN, stDevAlarmNotify.cNodeType, IVS_NODE_TYPE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cNodeType to szNodeType fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szNodeType);

		(void)xml.AddElem("AlarmLevelValue");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stDevAlarmNotify.uiAlarmLevelValue)).c_str());

		(void)xml.AddElem("AlarmType");
		IVS_CHAR szAlarmType[IVS_ALARM_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szAlarmType, IVS_ALARM_CODE_LEN, stDevAlarmNotify.cAlarmType, IVS_ALARM_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cAlaryType to szAlarmType fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szAlarmType);

		(void)xml.AddElem("AlarmCategory");
		IVS_CHAR szAlarmCategory[IVS_ALARM_CATEGORY_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szAlarmCategory, IVS_ALARM_CATEGORY_LEN, stDevAlarmNotify.cAlarmCategory, IVS_ALARM_CATEGORY_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cAlarmCategory to szAlarmCategory fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szAlarmCategory);

		(void)xml.AddElem("OccurFirTime");
		IVS_CHAR szOccurFirTime[IVS_TIME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szOccurFirTime, IVS_TIME_LEN, stDevAlarmNotify.cOccurFirstTime, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cOccurFirstTime to szOccurFirTime fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szOccurFirTime);

		(void)xml.AddElem("OccurEndTime");
		IVS_CHAR szOccurEndTime[IVS_TIME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szOccurEndTime, IVS_TIME_LEN, stDevAlarmNotify.cOccurEndTime, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cOccurEndTime to szOccurEndTime fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szOccurEndTime);

		(void)xml.AddElem("OccurNumber");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stDevAlarmNotify.uiOccurNumber)).c_str());

		(void)xml.AddElem("Reserve");
		IVS_CHAR szReserve[32 + 1] = {0};
		if (!CToolsHelp::Memcpy(szReserve, 32, stDevAlarmNotify.cReserver, 32))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cReserver to szReserve fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szReserve);

		(void)xml.AddElem("LocationInfo");
		IVS_CHAR szLocationInfo[IVS_DEVICE_ALARM_LOCATION_INFO_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szLocationInfo, IVS_DEVICE_ALARM_LOCATION_INFO_LEN, stDevAlarmNotify.cLocationInfo, IVS_DEVICE_ALARM_LOCATION_INFO_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cOccurEndTime to szOccurEndTime fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szLocationInfo);

		(void)xml.AddElem("AlarmDesc");
		IVS_CHAR szAlarmDesc[IVS_ALARM_DESCRIPTION_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, stDevAlarmNotify.cAlarmDest, IVS_ALARM_DESCRIPTION_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cAlarmDest to szAlarmDesc fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szAlarmDesc);

		(void)xml.AddElem("ExtParam");
		IVS_CHAR szExtParam[IVS_ALARM_EX_PARAM_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(szExtParam, IVS_ALARM_EX_PARAM_LEN, stDevAlarmNotify.cExtParam, IVS_ALARM_EX_PARAM_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cExtParam to szExtParam fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(szExtParam);

        (void)xml.AddElem("NetName");
        IVS_CHAR szNetName[IVS_NAME_LEN + 1] = {0};
        strncpy(szNetName, stDevAlarmNotify.cNetName, IVS_NAME_LEN);
        (void)xml.SetElemValue(szNetName);

		ui++;
		xml.OutOfElem();	
	}
	xml.OutOfElem();
	return IVS_SUCCEED;
}
// OMU设备告警上报拼装xml
IVS_INT32 COCXAlarmMgrXMLProcess::DevAlarmNotifyGetXML(CXml& xml, const IVS_DEVICE_ALARM_NOTIFY* pDevAlarmNotify)
{
	CHECK_POINTER(pDevAlarmNotify, IVS_PARA_INVALID);

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmInfo");
    (void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevAlarmNotify->ullAlarmEventID)).c_str());

	(void)xml.AddElem("AlarmInCode");
	IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szAlarmInCode, IVS_ALARM_CODE_LEN, pDevAlarmNotify->cAlarmInCode, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cAlarmInCode to szAlarmInCode fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szAlarmInCode);

	(void)xml.AddElem("AlarmInType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevAlarmNotify->uiAlarmInType)).c_str());

	(void)xml.AddElem("AlarmInName");
	IVS_CHAR szAlarmInName[IVS_ALARM_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szAlarmInName, IVS_ALARM_NAME_LEN, pDevAlarmNotify->cAlarmInName, IVS_ALARM_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cAlarmInName to szAlarmInName fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szAlarmInName);

	(void)xml.AddElem("NodeType");
	IVS_CHAR szNodeType[IVS_NODE_TYPE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szNodeType, IVS_NODE_TYPE_LEN, pDevAlarmNotify->cNodeType, IVS_NODE_TYPE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cNodeType to szNodeType fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szNodeType);

	(void)xml.AddElem("AlarmLevelValue");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevAlarmNotify->uiAlarmLevelValue)).c_str());

	(void)xml.AddElem("AlarmType");
	IVS_CHAR szAlarmType[IVS_ALARM_CODE_LEN + 1];
	if (!CToolsHelp::Memcpy(szAlarmType, IVS_ALARM_CODE_LEN, pDevAlarmNotify->cAlarmType, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cAlaryType to szAlarmType fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szAlarmType);

	(void)xml.AddElem("AlarmCategory");
	IVS_CHAR szAlarmCategory[IVS_ALARM_CATEGORY_LEN + 1];
	if (!CToolsHelp::Memcpy(szAlarmCategory, IVS_ALARM_CATEGORY_LEN, pDevAlarmNotify->cAlarmCategory, IVS_ALARM_CATEGORY_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cAlarmCategory to szAlarmCategory fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szAlarmCategory);

	(void)xml.AddElem("OccurFirTime");
	IVS_CHAR szOccurFirTime[IVS_TIME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szOccurFirTime, IVS_TIME_LEN, pDevAlarmNotify->cOccurFirstTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cOccurFirstTime to szOccurFirTime fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szOccurFirTime);

	(void)xml.AddElem("OccurEndTime");
	IVS_CHAR szOccurEndTime[IVS_TIME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szOccurEndTime, IVS_TIME_LEN, pDevAlarmNotify->cOccurEndTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cOccurEndTime to szOccurEndTime fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szOccurEndTime);

	(void)xml.AddElem("OccurNumber");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevAlarmNotify->uiOccurNumber)).c_str());

	(void)xml.AddElem("Reserve");
	IVS_CHAR szReserve[32 + 1] = {0};
	if (!CToolsHelp::Memcpy(szReserve, 32, pDevAlarmNotify->cReserver, 32))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cReserver to szReserve fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szReserve);

	(void)xml.AddElem("LocationInfo");
	IVS_CHAR szLocationInfo[IVS_DEVICE_ALARM_LOCATION_INFO_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szLocationInfo, IVS_DEVICE_ALARM_LOCATION_INFO_LEN, pDevAlarmNotify->cLocationInfo, IVS_DEVICE_ALARM_LOCATION_INFO_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cLocationInfo to szcLocationInfo fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szLocationInfo);

	(void)xml.AddElem("NetEleID");
	IVS_CHAR szNetEleID[IVS_NET_ELEMENT_ID_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szNetEleID, IVS_NET_ELEMENT_ID_LEN, pDevAlarmNotify->cNetEleID, IVS_NET_ELEMENT_ID_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotify->cNetEleID to szNetEleID fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szNetEleID);

	(void)xml.AddElem("AlarmDesc");
	IVS_CHAR szAlarmDesc[IVS_ALARM_DESCRIPTION_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, pDevAlarmNotify->cAlarmDest, IVS_ALARM_DESCRIPTION_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cAlarmDest to szAlarmDesc fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szAlarmDesc);

	(void)xml.AddElem("ExtParam");
	IVS_CHAR szExtParam[IVS_ALARM_EX_PARAM_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szExtParam, IVS_ALARM_EX_PARAM_LEN, pDevAlarmNotify->cExtParam, IVS_ALARM_EX_PARAM_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cExtParam to szExtParam fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szExtParam);

	(void)xml.AddElem("NetName");
	IVS_CHAR szNetName[IVS_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szNetName, IVS_NAME_LEN, pDevAlarmNotify->cNetName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stDevAlarmNotify.cNetName to szNetName fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szNetName);
	xml.OutOfElem();
	return IVS_SUCCEED;
}

// 重登录，拼装一个空的content
IVS_INT32 COCXAlarmMgrXMLProcess::ReLoginGetXML(CXml& xml)
{
    (void)xml.AddDeclaration("1.0", "UTF-8", "");
    (void)xml.AddElem("Content");
    return IVS_SUCCEED;
}

// 获取告警抓拍图片列表，若没有响应消息，拼装响应xml
IVS_INT32 COCXAlarmMgrXMLProcess::GetRspXml(CString& strResult, IVS_INT32 iRet)
{
	CXml xmlRsp;
	BP_RUN_LOG_ERR(iRet, " COCXAlarmMgrXMLProcess::GetRspXml", "NA");
	(void)xmlRsp.AddDeclaration("1.0","UTF-8","");

	(void)xmlRsp.AddElem("Content");
	(void)xmlRsp.AddChildElem("ResultCode");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	xmlRsp.OutOfElem();
	IVS_UINT32 uiLen = 0;
	const char* pXml = xmlRsp.GetXMLStream(uiLen);
	CHECK_POINTER(pXml, IVS_XML_PACK_ERROR);
	strResult = pXml;

	return IVS_SUCCEED;
}

// 删除，查询帮助信息，解析请求xml
IVS_INT32 COCXAlarmMgrXMLProcess::ParseHelpInfoXml(const IVS_CHAR* pHelpInfo, IVS_UINT32& ulSourceID, IVS_UINT32& ulSourceType, IVS_CHAR* pDomainCode)
{
	CHECK_POINTER(pHelpInfo, IVS_XML_INVALID);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xml;
	if (!xml.Parse(pHelpInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pHelpInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"Content\") fail", "NA");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	const IVS_CHAR* szElemValue = NULL;
	GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pDomainCode, IVS_DOMAIN_CODE_LEN, xml);
	if (!xml.FindElem("HelpInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"HelpInfo\") fail", "NA");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	GET_ELEM_VALUE_NUM_FOR_UINT("SourceID", szElemValue, ulSourceID, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("SourceType", szElemValue, ulSourceType, xml);

	xml.GetRootPos();

	return IVS_SUCCEED;
}
