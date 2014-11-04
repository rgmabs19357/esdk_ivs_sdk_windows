/********************************************************************
filename	: 	AlarmMgr.h
author		:	pkf57481
created		:	2012/11/1
description	:	告警管理;
copyright	:	Copyright (C) 2011-2015
history		:	2012/11/1 初始版本;
*********************************************************************/

#ifndef __ALARM_MGR_H__
#define __ALARM_MGR_H__

#include "hwsdk.h"

class CUserMgr;
class CAlarmMgr
{
public:
	CAlarmMgr(void);
	~CAlarmMgr(void);

public:
    void SetUserMgr(CUserMgr *pUserMgr);

	// 查看告警订阅列表
	IVS_INT32 SubscribeQuery(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml) const;

	// 新增，告警订阅
	IVS_INT32 SubscribeAlarm(const IVS_CHAR* pReqXml) const;

	// 查询自定义告警类型列表
    IVS_INT32 GetAlarmTypeList(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
        IVS_ALARM_TYPE_LIST* pAlarmTypeList, IVS_UINT32 uiBufferSize);

	// 设置告警类型列表
    IVS_INT32 SetAlarmTypeLevel(const IVS_ALARM_TYPE_LIST* pAlarmTypeList);
	
	// 告警确认
	IVS_INT32 ConfirmAlarm(IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo);

	//水印告警确认
	IVS_INT32 ConfirmWaterAlarm(const IVS_WATERMARK_ALARM_CONFIRM* pConfirmInfo);

	// 告警撤销
	IVS_INT32 CancelAlarm(IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo);

	//水印告警撤销
	IVS_INT32 CancelWaterAlarm(const IVS_WATERMARK_ALARM_CANCEL* pstConfirmInfo);

	// 告警授权
	IVS_INT32 AlarmCommission(const IVS_CHAR* pAlarmInCode, const IVS_ALARM_COMMISSION* pAlarmCommission);

	// 授权告警用户认证
	IVS_INT32 UserAlarmAuthorization(const IVS_CHAR *pPWD) const;

	// 告警上报（CU->SMU）
	IVS_INT32 AlarmReport(const IVS_ALARM_REPORT* pAlarmReport);

	//第三方告警上报
	IVS_INT32 ManualAlarm(const IVS_CHAR* /*pReq*/, IVS_CHAR* &/*pRsp*/);

	//启动开关量输出告警
	IVS_INT32 StartAlarmOut(const IVS_CHAR* pAlarmOutCode);

	// 停止开关量输出告警
	IVS_INT32 StopAlarmOut(const IVS_CHAR* pAlarmOutCode);

	// 查询告警信息
	IVS_INT32 GetAlarmInfo(IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, IVS_ALARM_EVENT* pAlarmEvent);

	// 查询历史告警信息列表
	IVS_INT32 GetAlarmList(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ALARM_EVENT_LIST* pAlarmEventList, IVS_UINT32 uiBufferSize);

	// 查询水印告警信息列表
	IVS_INT32 QueryWatermarkAlarmList(const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat, IVS_VOID* pBuffer,IVS_UINT32 uiBufferSize);

	// 获取拥有指定设备权限的用户列表
	IVS_INT32 GetDeviceUserList(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, IVS_UINT32 uiIsUserOnline, const IVS_INDEX_RANGE* pIndexRange, IVS_DEVICE_USER_INFO_LIST* pDevUserList, IVS_UINT32 uiBufferSize);

	// 获取平台配置信息
	IVS_INT32 GetConfigInfo(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	// 查询平台能力集信息
	IVS_INT32 GetAbilityInfo (IVS_UINT32* uiAbility,IVS_CHAR** pRspXml);

	// 查询OMU告警事件列表
	IVS_INT32 GetDeviceAlarmList(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_DEVICE_ALARM_EVENT_LIST* pAlarmEventList, IVS_UINT32 uiBufferSize);

	// 设备布撤防
	IVS_INT32 SetDeviceGuard(const IVS_CHAR* pReqXml);

	// 获取告警抓拍图片列表
	IVS_INT32 GetAlarmSnapshotList(const IVS_CHAR* pReqXml, IVS_CHAR** pRsqXml);

	// 报警复位
	IVS_INT32 ResetAlarm(const IVS_CHAR* pReqXml);

	// 外部域设备事件订阅
	IVS_INT32 ExEventSubscribe(const IVS_CHAR* pReqXml);

	// 查询外部域设备事件订阅
	IVS_INT32 GetExEventSubscribe(const IVS_CHAR* pReqXml, IVS_CHAR** pRsqXml);

	// 外部域设备目录订阅
	IVS_INT32 ExCatalogSubscribe(const IVS_CHAR* pReqXml);

	// 获取外部域设备目录订阅
	IVS_INT32 GetExCatalogSubscribeList(const IVS_CHAR* pReqXml, IVS_CHAR** pRsqXml);
private:
	IVS_INT32 GetDevCodeListInfo(const IVS_CHAR *pDevBigCode,std::list<std::string>& lDevCodeList);
 private:
 	CUserMgr *m_pUserMgr;
};
#endif // __ALARM_MGR_H__
