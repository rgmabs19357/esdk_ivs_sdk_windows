#include "RecordBackupMgr.h"
#include "UserMgr.h"
#include "NSSOperator.h"
#include "IVS_Trace.h"
#include "XmlProcess.h"
#include "NSSOperator.h"

CRecordBackupMgr::CRecordBackupMgr(void)
{
	m_pUserMgr = NULL;
}

CRecordBackupMgr::~CRecordBackupMgr(void)
{
	m_pUserMgr = NULL;
}

// 设置用户对象类
void CRecordBackupMgr::SetUserMgr(void *pUserMgr)
{
	m_pUserMgr = (CUserMgr *)pUserMgr;
}

// 设置摄像机备份参数
IVS_INT32 CRecordBackupMgr::SetCameraBackupCfg(const IVS_CHAR* pBackupXml)
{
	CHECK_POINTER(pBackupXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	if (!xmlReq.Parse(pBackupXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Set Camera Backup Cfg", "xmlReq.Parse(pBackupXml) fail");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = SetCameraCodeValue(xmlReq, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }


	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SET_CAMERA_BACKUP_CFG_REQ);
	sendNssMsgInfo.SetReqData(pGetXml); 

	//暂时只用本域，待多域时在添加以下两句
 	sendNssMsgInfo.SetCameraCode(szCameraCode);
 	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}

// 重设摄像机编码
IVS_INT32 CRecordBackupMgr::SetCameraCodeValue(CXml& xml, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode) const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_INT32 iRet = IVS_SUCCEED;
	if (xml.FindElemEx("Content/BackupInfo"))
	{
		xml.IntoElem();
		if (!xml.FindElem("CameraCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AlarmInCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xml.GetElemValue();
		iRet = CXmlProcess::ParseDevCode(pElemValue, pDevCode, pDomainCode);
		xml.ModifyElemValue(pDevCode);
		xml.OutOfElem();
	}

	xml.GetRootPos();
	if (!xml.MkAndSetElemValueEx("Content/BackupInfo/CameraDomainCode", pDomainCode))
	{
		BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"Set CameraCode Value", "add Camera DomanCode failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return iRet;
}

// 获取摄像机备份参数
IVS_INT32 CRecordBackupMgr::GetCameraBackupCfg(const IVS_CHAR* pCameraCode, IVS_CHAR** pBackConfigXml)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};

	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Camera Backup Config", "Parse DevCode failed");
		return iRet;
	}

	CXml xml;
	iRet = GetCameraBackupCfgGetXml(xml, szCameraCode, szDomainCode);

	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }


	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CAMERA_BACKUP_CFG_REQ);
	sendNssMsgInfo.SetReqData(pReq); 

	// 暂时只用本域，待多域时在添加以下两句
 	sendNssMsgInfo.SetCameraCode(szCameraCode);
 	sendNssMsgInfo.SetDomainCode(szDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	(void)IVS_NEW((IVS_CHAR *&)*pBackConfigXml, strpRsp.size() + 1);
	memset(*pBackConfigXml, 0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(*pBackConfigXml, strpRsp.size() + 1, strpRsp.c_str(), strpRsp.size()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Camera Backup Cfg", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return iRet;
}

// 拼装获取摄像机备份参数的xml
IVS_INT32 CRecordBackupMgr::GetCameraBackupCfgGetXml(CXml& xml, const IVS_CHAR *pDevCode, const IVS_CHAR* pDomainCode)const
{
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	
	(void)xml.AddElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevCode);
	xml.GetRootPos();
	
	return IVS_SUCCEED;
}

// 添加录像备份计划
IVS_INT32 CRecordBackupMgr::AddRecordBackupPlan(const IVS_CHAR* pBackupPlan)
{
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	return RecordBackupPlan(NSS_ADD_BACKUP_PLAN_REQ, pBackupPlan);
}

// 修改备份录像计划
IVS_INT32 CRecordBackupMgr::ModifyRecordBackupPlan(const IVS_CHAR* pBackupPlan)
{
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	return RecordBackupPlan(NSS_MOD_BACKUP_PLAN_REQ, pBackupPlan);
}

// 删除备份录像计划
IVS_INT32 CRecordBackupMgr::DeleteRecordBackupPlan(const IVS_CHAR* pBackupPlan)
{
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	return RecordBackupPlan(NSS_DEL_BACKUP_PLAN_REQ, pBackupPlan);
}

// 新增录像备份任务
IVS_INT32 CRecordBackupMgr::AddRecordBackupTask(const IVS_CHAR* pBackupTask)
{
	CHECK_POINTER(pBackupTask, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pBackupTask))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Start Record Backup Task", "xmlReq.Parse(pQueryInfo) fail");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = SetNodeFileNameValue(xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Add Record Backup Task", "xmlReq Add FileName(pQueryInfo) fail");
		return iRet;
	}
	iRet = SetNodeCameraCodeValue(xmlReq, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Add Record Backup Task", "xmlReq Add DomainCode(pQueryInfo) fail");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }


	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_BACKUP_TASK_REQ);
	sendNssMsgInfo.SetReqData(pGetXml); 
	// 	sendNssMsgInfo.SetCameraCode(szCameraCode);
	if (xmlReq.FindElemEx("Content/MBUDomainCode"))
	{
		const IVS_CHAR* pMBUDomainCode = xmlReq.GetElemValue();
        if (NULL != pMBUDomainCode)
        {
            sendNssMsgInfo.SetDomainCode(pMBUDomainCode);
        }
	}

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	return iRet;
}

// 查询备份录像计划
IVS_INT32 CRecordBackupMgr::GetRecordBackupPlan(const IVS_CHAR* pQueryInfo, IVS_CHAR** pBackupPlan)
{
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pQueryInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Set Camera Backup Cfg", "xmlReq.Parse(pReqXml) fail");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = SetNodeCameraCodeValue(xmlReq, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Node Camera Code Value", "failed");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);

    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_BACKUP_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq); 

	// 暂时只用本域，待多域时在添加以下两句
	sendNssMsgInfo.SetCameraCode(szCameraCode);
	sendNssMsgInfo.SetDomainCode(szDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	(void)IVS_NEW((IVS_CHAR *&)*pBackupPlan, strpRsp.size() + 1);
	if (NULL == *pBackupPlan)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}

	memset(*pBackupPlan, 0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(*pBackupPlan, strpRsp.size() + 1, strpRsp.c_str(), strpRsp.size()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record Backup Plan", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return iRet;
}

// 录像备份计划公共发送接口
IVS_INT32 CRecordBackupMgr::RecordBackupPlan(NSS_MSG_TYPE_E msgType, const IVS_CHAR* pBackupPlan)
{
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	if (!xmlReq.Parse(pBackupPlan))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Record Backup Plan", "xmlReq.Parse(pBackupPlan) fail");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = SetNodeCameraCodeValue(xmlReq, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }


	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(msgType);
	sendNssMsgInfo.SetReqData(pGetXml); 

	//暂时只查本域，待多域时此处就需要
	sendNssMsgInfo.SetCameraCode(szCameraCode);
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}

IVS_INT32 CRecordBackupMgr::SetNodeCameraCodeValue(CXml& xml, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode) const
{
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");
	if (xml.FindElemEx("Content"))
	{
		xml.IntoElem();
		if (!xml.FindElem("CameraCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(CameraCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xml.GetElemValue();
		IVS_INT32 iRet = CXmlProcess::ParseDevCode(pElemValue, pDevCode, pDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Set Add Record Backup Plan CameraCode Value", "Parse DevCode failed");
			return iRet;
		}
		xml.ModifyElemValue(pDevCode);
		xml.OutOfElem();
	}

	xml.GetRootPos();
	if (!xml.MkAndSetElemValueEx("Content/DomainCode", pDomainCode))
	{
		BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"Set CameraCode Value", "add Camera DomanCode failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return IVS_SUCCEED;
}

IVS_INT32 CRecordBackupMgr::SetNodeFileNameValue(CXml& xml) const
{
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iSourceType = 0;
	IVS_TIME_SPAN stTimeSpan; 
	memset(&stTimeSpan, 0, sizeof(IVS_TIME_SPAN)); 
	if (xml.FindElemEx("Content"))
	{
		xml.IntoElem();
		if (!xml.FindElem("CameraCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(CameraCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pCameraCode = xml.GetElemValue();
		if (!xml.FindElem("SourceType"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(SourceType)", "NA");
			return IVS_XML_INVALID;
		}
		const char* pTemp = NULL;
		GET_ELEM_VALUE_NUM("SourceType", pTemp,iSourceType,xml);
		if (!xml.FindElem("StartTime"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(StartTime)", "NA");
			return IVS_XML_INVALID;
		}
		GET_ELEM_VALUE_CHAR("StartTime",pTemp,stTimeSpan.cStart,IVS_TIME_LEN,xml);
		if (!xml.FindElem("EndTime"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(EndTime)", "NA");
			return IVS_XML_INVALID;
		}
		GET_ELEM_VALUE_CHAR("EndTime",pTemp,stTimeSpan.cEnd,IVS_TIME_LEN,xml);
		xml.OutOfElem();
		if(1 == iSourceType)//录像来源(实时备份只有平台):0-平台 1-前端
		{
			IVS_RECORD_INFO stRecordInfo;
			memset(&stRecordInfo, 0, sizeof(IVS_RECORD_INFO));
			IVS_INT32 iRet = m_pUserMgr->GetDownLoadMgr().GetPUDownLoadFileName(pCameraCode, &stTimeSpan,stRecordInfo);
			if (IVS_SUCCEED != iRet)
			{
				return iRet;
			}
			xml.GetRootPos();
			if (!xml.MkAndSetElemValueEx("Content/FileName", stRecordInfo.cRecordFileName))
			{
				BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"Set FileName Value", "add Record File Name failed");
				return IVS_OPERATE_MEMORY_ERROR;
			}
		}
	}
	return IVS_SUCCEED;
}


//启动/继续录像备份任务
IVS_INT32 CRecordBackupMgr::StartRecordBackupTask(const IVS_CHAR* pQueryInfo)
{
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pQueryInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Start Record Backup Task", "xmlReq.Parse(pQueryInfo) fail");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = SetNodeCameraCodeValue(xmlReq, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Start Record Backup Task", "xmlReq Add DomainCode(pQueryInfo) fail");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }


	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_START_BACKUP_TASK_REQ);
	sendNssMsgInfo.SetReqData(pGetXml); 
	// 	sendNssMsgInfo.SetCameraCode(szCameraCode);
	if (xmlReq.FindElemEx("Content/MBUDomainCode"))
	{
		const IVS_CHAR* pMBUDomainCode = xmlReq.GetElemValue();
        if (NULL != pMBUDomainCode)
        {
            sendNssMsgInfo.SetDomainCode(pMBUDomainCode);
        }
		
	}

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	return iRet;
}
//暂停/停止录像备份任务
IVS_INT32 CRecordBackupMgr::StopRecordBackupTask(const IVS_CHAR* pQueryInfo)
{
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pQueryInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Stop Record Backup Task", "xmlReq.Parse(pQueryInfo) fail");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = SetNodeCameraCodeValue(xmlReq, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Stop Record Backup Task", "xmlReq Add DomainCode(pQueryInfo) fail");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
 
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }
    
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_STOP_BACKUP_TASK_REQ);
	sendNssMsgInfo.SetReqData(pGetXml); 
	// 	sendNssMsgInfo.SetCameraCode(szCameraCode);
	if (xmlReq.FindElemEx("Content/MBUDomainCode"))
	{
		const IVS_CHAR* pMBUDomainCode = xmlReq.GetElemValue();

        if (NULL != pMBUDomainCode)
        {
            sendNssMsgInfo.SetDomainCode(pMBUDomainCode);
        }
		
	}

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	return iRet;
}

// 查询录像备份任务列表
IVS_INT32 CRecordBackupMgr::GetRecordBackupTaskList(const IVS_CHAR* pQueryInfo, IVS_CHAR** pRspXml)
{
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pQueryInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Get Record Backup Task List", "xmlReq.Parse(pQueryInfo) fail");
		return IVS_XML_INVALID;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }


	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_BACKUP_TASK_LIST_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	// 	sendNssMsgInfo.SetCameraCode(szCameraCode);
	if (xmlReq.FindElemEx("Content/MBUDomainCode"))
	{
		const IVS_CHAR* pMBUDomainCode = xmlReq.GetElemValue();

        if (NULL != pMBUDomainCode)
        {
            sendNssMsgInfo.SetDomainCode(pMBUDomainCode);
        }

	}
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Get Record Backup Task List", "Send Cmd Rsp fail");
		return iRet;
	}

	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());
	IVS_INT32 iRealNum = 0;
	IVS_INT32 iFromIndex = 0;
	if (xmlRsp.FindElemEx("Content/PageInfo"))
	{
		(void)xmlRsp.IntoElem();
		const char* pTemp = NULL;
		if (!xmlRsp.FindElem("RealNum"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(RealNum)", "NA");
			return IVS_XML_INVALID;
		}
		GET_ELEM_VALUE_NUM("RealNum",pTemp, iRealNum, xmlRsp);
		GET_ELEM_VALUE_NUM("FromIndex",pTemp, iFromIndex, xmlRsp);
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(PageInfo)", "NA");
		return IVS_XML_INVALID;
	}

	if (xmlRsp.FindElemEx("Content/BackupTaskList")&& iRealNum >0 && iRealNum >= iFromIndex)
	{
		do 
		{
			(void)xmlRsp.IntoElem();
			(void)xmlRsp.IntoElem();
			const char* szElemValue = NULL;
			char cCameraCode[IVS_DEV_CODE_LEN+1]={0};
			char cDomainCode[IVS_DEV_CODE_LEN+1]={0};
			if (!xmlRsp.FindElem("DomainCode"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(DomainCode)", "NA");
				return IVS_XML_INVALID;
			}
			GET_ELEM_VALUE_CHAR("DomainCode",szElemValue,cDomainCode,IVS_DEV_CODE_LEN,xmlRsp);
			xmlRsp.RemoveElem();
			if (!xmlRsp.FindElem("CameraCode"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(CameraCode)", "NA");
				return IVS_XML_INVALID;
			}
			GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,cCameraCode,IVS_DEV_CODE_LEN,xmlRsp);
			std::string strBigCode;
			strBigCode.append(cCameraCode).append("#").append(cDomainCode);
			xmlRsp.ModifyElemValue(strBigCode.c_str());			
			xmlRsp.OutOfElem();
		} while(xmlRsp.NextElem());
	}
	IVS_UINT32 uiXmlLen = 0;
	strpRsp = xmlRsp.GetXMLStream(uiXmlLen);

	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strpRsp.size() + 1);
	if (NULL == *pRspXml)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(*pRspXml, 0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(*pRspXml, strpRsp.size() + 1, strpRsp.c_str(), strpRsp.size()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record Backup Task List", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return iRet;
}

//删除录像备份任务
IVS_INT32 CRecordBackupMgr::DeleteRecordBackupTask(const IVS_CHAR* pQueryInfo)
{
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pQueryInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "Delete Record Backup Task", "xmlReq.Parse(pQueryInfo) fail");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = SetNodeCameraCodeValue(xmlReq, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Delete Record Backup Task", "xmlReq Add DomainCode(pQueryInfo) fail");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }


	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_BACKUP_TASK_REQ);
	sendNssMsgInfo.SetReqData(pGetXml); 
	// 	sendNssMsgInfo.SetCameraCode(szCameraCode);
	if (xmlReq.FindElemEx("Content/MBUDomainCode"))
	{
		const IVS_CHAR* pMBUDomainCode = xmlReq.GetElemValue();
        if (NULL != pMBUDomainCode)
        {
		    sendNssMsgInfo.SetDomainCode(pMBUDomainCode);
        }

	}
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	return iRet;
}

