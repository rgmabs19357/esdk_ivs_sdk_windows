#include "ClientNetworkMgr.h"
#include "UserMgr.h"
#include "NSSOperator.h"
#include "ClientNetworkMgrXMLProcess.h"
#include "IVS_Trace.h"

CClientNetworkMgr::CClientNetworkMgr(void)
    :m_UserMgr(NULL)
{
}

CClientNetworkMgr::~CClientNetworkMgr(void)
{
    m_UserMgr = NULL;
}

// 设置用户对象类
void CClientNetworkMgr::SetUserMgr(CUserMgr* pUserMgr)
{
	m_UserMgr = pUserMgr;
}

// 客户端查询CPU/内存性能
IVS_INT32 CClientNetworkMgr::GetCPUMemory(const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance)const
{
	CHECK_POINTER(pNodeCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	IVS_INT32 iRet = CClientNetworkMgrXMLProcess::NodeCodeGetXML(pNodeCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "NodeCodeGetXML  GetXML fail", "NA");
		return iRet;
	}

	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(uiXmlLen);
    // 获取本域OMU连接
    std::string strOMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strOMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd* pCmd = CNSSOperator::instance().BuildOMUCmd(NSS_CPU_QUERY_REQ, pReqXml, strOMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 同步发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	*pPerformance = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 获取网元树
IVS_INT32 CClientNetworkMgr::GetDomainList(IVS_CHAR** pDomainList)const
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    // 获取本域OMU连接
    std::string strOMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strOMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd *pCmd = CNSSOperator::instance().BuildOMUCmd(NSS_GET_DOMAIN_LIST_REQ,"", strOMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	//同步发送消息测试
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	IVS_INT32 iRet = IVS_FAIL;
	const char* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iRet);
	HW_DELETE(pCmdRsp);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ParseCmd2XML fail", "NA");
		return iRet;
	}

    if (NULL == pRsp)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "ParseCmd2XML fail", "pRsp is null.");
        return IVS_XML_INVALID;
    }

	IVS_CHAR* pResult = IVS_NEW(pResult, strlen(pRsp) + 1);
	if (!CToolsHelp::Memcpy(pResult, strlen(pRsp) + 1, pRsp, strlen(pRsp) + 1))
	{
		IVS_DELETE(pRsp, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pRsq to pResult failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	*pDomainList = pResult;
	IVS_DELETE(pRsp, MUILI);
	return iRet;
}

// 智能分析CPU/内存性能
IVS_INT32 CClientNetworkMgr::GetIACPUMemory(const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance)const
{
	CHECK_POINTER(pNodeCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	IVS_INT32 iRet = CClientNetworkMgrXMLProcess::NodeCodeGetXML(pNodeCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "NodeCode GetXML fail", "NA");
		return iRet;
	}

	IVS_UINT32 uixmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(uixmlLen);
    // 获取本域OMU连接
    std::string strOMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strOMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd* pCmd = CNSSOperator::instance().BuildOMUCmd(NSS_CPU_NEXT_QUERY_REQ, pReqXml, strOMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	CCmd* pRspCmd = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pRspCmd, IVS_NET_RECV_TIMEOUT);

	iRet = IVS_SUCCEED;
	const IVS_CHAR* pRsq = CNSSOperator::instance().ParseCmd2XML(pRspCmd, iRet);
	HW_DELETE(pRspCmd);
	CHECK_POINTER(pRsq, IVS_SMU_XML_INVALID);
	CXml xml;
	if (!xml.Parse(pRsq))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "!xml.Parse(pRsq)", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_SMU_XML_INVALID;
	}
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "!xml.Parse(pRsq)", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_SMU_XML_INVALID;
	}
	(void)xml.IntoElem();
	if (!xml.FindElem("RESULT_CODE"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "!xml.Parse(pRsq)", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_SMU_XML_INVALID;
	}
	xml.ModifyElemName("ResultCode");
	xml.OutOfElem();
	IVS_DELETE(pRsq, MUILI);
	uixmlLen = 0;
	const IVS_CHAR* pRsqXml = xml.GetXMLStream(uixmlLen);
    if (NULL == pRsqXml)
    {
        return IVS_XML_INVALID;
    }
	*pPerformance = IVS_NEW((IVS_CHAR* &)(*pPerformance), strlen(pRsqXml) + 1);
	memset(*pPerformance, 0, strlen(pRsqXml) + 1);
	if (!CToolsHelp::Memcpy(*pPerformance, strlen(pRsqXml) + 1, pRsqXml, strlen(pRsqXml) + 1))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy(*pPerformance, strlen((pRsqXml) + 1), pRsqXml, strlen((pRsqXml) + 1))", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	return iRet;
}

// 网元性能查询
IVS_INT32 CClientNetworkMgr::QueryPerformance(const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance)const
{
	CHECK_POINTER(pNodeCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	IVS_INT32 iRet = CClientNetworkMgrXMLProcess::NodeCodeGetXML(pNodeCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "NodeCode GetXML fail", "NA");
		return iRet;
	}

	IVS_UINT32 uixmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(uixmlLen);
    // 获取本域OMU连接
    std::string strOMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strOMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd* pCmd = CNSSOperator::instance().BuildOMUCmd(NSS_PERFORMACE_QUERY_REQ, pReqXml, strOMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	CCmd* pRspCmd = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pRspCmd, IVS_NET_RECV_TIMEOUT);

	iRet = IVS_SUCCEED;
	*pPerformance = CNSSOperator::instance().ParseCmd2XML(pRspCmd, iRet);
	HW_DELETE(pRspCmd);

	return iRet;
}

// 新增告警通知规则接口
IVS_INT32 CClientNetworkMgr::AddAlarmLinkageRule(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_ADD_ALARM_LINKAGE_RULE_REQ, pReqXml, pRspXml);
}

// 修改告警通知规则接口
IVS_INT32 CClientNetworkMgr::ModAlarmLinkageRule(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_MOD_ALARM_LINKAGE_RULE_REQ, pReqXml);
}

// 删除告警通知规则接口
IVS_INT32 CClientNetworkMgr::DelAlarmLinkageRule(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_DEL_ALARM_LINKAGE_RULE_REQ, pReqXml);
}

// 获取告警通知规则列表接口
IVS_INT32 CClientNetworkMgr::GetAlarmLinkageRuleList(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_GET_ALARM_LINKAGE_RULE_LIST_REQ, pReqXml, pRspXml);
}

// 获取告警通知规则详情接口
IVS_INT32 CClientNetworkMgr::GetAlarmLinkageRuleDetail(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_GET_ALARM_LINKAGE_RULE_BYID_REQ, pReqXml, pRspXml);
}

// 根据网元获取设备类型接口
IVS_INT32 CClientNetworkMgr::GetDevTypeByNodeCode(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_GET_MODULETYPE_BY_NODE_CODE_REQ, pReqXml, pRspXml);
}

IVS_INT32 CClientNetworkMgr::ComSendToOMU(NSS_MSG_TYPE_E msgType, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Com SendTo OMU", "xmlReq.Parse(pReqXml) fail");
		return IVS_XML_INVALID;
	}

	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiXmlLen);
    // 获取本域OMU连接
    std::string strOMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strOMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd* pCmd = CNSSOperator::instance().BuildOMUCmd(msgType, pReq, strOMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 同步发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	IVS_INT32 iRet = IVS_FAIL;
	*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);

	return iRet;
}

IVS_INT32 CClientNetworkMgr::ComSendToOMU(NSS_MSG_TYPE_E msgType, const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Com Send To OMU", "xmlReq.Parse(pReqXml) fail");
		return IVS_XML_INVALID;
	}

	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiXmlLen);
    // 获取本域OMU连接
    std::string strOMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_OMU_NSS, strOMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd* pCmd = CNSSOperator::instance().BuildOMUCmd(msgType, pReq, strOMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 同步发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	IVS_INT32 iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	
	HW_DELETE(pCmdRsp);

	return iRet;
}


// 客户端升级检查
IVS_INT32 CClientNetworkMgr::CheckNewVersion(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Check New Version", "xmlReq.Parse(pReqXml) fail");
		return IVS_XML_INVALID;
	}

	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiXmlLen);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_CU_VERSION_REQ, pReq, strSMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 同步发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	IVS_INT32 iRet = IVS_FAIL;
	*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);

	return iRet;

}

// 磁盘组信息查询
IVS_INT32 CClientNetworkMgr::QueryRaidGroup(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_RAID_GROUP_REQ, pReqXml, pRspXml);
}

// 磁盘信息查询
IVS_INT32 CClientNetworkMgr::QueryRaidGroupDisk(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	return ComSendToOMU(NSS_OMU_RAID_GROUP_DISK_REQ, pReqXml, pRspXml);
}

