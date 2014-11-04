#include "ReportMgr.h"
#include "UserMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "IVS_Trace.h"

CReportMgr::CReportMgr(void)
    :m_UserMgr(NULL)
{
}

CReportMgr::~CReportMgr(void)
{
    m_UserMgr = NULL;
}

void CReportMgr::SetUserMgr(CUserMgr *pUserMgr)
{
    m_UserMgr = pUserMgr;
}

// 实时报表查询
IVS_INT32 CReportMgr::OMUQueryDevReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_DEV_REPORT_FORMS_QUERY_REQ);
}

// 实时报表详情查询
IVS_INT32 CReportMgr::OMUQueryDevReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_DEV_REPORT_DETAIL_FORMS_QUERY_REQ);
}

// 实时报表查询
IVS_INT32 CReportMgr::OMUQueryRealtimeReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_REALTIME_REPORT_FORMS_QUERY_REQ);
}

// 实时报表详情查询
IVS_INT32 CReportMgr::OMUQueryRealtimeReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_REALTIME_REPORT_DETAIL_FORMS_QUERY_REQ);
}

// 视频质量诊断查询
IVS_INT32 CReportMgr::OMUQueryQDReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_QD_FORMS_QUERY_REQ);
}

// 视频质量诊断详情查询
IVS_INT32 CReportMgr::OMUQueryQDReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_QD_DETAIL_FORMS_QUERY_REQ);
}

// 录像执行计划报表查询
IVS_INT32 CReportMgr::OMUQueryRecordPlanReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_RECORD_PLAN_FORMS_QUERY_REQ);
}

// 录像执行计划报表详情查询
IVS_INT32 CReportMgr::OMUQueryRecordPlanReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_RECORD_PLAN_DETAIL_FORMS_QUERY_REQ);
}

// 录像完整性报表查询
IVS_INT32 CReportMgr::OMUQueryRecordIntegralityReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_RECORD_INTEGRALITY_FORMS_QUERY_REQ);
}

// 录像完整性报表详情查询
IVS_INT32 CReportMgr::OMUQueryRecordIntegralityReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_RECORD_INTEGRALITY_DETAIL_FORMS_QUERY_REQ, 60);
}

// 丢包率报表查询
IVS_INT32 CReportMgr::OMUQueryPacketLossRateReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_LOSE_RATE_FORMS_QUERY_REQ);
}

// 丢包率报表详情查询
IVS_INT32 CReportMgr::OMUQueryPacketLossRateReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_LOSE_RATE_DETAIL_FORMS_QUERY_REQ);
}

// 磁盘存储占用率报表查询
IVS_INT32 CReportMgr::OMUQueryDiskUsageRateReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
    return SendQueryCmd(pReqXml, pRspXml, NSS_DISK_RATE_QUERY_REQ);
}

//查询磁盘信息接口
IVS_INT32 CReportMgr::OMUQueryDiskProperty(IVS_CHAR** pRspXml)
{
	return SendQueryCmd("<Content></Content>", pRspXml, NSS_OMU_DISK_PROPERTY_REQ);
}

IVS_INT32 CReportMgr::SendQueryCmd(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml, IVS_INT32 iMsgType,  IVS_INT32 iTimeOut)
{
	IVS_DEBUG_TRACE("");
    CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);

    IVS_INT32 iRet = IVS_FAIL;
	std::string strLocalDomainLink;

	iRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strLocalDomainLink);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Send Query Cmd", "Get Local Domain OMU Link Failed");
		return iRet;
	}
	CCmd *pCmd = CNSSOperator::instance().BuildOMUCmd(iMsgType, pReqXml, strLocalDomainLink);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

    CCmd* pRspCmd =NULL;
    if (0 < iTimeOut)
    {
        // 1表示不采用默认的超时时间，即用iTimeOut
        pRspCmd = CNSSOperator::instance().SendSyncCmd(pCmd, (IVS_UINT32)iTimeOut, 1);
    }
    else
    {
        pRspCmd = CNSSOperator::instance().SendSyncCmd(pCmd);
    }
	if (NULL == pRspCmd)
	{
		BP_RUN_LOG_ERR(IVS_NET_RECV_TIMEOUT, "Send Query Cmd", "pRspCmd is NULL");
		return IVS_NET_RECV_TIMEOUT;
	}

    iRet = IVS_SUCCEED;
    *pRspXml = CNSSOperator::instance().ParseCmd2XML(pRspCmd, iRet);
    HW_DELETE(pRspCmd);

    return iRet;
}//lint !e1762

