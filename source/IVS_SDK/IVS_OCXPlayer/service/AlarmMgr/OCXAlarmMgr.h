/********************************************************************
	filename	: 	OCXAlarmMgr.h
	author		:	z00193167
	created		:	2012/12/06	
	description	:	处理告警OCX
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本
*********************************************************************/

#ifndef __OCX_ALARM_MGR_H__
#define __OCX_ALARM_MGR_H__

#include "hwsdk.h"

class COCXAlarmMgr
{
public:
	COCXAlarmMgr(void);
	~COCXAlarmMgr(void);

	/***********************************************************************************
    * name       : AddAlarmLevel
    * description: 新增告警级别ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数; strResult:返回值
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID AddAlarmLevel(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmLevel, CString &strResult);

	/***********************************************************************************
    * name       : ModifyAlarmLevel
    * description: 修改告警级别ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyAlarmLevel(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmLevel);

	/***********************************************************************************
    * name       : ModifyAlarmLevel
    * description: 修改告警级别ocx
    * input      : iSessionId:登录Id; uiAlarmLevelId:CU传参数(告警级别Id);
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DeleteAlarmLevel(IVS_INT32 iSessionId, IVS_UINT32 uiAlarmLevelId);

	/***********************************************************************************
    * name       : GetAlarmLevel
    * description: 查询级别详细信息ocx
    * input      : iSessionId:登录Id; uiAlarmLevelId:CU传参数(告警级别Id); strResult:返回值;
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID GetAlarmLevel(IVS_INT32 iSessionId, IVS_INT32 iAlarmLevelID, CString &strResult);

	/***********************************************************************************
    * name       : GetAlarmLevelList
    * description: 查询级别列表ocx
    * input      : iSessionId:登录Id; strResult:返回值;
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID GetAlarmLevelList(IVS_INT32 iSessionId, CString &strResult);

	/***********************************************************************************
    * name       : AddHelpInfo
    * description: 增加告警帮助ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : ModifyHelpInfo
    * description: 修改告警帮助ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : DeleteHelpInfo
    * description: 删除告警帮助ocx
    * input      : iSessionId:登录Id; uiAlarmHelpSourceType:告警帮助源类型; uiAlarmHelpSourceId:告警帮助源Id; 
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DeleteHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pHelpInfo);

	/***********************************************************************************
    * name       : QueryHelpInfo
    * description: 查询告警帮助信息ocx
    * input      : iSessionId:登录Id; uiAlarmHelpSourceType:告警帮助源类型; uiAlarmHelpSourceId:告警帮助源Id; strResult:返回值;
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID QueryHelpInfo(IVS_INT32 iSessionId, const IVS_CHAR* pHelpInfo, CString &strResult);

	/***********************************************************************************
    * name       : SetAlarmTypeLevel
    * description: 设置告警类型级别ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 SetAlarmTypeLevel(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : GetAlarmTypeList
    * description: 查询告警类型列表ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数; strResult:返回值;
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID GetAlarmTypeList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult);

	/***********************************************************************************
    * name       : SubscribeAlarm
    * description: 新增告警订阅ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 SubscribeAlarm(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : SubscribeAlarm
    * description: 新增告警订阅ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数; strResult:返回值;
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID SubscribeAlarmQuery(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult);

	/***********************************************************************************
    * name       : ModifyAlarmArea
    * description: 修改告警防区ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyAlarmArea(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : DeleteAlarmArea
    * description: 删除告警防区ocx
    * input      : iSessionId:登录Id; uiAlarmAreaId:CU传参数;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DeleteAlarmArea(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId);

	/***********************************************************************************
    * name       : GetAlarmAreaList
    * description: 查询告警防区列表ocx
    * input      : iSessionId:登录Id; pReqXml:CU传参数; strResult:返回值;
    * output     : strResult:返回给CU的xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID GetAlarmAreaList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult);

	/***********************************************************************************
    * name       : UserAuthentication
    * description: 用户认证ocx
    * input      : iSessionId:登录Id; pPasswordInfo:用户密码;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 UserAuthentication(IVS_INT32 iSessionId, const IVS_CHAR* pPWD);

	/***********************************************************************************
    * name       : AllocAreaGuardPlan
    * description: 指定防区布防计划
    * input      : iSessionId:登录Id; pReqXml:请求xml;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AllocAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : ModifyAreaGuardPlan
    * description: 修改防区布防计划
    * input      : iSessionId:登录Id; pReqXml:请求xml;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : DeleteAreaGuardPlan
	* description: 删除防区布防计划
	* input      : iSessionId:登录Id; ulAlarmAreaId:防区ID;
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 DeleteAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId);

	/***********************************************************************************
    * name       : GetAreaGuardPlan
	* description: 获取防区布防计划
	* input      : iSessionId:登录Id; ulAlarmAreaId:防区ID;
	* output     : RspXml 响应消息xml
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetAreaGuardPlan(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId, CString& strResult);

	/***********************************************************************************
    * name       : AddAlarmArea
	* description: 增加防区
	* input      : iSessionId:登录Id; 
	* output     : xmlRsp 响应消息xml
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 AddAlarmArea(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml,  CString& strResult);

	/***********************************************************************************
    * name       : GetAlarmAreaInfo
	* description: 获取防区详细信息
	* input      : iSessionId:登录Id; pReqXml:请求消息;
	* output     : pRspXml 响应消息
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetAlarmAreaInfo(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId, CString& strResult);

	/***********************************************************************************
    * name       : StartAlarmAreaGuard
	* description: 手动布防
	* input      : iSessionId:登录Id; ulAlarmAreaId:防区ID;
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 StartAlarmAreaGuard(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId);
	
	/***********************************************************************************
    * name       : StopAlarmAreaGuard
	* description: 手动撤防
	* input      : iSessionId:登录Id; ulAlarmAreaId:防区ID;
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 StopAlarmAreaGuard(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, ULONG ulAlarmAreaId);

	/***********************************************************************************
    * name       : CopySubscribeAlarm
	* description: 告警订阅复制
	* input      : iSessionId:登录Id; uiSrcUserID:源用户ID;uiDstUserID，目标用户ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 AlarmCommission(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : GetAlarmList
	* description: 查询告警历史信息列表
	* input      : iSessionId:登录Id; pReqXml:CU传XML;strResult:返回值
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetAlarmList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult);

	/***********************************************************************************
    * name       : GetAlarmInfo
	* description: 查询告警信息事件
	* input      : iSessionId:登录Id; uiAlarmEventId:告警事件ID;pAlarmInCode:告警源编码; strResult:返回值
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetAlarmInfo(IVS_INT32 iSessionId, IVS_UINT64 uiAlarmEventId, const IVS_CHAR* pAlarmInCode, CString &strResult);

	/***********************************************************************************
    * name       : AlarmCancel
	* description: 告警撤销
	* input      : iSessionId:登录Id; pReqXml:CU传XML;
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 AlarmCancelORConfirm(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, const IVS_INT32& iType);

	/***********************************************************************************
    * name       : AlarmReport
	* description: 告警信息上报
	* input      : iSessionId:登录Id; uiAlarmEventId:告警事件ID;strResult:返回值
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 AlarmReport(IVS_INT32 iSessionId, const IVS_CHAR* pReqXML);

	/***********************************************************************************
    * name       : GetDeviceUserList
	* description: 获取拥有指定设备权限的用户列表
	* input      : iSessionId:登录Id; pDevCode:设备编码; uiFromIndex:开始索引; uiToIndex:结束索引; strResult:返回值
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetDeviceUserList(IVS_INT32 iSessionId, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, IVS_UINT32 ulIsUserOnline, IVS_UINT32 uiFromIndex, IVS_UINT32 uiToIndex, CString &strResult);

	/***********************************************************************************
    * name       : GetConfigInfo
	* description: 获取平台配置信息
	* input      : iSessionId:登录Id; pReqXml:CU传参数; strResult:返回值
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetConfigInfo(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult);

		/***********************************************************************************
    * name       : GetAbilityInfo
	* description:查询平台能力集信息
	* input      : iSessionId:登录Id
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_ULONG GetAbilityInfo(IVS_INT32 iSessionId);

	/***********************************************************************************
    * name       : StartAlarmOut
	* description: 启动开关量输出告警
	* input      : iSessionId:登录Id; pAlarmOutCode，开关量输出设备编码
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 StartAlarmOut(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmOutCode);

	/***********************************************************************************
    * name       : StopAlarmOut
	* description: 停止开关量输出告警
	* input      : iSessionId:登录Id; pAlarmOutCode，开关量输出设备编码
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 StopAlarmOut(IVS_INT32 iSessionId, const IVS_CHAR* pAlarmOutCode);

	/***********************************************************************************
    * name       : GetDeviceAlarmList
	* description: 查询OMU告警事件列表
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetDeviceAlarmList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : SetDeviceGuard
	* description: 设备布撤防
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 SetDeviceGuard(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : ResetAlarm
	* description: 报警复位
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 ResetAlarm(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : ExEventSubscribe
	* description: 外部域告警事件订阅
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 ExEventSubscribe(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : GetExEventSubscribe
	* description: 查询外部域告警事件订阅
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetExEventSubscribe(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : ExCatalogSubscribe
	* description: 外部域设备目录订阅
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 ExCatalogSubscribe(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : GetExCatalogSubscribeList
	* description: 查询外部域设备目录订阅
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetExCatalogSubscribeList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);

   /***********************************************************************************
    * name       : GetAlarmSnapshotList
	* description: 获取告警抓拍图片列表
	* input      : iSessionId:登录Id; pReqXml，查询请求xml
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetAlarmSnapshotList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : ReleaseListBuffer
	* description: 释放list中的内存
	* input      : listDevUserInfoList， list对象
	* output     : NA
	* return     : 无返回值
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 ReleaseListBuffer(std::list<IVS_DEVICE_USER_INFO_LIST*> listDevUserInfoList);
};

#endif
