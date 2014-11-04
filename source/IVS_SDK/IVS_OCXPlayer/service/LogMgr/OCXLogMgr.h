/********************************************************************
	filename	: 	OCXLogMgr.h
	author		:	wWX155351
	created		:	2012/12/10	
	description	:	处理告警OCX
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/10 初始版本
*********************************************************************/

#ifndef __OCX_LOG_MGR_H__
#define __OCX_LOG_MGR_H__

#include "hwsdk.h"

class COCXLogMgr
{
public:
	COCXLogMgr();
	~COCXLogMgr();

	// 查询操作日志
	static IVS_INT32 GetOperationLog(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult);

};

#endif // __OCX_LOG_MGR_H__
