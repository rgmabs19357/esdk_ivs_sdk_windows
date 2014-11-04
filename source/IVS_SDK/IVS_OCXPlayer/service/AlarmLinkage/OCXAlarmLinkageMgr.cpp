#include "stdafx.h"
#include "SDKDef.h"
#include "OCXAlarmLinkageMgr.h"
#include "hwsdk.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "OCXAlarmMgrXMLProcess.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"
COCXAlarmLinkageMgr::COCXAlarmLinkageMgr()
{
}

COCXAlarmLinkageMgr::~COCXAlarmLinkageMgr()
{
}

// 新增告警联动策略
IVS_INT32 COCXAlarmLinkageMgr::AddAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_AddAlarmLinkage(iSessionId, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddAlarmLinkage SDK Interface is fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pRspXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		IVS_SDK_ReleaseBuffer(pRspXml);
		return iRet;
	}
	(void)IVS_SDK_ReleaseBuffer(pRspXml);

	return iRet;

}

// 修改告警联动策略
IVS_INT32 COCXAlarmLinkageMgr::ModifyAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_ModifyAlarmLinkage(iSessionId, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ModifyAlarmLinkage SDK Interface is fail", "NA");
		return iRet;
	}
	return iRet;
}

// 删除告警联动策略
IVS_INT32 COCXAlarmLinkageMgr::DeleteAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml)
{
	IVS_DEBUG_TRACE("");
	
	IVS_INT32 iRet = IVS_SDK_DeleteAlarmLinkage(iSessionId, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DeleteAlarmLinkage SDK Interface is fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询告警联动策略
IVS_VOID COCXAlarmLinkageMgr::GetAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult)
{
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetAlarmLinkage(iSessionId, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLinkage SDK Interface fail", "NA");
		if (NULL != pRspXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pRspXml);
		}		
		COCXXmlProcess::GetErrString(strResult, iRet);
		return; 
	}
	iRet = COCXAlarmMgrXMLProcess::GeneralGetString(strResult, pRspXml, iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Restltxml to CU  fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		(void)IVS_SDK_ReleaseBuffer(pRspXml);
		return; 
	}
	(void)IVS_SDK_ReleaseBuffer(pRspXml);
	return;
}

// 查询告警联动列表
IVS_VOID COCXAlarmLinkageMgr::GetAlarmLinkageList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult)
{
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetAlarmlinkageList(iSessionId, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmlinkageList SDK Interface fail", "NA");
		if (NULL != pRspXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pRspXml);
		}		
		COCXXmlProcess::GetErrString(strResult, iRet);
		return; 
	}
	iRet = COCXAlarmMgrXMLProcess::GeneralGetString(strResult, pRspXml, iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get RestltXml to CU  fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		(void)IVS_SDK_ReleaseBuffer(pRspXml);
		return; 
	}
	(void)IVS_SDK_ReleaseBuffer(pRspXml);

	return;

}

// 查询告警联动动作详情
IVS_INT32 COCXAlarmLinkageMgr::GetAlarmLinkageAction(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRsqXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetAlarmLinkageAction(iSessionID, pReqXml, &pRsqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get AlarmLinkage Action Failed.", "Reason:SDK AlarmLinkageMgr Get AlarmLinkage ActionFailed.");
		if (NULL != pRsqXml)
		{
            IVS_SDK_ReleaseBuffer(pRsqXml);
		}
		return iRet;
	}
	// 在content节点下面拼装一个resultcode节点
	iRet = COCXAlarmMgrXMLProcess::GeneralGetString(strResult, pRsqXml, iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "add result code failed", "xml error");
		IVS_SDK_ReleaseBuffer(pRsqXml);
		return iRet;
	}

	(void)IVS_SDK_ReleaseBuffer(pRsqXml);

	return iRet;
}
