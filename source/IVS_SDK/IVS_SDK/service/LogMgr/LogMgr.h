/*****************************************************************
 filename    :    LogMgr.h
 author      :    wWX155351
 created     :    2012/11/14
 description :    查询操作日志;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/14 初始版本
*****************************************************************/

//#pragma once

#ifndef _OPERATIONLOG_MNG_H_
#define _OPERATIONLOG_MNG_H_


#include "hwsdk.h"
#include "ivs_error.h"

/*
#include "NSSOperator.h"
#include "nss_mt.h"*/
class CUserMgr;
class CLogMgr
{
public:
	CLogMgr(void);
	~CLogMgr(void);

	// 设置用户对象类
	void SetUserMgr(void *pUserMgr);

private:
	CUserMgr *m_pUserMgr;

public:
	// 查询操作日志
	IVS_INT32 GetOperationLog(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_OPERATION_LOG_LIST* pResOperLogList,const IVS_UINT32& uiBufSize) const;

	/*****************************************************************
		 function   : GetOperationLogEx
		 description: 扩展查询操作日志
		 * input      : pUnifiedQuery 统一通用查询条件
		 * input      : pOptLogQueryEx 扩展查询操作日志条件
		 * input      : uiBufSize 事件内容大小
		 * output   : pResOperLogList 操作日志查询查询结果
		 * return		: 成功返回IVS_SUCCESSED，失败返回错误码
		 return		: 成功返回0，失败返回错误码;
		 *****************************************************************/
	IVS_INT32 GetOperationLogEx(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
		const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
		IVS_OPERATION_LOG_LIST* pResOperLogList, 
		const IVS_UINT32 uiBufSize) const;

};
#endif 

