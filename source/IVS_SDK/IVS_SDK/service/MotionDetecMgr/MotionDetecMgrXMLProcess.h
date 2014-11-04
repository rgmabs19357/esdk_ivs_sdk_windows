/********************************************************************
filename	: 	MotionDetecMgrXMLProcess.h
author		:	z00201790
created		:	2012/12/06	
description	:	解析和拼装移动侦测管理xml的类;
copyright	:	Copyright (C) 2011-2015
history		:	2012/12/06 初始版本;
*********************************************************************/
#ifndef __MOTIONDETECTION_MGR_XML_PROCESS_H__
#define __MOTIONDETECTION_MGR_XML_PROCESS_H__

#include "XmlProcess.h"
#include "SDKDef.h"

class CMotionDetecMgrXMLProcess
{
private:
	CMotionDetecMgrXMLProcess();
public:
	~CMotionDetecMgrXMLProcess();

public:
	/***********************************************************************************
    * function       : GetMotionDetecXML
	* description: 拼装查询运动检测的请求xml
	* input      : pLoginId,登录Id；pCameraCode，摄像机编码；pDomainCode，域编码
	* output     : xml，定义的目标xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetMotionDetecXML(const IVS_CHAR* pLoginId, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, CXml& xml);

	/***********************************************************************************
    * function       : ParseMotionDetecXML
	* description: 解析运动检测的响应消息xml
	* input      : xml,响应消息xml
	* output     : pMotionDetection，运动检测结构体
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 ParseMotionDetecXML(CXml& xml, IVS_MOTION_DETECTION* pMotionDetection);

	/***********************************************************************************
    * function       : GetMotionDetecAreaInfoNum
	* description: 计算运动检测的响应消息中AreaInfo的条数
	* input      : xml,响应消息xml
	* output     : iNum，AreaInfo的条数
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum);

	/***********************************************************************************
    * function       : SetMotionDetecGetXML
	* description: 拼装修改运动检测的请求xml
	* input      : pLoginId,登录Id；pCameraCode，摄像机编码；pDomainCode，域编码
	               pMotionDetection, 运动检测结构体
	* output     : xml，定义的目标xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 SetMotionDetecGetXML(const IVS_CHAR* pLoginId, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, const IVS_MOTION_DETECTION* pMotionDetection, CXml& xml);
	
	/***********************************************************************************
    * function   : GetMotionRangeDataPlatformReqXML
	* description: 拼装查询移动侦测幅度数据的请求xml
	* input      : pCameraCode，摄像机编码；pDomainCode，域编码
	               pTimeSpan, 时间间隔结构体；pIndexRange，分页信息结构体
	* output     : xml，定义的目标xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetMotionRangeDataPlatformReqXML(const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode,const IVS_TIME_SPAN* pTimeSpan,
		const IVS_INDEX_RANGE* pIndexRange,CXml& xml);

	/***********************************************************************************
    * function   : ParseGetMotionRangeDataPlatformRspXML
	* description: 解析查询移动侦测幅度数据的响应xml
	* input      : uiBufferSize:响应结果分配空间大小；
				   RspXml，定义的目标xml
	* output     : pMotionRangeData，移动侦测幅度数据的响应信息结构体
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 ParseGetMotionRangeDataPlatformRspXML(CXml &RspXml, IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_UINT32 uiBufferSize);
	
};

#endif //__MOTIONDETECTION_MGR_XML_PROCESS_H__
