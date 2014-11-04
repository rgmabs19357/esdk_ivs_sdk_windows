/********************************************************************
	filename	: 	OCXRealPlayXMLProcess.h
	author		:	songjianfeng(00193168)
	created		:	2012/12/03	
	description	:	处理媒体视频回放的XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/
#ifndef __OCX_REALPLAY_XML_PROCESS_H__ 
#define __OCX_REALPLAY_XML_PROCESS_H__

#include "OCXXmlProcess.h"
#include "OCXDatatype.h"
#include "SDKDef.h"

class COCXRealPlayXMLProcess
{
private:
	COCXRealPlayXMLProcess();
public:
	~COCXRealPlayXMLProcess();

public:
	/******************************************************************
	function : GetMediaParamXML
	description: 获取媒体参数信息;
	input : pMediaParamXml,媒体参数信息XML；mediaParam，媒体参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static LONG GetMediaParamXML(LPCTSTR pRealPlayParamXml, IVS_REALPLAY_PARAM &realPlayParam);

    /******************************************************************
	function : EnableGPUParseClientConfig
	description: 获取CU配置文件中的媒体参数;
	input : pMediaParamXml,媒体参数信息XML；mediaParam，媒体参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static LONG EnableGPUParseClientConfig(LPCTSTR pParamXml, BOOL& bEnableGPU);


	static LONG BuildAsyncRealPlayXml(CXml &xml, long winID, long result, unsigned long transid);



};
#endif

