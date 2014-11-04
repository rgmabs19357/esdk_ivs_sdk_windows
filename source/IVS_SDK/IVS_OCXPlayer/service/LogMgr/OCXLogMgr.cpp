#include "stdafx.h"
#include "OCXLogMgr.h"
#include "SDKDef.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "OCXLogMgrXMLProcess.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"

COCXLogMgr::COCXLogMgr()
{
}

COCXLogMgr::~COCXLogMgr()
{
}

IVS_INT32 COCXLogMgr::GetOperationLog(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString &strResult)
{
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlRet;
	if (!xmlRet.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Operation Log", "xmlRet.Parse is fail");
		return IVS_XML_INVALID;
	}
	IVS_INT32 iNum = COCXXmlProcess::GetFieldNum(xmlRet);
	// 为请求分配内存，解析xml
	IVS_VOID *pBuff = NULL;
	IVS_INT32 iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pBuff,xmlRet,iNum);

	if (IVS_SUCCEED != iRet)
	{	
		// 如果IVS_SUCCEED != iRet, pBuff的内存在GetUnifiedFormatQueryParseXML已经释放

		BP_RUN_LOG_ERR(iRet, "Get Operation Log", "Get Unified Format Query Parse XML is fail");
		return iRet;
	}
	IVS_QUERY_UNIFIED_FORMAT* pQueryUnifiedFormat = reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(pBuff);

	// 为List分配内存，判断FromIndex和ToIndex的合法性
	IVS_UINT32 uiReqNum = ((pQueryUnifiedFormat->stIndex).uiToIndex - (pQueryUnifiedFormat->stIndex).uiFromIndex) + 1;
	IVS_INT32 iReqNum = static_cast<IVS_INT32>(uiReqNum);

	// 判断请求的页数是否小于等于0
	if (0 >= iReqNum)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Operation Log", "0 >= iReqNum");
		IVS_DELETE(pBuff,MUILI);
		return iRet;
	}

	// 为结果LIST分配内存
	IVS_UINT32 uiBufSize = (uiReqNum - 1) * sizeof(IVS_OPERATION_LOG_INFO) + sizeof(IVS_OPERATION_LOG_LIST);

	IVS_VOID* pResOperLogListBuf = IVS_NEW((IVS_CHAR*&)pResOperLogListBuf,uiBufSize);    
	if (NULL == pResOperLogListBuf)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Operation Log", "new Response Operation Log List Buffer failed");
		IVS_DELETE(pBuff,MUILI);
		return iRet;
	}
	memset(pResOperLogListBuf, 0x0, uiBufSize);
	IVS_OPERATION_LOG_LIST* pResOperLogList = reinterpret_cast<IVS_OPERATION_LOG_LIST*>(pResOperLogListBuf);
	// 调用SDK接口
	iRet = IVS_SDK_GetOperationLog(iSessionId,pQueryUnifiedFormat,pResOperLogList,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Operation Log","ivs sdk Get Operation Log failed");
		IVS_DELETE(pResOperLogListBuf,MUILI);	
		IVS_DELETE(pBuff,MUILI);
		return iRet;
	}	

	// 拼装响应消息结构体为XML，
	CXml xmlRes;
	if (IVS_SUCCEED != COCXLogMgrXMLProcess::GetResOperationLogXML(pResOperLogList, xmlRes))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get Operation Log","Get Response Operation Log XML is fail");
		IVS_DELETE(pResOperLogListBuf,MUILI);	
		IVS_DELETE(pBuff,MUILI);
		return IVS_SMU_XML_INVALID;
	}

	IVS_UINT32 xmlLen = 0;
	strResult = xmlRes.GetXMLStream(xmlLen);

	IVS_DELETE(pResOperLogListBuf,MUILI);	
	IVS_DELETE(pBuff,MUILI);
	return iRet;
}
