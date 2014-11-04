/********************************************************************
filename : ReportMgr.h
author : pkf57481
created : 2013/03/06
description : 报表管理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/03/06 初始版本
*********************************************************************/


#ifndef __REPORT_MGR_H_
#define __REPORT_MGR_H_

#include "hwsdk.h"
#include "IVSCommon.h"

class CUserMgr;
class CReportMgr
{
public:
    CReportMgr(void);
    ~CReportMgr(void);

    void SetUserMgr(CUserMgr *pUserMgr);

public:
    // 实时报表查询
    IVS_INT32 OMUQueryDevReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 实时报表详情查询
    IVS_INT32 OMUQueryDevReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 实时报表查询
    IVS_INT32 OMUQueryRealtimeReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 实时报表详情查询
    IVS_INT32 OMUQueryRealtimeReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 视频质量诊断查询
    IVS_INT32 OMUQueryQDReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 视频质量诊断详情查询
    IVS_INT32 OMUQueryQDReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 录像执行计划报表查询
    IVS_INT32 OMUQueryRecordPlanReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 录像执行计划报表详情查询
    IVS_INT32 OMUQueryRecordPlanReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 录像完整性报表查询
    IVS_INT32 OMUQueryRecordIntegralityReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 录像完整性报表详情查询
    IVS_INT32 OMUQueryRecordIntegralityReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 丢包率报表查询
    IVS_INT32 OMUQueryPacketLossRateReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 丢包率报表详情查询
    IVS_INT32 OMUQueryPacketLossRateReportDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    // 磁盘存储占用率报表查询
    IVS_INT32 OMUQueryDiskUsageRateReport(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    IVS_INT32 SendQueryCmd(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml, IVS_INT32 iMsgType, IVS_INT32 iTimeOut = -1);

	// 查询磁盘信息接口
	IVS_INT32 OMUQueryDiskProperty(IVS_CHAR** pRspXml);

private:
    CUserMgr *m_UserMgr;
};

#endif
