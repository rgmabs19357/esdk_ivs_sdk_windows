#include "AlarmLevelMgr.h"
#include "AlarmMgrXMLProcess.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "UserMgr.h"
#include "bp_log.h"
#include "IVS_Trace.h"
CAlarmLevelMgr::CAlarmLevelMgr(void)
	:m_AlarmLevelMgr(NULL)
	,m_UserMgr(NULL)//lint !e831
{
}
CAlarmLevelMgr::~CAlarmLevelMgr(void)//lint !e830
{
	m_AlarmLevelMgr = NULL;
	m_UserMgr = NULL;
}

void CAlarmLevelMgr::SetUserMgr( CUserMgr *pUserMgr)
{
	m_UserMgr = pUserMgr;
}

// 新增告警级别
IVS_INT32 CAlarmLevelMgr::AddAlarmLevel(const IVS_ALARM_LEVEL *pAlarmLevel,IVS_UINT32* pAlarmLevelID) const
{
	CHECK_POINTER(pAlarmLevel, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmLevelID,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_UserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	// 数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;
	
	IVS_INT32 iReqType = 0;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::AlarmLevelGetXML(pAlarmLevel, iReqType, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmLevel GetXML fail", "NA");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * aAlarmLevel = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(aAlarmLevel, IVS_XML_PACK_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_ADD_ALARM_LEVEL_REQ, aAlarmLevel,strSMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);

	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);
	const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	if (NULL == pRsp)
	{
		BP_RUN_LOG_ERR(iRet,  "pRsp null", "NA");
		HW_DELETE(pCmdRsp);
		*pAlarmLevelID = 0;
		return iRet;
	}
	   
	// 解析XML响应消息为结构体
	CXml xmlRsp;
	if (!xmlRsp.Parse(pRsp))
	{
		BP_RUN_LOG_ERR(IVS_XML_PACK_ERROR, "Prase xml fail", "NA");
		HW_DELETE(pCmdRsp);
		IVS_DELETE(pRsp, MUILI);
		return IVS_XML_PACK_ERROR;
	}
	
	iRet = CAlarmMgrXMLProcess::AddAlarmLevelPraseXML(pAlarmLevelID, xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddAlarmLevel PraseXML fail", "NA");
		HW_DELETE(pCmdRsp);
		IVS_DELETE(pRsp, MUILI);
		return iRet;

	}
	HW_DELETE(pCmdRsp);
	IVS_DELETE(pRsp, MUILI);
	return iRet;
}

// 修改告警级别
IVS_INT32 CAlarmLevelMgr::ModifyAlarmLevel(const IVS_ALARM_LEVEL *pAlarmLevel) const
{  
	CHECK_POINTER(pAlarmLevel, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	// 数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;
	IVS_INT32 iReqType = 1;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::AlarmLevelGetXML(pAlarmLevel, iReqType, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmLevel GetXML fail", "NA");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * mAlarmLevel = xmlReq.GetXMLStream(xmlLen);

	// 发送NSS协议
	CHECK_POINTER(mAlarmLevel, IVS_XML_PACK_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_MOD_ALARM_LEVEL_REQ, mAlarmLevel,strSMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);

	// const IVS_CHAR* pRsp转换为XML并转换为数据结构
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);
	return iRet;
}

// 删除告警级别
IVS_INT32 CAlarmLevelMgr::DeleteAlarmLevel(const IVS_UINT32 uiAlarmLevelId) const
{
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;
	
	IVS_INT32 iRet = CAlarmMgrXMLProcess::DeleteAlarmLevelGetXML(uiAlarmLevelId, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Get DeleteAlarmLevel XML fail", "NA");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * dAlarmLevel = xmlReq.GetXMLStream(xmlLen);

	CHECK_POINTER(dAlarmLevel, IVS_XML_PACK_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_DEL_ALARM_LEVEL_REQ, dAlarmLevel,strSMULinkID);

	CHECK_POINTER(pCmd,IVS_OPERATE_MEMORY_ERROR);
	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);

	// const IVS_CHAR* pRsp转换为XML并转换为数据结构
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);
	return iRet;
}

// 查询告警级别列表
IVS_INT32 CAlarmLevelMgr::GetAlarmLevelList(IVS_UINT32 uiMaxAlarmLevelNum, IVS_UINT32* puiAlarmLevelNum, IVS_ALARM_LEVEL* pAlarmLevel, IVS_UINT32 /*uiBufferSize*/) const
{
	CHECK_POINTER(puiAlarmLevelNum, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmLevel, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);//lint !e613
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
 	//构造nss协议
 	CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_ALARM_LEVEL_LIST_REQ,"",strSMULinkID);
 	CHECK_POINTER(pCmd,IVS_OPERATE_MEMORY_ERROR);
 
 	//发送nss消息
 	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
  	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);

	IVS_INT32 iRet = IVS_SUCCEED;
	const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ParseCmd2XML fail", "NA");
		HW_DELETE(pCmdRsp);
		return iRet;
	}    

 	//解析XML响应消息为结构体
 	CXml xmlRsp;
	if (!xmlRsp.Parse(pRsp))
	{
		BP_RUN_LOG_ERR(IVS_XML_PACK_ERROR, "parse xml failed", "NA");
		HW_DELETE(pCmdRsp);
		IVS_DELETE(pRsp,MUILI);
		return IVS_XML_PACK_ERROR;
	}
 	
 	iRet = CAlarmMgrXMLProcess::GetAlarmTypeLevelPraseXML(uiMaxAlarmLevelNum, puiAlarmLevelNum, pAlarmLevel, xmlRsp);
 
	HW_DELETE(pCmdRsp);
	IVS_DELETE(pRsp,MUILI);

	return iRet;

}

// 查询告警级别详细信息
IVS_INT32 CAlarmLevelMgr::GetAlarmLevel(IVS_UINT32 uiAlarmLevelID,IVS_ALARM_LEVEL* pAlarmLevel) const
{
	CHECK_POINTER(m_UserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmLevel, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	// 拼装xml
	CXml xmlReq;
	
	IVS_INT32 iRet = CAlarmMgrXMLProcess::GetAlarmLevelGetXML(uiAlarmLevelID,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLevel  GetXML fail", "NA");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	// 发送nss协议
	CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_ALARM_LEVEL_REQ, pReqSpace,strSMULinkID);
	CHECK_POINTER(pCmd,IVS_OPERATE_MEMORY_ERROR);
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);

	// const IVS_CHAR* pRsp转换为XML并转换为数据结构
	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);
	IVS_CHAR* pNSSRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ParseCmd2XML fail", "NA");
		HW_DELETE(pCmdRsp);
		return iRet;
	}

	if (!xmlReq.Parse(pNSSRsp))
	{
		BP_RUN_LOG_ERR(IVS_XML_PACK_ERROR, "prase xml fail", "NA");
		HW_DELETE(pCmdRsp);
		IVS_DELETE(pNSSRsp,MUILI);
		return IVS_XML_PACK_ERROR;
	}
	
	iRet = CAlarmMgrXMLProcess::GetAlarmLevelPraseXML(pAlarmLevel,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Prase Response XML fail", "NA");
		HW_DELETE(pCmdRsp);
		IVS_DELETE(pNSSRsp, MUILI);
		return iRet;
	}
	HW_DELETE(pCmdRsp);
	IVS_DELETE(pNSSRsp,MUILI);

	return iRet;
}
