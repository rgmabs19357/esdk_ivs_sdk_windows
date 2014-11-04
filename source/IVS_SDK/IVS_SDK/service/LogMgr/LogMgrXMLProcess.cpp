#include "LogMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
#include "ParaCheck.h"

CLogMgrXMLProcess::CLogMgrXMLProcess()
{
}

CLogMgrXMLProcess::~CLogMgrXMLProcess()
{
}

// 解析响应xml
IVS_INT32 CLogMgrXMLProcess::PraseResOperationLogXML(CXml& xml,IVS_OPERATION_LOG_LIST* pResOperationLog,const IVS_UINT32& uiBufSize)
{
	CHECK_POINTER(pResOperationLog, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML ", "xml.FindElem(Content) is fail");
		return IVS_XML_INVALID;
	}   
	xml.IntoElem();
	if (!xml.FindElem("PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "xml.FindElem(PageInfo) is fail");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	const char* szElemValue = NULL;
	IVS_INT32 iTotal = 0;
	IVS_INT32 iFromIndex = 0;
	IVS_INT32 iToIndex = 0;
	GET_ELEM_VALUE_NUM("RealNum", szElemValue,iTotal , xml);
	GET_ELEM_VALUE_NUM("FromIndex", szElemValue, iFromIndex, xml);
	GET_ELEM_VALUE_NUM("ToIndex",szElemValue,iToIndex , xml);

	// 判断RealNum，FromIndex，ToIndex是否合法
	if (iTotal < 0 || iToIndex < 0 || iFromIndex < 0 || iFromIndex > iToIndex)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "ToIndex[%d], FromIndex[%d], ToIndex[%d]", iTotal, iFromIndex, iToIndex);
		return IVS_XML_INVALID;
	}	

	pResOperationLog->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromIndex);
	pResOperationLog->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToIndex);
	pResOperationLog->uiTotal = static_cast<IVS_UINT32>(iTotal);
	xml.OutOfElem();	
	if (0 == iTotal)
	{
		return IVS_SUCCEED;
	}
	if (!xml.FindElem("OperationLogList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "Find Elememt(OperationLogList) failed");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();  
	// 响应消息中传回来的页数
	IVS_UINT32 uiResNum = ((pResOperationLog->stIndexRange).uiToIndex - (pResOperationLog->stIndexRange).uiFromIndex) + 1;
	// 实际要用到的内存大小
	IVS_UINT32 uiResBufSize = sizeof(IVS_OPERATION_LOG_LIST) + ((uiResNum) - 1) * sizeof(IVS_OPERATION_LOG_INFO);
	// 实际需要内存与分配内存大小的比较
	if (uiBufSize < uiResBufSize)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Prase Response Operation Log XML","memory is not enough");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_UINT32 i = 0;	

	if (!xml.FindElem("OperationLogInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "Find Elememt(OperationLogInfo) failed");
		return IVS_XML_INVALID;
	}  
	while(i < uiResNum)
	{
		IVS_OPERATION_LOG_INFO &pOperationLogInfo = pResOperationLog->stOperationInfo[i];
		xml.IntoElem();		
		IVS_INT32 iTemp = 0;
		GET_ELEM_VALUE_CHAR("Account", szElemValue,  pOperationLogInfo.cAccount, (IVS_NAME_LEN), xml);
		GET_ELEM_VALUE_CHAR("ClientIP", szElemValue, pOperationLogInfo.cClientIp, (IVS_IP_LEN), xml);
		GET_ELEM_VALUE_NUM("Grade", szElemValue, iTemp,xml);
		pOperationLogInfo.iGrade = static_cast<IVS_UINT32>(iTemp);
		GET_ELEM_VALUE_CHAR("ServiceCode", szElemValue,pOperationLogInfo.cServiceCode, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_CHAR("ModuleType", szElemValue,pOperationLogInfo.cModuleType, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_CHAR("ModuleCode", szElemValue,pOperationLogInfo.cModuleCode, (IVS_DEV_CODE_LEN), xml);
		GET_ELEM_VALUE_CHAR("OperationCode", szElemValue,pOperationLogInfo.cOperationCode, (IVS_DEV_CODE_LEN), xml);
		GET_ELEM_VALUE_CHAR("OperationObjectCode", szElemValue,pOperationLogInfo.cOperationObjectCode, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_CHAR("OccurTime", szElemValue,pOperationLogInfo.cOccurTime, (IVS_TIME_LEN),xml);
		GET_ELEM_VALUE_CHAR("ErrorCode", szElemValue,pOperationLogInfo.cErrorCode, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_NUM("Result", szElemValue, iTemp, xml);
		pOperationLogInfo.iResult = static_cast<IVS_UINT32>(iTemp);
		GET_ELEM_VALUE_CHAR("AdditionalInfo", szElemValue,pOperationLogInfo.cAdditionalInfo, (IVS_DESCRIBE_LEN), xml);
		xml.OutOfElem();
		if (!xml.NextElem())
		{
			break;
		}
		i++;
	}    

	xml.OutOfElem();
	return IVS_SUCCEED;
}


//拼装查询操作日志请求Xml
IVS_INT32 CLogMgrXMLProcess::GetResOperationLogExXML(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
	const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
	CXml &xml)
{
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR); 
	CHECK_POINTER(pOptLogQueryEx, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("GetResOperationLogExXML NVRCode: %s, ModuleName: %s", 
		pOptLogQueryEx->cNVRCode, pOptLogQueryEx->cModuleName);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	
	(void)xml.AddChildElem("LoginInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("LoginID");
	(void)xml.IntoElem();
	IVS_CHAR cLoginID[IVS_NAME_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cLoginID, IVS_NAME_LEN + 1, pOptLogQueryEx->cLoginID, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "GetResOperationLogExXML failed", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cLoginID);
	(void)xml.OutOfElem();

	(void)xml.AddElem("PageInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("FromIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pUnifiedQuery->stIndex).uiFromIndex)).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pUnifiedQuery->stIndex).uiToIndex)).c_str());

	IVS_UINT32 iFieldNum = static_cast<IVS_UINT32>(pUnifiedQuery->iFieldNum);	

	if (iFieldNum > 0) //如果是0的话，这个QueryCond节点都不要了
	{
		IVS_CHAR szValue[IVS_QUERY_VALUE_LEN+1]={0};
		(void)xml.AddElem("QueryCond");	
		for (unsigned int i=0;i < iFieldNum;i++)
		{	
			if (0 == i)
			{
				(void)xml.AddChildElem("QueryField");
			}
			else
			{
				(void)xml.AddElem("QueryField");	
			}

			(void)xml.IntoElem();
			(void)xml.AddChildElem("Field");
			(void)xml.IntoElem();

			std::string  iField = QUERYFILED.GetValue(pUnifiedQuery->stQueryField[i].eFieID);
			(void)xml.SetElemValue(iField.c_str());
			(void)xml.AddElem("Value");
			if (!CToolsHelp::Memcpy(szValue, IVS_QUERY_VALUE_LEN, pUnifiedQuery->stQueryField[i].cValue, IVS_QUERY_VALUE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Unified Format Query XML", "Memcpy szValue failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(szValue);

			//注意如果这里的值不是0/1，直接返回参数错误的错误码
			CHECK_IVSBOOL(pUnifiedQuery->stQueryField[i].bExactQuery);

			(void)xml.AddElem("QueryType");
			if (MATCHED_SUCCEED == pUnifiedQuery->stQueryField[i].bExactQuery)
			{
				(void)xml.SetElemValue("EXACT");
			}
			else
			{
				(void)xml.SetElemValue("INEXACT");
			}
			xml.OutOfElem();
		}
		xml.OutOfElem();

		//注意如果这里的值不是0/1，直接返回参数错误的错误码
		CHECK_IVSBOOL(pUnifiedQuery->stOrderCond.bEnableOrder);

		if (pUnifiedQuery->stOrderCond.bEnableOrder)
		{
			(void)xml.AddElem("OrderCond");
			(void)xml.AddChildElem("OrderField");
			(void)xml.IntoElem();	
			std::string iOrderField = QUERYFILED.GetValue(pUnifiedQuery->stOrderCond.eFieID);
			(void)xml.SetElemValue(iOrderField.c_str());

			//注意如果这里的值不是0/1，直接返回参数错误的错误码
			CHECK_IVSBOOL(pUnifiedQuery->stOrderCond.bUp);

			(void)xml.AddElem("Order");
			if (MATCHED_SUCCEED==pUnifiedQuery->stOrderCond.bUp)
			{
				(void)xml.SetElemValue("UP");
			}
			else
			{
				(void)xml.SetElemValue("DOWN");
			}
			xml.OutOfElem();
		}
	}
	xml.OutOfElem();

	(void)xml.AddElem("NodeCode");
	IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cNVRCode, IVS_NVR_CODE_LEN + 1, pOptLogQueryEx->cNVRCode, IVS_NVR_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "GetResOperationLogExXML failed", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cNVRCode);

	(void)xml.AddElem("ModuleName");
	IVS_CHAR cTmpModuleName[IVS_DESCRIBE_LEN + 1] = {0};
	CToolsHelp::Strncpy(cTmpModuleName, IVS_DESCRIBE_LEN + 1, pOptLogQueryEx->cModuleName, IVS_DESCRIBE_LEN);
	(void)xml.SetElemValue(cTmpModuleName);
	(void)xml.AddElem("PortalClientIP");
	IVS_CHAR cTmpIP[IVS_IP_LEN + 1] = {0};
	CToolsHelp::Strncpy(cTmpIP, IVS_IP_LEN + 1, pOptLogQueryEx->cPortalClientIP, IVS_IP_LEN);
	(void)xml.SetElemValue(cTmpIP);

	return IVS_SUCCEED;
}


//解析查询操作日志响应Xml
IVS_INT32 CLogMgrXMLProcess::ParseRspOperationLogExXML(CXml& xml,
	IVS_OPERATION_LOG_LIST* pResOperationLog, 
	IVS_UINT32 uiBufSize)
{
	CHECK_POINTER(pResOperationLog, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML ", "xml.FindElem(Content) is fail");
		return IVS_XML_INVALID;
	}   
	xml.IntoElem();
	if (!xml.FindElem("PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "xml.FindElem(PageInfo) is fail");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	memset(pResOperationLog, 0, uiBufSize);
	const char* szElemValue = NULL;
	IVS_INT32 iTotal = 0;
	IVS_INT32 iFromIndex = 0;
	IVS_INT32 iToIndex = 0;
	GET_ELEM_VALUE_NUM("RealNum", szElemValue,iTotal , xml);
	GET_ELEM_VALUE_NUM("FromIndex", szElemValue, iFromIndex, xml);
	GET_ELEM_VALUE_NUM("ToIndex",szElemValue,iToIndex , xml);

	// 判断RealNum，FromIndex，ToIndex是否合法
	if (iTotal < 0 || iToIndex < 0 || iFromIndex < 0 || iFromIndex > iToIndex)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "ToIndex[%d], FromIndex[%d], ToIndex[%d]", iTotal, iFromIndex, iToIndex);
		return IVS_XML_INVALID;
	}	

	pResOperationLog->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromIndex);
	pResOperationLog->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToIndex);
	pResOperationLog->uiTotal = static_cast<IVS_UINT32>(iTotal);
	xml.OutOfElem();	
	if (0 == iTotal)
	{
		return IVS_SUCCEED;
	}
	if (!xml.FindElem("OperationLogList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "Find Elememt(OperationLogList) failed");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();  
	// 响应消息中传回来的页数
	IVS_UINT32 uiResNum = ((pResOperationLog->stIndexRange).uiToIndex - (pResOperationLog->stIndexRange).uiFromIndex) + 1;
	// 实际要用到的内存大小
	IVS_UINT32 uiResBufSize = sizeof(IVS_OPERATION_LOG_LIST) + ((uiResNum) - 1) * sizeof(IVS_OPERATION_LOG_INFO);
	// 实际需要内存与分配内存大小的比较
	if (uiBufSize < uiResBufSize)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Prase Response Operation Log XML","memory is not enough");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_UINT32 i = 0;	

	if (!xml.FindElem("OperationLogInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "Find Elememt(OperationLogInfo) failed");
		return IVS_XML_INVALID;
	}  
	while(i < uiResNum)
	{
		IVS_OPERATION_LOG_INFO &pOperationLogInfo = pResOperationLog->stOperationInfo[i];
		xml.IntoElem();		
		IVS_INT32 iTemp = 0;
		GET_ELEM_VALUE_CHAR("Account", szElemValue,  pOperationLogInfo.cAccount, (IVS_NAME_LEN), xml);
		GET_ELEM_VALUE_CHAR("ClientIP", szElemValue, pOperationLogInfo.cClientIp, (IVS_IP_LEN), xml);
		GET_ELEM_VALUE_NUM("Grade", szElemValue, iTemp,xml);
		pOperationLogInfo.iGrade = static_cast<IVS_UINT32>(iTemp);
		GET_ELEM_VALUE_CHAR("ServiceCode", szElemValue,pOperationLogInfo.cServiceCode, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_CHAR("ModuleType", szElemValue,pOperationLogInfo.cModuleType, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_CHAR("ModuleCode", szElemValue,pOperationLogInfo.cModuleCode, (IVS_DEV_CODE_LEN), xml);
		GET_ELEM_VALUE_CHAR("OperationCode", szElemValue,pOperationLogInfo.cOperationCode, (IVS_DEV_CODE_LEN), xml);
		GET_ELEM_VALUE_CHAR("OperationObjectCode", szElemValue,pOperationLogInfo.cOperationObjectCode, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_CHAR("OccurTime", szElemValue,pOperationLogInfo.cOccurTime, (IVS_TIME_LEN),xml);
		GET_ELEM_VALUE_CHAR("ErrorCode", szElemValue,pOperationLogInfo.cErrorCode, (IVS_DESCRIBE_LEN), xml);
		GET_ELEM_VALUE_NUM("Result", szElemValue, iTemp, xml);
		pOperationLogInfo.iResult = static_cast<IVS_UINT32>(iTemp);
		GET_ELEM_VALUE_CHAR("AdditionalInfo", szElemValue,pOperationLogInfo.cAdditionalInfo, (IVS_DESCRIBE_LEN), xml);
		xml.OutOfElem();
		if (!xml.NextElem())
		{
			break;
		}
		i++;
	}    

	xml.OutOfElem();
	return IVS_SUCCEED;
}