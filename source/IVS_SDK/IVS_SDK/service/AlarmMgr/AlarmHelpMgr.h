/********************************************************************
filename	: 	AlarmHelpMgr.h
author		:	pkf57481
created		:	2012/11/7
description	:	告警帮助管理;
copyright	:	Copyright (C) 2011-2015
history		:	2012/11/7 初始版本;
*********************************************************************/

#ifndef __ALARMHELP_MGR_H
#define __ALARMHELP_MGR_H
#include "hwsdk.h"

class CUserMgr;
class CAlarmHelpMgr
{
public:
	CAlarmHelpMgr(void);
	~CAlarmHelpMgr(void);
	void SetUserMgr(CUserMgr *pUserMgr);
public:
	// 新增告警帮助
	IVS_INT32 AddHelpInfo(const IVS_CHAR* pDomainCode, const IVS_HELP_INFO *helpInfo) const;

	// 修改告警帮助
	IVS_INT32 ModifyHelpInfo(const IVS_CHAR* pDomainCode, const IVS_HELP_INFO *helpInfo) const;

	// 删除告警帮助
	IVS_INT32 DeleteHelpInfo(IVS_UINT32 uiSourceId,IVS_UINT32 uiSourceType, const IVS_CHAR* pDomainCode) const;

	// 查询告警帮助详情
	IVS_INT32 QueryHelpInfo(IVS_UINT32 uiAlarmHelpSourceType,IVS_UINT32 uiAlarmHelpSourceId,const IVS_CHAR* pDomainCode, IVS_HELP_INFO *helpInfo) const;

    /******************************************************************
     function   : QueryHelpInfoText
     description: 查询帮助信息正文
     input      : const IVS_CHAR * pAlarmInCode
     input      : const IVS_CHAR * pAlarmType
     input      : IVS_CHAR * pHelpInfo
     input      : IVS_UINT32 uiBufferSize
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 QueryHelpInfoText(const IVS_CHAR* pAlarmInCode, const IVS_CHAR* pAlarmType, IVS_CHAR* pHelpInfo, IVS_UINT32 uiBufferSize);

private:
	CAlarmHelpMgr* m_AlarmHelpMgr;
	CUserMgr* m_UserMgr;
};	

#endif
