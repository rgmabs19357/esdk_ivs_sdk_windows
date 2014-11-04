/********************************************************************
filename : ClientNetworkMgrXMLProcess.h
author : pkf57481
created : 2013/01/22
description : 客户端网管
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/22 初始版本
*********************************************************************/


#ifndef __CLIENTNETWORK_MGR_XML_PROCESS_H__
#define __CLIENTNETWORK_MGR_XML_PROCESS_H__

#include "ToolsHelp.h"
#include "XmlProcess.h"
#include <string.h>
#include "QueryFiled.h"
#include "hwsdk.h"

class CClientNetworkMgrXMLProcess
{
private:
    CClientNetworkMgrXMLProcess();
public:
    ~CClientNetworkMgrXMLProcess();

public:
	/***********************************************************************************
    * name       : GetCPUMemoryGetXML
    * description: 客户端查询CPU/内存性能拼接xml
    * input      : pNodeCode:网元编码; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 NodeCodeGetXML(const IVS_CHAR* pNodeCode, CXml& xml);

};

#endif

