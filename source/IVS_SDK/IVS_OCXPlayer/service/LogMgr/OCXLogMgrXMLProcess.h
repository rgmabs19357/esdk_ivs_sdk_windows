/********************************************************************
	filename	: 	OCXLogMgrXMLProcess.h
	author		:	wulixia
	created		:	2012/12/03	
	description	:	处理告警XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __OCX_LOG_MGR_XML_PROCESS_H__
#define __OCX_LOG_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"

class COCXLogMgrXMLProcess
{
private:
	COCXLogMgrXMLProcess();
public:
	~COCXLogMgrXMLProcess();

public:	

	/**************************************************************************
	* name       : GetResOperationLogXML()
	* description: 拼装响应消息的xml
	* input      : pQueryUnifiedFormat        申请的内存结构体	
	*              iFieNum    查询字段的个数
	*              pUserData		 用户数据;
	* output     : xmlReq
	* return     : 成功返回0 ，错误返回错误码
	* remark     : NA
	**************************************************************************/
	static IVS_INT32 GetResOperationLogXML(IVS_OPERATION_LOG_LIST* pOperationLogList, CXml& xmlRes);
};

#endif //__OCX_LOG_MGR_XML_PROCESS_H__
