#include "RecordBookmark.h"
#include "UserMgr.h"
#include "NSSOperator.h"
#include "RecordXMLProcess.h"
#include "IVS_Trace.h"

CRecordBookmark::CRecordBookmark(void)
{
	m_UserMgr = NULL;
}


CRecordBookmark::~CRecordBookmark(void)
{
	m_UserMgr = NULL;
}

void CRecordBookmark::SetUserMgr( CUserMgr *pUserMgr)
{
	m_UserMgr = pUserMgr;
}

//新增录像书签;
IVS_INT32 CRecordBookmark::AddBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pBookmarkName, IVS_PARA_INVALID);
	CHECK_POINTER(pBookmarkTime, IVS_PARA_INVALID);
	CHECK_POINTER(uiBookmarkID, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;
	CXml xmlRsp;

	CUserInfo* userInfo = m_UserMgr->GetUserInfoEx();
	CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
	string userName = userInfo->GetLoginName();
	std::string strUserDomainCode;
	m_UserMgr->GetDomainCode(strUserDomainCode);
	IVS_INT32 iRet = CRecordXMLProcess::AddBookmarkGetXML(iRecordMethod,m_UserMgr->GetUserID(),strUserDomainCode.c_str(),userName.c_str(),pDomainCode,pNVRCode,pCameraCode,pBookmarkName, pBookmarkTime, xmlReq);
	unsigned int uiXmlLen = 0;
	const IVS_CHAR* pReqPlan = xmlReq.GetXMLStream(uiXmlLen);
	CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
	IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomainCode);
	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_RECORD_BOOKMARK_REQ);
	sendNssMsgInfo.SetReqData(pReqPlan); 
	sendNssMsgInfo.SetCameraCode(chDevCode);
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		sendNssMsgInfo.SetDomainCode(pDomainCode);
	}
	else
	{
		sendNssMsgInfo.SetDomainCode(chDomainCode);
	}

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if(IVS_SUCCEED == iRet && RECORD_METHOD_PLATFORM == iRecordMethod)
	{
		BP_RUN_LOG_INF("Add Book mark", "ok", "Add BookMark Success.");
		xmlRsp.Parse(strpRsp.c_str());
		if (!xmlRsp.FindElemEx("Content/RecordBookmark/BookmarkID"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Response XML", "xml.FindElem(Content/RecordBookmark/BookmarkID) is fail");
			return IVS_XML_INVALID;
		} 
		const char* szElemValue = NULL;
		GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkID",szElemValue, *uiBookmarkID,xmlRsp);
		return iRet;
	}

	BP_RUN_LOG_INF("Add Book mark", "ok", "Add BookMark Failed.");
	return iRet;
}

//修改录像书签;
IVS_INT32 CRecordBookmark::ModifyBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNewBookmarkName, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;

	CUserInfo* userInfo = m_UserMgr->GetUserInfoEx();
	CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
	string userName = userInfo->GetLoginName();
	std::string strUserDomainCode;
	m_UserMgr->GetDomainCode(strUserDomainCode);
	IVS_INT32 iRet = CRecordXMLProcess::ModifyBookmarkGetXML(iRecordMethod,m_UserMgr->GetUserID(),userName.c_str(),strUserDomainCode.c_str(),pDomainCode,pNVRCode,pCameraCode,uiBookmarkID, pNewBookmarkName, xmlReq);
	unsigned int xmlLen = 0;
	const IVS_CHAR* pModifyBookmarkReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pModifyBookmarkReq, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chCamCode[IVS_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chCamCode, chDomaCode);
	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_RECORD_BOOKMARK_REQ);
	sendNssMsgInfo.SetReqData(pModifyBookmarkReq); 
	sendNssMsgInfo.SetCameraCode(chCamCode);
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		sendNssMsgInfo.SetDomainCode(pDomainCode);
	}
	else
	{
		sendNssMsgInfo.SetDomainCode(chDomaCode);
	}

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("Modify Book mark", "ok", "Modify BookMark Success.");
		return iRet;
	}

	BP_RUN_LOG_INF("Modify Book mark", "ok", "Modify BookMark Failed.");
	return iRet;
}

//删除录像书签;
IVS_INT32 CRecordBookmark::DeleteBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;

	IVS_INT32 iRet = CRecordXMLProcess::DeleteBookmarkGetXML(iRecordMethod,pDomainCode,pNVRCode,pCameraCode,uiBookmarkID, xmlReq);
	unsigned int xmlLen = 0;
	const IVS_CHAR* pReqPlan = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chCameraCode, chDomaCode);
	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_RECORD_BOOKMARK_REQ);
	sendNssMsgInfo.SetReqData(pReqPlan); 
	sendNssMsgInfo.SetCameraCode(chCameraCode);
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		sendNssMsgInfo.SetDomainCode(pDomainCode);
	}
	else
	{
		sendNssMsgInfo.SetDomainCode(chDomaCode);
	}

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("Delete Book mark", "ok", "Delete BookMark Success.");
		return iRet;
	}

	BP_RUN_LOG_INF("Delete Book mark", "ok", "Delete BookMark Failed.");
	return iRet;
}

//查询录像书签列表;
IVS_INT32 CRecordBookmark::GetBookmarkList(IVS_INT32 iRecordMethod,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
	IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiBufSize)const
{
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBookmarkList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	//if(RECORD_METHOD_MBU == iRecordMethod)
	//{
	//	iRet = GetBookmarkListByBackupServer(pUnifiedQuery,pBookmarkList,uiBufSize);
	//	return iRet;
	//}

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
	memset(chDevCode,0,IVS_DEV_CODE_LEN+1);
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	memset(chDomaCode,0,IVS_DOMAIN_CODE_LEN+1);
	std::string strCameraCode = "";
	std::string strDomainCode = "";
	for (int i=0;i<pUnifiedQuery->iFieldNum;i++)
	{
		if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID)
		{
			(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, chDevCode, chDomaCode);
			strCameraCode = chDevCode;
			strDomainCode = chDomaCode;
			break;
		}
	}
	if ("" == strCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Bookmark List QueryField invalid,lack of CameraCode","failed");
		return IVS_PARA_INVALID;
	}

	CXml xmlReq;
	//拼装XML查询请求
	iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery, xmlReq, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get request xml failed","failed");
		return iRet;
	}

	std::string sRecordMethod = "RecordMethod";
	iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(sRecordMethod.c_str(), CToolsHelp::Int2Str(iRecordMethod).c_str(), xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_BOOKMARK_LIST_REQ);
	sendNssMsgInfo.SetReqData(pReq); 
	sendNssMsgInfo.SetCameraCode(chDevCode);

	std::string strLocalDoaminCode;
	m_UserMgr->GetDomainCode(strLocalDoaminCode);
	if (RECORD_METHOD_MBU == iRecordMethod)
	{
		sendNssMsgInfo.SetDomainCode(strLocalDoaminCode);
	}
	else
	{
		sendNssMsgInfo.SetDomainCode(chDomaCode);
	}

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
	if (IVS_SUCCEED == iNeedRedirect)
	{
		iRet = m_UserMgr->SendRedirectServe(sendNssMsgInfo, strpRsp);
	}
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Bookmark List", "SendCmd operation succeed,rspCode = %d", iRet);
		return iRet;
	}

	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());

	//解析查询请求XML
	if (IVS_SUCCEED != CRecordXMLProcess::ParseGetBookmarkList(xmlRsp, pBookmarkList, strDomainCode, uiBufSize))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse response xml failed","failed");
		return IVS_XML_INVALID;
	}	
	return iRet;
}

//查询录像书签列表(从备份服务器)
IVS_INT32 CRecordBookmark::GetBookmarkListByBackupServer(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
	IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiMaxBufSize)const
{
	CHECK_POINTER(pUnifiedQuery, IVS_PARA_INVALID);
	CHECK_POINTER(pBookmarkList, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	IVS_UINT32 xmlLen = 0;
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	std::string strCameraCode = "";
	IVS_INT32 i = 0;
	IVS_UINT32 j = 0;
	for (i=0;i<pUnifiedQuery->iFieldNum;i++)
	{
		if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID)
		{
			(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, chDevCode, chDomaCode);
			strCameraCode = chDevCode;
			break;
		}
	}
	if ("" == strCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Bookmark List By BackupServer QueryField invalid,lack of CameraCode","failed");
		return IVS_PARA_INVALID;
	}

	/**************************************
	**第一次：查询MBU记录的总条数
	***************************************/
	IVS_UINT32 uiDiffMBUChangeInfoNum = 0;
    //分配结果List的内存
    IVS_UINT32 uiBufSize = sizeof(IVS_MBU_CHANGE_INFO_LIST) + sizeof(IVS_MBU_CHANGE_INFO);
    IVS_CHAR* pMBUInfoListBuf = NULL;
    try
    {
        pMBUInfoListBuf = new IVS_CHAR[uiBufSize];
    }
    catch (...)
    {
        iRet = static_cast <IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
        BP_RUN_LOG_INF("Get Bookmark List By BackupServer", "Get MBU Change History Fail,ErrorCode =%d", iRet);
        return iRet;
    }
    memset(pMBUInfoListBuf, 0, uiBufSize);
    IVS_MBU_CHANGE_INFO_LIST* pMBUInfoList = (IVS_MBU_CHANGE_INFO_LIST*)pMBUInfoListBuf;
    iRet = m_UserMgr->GetRecordList().QueryMBUChangeHistoryList(pUnifiedQuery, chDevCode, chDomaCode, pMBUInfoList,2,&uiDiffMBUChangeInfoNum);
    if (IVS_SUCCEED != iRet)
    {
		IVS_DELETE(pMBUInfoListBuf, MUILI);
        BP_RUN_LOG_ERR(iRet, "Get Bookmark List By BackupServer:Query MBU Change History List failed", "failed");
        return iRet;
    }
	
	/**************************************
	**第二次：查询全部MBU记录信息
	***************************************/
	uiDiffMBUChangeInfoNum = 0;
	//分配结果List的内存
	uiBufSize = sizeof(IVS_MBU_CHANGE_INFO_LIST) + sizeof(IVS_MBU_CHANGE_INFO) * (pMBUInfoList->uiTotal-1);
	IVS_CHAR* pTotalMBUInfoListBuf = NULL;
	try
	{
		pTotalMBUInfoListBuf = new IVS_CHAR[uiBufSize];
	}
	catch (...)
	{
		IVS_DELETE(pMBUInfoListBuf, MUILI);
		iRet = static_cast <IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		BP_RUN_LOG_INF("Get Bookmark List By BackupServer", "Get MBU Change History Fail,ErrorCode =%d", iRet);
		return iRet;
	}
	memset(pTotalMBUInfoListBuf, 0, uiBufSize);
	IVS_MBU_CHANGE_INFO_LIST* pTotalMBUInfoList = (IVS_MBU_CHANGE_INFO_LIST*)pTotalMBUInfoListBuf;     //lint !e826
	iRet = m_UserMgr->GetRecordList().QueryMBUChangeHistoryList(pUnifiedQuery, chDevCode, chDomaCode, pTotalMBUInfoList,static_cast<IVS_INT32>(pMBUInfoList->uiTotal),&uiDiffMBUChangeInfoNum);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pMBUInfoListBuf, MUILI);
		IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
		BP_RUN_LOG_ERR(iRet, "Get Bookmark List By BackupServer:Query MBU Change History List failed", "failed");
		return iRet;
	}

	char cMBUCode[IVS_DOMAIN_CODE_LEN+1]={0};
	memset(cMBUCode,0x0,IVS_DOMAIN_CODE_LEN+1);
	char cMBUDomainCode[IVS_DOMAIN_CODE_LEN+1]={0};
	memset(cMBUDomainCode,0x0,IVS_DOMAIN_CODE_LEN+1);
	bool bRet = false;
	std::string strNVRCode = "";
	for (i=0;i<pUnifiedQuery->iFieldNum;i++)
	{
		if (QUERY_DEV_NVR_CODE == pUnifiedQuery->stQueryField[i].eFieID)
		{
			strNVRCode = pUnifiedQuery->stQueryField[i].cValue;
			break;
		}
	}
	if(strlen(strNVRCode.c_str()) > 0)
	{
		//只发送一个域的查询请求并响应后直接返回的逻辑，满足第三方调用需求
		for (j = 0; j < uiDiffMBUChangeInfoNum; j++)
		{
			bRet = CToolsHelp::Memcpy(cMBUCode,IVS_DOMAIN_CODE_LEN, pTotalMBUInfoList->stMBUChangeInfo[j].cMBUCode, IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (0 == strcmp(cMBUCode, strNVRCode.c_str()))
			{
				bRet = CToolsHelp::Memcpy(cMBUDomainCode,IVS_DOMAIN_CODE_LEN, pTotalMBUInfoList->stMBUChangeInfo[j].cMBUDomainCode, IVS_DOMAIN_CODE_LEN);
				if(false == bRet)
				{
					IVS_DELETE(pMBUInfoListBuf, MUILI);
					IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				break;
			}
		}

		IVS_DELETE(pMBUInfoListBuf, MUILI);
		IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
		return GetBookmarkListByBackupServerForOneDomain(pUnifiedQuery->stIndex.uiFromIndex,pUnifiedQuery->stIndex.uiToIndex,strNVRCode.c_str(),cMBUDomainCode,pUnifiedQuery,pBookmarkList,uiMaxBufSize);
	}

	/**************************************
	**查询每个MBU记录条数信息
	***************************************/
    IVS_UINT32 uiRecordNum = 0;

	//分配内存
	IVS_CHAR* pRecordListBuf = NULL;
	try{
		pRecordListBuf = new IVS_CHAR[sizeof(IVS_RECORD_BOOKMARK_INFO_LIST)];
	}
	catch(...)
	{
		IVS_DELETE(pMBUInfoListBuf, MUILI);
		IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		BP_RUN_LOG_INF("Get Bookmark List For Backup：allocate Memory", "ErrorCode =%d", iRet);
		return iRet;
	}
	memset(pRecordListBuf, 0, uiBufSize);
	IVS_RECORD_BOOKMARK_INFO_LIST* pInfoList = (IVS_RECORD_BOOKMARK_INFO_LIST*)pRecordListBuf;//lint !e826

	for (j = 0; j < uiDiffMBUChangeInfoNum; j++)
	{
		if(j < pTotalMBUInfoList->uiTotal)
		{
			bRet = CToolsHelp::Memcpy(cMBUCode,IVS_DOMAIN_CODE_LEN, pTotalMBUInfoList->stMBUChangeInfo[j].cMBUCode, IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			bRet = CToolsHelp::Memcpy(cMBUDomainCode,IVS_DOMAIN_CODE_LEN, pTotalMBUInfoList->stMBUChangeInfo[j].cMBUDomainCode, IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			iRet = GetBookmarkListByBackupServerForOneDomain(1,1,cMBUCode,cMBUDomainCode,pUnifiedQuery,pInfoList,uiMaxBufSize);
			pTotalMBUInfoList->stMBUChangeInfo[j].uiRecordNum = pInfoList->uiTotal;
		}
	}


	pTotalMBUInfoList->uiTotal = uiDiffMBUChangeInfoNum;
    iRet = m_UserMgr->GetRecordList().ComputeIndexOfRecordList(pUnifiedQuery->stIndex.uiFromIndex, pUnifiedQuery->stIndex.uiToIndex, pTotalMBUInfoList);
    if (IVS_SUCCEED != iRet)
    {
		IVS_DELETE(pMBUInfoListBuf, MUILI);
		IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
		IVS_DELETE(pRecordListBuf, MUILI);
        BP_RUN_LOG_ERR(iRet, "Compute Index of Record List failed", "failed");
        return iRet;
    }

	for (IVS_UINT32 k = 0; k < uiDiffMBUChangeInfoNum; k++)
	{
		if ((pTotalMBUInfoList->stMBUChangeInfo[k].uiFromIndex != 0) && (pTotalMBUInfoList->stMBUChangeInfo[k].uiToIndex != 0))
		{
			CXml xmlReq;
			memset(cMBUCode,0x0,IVS_DOMAIN_CODE_LEN+1);
			memset(cMBUDomainCode,0x0,IVS_DOMAIN_CODE_LEN+1);
			bRet = CToolsHelp::Memcpy(cMBUDomainCode,IVS_DOMAIN_CODE_LEN, pTotalMBUInfoList->stMBUChangeInfo[k].cMBUDomainCode, IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			for (i=0;i<pUnifiedQuery->iFieldNum;i++)
			{
				if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID)
				{
					(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, chDevCode, chDomaCode);
					break;
				}
			}

			//拼装XML查询请求
			iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery,xmlReq,cMBUDomainCode);
			if (IVS_SUCCEED != iRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(iRet,"Get request xml failed","failed");
				return iRet;
			}

			std::string sRecordMethod = "RecordMethod";
			iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(sRecordMethod.c_str(),CToolsHelp::Int2Str(RECORD_METHOD_MBU).c_str(), xmlReq);
			if (IVS_SUCCEED != iRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
				return iRet;
			}

			bRet = CToolsHelp::Memcpy(cMBUCode,IVS_DOMAIN_CODE_LEN, pTotalMBUInfoList->stMBUChangeInfo[k].cMBUCode, IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML("NVRCode",cMBUCode, xmlReq);
			if (IVS_SUCCEED != iRet)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
				return iRet;
			}
			//修改请求的分页信息字段	
			if (xmlReq.FindElemEx("Content/PageInfo"))
			{
				xmlReq.IntoElem();
				if (xmlReq.FindElem("FromIndex"))
				{
					xmlReq.ModifyElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pUnifiedQuery->stIndex.uiFromIndex)).c_str());
				}
				if (xmlReq.FindElem("ToIndex"))
				{
					xmlReq.ModifyElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pUnifiedQuery->stIndex.uiToIndex)).c_str());
				}
				xmlReq.OutOfElem();
			}

			const IVS_CHAR * pBookMarkReq = xmlReq.GetXMLStream(xmlLen);
			if (NULL == pBookMarkReq)
			{
				IVS_DELETE(pMBUInfoListBuf, MUILI);
				IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
				IVS_DELETE(pRecordListBuf, MUILI);
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pBookMarkReq", "failed");
				return IVS_OPERATE_MEMORY_ERROR;
			}
			//发送nss协议
			CSendNssMsgInfo sendNssMsgInfo;	
			sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
			sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
			sendNssMsgInfo.SetReqData(pBookMarkReq); 
			sendNssMsgInfo.SetReqID(NSS_GET_RECORD_BOOKMARK_LIST_REQ);
			sendNssMsgInfo.SetCameraCode(chDevCode);
			sendNssMsgInfo.SetDomainCode(cMBUDomainCode);

			std::string strpRsp;
			IVS_INT32 iNeedRedirect = IVS_FAIL;
			iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
			if (IVS_SUCCEED == iNeedRedirect)
			{
				iRet = m_UserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
			}
			// NSS消息返回码错误，不解析数据，直接返回错误码
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "Get Bookmark List By BackupServer", "SendCmd operation succeed,rspCode = %d", iRet);
				//return iRet;
			}

			CXml xmlRsp;
			if(xmlRsp.Parse(strpRsp.c_str()))
			{
				//解析查询请求XML
				if (IVS_SUCCEED != CRecordXMLProcess::ParseGetBookmarkListForBackup(xmlRsp,pBookmarkList,cMBUDomainCode,uiMaxBufSize,
					uiRecordNum,pTotalMBUInfoList->stMBUChangeInfo[k].uiFromIndex,pTotalMBUInfoList->stMBUChangeInfo[k].uiToIndex))
				{
					BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Bookmark List By BackupServer","Parse response xml failed");
					//return IVS_XML_INVALID;
				}
			}
			
		}
	}

	pBookmarkList->uiTotal = 0;
	for (unsigned int iIndex = 0; iIndex < uiDiffMBUChangeInfoNum; iIndex++)//lint !e574
	{
		pBookmarkList->uiTotal += pTotalMBUInfoList->stMBUChangeInfo[iIndex].uiRecordNum;
	}
	IVS_DELETE(pMBUInfoListBuf, MUILI);
	IVS_DELETE(pTotalMBUInfoListBuf, MUILI);
	IVS_DELETE(pRecordListBuf, MUILI);
	return iRet;
}

IVS_INT32 CRecordBookmark::GetBookmarkListByBackupServerForOneDomain(IVS_UINT32 uiFromIndex,IVS_UINT32 uiToIndex,const char* cNVRCode,const char* cMBUDomainCode,
	const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiMaxBufSize)const
{
	CHECK_POINTER(cNVRCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(cMBUDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBookmarkList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	CXml xmlReq;
	IVS_UINT32 xmlLen = 0;

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] ={0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] ={0};
	for (int i=0;i<pUnifiedQuery->iFieldNum;i++)
	{
		if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID)
		{
			(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, chDevCode, chDomaCode);
			break;
		}
	}

	//拼装XML查询请求
	IVS_INT32 iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery,xmlReq,cMBUDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get request xml failed","failed");
		return iRet;
	}

	std::string sRecordMethod = "RecordMethod";
	iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(sRecordMethod.c_str(),CToolsHelp::Int2Str(RECORD_METHOD_MBU).c_str(), xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
		return iRet;
	}
	char cMBUCode[IVS_DOMAIN_CODE_LEN+1]={0};
	memset(cMBUCode,0x0,IVS_DOMAIN_CODE_LEN+1);
	char chMBUDomainCode[IVS_DOMAIN_CODE_LEN+1]={0};
	memset(chMBUDomainCode,0x0,IVS_DOMAIN_CODE_LEN+1);
	bool bRet = CToolsHelp::Memcpy(cMBUCode,IVS_DOMAIN_CODE_LEN, cNVRCode, IVS_DOMAIN_CODE_LEN);
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML("NVRCode",cMBUCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
		return iRet;
	}
	//修改请求的分页信息字段	
	if (xmlReq.FindElemEx("Content/PageInfo"))
	{
		xmlReq.IntoElem();
		if (xmlReq.FindElem("FromIndex"))
		{
			xmlReq.ModifyElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiFromIndex)).c_str());
		}
		if (xmlReq.FindElem("ToIndex"))
		{
			xmlReq.ModifyElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiToIndex)).c_str());
		}
		xmlReq.OutOfElem();
	}

	const IVS_CHAR * pBookMarkReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pBookMarkReq, IVS_OPERATE_MEMORY_ERROR);
	bRet = CToolsHelp::Memcpy(chMBUDomainCode,IVS_DOMAIN_CODE_LEN, cMBUDomainCode, IVS_DOMAIN_CODE_LEN);
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqData(pBookMarkReq); 
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_BOOKMARK_LIST_REQ);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chMBUDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED == iNeedRedirect)
	{
		iRet = m_UserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
	}
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Bookmark List By BackupServer", "SendCmd operation succeed,rspCode = %d", iRet);
		return iRet;
	}

	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());

	//解析查询请求XML
	if (IVS_SUCCEED != CRecordXMLProcess::ParseGetBookmarkList(xmlRsp,pBookmarkList,cMBUDomainCode,uiMaxBufSize))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Get Bookmark List By BackupServer","Parse response xml failed");
		return IVS_XML_INVALID;
	}
	return iRet;
}

