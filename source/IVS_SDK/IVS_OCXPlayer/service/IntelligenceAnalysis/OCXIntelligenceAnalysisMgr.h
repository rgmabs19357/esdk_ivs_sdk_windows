/********************************************************************
filename 	: COcxIntelligenceAnalysisMgr.h
author   	: z00227377
created 	: 2012/12/06	
description : OCX智能分析管理
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/06 初始版本
*********************************************************************/

#ifndef __OCX_INTELLIGENCE_ANALYSIS_MGR_H__
#define __OCX_INTELLIGENCE_ANALYSIS_MGR_H__

#include "hwsdk.h"
#include "stdafx.h"

class COcxIntelligenceAnalysisMgr
{
public:
    COcxIntelligenceAnalysisMgr(void);
    ~COcxIntelligenceAnalysisMgr(void);

public:

	// 智能分析统一透传接口
	static void IntelligenceAnalysis(CString &strResult, IVS_INT32 iSessionID, ULONG ulInterfaceType, const IVS_CHAR* pReqXml);

private:

	// 构建返回消息
    static void RetToCUMsg(CString &strResult, IVS_INT32 iRet, IVS_CHAR* pRspXml);

};


#endif
