/********************************************************************
	filename	: 	OCXLogMgrXMLProcess.h
	author		:	songjianfeng(00193168)
	created		:	2012/12/03	
	description	:	处理媒体视频回放的XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/
#ifndef __OCX_RECORD_XML_PROCESS_H__ 
#define __OCX_RECORD_XML_PROCESS_H__

#include "OCXDatatype.h"
#include "SDKDef.h"

class COCXRecordXMLProcess
{
private:
	COCXRecordXMLProcess();
public:
	~COCXRecordXMLProcess();

public:

	/******************************************************************
	function : GetRecordListGetXML
	description: 查询录像列表请求xml;
	input : pCameraCode,摄像机编号；pRecordInfoList，录像信息列表；iRetCord，结果码；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetRecordListGetXML(const IVS_CHAR* pCameraCode,IVS_RECORD_INFO_LIST *pRecordInfoList,IVS_INT32 iRetCord,CXml &xml);

	/******************************************************************
	function : GetQueryUnifiedCameraCode
	description: 获取录像查询条件中的CameraCode;
	input : pCameraCode,摄像机编号；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetQueryUnifiedCameraCode(CXml &xml,IVS_CHAR* pCameraCode);
	
	/******************************************************************
	function : AddOrModRecordPlanGetXML
	description: 添加/修改录像计划:将请求XML转换为结构体;
	input : uiDeviceNum，摄像机数目；pDeviceList,摄像机编号；pRecordPlan，录像计划操作结果；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 AddOrModRecordPlanGetXML(IVS_UINT32& uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, IVS_RECORD_PLAN *pRecordPlan, CXml &xml);

	/******************************************************************
	function : DelRecordPlanGetXML
	description: 删除录像计划:将请求XML转换为结构体;
	input :  uiDeviceNum，摄像机数目；pDeviceList,摄像机编号；uiRecordMethod，录像方式；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 DelRecordPlanGetXML(IVS_UINT32& uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, IVS_UINT32 &uiRecordMethod, CXml &xml);

	/******************************************************************
	function : GetRecordPlanGetXML
	description: 查询录像计划:将请求XML转换为结构体;
	input : pCameraCode,摄像机编号；uiRecordMethod，录像计划方式；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetRecordPlanGetXML(const char* pCameraCode, IVS_UINT32 &uiRecordMethod, CXml &xml);

	/******************************************************************
	function : RecordPlanGetRspXML
	description: 添加/修改/删除录像计划:将结果结构体转换为响应XML;
	input : iRet，结果码；resultList，录像计划操作结果；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    static IVS_INT32 RecordPlanGetRspXML(IVS_INT32 iRet,const IVS_RESULT_LIST* resultList, CXml &xml);

	/******************************************************************
	function : RecordPlanGetRspXMLBypDeviceList
	description: 添加/修改/删除录像计划:将结果结构体转换为响应XML(用于异常情况返回);
	input : iRet，结果码；uiDeviceNum，摄像机数目；pDeviceList,摄像机编号；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static void RecordPlanGetRspXMLBypDeviceList(IVS_INT32 iRet,IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, CXml &xml);

	/******************************************************************
	function : GetRecordPlanGetRspXML
	description: 查询录像计划:将结果结构体转换为响应XML;
	input : iRet，结果码；pCameraCode,摄像机编号；pRecordPlan，录像计划查询结果；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetRecordPlanGetRspXML(IVS_INT32 iRet,const char* pCameraCode,const IVS_RECORD_PLAN* pRecordPlan, CXml &xml);

	/******************************************************************
	function : GetRecordStatusRspXML
	description: 录像状态查询;
	input : iRet，结果码；pCameraCode,摄像机编号；uiRecordMethod，录像计划方式；ulRecordStatus，录像状态；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetRecordStatusRspXML(IVS_INT32 iRet,const char* pCameraCode,IVS_UINT32 ulRecordMethod,IVS_UINT32 ulRecordStatus, CXml &xml);

	/******************************************************************
	function : RecordPolicyPraseXML
	description: 设置录像策略响应XML;
	input : pCameraCode,摄像机编号；pRecordSpace，录像策略（时间）；xml，响应xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 RecordPolicyPraseXML(IVS_RECORD_POLICY_TIME* pRecordSpace, IVS_CHAR *cCameraCode,CXml &xml);

	/******************************************************************
	function : GetRecordPolicyGetXML
	description: 录像策略解析响应消息XML;
	input : pCameraCode,摄像机编号；pRecordPolicy，录像策略信息；xml，响应xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetRecordPolicyGetXML(const IVS_RECORD_POLICY_TIME* pRecordPolicy ,LPCTSTR cCameraCode ,CXml &xml);

	/******************************************************************
	function : LocalRecordParseXML
	description: 解析本地录像配置参数XML;
	input : LocalParam,本地录像参数；pXmlStr，XML请求字符串
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    static IVS_INT32 LocalRecordParseXML(IVS_LOCAL_RECORD_PARAM &LocalParam, LPCTSTR pXmlStr);

	/******************************************************************
	function : LocalRecordParseClientConfig
	description: 读取CU配置XML 转测使用;
	input : LocalParam,本地录像参数；pXmlStr，XML请求字符串
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 LocalRecordParseClientConfig(IVS_LOCAL_RECORD_PARAM &LocalParam, LPCTSTR pXmlStr);

	/******************************************************************
	function : LocalRecordBuildXML
	description: 本地录像创建xml;
	input : pInfo,本地录像文件上报信息；xml，创建的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 LocalRecordBuildXML(IVS_LOCAL_RECORD_SUCCESS_INFO *pInfo, 
		           unsigned long ulHandle, CXml &xml);

	/******************************************************************
	function : LocalRecordBuidlXMLGeneric
	description: 本地录像请求xml;
	input : code,编号；wndID，窗格ID；xml，响应xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 LocalRecordBuidlXMLGeneric(int code, int wndID, CXml &xml);

	/******************************************************************
	function : RecordDownLoadParseXml
	description: 平台/前端录像下载解析xml;
	input :  pXmlStr xml字符串
	output : pParam  参数结构体
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 RecordDownLoadParseXml(IVS_DOWNLOAD_PARAM &DLParam, LPCTSTR pXmlStr);

	/******************************************************************
	function : RecordDownLoadBuildXml
	description: 平台/前端录像下载返回xml;
	input : ulHandle 下载通道句柄，uiCode 错误码
	output : NA
	return : 成功-0 失败-1 
	*******************************************************************/
	static IVS_INT32 RecordDownLoadBuildXml(unsigned long ulHandle, unsigned int uiCode, CXml &xml);

	/******************************************************************
	function : RecordDownLoadInfoBuildXml
	description: 平台/前端录像下载信息;
	input  : ulHandle 下载通道句柄，IVS_DOWNLOAD_INFO 下载信息
	output : NA
	return : 成功-0 失败-1 
	*******************************************************************/
	static IVS_INT32 RecordDownLoadInfoBuildXml(int ResultCode, IVS_DOWNLOAD_INFO& DLInfo, CXml &xml);

	/******************************************************************
	function : GetRecordBookmarkListGetXML
	description: 录像书签列表;
	input : pCameraCode,摄像机编号；pRecordBookmarkInfoList，录像书签信息列表；iRetCode，结果码；xml，响应xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetRecordBookmarkListGetXML(const IVS_CHAR* pCameraCode,IVS_RECORD_BOOKMARK_INFO_LIST *pRecordBookmarkInfoList,IVS_INT32 iRetCode,CXml &xml);

	/***********************************************************************************
    * name       : ManualRecordStateGetXML
    * description: 手动录像状态上报拼接XML
    * input      : pManualRecordStatusNotify:手动录像状态上报结构体; xml:定义XML; 
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ManualRecordStateGetXML(const IVS_MANUAL_RECORD_STATUS_NOTIFY* pManualRecordStatusNotify, CXml& xml);

	/***********************************************************************************
    * name       : LockRecordGetXML
    * description: 录像锁定解析xml
    * input      : pRecordLockInfo:录像锁定信息结构体; 
	               strLockInfo:请求XML;
				   strCameraCode：摄像机编号
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 LockRecordGetXML(const std::string& strLockInfo,std::string& strDomainCode, std::string& strNVRCode, std::string& strCameraCode, IVS_RECORD_LOCK_INFO* pRecordLockInfo);

	/***********************************************************************************
    * name       : ModLockRecordGetXML
    * description: 修改录像锁定信息解析xml
    * input      : pRecordLockInfo:录像锁定信息结构体; 
	               strLockInfo:请求XML;
				   strCameraCode：摄像机编号
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModLockRecordGetXML(const std::string& strLockInfo,std::string& strDomainCode, std::string& strNVRCode, std::string& strCameraCode, IVS_RECORD_LOCK_INFO* pRecordLockInfo);


	/***********************************************************************************
    * name       : UnLockRecordGetXML
    * description: 录像解锁解析xml
    * input      : pRecordLockInfo:录像锁定信息结构体; 
	               strUnLockInfo:请求XML;
				   strCameraCode：摄像机编号
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 UnLockRecordGetXML(const std::string& strUnLockInfo,std::string& strDomainCode,std::string& strNVRCode, std::string& strCameraCode, IVS_RECORD_LOCK_INFO* pRecordLockInfo);

	/******************************************************************
	function : AddBookmarkParseXML
	description: 添加书签生成结果XML；
	input : iRetCode，结果码；uiBookmarkID，生成的书签ID；xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    static IVS_INT32 AddBookmarkParseXML(IVS_UINT32 uiBookmarkID,IVS_INT32 iRetCode,CXml &xml);

    static IVS_INT32 GetCameraPlanInfoParseXML(const IVS_CHAR* pReqXml, IVS_UINT32& uiPlanType);
};
#endif

