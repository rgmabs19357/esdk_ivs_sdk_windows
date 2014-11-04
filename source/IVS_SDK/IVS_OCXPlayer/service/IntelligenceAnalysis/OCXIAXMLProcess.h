/********************************************************************
filename 	: IAXMLProcess.h
author   	: z00227377
created 	: 2012/12/22
description : 轨迹推送相关XML
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/22 初始版本
*********************************************************************/


#ifndef __OCX_IA_XML_PROCESS_H__
#define __OCX_IA_XML_PROCESS_H__

#include "ivs_xml.h"
#include "hwsdk.h"
#include "IVS_IATrackAPI.h"
#include "GlobalIAMessageMgr.h"



class CIAXMLProcess
{
private:
	CIAXMLProcess();
public:
	~CIAXMLProcess();

public:

	// 构建智能分析请求轨迹报文
	static IVS_INT32 GetAnalyzeResultBuildXML(const IA_REQ_PARAM &IAReqParam, CXml &ReqXML);

	// 解析智能分析请求轨迹报文
	static IVS_INT32 GetAnalyzeResultPraseXML(LPCTSTR pRspXML, IVS_ULONG &ulResultCode, IA_REQ_PARAM &IAReqParam);

	// 解析智能分析轨迹推送报文
	static IVS_INT32 TrackInfoContentPraseXML(LPCTSTR pTrackXml, IA_LAYOUT_MESSAGE &pIALayoutMessage, IA_REQ_PARAM &IAReqParameters);
	
	// 构建停止智能分析请求轨迹报文
	static IVS_INT32 StopAnalyzeResultBuildXML(IVS_ULONG ulRequestID, IVS_ULONG ulRuleType, IVS_ULONG ulQueryType, const IVS_CHAR *CameraID, CXml &ReqXML);
	
	// 解析停止智能分析请求轨迹报文
	static IVS_INT32 StopAnalyzeResultPraseXML(LPCTSTR pRspXML, IVS_ULONG &ulResultCode);

private:

	// 解析分辨率长宽字段
	static IVS_INT32 PraseFrameSize(char *tempBuf, IA_LAYOUT_MESSAGE &pIALayoutMessage);

	// 解析颜色字段
	static IVS_INT32 PraseColor(char *tempBuf, COLORREF &ulColor);

	// 解析点坐标字段
	static IVS_INT32 PrasePoint(char *tempBuf, POINT &point);

	// 解析点列表字段
	static IVS_INT32 PrasePointList(char *tempBuf, POINT *PointList, ULONG &ulListPointsNum);

};


#endif // __OCX_IA_XML_PROCESS_H__
