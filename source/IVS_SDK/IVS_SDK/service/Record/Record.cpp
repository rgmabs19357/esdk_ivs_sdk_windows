#include "Record.h"
#include "UserMgr.h"
#include "bp_log.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "RecordXMLProcess.h"
#include "IVS_Trace.h"

CRecord::CRecord(void)
{
	m_ulPort = 0;
	m_iPlayHandle = 0;
	m_RecordStatus = RECORD_IDLE;
	m_pUserMgr = NULL;
}


CRecord::~CRecord(void)
{
	m_pUserMgr = NULL;
}

void CRecord::Release()
{
	SetState(RECORD_IDLE);
}


int CRecord::GetPlayHandle()const
{
	return m_iPlayHandle;
}

void CRecord::SetPlayHandle(int iPlayHandle)
{
	m_iPlayHandle = iPlayHandle;
}


RECORD_STATUS CRecord::GetState()const
{
	return m_RecordStatus;
}

void CRecord::SetState(RECORD_STATUS RecordStatus)
{
	m_RecordStatus = RecordStatus;
}

void CRecord::SetUserMgr(void *pUserMgr)
{
	m_pUserMgr = (CUserMgr *)pUserMgr;
}

unsigned long CRecord::GetPort()const
{
	return m_ulPort;
}

//处理MP的流异常回调
void CRecord::DealException(IVS_INT32 /*iPort*/, IVS_INT32 /*iMsgType*/, const void* pHandle)
{
	CHECK_POINTER_VOID(pHandle);

	//关闭本地录像
	//(void)StopLocalRecordEx((unsigned int)iPort);

	if (NULL == m_pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Deal Exception","user obj is null");
		return;
	}

	EventCallBack fnCallBack = m_pUserMgr->GetEventCallBack();
	if (NULL == fnCallBack)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Deal Exception","fnCallBack obj is null");
		return;
	}

	//SOURCE_EXCEPTION_DATA* pExceptionData = (SOURCE_EXCEPTION_DATA*)pHandle;
	//if (NULL == pExceptionData)
	//{
	//	//IVS_LOG(LOG_ERR, "deal exception error, exception data is null");
	//	return;
	//}

	STREAM_EXCEPTION_INFO stuExceptionInfo;
	memset(&stuExceptionInfo, 0, sizeof(STREAM_EXCEPTION_INFO));
	stuExceptionInfo.iSessionID = m_pUserMgr->GetSessionID();
	stuExceptionInfo.ulHandle = static_cast<IVS_UINT32>(m_iPlayHandle);
	//stuExceptionInfo.iExceptionCode = (int)pExceptionData->iExceptionCode;
	fnCallBack(IVS_EVENT_RECORD_FAILED, &stuExceptionInfo, sizeof(STREAM_EXCEPTION_INFO), m_pUserMgr->GetEventUserData());

}

//设置录像空间
IVS_INT32 CRecord::SetRecordSpace(const IVS_CHAR* pCameraCode, const IVS_RECORD_POLICY_TIME* pRecordSpace) const
{
	CHECK_POINTER(pRecordSpace,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	IVS_INT32 iRet = IVS_FAIL;
	iRet = CRecordXMLProcess::AddRecordSpaceGetXML(pRecordSpace,pCameraCode,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Set Record Space","failed");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);
	CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_STORE_SPACE_REQ, pReqSpace);
	CHECK_POINTER(pCmd,IVS_SDK_RET_BP_CMD_REQ_INVALID);
	//发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	//const IVS_CHAR* pRsp转换为XML并转换为数据结构
	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);
	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}//lint !e818
//删除录像空间
IVS_INT32 CRecord::DeleteRecordSpace(const IVS_CHAR* pCameraCode, IVS_INT32 /*iRecordMethod*/) const
{
	//数据结构拼装XML并转换为const IVS_CHAR *
	//IVS_INT32 iLoginID = 0;
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	//CRecordXMLProcess::GetRecordSpaceGetXML(iLoginID, pCameraCode,iRecordMethod,xmlReq);

	CXml xmlReq;
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);
	CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_DEL_STORE_SPACE_REQ, pReqSpace);

	CHECK_POINTER(pCmd,IVS_SDK_RET_BP_CMD_REQ_INVALID);
	//发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);

	//const IVS_CHAR* pRsp转换为XML并转换为数据结构
	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);
	IVS_INT32 iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}

//开始平台录像
IVS_INT32 CRecord::StartPlatformRecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime) const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	return StartRemoteRecord(RECORD_TYPE_PLAT,pCameraCode,iRecordTime);
}

//停止平台录像
IVS_INT32 CRecord::StopPlatformRecord(const IVS_CHAR* pCameraCode) const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	return StopRemoteRecord(RECORD_TYPE_PLAT,pCameraCode);
}

//开始前端录像
IVS_INT32 CRecord::StartPURecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime) const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	return StartRemoteRecord(RECORD_TYPE_PU,pCameraCode,iRecordTime);
}

//停止前端录像
IVS_INT32 CRecord::StopPURecord(const IVS_CHAR* pCameraCode) const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	return StopRemoteRecord(RECORD_TYPE_PU,pCameraCode);
}

//开始远程录像回放
IVS_INT32 CRecord::StartRemoteRecord(RECORD_TYPE eRecordType,const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime)const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	//拼装XML查询请求
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	std::string strUserDomainCode;
	m_pUserMgr->GetDomainCode(strUserDomainCode);
	iRet = CRecordXMLProcess::StartRemoteRecordReqXML(m_pUserMgr->GetUserID(),strUserDomainCode.c_str(),pCameraCode,
		static_cast<IVS_UINT32>(eRecordType),static_cast<IVS_UINT32>(iRecordTime),xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start Remote Record Req XML","StartRemoteRecordReqXML is faile");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pStatusReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pStatusReq, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_START_RECORD_REQ);
	sendNssMsgInfo.SetReqData(pStatusReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}

//停止远程录像回放
IVS_INT32 CRecord::StopRemoteRecord(RECORD_TYPE eRecordType,const IVS_CHAR* pCameraCode)const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	//拼装XML查询请求
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	std::string strUserDomainCode;
	m_pUserMgr->GetDomainCode(strUserDomainCode);
	iRet = CRecordXMLProcess::StopRemoteRecordReqXML(m_pUserMgr->GetUserID(),strUserDomainCode.c_str(),pCameraCode,
		static_cast<IVS_UINT32>(eRecordType),xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start Remote Record Req XML","StartRemoteRecordReqXML is faile");
		return iRet;
	}

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	unsigned int xmlLen = 0;
	const IVS_CHAR * pStatusReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pStatusReq, IVS_OPERATE_MEMORY_ERROR);
	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_STOP_RECORD_REQ);
	sendNssMsgInfo.SetReqData(pStatusReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}
