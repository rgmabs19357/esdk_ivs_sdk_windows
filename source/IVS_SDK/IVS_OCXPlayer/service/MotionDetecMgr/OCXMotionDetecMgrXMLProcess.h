/********************************************************************
	filename	: 	OCXMotionDetecMgrXMLProcess.h
	author		:	wWX155351
	created		:	2012/12/03	
	description	:	拼装和解析移动侦测的xml
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __OCX_MOTIONDETECTION_MGR_XML_PROCESS_H__
#define __OCX_MOTIONDETECTION_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"

class COCXMotionDetecMgrXMLProcess
{
public: 
	COCXMotionDetecMgrXMLProcess();
	~COCXMotionDetecMgrXMLProcess();

public:
	//解析平台移动检测xml
	static IVS_INT32 ParseSetMotionDetecXml(IVS_MOTION_DETECTION* pMotionDetection, CXml& xml);

	/*****************************************************************
	     function   : GetMotionDetecAreaInfoNum
	     description: 获取运动检测的响应消息中AreaInfo的条数
	     input      : CXml & xml   响应消息xml
	     output     : IVS_INT32 & iNum  AreaInfo的条数
	     return     : AreaInfo的条数
	*****************************************************************/
	static IVS_INT32 GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum);

	// 拼装平台移动检测响应消息xml
	static IVS_INT32 SetMotionDetecGetXML(const IVS_MOTION_DETECTION* pMotionDetec, CXml& xml);

	/*****************************************************************
	     function   : ParseGetMotionRangeDataPlatformXml
	     description: 解析查询平台运动检测幅度数据xml
	     input      : pStartTime：查询开始时间；
		              pEndTime：查询截止时间；
					  pMotionDetection：查询平台运动检测幅度数据结构体；
					  iRet：结果码；
	     output     : CXml & xml   响应消息xml
	     return     : 结果码
	*****************************************************************/
	static IVS_INT32 ParseGetMotionRangeDataPlatformXml(const IVS_CHAR* pStartTime,const IVS_CHAR* pEndTime,
		const IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_INT32 iRetCode, CXml& xml);
};
#endif // __OCX_MOTIONDETECTION_MGR_XML_PROCESS_H__
