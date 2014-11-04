/********************************************************************
filename : OCXClientNetworkMgr.h
author : pkf57481
created : 2013/01/22
description : 客户端网管
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/22 初始版本
*********************************************************************/


#ifndef __OCX_CLIENTNETWORK_MGR_H_
#define __OCX_CLIENTNETWORK_MGR_H_

#include "hwsdk.h"
#include "IVSCommon.h"

class COCXClientNetworkMgr
{
public:
    COCXClientNetworkMgr(void);
    ~COCXClientNetworkMgr(void);

public: 
	/***********************************************************************************
    * name       : GetCPUMemory
    * description: CPU/内存性能查询ocx
    * input      : iSessionId:登录Id; pNodeCode:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetCPUMemory(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, CString& strResult);

	/***********************************************************************************
    * name       : GetIACPUMemory
    * description: 智能分析CPU/内存性能查询ocx
    * input      : iSessionId:登录Id; pNodeCode:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetIACPUMemory(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, CString& strResult);

	/***********************************************************************************
    * name       : QueryPerformance
    * description: 网元性能查询ocx
    * input      : iSessionId:登录Id; pNodeCode:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 QueryPerformance(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, CString& strResult);

	/*****************************************************************
	     function   : AddAlarmLinkageRule
	     description: 新增告警通知规则接口
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml  请求内容XML
	     output     : CString & strResult       结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 AddAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);

	/*****************************************************************
	     function   : ModAlarmLinkageRule
	     description: 修改告警通知规则接口
	     output     : IVS_INT32 iSessionID       登录成功后的会话ID
	     output     : const IVS_CHAR * pReqXml   请求内容XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 ModAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*****************************************************************
	     function   : DelAlarmLinkageRule
	     description: 删除告警通知规则接口
	     output     : IVS_INT32 iSessionID       登录成功后的会话ID
	     output     : const IVS_CHAR * pReqXml   请求内容XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 DelAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*****************************************************************
	     function   : GetAlarmLinkageRuleList
	     description: 获取告警通知规则列表接口
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml  请求内容XML
	     output     : CString & strResult       结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 GetAlarmLinkageRuleList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);

	/*****************************************************************
	     function   : GetAlarmLinkageRuleDetail
	     description: 获取告警通知规则详情接口
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml  请求内容XML
	     output     : CString & strResult       结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 GetAlarmLinkageRuleDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);

	/*****************************************************************
	     function   : GetDevTypeByNodeCode
	     description: 根据网元获取设备类型接口
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml  请求内容XML
	     output     : CString & strResult       结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 GetDevTypeByNodeCode(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);

	/*****************************************************************
	     function   : GetAlarmLinkageRuleDetail
	     description: 客户端升级检查
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml  请求内容XML
	     output     : CString & strResult       结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 CheckNewVersion(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);
};

#endif
