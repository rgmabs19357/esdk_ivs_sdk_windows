/********************************************************************
	filename	: 	OCXRecordMgr.h
	author		:	s00193168
	created		:	2012/12/05	
	description	:	定义录像管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/05 初始版本;
*********************************************************************/
#ifndef __OCX_RECORD_MGR_H__ 
#define __OCX_RECORD_MGR_H__

#include "hwsdk.h"
#include "stdafx.h"
class OCXRecordMgr
{
public:
	OCXRecordMgr(void);
	~OCXRecordMgr(void);

	/**********************************************************************
	* name			: AddRecordPlan
	* description	: 增加录像计划
	* input			: strResult 录像计划xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString AddRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: ModifyRecordPlan
	* description	: 修改录像计划
	* input			: strResult 录像计划xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString ModifyRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: DeleteRecordPlan
	* description	: 删除录像计划
	* input			: strResult 录像计划xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString DeleteRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: GetRecordPlan
	* description	: 查询录像计划
	* input			: strResult 录像计划xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString GetRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: GetRecordStatus
	* description	: 查询录像状态
	* input			: strResult 录像状态xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString GetRecordStatus(const CString& pCameraCode,const ULONG ulRecordMethod,const IVS_INT32 m_iSDKSessionID);

	/*****************************************************************
	     function   : SetCameraBackupConfig
	     description: 设置摄像机备份参数        
	     output     : IVS_INT32 iSessionID          登录Id
	     output     : const IVS_CHAR * pBackupXml   参数xml
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 SetCameraBackupConfig(IVS_INT32 iSessionID, const IVS_CHAR* pBackupXml);

	/*****************************************************************
	     function   : GetCameraBackupCfg
	     description: 获取摄像机备份参数
	     input      : IVS_INT32 iSessionId			 登录Id
				   	  const IVS_CHAR * pCameraCode   摄像机编码
	     output     : CString & strResult            SMU的响应
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 GetCameraBackupCfg(IVS_INT32 iSessionId, const IVS_CHAR* pCameraCode, CString &strResult);

	/*****************************************************************
	     function   : AddRecordBackupPlan
	     description: 添加录像备份计划
	     input      : IVS_INT32 iSessionID   登录成功后的会话ID
	                  const IVS_CHAR * pBackupPlan   添加录像备份计划XML
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 AddRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan);

	/*****************************************************************
	     function   : AddRecordBackupPlan
	     description: 修改录像备份计划
	     input      : IVS_INT32 iSessionID   登录成功后的会话ID
	                  const IVS_CHAR * pBackupPlan   修改录像备份计划XML
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 ModifyRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan);

	/*****************************************************************
	     function   : AddRecordBackupPlan
	     description: 删除录像备份计划
	     input      : IVS_INT32 iSessionID   登录成功后的会话ID
	                  const IVS_CHAR * pBackupPlan   删除录像备份计划XML
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 DeleteRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan);

	
	/*****************************************************************
	     function   : GetRecordBackupPlan
	     description: 查询录像备份计划
	     output     : IVS_INT32 iSessionID          登录成功后的会话ID
	     output     : const IVS_CHAR * pQueryInfo   查询录像备份计划XML
	     output     : CString & strResult           录像备份计划XML
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 GetRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pQueryInfo, CString &strResult);

	/*****************************************************************
	     function   : GetRecordTask
	     description: 查询手动录像任务
	     input     : IVS_INT32 iSessionID      登录成功后的会话ID
	     output     : CString & strResult      查询手动录像任务XML
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 GetRecordTask(IVS_INT32 iSessionId, CString &strResult);

	/*****************************************************************
	     function   : GetRecordBackupTaskList
	     description: 查询录像备份任务列表
	     output     : IVS_INT32 iSessionID          登录成功后的会话ID
	     output     : const IVS_CHAR * pQueryInfo   查询录像备份任务XML
	     output     : CString & strResult           录像备份任务XML
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 GetRecordBackupTaskList(IVS_INT32 iSessionID, const IVS_CHAR* pQueryInfo, CString &strResult);

	/*****************************************************************
	     function   : AddRecordBackupTask
	     description: 新增录像备份任务列表
	     output     : IVS_INT32 iSessionID   登录成功后的会话ID
	     output     : const IVS_CHAR * pBackupTask  新增录像备份任务XML
	     return     : 
	*****************************************************************/
	static IVS_INT32 AddRecordBackupTask(IVS_INT32 iSessionID, const IVS_CHAR* pBackupTask);

    /******************************************************************
    function : GetCameraPlanInfo
    description: 查询摄像机计划设置信息
    input : IVS_INT32 iSessionID            登录成功后的会话ID   
    input : const IVS_CHAR * pCameraList    摄像机编码列表
    output: CString & strResult             结果XML
    return : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetCameraPlanInfo(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString &strResult);

		/**********************************************************************
	* name			: AddExDomainRecordPlan
	* description	: 配置外域摄像机容灾录像计划
	* input			: pRecordPlanReq 录像计划请求xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString AddExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: ModifyExDomainRecordPlan
	* description	: 修改外域摄像机容灾录像计划
	* input			: pRecordPlanReq 录像计划请求xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString ModifyExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: DeleteExDomainRecordPlan
	* description	: 删除外域摄像机容灾录像计划
	* input			: pRecordPlanReq 录像计划请求xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString DeleteExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: GetExDomainRecordPlan
	* description	: 查询外域摄像机容灾录像计划
	* input			: pRecordPlanReq 录像计划请求xml
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	static CString GetExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID);
};

#endif

