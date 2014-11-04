#include "stdafx.h"
#include "OCXAlarmMgr.h"
#include "hwsdk.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "OCXAlarmMgrXMLProcess.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"
COCXAlarmMgr::COCXAlarmMgr(void)
{
}


COCXAlarmMgr::~COCXAlarmMgr(void)
{
}

// 新增告警级别
IVS_VOID COCXAlarmMgr::AddAlarmLevel(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmLevel, CString &strResult)
{
	if (NULL == pAlarmLevel)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml fail", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("");
	//1.STR 转为 XML
	CXml xml;	
	if (!xml.Parse(pAlarmLevel))
	{
		BP_RUN_LOG_ERR(IVS_XML_PACK_ERROR, "prase xml fail", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_PACK_ERROR);
		return;
	}

	//2.XML 转为 struct
	IVS_ALARM_LEVEL stAlarmLevel;
	memset(&stAlarmLevel,0,sizeof(IVS_ALARM_LEVEL));
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::AlarmLevelPraseXML(&stAlarmLevel,xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmLevel Prase Request XML fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}

	//3.传入struct 调用 sdk
	IVS_UINT32 uiAlarmLevelID = 0;
	iRet = IVS_SDK_AddAlarmLevel(iSessionId, &stAlarmLevel, &uiAlarmLevelID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddAlarmLevel SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return ;
	}
	COCXAlarmMgrXMLProcess::GetAddAlarmErrString(strResult, uiAlarmLevelID, iRet);

	return;
}

// 修改告警级别
IVS_INT32 COCXAlarmMgr::ModifyAlarmLevel(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmLevel)
{
	CHECK_POINTER(pAlarmLevel, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	// 1.STR 转为 XML	
	CXml xml;
	if (!xml.Parse(pAlarmLevel))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "prase xml fail", "NA");
		return IVS_XML_INVALID;
	}

	// 2.XML 转为 struct
	IVS_ALARM_LEVEL stAlarmLevel;
	memset(&stAlarmLevel,0,sizeof(IVS_ALARM_LEVEL));
	
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::AlarmLevelPraseXML(&stAlarmLevel, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmLevel Prase Request XML fail", "NA");
		return iRet;
	}
	// 3.传入strct 调用 sdk
	iRet = IVS_SDK_ModifyAlarmLevel(iSessionId, &stAlarmLevel);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ModifyAlarmLevel SDK Interface fail", "NA");
	} 
	return iRet;
}

// 删除告警级别
IVS_INT32 COCXAlarmMgr::DeleteAlarmLevel(IVS_INT32 iSessionId, IVS_UINT32 uiAlarmLevelId)
{
	IVS_DEBUG_TRACE("");
	// 传入strct 调用 sdk
	IVS_INT32 iRet = IVS_SDK_DeleteAlarmLevel(iSessionId ,uiAlarmLevelId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DeleteAlarmLevel SDK Interface fail", "NA");
	} 
	return iRet;
}

// 查询告警级别详细信息
IVS_VOID COCXAlarmMgr::GetAlarmLevel(IVS_INT32 iSessionId, IVS_INT32 iAlarmLevelID, CString &strResult)
{
	IVS_DEBUG_TRACE("");

	IVS_ALARM_LEVEL pAlarmLevel;
	memset(&pAlarmLevel, 0, sizeof(IVS_ALARM_LEVEL));
	IVS_INT32 iRet = IVS_SDK_GetAlarmLevel(iSessionId, (IVS_UINT32)iAlarmLevelID, &pAlarmLevel);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLevel SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}

	CXml xml;
	iRet = COCXAlarmMgrXMLProcess::GetAlarmLevel(strResult, &pAlarmLevel, iRet, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLevel GetResultToCU fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}
	 
	return;
}

// 查询告警级别列表
IVS_VOID COCXAlarmMgr::GetAlarmLevelList(IVS_INT32 iSessionId, CString &strResult)
{
	IVS_DEBUG_TRACE("");

	IVS_UINT32 iBufferSize = IVS_MAX_ALARM_LEVEL_NUM * sizeof(IVS_ALARM_LEVEL);
	IVS_VOID* pBuff = IVS_NEW((IVS_CHAR*&)pBuff, iBufferSize);	
	CHECK_POINTER_VOID(pBuff);
	memset(pBuff, 0, iBufferSize);
	IVS_ALARM_LEVEL *pAlarmLevel  = reinterpret_cast<IVS_ALARM_LEVEL*>(pBuff);

	IVS_UINT32 iAlarmLevelNum = 0;
	IVS_INT32 iRet = IVS_SDK_GetAlarmLevelList(iSessionId, IVS_MAX_ALARM_LEVEL_NUM, &iAlarmLevelNum, pAlarmLevel, iBufferSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLevelList SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_DELETE(pBuff, MUILI);
		return;	
	}

	if (iAlarmLevelNum * sizeof(IVS_ALARM_LEVEL) > iBufferSize)
	{
		COCXXmlProcess::GetErrString(strResult, IVS_ALLOC_MEMORY_ERROR);
		IVS_DELETE(pBuff, MUILI);
		return;
	}

	CXml xml;
	if (IVS_SUCCEED != COCXAlarmMgrXMLProcess::GetAlarmLevelListGetXML(strResult, &iAlarmLevelNum, pAlarmLevel, iRet, xml))
	{
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		IVS_DELETE(pBuff, MUILI);
		return;
	}
	IVS_DELETE(pBuff, MUILI);
	return;
}

// 新增告警帮助
IVS_INT32 COCXAlarmMgr::AddHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	// 1.STR 转为 XML
	CXml xml;
	if (!xml.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "prase xml fail", "NA");
		return IVS_XML_INVALID;
	}

	// 2.XML 转为 struct
	IVS_HELP_INFO* pstHelpInfo = (IVS_HELP_INFO*)IVS_NEW((IVS_CHAR* &)pstHelpInfo, sizeof(IVS_HELP_INFO));//lint !e826
	CHECK_POINTER(pstHelpInfo, IVS_ALLOC_MEMORY_ERROR);
	memset(pstHelpInfo, 0, sizeof(IVS_HELP_INFO));

	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::HelpInfoPraseXML(pstHelpInfo, xml, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pstHelpInfo, MUILI);
		BP_RUN_LOG_ERR(iRet, "Prase HelpInfo Request XML fail", "NA");
		return iRet;
	}

	// 3.传入strct 调用 sdk
	iRet = IVS_SDK_AddHelpInfo(iSessionId, szDomainCode, pstHelpInfo);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pstHelpInfo, MUILI);
		BP_RUN_LOG_ERR(iRet, "AddHelpInfo SDK Interface fail", "NA");
		return iRet;
	} 
	IVS_DELETE(pstHelpInfo, MUILI);
	return iRet;
}

// 修改告警帮助
IVS_INT32 COCXAlarmMgr::ModifyHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	// 1.STR 转为 XML
	CXml xml;
	if (!xml.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "prase xml fail", "NA");
		return IVS_XML_INVALID;
	}

	// 2.XML 转为 struct
	IVS_HELP_INFO* pstHelpInfo = (IVS_HELP_INFO*)IVS_NEW((IVS_CHAR* &)pstHelpInfo, sizeof(IVS_HELP_INFO));//lint !e826
	CHECK_POINTER(pstHelpInfo, IVS_ALLOC_MEMORY_ERROR);
	memset(pstHelpInfo, 0, sizeof(IVS_HELP_INFO));
	IVS_CHAR cszDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::HelpInfoPraseXML(pstHelpInfo, xml, cszDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pstHelpInfo, MUILI);
		BP_RUN_LOG_ERR(iRet, "Prase HelpInfo Request XML fail", "NA");
		return iRet;
	}

	// 3.传入strct 调用 sdk
	iRet = IVS_SDK_ModifyHelpInfo(iSessionId, cszDomainCode, pstHelpInfo);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pstHelpInfo, MUILI);
		BP_RUN_LOG_ERR(iRet, "ModifyHelpInfo SDK Interface fail", "NA");
		return iRet;
	}
	IVS_DELETE(pstHelpInfo, MUILI);
	return iRet;
}

// 删除告警帮助
IVS_INT32 COCXAlarmMgr::DeleteHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pHelpInfo)
{
	CHECK_POINTER(pHelpInfo, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiSourceID = 0;
	IVS_UINT32 uiSourceType = 0;
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::ParseHelpInfoXml(pHelpInfo, uiSourceID, uiSourceType, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Parse HelpInfo fail", "NA");
		return iRet;
	}
	//调用 sdk
	iRet = IVS_SDK_DeleteHelpInfo(iSessionId, uiSourceType, uiSourceID, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DeleteHelpInfo SDK Interface fail", "NA");
		return iRet;
	}
	 
	return iRet;
}

// 查询告警帮助
IVS_VOID COCXAlarmMgr::QueryHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pHelpInfo, CString &strResult)
{
	IVS_DEBUG_TRACE("");

	//结构体初始化
	IVS_UINT32 uiAlarmHelpSourceID = 0;
	IVS_UINT32 uiAlarmHelpSourceType = 0;
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::ParseHelpInfoXml(pHelpInfo, uiAlarmHelpSourceID, uiAlarmHelpSourceType, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Parse HelpInfo fail when QueryHelpInfo ", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}
	IVS_HELP_INFO* pstHelpInfo = (IVS_HELP_INFO*)IVS_NEW((IVS_CHAR* &)pstHelpInfo, sizeof(IVS_HELP_INFO));//lint !e826
	CHECK_POINTER_VOID(pstHelpInfo);
	memset(pstHelpInfo, 0, sizeof(IVS_HELP_INFO));

	//传入strct 调用 sdk	
	iRet = IVS_SDK_QueryHelpInfo(iSessionId, uiAlarmHelpSourceType, uiAlarmHelpSourceID, szDomainCode, pstHelpInfo);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pstHelpInfo, MUILI);
		BP_RUN_LOG_ERR(iRet, "QueryHelpInfo SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}
	CXml xml;
	pstHelpInfo->uiSourceId = uiAlarmHelpSourceID;
	pstHelpInfo->uiSourceType = uiAlarmHelpSourceType;
	iRet = COCXAlarmMgrXMLProcess::QueryHelpInfoGetXML(strResult, pstHelpInfo, iRet, xml);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pstHelpInfo, MUILI);
		BP_RUN_LOG_ERR(iRet, "Get QueryHelpInfo Response XML fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}
	IVS_DELETE(pstHelpInfo, MUILI);
	return;
}

// 设置告警类型级别
IVS_INT32 COCXAlarmMgr::SetAlarmTypeLevel(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	
	CXml xml;
	if (!xml.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pReqXml) failed", "NA");
		return IVS_XML_INVALID;
	}
	// 获取有几条记录	
	IVS_UINT32 iNum = 0;
	if (xml.FindElemEx("Content/AlarmTypeList"))
	{  
		if (!xml.FindElemValue("AlarmTypeInfo"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemValue(AlarmTypeInfo) fail ", "NA");
			return IVS_XML_INVALID;
		}
		do
		{            
			iNum++;
		}while (xml.NextElem());
	}
	if (iNum < 1)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemValue(AlarmTypeList) fail ", "NUM=%d", iNum);
		return IVS_XML_INVALID;
	}
	//分配内存
	IVS_UINT32 uiBufferSize = (iNum - 1) * sizeof(IVS_ALARM_TYPE) + sizeof(IVS_ALARM_TYPE_LIST);
	IVS_VOID *pBuff = IVS_NEW((IVS_CHAR*&)pBuff, uiBufferSize);
	CHECK_POINTER(pBuff, IVS_OPERATE_MEMORY_ERROR);
	memset(pBuff, 0, uiBufferSize);
	IVS_ALARM_TYPE_LIST* pAlarmTypeList = reinterpret_cast<IVS_ALARM_TYPE_LIST*>(pBuff); //lint !e826 保证空间足够

	pAlarmTypeList->iTotal = iNum;
	// 	IVS_UINT32 uiUserId = 0;
	(void)COCXAlarmMgrXMLProcess::SetAlarmTypeLevelGetXml(xml, pAlarmTypeList);

	IVS_INT32 iRet = IVS_SDK_SetAlarmTypeLevel(iSessionId, pAlarmTypeList);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, " SetAlarmTypeLevel SDK Interface fail", "NA");
		IVS_DELETE(pBuff, MUILI);
		return iRet;
	}
	IVS_DELETE(pBuff, MUILI);

	return iRet;
}

// 查询告警类型列表
IVS_VOID COCXAlarmMgr::GetAlarmTypeList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult)
{
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml fail", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("");

	CXml reqXml;
	//判断是否解析正确
	if (!reqXml.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "FALSE == reqXml.Parse(pReqXml)", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}

	//获取字段的数量 0的话表示没有查询条件，查询所有
	IVS_INT32 iFieldNum = COCXXmlProcess::GetFieldNum(reqXml);

	IVS_VOID *pReqBuffer = NULL;
	//调用通用的方法
	IVS_INT32 iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pReqBuffer, reqXml, iFieldNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Parse GetUnifiedFormatQuery XML fail", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return;
	}

	IVS_QUERY_UNIFIED_FORMAT *pQueryFormat = reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(pReqBuffer);
	//请求的数量小于等于0时
	IVS_INT32 iReqNum = (IVS_INT32)((pQueryFormat->stIndex.uiToIndex - pQueryFormat->stIndex.uiFromIndex) + 1);
	if (0 >= iReqNum)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "0 >= iReqNum","NA");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return;
	}

	IVS_UINT32 uiBufSize = sizeof(IVS_ALARM_TYPE_LIST) + sizeof(IVS_ALARM_TYPE)*(IVS_UINT32)(iReqNum - 1);
	//  要根据这个去分配内存，如果这个过大的话，需要返回参数失败
	IVS_VOID* pBuf = IVS_NEW((IVS_CHAR*&)pBuf, uiBufSize);
	CHECK_POINTER_VOID(pBuf);
	memset(pBuf, 0x0, uiBufSize);
	IVS_ALARM_TYPE_LIST* pAlarmTypeList = (IVS_ALARM_TYPE_LIST*)pBuf;

	//获取list
	iRet = IVS_SDK_GetAlarmTypeList(iSessionId, pQueryFormat, pAlarmTypeList, uiBufSize);
	//根据iRet组装xml然后返回给cu
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pReqBuffer, MUILI);
		BP_RUN_LOG_ERR(iRet, "GetAlarmTypeList SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_DELETE(pBuf, MUILI);
		return;
	}  

	CXml xmlRsp;
	//组装xml
	iRet = COCXAlarmMgrXMLProcess::GetAlarmTypeListGetXml(pAlarmTypeList, xmlRsp, uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmTypeList Get Response Xml fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_DELETE(pBuf, MUILI);
		IVS_DELETE(pReqBuffer, MUILI);
		pAlarmTypeList = NULL;
		return;
	}
	IVS_DELETE(pBuf, MUILI);
	IVS_DELETE(pReqBuffer, MUILI);
	IVS_UINT32 uiLen = 0;
	strResult = xmlRsp.GetXMLStream(uiLen);

	return;
}

// 新增告警订阅
IVS_INT32 COCXAlarmMgr::SubscribeAlarm(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	// 调用SDK接口
	IVS_INT32 iRet = IVS_SDK_SubscribeAlarm(iSessionId, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SubscribeAlarm SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询告警订阅
IVS_VOID COCXAlarmMgr::SubscribeAlarmQuery(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult)
{
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "pReqXml is NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_SubscribeAlarmQuery(iSessionId, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SubscribeAlarmQuery SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}		
		return; 
	}
	iRet = COCXAlarmMgrXMLProcess::GeneralGetString(strResult, pRspXml, iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get RestltToCU String fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_SDK_ReleaseBuffer(pRspXml);
		return; 
	}
	IVS_SDK_ReleaseBuffer(pRspXml); 
	return;
}

// 修改告警防区
IVS_INT32 COCXAlarmMgr::ModifyAlarmArea(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_ModifyAlarmArea(iSessionId, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ModifyAlarmArea SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 删除告警防区
IVS_INT32 COCXAlarmMgr::DeleteAlarmArea(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_DeleteAlarmArea(iSessionId, pDomainCode, uiAlarmAreaId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DeleteAlarmArea SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询告警防区列表
IVS_VOID COCXAlarmMgr::GetAlarmAreaList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult)
{
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "pReqXml is NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetAlarmAreaList(iSessionId, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get AlarmAreaList SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return; 
	}
	iRet = COCXAlarmMgrXMLProcess::GeneralGetString(strResult, pRspXml, iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get ResultXMl TO CU fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_SDK_ReleaseBuffer(pRspXml);
		return; 
	}
	IVS_SDK_ReleaseBuffer(pRspXml);
	return;
}

// 用户认证
IVS_INT32 COCXAlarmMgr::UserAuthentication(IVS_INT32 iSessionId, const IVS_CHAR* pPWD)
{	
	CHECK_POINTER(pPWD, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	//调用SDK
	IVS_INT32 iRet = IVS_SDK_UserAuthentication(iSessionId, pPWD);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "UserAuthentication SDK Interface fail", "NA");
		return iRet; 
	} 
	return iRet;
}
// 指定防区布防计划
IVS_INT32 COCXAlarmMgr::AllocAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_AllocAreaGuardPlan(iSessionId, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AllocAreaGuardPlan SDK Interface is fail", "NA");
		return iRet;		
	}
	return iRet;
}

// 修改防区布防计划
IVS_INT32 COCXAlarmMgr::ModifyAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_ModifyAreaGuardPlan(iSessionId, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ModifyAreaGuardPlan SDK Interface is fail", "NA");
		return iRet;		
	}
	return iRet;
}

// 删除防区布防计划
IVS_INT32 COCXAlarmMgr::DeleteAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_DeleteAreaGuardPlan(iSessionId, pDomainCode, ulAlarmAreaId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DeleteAreaGuardPlan SDK Interface is fail", "NA");
		return iRet;		
	} 
	return iRet;
}

// 获取防区布防计划
IVS_INT32 COCXAlarmMgr::GetAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId, CString& strResult)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetAreaGuardPlan(iSessionId, pDomainCode, ulAlarmAreaId, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAreaGuardPlan SDK Interface is fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}	

	iRet = COCXXmlProcess::GetResultXML(iRet,pRspXml,strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get ResultXML To CU fail", "NA");
		IVS_SDK_ReleaseBuffer(pRspXml);
		return iRet;		
	}

	IVS_SDK_ReleaseBuffer(pRspXml);
	return iRet;
}

// 增加防区
IVS_INT32 COCXAlarmMgr::AddAlarmArea(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml,IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_AddAlarmArea(iSessionId, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddAlarmArea SDK Interface is fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet,pRspXml,strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML To CU fail", "NA");
		IVS_SDK_ReleaseBuffer(pRspXml);
		return iRet;
	}

	(void)IVS_SDK_ReleaseBuffer(pRspXml); 
	return iRet;
}

// 获取防区详细信息
IVS_INT32 COCXAlarmMgr::GetAlarmAreaInfo(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId, CString& strResult)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetAlarmAreaInfo(iSessionId, pDomainCode, ulAlarmAreaId, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmAreaInfo SDK Interface is fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pRspXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML To CU fail", "NA");
		IVS_SDK_ReleaseBuffer(pRspXml);
		return iRet;
	}

	IVS_SDK_ReleaseBuffer(pRspXml); 
	return iRet;
}

// 手动布防
IVS_INT32 COCXAlarmMgr::StartAlarmAreaGuard(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_StartAlarmAreaGuard(iSessionId, pDomainCode, ulAlarmAreaId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, " StartAlarmAreaGuard SDK Interface is error", "NA");
		return iRet;
	} 
	return iRet;
}

// 手动撤防
IVS_INT32 COCXAlarmMgr::StopAlarmAreaGuard(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_StopAlarmAreaGuard(iSessionId, pDomainCode, ulAlarmAreaId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "StopAlarmAreaGuard SDK Interface is error", "NA");
		return iRet;
	}
	return iRet;
}

// 告警授权
IVS_INT32 COCXAlarmMgr::AlarmCommission(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{	
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	// 计算出有多少UserID
	IVS_INT32 iNum = 0;
	(void)COCXAlarmMgrXMLProcess::GetUserNum(pReqXml, iNum);
	if (iNum < 1)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "get num failed ", "NA");
		return IVS_XML_INVALID;
	}
	// 动态分配内存，计算内存大小
	IVS_UINT32 uiBufSize = sizeof(IVS_ALARM_COMMISSION) + (static_cast<IVS_UINT32>(iNum - 1)) * sizeof(IVS_USER_ID_INFO);
	IVS_VOID* pData = IVS_NEW((IVS_CHAR*&)pData, uiBufSize);
	CHECK_POINTER(pData, IVS_ALLOC_MEMORY_ERROR);
	memset(pData, 0x0, uiBufSize);
	IVS_ALARM_COMMISSION* pAlarmCommission = reinterpret_cast<IVS_ALARM_COMMISSION*>(pData);

	pAlarmCommission->uiUserNum = static_cast<IVS_UINT32>(iNum);
	IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::ParseAlarmCommissionXML(pReqXml, szAlarmInCode, pAlarmCommission);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmCommission Parse ReqXML is error", "NA");
		IVS_DELETE(pData, MUILI);
		return iRet;
	}

	iRet = IVS_SDK_AlarmCommission(iSessionId, szAlarmInCode, pAlarmCommission);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, " AlarmCommission SDK Interface is error", "NA");
		IVS_DELETE(pData, MUILI);
		return iRet;
	}
	IVS_DELETE(pData, MUILI);
	return iRet;
}


// 查询告警信息事件
IVS_VOID COCXAlarmMgr::GetAlarmInfo(IVS_INT32 iSessionId, IVS_UINT64 uiAlarmEventId, const IVS_CHAR* pAlarmInCode, CString &strResult)
{
	IVS_DEBUG_TRACE("");
    if (NULL == pAlarmInCode)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pAlarmInCode", "NA");
        COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
        return;
    }
	IVS_ALARM_EVENT stAlarmEvent;
	memset(&stAlarmEvent, 0x0, sizeof(IVS_ALARM_EVENT));

	IVS_INT32 iRet = IVS_SDK_GetAlarmEventInfo(iSessionId, uiAlarmEventId, pAlarmInCode, &stAlarmEvent);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmEventInfo SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}

	iRet = COCXAlarmMgrXMLProcess::GetAlarmInfoGetXML(strResult, &stAlarmEvent);

	return;
}

// 查询告警历史信息列表
IVS_VOID COCXAlarmMgr::GetAlarmList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult)
{ 	
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pReqXml", "NA");
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return;
	}
    IVS_DEBUG_TRACE("");
	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_BP_CMD_RSP_INVALID, "xmlReq.Parse(pReqXml) == NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_SDK_RET_BP_CMD_RSP_INVALID);
		return;
	}

	// 计算出XML中有几个条件字段，即查看有几个查询条件
	IVS_INT32 iNum = COCXXmlProcess::GetFieldNum(xmlReq);

	// 为请求分配内存，解析xml
	IVS_VOID *pBuff = NULL;
	IVS_INT32 iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pBuff, xmlReq, iNum);

	if (IVS_SUCCEED != iRet)
	{	
		BP_RUN_LOG_ERR(iRet, " Parse GetUnifiedFormatQueryXML is fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_DELETE(pBuff, MUILI);
		return;
	}	

	IVS_QUERY_UNIFIED_FORMAT* pQueryUnifiedFormat = reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(pBuff);

	IVS_UINT32 uiReqNum = ((pQueryUnifiedFormat->stIndex).uiToIndex - (pQueryUnifiedFormat->stIndex).uiFromIndex) + 1;
	IVS_INT32 iReqNum = static_cast<IVS_INT32>(uiReqNum);

	// 判断请求的页数是否小于等于0
	if (0 >= iReqNum)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "0 >= iReqNum", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		IVS_DELETE(pBuff, MUILI);
		return;
	}

	// 为结果LIST分配内存
	IVS_UINT32 uiBufSize = (uiReqNum - 1) * sizeof(IVS_ALARM_EVENT) + sizeof(IVS_ALARM_EVENT_LIST);

	IVS_VOID* pAlarmEventListBuf = IVS_NEW((IVS_CHAR*&)pAlarmEventListBuf, uiBufSize);    
	if (NULL == pAlarmEventListBuf)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pAlarmEventListBuf is NULL", "NA");
		IVS_DELETE(pBuff, MUILI);
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return;
	}
	memset(pAlarmEventListBuf, 0x0, uiBufSize);
	IVS_ALARM_EVENT_LIST* pResAlarmEventList = reinterpret_cast<IVS_ALARM_EVENT_LIST*>(pAlarmEventListBuf);
	// 调用SDK接口
	iRet = IVS_SDK_GetAlarmEventList(iSessionId, pQueryUnifiedFormat, pResAlarmEventList, uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, " GetAlarmList SDK Interface is fail", "NA");
		IVS_DELETE(pAlarmEventListBuf, MUILI);	
		IVS_DELETE(pBuff, MUILI);
		COCXXmlProcess::GetErrString(strResult,iRet);
		return;
	}	

	// 拼装响应消息结构体为XML
	
	CXml xmlRsp;
	iRet = COCXAlarmMgrXMLProcess::GetAlarmListGetXML(strResult, pResAlarmEventList, iRet, xmlRsp);
	IVS_DELETE(pAlarmEventListBuf, MUILI);	
	IVS_DELETE(pBuff, MUILI);
	 
	return;
}


// 告警撤销
IVS_INT32 COCXAlarmMgr::AlarmCancelORConfirm(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, const IVS_INT32& iType)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) = NULL", "NA");
		return IVS_XML_INVALID;
	}

	IVS_UINT64 ullAlarmEventID = 0;
	IVS_ALARM_OPERATE_INFO pAlarmOperateInfo;
	memset(&pAlarmOperateInfo, 0x0, sizeof(IVS_ALARM_OPERATE_INFO));
	IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::AlarmConfirmParseXML(&ullAlarmEventID, &pAlarmOperateInfo, szAlarmInCode, xmlReq);
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmConfirm ParseReqXML fail", "NA");
		return iRet;
	}
	// iType是0，调用告警撤销接口，若是1，调用告警确认接口
	if(0 == iType)
	{
		iRet = IVS_SDK_AlarmCancel(iSessionId, ullAlarmEventID, szAlarmInCode, &pAlarmOperateInfo);
	}
	else if (1 == iType)
	{
		iRet = IVS_SDK_AlarmConfirm(iSessionId, ullAlarmEventID, szAlarmInCode, &pAlarmOperateInfo);
	}	
	 
	return iRet;
}


// 告警信息上报CU->SMU
IVS_INT32 COCXAlarmMgr::AlarmReport(IVS_INT32 iSessionId, const IVS_CHAR* pReqXML)
{	
	CHECK_POINTER(pReqXML, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_ALARM_REPORT stAlarmReport;
	memset(&stAlarmReport, 0x0, sizeof(IVS_ALARM_REPORT));

	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::ParseAlarmReportXML(pReqXML, &stAlarmReport);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Parse AlarmReport Request XML is error", "NA");
		return iRet;
	}

	iRet = IVS_SDK_AlarmReport(iSessionId, &stAlarmReport);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmReport SDK Interface is error", "NA");
		return iRet;
	}
	return iRet;
}
#define STEPINDEX 256
// 获取拥有指定设备权限的用户列表
IVS_VOID COCXAlarmMgr::GetDeviceUserList(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, IVS_UINT32 ulIsUserOnline, IVS_UINT32 uiFromIndex, IVS_UINT32 uiToIndex, CString &strResult)
{
	if (NULL == pDevCode || NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "pDevCode is NULL or pDomainCode is NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("iSessionId: %d, pDomainCode: %s, pDevCode: %s", iSessionId, pDomainCode, pDevCode);
	IVS_INDEX_RANGE pIndexRange;
	memset(&pIndexRange, 0x0, sizeof(IVS_INDEX_RANGE));
	pIndexRange.uiFromIndex = uiFromIndex;
	IVS_UINT32 uiNumber = (uiToIndex - uiFromIndex) + 1;
	IVS_INT32 iNumber = static_cast<IVS_INT32>(uiNumber);
	// 判断请求的页数是否小于等于0
	if (0 >= iNumber)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "0 >= iNumber", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}

	IVS_UINT32 uiNum = 0;
	if (0 == uiNumber % STEPINDEX)
	{
		uiNum = uiNumber / STEPINDEX;
	}
	else
	{
		uiNum = uiNumber / STEPINDEX + 1;
	}
	IVS_UINT32 ui = 0;
	std::list<IVS_DEVICE_USER_INFO_LIST*> listDevUserList;
	pIndexRange.uiToIndex = STEPINDEX;
	IVS_INT32 iRet = IVS_FAIL;
	while(ui < uiNum)
	{
		// 为结果LIST分配内存
		IVS_UINT32 uiBufSize = (STEPINDEX - 1) * sizeof(IVS_DEVICE_USER_INFO) + sizeof(IVS_DEVICE_USER_INFO_LIST);
		IVS_VOID* pDeviceUserInfoList = IVS_NEW((IVS_CHAR*&)pDeviceUserInfoList, uiBufSize);
		if (NULL == pDeviceUserInfoList)
		{
			(void)ReleaseListBuffer(listDevUserList);//lint !e64
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pDeviceUserInfoList is NULL", "NA");
			COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
			return;
		}
		memset(pDeviceUserInfoList, 0x0, uiBufSize);
		IVS_DEVICE_USER_INFO_LIST* pDeviceUserInfoListTemp = reinterpret_cast<IVS_DEVICE_USER_INFO_LIST*>(pDeviceUserInfoList);
		// 调用SDK接口
		iRet = IVS_SDK_GetDeviceUserList(iSessionId, pDomainCode, pDevCode, ulIsUserOnline, &pIndexRange, pDeviceUserInfoListTemp, uiBufSize);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "GetDeviceUserList SDK Interface is fail", "NA");
			(void)ReleaseListBuffer(listDevUserList);//lint !e64
			IVS_DELETE(pDeviceUserInfoList, MUILI);	
			COCXXmlProcess::GetErrString(strResult,iRet);
			return;
		}
		listDevUserList.push_back(pDeviceUserInfoListTemp);
		pIndexRange.uiFromIndex += STEPINDEX;
		pIndexRange.uiToIndex += STEPINDEX;
		ui++;
		// 达到总数，可以返回
		if (pDeviceUserInfoListTemp->uiTotal <= (ui * STEPINDEX))
		{
			BP_RUN_LOG_INF("have all info", "NA");
			break;
		}
		
	}
	// 拼装响应消息结构体为XML
	CXml xmlRsp;
	iRet = COCXAlarmMgrXMLProcess::GetDeviceUserList(listDevUserList, strResult, uiFromIndex, uiToIndex, xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDeviceUserList is fail", "NA");
		COCXXmlProcess::GetErrString(strResult,iRet);
	}
	(void)ReleaseListBuffer(listDevUserList);//lint !e64	
	return;
}

// 获取平台配置信息
IVS_VOID COCXAlarmMgr::GetConfigInfo(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult)
{	
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "pReqXml = NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pRsqXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetConfigInfo(iSessionId, pReqXml, &pRsqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetConfigInfo SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		if (NULL != pRsqXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pRsqXml);
		}
		
		return; 
	}
	iRet = COCXAlarmMgrXMLProcess::AddResultCodeUp(pRsqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddResultCode To RspXML fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_SDK_ReleaseBuffer(pRsqXml);
		return; 
	}
	IVS_SDK_ReleaseBuffer(pRsqXml);

	return;
}

// 获取平台配置信息
IVS_ULONG COCXAlarmMgr::GetAbilityInfo(IVS_INT32 iSessionId)
{	
	
	IVS_UINT32 uiAbility = 0;
	IVS_INT32 iRet = IVS_SDK_GetAbilityInfo(iSessionId,&uiAbility);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAbilityInfoL fail", "NA");
		return 0; // 错误情况下不返回错误码，直接返回无能力 
	}
	return uiAbility;
}

// 启动开关量输出告警
IVS_INT32 COCXAlarmMgr::StartAlarmOut(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmOutCode)
{
	if (NULL == pAlarmOutCode)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pReqXml = NULL", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_StartAlarmOut(iSessionId, pAlarmOutCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "StartAlarmOut SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 停止开关量输出告警
IVS_INT32 COCXAlarmMgr::StopAlarmOut(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmOutCode)
{
	if (NULL == pAlarmOutCode)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pReqXml = NULL", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_StopAlarmOut(iSessionId, pAlarmOutCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "StopAlarmOut SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询OMU告警事件列表
IVS_INT32 COCXAlarmMgr::GetDeviceAlarmList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult)
{
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pReqXml = NULL", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	IVS_DEBUG_TRACE("");
	CXml xmlRet;
	if (!xmlRet.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRet.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	IVS_INT32 iNum = COCXXmlProcess::GetFieldNum(xmlRet);
	// 为请求分配内存，解析xml
	IVS_VOID *pBuff = NULL;
	IVS_INT32 iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pBuff,xmlRet,iNum);

	if (IVS_SUCCEED != iRet)
	{	
		// 如果IVS_SUCCEED != iRet, pBuff的内存在GetUnifiedFormatQueryParseXML已经释放

		BP_RUN_LOG_ERR(iRet, "Get Device AlarmList", "Get Unified Format Query Parse XML is fail");
		return iRet;
	}
	IVS_QUERY_UNIFIED_FORMAT* pQueryUnifiedFormat = reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(pBuff);

	// 为List分配内存，判断FromIndex和ToIndex的合法性
	IVS_UINT32 uiReqNum = ((pQueryUnifiedFormat->stIndex).uiToIndex - (pQueryUnifiedFormat->stIndex).uiFromIndex) + 1;
	IVS_INT32 iReqNum = static_cast<IVS_INT32>(uiReqNum);

	// 判断请求的页数是否小于等于0
	if (0 >= iReqNum)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get DeviceAlarmList", "0 >= iReqNum");
		IVS_DELETE(pBuff);
		return iRet;
	}

	// 为结果LIST分配内存
	IVS_UINT32 uiBufSize = (uiReqNum - 1) * sizeof(IVS_DEVICE_ALARM_NOTIFY) + sizeof(IVS_DEVICE_ALARM_EVENT_LIST);

	IVS_VOID* pResBuf = IVS_NEW((IVS_CHAR*&)pResBuf,uiBufSize);    
	if (NULL == pResBuf)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get DevAlarmList", "new Response Device Alarm List Buffer failed");
		IVS_DELETE(pResBuf);
		return iRet;
	}
	memset(pResBuf, 0x0, uiBufSize);
	IVS_DEVICE_ALARM_EVENT_LIST* pDevAlarmList = reinterpret_cast<IVS_DEVICE_ALARM_EVENT_LIST*>(pResBuf);
	// 调用SDK接口
	iRet = IVS_SDK_GetDeviceAlarmList(iSessionId,pQueryUnifiedFormat,pDevAlarmList,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get DevAlarm List","ivs sdk Get DevAlarm List failed");
		IVS_DELETE(pResBuf,MUILI);	
		IVS_DELETE(pBuff,MUILI);
		return iRet;
	}	

	// 拼装响应消息结构体为XML，
	CXml xmlRes;
	if (IVS_SUCCEED != COCXAlarmMgrXMLProcess::GetDevAlarmListGetXML(xmlRes, pDevAlarmList, iRet))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get DevAlarm List","Get Response DevAlarm List XML is fail");
		IVS_DELETE(pResBuf,MUILI);	
		IVS_DELETE(pBuff,MUILI);
		return IVS_SMU_XML_INVALID;
	}

	IVS_UINT32 xmlLen = 0;
	strResult = xmlRes.GetXMLStream(xmlLen);

	IVS_DELETE(pResBuf,MUILI);	
	IVS_DELETE(pBuff,MUILI);
	return iRet;
}

// 设备布撤防
IVS_INT32 COCXAlarmMgr::SetDeviceGuard(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_SetDeviceGuard(iSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Device Guard SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 获取告警抓拍图片列表
IVS_INT32 COCXAlarmMgr::GetAlarmSnapshotList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRsqXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetAlarmSnapshotList(iSessionID, pReqXml, &pRsqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Alarm Snap shot List fail", "NA");
		if (NULL != pRsqXml)
		{
			IVS_SDK_ReleaseBuffer(pRsqXml);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return iRet;
	}
	if (NULL == pRsqXml)
	{
		BP_RUN_LOG_INF("SDK interface succeed, but no response xml", "NA");
		(void)COCXAlarmMgrXMLProcess::GetRspXml(strResult, iRet);
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pRsqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Result XML fail", "NA");
		IVS_SDK_ReleaseBuffer(pRsqXml);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return iRet;
	}
	IVS_SDK_ReleaseBuffer(pRsqXml);
	return iRet;
}

// 报警复位
IVS_INT32 COCXAlarmMgr::ResetAlarm(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_ResetAlarm(iSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Reset Alarm SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 外部域告警事件订阅
IVS_INT32 COCXAlarmMgr::ExEventSubscribe(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_ExEventSubscribe(iSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Ex Event Subscribe SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询外部域告警事件订阅
IVS_INT32 COCXAlarmMgr::GetExEventSubscribe(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRsqXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetExEventSubscribe(iSessionID, pReqXml, &pRsqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Ex Event Subscribe List SDK Interface fail", "NA");
		if (NULL != pRsqXml)
		{
			IVS_SDK_ReleaseBuffer(pRsqXml);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pRsqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Ex Event Subscribe List Get Result XML fail", "NA");
		IVS_SDK_ReleaseBuffer(pRsqXml);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return iRet;
	}
	IVS_SDK_ReleaseBuffer(pRsqXml);
	return iRet;
}

// 外部域设备目录订阅
IVS_INT32 COCXAlarmMgr::ExCatalogSubscribe(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_ExCatalogSubscribe(iSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Ex Catalog Subscribe SDK Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询外部域设备目录订阅
IVS_INT32 COCXAlarmMgr::GetExCatalogSubscribeList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRsqXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetExCatalogSubscribeList(iSessionID, pReqXml, &pRsqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Ex Catalog Subscribe List SDK Interface fail", "NA");
		if (NULL != pRsqXml)
		{
			IVS_SDK_ReleaseBuffer(pRsqXml);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pRsqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Ex Catalog Subscribe List Get Result XML fail", "NA");
		IVS_SDK_ReleaseBuffer(pRsqXml);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return iRet;
	}
	IVS_SDK_ReleaseBuffer(pRsqXml);
	return iRet;
}

IVS_INT32 COCXAlarmMgr::ReleaseListBuffer(std::list<IVS_DEVICE_USER_INFO_LIST*> listDevUserInfoList)
{//lint !e18 !e1039
	std::list<IVS_DEVICE_USER_INFO_LIST*>::iterator InfoIter = listDevUserInfoList.begin();
	std::list<IVS_DEVICE_USER_INFO_LIST*>::iterator InfoIterEnd = listDevUserInfoList.end();
	IVS_DEVICE_USER_INFO_LIST* pDeviceUserList = NULL;
	for (; InfoIter != InfoIterEnd; InfoIter++)
	{
		pDeviceUserList = (IVS_DEVICE_USER_INFO_LIST*)(*InfoIter);
		IVS_DELETE(pDeviceUserList, MUILI);
	}
	return IVS_SUCCEED;
}//lint !e18 !e1039
