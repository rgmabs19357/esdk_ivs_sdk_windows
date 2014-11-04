#include "AlarmMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
/*#include "ivs_xml.h"*/
#include "UserMgr.h"
#include "IVS_Trace.h"
#include "bp_os.h"




CAlarmMgrXMLProcess::CAlarmMgrXMLProcess()
{
}

CAlarmMgrXMLProcess::~CAlarmMgrXMLProcess()
{
}

//水印告警确认XML
IVS_INT32 CAlarmMgrXMLProcess::GetConfirmWaterAlarmXML(const IVS_UINT64 ullAlarmEventID,const IVS_CHAR* pConfirmInfo,
	const IVS_CHAR* pConfirmTime,const std::string& strUserDomainCode,const std::string& strDomainCode,CXml &xml)
{
	CHECK_POINTER(pConfirmInfo, IVS_PARA_INVALID);
	CHECK_POINTER(pConfirmTime, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("LoginInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("UserIDDomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strUserDomainCode.c_str());
	xml.OutOfElem();


	(void)xml.AddElem("AlarmEventID");
	(void)xml.IntoElem();
	char szAlarmEventID[64] = {0};
	(void)_snprintf_s(szAlarmEventID, 64, "%lld",ullAlarmEventID);
	(void)xml.SetElemValue(szAlarmEventID);

	if (!strDomainCode.empty())
	{
		(void)xml.AddElem("DomainCode");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(strDomainCode.c_str());
	}

	(void)xml.AddElem("ConfirmInfo");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pConfirmInfo);
	(void)xml.AddElem("ConfirmTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pConfirmTime);
	xml.OutOfElem();
	return IVS_SUCCEED;
}
//水印告警撤销
IVS_INT32 CAlarmMgrXMLProcess::GetCancelWaterAlarmXML(const IVS_UINT64 ullAlarmEventID,const IVS_CHAR* pCancelInfo,
	const IVS_CHAR* pCancelTime,const std::string& strUserDomainCode,const std::string& strDomainCode,CXml &xml)
{
	CHECK_POINTER(pCancelInfo, IVS_PARA_INVALID);
	CHECK_POINTER(pCancelTime, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("LoginInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("UserIDDomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strUserDomainCode.c_str());
	xml.OutOfElem();

	(void)xml.AddElem("AlarmEventID");
	(void)xml.IntoElem();
	char szAlarmEventID[64] = {0};
	(void)_snprintf_s(szAlarmEventID, 64, "%lld",ullAlarmEventID);
	(void)xml.SetElemValue(szAlarmEventID);

	if (!strDomainCode.empty())
	{
		(void)xml.AddElem("DomainCode");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(strDomainCode.c_str());
	}

	(void)xml.AddElem("CancelInfo");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pCancelInfo);
	(void)xml.AddElem("CancelTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pCancelTime);
	xml.OutOfElem();
	return IVS_SUCCEED;
}
IVS_INT32 CAlarmMgrXMLProcess::GetWaterAlarmListParseXML(IVS_WATERMARK_ALARM_INFO_LIST* pWaterAlarmInfoList,const std::string& strDoamianCode,
		IVS_UINT32 uiBufferSize, CXml &Rspxml)
{
	CHECK_POINTER(pWaterAlarmInfoList, IVS_PARA_INVALID);
	IVS_UINT32 iReqNum = (uiBufferSize-sizeof(IVS_WATERMARK_ALARM_INFO_LIST))/sizeof(IVS_WATERMARK_ALARM_INFO_LIST) + 1;
	// 没有找到相关的节点
	if (!Rspxml.FindElemEx("Content/PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get WaterAlarm List ParseXML fail", "Fail", "FALSE == Rspxml.FindElem(Content/PageInfo)");
		return IVS_XML_INVALID;
	}  

	const IVS_CHAR *pTemp = NULL;
	IVS_INT32 iFromTemp = 0;
	IVS_INT32 iToTemp = 0;
	IVS_INT32 iTotal = 0;

	GET_ELEM_VALUE_NUM("RealNum", pTemp, iTotal, Rspxml);
	GET_ELEM_VALUE_NUM("ToIndex", pTemp,  iToTemp, Rspxml);
	GET_ELEM_VALUE_NUM("FromIndex", pTemp, iFromTemp, Rspxml);
	// 判断是否正确 如果RealNum返回的是0，那么就直接返回
	if (0 == iTotal)
	{
		pWaterAlarmInfoList->uiTotal = static_cast<IVS_UINT32>(iTotal);    
		pWaterAlarmInfoList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromTemp);    
		pWaterAlarmInfoList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToTemp);
		return IVS_SUCCEED;
	}
	else
	{
		if (iTotal < 0 || iToTemp < 0 || iFromTemp < 0 || iFromTemp > iToTemp)
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get WaterAlarm List ParseXML  fail", "Total=%d,FromIndex=%d,ToIndex=%d",iTotal, iFromTemp, iToTemp);
			return IVS_XML_INVALID;
		}
	}

	pWaterAlarmInfoList->uiTotal = static_cast<IVS_UINT32>(iTotal);    
	pWaterAlarmInfoList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromTemp);    
	pWaterAlarmInfoList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToTemp);

	IVS_UINT32 iRspNum = (pWaterAlarmInfoList->stIndexRange.uiToIndex - pWaterAlarmInfoList->stIndexRange.uiFromIndex) + 1;

	//判断返回的数量大于请求的   
	if (iRspNum > iReqNum)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get WaterAlarm List ParseXML fail", "iRspNum=%d,iReqNum=%d", iRspNum, iReqNum);
		return IVS_XML_INVALID;
	}
	else
	{
		iReqNum = iRspNum;
	}

	Rspxml.OutOfElem();

	if (!Rspxml.FindElem("WatermarkAlarmList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get WaterAlarm List ParseXML fail", "no WatermarkAlarmList element in rsPxml");
		return IVS_XML_INVALID;
	}
	Rspxml.IntoElem(); 
	IVS_UINT32 iIndex = 0;
	IVS_CHAR cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	//追加的域编码
	std::string strAppendDomainCode;
	strAppendDomainCode.clear();
	if (!strDoamianCode.empty())
	{
		strAppendDomainCode.append("#");
		strAppendDomainCode.append(strDoamianCode);
	}
	std::string strDevCode;
	do 
	{
		if (!Rspxml.FindElemValue("WatermarkAlarmInfo"))
		{
			break;
		}
		Rspxml.IntoElem();
		IVS_WATERMARK_ALARM_INFO &stAlarmInfo = pWaterAlarmInfoList->stWatermarkAlarmInfo[iIndex];
		memset(&stAlarmInfo, 0 ,sizeof(IVS_WATERMARK_ALARM_INFO));
		memset(cDevCode,0,IVS_DEV_CODE_LEN + 1);              
		
		GET_ELEM_VALUE_CHAR("CheckoutOperator", pTemp, stAlarmInfo.cCheckoutOperator ,IVS_NAME_LEN, Rspxml);

		GET_ELEM_VALUE_NUM_FOR_UINT64("AlarmEventID", pTemp, stAlarmInfo.ullAlarmEventID,Rspxml); 

		GET_ELEM_VALUE_CHAR("AlarmType", pTemp, stAlarmInfo.cAlarmType , IVS_ALARM_TYPE_LEN, Rspxml);        

		//对设备编码需要添加上域编码返回个CU
		GET_ELEM_VALUE_CHAR("CameraCode", pTemp, cDevCode , IVS_DEV_CODE_LEN, Rspxml);
		strDevCode = cDevCode;
		strDevCode.append(strAppendDomainCode);
		memcpy(stAlarmInfo.cCameraCode,strDevCode.c_str(),strDevCode.size());

		GET_ELEM_VALUE_CHAR("OccurStartTime", pTemp, stAlarmInfo.cOccurStartTime ,IVS_TIME_LEN, Rspxml); 
 
		GET_ELEM_VALUE_CHAR("OccurStopTime", pTemp, stAlarmInfo.cOccurStopTime , IVS_TIME_LEN, Rspxml);  

		GET_ELEM_VALUE_NUM_FOR_UINT("WatermarkAlarmStatus", pTemp, stAlarmInfo.uiWatermarkAlarmStatus,Rspxml); 

		GET_ELEM_VALUE_CHAR("ConfirmOperator", pTemp, stAlarmInfo.cConfirmOperator, IVS_NAME_LEN, Rspxml);    
	
		GET_ELEM_VALUE_CHAR("ConfirmTime", pTemp,  stAlarmInfo.cConfirmTime, IVS_TIME_LEN, Rspxml); 

		GET_ELEM_VALUE_CHAR("ConfirmInfo", pTemp,stAlarmInfo.cConfirmInfo, IVS_WATERMARK_ALARM_DESCRIPTION_LEN, Rspxml);

		Rspxml.OutOfElem();

		iIndex++;
	} while (Rspxml.NextElem() && iIndex < iReqNum);

	return IVS_SUCCEED;
}

// 告警级别拼接xml
IVS_INT32 CAlarmMgrXMLProcess::AlarmLevelGetXML(const IVS_ALARM_LEVEL *pAlarmLevel, IVS_INT32 iReqType, CXml &xml)
{
	CHECK_POINTER(pAlarmLevel, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	char pointer[POINTER_MAX_NUM] = {0};
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("AlarmLevel");
	{
		(void)xml.IntoElem();
		if (0 == iReqType)
		{
			(void)xml.AddChildElem("AlarmLevelName");
			(void)xml.IntoElem();
			if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmLevel->cName, IVS_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel->cName to pointer fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(pointer);
		}
		if (1 == iReqType)
		{
			(void)xml.AddChildElem("AlarmLevelID");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmLevel->uiAlarmLevelId)).c_str());

			(void)xml.AddElem("AlarmLevelName");
			(void)xml.IntoElem();
			if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmLevel->cName, IVS_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel->cName to pointer fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(pointer);
		}
		memset(pointer, 0, POINTER_MAX_NUM);

		(void)xml.AddElem("AlarmLevelValue");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmLevel->uiValue)).c_str());

		(void)xml.AddElem("AlarmLevelColor");
		(void)xml.IntoElem();
		if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, pAlarmLevel->cColor, ALARM_COLOR_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel->cColor to pointer fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(pointer);
		memset(pointer, 0, POINTER_MAX_NUM);

		(void)xml.AddElem("AlarmLevelDesc");
		(void)xml.IntoElem();
		IVS_CHAR cAlarmLevelDesc[IVS_ALARM_DESCRIPTION_LEN + 1];
		memset(cAlarmLevelDesc, 0x0, IVS_ALARM_DESCRIPTION_LEN + 1);
		if (!CToolsHelp::Memcpy(cAlarmLevelDesc, IVS_ALARM_DESCRIPTION_LEN, pAlarmLevel->cDescription, IVS_ALARM_DESCRIPTION_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmLevel->cDescription to cAlarmLevelDesc fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cAlarmLevelDesc);

		xml.OutOfElem();	
	}
	return IVS_SUCCEED;
}

// 新增告警级别解析xml
IVS_INT32 CAlarmMgrXMLProcess::AddAlarmLevelPraseXML(IVS_UINT32* pAlarmLevelID,CXml &xml)
{
	CHECK_POINTER(pAlarmLevelID, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "fail is no 'Content' elemenet", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.IntoElem();
	if (!xml.FindElem("AlarmLevel"))
	{
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.IntoElem();

	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelID",pTemp,*pAlarmLevelID,xml);

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 删除告警级别拼接XML
IVS_INT32 CAlarmMgrXMLProcess::DeleteAlarmLevelGetXML(const IVS_UINT32 uiAlarmLevelId, CXml &xml)
{
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	
	(void)xml.AddChildElem("AlarmLevel");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmLevelID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(uiAlarmLevelId).c_str());//lint !e713
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询告警级别解析XML
IVS_INT32 CAlarmMgrXMLProcess::GetAlarmTypeLevelPraseXML(IVS_UINT32 uiAlarmLevelNum,IVS_UINT32* pAlarmLevelNum,IVS_ALARM_LEVEL* pAlarmLevel,CXml &xml)
{
	CHECK_POINTER(pAlarmLevelNum, IVS_PARA_INVALID);
	CHECK_POINTER(pAlarmLevel, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID,  "fail is no 'Content' elemenet", "NA");
		return IVS_SMU_XML_INVALID;
	}
	(void)xml.IntoElem();
	if (!xml.FindElem("AlarmLevelList"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID,  "fail is no 'AlarmLevelList' elemenet", "NA");
		return IVS_SMU_XML_INVALID;
	}
	(void)xml.IntoElem();

	const char* pTemp = NULL;
	IVS_INT32 i = 0;

	if (!xml.FindElem("AlarmLevel"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID,  "fail is no 'AlarmLevel' elemenet", "NA");
		return IVS_SMU_XML_INVALID;
	}

	do 
	{
		(void)xml.IntoElem();
		
		GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelID", pTemp, pAlarmLevel[i].uiAlarmLevelId, xml);
		GET_ELEM_VALUE_CHAR("AlarmLevelName",pTemp,pAlarmLevel[i].cName,IVS_NAME_LEN,xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelValue",pTemp,pAlarmLevel[i].uiValue,xml);
		GET_ELEM_VALUE_CHAR("AlarmLevelColor",pTemp,pAlarmLevel[i].cColor,IVS_ALARM_LEVEL_COLOR_LEN,xml);

		GET_ELEM_VALUE_CHAR("AlarmLevelDesc",pTemp,pAlarmLevel[i].cDescription,IVS_ALARM_DESCRIPTION_LEN,xml);
		GET_ELEM_VALUE_NUM("IsUserDefined",pTemp,pAlarmLevel[i].bIsUserDefined,xml);
		i++;
		xml.OutOfElem();
	} while (xml.NextElem() && i < static_cast<IVS_INT32>(uiAlarmLevelNum));

	*pAlarmLevelNum = static_cast<IVS_UINT32>(i);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询告警级别详细信息拼接XML
IVS_INT32 CAlarmMgrXMLProcess::GetAlarmLevelGetXML(IVS_UINT32 uiAlarmLevelID, CXml& xml)
{
	IVS_DEBUG_TRACE("");

	(void)xml.AddElem("Content");
	
	(void)xml.AddChildElem("AlarmLevel");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmLevelID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiAlarmLevelID)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询告警级别详细信息解析XML
IVS_INT32 CAlarmMgrXMLProcess::GetAlarmLevelPraseXML(IVS_ALARM_LEVEL* pAlarmLevel, CXml& xml)
{
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID,  "fail is no 'Content' elemenet", "NA");
		return IVS_SMU_XML_INVALID;
	}
	(void)xml.IntoElem();
	if (!xml.FindElem("AlarmLevel"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID,  "fail is no 'AlarmLevel' elemenet", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.IntoElem();

	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelID",pTemp,pAlarmLevel->uiAlarmLevelId,xml);
	GET_ELEM_VALUE_CHAR("AlarmLevelName",pTemp,pAlarmLevel->cName,IVS_NAME_LEN,xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelValue",pTemp,pAlarmLevel->uiValue,xml);
	GET_ELEM_VALUE_CHAR("AlarmLevelColor",pTemp,pAlarmLevel->cColor,ALARM_COLOR_LEN,xml);
	GET_ELEM_VALUE_CHAR("AlarmLevelDesc",pTemp,pAlarmLevel->cDescription,IVS_ALARM_DESCRIPTION_LEN,xml);
	GET_ELEM_VALUE_NUM("IsUserDefined",pTemp,pAlarmLevel->bIsUserDefined,xml);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 告警帮助拼接XML
IVS_INT32 CAlarmMgrXMLProcess::HelpInfoGetXML(const IVS_HELP_INFO *helpInfo, CXml &xml)
{
	CHECK_POINTER(helpInfo, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	char* pointer = IVS_NEW(pointer, IVS_MAX_HELP_LEN + 1);
	CHECK_POINTER(pointer, IVS_OPERATE_MEMORY_ERROR);
	memset(pointer, 0, IVS_MAX_HELP_LEN + 1);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	
	(void)xml.AddChildElem("HelpInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("SourceID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(helpInfo->uiSourceId)).c_str());

	(void)xml.AddElem("SourceType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(helpInfo->uiSourceType)).c_str());

	(void)xml.AddElem("HelpInfo");
	(void)xml.IntoElem();
	if (!CToolsHelp::Memcpy(pointer, IVS_MAX_HELP_LEN, helpInfo->cHelpInfo, IVS_MAX_HELP_LEN))
	{
		IVS_DELETE(pointer, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy helpInfo->cHelpInfo to pointer failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);

	xml.OutOfElem();
	IVS_DELETE(pointer, MUILI);
	return IVS_SUCCEED;
}

// 删除告警帮助拼接XML
IVS_INT32 CAlarmMgrXMLProcess::DeleteHelpInfoGetXML(const IVS_UINT32 uiSourceId,IVS_UINT32 uiSourceType, CXml &xml)
{
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	
	(void)xml.AddChildElem("HelpInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("SourceID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiSourceId)).c_str());

	(void)xml.AddElem("SourceType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiSourceType)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询帮助信息拼接XML
IVS_INT32 CAlarmMgrXMLProcess::QueryHelpInfoGetXML(IVS_UINT32 uiAlarmHelpSourceType,IVS_UINT32 uiAlarmHelpSourceId,CXml &xml)
{	
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("HelpInfo");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("SourceID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiAlarmHelpSourceId)).c_str());

	(void)xml.AddElem("SourceType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiAlarmHelpSourceType)).c_str());

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询帮助信息解析XML
IVS_INT32 CAlarmMgrXMLProcess::QueryHelpInfoPraseXML(IVS_HELP_INFO *helpInfo, CXml &xml)
{
	CHECK_POINTER(helpInfo, IVS_FAIL); 
	IVS_DEBUG_TRACE("");
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "fail is no 'Content' elemenet", "NA");
		return IVS_SMU_XML_INVALID;
	}
	xml.IntoElem();
	if (!xml.FindElem("HelpInfo"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "fail is no 'HelpInfo' elemenet", "NA");
		return IVS_SMU_XML_INVALID;
	}
	xml.IntoElem();
	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("SourceType", pTemp, helpInfo->uiSourceType, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("SourceID", pTemp, helpInfo->uiSourceId, xml);
	GET_ELEM_VALUE_CHAR("SourceName",pTemp,helpInfo->cSourceName,IVS_ALARM_NAME_LEN,xml);
	GET_ELEM_VALUE_CHAR("HelpInfo", pTemp, helpInfo->cHelpInfo,IVS_MAX_HELP_LEN, xml);
	GET_ELEM_VALUE_CHAR("UserName",pTemp,helpInfo->cUserName,IVS_NAME_LEN,xml);
	GET_ELEM_VALUE_CHAR("UpdateTime",pTemp,helpInfo->cUpdateTime,IVS_TIME_LEN,xml);

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询告警帮助信息正文拼接XML
IVS_INT32 CAlarmMgrXMLProcess::QueryHelpInfoTextGetXML(const IVS_CHAR* pAlarmInCode, const IVS_CHAR* pAlarmType, CXml &xml)
{
    CHECK_POINTER(pAlarmInCode, IVS_PARA_INVALID);
    CHECK_POINTER(pAlarmType, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("AlarmInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("AlarmInCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pAlarmInCode);
    (void)xml.AddElem("AlarmType");
    /*(void)xml.IntoElem();*/
    (void)xml.SetElemValue(pAlarmType);
    return IVS_SUCCEED;
}

// 设置告警类型拼接XML
IVS_INT32 CAlarmMgrXMLProcess::SetAlarmTypeLevelGetXML(const IVS_ALARM_TYPE_LIST* pAlarmTypeList, CXml& xml)
{
	CHECK_POINTER(pAlarmTypeList,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("AlarmTypeList");
	(void)xml.IntoElem();

	for (unsigned int i=0;i<pAlarmTypeList->iTotal;i++)
	{
		const IVS_ALARM_TYPE& stAlarmType = pAlarmTypeList->stSubscribeInfo[i];
		if (0 == i)
		{
			(void)xml.AddChildElem("AlarmTypeInfo");
			(void)xml.IntoElem();
		}
		else
		{
			(void)xml.AddElem("AlarmTypeInfo");
		}
		(void)xml.AddChildElem("AlarmTypeID");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stAlarmType.uiID).c_str());
		(void)xml.AddElem("AlarmLevelID");
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stAlarmType.stAlarmLevel.uiAlarmLevelId).c_str());
		xml.OutOfElem();
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询自定义告警类型列表解析xml
IVS_INT32 CAlarmMgrXMLProcess::GetAlarmTypeListParseXML(IVS_ALARM_TYPE_LIST* pAlarmTypeList, CXml &xml, IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pAlarmTypeList, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElem(Content) is fail", "NA");
		return IVS_SMU_XML_INVALID;
	}   
	xml.IntoElem();
	if (!xml.FindElem("PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElem(PageInfo) is fail", "NA");
		return IVS_SMU_XML_INVALID;
	}
	xml.IntoElem();
	const char* pTemp = NULL;
	IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_NUM("FromIndex", pTemp, iTemp, xml);//lint !e732
	pAlarmTypeList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iTemp);
	iTemp = 0;
	GET_ELEM_VALUE_NUM("ToIndex", pTemp, iTemp, xml);//lint !e732
	pAlarmTypeList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iTemp);
	iTemp = 0;
	GET_ELEM_VALUE_NUM("RealNum", pTemp, iTemp , xml);//lint !e734 !e732
	pAlarmTypeList->iTotal = static_cast<IVS_UINT32>(iTemp);

	xml.OutOfElem();	
	// 如果查到没有数据 直接就返回成功了
	if (0 == pAlarmTypeList->iTotal)   //unsigned int 不会小于0
	{
		BP_RUN_LOG_INF("0 == pAlarmTypeList->iTotal", "NA");
		return IVS_SUCCEED;
	}

	if (!xml.FindElem("AlarmTypeList"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElem(AlarmTypeList)", "NA");
		return IVS_SMU_XML_INVALID;
	}
	xml.IntoElem();
	if (!xml.FindElem("AlarmTypeInfo"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElem(AlarmTypeInfo)", "NA");
		return IVS_SMU_XML_INVALID;
	}

	IVS_UINT32 i = 0;
	IVS_UINT32 iRspSize = (pAlarmTypeList->stIndexRange.uiToIndex - pAlarmTypeList->stIndexRange.uiFromIndex) + 1;
	IVS_UINT32 iRspNeedBuf = (iRspSize - 1)*sizeof(IVS_ALARM_TYPE) + sizeof(IVS_ALARM_TYPE_LIST);
	// 如果要分配的内存小于需要的内存
	if (uiBufferSize < iRspNeedBuf)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	while(i < iRspSize)//lint !e734 !e737 !e574
	{
		IVS_ALARM_TYPE &pAlarmTypeInfo = pAlarmTypeList->stSubscribeInfo[i];
		xml.IntoElem();
		const char* szElemValue = NULL;
		GET_ELEM_VALUE_NUM_FOR_UINT("AlarmTypeID",szElemValue, pAlarmTypeInfo.uiID, xml);
		GET_ELEM_VALUE_CHAR("AlarmType",szElemValue, pAlarmTypeInfo.cType,(IVS_ALARM_CODE_LEN), xml);
		GET_ELEM_VALUE_CHAR("AlarmTypeName",szElemValue, pAlarmTypeInfo.cName,(IVS_ALARM_NAME_LEN), xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("AlarmInType",szElemValue, pAlarmTypeInfo.uiType, xml);
		GET_ELEM_VALUE_CHAR("AlarmCategory",szElemValue,pAlarmTypeInfo.cCategory,(IVS_ALARM_TYPE_CATEGORY_LEN), xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelID",szElemValue, pAlarmTypeInfo.stAlarmLevel.uiAlarmLevelId, xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelValue",szElemValue, pAlarmTypeInfo.stAlarmLevel.uiValue, xml);
		GET_ELEM_VALUE_CHAR("AlarmLevelName",szElemValue,pAlarmTypeInfo.stAlarmLevel.cName,(IVS_NAME_LEN),xml);
		GET_ELEM_VALUE_NUM("IsUserDefined",szElemValue,pAlarmTypeInfo.stAlarmLevel.bIsUserDefined, xml);
		xml.OutOfElem();
		if (!xml.NextElemEx(1))
		{
			break;
		}
		i++;
	}    
	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;    
}

// 告警用户认证拼接XML
IVS_INT32 CAlarmMgrXMLProcess::UserAlarmAuthorGetXML(const IVS_CHAR *pPasswordInfo, CXml &xml)
{
	CHECK_POINTER(pPasswordInfo, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("LoginPWD");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pPasswordInfo);

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 告警上报拼接XML
IVS_INT32 CAlarmMgrXMLProcess::GetAlarmNotifyGetXML(const IVS_ALARM_NOTIFY* palarmNotify,CXml& xml)
{
	IVS_DEBUG_TRACE("");
	char pointer[POINTER_MAX_NUM] = {0};
	memset(pointer, 0, POINTER_MAX_NUM);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
 	(void)xml.IntoElem();
	
	(void)xml.AddChildElem("AlarmNotify");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmEventId");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<int>(palarmNotify->ullAlarmEventID)).c_str());

	(void)xml.AddElem("AlarmInCode");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cAlarmInCode, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cAlarmInCode to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("AlarmInType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(palarmNotify->uiAlarmInType)).c_str());
	(void)xml.AddElem("AlarmInName");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cAlarmInName, IVS_ALARM_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cAlarmInName to fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("AlarmLevelValue");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(palarmNotify->uiAlarmLevelValue)).c_str());
	(void)xml.AddElem("AlarmLevelName");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cAlarmLevelName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cAlarmLevelName to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("AlarmLevelColor");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cAlarmLevelColor to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("AlarmType");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cAlarmType, IVS_ALARM_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cAlarmType to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("AlarmCategory");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cAlarmCategory, IVS_ALARM_CATEGORY_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cAlarmCategory to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("OccurTime");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cOccurTime, IVS_TIME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cOccurTime to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("AlarmStatus");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(palarmNotify->uiAlarmStatus)).c_str());
	(void)xml.AddElem("OccurNumber");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(palarmNotify->uiOccurNumber)).c_str());

	(void)xml.AddElem("IsCommission");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(palarmNotify->bIsCommission).c_str());
	(void)xml.AddElem("AlarmDesc");
	IVS_CHAR cAlarmDesc[IVS_ALARM_DESCRIPTION_LEN + 1];
	memset(cAlarmDesc, 0, IVS_ALARM_DESCRIPTION_LEN + 1);
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cAlarmDesc to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmDesc);

	(void)xml.AddElem("PreviewUrl");
	if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, palarmNotify->cPreviewUrl, IVS_URL_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy palarmNotify->cPreviewUrl to pointer fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(pointer);
	memset(pointer, 0, POINTER_MAX_NUM);
	(void)xml.AddElem("ExistsRecord");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(palarmNotify->bExistsRecord).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询告警类型列表拼接xml
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmTypeListGetXML(const IVS_CHAR* pLoginID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, CXml& xml)
 {
     CHECK_POINTER(pLoginID, IVS_OPERATE_MEMORY_ERROR);  
     CHECK_POINTER(pUnifiedQuery,IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	 char pointer[POINTER_MAX_NUM] = {0};
	 memset(pointer, 0, POINTER_MAX_NUM);

     (void)xml.AddDeclaration("1.0","UTF-8","");
     (void)xml.AddElem("Content");
     
     (void)xml.AddChildElem("PageInfo");
 	 (void)xml.IntoElem();
     (void)xml.AddChildElem("FromIndex");
     (void)xml.IntoElem();
     (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pUnifiedQuery->stIndex.uiFromIndex))).c_str());
     (void)xml.AddElem("ToIndex");
     (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pUnifiedQuery->stIndex.uiToIndex))).c_str());
 
     (void)xml.AddElem("OrderCond");
     (void)xml.AddChildElem("OrderField");
     (void)xml.IntoElem();	
     std::string iOrderField = QUERYFILED.GetValue(pUnifiedQuery->stOrderCond.eFieID);
     (void)xml.SetElemValue(iOrderField.c_str());
     (void)xml.AddElem("Order");
     const char* iOrder = NULL;
     if (1 == pUnifiedQuery->stOrderCond.bUp)
     {
         iOrder = "UP";
     }
     else if (0 == pUnifiedQuery->stOrderCond.bUp)
     {
         iOrder = "DOWN";
     }
     else
     {
		 BP_RUN_LOG_ERR(IVS_FAIL, "the value of pUnifiedQuery->stOrderCond.bUp illegal", "NA");
         return IVS_FAIL;
     }
     (void)xml.SetElemValue(iOrder);
     xml.OutOfElem();
     IVS_UINT32 iFieldNum = static_cast<IVS_UINT32>(pUnifiedQuery->iFieldNum);		 
     (void)xml.AddElem("QueryCond");	
     (void)xml.IntoElem();
     for (unsigned int i=0;i<iFieldNum;i++)
     {
         const IVS_QUERY_FIELD &stQueryFieid = pUnifiedQuery->stQueryField[i];
         i==0?(void)xml.AddChildElem("QueryField"):(void)xml.AddElem("QueryField");		
         (void)xml.IntoElem();
         (void)xml.AddChildElem("Field");
         (void)xml.IntoElem();
         std::string  iField = QUERYFILED.GetValue(stQueryFieid.eFieID);
         (void)xml.SetElemValue(iField.c_str());
         (void)xml.AddElem("Value");
		 if (!CToolsHelp::Memcpy(pointer, POINTER_MAX_NUM, stQueryFieid.cValue, IVS_DESCRIBE_LEN))
		 {
			 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy stQueryFieid.cValue to pointer fail", "NA");
			 return IVS_ALLOC_MEMORY_ERROR;
		 }
         (void)xml.SetElemValue(pointer);
		 memset(pointer, 0, POINTER_MAX_NUM);
         (void)xml.AddElem("QueryType");
         const char* iQueryType = NULL;
         if (1 == stQueryFieid.bExactQuery)
         {
             iQueryType = "EXACT";
         }
         else if (0 == stQueryFieid.bExactQuery)
         {
             iQueryType = "INEXACT";
         }
         else
         {
			 BP_RUN_LOG_ERR(IVS_FAIL, "the value of stQueryFieid.bExactQuery illegal", "NA");
             return IVS_FAIL;
		 }
         (void)xml.SetElemValue(iQueryType);
         xml.OutOfElem();
     }
     xml.OutOfElem();
     return IVS_SUCCEED;
 }

 // 拼装删除和查询布防计划xml
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmAreaPlanXML(const IVS_CHAR* pDomainCode, IVS_UINT32& uiAlarmAreaID, CXml& xml)
 {
	IVS_DEBUG_TRACE("");

	 (void)xml.AddDeclaration("1.0","UTF-8","");

	 (void)xml.AddElem("Content");
	 (void)xml.AddChildElem("DomainCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(pDomainCode);

	 (void)xml.AddElem("Plan");
	 (void)xml.IntoElem();
	 (void)xml.AddChildElem("AlarmAreaID");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiAlarmAreaID)).c_str());
	 xml.OutOfElem();

	 return IVS_SUCCEED;
 }

 
 // 查询告警历史信息列表解析XML
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmListParseXML(IVS_ALARM_EVENT_LIST* pAlarmEventList, IVS_UINT32 uiBufferSize, CXml &xml, const std::string& strDomainCode)
 {
	 CHECK_POINTER(pAlarmEventList, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	 if (!xml.FindElem("Content"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "xml.FindElem(Content) is fail", "NA");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }
	 (void)xml.IntoElem();
	 if (!xml.FindElem("PageInfo"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "xml.FindElem(PageInfo) is fail", "NA");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }
	 xml.IntoElem();
	 const char* pTemp = NULL;
	 IVS_INT32 iTotal = 0;
	 IVS_INT32 iFromIndex = 0;
	 IVS_INT32 iToIndex = 0;
	 GET_ELEM_VALUE_NUM("RealNum", pTemp, iTotal , xml);
	 GET_ELEM_VALUE_NUM("FromIndex", pTemp, iFromIndex, xml);
	 GET_ELEM_VALUE_NUM("ToIndex",pTemp, iToIndex , xml);

	 if (iTotal < 0 || iToIndex < 0 || iFromIndex < 0 || iFromIndex > iToIndex)
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "pAlarmEventList->stIndexRange and iTotal are error", "NA");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }

	 pAlarmEventList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromIndex);
	 pAlarmEventList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToIndex);
	 pAlarmEventList->iTotal = static_cast<IVS_UINT32>(iTotal);
	 xml.OutOfElem();
	 if (0 == iTotal)
	 {
		 BP_RUN_LOG_INF("iTotal == 0", "NA");
		 return IVS_SUCCEED;
	 }
	 if (!xml.FindElem("AlarmList"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AlarmList) is fail", "NA");
		 return IVS_XML_INVALID;
	 }

	 //响应消息中传回来的页数
	 IVS_UINT32 uiResNum = ((pAlarmEventList->stIndexRange).uiToIndex - (pAlarmEventList->stIndexRange).uiFromIndex) + 1;
	 //实际要用到的内存大小
	 IVS_UINT32 uiResBufSize = sizeof(IVS_ALARM_EVENT_LIST) + ((uiResNum) - 1) * sizeof(IVS_ALARM_EVENT);
	 //实际需要内存与分配内存大小的比较
	 if (uiBufferSize < uiResBufSize)
	 {
		 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
		 return IVS_ALLOC_MEMORY_ERROR;
	 }
	 IVS_UINT32 i = 0;	
	 (void)xml.IntoElem();
	 if (!xml.FindElem("AlarmInfo"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AlarmInfo) is fail", "NA");
		 return IVS_XML_INVALID;
	 } 
	 while(i < uiResNum)
	 {
		 IVS_ALARM_EVENT &pAlarmEvent = pAlarmEventList->stSubscribeInfo[i];
		 xml.IntoElem();
		 const char* szElemValue = NULL;
		 IVS_INT32 iTemp = 0;
		 GET_ELEM_VALUE_NUM("AlarmEventID", szElemValue,  iTemp, xml);
		 pAlarmEvent.stAlarmNotify.ullAlarmEventID = static_cast<IVS_UINT64>(iTemp);//lint !e571

		 GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, pAlarmEvent.stAlarmNotify.cAlarmInCode, IVS_ALARM_CODE_LEN, xml);
		 if (!CToolsHelp::Memcpy(pAlarmEvent.stAlarmNotify.cDevDomainCode, IVS_DOMAIN_CODE_LEN, strDomainCode.c_str(), strDomainCode.size()))
		 {
			 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy strDomainCode to pAlarmEvent.stAlarmNotify.cDevDomainCode failed", "NA");
			 return IVS_ALLOC_MEMORY_ERROR;
		 }

		 GET_ELEM_VALUE_NUM("AlarmInType", szElemValue, iTemp, xml);
		 pAlarmEvent.stAlarmNotify.uiAlarmInType = static_cast<IVS_UINT32>(iTemp);
		 GET_ELEM_VALUE_CHAR("AlarmInName", szElemValue, pAlarmEvent.stAlarmNotify.cAlarmInName, IVS_ALARM_NAME_LEN, xml);
		 GET_ELEM_VALUE_NUM("AlarmLevelValue", szElemValue, iTemp, xml);
		 pAlarmEvent.stAlarmNotify.uiAlarmLevelValue = static_cast<IVS_UINT32>(iTemp);
		 GET_ELEM_VALUE_CHAR("AlarmLevelName", szElemValue, pAlarmEvent.stAlarmNotify.cAlarmLevelName, IVS_NAME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmLevelColor", szElemValue, pAlarmEvent.stAlarmNotify.cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmType", szElemValue, pAlarmEvent.stAlarmNotify.cAlarmType, IVS_ALARM_CODE_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmCategory", szElemValue, pAlarmEvent.stAlarmNotify.cAlarmCategory, IVS_ALARM_CATEGORY_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OccurTime", szElemValue, pAlarmEvent.stAlarmNotify.cOccurTime, IVS_TIME_LEN, xml);
		 GET_ELEM_VALUE_NUM_FOR_UINT("OccurNumber", szElemValue, pAlarmEvent.stAlarmNotify.uiOccurNumber, xml);
		 GET_ELEM_VALUE_NUM("AlarmStatus", szElemValue, iTemp, xml);
		 pAlarmEvent.stAlarmNotify.uiAlarmStatus = static_cast<IVS_UINT32>(iTemp);
		 GET_ELEM_VALUE_CHAR("Operator", szElemValue, pAlarmEvent.stOperateInfo.cOperatorName, IVS_NAME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OperateInfo", szElemValue, pAlarmEvent.stOperateInfo.cOperateInfo, IVS_ALARM_DESCRIPTION_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OperateTime", szElemValue, pAlarmEvent.stOperateInfo.cOperateTime, IVS_TIME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmDesc", szElemValue, pAlarmEvent.stAlarmNotify.cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, xml);
		 GET_ELEM_VALUE_NUM("ExistsRecord", szElemValue, pAlarmEvent.stAlarmNotify.bExistsRecord, xml);
		 GET_ELEM_VALUE_CHAR("PreviewUrl", szElemValue, pAlarmEvent.stAlarmNotify.cPreviewUrl, IVS_URL_LEN, xml);

		 xml.OutOfElem();
		 (void)xml.NextElem();
		 i++;
	 }    
	 xml.OutOfElem();

	 return IVS_SUCCEED;

 }

 // 告警状态上报
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmStatusGetXML(const IVS_ALARM_EVENT* pAlarmEvent, CXml& xml)
 {
	 CHECK_POINTER(pAlarmEvent, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("AlarmNotify");
	(void)xml.IntoElem();
	{
		(void)xml.AddChildElem("AlarmEventId");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.ullAlarmEventID)).c_str());

		(void)xml.AddElem("AlarmInCode");
		(void)xml.IntoElem();
		IVS_CHAR cAlarmInCode[IVS_ALARM_CODE_LEN + 1];
		memset(cAlarmInCode, 0x0, IVS_ALARM_CODE_LEN + 1);
		if (!CToolsHelp::Memcpy(cAlarmInCode, IVS_ALARM_CODE_LEN, pAlarmEvent->stAlarmNotify.cAlarmInCode, IVS_ALARM_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmInCode to cAlarmInCode failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cAlarmInCode);

		(void)xml.AddElem("AlarmType");
		(void)xml.IntoElem();
		IVS_CHAR cAlarmType[IVS_ALARM_CODE_LEN + 1];
		memset(cAlarmType, 0x0, IVS_ALARM_CODE_LEN + 1);
		if (!CToolsHelp::Memcpy(cAlarmType, IVS_ALARM_CODE_LEN, pAlarmEvent->stAlarmNotify.cAlarmType, IVS_ALARM_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stAlarmNotify.cAlarmType to cAlarmType failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cAlarmType);

		(void)xml.AddElem("AlarmStatus");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.uiAlarmStatus)).c_str());

		(void)xml.AddElem("OccurNumber");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stAlarmNotify.uiOccurNumber)).c_str());
	}
	xml.OutOfElem();

	(void)xml.AddElem("AlarmOperateInfo");
	(void)xml.IntoElem();
	{	
		// const IVS_CHAR* pOperatorId = "";
        string strOperatorId;
		if (0 != pAlarmEvent->stOperateInfo.uiOperatorID)
		{
			strOperatorId = CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmEvent->stOperateInfo.uiOperatorID)).c_str();
		}


		(void)xml.AddChildElem("OperatorId");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(strOperatorId.c_str());

		(void)xml.AddElem("OperatorName");
		(void)xml.IntoElem();
		IVS_CHAR cOperatorName[IVS_NAME_LEN + 1];
		memset(cOperatorName, 0x0, IVS_NAME_LEN + 1);
		if (!CToolsHelp::Memcpy(cOperatorName, IVS_NAME_LEN, pAlarmEvent->stOperateInfo.cOperatorName, IVS_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy  pAlarmEvent->stOperateInfo.cOperatorName cOperatorName failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cOperatorName);

		(void)xml.AddElem("OperateTime");
		(void)xml.IntoElem();
		IVS_CHAR cOperateTime[IVS_TIME_LEN + 1];
		memset(cOperateTime, 0x0, IVS_TIME_LEN + 1);
		if (!CToolsHelp::Memcpy(cOperateTime, IVS_TIME_LEN, pAlarmEvent->stOperateInfo.cOperateTime, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stOperateInfo.cOperateTime to cOperateTime failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cOperateTime);

		(void)xml.AddElem("Reserve");
		(void)xml.IntoElem();
		IVS_CHAR cReserve[ALARM_RESERVER_LEN + 1];
		memset(cReserve, 0x0, ALARM_RESERVER_LEN + 1);
		if (!CToolsHelp::Memcpy(cReserve, ALARM_RESERVER_LEN, pAlarmEvent->stOperateInfo.cReserver, ALARM_RESERVER_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stOperateInfo.cReserver to cReserve failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cReserve);

		(void)xml.AddElem("OperateInfo");
		(void)xml.IntoElem();
		IVS_CHAR cOperateInfo[IVS_ALARM_DESCRIPTION_LEN + 1];
		memset(cOperateInfo, 0x0, IVS_ALARM_DESCRIPTION_LEN + 1);
		if (!CToolsHelp::Memcpy(cOperateInfo, IVS_ALARM_DESCRIPTION_LEN, pAlarmEvent->stOperateInfo.cOperateInfo, IVS_ALARM_DESCRIPTION_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pAlarmEvent->stOperateInfo.cOperateInfo to cOperateInfo failed", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cOperateInfo);
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
 }

 // 查询告警信息事件拼接XML
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmInfoGetXML(IVS_UINT64 ullAlarmEventID, CXml &xml)
 {
	 IVS_DEBUG_TRACE("");
	 (void)xml.AddElem("Content");
	 
	 (void)xml.AddChildElem("AlarmInfo");
	 (void)xml.IntoElem();
	 (void)xml.AddChildElem("AlarmEventID");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(ullAlarmEventID)).c_str());
	 xml.OutOfElem();

	 xml.OutOfElem();
	 
	 return IVS_SUCCEED;
 }

 // 查询告警信息事件解析XML
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmInfoParseXML(IVS_ALARM_EVENT* pAlarmEvent, CXml &xml, const IVS_CHAR* pDomainCode)
 {
	 CHECK_POINTER(pAlarmEvent, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	 if (!xml.FindElem("Content"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "!xml.FindElem(Content)", "NA");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }
	 (void)xml.IntoElem();

	 if (!xml.FindElem("AlarmInfo"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "!xml.FindElem(AlarmInfo)", "NA");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }
	 (void)xml.IntoElem();
	 const char* pTemp = NULL;
	 {
		 IVS_INT32 iTemp = 0;
		 GET_ELEM_VALUE_NUM("AlarmEventID", pTemp, iTemp, xml);
		 pAlarmEvent->stAlarmNotify.ullAlarmEventID = static_cast<IVS_UINT64>(iTemp); //lint !e571

		 GET_ELEM_VALUE_CHAR("AlarmInCode", pTemp, pAlarmEvent->stAlarmNotify.cAlarmInCode, IVS_ALARM_CODE_LEN, xml);
		 if (!CToolsHelp::Memcpy(pAlarmEvent->stAlarmNotify.cDevDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, strlen(pDomainCode)))
		 {
			 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDomainCode to pAlarmEvent->stAlarmNotify.cDevDomainCode fail", "NA");
			 return IVS_ALLOC_MEMORY_ERROR;
		 }
		 GET_ELEM_VALUE_NUM("AlarmInType", pTemp, iTemp, xml);
		 pAlarmEvent->stAlarmNotify.uiAlarmInType = static_cast<IVS_UINT32>(iTemp);

		 GET_ELEM_VALUE_CHAR("AlarmInName", pTemp, pAlarmEvent->stAlarmNotify.cAlarmInName, IVS_ALARM_NAME_LEN, xml);
		 GET_ELEM_VALUE_NUM("AlarmLevelValue", pTemp, iTemp, xml);
		 pAlarmEvent->stAlarmNotify.uiAlarmLevelValue = static_cast<IVS_UINT32>(iTemp);

		 GET_ELEM_VALUE_CHAR("AlarmLevelName", pTemp, pAlarmEvent->stAlarmNotify.cAlarmLevelName, IVS_NAME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmLevelColor", pTemp, pAlarmEvent->stAlarmNotify.cAlarmLevelColor, IVS_ALARM_LEVEL_COLOR_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmType", pTemp, pAlarmEvent->stAlarmNotify.cAlarmType, IVS_ALARM_CODE_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmCategory", pTemp, pAlarmEvent->stAlarmNotify.cAlarmCategory, IVS_ALARM_CATEGORY_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OccurTime", pTemp, pAlarmEvent->stAlarmNotify.cOccurTime, IVS_TIME_LEN, xml);
		 GET_ELEM_VALUE_NUM_FOR_UINT("OccurNumber", pTemp, pAlarmEvent->stAlarmNotify.uiOccurNumber, xml);
		 GET_ELEM_VALUE_NUM("AlarmStatus", pTemp, iTemp, xml);
		 pAlarmEvent->stAlarmNotify.uiAlarmStatus = static_cast<IVS_UINT32>(iTemp);

		 GET_ELEM_VALUE_CHAR("Operator", pTemp, pAlarmEvent->stOperateInfo.cOperatorName, IVS_NAME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OperateInfo", pTemp, pAlarmEvent->stOperateInfo.cOperateInfo, IVS_ALARM_DESCRIPTION_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OperateTime", pTemp, pAlarmEvent->stOperateInfo.cOperateTime, IVS_TIME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmDesc", pTemp, pAlarmEvent->stAlarmNotify.cAlarmDesc, IVS_ALARM_DESCRIPTION_LEN, xml);
		 GET_ELEM_VALUE_NUM("ExistsRecord", pTemp, pAlarmEvent->stAlarmNotify.bExistsRecord, xml);
		 GET_ELEM_VALUE_CHAR("PreviewUrl", pTemp, pAlarmEvent->stAlarmNotify.cPreviewUrl, IVS_URL_LEN, xml);
	 }
	 xml.OutOfElem();
	 xml.OutOfElem();
	 
	 return IVS_SUCCEED;
 }

 // 获取拥有指定设备权限的用户列表拼接XML
 IVS_INT32 CAlarmMgrXMLProcess::GetDeviceUserListGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, IVS_UINT32 uiIsUserOnline, const IVS_INDEX_RANGE* pIndexRange, const std::string& strGroupID, CXml &xml)
 {
	 CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	 // 拆分设备编码
	 IVS_CHAR szDevCode[IVS_ALARM_CODE_LEN + 1] = {0};
	 IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	 IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, szDevCode, szDomainCode);

	 (void)xml.AddDeclaration("1.0","UTF-8","");
	 (void)xml.AddElem("Content");
	 (void)xml.AddChildElem("DomainCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(pDomainCode);

	 (void)xml.AddElem("PageInfo");	 
	 (void)xml.IntoElem();

	 (void)xml.AddChildElem("FromIndex");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiFromIndex)).c_str());

	(void)xml.AddElem("ToIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiToIndex)).c_str());

	(void)xml.AddElem("QueryCond");

	(void)xml.AddChildElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("CameraCode");

	(void)xml.AddElem("Value");
	(void)xml.SetElemValue(szDevCode);

	(void)xml.AddElem("QueryType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();

	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("DevDomainCode");

	(void)xml.AddElem("Value");
	(void)xml.SetElemValue(szDomainCode);

	(void)xml.AddElem("QueryType");
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();

	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("GroupID");

	(void)xml.AddElem("Value");
	(void)xml.SetElemValue(strGroupID.c_str());

	(void)xml.AddElem("QueryType");
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();

	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("IsOnline");

	(void)xml.AddElem("Value");
	(void)xml.IntoElem();
	if (0 == uiIsUserOnline)
	{
		(void)xml.SetElemValue("OFF");
	}
	else if (1 == uiIsUserOnline)
	{
		(void)xml.SetElemValue("ON");
	}
	else if (2 == uiIsUserOnline)
	{
		(void)xml.SetElemValue("ALL");
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IsOnline fail", "NA");
		return IVS_XML_INVALID;
	}

	(void)xml.AddElem("QueryType");
	(void)xml.SetElemValue("EXACT");
	xml.OutOfElem();
		 
	xml.OutOfElem();

	(void)xml.AddElem("OrderCond");
	
	(void)xml.IntoElem();
	(void)xml.AddChildElem("OrderField");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("");

	(void)xml.AddElem("Order");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("");
	xml.OutOfElem();		 
	xml.OutOfElem();

	xml.OutOfElem();	 

	return iRet;
 }
 IVS_INT32 CAlarmMgrXMLProcess::GetAbilityInfoParseXML(IVS_UINT32* uiAbility,  const char * pRspXml)
 {
	 CHECK_POINTER(uiAbility, IVS_OPERATE_MEMORY_ERROR);
	 *uiAbility = 0;
	 //IVS_UINT32 iTemp = 0;
	 IVS_DEBUG_TRACE("");
 // 遍历是存在这个节点数据
	 TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	 CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	 pXmlDoc->Parse(pRspXml);
	 TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,"Content/AbilityList/Ability");
	 const char* pCode = NULL;
	 while(NULL != pConnListElem)
	 {
		 pCode = pConnListElem->GetText();
		 CHECK_POINTER(pCode, IVS_XML_INVALID);
		 // 如果找到一样的code
		 if (1 == static_cast<unsigned int>(atoi(pCode)))
		 {
			 *uiAbility |=IVS_ABILITY_IA_FR;
		 }
		 if (2 == static_cast<unsigned int>(atoi(pCode)))
		 {
			 *uiAbility |=IVS_ABILITY_IA_BA;
	
		 }
		 if (3 == static_cast<unsigned int>(atoi(pCode)))
		 {
			 *uiAbility |=IVS_ABILITY_IA_QD;
		 }
		 pConnListElem = pConnListElem->NextSiblingElement("Ability");
	 }
	 IVS_DELETE(pXmlDoc);
	return IVS_SUCCEED;
 }

 // 获取拥有指定设备权限的用户列表解析XML
 IVS_INT32 CAlarmMgrXMLProcess::GetDeviceUserListParseXML(IVS_DEVICE_USER_INFO_LIST* pDevUserList, IVS_UINT32 uiBufferSize, CXml &xml)
 {
	 CHECK_POINTER(pDevUserList, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	 if (!xml.FindElem("Content"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "xml.FindElem(Content) is NULL", "NA");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }
	 (void)xml.IntoElem();
	 if (!xml.FindElem("PageInfo"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "xml.FindElem(PageInfo) is NULL", "NA");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }
	 xml.IntoElem();
	 const char* pTemp = NULL;
	 IVS_INT32 iTotal = 0;
	 IVS_INT32 iFromIndex = 0;
	 IVS_INT32 iToIndex = 0;
	 GET_ELEM_VALUE_NUM("RealNum", pTemp, iTotal , xml);
	 GET_ELEM_VALUE_NUM("FromIndex", pTemp, iFromIndex, xml);
	 GET_ELEM_VALUE_NUM("ToIndex",pTemp, iToIndex , xml);

	 if (iTotal < 0 || iToIndex < 0 || iFromIndex < 0 || iFromIndex > iToIndex)
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "Total", "pDevUserList->stIndexRange  error");
		 return IVS_SMU_DEV_REQ_XML_INVALID;
	 }
	  
	 pDevUserList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromIndex);
	 pDevUserList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToIndex);
	 pDevUserList->uiTotal = static_cast<IVS_UINT32>(iTotal);
	 xml.OutOfElem();

	 if (0 == pDevUserList->uiTotal)
	 {
		 BP_RUN_LOG_INF("0 == pDevUserList->uiTotal", "NA");
		 return IVS_SUCCEED;
	 }

	 if (!xml.FindElem("UserList"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(UserList) is fail", "NA");
		 return IVS_XML_INVALID;
	 }

	 //响应消息中传回来的页数
	 IVS_UINT32 uiResNum = (pDevUserList->stIndexRange.uiToIndex - pDevUserList->stIndexRange.uiFromIndex) + 1;
	 //实际要用到的内存大小
	 IVS_UINT32 uiResBufSize = sizeof(IVS_DEVICE_USER_INFO_LIST) + ((uiResNum) - 1) * sizeof(IVS_DEVICE_USER_INFO);
	 //实际需要内存与分配内存大小的比较
	 if (uiBufferSize < uiResBufSize)
	 {
		 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "ocx memory is not enough", "NA");
		 return IVS_ALLOC_MEMORY_ERROR;
	 }
	 IVS_UINT32 i = 0;	
	 (void)xml.IntoElem();
	 if (!xml.FindElem("UserInfo"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(UserInfo) is fail", "NA");
		 return IVS_XML_INVALID;
	 }  
	 do 
	 {
		 IVS_DEVICE_USER_INFO &pDevUserInfo = pDevUserList->stDeviceUserInfo[i];
		 xml.IntoElem();
		 const char* szElemValue = NULL;
		 IVS_INT32 iTemp = 0;
		 GET_ELEM_VALUE_NUM("UserID", szElemValue,  iTemp, xml);
		 pDevUserInfo.uiUserID = static_cast<IVS_UINT32>(iTemp);//lint !e571
		 GET_ELEM_VALUE_CHAR("LoginName", szElemValue, pDevUserInfo.cLoginName, IVS_NAME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("UserName", szElemValue, pDevUserInfo.cUserName, IVS_NAME_LEN, xml);
		 GET_ELEM_VALUE_NUM("UserGroupID", szElemValue,  iTemp, xml);
		 pDevUserInfo.uiGroupID = static_cast<IVS_UINT32>(iTemp);//lint !e571
		 xml.OutOfElem();
		 i++;
	 } while (i < uiResNum && xml.NextElem());
	 
	 xml.OutOfElem();

	 return IVS_SUCCEED;
 }

 // 告警订阅处理请求的AlarmInCode(增，删，改通用)
 IVS_INT32 CAlarmMgrXMLProcess::SetAlarmInCodeValue(CXml& xml)
 {
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_SUCCEED;
    do 
    {
        if (!xml.FindElemEx("Content/Subscribe/AddSubscribeList"))
        {
            break;
        }
        if (!xml.FindElem("SubscribeInfo"))
        {
            break;
        }
        do 
        {
            xml.IntoElem();
            if (!xml.FindElem("AlarmInCode"))
            {
                BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AlarmInCode)", "NA");
                return IVS_XML_INVALID;
            }
            const IVS_CHAR* pElemValue = xml.GetElemValue();
            IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
            IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
            iRet = CXmlProcess::ParseDevCode(pElemValue, szAlarmInCode, szDomainCode);
            xml.ModifyElemValue(szAlarmInCode);
            xml.AddElem("DevDomainCode");
            xml.SetElemValue(szDomainCode);
            xml.OutOfElem();
        } while (xml.NextElem());

    } while(false);  //lint !e717

    iRet = IVS_SUCCEED;
    xml.GetRootPos();
    if (!xml.FindElemEx("Content/Subscribe/DelSubscribeList"))
    {
        return iRet;
    }
    if (!xml.FindElem("SubscribeInfo"))
    {
        return iRet;
    }
    do 
    {
        xml.IntoElem();
        if (!xml.FindElem("AlarmInCode"))
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AlarmInCode)", "NA");
            return IVS_XML_INVALID;
        }
        const IVS_CHAR* pElemValue = xml.GetElemValue();
        IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
        IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
        iRet = CXmlProcess::ParseDevCode(pElemValue, szAlarmInCode, szDomainCode);
        xml.ModifyElemValue(szAlarmInCode);
        xml.AddElem("DevDomainCode");
        xml.SetElemValue(szDomainCode);
        xml.OutOfElem();
    } while (xml.NextElem());

    return iRet;
 }

 // 删除告警防区, 查询防区详情，手动布撤防拼接XML
 IVS_INT32 CAlarmMgrXMLProcess::AlarmAreaGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId, CXml& xml)
 {
	 IVS_DEBUG_TRACE("");
	 (void)xml.AddDeclaration("1.0", "UTF-8", "");
	 (void)xml.AddElem("Content");
	 (void)xml.AddChildElem("DomainCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(pDomainCode);

	 (void)xml.AddElem("AlarmAreaInfo");
	 (void)xml.IntoElem();
	 (void)xml.AddChildElem("AlarmAreaID");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiAlarmAreaId)).c_str());
	 xml.OutOfElem();
	 xml.OutOfElem();
	 return IVS_SUCCEED;
 }

 // 查询告警历史列表修改AlarmInCode字段
 IVS_INT32 CAlarmMgrXMLProcess::ModifyAlarmInCodeOfGetAlarmList(CXml& xml, std::string& strDstDomainCode)
 {
	 IVS_DEBUG_TRACE("");

	 IVS_INT32 iRet = IVS_SUCCEED;
	 if (!xml.FindElem("QueryField"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(QueryField) error", "NA");
		 return IVS_XML_INVALID;
	 }
	 std::string strElemValue;
	 std::string strAlarmInCode;
	 IVS_CHAR cAlarmInCode[IVS_QUERY_VALUE_LEN + 1] = {0};
	 const IVS_CHAR* szElemValue = NULL;
	 IVS_CHAR szElemField[IVS_DEV_CODE_LEN + 1] = {0};
	 do 
	 {
		 (void)xml.IntoElem();
		 GET_ELEM_VALUE_CHAR("Field", szElemValue, szElemField, IVS_DEV_CODE_LEN, xml);
		 if (0 != strcmp(szElemField, "AlarmInCode"))
		 {
			 xml.OutOfElem();
			 memset(szElemField, 0x0, IVS_DEV_CODE_LEN + 1);
			 continue;
		 }
		 GET_ELEM_VALUE_CHAR("Value", szElemValue, cAlarmInCode, IVS_QUERY_VALUE_LEN, xml);
		 strElemValue = cAlarmInCode;
		 IVS_UINT32 uiPos = strElemValue.find(",");
		 if (std::string::npos != uiPos && !strElemValue.empty())
		 {
			 char* pNext = NULL;
			 char* token = strtok_s(cAlarmInCode, ",", &pNext); 
             while (NULL!= token)
             {
                 IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
                 IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
                 iRet += CXmlProcess::ParseDevCode(token, szAlarmInCode, szDomainCode);
                 strDstDomainCode = szDomainCode;    // 消息发送目标域
                 strAlarmInCode.append(szAlarmInCode).append(",");
                 token = strtok_s(NULL, ",", &pNext);
             }
			 strAlarmInCode = strAlarmInCode.substr(0, strAlarmInCode.size() - 1);
			 xml.ModifyElemValue(strAlarmInCode.c_str());
			 break;
		 }
		 else  if(!strElemValue.empty())
		 {
			 IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
			 IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
			 iRet = CXmlProcess::ParseDevCode(strElemValue.c_str(), szAlarmInCode, szDomainCode);
             strDstDomainCode = szDomainCode;    // 消息发送目标域
			 strAlarmInCode.append(szAlarmInCode);
			 xml.ModifyElemValue(strAlarmInCode.c_str());
			 break;
		 }

		 xml.OutOfElem();
	 }while(xml.NextElem());

	 return iRet;
 }

 // 查询OMU告警事件列表
 IVS_INT32 CAlarmMgrXMLProcess::GetDevAlarmListParseXML(CXml& xml, IVS_DEVICE_ALARM_EVENT_LIST* pDevAlarmList, const IVS_UINT32& uiBufSize)
 {
	 CHECK_POINTER(pDevAlarmList, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	 if (!xml.FindElem("Content"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Device AlarmEventList XML ", "xml.FindElem(Content) is fail");
		 return IVS_XML_INVALID;
	 }   
	 xml.IntoElem();
	 if (!xml.FindElem("PageInfo"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Device AlarmEventList XML", "xml.FindElem(PageInfo) is fail");
		 return IVS_XML_INVALID;
	 }
	 xml.IntoElem();
	 const char* pTemp = NULL;
	 IVS_INT32 iTotal = 0;
	 IVS_INT32 iFromIndex = 0;
	 IVS_INT32 iToIndex = 0;
	 GET_ELEM_VALUE_NUM("RealNum", pTemp,iTotal , xml);
	 GET_ELEM_VALUE_NUM("FromIndex", pTemp, iFromIndex, xml);
	 GET_ELEM_VALUE_NUM("ToIndex",pTemp,iToIndex , xml);

	 // 判断RealNum，FromIndex，ToIndex是否合法
	 if (iTotal < 0 || iToIndex < 0 || iFromIndex < 0 || iFromIndex > iToIndex)
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Device AlarmEventList XML", "ToIndex[%d], FromIndex[%d], ToIndex[%d]", iTotal, iFromIndex, iToIndex);
		 return IVS_XML_INVALID;
	 }	

	 pDevAlarmList->iTotal = static_cast<IVS_UINT32>(iTotal);
	 pDevAlarmList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromIndex);
	 pDevAlarmList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToIndex);
	 if (0 == iTotal)
	 {
		 return IVS_SUCCEED;
	 }

	 xml.OutOfElem();
	 if (!xml.FindElem("AlarmList"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Device AlarmEventList XML", "xml.FindElem(AlarmList) is fail");
		 return IVS_XML_INVALID;
	 }
	 xml.IntoElem();
	 if (!xml.FindElem("AlarmInfo"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Device AlarmEventList XML", "xml.FindElem(AlarmInfo) is fail");
		 return IVS_XML_INVALID;
	 }

	 IVS_UINT32 uiRspNum = (pDevAlarmList->stIndexRange.uiToIndex - pDevAlarmList->stIndexRange.uiFromIndex) + 1;
	 IVS_UINT32 uiReqBuf = (uiRspNum - 1) * sizeof(IVS_DEVICE_ALARM_NOTIFY) + sizeof(IVS_DEVICE_ALARM_EVENT_LIST);
	 if (uiReqBuf > uiBufSize)
	 {
		 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "ocx memeory not enough", "NA");
		 return IVS_ALLOC_MEMORY_ERROR;
	 }
	 IVS_INT32 ui = 0;
	 const IVS_CHAR* szElemValue = NULL;
	 while((IVS_UINT32)ui < uiRspNum)
	 {
		 IVS_DEVICE_ALARM_NOTIFY& stDevAlarmNotify = pDevAlarmList->stDeviceAlarmNotify[ui];
		 xml.IntoElem();
		 GET_ELEM_VALUE_NUM_FOR_UINT("AlarmID", szElemValue, stDevAlarmNotify.ullAlarmEventID, xml);
		 GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, stDevAlarmNotify.cAlarmInCode, IVS_ALARM_CODE_LEN, xml);
		 GET_ELEM_VALUE_NUM_FOR_UINT("AlarmInType", szElemValue, stDevAlarmNotify.uiAlarmInType, xml);
		 GET_ELEM_VALUE_CHAR("AlarmInName", szElemValue, stDevAlarmNotify.cAlarmInName, IVS_ALARM_NAME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("NodeType", szElemValue, stDevAlarmNotify.cNodeType, IVS_NODE_TYPE_LEN, xml);
		 GET_ELEM_VALUE_NUM_FOR_UINT("AlarmLevelValue", szElemValue, stDevAlarmNotify.uiAlarmLevelValue, xml);
		 GET_ELEM_VALUE_CHAR("AlarmType", szElemValue, stDevAlarmNotify.cAlarmType, IVS_ALARM_CODE_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmCategory", szElemValue, stDevAlarmNotify.cAlarmCategory, IVS_ALARM_CATEGORY_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OccurFirTime", szElemValue, stDevAlarmNotify.cOccurFirstTime, IVS_TIME_LEN, xml);
		 GET_ELEM_VALUE_CHAR("OccurEndTime", szElemValue, stDevAlarmNotify.cOccurEndTime, IVS_TIME_LEN, xml);
		 GET_ELEM_VALUE_NUM_FOR_UINT("OccurNumber", szElemValue, stDevAlarmNotify.uiOccurNumber, xml);
		 GET_ELEM_VALUE_CHAR("Reserve", szElemValue, stDevAlarmNotify.cReserver, 32, xml);
		 GET_ELEM_VALUE_CHAR("LocationInfo", szElemValue, stDevAlarmNotify.cLocationInfo, IVS_DEVICE_ALARM_LOCATION_INFO_LEN, xml);
		 GET_ELEM_VALUE_CHAR("AlarmDesc", szElemValue, stDevAlarmNotify.cAlarmDest, IVS_ALARM_DESCRIPTION_LEN, xml);
		 GET_ELEM_VALUE_CHAR("ExtParam", szElemValue, stDevAlarmNotify.cExtParam, IVS_ALARM_EX_PARAM_LEN, xml);
         GET_ELEM_VALUE_CHAR("NetName", szElemValue, stDevAlarmNotify.cNetName, IVS_NAME_LEN, xml);
		 xml.OutOfElem();
		 ui++;
		 if (!xml.NextElem())
		 {
			 break;
		 }
	 }

	 xml.OutOfElem();
	 return IVS_SUCCEED;
 }

 // 启停开关量拼装xml
 IVS_INT32 CAlarmMgrXMLProcess::AlarmOutGetXML(CXml& xml, const IVS_CHAR* pAlarmInCode, const IVS_CHAR* pDomainCode, IVS_INT32 iStateType)
 {
	 CHECK_POINTER(pAlarmInCode, IVS_PARA_INVALID);
	 CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);

	 (void)xml.AddDeclaration("1.0", "UTF-8", "");
	 (void)xml.AddElem("Content");
	 (void)xml.AddChildElem("DomainCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(pDomainCode);

	 (void)xml.AddElem("AlarmOutChanInfo");
	 (void)xml.IntoElem();
	 (void)xml.AddChildElem("AlarmOutCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(pAlarmInCode);

	 (void)xml.AddElem("State");
	 if (0 == iStateType)
	 {
		 (void)xml.SetElemValue("ON");
	 }
	 else if (1 == iStateType)
	 {
		 (void)xml.SetElemValue("OFF");
	 }
	 xml.OutOfElem();

	 return IVS_SUCCEED;
 }

 // 获取防区信息列表从请求xml中获取域编码,拆分查询条件中的AlarmInCode
 IVS_INT32 CAlarmMgrXMLProcess::GetDomainCodeANDParseAlarmInCode(const IVS_CHAR* pReqXml, std::string& strDomainCode, CXml& xml)
 {
	 CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	 if (!xml.Parse(pReqXml))
	 {
		 BP_RUN_LOG_ERR(IVS_PARA_INVALID, "xml.Parse(pReqXml) fail", "NA");
		 return IVS_XML_INVALID;
	 }
	 // 获取域编码
	 if (!xml.FindElemEx("Content/DomainCode"))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(\"Content/DomainCode\")", "NA");
		 return IVS_XML_INVALID;
	 }

     const IVS_CHAR* pDomainCode = xml.GetElemValue();
     if (NULL != pDomainCode)
     {
         strDomainCode = pDomainCode;
     }

	 // 拆分AlarmInCode
	 if (xml.FindElemEx("Content/PageInfo/QueryCond"))
	 {
		if (!xml.FindElem("QueryField"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"QueryField\")", "NA");
			return IVS_XML_INVALID;
		}
		do 
		{
			xml.IntoElem();
			if (!xml.FindElem("Field"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"Field\")", "NA");
				return IVS_XML_INVALID;
			}
			std::string strField;
            const IVS_CHAR* pField = xml.GetElemValue();
            if (NULL != pField)
            {
                strField = pField;
            }
			if (strField.compare("AlarmInCode") != 0 )
			{
				xml.OutOfElem();
				continue;
			}
			if (!xml.FindElem("Value"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"Value\")", "NA");
				return IVS_XML_INVALID;
			}
			std::string strValue;
            const IVS_CHAR* pValue = xml.GetElemValue();
            if (NULL != pValue)
            {
                strValue = pValue;
            }
			if (!strValue.empty())
			{
				IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
				IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
				(void)CXmlProcess::ParseDevCode(strValue.c_str(), szAlarmInCode, szDomainCode);
				xml.ModifyElemValue(szAlarmInCode);
			}
			
			xml.OutOfElem();
			
		} while (xml.NextElem());
	 }

	 return IVS_SUCCEED;
 }

 // 根据外域设备取得设备组列表拼装xml
 IVS_INT32 CAlarmMgrXMLProcess::GetGroupListGetXml(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, CXml& xml)
 {
	 CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	 CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	 IVS_DEBUG_TRACE("");

	 (void)xml.AddDeclaration("1.0", "UTF-8", "");
	 (void)xml.AddElem("Content");
	 (void)xml.AddChildElem("DomainCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(pDomainCode);

	 (void)xml.AddElem("ChannelCode");
	 (void)xml.SetElemValue(pDevCode);

	 xml.GetRootPos();
	 return IVS_SUCCEED;
 }

 // 获取GroupID，将多个GroupID连在一起，中间用","隔开
 IVS_INT32 CAlarmMgrXMLProcess::GetGroupID(CXml& xml, std::string& strGroupIDs)
 {
	 IVS_DEBUG_TRACE(""); 
	 if (!xml.FindElemEx("Content/GroupList"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlReqGroupID.FindElemEx(\"Content/GroupList\")", "NA");
		 return IVS_SMU_XML_INVALID;
	 }
	 if (!xml.FindElem("GroupID"))
	 {
		 BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlReqGroupID.FindElemEx(\"GroupID\")", "NA");
		 return IVS_SMU_XML_INVALID;
	 }
	 std::string strGroupID;
	 do 
	 {
         const IVS_CHAR* pGroupID = xml.GetElemValue();
         if (NULL != pGroupID)
         {
             strGroupID = pGroupID;
         }

		 if (!strGroupID.empty())
		 {
			 strGroupIDs.append(strGroupID).append(",");
		 }	   
	 } while (xml.NextElem());
	 if (strGroupID.empty())
	 {
		 strGroupIDs = "0";
	 }
	 else
	 {
		 strGroupIDs = strGroupIDs.substr(0, strGroupIDs.size() - 1);
	 }	 
	 return IVS_SUCCEED;
 }

 // 透传接口中，将相应消息从string转到pRsqXml所指内存中
 IVS_INT32 CAlarmMgrXMLProcess::GetRsqXML(const IVS_CHAR* pRsp, IVS_CHAR*& pRspXml)
 {
	 IVS_DEBUG_TRACE("");
	 IVS_CHAR* pResult = IVS_NEW(pResult, strlen(pRsp) + 1);
	 CHECK_POINTER(pResult, IVS_ALLOC_MEMORY_ERROR);
	 memset(pResult, 0x0, strlen(pRsp) + 1);
	 if (!CToolsHelp::Memcpy(pResult, strlen(pRsp) + 1, pRsp, strlen(pRsp) + 1))
	 {
		 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pResult, strpRsp.size(), strpRsp.c_str(), strpRsp.size()) fail", "NA");
		 return IVS_ALLOC_MEMORY_ERROR;
	 }
	 pRspXml = pResult;

	 return IVS_SUCCEED;
 }

 // 拆分AlarmInCode，添加DevDomainCode节点(防区中使用)
 IVS_INT32 CAlarmMgrXMLProcess::SetAlarmInCode4AlarmAreaReq(CXml& xml)
 {
	 IVS_DEBUG_TRACE("");
	 IVS_INT32 iRet = IVS_SUCCEED;
	 IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	 IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	 // 可以增加空防区，即没有AlarmInList节点
	 if (xml.FindElemEx("Content/AlarmAreaInfo/AlarmInList"))
	 {
		 // 在修改防区中，如果没有AlarmInList，即修改名称
		 //               如果有AlarmInList，没有AlarmInfo，删除防区下的所有设备
		 //				  如果AlarmInList和AlarmInfo，即修改防区下的设备
		 if (xml.FindElem("AlarmInInfo"))
		 {
			 do 
			 {
				 xml.IntoElem();
				 if (!xml.FindElem("AlarmInCode"))
				 {
					 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"AlarmInCode\") fail", "NA");
					 return IVS_XML_INVALID;
				 }
				 std::string strAlarmInCode;
                 const IVS_CHAR* pAlarmInCode = xml.GetElemValue();		
                 if (NULL != pAlarmInCode)
                 {
                     strAlarmInCode = pAlarmInCode;
                 }
				 iRet = CXmlProcess::ParseDevCode(strAlarmInCode.c_str(), szAlarmInCode, szDomainCode);
				 xml.ModifyElemValue(szAlarmInCode);
				 (void)xml.AddElem("DevDomainCode");
				 (void)xml.SetElemValue(szDomainCode);

				 xml.OutOfElem();
			 } while (xml.NextElem());
		 }
		 
	 }	

	 return iRet;
 }

// 获得消息目标域
 IVS_INT32 CAlarmMgrXMLProcess::GetDstDomainCode(CXml& xml, const IVS_CHAR* pReqXml, std::string& strDomainCode)
 {
	 CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	 IVS_DEBUG_TRACE("");
	 if (!xml.Parse(pReqXml))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		 return IVS_XML_INVALID;
	 }
	 // 获取消息发送域编码
	 if (xml.FindElemEx("Content/DomainCode"))
	 {
		 const char* pDomainCode = xml.GetElemValue();
		 if (NULL != pDomainCode)
		 {
			 strDomainCode = pDomainCode;
		 }        
	 }
	 return IVS_SUCCEED;
 }

 // 拆分DevCode
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmSnapListGetXml(IVS_ULONG ulAlarmEventID, const IVS_CHAR* pAlarmInCode, const std::string& strDomainCode, CXml& xml)
 {
	 CHECK_POINTER(pAlarmInCode, IVS_PARA_INVALID);
	 IVS_DEBUG_TRACE("");
     
	 (void)xml.AddDeclaration("1.0", "UTF-8", "");
	 (void)xml.AddElem("Content");
	 (void)xml.AddChildElem("DomainCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(strDomainCode.c_str());

	 (void)xml.AddElem("AlarmInfo");
	 (void)xml.IntoElem();
	 (void)xml.AddChildElem("AlarmEventID");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(ulAlarmEventID)).c_str());

	 IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	 IVS_CHAR szAlarmDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	 IVS_INT32 iRet = CXmlProcess::ParseDevCode(pAlarmInCode, szAlarmInCode, szAlarmDomainCode);
	 (void)xml.AddElem("AlarmDomainCode");
	 (void)xml.SetElemValue(szAlarmDomainCode);

	 xml.GetRootPos();

	 return iRet;
 }

 // 拆分设备编码，取出域编码作为消息目标域, 设备布撤防，报警复位用到
 IVS_INT32 CAlarmMgrXMLProcess::SetDevCode(CXml& xml, const IVS_CHAR* pReqXml, const std::string& strPath, IVS_CHAR* pDevDomainCode, IVS_CHAR* pDevCode)
 {
	 CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	 CHECK_POINTER(pDevDomainCode, IVS_PARA_INVALID);
	 BP_RUN_LOG_INF("strPath: %s",strPath.c_str());
	 IVS_DEBUG_TRACE("");

	 if (!xml.Parse(pReqXml))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		 return IVS_XML_INVALID;
	 }
	 if (!xml.FindElemEx(strPath.c_str()))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(strPath) fail", "NA");
		 return IVS_XML_INVALID;
	 }
	 std::string strDevCode;
     const IVS_CHAR* pDevCodeTemp = xml.GetElemValue();
     if (NULL != pDevCodeTemp)
     {
         strDevCode = pDevCodeTemp;
     }
	 IVS_INT32 iRet = CXmlProcess::ParseDevCode(strDevCode.c_str(), pDevCode, pDevDomainCode);
	 if (!strDevCode.empty())
	 {
		 xml.ModifyElemValue(pDevCode);
	 }

	 (void)xml.MkAndSetElemValueEx("Content/DomainCode", pDevDomainCode);
	 return iRet;
 }

 // 外域告警订阅，目录订阅中拆分LocalDevCode
 IVS_INT32 CAlarmMgrXMLProcess::ParseLocalDevCode(CXml& xml, const IVS_CHAR* pReqXml, const IVS_CHAR* pGrandPath, const IVS_CHAR* pParentPath)
 {
	 CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	 CHECK_POINTER(pGrandPath, IVS_PARA_INVALID);
	 CHECK_POINTER(pParentPath, IVS_PARA_INVALID);
	 IVS_DEBUG_TRACE("");

	 IVS_INT32 iRet = IVS_SUCCEED;
	 if (!xml.Parse(pReqXml))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pReqXml) fail", "NA");
		 return IVS_XML_INVALID;
	 }
	 if (!xml.FindElemEx(pGrandPath))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(pGrandPath) fail, GrandPath: %s ", pGrandPath);
		 return IVS_XML_INVALID;
	 }
	 if (!xml.FindElem(pParentPath))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(pParentPath) fail, ParentPath: %s ", pParentPath);
		 return IVS_XML_INVALID;
	 }
	 do 
	 {
		 xml.IntoElem();
		 if (!xml.FindElem("LocalDevCode"))
		 {
			 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"LocalDevCode\") fail", "NA");
			 return IVS_XML_INVALID;
		 }
		 std::string strLocalDevCode;
         const IVS_CHAR* pLocalDevCode = xml.GetElemValue();
         if (NULL != pLocalDevCode)
         {
             strLocalDevCode = pLocalDevCode;
         }
		 IVS_CHAR szDevLocalCode[IVS_DEV_CODE_LEN + 1] = {0};
		 IVS_CHAR szDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
		 iRet = CXmlProcess::ParseDevCode(strLocalDevCode.c_str(), szDevLocalCode, szDevDomainCode);
		 if (!strLocalDevCode.empty())
		 {
			 xml.ModifyElemValue(szDevLocalCode);
		 }
		 xml.OutOfElem();
	 } while (xml.NextElem());

	 return iRet;
 }

 // 查询外域告警订阅，目录订阅中拼装响应消息中的LocalDevCode
 IVS_INT32 CAlarmMgrXMLProcess::ModifyLocalDevCode(CXml& xmlRsp, const std::string& strDomainCode, const IVS_CHAR* pGrandPath, const IVS_CHAR* pParentPath)
 {
	 CHECK_POINTER(pGrandPath, IVS_PARA_INVALID);
	 CHECK_POINTER(pParentPath, IVS_PARA_INVALID);
	 IVS_DEBUG_TRACE("");

	 if (xmlRsp.FindElemEx(pGrandPath))
	 {
		 if (!xmlRsp.FindElem(pParentPath))
		 {
			 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(pParentPath) fail, ParentPath: %s ", pParentPath);
			 return IVS_XML_INVALID;
		 }
		 do 
		 {
			 xmlRsp.IntoElem();
			 if (!xmlRsp.FindElem("LocalDevCode"))
			 {
				 BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(\"LocalDevCode\") fail", "NA");
				 return IVS_XML_INVALID;
			 }
			 std::string strLocalDevCode;
             const IVS_CHAR* pLocalDevCode = xmlRsp.GetElemValue();
             if (NULL != pLocalDevCode)
             {
                 strLocalDevCode = pLocalDevCode;
             }
			 if (!strLocalDevCode.empty())
			 {
				 strLocalDevCode.append("#").append(strDomainCode);
			 }
			 xmlRsp.ModifyElemValue(strLocalDevCode.c_str());
			 xmlRsp.OutOfElem();
		 } while (xmlRsp.NextElem());
	 }
	

	 return IVS_SUCCEED;
 }

 // 获取告警抓拍图片列表组合最终的xml
 IVS_INT32 CAlarmMgrXMLProcess::GetAlarmSnapshotListGetXml(const std::string& strInXml, std::string& strOutXml, IVS_INT32& iFlag)
 {
	  IVS_DEBUG_TRACE("");
	  CXml xmlReq;
	 // 将第一次解析出的xml直接放在strNSSRsq中,或者是strOutXml还为空，都将解析出的xml直接赋值
	  if (0 == iFlag || strOutXml.empty())
	  {
		  strOutXml.append(strInXml);
		  return IVS_SUCCEED;
	  }
	 // 第二次解析出来的xml需要取出其中所有的Info节点查到strNSSRsq中，组合成最终的xml	 
	 // 将strNSSRsq先解析，找到SnapshotList节点，之后找到的Info全部链接在SnapshotList节点下
	 TiXmlDocument pSnapShotXmlDoc;
	 (void)(pSnapShotXmlDoc.Parse(strOutXml.c_str(), NULL, TIXML_ENCODING_LEGACY));
	 if (0 != pSnapShotXmlDoc.Error())
	 {
		 BP_RUN_LOG_ERR(IVS_PARA_INVALID, "parse xml failed","NA");
		 return IVS_PARA_INVALID;
	 }
	 TiXmlElement* pSnapshotListElem = CToolsHelp::FindElementByPath(&pSnapShotXmlDoc,"Content/SnapshotList");
	 if (NULL == pSnapshotListElem)
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "Content/SnapshotList can not find in strNSSRsq", "NA");
		 return IVS_XML_INVALID;
	 }

	 // 第二次开始解析出来的响应消息，取出Info节点全部链接在SnapshotList节点下
	 TiXmlDocument pXmlDoc;
	 (void)(pXmlDoc.Parse(strInXml.c_str(), NULL, TIXML_ENCODING_LEGACY));
	 if (0 != pXmlDoc.Error())
	 {
		 BP_RUN_LOG_ERR(IVS_PARA_INVALID, "parse xml failed","NA");
		 return IVS_PARA_INVALID;
	 }
	 // 如果没有信息返回，smu返回空字符串
	 TiXmlElement* pListElem = CToolsHelp::FindElementByPath(&pXmlDoc,"Content/SnapshotList");
	 if (NULL != pListElem)
	 {
		 TiXmlElement* pInfoElem = pListElem->FirstChildElement("SnapInfo");
		 if (NULL != pListElem)//lint !e774
		 {
			 for (;;)
			 {
				 TiXmlNode* pInfoNode = pInfoElem->Clone();
                 if (NULL != pInfoNode)
                 {
                     BP_RUN_LOG_ERR(IVS_PARA_INVALID, "pInfoNode is null","NA");
                     pSnapshotListElem->LinkEndChild(pInfoNode->ToElement());
                 }
				 
				 pInfoElem = pInfoElem->NextSiblingElement("SnapInfo");
				 if (NULL == pInfoElem)
				 {
					 break;
				 }
			 }	
		 }
	 }

	 // 将最后拼装好的字符串打印到xml里
	 TiXmlPrinter xmlPrinter;
	 xmlPrinter.SetStreamPrinting();
 
	 if (!pSnapShotXmlDoc.Accept(&xmlPrinter))
	 {
		 BP_RUN_LOG_ERR(IVS_FAIL, "print error", "NA");
		 return IVS_FAIL;//lint !e429
	 }

	 (void)xmlReq.Parse(xmlPrinter.CStr());

	 IVS_UINT32 uiXmlLen = 0;
     const IVS_CHAR* pOutXml = xmlReq.GetXMLStream(uiXmlLen); 	
     if (NULL != pOutXml)
     {
         strOutXml = pOutXml;

     }
	 
	 return IVS_SUCCEED;
 }//lint !e1764
