/********************************************************************
	filename	: 	RecordService.h
	author		:	z00201790
	created		:	2012/11/12	
	description	:	定义录像业务服务管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/05 初始版本;
*********************************************************************/

#ifndef __RECORD_SERVICE_H__
#define __RECORD_SERVICE_H__

#include "IVSCommon.h"
#include "vos.h"
#include "Record.h"
#include "SDKDef.h"

class CRecordService
{
public:
	CRecordService(void);
	~CRecordService(void);

public:
	void SetUserMgr(CUserMgr *pUserMgr);

	/******************************************************************
	function : DealException
	description: 处理MP的流异常回调;
	input : iPort,端口；iMsgType，回调消息类型；pHandle，句柄；
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	void DealException(IVS_INT32 iPort, IVS_INT32 iMsgType,const void* pHandle);

	/******************************************************************
	function : Release
	description: 释放录像线程锁;
	input : 无
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	void Release();
	/******************************************************************
	function : GetRecordInst
	description: 
	input : 
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int GetRecordInst(int iPlayHandle, bool bStart, unsigned int& uiIndex);
	/******************************************************************
	function : CancelRecordInst
	description: 
	input : 
	output : NA
	return : 无
	*******************************************************************/
	void CancelRecordInst(unsigned int uiIndex);
	/******************************************************************
	function : GetRecordInstByMpPort
	description: 
	input : 
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	 int GetRecordInstByMpPort(unsigned long ulMpPort, unsigned int& uiIndex);

	/******************************************************************
	function : StartPlatformRecord
	description: 开始平台录像;
	input : pCameraCode,摄像机编号；iRecordTime，录像时间
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StartPlatformRecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime);

	/******************************************************************
	function : StopPlatformRecord
	description: 停止平台录像;
	input : pCameraCode,摄像机编号
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StopPlatformRecord(const IVS_CHAR* pCameraCode);

	/******************************************************************
	function : StartPURecord
	description: 开始前端录像;
	input : pCameraCode,摄像机编号；iRecordTime，录像时间
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StartPURecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime);

	/******************************************************************
	function : StopPURecord
	description: 停止前端录像;
	input : pCameraCode,摄像机编号
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StopPURecord(const IVS_CHAR* pCameraCode);

	/******************************************************************
	function : GetRecordPolicy
	description: 获取录像策略;
	input : pCameraCode,摄像机编号；pRecordPolicy，录像策略信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 GetRecordPolicy(const IVS_CHAR* pCameraCode,IVS_RECORD_POLICY_TIME* pRecordPolicy)const;

	/******************************************************************
	function : SetRecordPolicy
	description: 设置录像策略;
	input : pCameraCode,摄像机编号；pRecordPolicy，录像策略信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 SetRecordPolicy(const IVS_CHAR* pCameraCode,const IVS_RECORD_POLICY_TIME* pRecordPolicy)const;

	/******************************************************************
	function : GetExDomainRecordPolicy
	description: 查询外域摄像机容灾录像策略;
	input : pReqXml,请求XML
	output : pRspXml 响应XML
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 GetExDomainRecordPolicy(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const;

	/******************************************************************
	function : SetExDomainRecordPolicy
	description: 设置外域摄像机容灾录像策略;
	input : pReqXml,请求XML
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 SetExDomainRecordPolicy(const IVS_CHAR* pReqXml)const;

    /******************************************************************
	function : AddExDomainRecordPlan
	description: 配置外域摄像机容灾录像计划;
	input : pReqXml,请求XML
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 AddExDomainRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;
    /******************************************************************
	function : ModExDomainRecordPlan
	description: 修改外域摄像机容灾录像计划;
	input : pReqXml,请求XML
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 ModExDomainRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;

    /******************************************************************
	function : DelExDomainRecordPlan
	description: 删除外域摄像机容灾录像计划;
	input : pReqXml,请求XML
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/

    IVS_INT32 DelExDomainRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        IVS_UINT32 uiRecordMethod,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;

    
	/******************************************************************
	function : GetExDomainRecordPlan
	description: 查询外域摄像机容灾录像计划;
	input : pCameraCode,摄像机编号；uiRecordMethod，录像计划方式；pRecordPlan，录像计划查询结果；
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 GetExDomainRecordPlan(const char* pCameraCode,IVS_UINT32 uiRecordMethod,
		IVS_RECORD_PLAN* pRecordPlan);

	/***********************************************************************************
    * name       : GetPrerecordCameraList
    * description: 查询开启预录的摄像机列表
	input      : IVS_INT32 iSessionID 登录成功后的会话ID;
	output     : IVS_CHAR * * pRspXml  查询录像备份任务列表返回信息XML
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetPrerecordCameraList(IVS_CHAR** pRspXml);

private:
	CRecord m_RecordInst[MAX_RECORD_COUNT];

	CUserMgr *m_pUserMgr;
    VOS_Mutex* m_pInstMutex; //锁
};

#endif
