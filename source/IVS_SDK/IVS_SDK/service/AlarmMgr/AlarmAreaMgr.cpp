#include "AlarmAreaMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "ivs_error.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "AlarmMgrXMLProcess.h"
#include "IVS_Trace.h"
CAlarmAreaMgr::CAlarmAreaMgr(void)
	:m_pUserMgr(NULL)
{
}
CAlarmAreaMgr::~CAlarmAreaMgr(void)
{
	m_pUserMgr = NULL;
}

void CAlarmAreaMgr::SetUserMgr( CUserMgr *pUserMgr)
{
	m_pUserMgr = pUserMgr;
}



// 新增防区
IVS_INT32 CAlarmAreaMgr::AddAlarmArea(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	IVS_INT32 iRet = IVS_FAIL;	
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}

	std::string strDomainCode;
    
    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	// 拆分AlarmInCode，添加DevDomainCode节点(可以增加空防区，即可能没有告警源设备信息信息)
	iRet = CAlarmMgrXMLProcess::SetAlarmInCode4AlarmAreaReq(xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Alarm In Code fail", "NA");
		return iRet;
	}
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_ALARM_AREA_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);
	
	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add AlarmArea fail","Send Cmd failed");
		return iRet;
	}
	iRet = CAlarmMgrXMLProcess::GetRsqXML(strpRsp.c_str(), *pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Rsq XML Add AlarmArea fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

//修改防区
IVS_INT32 CAlarmAreaMgr::ModifyAlarmArea(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	IVS_INT32 iRet = IVS_FAIL;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	// 获取域编码
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}

    std::string strDomainCode;

    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	// 拆分AlarmInCode，添加DevDomainCode节点
	// 只修改防区名称没有AlarmInList节点；修改防区的设备带AlarmInList节点；如果只有AlarmInList节点，但是没有AlarmInInfo节点，表示删除防区下的所有设备
	iRet = CAlarmMgrXMLProcess::SetAlarmInCode4AlarmAreaReq(xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Alarm In Code of Modify AlarmArea fail", "NA");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReqMsg = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqMsg, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_ALARM_AREA_REQ);
	sendNssMsgInfo.SetReqData(pReqMsg);
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Modify AlarmArea fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 删除防区
IVS_INT32 CAlarmAreaMgr::DeleteAlarmArea(const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get DeleteAlarmArea XML fail", "NA");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pAlarmArea = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pAlarmArea, IVS_XML_PACK_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_ALARM_AREA_REQ);
	sendNssMsgInfo.SetReqData(pAlarmArea);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Delete Alarm Area fail","Send Cmd failed");
		return iRet;
	}

	return iRet;
}

// 查看防区列表
IVS_INT32 CAlarmAreaMgr::GetAlarmAreaList(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	// 获取域编码，拆分AlarmInCode
	std::string strDomainCode;
	CXml xml;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::GetDomainCodeANDParseAlarmInCode(pReqXml, strDomainCode, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDomainCodeANDParseAlarmInCode fail", "NA");
		return iRet;
	}

	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(uiXmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_ALARM_AREA_LIST_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Alarm Area List fail","Send Cmd failed");
		return iRet;
	}

	// 响应消息中的DevDomainCode拼回给AlarmInCode
	CXml xmlRsq;
	if (!xmlRsq.Parse(strpRsp.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.Parse(strpRsp.c_str()) fail", "NA");
		return IVS_SMU_XML_INVALID;
	}
	if (xmlRsq.FindElemEx("Content/AlarmAreaList"))
	{
		BP_RUN_LOG_INF("FindElemEx(Content/AlarmAreaList)", "NA");
		if (!xmlRsq.FindElem("AlarmAreaInfo"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.FindElem(\"AlarmInInfo\") fail", "NA");
			return IVS_SMU_XML_INVALID;
		}
		do 
		{
			xmlRsq.IntoElem();
			if (xmlRsq.FindElem("AlarmInList"))
			{
				xmlRsq.IntoElem();
				if (!xmlRsq.FindElem("AlarmInInfo"))
				{
					BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.FindElem(\"AlarmInInfo\") fail", "NA");
					return IVS_SMU_XML_INVALID;
				}
				do 
				{
					xmlRsq.IntoElem();
					const IVS_CHAR* szElemValue = NULL;
					IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
					IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
					GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, szDomainCode, IVS_DOMAIN_CODE_LEN, xmlRsq);
					GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, szAlarmInCode, IVS_ALARM_CODE_LEN, xmlRsq);
					std::string strAlarmInCode = szAlarmInCode;
					if (!strAlarmInCode.empty())
					{
						strAlarmInCode.append("#").append(szDomainCode);
						xmlRsq.ModifyElemValue(strAlarmInCode.c_str());
					}
					xmlRsq.OutOfElem();
				} while (xmlRsq.NextElem());
				xmlRsq.OutOfElem();
			}
			xmlRsq.OutOfElem();
		} while (xmlRsq.NextElem());
	}	

	unsigned int xmlLen = 0;
	const IVS_CHAR* pRsq = xmlRsq.GetXMLStream(xmlLen);
	CHECK_POINTER(pRsq, IVS_OPERATE_MEMORY_ERROR);

	iRet = CAlarmMgrXMLProcess::GetRsqXML(pRsq, *pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Rsq XML Get AlarmArea List fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 查看防区详情
IVS_INT32 CAlarmAreaMgr::GetAlarmAreaInfo(const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId,IVS_CHAR** pAlarmArea)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmArea, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId,xmlReq);	
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, " AlarmArea GetXML fail", "NA");
		return iRet;
	}
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_ALARM_AREA_INFO_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get AlarmArea Info fail","Send Cmd failed");
		return iRet;
	}

	// 响应消息中的DevDomainCode拼回给AlarmInCode
	CXml xmlRsq;
	if (!xmlRsq.Parse(strpRsp.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.Parse(strpRsp.c_str()) fail", "NA");
		return IVS_SMU_XML_INVALID;
	}
	if (xmlRsq.FindElemEx("Content/AlarmAreaInfo/AlarmInList"))
	{
		if (!xmlRsq.FindElem("AlarmInInfo"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.FindElem(\"AlarmInInfo\") fail", "NA");
			return IVS_SMU_XML_INVALID;
		}
		do 
		{
			xmlRsq.IntoElem();
			const IVS_CHAR* szElemValue = NULL;
			IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
			IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
			GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, szDomainCode, IVS_DOMAIN_CODE_LEN, xmlRsq);
			GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, szAlarmInCode, IVS_ALARM_CODE_LEN, xmlRsq);
			std::string strAlarmInCode = szAlarmInCode;
			if (!strAlarmInCode.empty())
			{
				strAlarmInCode.append("#").append(szDomainCode);
				xmlRsq.ModifyElemValue(strAlarmInCode.c_str());
			}

			xmlRsq.OutOfElem();
		} while (xmlRsq.NextElem());
	}	

	const IVS_CHAR* pRsq = xmlRsq.GetXMLStream(xmlLen);
	CHECK_POINTER(pRsq, IVS_OPERATE_MEMORY_ERROR);

	iRet = CAlarmMgrXMLProcess::GetRsqXML(pRsq, *pAlarmArea);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmAreaInfo fail", "NA");
		return iRet;
	}

	return iRet;
}

// 手动布防
IVS_INT32 CAlarmAreaMgr::StartAlarmAreaGuard(const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get AlarmArea XML fail", "NA");
		return iRet;
	}
	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(uiXmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_START_AREA_GUARD_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start AlarmArea Guard fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 手动撤防
IVS_INT32 CAlarmAreaMgr::StopAlarmAreaGuard(const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get AlarmArea XML", "NA");
		return iRet;
	}
	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(uiXmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_STOP_AREA_GUARD_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Stop AlarmArea Guard fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 新增布防计划
IVS_INT32 CAlarmAreaMgr::AllocAreaGuardPlan(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}


	std::string strDomainCode;
    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ALLOC_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	IVS_INT32 iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Alloc AreaGuard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 修改布防计划
IVS_INT32 CAlarmAreaMgr::ModifyAreaGuardPlan(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}

	std::string strDomainCode;
    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);
	IVS_INT32 iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Modify AreaGuard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 删除布防计划
IVS_INT32 CAlarmAreaMgr::DeleteAreaGuardPlan(const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::GetAlarmAreaPlanXML(pDomainCode, uiAlarmAreaId,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, " AlarmAreaPlan GetXML is fail", "NA");
		return iRet;
	}
	const IVS_CHAR* pReqXml = NULL;
	IVS_UINT32 xmlLen = 0;
	pReqXml = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Delete Area Guard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 查看布防计划
IVS_INT32 CAlarmAreaMgr::GetAreaGuardPlan(const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::GetAlarmAreaPlanXML(pDomainCode, uiAlarmAreaId,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AlarmAreaPlan GetXML is fail", "NA");
		return iRet;
	}
	const IVS_CHAR* pReqXml = NULL;
	IVS_UINT32 xmlLen = 0;
	pReqXml = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	iRet = IVS_FAIL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Area Guard Plan fail","Send Cmd failed");
		return iRet;
	}

	iRet = CAlarmMgrXMLProcess::GetRsqXML(strpRsp.c_str(), *pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Rsq XML Get AreaGuard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}
