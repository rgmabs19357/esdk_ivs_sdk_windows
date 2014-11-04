/********************************************************************
	filename	: 	AlarmMgrXMLProcess.h
	author		:	z00193167
	created		:	2012/12/03	
	description	:	处理告警XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __ALARM_MGR_XML_PROCESS_H__
#define __ALARM_MGR_XML_PROCESS_H__

#include "IVSCommon.h"
#include "ivs_error.h"
#include "ToolsHelp.h"
#include "XmlProcess.h"
// #include <string.h>
#include "QueryFiled.h"
#include "hwsdk.h"
#include "SDKDef.h"

class CAlarmMgrXMLProcess
{
private:
	CAlarmMgrXMLProcess();
public:
	~CAlarmMgrXMLProcess();

public:
	/***********************************************************************************
    * name       : GetConfirmWaterAlarmXML
    * description: 水印告警确认xml
    * input      : ullAlarmEventID:告警ID; pConfirmInfo:告警处理人员输入的信息; pConfirmTime:告警处理时间; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetConfirmWaterAlarmXML(const IVS_UINT64 ullAlarmEventID,const IVS_CHAR* pConfirmInfo,
													const IVS_CHAR* pConfirmTime,const std::string& strUserDomainCode,const std::string& strDomainCode,CXml &xml);

	/***********************************************************************************
    * name       : GetCancelWaterAlarmXML
    * description: 水印告警撤销xml
    * input      : ullAlarmEventID:告警ID; pCancelInfo:告警取消描述信息; pCancelTime:告警取消时间; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetCancelWaterAlarmXML(const IVS_UINT64 ullAlarmEventID,const IVS_CHAR* pCancelInfo,
													const IVS_CHAR* pCancelTime,const std::string& strUserDomainCode,const std::string& strDomainCode,CXml &xml);

	/******************************************************************
	* name       : GetWaterAlarmListParseXML
	* description: 水印告警查询
	* input      : pWaterAlarmInfoList
				   strDoamianCode
				   uiBufferSize
	* output     : Rspxml
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
    *******************************************************************/   
	static IVS_INT32 GetWaterAlarmListParseXML(IVS_WATERMARK_ALARM_INFO_LIST* pWaterAlarmInfoList,const std::string& strDoamianCode, IVS_UINT32 uiBufferSize, CXml &Rspxml);

	/***********************************************************************************
    * name       : AlarmLevelGetXML
    * description: 告警级别拼接xml
    * input      : cLoginID:登录ID; pAlarmLevel:告警级别结构体; i:判断是增加告警级别还是修改; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AlarmLevelGetXML(const IVS_ALARM_LEVEL *pAlarmLevel, IVS_INT32 i, CXml &xml);

	/***********************************************************************************
    * name       : AddAlarmLevelPraseXML
    * description: 增加告警级别解析xml
    * input      : pAlarmLevelID:告警级别ID; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddAlarmLevelPraseXML(IVS_UINT32* pAlarmLevelID,CXml &xml);

	/***********************************************************************************
    * name       : DeleteAlarmLevelGetXML
    * description: 删除告警级别拼接xml
    * input      : cLoginID:登录ID; uiAlarmLevelId:告警级别ID; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DeleteAlarmLevelGetXML(const IVS_UINT32 uiAlarmLevelId, CXml &xml); 
	
	/***********************************************************************************
    * name       : GetAlarmTypeLevelPraseXML
    * description: 查询告警级别列表解析xml
    * input      : uiAlarmLevelNum:系统最大告警级别个数; pAlarmLevelNum:查询返回的系统告警级别个数; pAlarmLevel:告警类型列表; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmTypeLevelPraseXML(IVS_UINT32 uiAlarmLevelNum,IVS_UINT32* pAlarmLevelNum,IVS_ALARM_LEVEL* pAlarmLevel,CXml &xml);

	/***********************************************************************************
    * name       : GetAlarmLevelGetXML
    * description: 查询告警级别详细信息拼接xml
    * input      : pLoginID:登录ID; uiAlarmLevelId:告警级别ID; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmLevelGetXML(IVS_UINT32 uiAlarmLevelID, CXml& xml);

	/***********************************************************************************
    * name       : GetAlarmLevelPraseXML
    * description: 查询告警级别详细信息解析xml
    * input      : pAlarmLevel:告警级别结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmLevelPraseXML(IVS_ALARM_LEVEL* pAlarmLevel, CXml& xml);

	/***********************************************************************************
    * name       : HelpInfoGetXML
    * description: 告警帮助拼接xml
    * input      : cLoginID:登录ID; helpInfo:帮助信息结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 HelpInfoGetXML(const IVS_HELP_INFO *helpInfo, CXml &xml);

	/***********************************************************************************
    * name       : DeleteHelpInfoGetXML
    * description: 删除告警帮助拼接xml
    * input      : cLoginID:登录ID; uiSourceId:目标类型1：防区2：告警类型; uiSourceType:帮助信息目标ID目前支持防区/告警类型; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DeleteHelpInfoGetXML(const IVS_UINT32 uiSourceId,IVS_UINT32 uiSourceType, CXml &xml);

	/***********************************************************************************
    * name       : QueryHelpInfoGetXML
    * description: 查询告警帮助拼接xml
    * input      : uiAlarmHelpSourceType:帮助信息目标ID目前支持防区/告警类型; uiAlarmHelpSourceId:目标类型1：防区2：告警类型; helpInfo:帮助信息结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 QueryHelpInfoGetXML(IVS_UINT32 uiAlarmHelpSourceType,IVS_UINT32 uiAlarmHelpSourceId,CXml &xml);

    /******************************************************************
     function   : QueryHelpInfoTextGetXML
     description: 查询告警帮助信息正文
     input      : const IVS_CHAR * pAlarmInCode 告警源编码
     input      : const IVS_CHAR * pAlarmType 告警类型
     output      : CXml & xml
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 QueryHelpInfoTextGetXML(const IVS_CHAR* pAlarmInCode, const IVS_CHAR* pAlarmType, CXml &xml);

	/***********************************************************************************
    * name       : QueryHelpInfoPraseXML
    * description: 查询告警帮助解析xml
    * input      : helpInfo:帮助信息结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 QueryHelpInfoPraseXML(IVS_HELP_INFO *helpInfo, CXml &xml);

	/***********************************************************************************
    * name       : SetAlarmTypeLevelGetXML
    * description: 设置告警类型拼接xml
    * input      : pLoginID:登录ID; pAlarmTypeList:告警类型列表结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 SetAlarmTypeLevelGetXML(const IVS_ALARM_TYPE_LIST* pAlarmTypeList, CXml& xml);

	/***********************************************************************************
    * name       : GetAlarmTypeListParseXML
    * description: 查询自定义告警类型列表解析xml
    * input      : pAlarmTypeList:告警类型列表结构体; xml:定义XML; uiBufferSize:缓存区
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmTypeListParseXML(IVS_ALARM_TYPE_LIST* pAlarmTypeList, CXml &xml, IVS_UINT32 uiBufferSize);

	/***********************************************************************************
    * name       : UserAlarmAuthorGetXML
    * description: 告警用户认证拼接xml
    * input      : pLoginID:登录ID; pPasswordInfo:用户密码字段; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 UserAlarmAuthorGetXML(const IVS_CHAR *pPasswordInfo, CXml &xml);

	/***********************************************************************************
    * name       : GetAlarmNotifyGetXML
    * description: 告警上报拼接xml
    * input      : palarmNotify:告警上报结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmNotifyGetXML(const IVS_ALARM_NOTIFY* palarmNotify,CXml& xml);

	/***********************************************************************************
    * name       : GetAlarmTypeListGetXML
    * description: 查询告警类型列表拼接xml
    * input      : pLoginID:登录ID; palarmNotify:告警上报结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmTypeListGetXML(const IVS_CHAR* pLoginID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, CXml& xml);

	/***********************************************************************************
    * name       : GetAlarmAreaPlanXML
    * description: 防区布防计划 拼装请求xml
    * input      : pLoginID:登录ID; pAlarmAreaPlan:防区布防计划结构体; 
    * output     : xml，目标xml
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmAreaPlanXML(const IVS_CHAR* pDomainCode,IVS_UINT32& uiAlarmAreaID,CXml& xml);

	/***********************************************************************************
    * name       : AlarmAreaGetXML
    * description: 删除告警防区, 查询防区详情，手动布撤防拼接XML
    * input      : pDomainCode:域编码; uiAlarmAreaId:防区ID; xml:拼接的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AlarmAreaGetXML(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiAlarmAreaId, CXml &xml);

	/***********************************************************************************
    * name       : GetAlarmListParseXML
    * description: 查询历史告警信息列表拼接XML
    * input      : pAlarmEventList:告警上报信息分页查询列表; uiBufferSize:计划分配的内存大小; xml:拼接的xml,strDomainCode,域编码，拼接在AlarmInCode中
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmListParseXML(IVS_ALARM_EVENT_LIST* pAlarmEventList, IVS_UINT32 uiBufferSize, CXml &xml, const std::string& strDomainCode);

	/***********************************************************************************
    * name       : GetAlarmStatusGetXML
    * description: 告警状态上报拼接XML
    * input      : pAlarmEventList:告警上报信息分页查询列表; uiBufferSize:计划分配的内存大小; xml:拼接的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmStatusGetXML(const IVS_ALARM_EVENT* pAlarmEvent, CXml& xml);

	/***********************************************************************************
    * name       : GetAlarmInfoGetXML
    * description: 查询告警信息事件拼接XML
    * input      : pLoginID:登录ID; ullAlarmEventID:告警事件ID; xml:拼接的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmInfoGetXML(IVS_UINT64 ullAlarmEventID, CXml &xml);


	/***********************************************************************************
    * name       : GetAlarmInfoParseXML
    * description: 查询告警信息事件解析XML
    * input      : pAlarmEvent:告警信息结构体; xml:解析的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmInfoParseXML(IVS_ALARM_EVENT* pAlarmEvent, CXml &xml, const IVS_CHAR* pDomainCode);

	/***********************************************************************************
    * name       : GetDeviceUserListGetXML
    * description: 获取拥有指定设备权限的用户列表拼接XML
    * input      : pDevCode:设备编码; pIndexRange:分页信息 xml:拼接的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetDeviceUserListGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, IVS_UINT32 uiIsUserOnline, const IVS_INDEX_RANGE* pIndexRange, const std::string& strGroupID, CXml &xml);

	/***********************************************************************************
    * name       : GetDeviceUserListParseXML
    * description: 获取拥有指定设备权限的用户列表解析XML
    * input      : pDevUserList:设备用户列表结构体; uiBufferSize:缓存大小 xml:解析的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetDeviceUserListParseXML(IVS_DEVICE_USER_INFO_LIST* pDevUserList, IVS_UINT32 uiBufferSize, CXml &xml);

		/***********************************************************************************
    * name       : GetAbilityInfoParseXML
    * description: 获取拥有指定设备权限的用户列表解析XML
    * input      : uiAbility:平台支持的能力值 xml:解析的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAbilityInfoParseXML(IVS_UINT32* uiAbility,   const char*);

	/***********************************************************************************
    * name       : SetAlarmInCodeValue
	* description: 告警订阅处理请求的AlarmInCode(增，删，改通用)
	* input      : xml，请求的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 SetAlarmInCodeValue(CXml& xml);

	/***********************************************************************************
    * name       : ModifyAlarmInCodeOfGetAlarmList
    * description: 查询告警历史列表修改AlarmInCode字段
    * input      : xml，请求的xml;
    * output     : std::string strDstDomainCode 消息目标域
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyAlarmInCodeOfGetAlarmList(CXml& xml, std::string& strDstDomainCode);

	/***********************************************************************************
    * name       : GetDevAlarmListParseXML
    * description: 查询OMU告警事件列表
    * input      : xml，响应消息xml;
	               pDevAlarmList，告警事件列表结构体
				   uiBufSize, 缓存大小
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetDevAlarmListParseXML(CXml& xml, IVS_DEVICE_ALARM_EVENT_LIST* pDevAlarmList, const IVS_UINT32& uiBufSize);

	/***********************************************************************************
    * name       : AlarmOutGetXML
    * description: 启停告警开关量拼装xml
    * input      : xml，响应消息xml;
	               pAlarmInCode，告警输出设备编码
				   pDomainCode, 域编码
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AlarmOutGetXML(CXml& xml, const IVS_CHAR* pAlarmInCode, const IVS_CHAR* pDomainCode, IVS_INT32 iStateType);

	/***********************************************************************************
    * name       : GetAlarmAreaListGetDomainCode
    * description: 获取防区信息列表从请求xml中获取域编码,拆分查询条件中的AlarmInCode
    * input      : pReqXml，请求xml;
    * output     : strDomainCode， 域编码  xml，修改之后的xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetDomainCodeANDParseAlarmInCode(const IVS_CHAR* pReqXml, std::string& strDomainCode, CXml& xml);

   /***********************************************************************************
    * name       : GetGroupListGetXml
    * description: 根据外域设备取得设备组列表拼装xml
    * input      : pDevCode，设备编码
	               pDomainCode, 域编码
    * output     : xml，拼装之后的xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetGroupListGetXml(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, CXml& xml);

    /***********************************************************************************
    * name       : GetGroupID
    * description: 获取GroupID，将多个GroupID连在一起，中间用","隔开
    * input      : xml，源xml
    * output     : strGroupID，多个GroupID组合之后的字符串
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetGroupID(CXml& xml, std::string& strGroupIDs);
	/***********************************************************************************
    * name       : GetRsqXML
    * description: 透传接口中，将相应消息从string转到pRsqXml所指内存中
    * input      : pRsp, 响应消息
    * output     : pRsqXml， 转之后的xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetRsqXML(const IVS_CHAR* pRsp, IVS_CHAR*& pRspXml);

   /***********************************************************************************
    * name       : SetAlarmInCode4AlarmAreaReq
    * description: 拆分AlarmInCode，添加DevDomainCode节点(可以增加空防区，即可能没有告警源设备信息信息)，在新增和修改防区中使用
    * input      : iMsgType, 消息类型
	               pReqXml， 请求xml
				   pDomainCode， 消息目标域
				   iNeedRsp, 是否需要响应消息
    * output     : strpRsp， 响应消息
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 SetAlarmInCode4AlarmAreaReq(CXml& xml);

	/***********************************************************************************
    * name       : GetDstDomainCode
    * description: 获取消息目标域
    * input      : pReqXml， 请求xml
    * output     : strDomainCode， 消息目标域
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetDstDomainCode(CXml& xml, const IVS_CHAR* pReqXml, std::string& strDomainCode);

	/***********************************************************************************
    * name       : GetAlarmSnapListGetXml
    * description: 获取抓拍图片列表拼装请求xml
    * input      : ulAlarmEventID， 事件ID
	               pAlarmInCode，告警源编码
				   strDomainCode， 消息目标域
    * output     : xml， 请求xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmSnapListGetXml(IVS_ULONG ulAlarmEventID, const IVS_CHAR* pAlarmInCode, const std::string& strDomainCode, CXml& xml);

    /***********************************************************************************
    * name       : SetDevCode
    * description: 拆分设备编码，取出域编码作为消息目标域, 设备布撤防，报警复位用到
    * input      : xml， 请求xml
    * output     : pDevDomainCode， 消息目标域
				   xml, 修改之后的xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 SetDevCode(CXml& xml, const IVS_CHAR* pReqXml, const std::string& strPath, IVS_CHAR* pDevDomainCode, IVS_CHAR* pDevCode);

    /***********************************************************************************
    * name       : ParseLocalDevCode
    * description: 拆分设备编码，
    * input      : xml， 请求xml
	               pReqXml, 请求xml
				   pGrandPath, 一级路径
				   pParentPath， 二级路径
				   xml, 修改之后的xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ParseLocalDevCode(CXml& xml, const IVS_CHAR* pReqXml, const IVS_CHAR* pGrandPath, const IVS_CHAR* pParentPath);

	/***********************************************************************************
    * name       : ModifyLocalDevCode
    * description: 拼装设备编码，
    * input      : xml， 请求xml
				   pGrandPath, 一级路径
				   pParentPath， 二级路径
				   xml, 修改之后的xml
				   strDomainCode，在LocalDevCode后面拼装的xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyLocalDevCode(CXml& xml, const std::string& strDomainCode, const IVS_CHAR* pGrandPath, const IVS_CHAR* pParentPath);

	/***********************************************************************************
    * name       : GetAlarmSnapshotListGetXml
    * description: 组合最后响应的xml，
    * input      : xml， 请求xml
				   strInXml, 每次解析出的xml
				   iFlag, 标记是否是第一次解析出来的xml，第一次的不用做处理
    * output     :strOutXml, 最后组合好的xml
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmSnapshotListGetXml(const std::string& strInXml, std::string& strOutXml, IVS_INT32& iFlag);
};
#endif // __ALARM_MGR_XML_PROCESS_H__
