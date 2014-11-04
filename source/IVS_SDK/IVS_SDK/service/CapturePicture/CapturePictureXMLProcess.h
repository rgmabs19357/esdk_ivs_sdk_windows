/********************************************************************
	filename	: 	CapturePictureXMLProcess.h
	author		:	s00193168
	created		:	2013/1/24
	description	:	定义抓拍XML处理类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2013/1/24 初始版本;
*********************************************************************/

#ifndef __CAPTUREPICTURE_XML_PROCESS_H__
#define __CAPTUREPICTURE_XML_PROCESS_H__

#include "XmlProcess.h"

class CapturePictureXMLProcess
{
public:
	/***********************************************************************************
    * function: PlatformSnapshotGetGenericXML
	* description: 拼装平台手动抓拍请求的xml
	* input      : pLoginID 登录ID
	               pCameraCode 摄像机编码
	* output     : xml  平台手动抓拍的xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 PlatformSnapshotGetGenericXML(const IVS_CHAR* pCameraCode,CXml &xml);

	/***********************************************************************************
    * function: SetSnapshotPlanGetXML
	* description: 拼装设置平台抓拍计划请求的xml
	* input      : pCameraCode 摄像机编码
				   pSnapShotPlan 抓拍计划信息
	* output     : xml  平台手动抓拍的xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 SetSnapshotPlanGetXML(const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml &xml);

	/********************************************************************
	* name       : ParseGetSnapshotPlan
	* description: 解析查询平台抓拍计划响应的xml
	* input      : pSnapShotPlan：平台抓拍计划信息结构体； xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ParseGetSnapshotPlan(IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml& xml);


	/*****************************************************************
	     function   : GetSnapshotListGetXML
	     description: 拼装抓拍图片查询参数xml
	     input      : const IVS_QUERY_SNAPSHOT_PARAM * pQueryParam  抓拍图片查询参数
	     output     : CXml & xml    拼装抓拍图片查询参数的xml
	     return     : 成功返回0，失败返回错误码;
	*****************************************************************/
	static IVS_INT32 GetSnapshotListGetXML(const IVS_CHAR* pCameraCode,const IVS_QUERY_SNAPSHOT_PARAM* pQueryParam, CXml &xml);

	/*****************************************************************
	     function   : GetSnapshotListParseXML
	     description: 解析抓拍图片列表信息xml
	     input      : CXml & xml   抓拍图片列表信息的xml
	     output     : IVS_SNAPSHOT_INFO_LIST * pSnapshotList   抓拍图片列表信息
	     output     : IVS_UINT32 uiBufSize  缓存大小
	     return     : 成功返回0，失败返回错误码;
	*****************************************************************/
	static IVS_INT32 GetSnapshotListParseXML(CXml& xml, IVS_SNAPSHOT_INFO_LIST* pSnapshotList, IVS_UINT32 uiBufSize, const IVS_CHAR* pDomainCode);

	/*****************************************************************
	     function   : DelSnapshotPlanGetXml
	     description: 删除录像计划xml
	     input      : const IVS_CHAR * pCameraCode
	                  const IVS_CHAR * pDomainCode
	     output     : CXml & xml   删除录像计划xml
	     return     :  成功返回0，失败返回错误码;
	*****************************************************************/
	static IVS_INT32 DelSnapshotPlanGetXml(const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode,CXml& xml);


	/*****************************************************************
	     function   : DeleteSnapshotGetXML
	     description: 拼装删除平台抓拍图片请求的xml
	     input      : const IVS_CHAR * pCameraCode  摄像机编码
	     input      : IVS_UINT32 uiPictureID        图片ID
	     output     : CXml & xml                    删除平台抓拍图片xml
	     return     : 成功返回0，失败返回错误码;
	*****************************************************************/
	static IVS_INT32 DeleteSnapshotGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID, CXml &xml);
};
#endif 
