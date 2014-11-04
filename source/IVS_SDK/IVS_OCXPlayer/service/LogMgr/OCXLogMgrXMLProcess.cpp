#include "OCXLogMgrXMLProcess.h"
#include "bp_log.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"

// 拼装查询操作日志响应的xml
IVS_INT32 COCXLogMgrXMLProcess::GetResOperationLogXML(IVS_OPERATION_LOG_LIST* pOperationLogList, CXml& xmlRes)
{
	CHECK_POINTER(pOperationLogList, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	
	(void)xmlRes.AddDeclaration("1.0","UTF-8","");
	(void)xmlRes.AddElem("Content");
	(void)xmlRes.AddChildElem("ResultCode");
	(void)xmlRes.IntoElem();
	(void)xmlRes.SetElemValue("0");	

	(void)xmlRes.AddElem("PageInfo");
	(void)xmlRes.IntoElem();
	(void)xmlRes.AddChildElem("RealNum");
	(void)xmlRes.IntoElem();

	(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pOperationLogList->uiTotal)).c_str());	
	(void)xmlRes.AddElem("FromIndex");
	(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pOperationLogList->stIndexRange).uiFromIndex)).c_str());	

	(void)xmlRes.AddElem("ToIndex");
	(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pOperationLogList->stIndexRange).uiToIndex)).c_str());	
	xmlRes.OutOfElem();

	(void)xmlRes.AddElem("OperationLogList");

	
	IVS_UINT32 uiFromIndex = pOperationLogList->stIndexRange.uiFromIndex;
	IVS_UINT32 uiToIndex = pOperationLogList->stIndexRange.uiToIndex;
	IVS_UINT32 uiTotal = pOperationLogList->uiTotal;
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
	IVS_UINT32  i = 0;
	while(i < uiRestltNum)
	{
		IVS_OPERATION_LOG_INFO &pOperationLogInfo = pOperationLogList->stOperationInfo[i];
		i == 0?(void)xmlRes.AddChildElem("OperationLogInfo"):(void)xmlRes.AddElem("OperationLogInfo");
		(void)xmlRes.IntoElem();
		(void)xmlRes.AddChildElem("Account");
		(void)xmlRes.IntoElem();

		IVS_CHAR szAccount[IVS_NAME_LEN + 1];
		memset(szAccount, 0x0, IVS_NAME_LEN + 1);
		if (!CToolsHelp::Memcpy(szAccount, IVS_NAME_LEN, pOperationLogInfo.cAccount, IVS_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cAccount failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.SetElemValue(szAccount);

		IVS_CHAR szClientIP[IVS_IP_LEN + 1];
		memset(szClientIP, 0x0, IVS_IP_LEN + 1);
		if (!CToolsHelp::Memcpy(szClientIP, IVS_IP_LEN, pOperationLogInfo.cClientIp, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cClientIp failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("ClientIP");
		(void)xmlRes.SetElemValue(szClientIP);

		(void)xmlRes.AddElem("Grade");
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pOperationLogInfo.iGrade)).c_str());

		IVS_CHAR szServiceCode[IVS_DESCRIBE_LEN + 1];
		memset(szServiceCode, 0x0, IVS_DESCRIBE_LEN + 1);
		if (!CToolsHelp::Memcpy(szServiceCode, IVS_DESCRIBE_LEN, pOperationLogInfo.cServiceCode, IVS_DESCRIBE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cServiceCode failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("ServiceCode");
		(void)xmlRes.SetElemValue(szServiceCode);

		IVS_CHAR szModuleType[IVS_DESCRIBE_LEN + 1];
		memset(szModuleType, 0x0, IVS_DESCRIBE_LEN + 1);
		if (!CToolsHelp::Memcpy(szModuleType, IVS_DESCRIBE_LEN, pOperationLogInfo.cModuleType, IVS_DESCRIBE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cModuleType failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("ModuleType");
		(void)xmlRes.SetElemValue(szModuleType);

		IVS_CHAR szModuleCode[IVS_DEV_CODE_LEN + 1];
		memset(szModuleCode, 0x0, IVS_DEV_CODE_LEN + 1);
		if (!CToolsHelp::Memcpy(szModuleCode, IVS_DEV_CODE_LEN, pOperationLogInfo.cModuleCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cModuleCode failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("ModuleCode");
		(void)xmlRes.SetElemValue(szModuleCode); 

		IVS_CHAR szOperationCode[IVS_DEV_CODE_LEN + 1];
		memset(szOperationCode, 0x0, IVS_DEV_CODE_LEN + 1);
		if (!CToolsHelp::Memcpy(szOperationCode, IVS_DEV_CODE_LEN, pOperationLogInfo.cOperationCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cOperationCode failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("OperationCode");
		(void)xmlRes.SetElemValue(szOperationCode);

		IVS_CHAR szOperationObjectCode[IVS_DESCRIBE_LEN + 1];
		memset(szOperationObjectCode, 0x0, IVS_DESCRIBE_LEN + 1);
		if (!CToolsHelp::Memcpy(szOperationObjectCode, IVS_DESCRIBE_LEN, pOperationLogInfo.cOperationObjectCode, IVS_DESCRIBE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cOperationCode failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("OperationObjectCode");
		(void)xmlRes.SetElemValue(szOperationObjectCode);

		IVS_CHAR szOccurTime[IVS_TIME_LEN + 1];
		memset(szOccurTime, 0x0, IVS_TIME_LEN + 1);
		if (!CToolsHelp::Memcpy(szOccurTime, IVS_TIME_LEN, pOperationLogInfo.cOccurTime, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy szOccurTime failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("OccurTime");
		(void)xmlRes.SetElemValue(szOccurTime);

		IVS_CHAR szErrorCode[IVS_DESCRIBE_LEN + 1];
		memset(szErrorCode, 0x0, IVS_DESCRIBE_LEN + 1);
		if (!CToolsHelp::Memcpy(szErrorCode, IVS_DESCRIBE_LEN, pOperationLogInfo.cErrorCode, IVS_DESCRIBE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cErrorCode failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("ErrorCode");
		(void)xmlRes.SetElemValue(szErrorCode);

		(void)xmlRes.AddElem("Result");
		(void)xmlRes.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pOperationLogInfo.iResult)).c_str());	

		IVS_CHAR szAdditionalInfo[IVS_DESCRIBE_LEN + 1];
		memset(szAdditionalInfo, 0x0, IVS_DESCRIBE_LEN + 1);
		if (!CToolsHelp::Memcpy(szAdditionalInfo, IVS_DESCRIBE_LEN, pOperationLogInfo.cAdditionalInfo, IVS_DESCRIBE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Response Operation Log XML", "Memcpy cAdditionalInfo failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRes.AddElem("AdditionalInfo");
		(void)xmlRes.SetElemValue(szAdditionalInfo);

		xmlRes.OutOfElem();
		i++;
	}

	xmlRes.OutOfElem();
	xmlRes.OutOfElem();

	return IVS_SUCCEED;
}
