/********************************************************************
	filename	: 	UpgradeMgrXMLProcess.h 
	author		:	wanglei
	created		:	2012/12/03	
	description	:	处理操作日志XML
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __UPGRADE_MGR_XML_PROCESS_H__
#define __UPGRADE_MGR_XML_PROCESS_H__

#include "XmlProcess.h"

class CUpgradeMgrXMLProcess
{
private:
	CUpgradeMgrXMLProcess();
public:
	~CUpgradeMgrXMLProcess();

public:
    static IVS_INT32 GetCUVersionReqXML(const IVS_CU_INFO* pCUInfo, CXml& xml);

    static IVS_INT32 ParseGetCuVersionRspXML(const std::string& strRspXML, IVS_UPGRADE_INFO* pUpgradeInfo);
};

#endif // __UPGRADE_MGR_XML_PROCESS_H__