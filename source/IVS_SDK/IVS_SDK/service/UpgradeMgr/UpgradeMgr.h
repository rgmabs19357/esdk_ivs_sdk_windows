/********************************************************************
	filename	: 	UpgradeMgr.h
	author		:	w00165153
	created		:	2013/11/13
	description	:	升级管理类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2013/11/13 初始版本;
*********************************************************************/
#ifndef __UPGRADEMGR_H__
#define __UPGRADEMGR_H__
#include "hwsdk.h"
#include "ivs_error.h"


class CUserMgr;
class CUpgradeMgr
{
public:
    CUpgradeMgr(void);
    ~CUpgradeMgr(void);
public:
    // 设置用户对象类
    void SetUserMgr(CUserMgr* pUserMgr);
    IVS_INT32 GetCUVersion(const IVS_CU_INFO* pCUInfo, IVS_UPGRADE_INFO* pUpgradeInfo);
private:
    CUserMgr *m_pUserMgr; //lint !e830 //不需要//
};

#endif