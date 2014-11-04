#include "RecordPlanMgr.h"
#include "UserMgr.h"
//#include "bp_log.h"
//#include "nss_mt.h"
#include "NSSOperator.h"
#include "RecordXMLProcess.h"
#include "IVS_Trace.h"

CRecordPlanMgr::CRecordPlanMgr(void)
{
	m_UserMgr = NULL;
}


CRecordPlanMgr::~CRecordPlanMgr(void)
{
	m_UserMgr = NULL;
}

void CRecordPlanMgr::SetUserMgr( CUserMgr *pUserMgr)
{
	m_UserMgr = pUserMgr;
}

//添加平台录像计划;
IVS_INT32 CRecordPlanMgr::AddRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
	const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const
{
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
	CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiReqBufSize = sizeof(IVS_RESULT_LIST) + (uiDeviceNum-1)*sizeof(IVS_RESULT_INFO);
	if (uiReqBufSize > uiResultBufferSize)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Add Record Plan", "Invalid DeviceNum or BufferSize");
		return IVS_PARA_INVALID;
	}

	if ((RECORD_PLAN_ALLDAY < pRecordPlan->uiRecordPlanType) ||
		(PLAN_WEEK == pRecordPlan->uiRecordPlanType && 7 < pRecordPlan->uiPlanInfoNum) ||
		(PLAN_DAY == pRecordPlan->uiRecordPlanType && 1 != pRecordPlan->uiPlanInfoNum) ||
		(PLAN_ALLDAY == pRecordPlan->uiRecordPlanType && 0 != pRecordPlan->uiPlanInfoNum))
	{
		pResultList->uiNum = uiDeviceNum;
		for (IVS_UINT32 i=0; i<pResultList->uiNum; i++)
		{
			pResultList->stInfo[i].uiSequence = i;
			pResultList->stInfo[i].iResult = IVS_PARA_INVALID;
			strncpy(pResultList->stInfo[i].cCode, pDeviceList[i].cDevCode, IVS_DEV_CODE_LEN);
		}
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Add Record Plan", "Invalid PlanType or PlanInfoNum");
		return IVS_PARA_INVALID;
	}

	DeviceCodeMap deviceCodeMap;
	std::string strSendDomainCode;
	CmdMap cmdMap;
	CCmd *pCmd = NULL;
	IVS_INT32 iRet = m_UserMgr->RecollectCameraDomainCodes2Map(uiDeviceNum,pDeviceList,deviceCodeMap);

	DeviceCodeMapIterator deviceCodeMapIter = deviceCodeMap.begin();
	DeviceCodeMapIterator deviceCodeMapIterEnd = deviceCodeMap.end();
	for (; deviceCodeMapIter != deviceCodeMapIterEnd; deviceCodeMapIter++)
	{
		DeviceCodeList devList = static_cast<DeviceCodeList>(deviceCodeMapIter->second); //lint !e612 !e69
		strSendDomainCode = deviceCodeMapIter->first;//lint !e612

		//数据结构拼装XML并转换为const IVS_CHAR *
		CXml addRecordPlanXmlReq;
		iRet = CRecordXMLProcess::AddOrModRecordPlanGetXML(strSendDomainCode,devList, pRecordPlan, addRecordPlanXmlReq);
		if (IVS_SUCCEED != iRet)
		{
			pResultList->uiNum = uiDeviceNum;
			for (IVS_UINT32 i=0; i<pResultList->uiNum; i++)
			{
				pResultList->stInfo[i].uiSequence = i;
				pResultList->stInfo[i].iResult = iRet;
				strncpy(pResultList->stInfo[i].cCode, pDeviceList[i].cDevCode, IVS_DEV_CODE_LEN);
			}
			BP_RUN_LOG_ERR(iRet, "Add Record Plan", "Add Or Mod Record Plan Get XML failed");
			return iRet;
		}
		unsigned int xmlLen = 0;
		const IVS_CHAR* pReqPlan = addRecordPlanXmlReq.GetXMLStream(xmlLen);
		CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);
		BP_DBG_LOG("ReqPlan:%s\n", pReqPlan);

		//构建发送nss协议
		CSendNssMsgInfo sendNssMsgInfo;	
		sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
		sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
		sendNssMsgInfo.SetReqID(NSS_MOD_RECORD_PLAN_REQ);
		sendNssMsgInfo.SetReqData(pReqPlan); 
		sendNssMsgInfo.SetDomainCode(strSendDomainCode.c_str());
		pCmd = m_UserMgr->BuildCmd(sendNssMsgInfo);
		if (NULL == pCmd)
		{
			BP_RUN_LOG_INF("build cmd error","cmd req id=%d, domain code=%s",NSS_MOD_RECORD_PLAN_REQ,strSendDomainCode.c_str());
			continue;
		}
		(void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
	}
	if (cmdMap.empty())
	{
		return IVS_SUCCEED;
	}

	IVS_UINT32 uiTimeOut =  CSDKConfig::instance().GetTimeOutValue();
	std::string strpRsp;
	iRet = CDispatcher::instance().dispatcherSyncMsgOutBatch(cmdMap, uiTimeOut);

	//IVS_UINT32 iSize = deviceCodeMap.size();	
	IVS_INT32 iEachRet = 0;
	IVS_UINT32 iFailCount = 0;
	IVS_UINT32 uiRecordNum = 0;
	CmdMapIterator cmdMapIter = cmdMap.begin();
	for (; cmdMapIter != cmdMap.end(); cmdMapIter++)
	{
		CCmd *pCmdRsp = cmdMapIter->second; //lint !e612
		if(NULL != pCmdRsp)
		{
			IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iEachRet);
			if (IVS_SUCCEED != iEachRet)
			{
				iFailCount++;
				iRet = iEachRet;
				BP_RUN_LOG_ERR(iEachRet, "Add Record Plan", "Parse Cmd 2XML failed,rspCode = %d", iEachRet);

				// 解析响应的xml
				CXml xmlRsp;
				if (NULL != pRsp && xmlRsp.Parse(pRsp))
				{
					iEachRet = CRecordXMLProcess::RecordPlanParseRSP(uiDeviceNum,pDeviceList,pResultList,uiResultBufferSize,xmlRsp,uiRecordNum);
				}
			}
		}
	}


	if(iFailCount >0)
	{
		//iRet = IVS_FAIL;
		BP_RUN_LOG_ERR(iRet, "Add Platform Record Plan", "Add Platform Record Plan in some Domains failed.");
		return iRet;
	}

	BP_RUN_LOG_INF("Add Platform Record Plan", "ok", "msg have Parsed");
	return IVS_SUCCEED;
}//lint !e529 

 //修改平台录像计划;
IVS_INT32 CRecordPlanMgr::ModifyRecordPlan(IVS_UINT32 uiDeviceNum, const IVS_DEVICE_CODE* pDeviceList,
	const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const
{
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
	CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiReqBufSize = sizeof(IVS_RESULT_LIST) + (uiDeviceNum-1)*sizeof(IVS_RESULT_INFO);
	if (uiReqBufSize > uiResultBufferSize)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Add Record Plan", "Invalid DeviceNum or BufferSize");
		return IVS_PARA_INVALID;
	}

	if ((RECORD_PLAN_ALLDAY < pRecordPlan->uiRecordPlanType) ||
		(PLAN_WEEK == pRecordPlan->uiRecordPlanType && 7 < pRecordPlan->uiPlanInfoNum) ||
		(PLAN_DAY == pRecordPlan->uiRecordPlanType && 1 != pRecordPlan->uiPlanInfoNum) ||
		(PLAN_ALLDAY == pRecordPlan->uiRecordPlanType && 0 != pRecordPlan->uiPlanInfoNum))
	{
		pResultList->uiNum = uiDeviceNum;
		for (IVS_UINT32 i=0; i<pResultList->uiNum; i++)
		{
			pResultList->stInfo[i].uiSequence = i;
			pResultList->stInfo[i].iResult = IVS_PARA_INVALID;
			strncpy(pResultList->stInfo[i].cCode, pDeviceList[i].cDevCode, IVS_DEV_CODE_LEN);
		}
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Add Record Plan", "Invalid PlanType or PlanInfoNum");
		return IVS_PARA_INVALID;
	}

	DeviceCodeMap deviceCodeMap;
	std::string strSendDomainCode;
	CCmd *pCmd = NULL;
	CmdMap cmdMap;
	IVS_INT32 iRet = m_UserMgr->RecollectCameraDomainCodes2Map(uiDeviceNum,pDeviceList,deviceCodeMap);
	IVS_UINT32 uiTimeOut =  CSDKConfig::instance().GetTimeOutValue();

	DeviceCodeMapIterator deviceCodeMapIter = deviceCodeMap.begin();
	DeviceCodeMapIterator deviceCodeMapIterEnd = deviceCodeMap.end();
	for (; deviceCodeMapIter != deviceCodeMapIterEnd; deviceCodeMapIter++)
	{
		DeviceCodeList devList = static_cast<DeviceCodeList>(deviceCodeMapIter->second); //lint !e612 !e69
		strSendDomainCode = deviceCodeMapIter->first;//lint !e612

		//数据结构拼装XML并转换为const IVS_CHAR *
		CXml modRecordPlanXmlReq;
		iRet = CRecordXMLProcess::AddOrModRecordPlanGetXML(strSendDomainCode,devList, pRecordPlan, modRecordPlanXmlReq);
		if (IVS_SUCCEED != iRet)
		{
			pResultList->uiNum = uiDeviceNum;
			for (IVS_UINT32 i=0; i<pResultList->uiNum; i++)
			{
				pResultList->stInfo[i].uiSequence = i;
				pResultList->stInfo[i].iResult = iRet;
				strncpy(pResultList->stInfo[i].cCode, pDeviceList[i].cDevCode, IVS_DEV_CODE_LEN);
			}
			BP_RUN_LOG_ERR(iRet, "Add Record Plan", "Add Or Mod Record Plan Get XML failed");
			return iRet;
		}
		unsigned int xmlLen = 0;
		const IVS_CHAR* pReqPlan = modRecordPlanXmlReq.GetXMLStream(xmlLen);
		CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);
		BP_DBG_LOG("ReqPlan:%s\n", pReqPlan);

		//构建发送nss协议
		CSendNssMsgInfo sendNssMsgInfo;	
		sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
		sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
		sendNssMsgInfo.SetReqID(NSS_MOD_RECORD_PLAN_REQ);
		sendNssMsgInfo.SetReqData(pReqPlan); 
		sendNssMsgInfo.SetDomainCode(strSendDomainCode.c_str());
		pCmd = m_UserMgr->BuildCmd(sendNssMsgInfo);
		if (NULL == pCmd)
		{
			BP_RUN_LOG_INF("build cmd error","cmd req id=%d, domain code=%s",NSS_MOD_RECORD_PLAN_REQ,strSendDomainCode.c_str());
			continue;
		}
		(void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
	}
	if (cmdMap.empty())
	{
		return IVS_SUCCEED;
	}

	std::string strpRsp;
	iRet = CDispatcher::instance().dispatcherSyncMsgOutBatch(cmdMap, uiTimeOut);

	//IVS_UINT32 iSize = deviceCodeMap.size();	
	IVS_INT32 iEachRet = 0;
	IVS_UINT32 iFailCount = 0;
	IVS_UINT32 uiRecordNum = 0;
	CmdMapIterator cmdMapIter = cmdMap.begin();
	CmdMapIterator cmdMapIterEnd = cmdMap.end();
	for (; cmdMapIter != cmdMapIterEnd; cmdMapIter++)
	{
		CCmd *pCmdRsp = cmdMapIter->second; //lint !e612
		if(NULL != pCmdRsp)
		{
			IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iEachRet);
			if (IVS_SUCCEED != iEachRet)
			{
				iFailCount++;
				iRet = iEachRet;
				BP_RUN_LOG_ERR(iEachRet, "Modify Record Plan", "Parse Cmd 2XML failed,rspCode = %d", iEachRet);

				// 解析响应的xml
				CXml xmlRsp;
				if (NULL != pRsp && xmlRsp.Parse(pRsp))
				{
					iEachRet = CRecordXMLProcess::RecordPlanParseRSP(uiDeviceNum,pDeviceList,pResultList,uiResultBufferSize,xmlRsp,uiRecordNum);
				}
			}
		}
	}


	if(iFailCount >0)
	{
		//iRet = IVS_FAIL;
		BP_RUN_LOG_ERR(iRet, "Modify Platform Record Plan", "Modify Platform Record Plan in some Domains failed.");
		return iRet;
	}

	BP_RUN_LOG_INF("Modify Platform Record Plan", "ok", "All succeed.");
	return IVS_SUCCEED;
}//lint !e529
 //删除平台录像计划;
IVS_INT32 CRecordPlanMgr::DeleteRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
	IVS_UINT32 uiRecordMethod,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const
 {
	 CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	 CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	 CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
 	
	DeviceCodeMap deviceCodeMap;
	std::string strSendDomainCode;
	CCmd *pCmd = NULL;
	CmdMap cmdMap;
	IVS_INT32 iRet = m_UserMgr->RecollectCameraDomainCodes2Map(uiDeviceNum,pDeviceList,deviceCodeMap);

	DeviceCodeMapIterator deviceCodeMapIter = deviceCodeMap.begin();
	DeviceCodeMapIterator deviceCodeMapIterEnd = deviceCodeMap.end();
	for (; deviceCodeMapIter != deviceCodeMapIterEnd; deviceCodeMapIter++)
	{
		DeviceCodeList devList = static_cast<DeviceCodeList>(deviceCodeMapIter->second); //lint !e612 !e69
		strSendDomainCode = deviceCodeMapIter->first;//lint !e612

		//数据结构拼装XML并转换为const IVS_CHAR *
		CXml delRecordPlanXmlReq;
		iRet = CRecordXMLProcess::DelRecordPlanGetXML(strSendDomainCode,devList, uiRecordMethod, delRecordPlanXmlReq);
		unsigned int xmlLen = 0;
		const IVS_CHAR* pReqPlan = delRecordPlanXmlReq.GetXMLStream(xmlLen);
		CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

		//构建发送nss协议
		CSendNssMsgInfo sendNssMsgInfo;	
		sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
		sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
		sendNssMsgInfo.SetReqID(NSS_DEL_RECORD_PLAN_REQ);
		sendNssMsgInfo.SetReqData(pReqPlan); 
		sendNssMsgInfo.SetDomainCode(strSendDomainCode.c_str());
		pCmd = m_UserMgr->BuildCmd(sendNssMsgInfo);
		if (NULL == pCmd)
		{
			BP_RUN_LOG_INF("build cmd error","cmd req id=%d, domain code=%s",NSS_DEL_RECORD_PLAN_REQ,strSendDomainCode.c_str());
			continue;
		}
		(void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
	}
	if (cmdMap.empty())
	{
		return IVS_SUCCEED;
	}

	std::string strpRsp;
	IVS_UINT32 uiTimeOut =  CSDKConfig::instance().GetTimeOutValue();
	iRet = CDispatcher::instance().dispatcherSyncMsgOutBatch(cmdMap, uiTimeOut);

	//IVS_UINT32 iSize = deviceCodeMap.size();	
	IVS_INT32 iEachRet = 0;
	IVS_UINT32 iFailCount = 0;
	IVS_UINT32 uiRecordNum = 0;
	CmdMapIterator cmdMapIter;
	for (cmdMapIter = cmdMap.begin(); cmdMapIter != cmdMap.end(); cmdMapIter++)
	{
		CCmd *pCmdRsp = cmdMapIter->second; //lint !e612
		if(NULL != pCmdRsp)
		{
			IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iEachRet);
			if (IVS_SUCCEED != iEachRet)
			{
				iFailCount++;
				iRet = iEachRet;
				BP_RUN_LOG_ERR(iEachRet, "Delete Record Plan", "Parse Cmd 2XML failed,rspCode = %d", iEachRet);

				// 解析响应的xml
				CXml xmlRsp;
				if (NULL != pRsp && xmlRsp.Parse(pRsp))
				{
					iEachRet = CRecordXMLProcess::RecordPlanParseRSP(uiDeviceNum,pDeviceList,pResultList,uiResultBufferSize,xmlRsp,uiRecordNum);
				}
			}
		}
	}

	if(iFailCount >0)
	{
		//iRet = IVS_FAIL;
		BP_RUN_LOG_ERR(iRet, "Delete Platform Record Plan", "Delete Platform Record Plan in some Domains failed.");
		return iRet;
	}

	BP_RUN_LOG_INF("Delete Record Plan", "ok", "msg have Parsed");
 	return IVS_SUCCEED;
 }//lint !e529
 
 //获取平台录像计划;
 IVS_INT32 CRecordPlanMgr::GetRecordPlan(const char* pCameraCode,IVS_UINT32 uiRecordMethod,
	 IVS_RECORD_PLAN* pRecordPlan)
 {
	 CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	 CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
	 CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");
	 //数据结构拼装XML并转换为const IVS_CHAR *
	 CXml xmlReq;
	 IVS_INT32 iRet = CRecordXMLProcess::GetRecordPlanGetXML(pCameraCode, uiRecordMethod, xmlReq);

	 unsigned int xmlLen = 0;
	 const IVS_CHAR * pReqPlan = xmlReq.GetXMLStream(xmlLen);
	 CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

	 IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
	 memset(chDevCode, 0, sizeof(chDevCode));

	 IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	 memset(chDomaCode, 0, sizeof(chDomaCode));

	 (IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	 //发送nss协议
	 CSendNssMsgInfo sendNssMsgInfo;	
	 sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	 sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	 sendNssMsgInfo.SetReqID(NSS_GET_RECORD_PLAN_REQ);
	 sendNssMsgInfo.SetReqData(pReqPlan); 
	 sendNssMsgInfo.SetCameraCode(chDevCode);
	 sendNssMsgInfo.SetDomainCode(chDomaCode);

	 std::string strpRsp;
	 IVS_INT32 iNeedRedirect = IVS_FAIL;
	 iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	 if(IVS_SUCCEED != iRet)
	 {
		 BP_RUN_LOG_INF("Get Platform Record Plan", "ok", "Get Platform RecordPlan Success.pResultList size is 0.");
		 return iRet;
	 }

	 // 将xml转化成结构体返回
	 CXml xmlRsp;
	 (void)xmlRsp.Parse(strpRsp.c_str());
	 IVS_INT32 iParseRSPRet = CRecordXMLProcess::GetRecordPlanParseRSP(pRecordPlan,xmlRsp);

	 BP_RUN_LOG_INF("Get Record Plan", "ok", "msg have Parsed");
	 return iRet;
 }//lint !e1762 !e818 !e529

//设置开启、关闭水印校验
IVS_INT32 CRecordPlanMgr::SetWaterMarkState(IVS_INT32 /*iHandle*/,IVS_BOOL /*bIsStart*/)const
{

	return IVS_SUCCEED;

}

// 查询摄像机计划设置信息
IVS_INT32 CRecordPlanMgr::GetCameraPlanInfo(IVS_UINT32 uiPlanType, IVS_CHAR** pRspXml)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;
    std::vector<std::string> domainCodeList;
    domainCodeList.clear();
    iRet = m_UserMgr->GetDeviceMgr().GetDomainListByUserID(domainCodeList);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Domain List By User ID","Failed");
        return iRet;
    }

    CmdMap cmdMap;

    std::map<std::string, std::string> domainCodeMap;
    std::map<std::string, std::string>::iterator domainCodeMapIter;
    IVS_UINT32 iSize = domainCodeList.size();
    for(IVS_UINT32 i = 0; i < iSize; i++)
    {
        std::string strDomainCode = domainCodeList.at(i);

        CXml xmlReq;
        CRecordXMLProcess::GetCameraPlanInfoGetXML(uiPlanType, strDomainCode.c_str(), xmlReq);
        IVS_UINT32 xmlLen = 0;
        const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
        CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

        // 构造带域的请求消息，并发送
        CSendNssMsgInfo sendNssMsgInfo;
        sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
        sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
        sendNssMsgInfo.SetReqID(NSS_GET_CAMERA_PLAN_INFO_REQ);
        sendNssMsgInfo.SetReqData(pReq);
        sendNssMsgInfo.SetDomainCode(strDomainCode);

        CCmd *pCmd = m_UserMgr->BuildCmd(sendNssMsgInfo);
		if (NULL == pCmd)
		{
			BP_RUN_LOG_INF("build cmd error","cmd req id=%d, domain code=%s",NSS_GET_CAMERA_PLAN_INFO_REQ,strDomainCode.c_str());
			continue;
		}
        (void)domainCodeMap.insert(std::make_pair(pCmd->GetTransID(), strDomainCode));
        (void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
    }

    if (cmdMap.empty())
    {
        return IVS_SUCCEED;
    }
    IVS_UINT32 uiTimeout = CSDKConfig::instance().GetTimeOutValue();
    iRet = CDispatcher::instance().dispatcherSyncMsgOutBatch(cmdMap, uiTimeout);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Camera Plan Info fail","SendCmd to SMU Return Failed");
        return iRet;
    }

    std::map<std::string, PLAN_INFO_VEC> m_PlanMap;
    CmdMapIterator cmdMapIter;
    std::string strDomainCode;
    CCmd *pCmdRsp = NULL;
    for (cmdMapIter = cmdMap.begin(); cmdMapIter != cmdMap.end(); cmdMapIter++)
    {
        domainCodeMapIter = domainCodeMap.find(cmdMapIter->first);
        if (domainCodeMapIter != domainCodeMap.end())
        {
            strDomainCode = domainCodeMapIter->second;
        }
        if(strDomainCode.empty())
        {
            continue;
        }

        pCmdRsp = static_cast<CCmd *>(cmdMapIter->second);//lint !e63
        IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);

        if (NULL == pRsp)
        {
            continue;
        }
        CXml xmlRsp;
        if(!xmlRsp.Parse(pRsp))        
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Camera Plan Info", "xml parsing error");
            continue;
        }
        PLAN_INFO_VEC m_PlanList;
        m_PlanList.clear();
        iRet = CRecordXMLProcess::GetCameraPlanInfoParseXML(xmlRsp, m_PlanList);
        if(IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "Get Camera PlanInfo ParseXML", "xml parsing error");
            continue;
        }
        (void)m_PlanMap.insert(std::make_pair(strDomainCode, m_PlanList));
    }
    CXml rspXml;
    CRecordXMLProcess::GetCameraPlanInfoGetXml(m_PlanMap, rspXml);
    IVS_UINT32 iLen = 0;

    const IVS_CHAR* pRsq = rspXml.GetXMLStream(iLen);
    IVS_DELETE(*pRspXml, MUILI);
    CHECK_POINTER(pRsq, IVS_OPERATE_MEMORY_ERROR);
    IVS_CHAR* pResult = IVS_NEW(pResult, strlen(pRsq) + 1);
    memset(pResult, 0x0, strlen(pRsq) + 1);
    if (!CToolsHelp::Memcpy(pResult, strlen(pRsq) + 1, pRsq, strlen(pRsq) + 1))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pRsq to pResult failed", "NA");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    *pRspXml = pResult;

    return iRet;
}

