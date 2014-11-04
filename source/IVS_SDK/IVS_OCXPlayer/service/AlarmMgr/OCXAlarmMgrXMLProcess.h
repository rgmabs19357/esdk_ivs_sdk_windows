/********************************************************************
	filename	: 	OCXAlarmMgrXMLProcess.h
	author		:	z00193167
	created		:	2012/12/03	
	description	:	处理告警XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __OCX_ALARM_MGR_XML_PROCESS_H__
#define __OCX_ALARM_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"
#include <atlstr.h>
#include "SDKDef.h"

class COCXAlarmMgrXMLProcess
{
private:
	COCXAlarmMgrXMLProcess();
public:
	~COCXAlarmMgrXMLProcess();

public:
	/******************************************************************
	function : GetWatermarkAlarmListGetXML
	description: 查询水印告警列表请求xml;
	input : pWatermarkAlarmList，水印告警列表；iRetCord，结果码；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetWatermarkAlarmListGetXML(IVS_WATERMARK_ALARM_INFO_LIST *pWatermarkAlarmList,IVS_INT32 iRetCord,CXml &xml);

	/******************************************************************
	function : GetWatermarkAlarmCancelInfo
	description: 获取水印告警撤销的结构;
	input : pCancelInfo，水印告警撤销XML；
	output : pstCancelInfo :结构信息
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetWatermarkAlarmCancelInfo(const IVS_CHAR * pCancelInfo,IVS_WATERMARK_ALARM_CANCEL* pstCancelInfo);

	/******************************************************************
	function : GetWatermarkAlarmConfirmInfo
	description: 获取水印告警确认的结构;
	input : pCancelInfo，水印告警确认XML；
	output : pstCancelInfo :结构信息
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetWatermarkAlarmConfirmInfo(const IVS_CHAR * pConfirmInfo,IVS_WATERMARK_ALARM_CONFIRM* pstConfirmInfo);

	/******************************************************************
	function : GetWaterTIPReportInfoXML
	description: 获取水印告警提示XML
	input : pWaterAlarmInfo，水印告警信息
	output : xml 
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetWaterTIPReportInfoXML(const IVS_ALARM_WATERMARK_INFO * pWaterAlarmInfo, CXml &xml);

	/******************************************************************
	function : GetWaterAlarmReportInfoXML
	description: 获取水印篡改告警
	input : pWaterAlarmInfo，水印告警信息
	output : xml 
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetWaterAlarmReportInfoXML(const IVS_ALARM_WATERMARK_INFO * pWaterAlarmInfo, CXml &xml);

	/***********************************************************************************
    * name       : AlarmLevelPraseXML
    * description: 告警级别通用解析XML
    * input      : pAlarmLevel:告警级别结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AlarmLevelPraseXML(IVS_ALARM_LEVEL *pAlarmLevel, CXml &xml);
	
	/***********************************************************************************
    * name       : HelpInfoPraseXML
    * description: 告警帮助通用解析XML
    * input      : helpInfo:帮助信息结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 HelpInfoPraseXML(IVS_HELP_INFO *helpInfo, CXml &xml, IVS_CHAR* pDomainCode);

	/***********************************************************************************
    * name       : QueryHelpInfoGetXML
    * description: 查询告警帮助拼接XML
    * input      : cstrRet:返回给CU的字符串; helpInfo:帮助信息结构体; iRet:错误码; uiAlarmHelpSourceType:帮助信息类型; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 QueryHelpInfoGetXML(CString &cstrRet, const IVS_HELP_INFO *helpInfo, IVS_INT32 iRet, CXml &xml);

    /******************************************************************
     function   : QueryHelpInfoTextGetXML
     description: 查询告警帮助信息正文
     input      : CString & cstrRet
     input      : const char * pHelpInfoText 帮助信息正文
     input      : IVS_INT32 iRet 
     output      : CXml & xml
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 QueryHelpInfoTextGetXML(CString &cstrRet, const char *pHelpInfoText, IVS_INT32 iRet, CXml &xml);

	/***********************************************************************************
    * name       : GetAlarmLevelListGetXML
    * description: 查询告警级别列表拼接XML
    * input      : cstrRet:返回给CU的字符串; pNum:获得的告警级别个数; pAlarmLevel:告警级别结构体; iRet:错误码; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmLevelListGetXML(CString &cstrRet,const IVS_UINT32* pNum ,const IVS_ALARM_LEVEL *pAlarmLevel,IVS_INT32 iRet, CXml &xml);

	/***********************************************************************************
    * name       : GetAlarmLevel
    * description: 查询告警级别+错误码拼接XML
    * input      : cstrRet:返回给CU的字符串; pAlarmLevel:告警级别结构体; iRet:错误码; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmLevel(CString &cstrRet,const IVS_ALARM_LEVEL *pAlarmLevel,IVS_INT32 iRet,CXml &xml);

	/***********************************************************************************
    * name       : GetAlarmLevel
    * description: 查询告警类型列表拼接XML
    * input      : pAlarmTypeList:告警类型列表结构体; xmlRsp:定义XML; bufSize:缓冲区;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmTypeListGetXml(IVS_ALARM_TYPE_LIST* pAlarmTypeList,CXml& xmlRsp, unsigned int bufSize);

	/***********************************************************************************
    * name       : GetAddAlarmErrString
    * description: 增加告警级别拼接XML
    * input      : strResult:返回给CU的字符串; iNum:告警ID; iRet:错误码; 
    * output     : NA
    * return     : 无返回值
    * remark     : NA
    ***********************************************************************************/
	static IVS_VOID GetAddAlarmErrString(CString &strResult,IVS_UINT32 iNum, IVS_INT32 iRet);

	/***********************************************************************************
    * name       : SetAlarmTypeLevelGetXml
    * description: 设置告警类型级别拼接XML
    * input      : ReqXml:定义XML; pAlarmType:告警类型结构体; 
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 SetAlarmTypeLevelGetXml(CXml &ReqXml, IVS_ALARM_TYPE_LIST* pAlarmType);

	/***********************************************************************************
    * name       : AlarmReportGetXML
    * description: 告警上报拼接XML
    * input      : pAlarmNotify:告警上报结构体; xml:定义XML; 
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AlarmReportGetXML(const IVS_ALARM_NOTIFY* pAlarmNotify, CXml& xml);

	/***********************************************************************************
    * name       : GeneralGetString
    * description: 通用获取返回码
    * input      : strResult:返回码; pRspXml:SMU返回的字符串; iRet:错误码;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GeneralGetString(CString &strResult, const IVS_CHAR* pRspXml, IVS_INT32 iRet);

	/***********************************************************************************
    * name       : ParseAlarmCommissionXML
    * description: 解析告警授权请求Xml
    * input      : pReqXml:请求xml; pAlarmCommission:告警授权结构体
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ParseAlarmCommissionXML(const IVS_CHAR* pReqXml, IVS_CHAR* pAlarmInCode, IVS_ALARM_COMMISSION*& pAlarmCommission) ;


	/***********************************************************************************
    * name       : GetAlarmListGetXML
    * description: 拼接查询告警历史信息列表Xml
    * input      : strResult:返回值; pResAlarmEventList:告警上报信息分页查询列表; iRet:错误码; xml;拼接XML
    * output     : NA
    * return     : 无返回值
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmListGetXML(CString &strResult, IVS_ALARM_EVENT_LIST* pResAlarmEventList, IVS_INT32 &iRet, CXml &xml);

	/***********************************************************************************
    * name       : GetAlarmListGetXML
    * description: 拼接查询告警历史信息列表Xml
    * input      : strResult:返回值; pResAlarmEventList:告警上报信息分页查询列表; iRet:错误码; xml;拼接XML
    * output     : NA
    * return     : 无返回值
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmInfoGetXML(CString &strResult, const IVS_ALARM_EVENT* pAlarmEvent);

	/***********************************************************************************
    * name       : GetAlarmStatusGetXML
    * description: 告警状态上报拼接XML
    * input      : pAlarmEventList:告警上报信息分页查询列表; uiBufferSize:计划分配的内存大小; xml:拼接的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmStatusGetXML(const IVS_ALARM_STATUS_NOTIFY* pAlarmStatusNotify, CXml& xml);

	/***********************************************************************************
    * name       : GetAlarmStatusGetXML
    * description: 告警信息上报解析XML
    * input      : pAlarmReport:告警上报结构体; pReqXml，cu传过来的xml
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ParseAlarmReportXML(const IVS_CHAR* pReqXml, IVS_ALARM_REPORT* pAlarmReport);

	/***********************************************************************************
    * name       : AlarmConfirmParseXML
    * description: 告警确认解析XML
    * input      : ullAlarmEventID:告警事件Id; pAlarmOperateInfo:告警处理信息结构体; xml:拼接的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AlarmConfirmParseXML(IVS_UINT64* ullAlarmEventID, IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo, IVS_CHAR* pAlarmInCode, CXml &xml);

	/***********************************************************************************
    * name       : GetDeviceUserList
    * description: 获取拥有指定设备权限的用户列表拼接XML
    * input      : pResAlarmEventList:设备用户列表; xml:拼接的xml;
    * output     : NA
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetDeviceUserList(std::list<IVS_DEVICE_USER_INFO_LIST*>& listDevUserInfoList, CString &strResult, IVS_UINT32 uiFromIndex, IVS_UINT32 uiToIndex, CXml &xml);

	/***********************************************************************************
    * name       : AddResultCodeUp
    * description: 将ResultCode拼在报文的前面
    * input      : pRsqXml:SMU返回的数据; xml:拼接的xml;
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddResultCodeUp(const char* pRsqXml, CString &strResult);
	
	/***********************************************************************************
    * name       : GetUserNum
    * description: 得到告警授权请求xml中UserID的个数
    * input      : pReqXml:CU请求数据; 
    * output     : iNum，UserID个数
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetUserNum(const char* pReqXml, IVS_INT32& iNum);

	/***********************************************************************************
    * name       : GetDevAlarmListGetXML
    * description: 查询OMU告警事件列表拼装响应xml
    * input      : xml，目标xml;
	               pDevAlarmList，告警事件列表结构体
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetDevAlarmListGetXML(CXml& xml, IVS_DEVICE_ALARM_EVENT_LIST* pDevAlarmList, IVS_INT32& iRet);

	/***********************************************************************************
    * name       : DevAlarmNotifyGetXML
    * description: OMU设备告警上报拼装xml
    * input      : xml，目标xml;
	               pDevAlarmNotify，设备告警上报结构体
    * output     : NA
    * return     : 成功返回正确码，失败返回错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DevAlarmNotifyGetXML(CXml& xml, const IVS_DEVICE_ALARM_NOTIFY* pDevAlarmNotify);

    /******************************************************************
     function   : ReLoginGetXML
     description: 拼接重登录事件内容
     input      : CXml & xml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ReLoginGetXML(CXml& xml);

	/******************************************************************
     function   : ParseHelpInfoXml
     description: 删除，查询帮助信息，解析请求xml
     input      : pHelpInfo， 请求xml
     output     : ulSourceID， 帮助信息目标ID
				  ulSourceType， 目标类型
				  pDomainCode， 消息目标域编码
     return     : 成功返回0，失败返回错误码
    *******************************************************************/
    static IVS_INT32 ParseHelpInfoXml(const IVS_CHAR* pHelpInfo, IVS_UINT32&  ulSourceID, IVS_UINT32& ulSourceType, IVS_CHAR* pDomainCode);



	/******************************************************************
     function   : GetRspXml
     description: 获取告警抓拍图片列表，若没有响应消息，拼装响应xml
     input      : iRet ，响应码
	              strResult，响应xml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetRspXml(CString& strResult, IVS_INT32 iRet);
};



#endif // __OCX_ALARM_MGR_XML_PROCESS_H__
