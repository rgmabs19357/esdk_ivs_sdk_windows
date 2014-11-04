/********************************************************************
filename	: 	AlarmMgr.h
author		:	pkf57481
created		:	2012/12/7
description	:	告警管理;
copyright	:	Copyright (C) 2011-2015
history		:	2012/12/7 初始版本;
*********************************************************************/

#ifndef __ALARMLINKAGE_MGR_H__
#define __ALARMLINKAGE_MGR_H__

#include "hwsdk.h"

class CUserMgr;
class CAlarmLinkageMgr
{
public:
	CAlarmLinkageMgr();
	~CAlarmLinkageMgr();

	void SetUserMgr(CUserMgr *pUserMgr);
public:
	// 新增告警联动策略
	IVS_INT32 AddAlarmLinkage(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml);

	// 修改告警联动策略
	IVS_INT32 ModifyAlarmLinkage(const IVS_CHAR* pReqXml);

	// 删除告警联动策略
	IVS_INT32 DeleteAlarmLinkage(const IVS_CHAR* pReqXml);

	// 查看告警联动策略列表
	IVS_INT32 GetAlarmlinkageList(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	// 查询告警联动策略
	IVS_INT32 GetAlarmLinkage(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml);

	// 查询告警联动动作详情
	IVS_INT32 GetAlarmLinkageAction(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);
private:
	CUserMgr *m_pUserMgr;
};

#endif //__ALARMLINKAGE_MGR_H__
