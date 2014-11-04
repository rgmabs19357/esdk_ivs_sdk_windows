#include "OCXIntelligenceAnalysisMgr.h"
#include "stdafx.h"
#include "SDKDef.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "IVS_OCXPlayerCtrl.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"

COcxIntelligenceAnalysisMgr::COcxIntelligenceAnalysisMgr(void)
{
    
}

COcxIntelligenceAnalysisMgr::~COcxIntelligenceAnalysisMgr(void)
{
    
}

// 构建返回消息
void COcxIntelligenceAnalysisMgr::RetToCUMsg(CString &strResult, IVS_INT32 iRet, IVS_CHAR* pRspXml)
{
	IVS_DEBUG_TRACE("iRet: %d", iRet);

	CXml xmlRsp;
   
    if ((IVS_SUCCEED != iRet) || (NULL == pRspXml))
    {
		(void)xmlRsp.AddDeclaration("1.0","UTF-8","");
		(void)xmlRsp.AddElem("MESSAGE");
		(void)xmlRsp.AddChildElem("CV_CONTENT");
		(void)xmlRsp.IntoElem();
		(void)xmlRsp.AddChildElem("RESULT_CODE");
		(void)xmlRsp.IntoElem();
		(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
		xmlRsp.OutOfElem();
		xmlRsp.OutOfElem();
		IVS_UINT32 uiLen = 0;
		strResult = xmlRsp.GetXMLStream(uiLen);
    } else {
        strResult = pRspXml;
    }
    IVS_SDK_ReleaseBuffer(pRspXml);

    return;
}


// 智能分析统一透传接口
void COcxIntelligenceAnalysisMgr::IntelligenceAnalysis(CString &strResult, IVS_INT32 iSessionID, ULONG ulInterfaceType, const IVS_CHAR* pReqXml)
{
//	***********pReqXml可以为空*********************
//  CHECK_POINTER(pReqXml, NULL);
	IVS_DEBUG_TRACE("SessionID: %d, InterfaceType: %lu", iSessionID, ulInterfaceType);

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_IA(iSessionID, ulInterfaceType, pReqXml, &pRspXml);
	RetToCUMsg(strResult, iRet, pRspXml);
}


