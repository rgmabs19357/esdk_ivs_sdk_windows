/********************************************************************
	filename	: 	OCXUpgradeMgrXMLProcess.h
	author		:	wanglei 00165153
	created		:	2013/11/13	
	description	:	处理告警XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/11/13 初始版本
*********************************************************************/

#ifndef __OCX_UPGRADE_MGR_XML_PROCESS_H__
#define __OCX_UPGRADE_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"

class COCXUpgradeMgrXMLProcess
{
private:
    COCXUpgradeMgrXMLProcess();
    ~COCXUpgradeMgrXMLProcess();

public:	
    static IVS_INT32 GetCUVersionParseReqXML(CXml& xmlReq,IVS_CU_INFO& stCuInfo);
    static IVS_INT32 GetCUVersionRspXML(IVS_UPGRADE_INFO& stUpgradeInfo,IVS_INT32 iRetCord,CXml& xmlRsp);
};

#endif