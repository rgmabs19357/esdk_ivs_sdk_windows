/********************************************************************
filename	: 	CIntelligenceAnalysisMgr.h
author		:	z00227377
created		:	2012/12/5
description	:	智能分析规则管理;
copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history		:	2012/12/5 初始版本;
*********************************************************************/

#ifndef __INTELLIGENCE_ANALYSIS_MGR_H
#define __INTELLIGENCE_ANALYSIS_MGR_H

#include "hwsdk.h"
#include "nss_mt.h"

class CIntelligenceAnalysisMgr
{
public:
    CIntelligenceAnalysisMgr(void);
    ~CIntelligenceAnalysisMgr(void);
    
    // 设置用户对象指针
    void SetUserMgr(CUserMgr *pUserMgr);

	// 智能分析统一透传接口
	IVS_INT32 IntelligenceAnalysis(IVS_UINT32 uiInterfaceType, IVS_CHAR* pTransID, IVS_UINT32 uiTransIDLen, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

private:
    CUserMgr   *m_pUserMgr;       // 用户对象指针
};

#endif


