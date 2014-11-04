/********************************************************************
filename	: 	AlarmLevelMgr.h
author		:	pkf57481
created		:	2012/11/7
description	:	告警级别管理;
copyright	:	Copyright (C) 2011-2015
history		:	2012/11/7 初始版本;
*********************************************************************/

#ifndef __ALARMLEVEL_MGR_H
#define __ALARMLEVEL_MGR_H

#include "hwsdk.h"
class CUserMgr;
class CAlarmLevelMgr
{
public:
	CAlarmLevelMgr(void);
	~CAlarmLevelMgr(void);
	void SetUserMgr(CUserMgr *pUserMgr);
public:
	//新增告警级别
	IVS_INT32 AddAlarmLevel(const IVS_ALARM_LEVEL *pAlarmLevel,IVS_UINT32* pAlarmLevelID) const;

	//修改告警级别
	IVS_INT32 ModifyAlarmLevel(const IVS_ALARM_LEVEL *pAlarmLevel) const;

	//删除告警级别
	IVS_INT32 DeleteAlarmLevel(const IVS_UINT32 uiAlarmLevelId) const;

	//查询告警级别列表
	IVS_INT32 GetAlarmLevelList(IVS_UINT32 uiMaxAlarmLevelNum, IVS_UINT32* puiAlarmLevelNum, IVS_ALARM_LEVEL* pAlarmLevel, IVS_UINT32 /*uiBufferSize*/) const;

	//查询告警级别详情
	IVS_INT32 GetAlarmLevel(IVS_UINT32 uiAlarmLevelID,IVS_ALARM_LEVEL* pAlarmLevel) const;


private:
	CAlarmLevelMgr* m_AlarmLevelMgr;
	CUserMgr* m_UserMgr;
};
#endif
