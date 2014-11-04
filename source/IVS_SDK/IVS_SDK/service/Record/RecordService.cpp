#include "RecordService.h"
#include "UserMgr.h" 
//#include "bp_log.h" //lint !e763
//#include "nss_mt.h"
#include "NSSOperator.h"
#include "IVS_Trace.h"

CRecordService::CRecordService(void)
{
	m_pInstMutex = VOS_CreateMutex();
	m_pUserMgr = NULL;//lint !e831
}


CRecordService::~CRecordService(void)
{
	if (NULL != m_pInstMutex)
	{
		(void)VOS_DestroyMutex(m_pInstMutex);
		m_pInstMutex = NULL;
	}

	m_pUserMgr = NULL;
}

void CRecordService::SetUserMgr( CUserMgr *pUserMgr)
{
	m_pUserMgr = pUserMgr;
}
//释放录像线程锁
void CRecordService::Release()
{
    (void)VOS_MutexLock(m_pInstMutex);
	for (int i = 0; i < MAX_RECORD_COUNT; i++)
	{
		if (RECORDING == m_RecordInst[i].GetState())
		{
			m_RecordInst[i].Release();
		}

		//m_RecordInst[i].m_Timer.exit();
	}
    (void)VOS_MutexUnlock(m_pInstMutex);
}


int CRecordService::GetRecordInst(int iPlayHandle, bool bStart, unsigned int& uiIndex)
{
	(void)VOS_MutexLock(m_pInstMutex);

	//IVS_LOG(LOG_INFO, "Enter GetRecordInst");

	if (bStart)
	{
		unsigned int i;
		for (i = 0; i < MAX_RECORD_COUNT; i++)
		{
			if (RECORD_IDLE == m_RecordInst[i].GetState())
			{
				m_RecordInst[i].SetState(RECORDING);
				m_RecordInst[i].SetPlayHandle(iPlayHandle);
				m_RecordInst[i].SetUserMgr(m_pUserMgr);
				uiIndex = i;
                (void)VOS_MutexUnlock(m_pInstMutex);
				return IVS_SUCCEED;
			}
		}
       (void) VOS_MutexUnlock(m_pInstMutex);
		//IVS_LOG(LOG_ERR, "CMediaService::GetRecordInst.");
		return IVS_FAIL;
	}
	else
	{
		unsigned int i;
		for (i = 0; i < MAX_RECORD_COUNT; i++)
		{
			if (RECORDING == m_RecordInst[i].GetState())
			{
				if (iPlayHandle == m_RecordInst[i].GetPlayHandle())
				{
					uiIndex = i;
                    (void)VOS_MutexUnlock(m_pInstMutex);
					return IVS_SUCCEED;
				}
			}
		}

        (void)VOS_MutexUnlock(m_pInstMutex);
		//IVS_LOG(LOG_ERR, "CMediaService::GetRecordInst.");
		return IVS_FAIL;
	}
}

int CRecordService::GetRecordInstByMpPort(unsigned long ulMpPort, unsigned int& uiIndex)
{
    (void)VOS_MutexLock(m_pInstMutex);
	//IVS_LOG(LOG_INFO, "Enter GetRecordInstByMpPort");

	unsigned int i;
	for (i = 0; i < MAX_RECORD_COUNT; i++)
	{
		if (RECORDING == m_RecordInst[i].GetState())
		{
			if (ulMpPort == m_RecordInst[i].GetPort())
			{
				uiIndex = i;
                (void)VOS_MutexUnlock(m_pInstMutex);
				return IVS_SUCCEED;
			}
		}
	}

	//IVS_LOG(LOG_INFO, "Out GetRecordInstByMpPort");
    (void)VOS_MutexUnlock(m_pInstMutex);
	return IVS_FAIL;
}

void CRecordService::CancelRecordInst(unsigned int uiIndex)
{
    (void)VOS_MutexLock(m_pInstMutex);
	//IVS_LOG(LOG_INFO, "Enter CancelRecordInst");

	if (uiIndex >= MAX_RECORD_COUNT)
	{
		//IVS_LOG(LOG_ERR, "CMediaService::CancelRecordInst.");
        (void)VOS_MutexUnlock(m_pInstMutex);
		return;
	}

	m_RecordInst[uiIndex].SetState(RECORD_IDLE);
	m_RecordInst[uiIndex].SetPlayHandle(0);
    (void)VOS_MutexUnlock(m_pInstMutex);
	//IVS_LOG(LOG_INFO, "Out CancelRecordInst");
}


//处理MP的流异常回调
void CRecordService::DealException(IVS_INT32 iPort, IVS_INT32 iMsgType,const void* pHandle)
{
	//关闭本地录像
	unsigned int uiIndex;
	int iRet = GetRecordInstByMpPort((unsigned int)iPort, uiIndex);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "CMediaService::DealException");
		return;
	}

	m_RecordInst[uiIndex].DealException(iPort, iMsgType, pHandle);
	CancelRecordInst(uiIndex);
}


//开始平台录像
IVS_INT32 CRecordService::StartPlatformRecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime)
{
    IVS_DEBUG_TRACE("");
	unsigned int uiIndex = 0;
	int iRet = GetRecordInst(0, true, uiIndex);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "creat download inst failed.");
		return IVS_FAIL;
	}

	iRet = m_RecordInst[uiIndex].StartPlatformRecord(pCameraCode,iRecordTime);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "failed.");
	}

	CancelRecordInst(uiIndex);
	return iRet;
}

//停止平台录像
IVS_INT32 CRecordService::StopPlatformRecord(const IVS_CHAR* pCameraCode)
{
	IVS_DEBUG_TRACE("");
	unsigned int uiIndex = 0;
	int iRet = GetRecordInst(0, true, uiIndex);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "creat download inst failed.");
		return IVS_FAIL;
	}

	iRet = m_RecordInst[uiIndex].StopPlatformRecord(pCameraCode);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "failed.");
	}

	CancelRecordInst(uiIndex);
	return iRet;
}

//开始前端录像
IVS_INT32 CRecordService::StartPURecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime)
{
	IVS_DEBUG_TRACE("");
	unsigned int uiIndex = 0;
	int iRet = GetRecordInst(0, true, uiIndex);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "creat download inst failed.");
		return IVS_FAIL;
	}

	iRet = m_RecordInst[uiIndex].StartPURecord(pCameraCode,iRecordTime);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "failed.");
	}

	CancelRecordInst(uiIndex);
	return iRet;
}

//停止前端录像
IVS_INT32 CRecordService::StopPURecord(const IVS_CHAR* pCameraCode)
{
	IVS_DEBUG_TRACE("");
	unsigned int uiIndex = 0;
	int iRet = GetRecordInst(0, true, uiIndex);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "creat download inst failed.");
		return IVS_FAIL;
	}

	iRet = m_RecordInst[uiIndex].StopPURecord(pCameraCode);
	if (IVS_SUCCEED != iRet)
	{
		//IVS_LOG(LOG_ERR, "failed.");
	}

	CancelRecordInst(uiIndex);
	return iRet;
}

//获取录像策略
IVS_INT32 CRecordService::GetRecordPolicy(const IVS_CHAR* pCameraCode, IVS_RECORD_POLICY_TIME* pRecordPolicy)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRecordPolicy, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	
	//解析pCameraCode
	IVS_INT32 iRet = IVS_FAIL;
	string chDevCode = "";
	string chDomaCode = "";
	iRet =CToolsHelp::SplitCameraDomainCode(pCameraCode,chDevCode,chDomaCode);
	//数据结构拼装XML并转换为const IVS_CHAR*
	CXml xmlReq;
	iRet = CRecordXMLProcess::GetRecordPolicyGetXML(xmlReq,chDevCode.c_str(),chDomaCode.c_str());
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Record Policy Get XML","failed");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_POLICY_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	sendNssMsgInfo.SetCameraCode(chDevCode.c_str());
	sendNssMsgInfo.SetDomainCode(chDomaCode.c_str());

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Record Policy", "SendCmd fail");
        return iRet;
    }

	CXml xmlRsp;
	if (!xmlRsp.Parse(strpRsp.c_str()))
	{
		BP_RUN_LOG_ERR(iRet, "Get Record Policy:Parse Result strpRsp failed", "NA");
		return iRet;
	}

	iRet = CRecordXMLProcess::GetRecordPolicyParseRSP(pRecordPolicy,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Record Policy:Result XML Parse Failed","failed");
		return iRet;
	}

	return iRet;
}

//设置录像策略
IVS_INT32 CRecordService::SetRecordPolicy(const IVS_CHAR* pCameraCode,const IVS_RECORD_POLICY_TIME* pRecordPolicy)const
{
	CHECK_POINTER(pRecordPolicy,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//解析pCameraCode
	string chDevCode = "";
	string chDomaCode = "";
	IVS_INT32 iRet =CToolsHelp::SplitCameraDomainCode(pCameraCode,chDevCode,chDomaCode);

	CXml xmlReq;
	iRet = CRecordXMLProcess::SetRecordPolicyGetXML(pRecordPolicy,chDevCode.c_str(),chDomaCode.c_str(),xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Set Record Policy","failed");
		return iRet;
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR *pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_RECORD_POLICY_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	sendNssMsgInfo.SetCameraCode(chDevCode.c_str());
	sendNssMsgInfo.SetDomainCode(chDomaCode.c_str());

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}

//设置外域摄像机容灾录像策略
IVS_INT32 CRecordService::SetExDomainRecordPolicy(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	CXml xmlReq;	
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (xmlReq.FindElemEx("Content"))
	{
		xmlReq.IntoElem();
		if (!xmlReq.FindElem("CameraCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(CameraCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xmlReq.GetElemValue();
		iRet = CXmlProcess::ParseDevCode(pElemValue, szCameraCode, szDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Set SetExDomain Record Policy Req XML CameraCode Value", "Parse DevCode failed");
			return iRet;
		}
		xmlReq.ModifyElemValue(szCameraCode);
		xmlReq.OutOfElem();
	}
	xmlReq.GetRootPos();
	if (!xmlReq.MkAndSetElemValueEx("Content/ExDomainCode", szDomainCode))
	{
		BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"Set ExDomain Record Policy:Set CameraCode Value", "add Camera ExDomainCode failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_EX_DOMAIN_RECORD_POLICY_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	//sendNssMsgInfo.SetCameraCode(chDevCode.c_str());
	//sendNssMsgInfo.SetDomainCode(chDomaCode.c_str());

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}

//查询外域摄像机容灾录像策略
IVS_INT32 CRecordService::GetExDomainRecordPolicy(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	CXml xmlReq;	
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (xmlReq.FindElemEx("Content"))
	{
		xmlReq.IntoElem();
		if (!xmlReq.FindElem("CameraCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(CameraCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xmlReq.GetElemValue();
		iRet = CXmlProcess::ParseDevCode(pElemValue, szCameraCode, szDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get ExDomain Record Policy Req XML CameraCode Value", "Parse DevCode failed");
			return iRet;
		}
		xmlReq.ModifyElemValue(szCameraCode);
		xmlReq.OutOfElem();
	}

    // 添加外域编码
    xmlReq.GetRootPos();
    if (!xmlReq.MkAndSetElemValueEx("Content/ExDomainCode", szDomainCode))
    {
        BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"Get ExDomain Record Policy:Set CameraCode Value", "add Camera ExDomainCode failed");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	unsigned int xmlLen = 0;
	const IVS_CHAR *pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_EX_DOMAIN_RECORD_POLICY_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	//sendNssMsgInfo.SetCameraCode(chDevCode.c_str());
	//sendNssMsgInfo.SetDomainCode(chDomaCode.c_str());

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());
	if (xmlRsp.FindElemEx("Content/CameraCode"))
	{

			(void)xmlRsp.IntoElem();
			const char* szElemValue = NULL;
			char cCameraCode[IVS_DEV_CODE_LEN+1]={0};
			char cDomainCode[IVS_DEV_CODE_LEN+1]={0};
			if (!xmlRsp.FindElem("ExDomainCode"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(ExDomainCode)", "NA");
				return IVS_XML_INVALID;
			}
			GET_ELEM_VALUE_CHAR("ExDomainCode",szElemValue,cDomainCode,IVS_DEV_CODE_LEN,xmlRsp);
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
	}
	IVS_UINT32 uiXmlLen = 0;
    const IVS_CHAR* pXmlRsp = xmlRsp.GetXMLStream(uiXmlLen);
    if (NULL != pXmlRsp)
    {
        strpRsp = pXmlRsp;
    }

	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strpRsp.size() + 1);
	if (NULL == *pRspXml)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(*pRspXml, 0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(*pRspXml, strpRsp.size() + 1, strpRsp.c_str(), strpRsp.size()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get ExDomain Record Policy", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return iRet;
}




// 查询开启预录的摄像机列表
IVS_INT32 CRecordService::GetPrerecordCameraList(IVS_CHAR** pRspXml)
{
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode;
	IVS_CHAR chCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	CCmd *pCmd = NULL;
	CmdMap cmdMap;
	CXml xmlRsp;
	std::string strpRsp;
	std::string strResult = "<Content><CameraList>";
	const IVS_CHAR* szElemValue = NULL;

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_PRERECORD_CAMERA_LIST_REQ);

	// 取得用户拥有设备的域列表
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	m_pUserMgr->GetDeviceMgr().m_vecCameraDomain.clear();
	IVS_INT32 iRet = m_pUserMgr->GetDeviceMgr().GetDomainListByUserID(m_pUserMgr->GetDeviceMgr().m_vecCameraDomain);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Prerecord CameraList:GetDomainList By UserID return failed", "NA");
		return iRet;
	}

	IVS_UINT32 iSize = m_pUserMgr->GetDeviceMgr().m_vecCameraDomain.size();	
	IVS_UINT32 xmlLen = 0;

    std::map<std::string, std::string> transIDDomainCodeMap;
	// 遍历域编码列表
	for(IVS_UINT32 i=0;i<iSize;i++)
	{
		strDomainCode = m_pUserMgr->GetDeviceMgr().m_vecCameraDomain.at(i);	

		//构建请求消息XML
		CXml xmlReq;
		(void)xmlReq.AddDeclaration("1.0","UTF-8","");
		(void)xmlReq.AddElem("Content");
		(void)xmlReq.AddChildElem("DomainCode");
		(void)xmlReq.IntoElem();
		(void)xmlReq.SetElemValue(strDomainCode.c_str());
		const IVS_CHAR* pPrerecordReq = xmlReq.GetXMLStream(xmlLen);
        if (NULL == pPrerecordReq)
        {
            return IVS_XML_INVALID;
        }

		sendNssMsgInfo.SetReqData(pPrerecordReq); 
		sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
		pCmd = m_pUserMgr->BuildCmd(sendNssMsgInfo);
		(void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
        (void)transIDDomainCodeMap.insert(std::make_pair(pCmd->GetTransID(), strDomainCode));
	}	

	if (cmdMap.empty())
	{
		return IVS_SUCCEED;
	}
	iRet = CDispatcher::instance().dispatcherSyncMsgOutBatch(cmdMap, 30);


	IVS_INT32 iEachRet = 0;
	IVS_UINT32 iFailCount = 0;
	CmdMapIterator cmdMapIter = cmdMap.begin();
	CmdMapIterator cmdMapIterEnd = cmdMap.end();
	for (; cmdMapIter != cmdMapIterEnd; cmdMapIter++)
	{
		CCmd *pCmdRsp = cmdMapIter->second; //lint !e612
        std::map<std::string, std::string>::iterator itor = transIDDomainCodeMap.find(cmdMapIter->first);
        if (transIDDomainCodeMap.end() == itor)
        {
            BP_RUN_LOG_ERR(IVS_SMU_GET_PRERECORD_CAMERA_LIST_FAIL, "Get Prerecord CameraList.", "fail to find domain code by trans id.");
            continue;
        }
        strDomainCode = itor->second;

		if(NULL != pCmdRsp)
		{
			IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iEachRet);
			if (IVS_SMU_GET_PRERECORD_CAMERA_LIST_TIMEOUT == iEachRet || IVS_SMU_GET_PRERECORD_CAMERA_LIST_FAIL == iEachRet
				|| IVS_SMU_GET_PRERECORD_CAMERA_LIST_PART_FAIL == iEachRet)
			{
				iFailCount++;
				iRet = IVS_SMU_GET_PRERECORD_CAMERA_LIST_PART_FAIL;
				BP_RUN_LOG_ERR(iEachRet, "Get Prerecord CameraList", "Parse Cmd 2XML failed,rspCode = %d", iEachRet);
			}

			// 解析响应的xml
			if (NULL != pRsp && xmlRsp.Parse(pRsp))
			{
				if(xmlRsp.FindElemEx("Content/CameraList"))
				{
					do 
					{
						xmlRsp.IntoElem();
						(void)xmlRsp.IntoElem();
						GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,chCameraCode,IVS_DEV_CODE_LEN,xmlRsp);
						std::string strCameraDomainCode;
						strCameraDomainCode.append(chCameraCode).append("#").append(strDomainCode);
						strResult += "<CameraInfo>";
						strResult += "<CameraCode>";
						strResult += strCameraDomainCode;
						strResult += "</CameraCode>";
						strResult += "</CameraInfo>";

						xmlRsp.OutOfElem();
					} while(xmlRsp.NextElem());
				}
			}
		}
	}

	strResult += "</CameraList></Content>";
	if(iSize == iFailCount)
	{
		iRet = IVS_SMU_GET_PRERECORD_CAMERA_LIST_FAIL;
		BP_RUN_LOG_ERR(iRet, "Get PreRecord CameraList", "Get Prerecord CameraList All Domains failed.");
		return iRet;
	}
	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strResult.size() + 1);
	if (NULL == *pRspXml)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(*pRspXml, 0, strResult.size() + 1);
	if (!CToolsHelp::Memcpy(*pRspXml, strResult.size() + 1, strResult.c_str(), strResult.size()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get PreRecord CameraList", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return iRet;
}

IVS_INT32 CRecordService::AddExDomainRecordPlan( IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize ) const
{
    CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
    CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
    CHECK_POINTER(pResultList, IVS_PARA_INVALID);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("Add ExDomain Record Plan.");
    //数据结构拼装XML并转换为const IVS_CHAR *
    CXml xmlReq;
    IVS_INT32 iRet = CRecordXMLProcess::AddOrModExDomainRecordPlanGetXML(uiDeviceNum,pDeviceList, pRecordPlan, xmlReq);
   
	unsigned int xmlLen = 0;
    const IVS_CHAR* pReqPlan = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);
    //发送nss协议
    CSendNssMsgInfo sendNssMsgInfo;	
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_ALLOC_EX_DOMAIN_RECORD_PLAN_REQ);
    sendNssMsgInfo.SetReqData(pReqPlan); 
    //sendNssMsgInfo.SetCameraCode(chDevCode);
    //sendNssMsgInfo.SetDomainCode(chDomaCode);

    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
    if(IVS_SUCCEED == iRet)
    {
        BP_RUN_LOG_INF("Add ExDomain Record Plan", "ok", "Add ExDomain Record Plan Success.pResultList size is 0.");
        return iRet;
    }

    CXml xmlRsp;
    (void)xmlRsp.Parse(strpRsp.c_str());
	IVS_UINT32 uiRecordNum = 0;
    IVS_INT32 iParseRSPRet = CRecordXMLProcess::RecordPlanParseRSP(uiDeviceNum,pDeviceList,pResultList,uiResultBufferSize,xmlRsp,uiRecordNum);

    BP_RUN_LOG_INF("Add ExDomain Record Plan", "ok", "msg have Parsed");
    return iRet;
}//lint !e1762 !e818 !e529

IVS_INT32 CRecordService::ModExDomainRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
    const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const
{
    CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
    CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
    CHECK_POINTER(pResultList, IVS_PARA_INVALID);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("Mod ExDomain Record Plan.");
    //数据结构拼装XML并转换为const IVS_CHAR *
    CXml xmlReq;
    IVS_INT32 iRet = CRecordXMLProcess::AddOrModExDomainRecordPlanGetXML(uiDeviceNum,pDeviceList, pRecordPlan, xmlReq); //CRecordXMLProcess::AddOrModRecordPlanGetXML(uiDeviceNum,pDeviceList, pRecordPlan, xmlReq);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
    (IVS_VOID)CXmlProcess::ParseDevCode(pDeviceList->cDevCode, chDevCode, chDomaCode);
    //// 添加外域编码
    //xmlReq.GetRootPos();
    //if (!xmlReq.MkAndSetElemValueEx("Content/ExDomainCode", chDomaCode))
    //{
    //    BP_RUN_LOG_ERR( IVS_SDK_RET_INTRINSIC_PTR_ERROR ,"Set CameraCode Value", "add Camera ExDomainCode failed");
    //    return IVS_SDK_RET_INTRINSIC_PTR_ERROR;
    //}

    unsigned int xmlLen = 0;
    const IVS_CHAR* pReqPlan = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

    //发送nss协议
    CSendNssMsgInfo sendNssMsgInfo;	
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_MOD_EX_DOMAIN_RECORD_PLAN_REQ);
    sendNssMsgInfo.SetReqData(pReqPlan); 
    //sendNssMsgInfo.SetCameraCode(chDevCode);
    //sendNssMsgInfo.SetDomainCode(chDomaCode);

    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
    if(IVS_SUCCEED == iRet)
    {
        BP_RUN_LOG_INF("Mod ExDomain Record Plan", "ok", "Mod ExDomain Record Plan Success.pResultList size is 0.");
        return iRet;
    }

    CXml xmlRsp;
    (void)xmlRsp.Parse(strpRsp.c_str());
	IVS_UINT32 uiRecordNum = 0;
    IVS_INT32 iParseRSPRet = CRecordXMLProcess::RecordPlanParseRSP(uiDeviceNum,pDeviceList,pResultList,uiResultBufferSize,xmlRsp,uiRecordNum);

    BP_RUN_LOG_INF("Mod ExDomain Record Plan", "ok", "msg have Parsed");
    return iRet;
}//lint !e1762 !e818 !e529

IVS_INT32 CRecordService::DelExDomainRecordPlan( IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList, IVS_UINT32 uiRecordMethod,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize ) const
{
    CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
    CHECK_POINTER(pResultList, IVS_PARA_INVALID);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("Del ExDomain Record Plan.");
    //数据结构拼装XML并转换为const IVS_CHAR *
    CXml xmlReq;
    IVS_INT32 iRet =CRecordXMLProcess::DelExDomainRecordPlanXML(uiDeviceNum,pDeviceList, uiRecordMethod, xmlReq); //CRecordXMLProcess::DelRecordPlanGetXML(uiDeviceNum,pDeviceList, uiRecordMethod, xmlReq);

    IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
    (IVS_VOID)CXmlProcess::ParseDevCode(pDeviceList->cDevCode, chDevCode, chDomaCode);

    //// 添加外域编码
    //xmlReq.GetRootPos();
    //if (!xmlReq.MkAndSetElemValueEx("Content/ExDomainCode", chDomaCode))
    //{
    //    BP_RUN_LOG_ERR( IVS_SDK_RET_INTRINSIC_PTR_ERROR ,"Set CameraCode Value", "add Camera ExDomainCode failed");
    //    return IVS_SDK_RET_INTRINSIC_PTR_ERROR;
    //}

    unsigned int xmlLen = 0;
    const IVS_CHAR * pReqPlan = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

    //发送nss协议
    CSendNssMsgInfo sendNssMsgInfo;	
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_DEL_EX_DOMAIN_RECORD_PLAN_REQ);
    sendNssMsgInfo.SetReqData(pReqPlan); 
    //sendNssMsgInfo.SetCameraCode(chDevCode);
    //sendNssMsgInfo.SetDomainCode(chDomaCode);

    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
    if(IVS_SUCCEED == iRet)
    {
        BP_RUN_LOG_INF("Delete Platform Record Plan", "ok", "Delete Platform RecordPlan Success.pResultList size is 0.");
        return iRet;
    }

    // 将xml转化成结构体返回
    CXml xmlRsp;
    (void)xmlRsp.Parse(strpRsp.c_str());
	IVS_UINT32 uiRecordNum = 0;
    IVS_INT32 iParseRSPRet = CRecordXMLProcess::RecordPlanParseRSP(uiDeviceNum,pDeviceList,pResultList,uiResultBufferSize,xmlRsp,uiRecordNum);

    BP_RUN_LOG_INF("Delete Record Plan", "ok", "msg have Parsed");
    return iRet;
}//lint !e1762 !e818 !e529

IVS_INT32 CRecordService::GetExDomainRecordPlan( const char* pCameraCode,IVS_UINT32 uiRecordMethod, IVS_RECORD_PLAN* pRecordPlan )
{
    CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
    CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("Get ExDomain Record Plan.");
    //数据结构拼装XML并转换为const IVS_CHAR *
    CXml xmlReq;
    IVS_INT32 iRet = CRecordXMLProcess::GetRecordPlanGetXML(pCameraCode, uiRecordMethod, xmlReq);

    IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
    (IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

    // 添加外域编码
    xmlReq.GetRootPos();
    if (!xmlReq.MkAndSetElemValueEx("Content/ExDomainCode", chDomaCode))
    {
        BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"Set CameraCode Value", "add Camera ExDomainCode failed");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    unsigned int xmlLen = 0;
    const IVS_CHAR * pReqPlan = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

    //发送nss协议
    CSendNssMsgInfo sendNssMsgInfo;	
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_GET_EX_DOMAIN_RECORD_PLAN_REQ);
    sendNssMsgInfo.SetReqData(pReqPlan); 
    //sendNssMsgInfo.SetCameraCode(chDevCode);
    //sendNssMsgInfo.SetDomainCode(chDomaCode);

    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_INF("Get ExDomain Record Plan", "ok", "Get ExDomain Record Plan Success.pResultList size is 0.");
        return iRet;
    }

    // 将xml转化成结构体返回
    CXml xmlRsp;
    (void)xmlRsp.Parse(strpRsp.c_str());
    IVS_INT32 iParseRSPRet = CRecordXMLProcess::GetRecordPlanParseRSP(pRecordPlan,xmlRsp);

    BP_RUN_LOG_INF("Get ExDomain Record Plan", "ok", "msg have Parsed");
    return iRet;
}//lint !e1762 !e818 !e529
