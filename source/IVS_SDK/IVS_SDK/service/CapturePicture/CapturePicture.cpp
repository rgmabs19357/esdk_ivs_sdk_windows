#include "CapturePicture.h"
#include "UserMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "CapturePictureXMLProcess.h"
#include "IVS_Trace.h"
#include "ivs_xml.h"

CCapturePicture::CCapturePicture(void)
	: m_pUserMgr(NULL)
{
}


CCapturePicture::~CCapturePicture(void)
{
	m_pUserMgr = NULL;
}

// 设置用户对象类
void CCapturePicture::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr = pUserMgr;
}


// 平台手动抓拍
IVS_INT32 CCapturePicture::PlatformSnapshot(const IVS_CHAR* pCameraCode)const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = CapturePictureXMLProcess::PlatformSnapshotGetGenericXML(pCameraCode,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Platform Snapshot is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR cDevCode[IVS_DEV_CODE_LEN+1];
	IVS_CHAR cDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, cDevCode, cDomaCode);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SNAPSHOT_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	sendNssMsgInfo.SetCameraCode(cDevCode);
	sendNssMsgInfo.SetDomainCode(cDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	BP_RUN_LOG_INF("Platform Snapshot", "ok", "msg have Parsed");
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Platform Snapshot", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}
	return iRet;
}

//设置平台抓拍计划
IVS_INT32 CCapturePicture::SetSnapshotPlan(const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan)const
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pSnapShotPlan, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = CapturePictureXMLProcess::SetSnapshotPlanGetXML(pCameraCode,pSnapShotPlan,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Set Snapshot Plan is failed","failed");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR cDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR cDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(void)CXmlProcess::ParseDevCode(pCameraCode, cDevCode, cDomaCode);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_SNAPSHOT_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	sendNssMsgInfo.SetCameraCode(cDevCode);
	sendNssMsgInfo.SetDomainCode(cDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	BP_RUN_LOG_INF("Set Snapshot Plan", "ok", "msg have Parsed");
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Snapshot Plan", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}
	return iRet;
}

//查询平台抓拍计划
IVS_INT32 CCapturePicture::GetSnapshotPlan(const IVS_CHAR* pCameraCode,IVS_SNAPSHOT_PLAN* pSnapShotPlan)const
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pSnapShotPlan, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = CapturePictureXMLProcess::PlatformSnapshotGetGenericXML(pCameraCode,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Snapshot Plan is failed","failed");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_SNAPSHOT_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Snapshot Plan", "Get Snapshot Plan Success.pResultList size is 0.");
		return iRet;
	}

	// 将xml转化成结构体返回
	CXml xmlRsp;
	if (!xmlRsp.Parse(strpRsp.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse response xml failed","failed");
		return IVS_XML_INVALID;
	}

	if (IVS_SUCCEED != CapturePictureXMLProcess::ParseGetSnapshotPlan(pSnapShotPlan,xmlRsp))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse response xml failed","failed");
		return IVS_XML_INVALID;
	}	

	BP_RUN_LOG_INF("Get Snapshot Plan", "ok", "msg have Parsed");
	return iRet;
}

 // 获取平台抓拍图片列表
IVS_INT32 CCapturePicture::GetSnapshotList(const IVS_CHAR* pCameraCode, const IVS_QUERY_SNAPSHOT_PARAM* pQueryParam, IVS_SNAPSHOT_INFO_LIST* pSnapshotList, IVS_UINT32 uiBufSize) const
{
	CHECK_POINTER(pQueryParam, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pSnapshotList, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1]= {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	CXml xmlReq;
	iRet = CapturePictureXMLProcess::GetSnapshotListGetXML(pCameraCode, pQueryParam, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_SNAPSHOT_LIST_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Snapshot List", "failed");
		return iRet;
	}
	//解析xml;
	if (!xmlReq.Parse(strpRsp.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Set Camera Backup Cfg", "xmlReq.Parse(pReqXml) fail");
		return IVS_XML_INVALID;
	}

	iRet = CapturePictureXMLProcess::GetSnapshotListParseXML(xmlReq, pSnapshotList, uiBufSize, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Platform Snapshot", "SendCmd operation failed");
		return iRet;
	}
	return iRet;
}

// 删除平台抓拍计划
IVS_INT32 CCapturePicture::DelSnapshotPlan(const IVS_CHAR* pCameraCode) const
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	CXml xmlReq;
	iRet = CapturePictureXMLProcess::DelSnapshotPlanGetXml(chDevCode, chDomaCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_SNAPSHOT_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	BP_RUN_LOG_INF("Del Snapshot Plan", "ok", "msg have Parsed");
	return iRet;
}

// 删除平台抓拍图片
IVS_INT32 CCapturePicture::DeleteSnapshot(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID)const
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = CapturePictureXMLProcess::DeleteSnapshotGetXML(pCameraCode, uiPictureID, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Delete Snapshot is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_SNAPSHOT_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Delete Snapshot", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}
	return iRet;
}

