/********************************************************************
	filename	: 	OCXCapturePictureXMLProcess.h
	author		:	songjianfeng(00193168)
	created		:	2013/1/23	
	description	:	处理图片抓拍的XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/1/23 初始版本
*********************************************************************/
#ifndef __OCX_CAPTUREPICTURE_XML_PROCESS_H__ 
#define __OCX_CAPTUREPICTURE_XML_PROCESS_H__

#include "OCXDatatype.h"
#include "SDKDef.h"

class OCXCapturePictureXMLProcess
{
private:
	OCXCapturePictureXMLProcess();
public:
	~OCXCapturePictureXMLProcess();

public:


	/******************************************************************
	function : SetSnapshotPlanGetXML
	description: 设置平台抓拍计划:将请求XML转换为结构体;
	input : pSnapShotPlan,抓拍计划信息;pCameraCode，摄像机编号;xml，拼装的请求xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 SetSnapshotPlanGetXML(IVS_CHAR* pCameraCode,IVS_SNAPSHOT_PLAN* pSnapShotPlan, CXml &xml);

	/******************************************************************
	function : GetSnapshotPlanGetXML
	description: 查询平台抓拍计划:将响应结构体转换为结果XML;
	input : pCameraCode,摄像机编号；pSnapShotPlan，响应抓拍计划信息结构体；iRetCode，结果码；xml，响应xml
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 GetSnapshotPlanGetXML(IVS_INT32 iRet,const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml &xml);
	
	/*****************************************************************
	     function   : GetSnapshotListParseXml
	     description: 获取通道号
	     output     : CXml & xml    抓拍图片查询参数xml
	     output     : IVS_QUERY_SNAPSHOT_PARAM * pQueryParam  抓拍图片查询参数
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 GetSnapshotListParseXml(CXml &xml, IVS_QUERY_SNAPSHOT_PARAM *pQueryParam, const IVS_CHAR* pCameraCode);

	/*****************************************************************
	     function   : GetSnapshotListGetXml
	     description: 拼装抓拍图片告警信息xml
	     output     : CXml & xml       抓拍图片列表信息xml
	     output     : const IVS_SNAPSHOT_INFO_LIST * pSnapshotList   抓拍图片列表信息
	     input      : const IVS_CHAR * pCameraCode  摄像机编码
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 GetSnapshotListGetXml(CXml &xml, const IVS_SNAPSHOT_INFO_LIST *pSnapshotList, IVS_UINT32 iTotalNum);
	
	/*****************************************************************
	     function   : LocalCaptureParseClientConfig
	     description: 解析本地抓拍参数客户端配置xml
	     input      : LPCTSTR					pXmlStr				
	     output     : IVS_LOCAL_CAPTURE_PARAM	&LocalCaptuParam   本地抓拍参数
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 LocalCaptureParseClientConfig(LPCTSTR pXmlStr, IVS_LOCAL_CAPTURE_PARAM &LocalCaptuParam);
	
	/*****************************************************************
	     function   : LocalCaptureBuildXMLGeneric
	     description: 拼装本地抓拍参数客户端配置xml
	     input      : LPCTSTR					pXmlStr				
	     output     : IVS_LOCAL_CAPTURE_PARAM	&LocalCaptuParam   本地抓拍参数
	     return     : 成功-0 失败-错误码 
	*****************************************************************/
	static IVS_INT32 LocalCaptureBuildXMLGeneric(LONG iEventCode, ULONG iWndID, const IVS_CHAR* pFileName, CXml &xml);

};
#endif

