#include "stdafx.h"
#include "SDKDef.h"
#include "hwsdk.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "OCXClientNetworkMgr.h"
#include "OCXClientNetworkMgrXMLProcess.h"
#include "IVS_Trace.h"

COCXClientNetworkMgr::COCXClientNetworkMgr(void)
{
}

COCXClientNetworkMgr::~COCXClientNetworkMgr(void)
{
}

// CPU/内存性能查询ocx
IVS_INT32 COCXClientNetworkMgr::GetCPUMemory(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, CString& strResult)
{
	CHECK_POINTER(pNodeCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	
	IVS_CHAR* pPerformance = NULL;
	IVS_INT32 iRet = IVS_SDK_GetCPUMemory(iSessionID, pNodeCode, &pPerformance);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetCPUMemory SDK Interface fail", "NA");
		if (NULL != pPerformance)
		{
			IVS_SDK_ReleaseBuffer(pPerformance);
		}
		return iRet;
	}
	std::string strInXml = pPerformance;
    iRet = COCXXmlProcess::GetResultXML(iRet, strInXml.c_str(), strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        IVS_SDK_ReleaseBuffer(pPerformance);
        return iRet;
    }

	IVS_SDK_ReleaseBuffer(pPerformance);
	return iRet;
}

// 智能分析CPU/内存性能查询ocx
IVS_INT32 COCXClientNetworkMgr::GetIACPUMemory(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, CString& strResult)
{
	CHECK_POINTER(pNodeCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pPerformance = NULL;
	IVS_INT32 iRet = IVS_SDK_GetIACPUMemory(iSessionID, pNodeCode, &pPerformance);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetIACPUMemory SDK Interface fail", "NA");
		if (NULL != pPerformance)
		{
			IVS_SDK_ReleaseBuffer(pPerformance);
		}
		return iRet;
	}
	strResult = pPerformance;
	IVS_SDK_ReleaseBuffer(pPerformance);
	return iRet;
}

// 网元性能查询
IVS_INT32 COCXClientNetworkMgr::QueryPerformance(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, CString& strResult)
{
	CHECK_POINTER(pNodeCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pPerformance = NULL;
	IVS_INT32 iRet = IVS_SDK_QueryPerformance(iSessionID, pNodeCode, &pPerformance);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "QueryPerformance SDK Interface fail", "NA");
		if (NULL != pPerformance)
		{
			IVS_SDK_ReleaseBuffer(pPerformance);
		}
		return iRet;
	}
	std::string strInXml = pPerformance;
    iRet = COCXXmlProcess::GetResultXML(iRet, strInXml.c_str(), strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        IVS_SDK_ReleaseBuffer(pPerformance);
        return iRet;
    }

	IVS_SDK_ReleaseBuffer(pPerformance);
	return iRet;
}

// 新增告警通知规则接口
IVS_INT32 COCXClientNetworkMgr::AddAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_OMUAddAlarmLinkageRule(iSessionID, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddAlarmLinkageRule SDK Interface fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	std::string strInXml = pRspXml;
    iRet = COCXXmlProcess::GetResultXML(iRet, strInXml.c_str(), strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        IVS_SDK_ReleaseBuffer(pRspXml);
        return iRet;
    }

	IVS_SDK_ReleaseBuffer(pRspXml);
	return iRet;
}

// 修改告警通知规则接口
IVS_INT32 COCXClientNetworkMgr::ModAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	return IVS_SDK_OMUModAlarmLinkageRule(iSessionID, pReqXml);
}

// 修改告警通知规则接口
IVS_INT32 COCXClientNetworkMgr::DelAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	return IVS_SDK_OMUDelAlarmLinkageRule(iSessionID, pReqXml);
}

// 获取告警通知规则列表接口
IVS_INT32 COCXClientNetworkMgr::GetAlarmLinkageRuleList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_OMUGetAlarmLinkageRuleList(iSessionID, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLinkageRuleList SDK Interface fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	std::string strInXml = pRspXml;
    iRet = COCXXmlProcess::GetResultXML(iRet, strInXml.c_str(), strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        IVS_SDK_ReleaseBuffer(pRspXml);
        return iRet;
    }

	IVS_SDK_ReleaseBuffer(pRspXml);
	return iRet;
}

// 获取告警通知规则详情接口
IVS_INT32 COCXClientNetworkMgr::GetAlarmLinkageRuleDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_OMUGetAlarmLinkageRuleDetail(iSessionID, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLinkageRuleDetail SDK Interface fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	std::string strInXml = pRspXml;
    iRet = COCXXmlProcess::GetResultXML(iRet, strInXml.c_str(), strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        IVS_SDK_ReleaseBuffer(pRspXml);
        return iRet;
    }

	IVS_SDK_ReleaseBuffer(pRspXml);
	return iRet;
}

// 根据网元获取设备类型接口
IVS_INT32 COCXClientNetworkMgr::GetDevTypeByNodeCode(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_OMUGetDevTypeByNodeCode(iSessionID, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDevTypeByNodeCode SDK Interface fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	std::string strInXml = pRspXml;
    iRet = COCXXmlProcess::GetResultXML(iRet, strInXml.c_str(), strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        IVS_SDK_ReleaseBuffer(pRspXml);
        return iRet;
    }

	IVS_SDK_ReleaseBuffer(pRspXml);
	return iRet;
}

// 获取告警通知规则详情接口
IVS_INT32 COCXClientNetworkMgr::CheckNewVersion(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_CheckNewVersion(iSessionID, pReqXml, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "CheckNewVersion SDK Interface fail", "NA");
		if (NULL != pRspXml)
		{
			IVS_SDK_ReleaseBuffer(pRspXml);
		}
		return iRet;
	}
	std::string strInXml = pRspXml;
    iRet = COCXXmlProcess::GetResultXML(iRet, strInXml.c_str(), strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add ResultCode Under Content fail", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        IVS_SDK_ReleaseBuffer(pRspXml);
        return iRet;
    }

	IVS_SDK_ReleaseBuffer(pRspXml);
	return iRet;
}

