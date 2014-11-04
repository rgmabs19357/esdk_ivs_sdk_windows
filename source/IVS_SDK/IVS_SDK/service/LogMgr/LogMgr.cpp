#include "LogMgr.h"
#include "NSSOperator.h"
#include "nss_mt.h"
#include "bp_log.h"
#include "UserMgr.h"
#include "ivs_xml.h"
#include "XmlProcess.h"
#include "IVS_Trace.h"

#include "LogMgrXMLProcess.h"
CLogMgr::CLogMgr(void):m_pUserMgr(NULL)
{
	
}


CLogMgr::~CLogMgr(void)
{
	m_pUserMgr = NULL;
}

// 设置用户对象类
void CLogMgr::SetUserMgr(void *pUserMgr)
{
	m_pUserMgr = (CUserMgr *)pUserMgr;
}

// 查询操作日志
IVS_INT32 CLogMgr::GetOperationLog(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_OPERATION_LOG_LIST* pResOperLogList,const IVS_UINT32& uiBufSize) const
{	
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUnifiedQuery,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResOperLogList,IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");
// 把结构体pUnifiedQuery和iLoginID组装到xmlReq中
	CXml xmlReq;
	IVS_INT32 iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Operation Log", "Get Unified Format Query XML is fail");
		//modify by zwx211831 更改错误返回码
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
	CHECK_POINTER(pReq,IVS_OPERATE_MEMORY_ERROR);
	// 摄像机编码捞出来，支持多域
	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	for (int i = 0; i < pUnifiedQuery->iFieldNum; i++)
	{
		if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID || QUERY_OPERATION_OBJECT_CODE == pUnifiedQuery->stQueryField[i].eFieID)
		{
			(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, szCameraCode, szDomainCode);
		}
	}

	// 组装命令
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_OPERATION_LOG_REQ);
	sendNssMsgInfo.SetReqData(pReq);
	sendNssMsgInfo.SetCameraCode(szCameraCode);
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Motion Range Data Platform", "SendCmd operation succeed,rspCode = %d", iRet);
		return iRet;
	}

    CXml xmlRsp;
    if (!xmlRsp.Parse(strpRsp.c_str()))
    {
        BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get Operation Log", "xmlRsp.Parse(pRsp) is fail");
        return IVS_SMU_XML_INVALID;
    }
    // 解析XML响应消息为结构体
    iRet = CLogMgrXMLProcess::PraseResOperationLogXML(xmlRsp,pResOperLogList,uiBufSize);	
	
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet, "Get Operation Log", "Prase Response Operation Log XML is fail");
		return iRet;
	}
	return iRet;
}


//扩展查询操作日志
IVS_INT32 CLogMgr::GetOperationLogEx(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
	const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
	IVS_OPERATION_LOG_LIST* pResOperLogList, 
	const IVS_UINT32 uiBufSize) const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUnifiedQuery,IVS_PARA_INVALID);
	CHECK_POINTER(pOptLogQueryEx, IVS_PARA_INVALID);
	CHECK_POINTER(pResOperLogList, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("GetOperationLogEx NVRCode: %s, Module Name: %s", pOptLogQueryEx->cNVRCode, pOptLogQueryEx->cModuleName);

	IVS_INT32 iRet = IVS_FAIL;

	//检测输出缓存大小是否正确
	if ( (sizeof(IVS_OPERATION_LOG_LIST) + (pUnifiedQuery->stIndex.uiToIndex - pUnifiedQuery->stIndex.uiFromIndex) * sizeof(IVS_OPERATION_LOG_INFO) )
		!= uiBufSize )
	{
		iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet, "Get Cluster Status", "Reason:Buffer Size is Error");
		return iRet;
	}

	// 把结构体pUnifiedQuery和iLoginID组装到xmlReq中
	CXml xmlReq;
	iRet = CLogMgrXMLProcess::GetResOperationLogExXML(pUnifiedQuery, pOptLogQueryEx, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Operation Log", "Get Unified Format Query XML is fail");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	//获取本域OMU连接，OMU暂不支持跨域操作
	std::string strOMULinkID;
	iRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strOMULinkID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Extending OperationLog", "Reason:Get Local Domain OMU LinkID");
		return iRet;
	}
	
	// 构造nss协议头，
	CCmd *pCmd = CNSSOperator::instance().BuildOMUCmd(NSS_GET_OPERATION_LOG_REQ, pReq, strOMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 发送nss消息
	CCmd *pCmdRsp = NULL;
	pCmdRsp =	CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);

	// 解析Cmd To Xml
	iRet = IVS_SUCCEED;
	char* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iRet);     
	HW_DELETE(pCmdRsp);
	CHECK_POINTER(pRsp, iRet);

	// 解析XML响应消息
	CXml xmlRsp;
	if (!xmlRsp.Parse(pRsp))
	{
		BP_RUN_LOG_ERR(IVS_XML_PACK_ERROR, "Prase xml fail", "NA");
		IVS_DELETE(pRsp, MUILI);
		return IVS_XML_PACK_ERROR;
	}

	iRet =CLogMgrXMLProcess::ParseRspOperationLogExXML(xmlRsp, pResOperLogList, uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Extending OperationLog", "Reason:Parse respond Xml failed");
		IVS_DELETE(pRsp, MUILI);
		return iRet;
	}

	IVS_DELETE(pRsp, MUILI);
	return iRet;
}
