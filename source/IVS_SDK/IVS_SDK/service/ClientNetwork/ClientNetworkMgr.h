/********************************************************************
filename : ClientNetworkMgr.h
author : pkf57481
created : 2013/01/22
description : ?????
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/22 ????
*********************************************************************/


#ifndef __CLIENTNETWORK_MGR_H_
#define __CLIENTNETWORK_MGR_H_

#include "hwsdk.h"
#include "IVSCommon.h"
#include "nss_mt.h"
class CUserMgr;
class CClientNetworkMgr
{
public:
    CClientNetworkMgr(void);
    ~CClientNetworkMgr(void);

    void SetUserMgr(CUserMgr *pUserMgr);

    // 查询网元树
	IVS_INT32 GetDomainList(IVS_CHAR** pDomainList)const;

	// 网元CPU/内存性能查询
	IVS_INT32 GetCPUMemory(const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance)const;

	// 智能分析CPU/内存性能
	IVS_INT32 GetIACPUMemory(const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance)const;

	//	网元性能查询
	IVS_INT32 QueryPerformance(const IVS_CHAR* pNodeCode,	IVS_CHAR** pPerformance)const;

	// 新增告警通知规则接口
	IVS_INT32 AddAlarmLinkageRule(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

	// 修改告警通知规则接口
	IVS_INT32 ModAlarmLinkageRule(const IVS_CHAR* pReqXml)const;

	// 删除告警通知规则接口
	IVS_INT32 DelAlarmLinkageRule(const IVS_CHAR* pReqXml)const;

	// 获取告警通知规则列表接口
	IVS_INT32 GetAlarmLinkageRuleList(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

	// 获取告警通知规则详情接口
	IVS_INT32 GetAlarmLinkageRuleDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

	// 根据网元获取设备类型接口
	IVS_INT32 GetDevTypeByNodeCode(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

	// 客户端升级检查
	IVS_INT32 CheckNewVersion(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

	//磁盘组信息查询
	IVS_INT32 QueryRaidGroup(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

	//磁盘信息查询
	IVS_INT32 QueryRaidGroupDisk(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

private:

	IVS_INT32 ComSendToOMU(NSS_MSG_TYPE_E msgType, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const;

	IVS_INT32 ComSendToOMU(NSS_MSG_TYPE_E msgType, const IVS_CHAR* pReqXml)const;
private:
    CUserMgr *m_UserMgr;
};

#endif
