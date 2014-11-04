#include "MotionDetection.h"
#include "NSSOperator.h"
#include "nss_mt.h"
#include "bp_log.h"
#include "UserMgr.h"
#include "ivs_xml.h"
#include "XmlProcess.h"
#include "MotionDetecMgrXMLProcess.h"
#include "IVS_Trace.h"


CMotionDetection::CMotionDetection(void)
	:m_pUserMgr(NULL)
{
}


CMotionDetection::~CMotionDetection(void)
{
	m_pUserMgr = NULL;
}


void CMotionDetection::SetUserMgr(void *pUserMgr)
{
	m_pUserMgr = (CUserMgr *)pUserMgr;
}


// 查询平台运动检测幅度数据
IVS_INT32 CMotionDetection::GetMotionRangeDataPlatform(const IVS_CHAR* pCameraCode,const IVS_TIME_SPAN* pTimeSpan,const IVS_INDEX_RANGE* pIndexRange,IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTimeSpan, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pMotionRangeData, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR szCameraCode[IVS_SDK_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	// 解析pCameraCode到摄像机编码和域编码中
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Motion Range Data Platform", "Parse Device Code failed");
		return iRet;
	}
	// 拼装请求xml
	CXml xmlReq;
	iRet = CMotionDetecMgrXMLProcess::GetMotionRangeDataPlatformReqXML(szCameraCode,szDomainCode,pTimeSpan,pIndexRange, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Motion Range Data Platform", "Get Motion Range Data Platform Req XML failed");
		return iRet;
	}

	std::string sRecordMethod = "RecordMethod";
	iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(sRecordMethod.c_str(),CToolsHelp::Int2Str(RECORD_METHOD_PLATFORM).c_str(), xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_MD_DATA_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);
	sendNssMsgInfo.SetCameraCode(szCameraCode);
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED == iNeedRedirect)
	{
		iRet = m_pUserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
	}
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Motion Range Data Platform", "SendCmd operation succeed,rspCode = %d", iRet);
		return iRet;
	}

	//解析数据
	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());
	
	//解析查询请求XML
	if (IVS_SUCCEED != CMotionDetecMgrXMLProcess::ParseGetMotionRangeDataPlatformRspXML(xmlRsp,pMotionRangeData,uiBufferSize))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Get Motion Range Data Platform Rsp XML is failed","failed");
		return IVS_XML_INVALID;
	}	
	return iRet;
}


