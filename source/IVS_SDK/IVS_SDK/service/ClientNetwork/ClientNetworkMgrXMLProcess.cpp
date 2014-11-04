#include "ClientNetworkMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"


CClientNetworkMgrXMLProcess::CClientNetworkMgrXMLProcess(void)
{
}

CClientNetworkMgrXMLProcess::~CClientNetworkMgrXMLProcess(void)
{
}

// 客户端查询CPU/内存性能拼接xml
IVS_INT32 CClientNetworkMgrXMLProcess::NodeCodeGetXML(const IVS_CHAR* pNodeCode, CXml& xml)
{
	CHECK_POINTER(pNodeCode, IVS_SDK_RET_INIT_RTSPCLIENT_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("NodeCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pNodeCode);
	xml.OutOfElem();

	return IVS_SUCCEED;
}
