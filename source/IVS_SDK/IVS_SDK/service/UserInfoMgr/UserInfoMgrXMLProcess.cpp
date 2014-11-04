#include "UserInfoMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"


CUserInfoMgrXMLProcess::CUserInfoMgrXMLProcess(void)
{
}

CUserInfoMgrXMLProcess::~CUserInfoMgrXMLProcess(void)
{
}

IVS_VOID CUserInfoMgrXMLProcess::GetDeleteUserXML(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, CXml& xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    IVS_DEBUG_TRACE("");
    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);
    (void)xml.AddElem("UserList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((int)uiUserID)).c_str());
    xml.OutOfElem();
    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::GetAddModUserInfoXML(const IVS_CHAR* pDomainCode, const IVS_USER_INFO* pUserInfo, const CSDKSecurityClient& oSecurityClient, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    CHECK_POINTER_VOID(pUserInfo);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("UserList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiUserID)).c_str());
    (void)xml.AddElem("LoginName");
    IVS_CHAR cLoginName[IVS_NAME_LEN + 1] = {0};
    strncpy(cLoginName, pUserInfo->cLoginName, IVS_NAME_LEN);
    (void)xml.SetElemValue(cLoginName);
    (void)xml.AddElem("LoginPWD");

	std::string strEncryptedData;
    IVS_CHAR cLoginPWD[IVS_PWD_LEN + 1] = {0};
    strncpy(cLoginPWD, pUserInfo->cPWD, IVS_PWD_LEN);
    strEncryptedData.clear();
    (void)oSecurityClient.EncryptString(cLoginPWD, strEncryptedData);      // 密码：加密
    (void)xml.SetElemValue(strEncryptedData.c_str());

    (void)xml.AddElem("UserType");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiUserType)).c_str());
    (void)xml.AddElem("UserDomain");
    IVS_CHAR cUserDomain[IVS_DOMAIN_LEN + 1] = {0};
    strncpy(cUserDomain, pUserInfo->cUserDomain, IVS_DOMAIN_LEN);
    (void)xml.SetElemValue(cUserDomain);
    (void)xml.AddElem("Status");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiStatus)).c_str());
    (void)xml.AddElem("RegisterDate");
    IVS_CHAR cRegiterDate[IVS_TIME_LEN + 1] = {0};
    strncpy(cRegiterDate, pUserInfo->cRegiterDate, IVS_TIME_LEN);

    (void)xml.SetElemValue(cRegiterDate);
    (void)xml.AddElem("UserDesc"); 
    IVS_CHAR cUserDesc[IVS_DESCRIBE_LEN + 1] = {0};
    strncpy(cUserDesc, pUserInfo->cUserDesc, IVS_DESCRIBE_LEN);

	IVS_CHAR *pUserDescUTF8 = CToolsHelp::ANSIToUTF8(cUserDesc);
    strEncryptedData.clear();
    (void)oSecurityClient.EncryptString(pUserDescUTF8, strEncryptedData);              // 用户描述：加密
	//add by zwx211831, 释放开辟的内存，避免内存泄露
	if (NULL != pUserDescUTF8)
	{
		free(pUserDescUTF8);
		pUserDescUTF8 = NULL;
	}
    IVS_CHAR* pDataANSI = CToolsHelp::UTF8ToANSI(strEncryptedData.c_str());
    if (NULL!= pDataANSI)
    {
        (void)xml.SetElemValue(pDataANSI);
		free(pDataANSI);
		pDataANSI = NULL;
    }

   //  (void)xml.SetElemValue(cUserDesc);
    (void)xml.AddElem("ControlLevel");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiPTZLevel)).c_str());
    (void)xml.AddElem("MaxVideoCNT");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiMaxVideoCnt)).c_str());
    (void)xml.AddElem("MaxSessionCNT");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiMaxSessionCnt)).c_str());
    (void)xml.AddElem("UserName");
    IVS_CHAR cUserName[IVS_NAME_LEN + 1] = {0};
    strncpy(cUserName, pUserInfo->cUserName, IVS_NAME_LEN);
    IVS_CHAR *pUserNameUTF8 = CToolsHelp::ANSIToUTF8(cUserName);
    strEncryptedData.clear();
    (void)oSecurityClient.EncryptString(pUserNameUTF8, strEncryptedData);              // 用户名称：加密
	//add by zwx211831, 释放开辟的内存，避免内存泄露
	if (NULL != pUserNameUTF8)
	{
		free(pUserNameUTF8);
		pUserNameUTF8 = NULL;
	}
    pDataANSI = CToolsHelp::UTF8ToANSI(strEncryptedData.c_str());
    if (NULL!= pDataANSI)
    {
        (void)xml.SetElemValue(pDataANSI);
		free(pDataANSI);
		pDataANSI = NULL;
    }


    // (void)xml.SetElemValue(cUserName);
    (void)xml.AddElem("Email");
    IVS_CHAR cEmail[IVS_EMAILE_LEN + 1] = {0};
    strncpy(cEmail, pUserInfo->cEmail, IVS_EMAILE_LEN);
    strEncryptedData.clear();
    (void)oSecurityClient.EncryptString(cEmail, strEncryptedData);              // Email：加密
    (void)xml.SetElemValue(strEncryptedData.c_str());

    // (void)xml.SetElemValue(cEmail);
    (void)xml.AddElem("Phone");
    IVS_CHAR cPhone[IVS_PHONE_NUMBER_LEN + 1] = {0};
    strncpy(cPhone, pUserInfo->cPhone, IVS_PHONE_NUMBER_LEN);
    strEncryptedData.clear();
    (void)oSecurityClient.EncryptString(cPhone, strEncryptedData);              // Phone：加密
    (void)xml.SetElemValue(strEncryptedData.c_str());

    // (void)xml.SetElemValue(cPhone);
    (void)xml.AddElem("Sex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiSex)).c_str());
    (void)xml.AddElem("UserGroupID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiGroupID)).c_str());
    (void)xml.AddElem("RoleID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiRoleID)).c_str());

    // add by wanglei 00165153:2013.10.22 用户有效期特性
    (void)xml.AddElem("ValidDateFlag");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiValidDateFlag)).c_str());
    
    if (strlen(pUserInfo->cValidDateStart))
    {
        (void)xml.AddElem("ValidDateStart");
        (void)xml.SetElemValue(pUserInfo->cValidDateStart);
    }
    if (strlen(pUserInfo->cValidDateEnd))
    {
        (void)xml.AddElem("ValidDateEnd");
        (void)xml.SetElemValue(pUserInfo->cValidDateEnd);
    }
    // end add


    xml.OutOfElem();

    xml.OutOfElem();
}

IVS_INT32 CUserInfoMgrXMLProcess::AddUserParseXML(CXml &xml, IVS_UINT32* uiUserID)
{
    IVS_DEBUG_TRACE("");

    if(!xml.FindElemEx("Content/UserInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add User Parse XML", "Can't find 'Content/UserInfo'");
        return IVS_XML_INVALID;
    }    
    const IVS_CHAR* szElemValue = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, *uiUserID, xml);
    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_INT32 CUserInfoMgrXMLProcess::GetUserListParseXML(CXml &ReqXml, const CSDKSecurityClient& oSecurityClient, IVS_USER_INFO_LIST* pUserInfoList)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER(pUserInfoList, IVS_OPERATE_MEMORY_ERROR);

    std::string strEncryptedData;
    const char* szElemValue = NULL;
    if(ReqXml.FindElemEx("Content/PageInfo"))
    {
        (void)ReqXml.IntoElem();
        GET_ELEM_VALUE_NUM_FOR_UINT("RealNum", szElemValue, pUserInfoList->uiTotal, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", szElemValue, pUserInfoList->stIndexRange.uiFromIndex, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", szElemValue, pUserInfoList->stIndexRange.uiToIndex, ReqXml);
        ReqXml.OutOfElem();
    }    

    if(!ReqXml.FindElemEx("Content/UserList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User List Parse XML", "FALSE == xml.FindElem('UserList')");
        return IVS_XML_INVALID;
    }  

    int iSize = 0;
    do{
        if (!ReqXml.FindElemValue("UserInfo"))
        {
            break;
        }
        (void)ReqXml.IntoElem();

        IVS_USER_INFO &stUserInfo = pUserInfoList->stUserInfo[iSize];
        memset(&stUserInfo, 0, sizeof(IVS_USER_INFO));

        GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, stUserInfo.uiUserID, ReqXml);
        GET_ELEM_VALUE_CHAR("LoginName", szElemValue, stUserInfo.cLoginName, IVS_NAME_LEN+1, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("UserType", szElemValue, stUserInfo.uiUserType, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("Status", szElemValue, stUserInfo.uiStatus, ReqXml);
        // UserDesc 需要加密
        GET_ELEM_VALUE_POINTER("UserDesc", szElemValue, ReqXml);
        if (NULL != szElemValue)
        {
            oSecurityClient.DecryptString(szElemValue, strEncryptedData);
			char* pDataANSI = CToolsHelp::UTF8ToANSI(strEncryptedData.c_str());
			if  (NULL  != pDataANSI)
			{
				strncpy(stUserInfo.cUserDesc, pDataANSI, IVS_DESCRIBE_LEN);
				free(pDataANSI);
				pDataANSI = NULL;
			}

        }

        GET_ELEM_VALUE_NUM_FOR_UINT("ControlLevel", szElemValue, stUserInfo.uiPTZLevel, ReqXml);

        // UserName 需要加密     
        GET_ELEM_VALUE_POINTER("UserName", szElemValue, ReqXml);
        if (NULL != szElemValue)
        {
            oSecurityClient.DecryptString(szElemValue, strEncryptedData);
			char* pDataANSI = CToolsHelp::UTF8ToANSI(strEncryptedData.c_str());
			if  (NULL  != pDataANSI)
			{
				strncpy(stUserInfo.cUserName, pDataANSI, IVS_NAME_LEN);
				free(pDataANSI);
				pDataANSI = NULL;
			}
            
        }

        // Email 需要加密
        GET_ELEM_VALUE_POINTER("Email", szElemValue, ReqXml);
        if (NULL != szElemValue)
        {
            oSecurityClient.DecryptString(szElemValue, strEncryptedData);
            strncpy(stUserInfo.cEmail, strEncryptedData.c_str(), IVS_EMAILE_LEN);
        }


        // Phone 需要加密
        GET_ELEM_VALUE_POINTER("Phone", szElemValue, ReqXml);
        if (NULL != szElemValue)
        {
            oSecurityClient.DecryptString(szElemValue, strEncryptedData);
            strncpy(stUserInfo.cPhone, strEncryptedData.c_str(), IVS_PHONE_NUMBER_LEN);
        }



        GET_ELEM_VALUE_NUM_FOR_UINT("UserGroupID", szElemValue, stUserInfo.uiGroupID, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("RoleID", szElemValue, stUserInfo.uiRoleID, ReqXml);

        // add by wanglei 00165153:2013.10.22 用户有效期特性
        GET_ELEM_VALUE_NUM_FOR_UINT("ValidDateFlag", szElemValue, stUserInfo.uiValidDateFlag, ReqXml);
        GET_ELEM_VALUE_CHAR("ValidDateStart", szElemValue, stUserInfo.cValidDateStart, IVS_TIME_LEN, ReqXml);
        GET_ELEM_VALUE_CHAR("ValidDateEnd", szElemValue, stUserInfo.cValidDateEnd, IVS_TIME_LEN, ReqXml);
        // end add

        ReqXml.OutOfElem();
        iSize++;
    }while (ReqXml.NextElem());

    return IVS_SUCCEED;
}

IVS_INT32 CUserInfoMgrXMLProcess::GetOnlineUserListParseXML(CXml &ReqXml, IVS_ONLINE_USER_LIST* pOnlUserList)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER(pOnlUserList, IVS_OPERATE_MEMORY_ERROR);

    int iSize = 0;
    const char* szElemValue = NULL;

    if(ReqXml.FindElemEx("Content/PageInfo"))
    {
        ReqXml.IntoElem();
        GET_ELEM_VALUE_NUM_FOR_UINT("RealNum", szElemValue, pOnlUserList->uiTotal, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", szElemValue, pOnlUserList->stIndexRange.uiFromIndex, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", szElemValue, pOnlUserList->stIndexRange.uiToIndex, ReqXml);
        ReqXml.OutOfElem();
    }   

    if(!ReqXml.FindElemEx("Content/LoginList"))
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get Online User List Parse XML", "FALSE == xml.FindElem('LoginList')");
        return IVS_OPERATE_MEMORY_ERROR;
    }  

    do{
        if (!ReqXml.FindElemValue("LoginInfo"))
        {
            break;
        }
        (void)ReqXml.IntoElem();

        IVS_ONLINE_USER &stOnlUser = pOnlUserList->stOnlineUser[iSize];
        memset(&stOnlUser, 0, sizeof(IVS_ONLINE_USER));

        GET_ELEM_VALUE_NUM_FOR_UINT("ID", szElemValue, stOnlUser.uiUserOnlineID, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, stOnlUser.uiUserID, ReqXml);
        GET_ELEM_VALUE_CHAR("LoginName", szElemValue, stOnlUser.cUserName, IVS_NAME_LEN+1, ReqXml);
        GET_ELEM_VALUE_CHAR("DomainName", szElemValue, stOnlUser.cDomainName, IVS_IP_LEN, ReqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ClientType", szElemValue, stOnlUser.uiClientType, ReqXml);
        GET_ELEM_VALUE_CHAR("IPAddress", szElemValue, stOnlUser.stIP.cIP, IVS_IP_LEN, ReqXml);
        GET_ELEM_VALUE_CHAR("MachineName", szElemValue, stOnlUser.cMachineName, IVS_MACHINE_NAME_LEN, ReqXml);
        GET_ELEM_VALUE_CHAR("LoginTime", szElemValue, stOnlUser.cLoginTime, IVS_TIME_LEN, ReqXml);

        iSize ++;
        ReqXml.OutOfElem();

    }while (ReqXml.NextElem());

    return IVS_SUCCEED;
}

IVS_VOID CUserInfoMgrXMLProcess::GetUserInfoGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserID)).c_str());
    xml.OutOfElem();
}

IVS_INT32 CUserInfoMgrXMLProcess::GetUserInfoParseXML(CXml &xml, const CSDKSecurityClient& oSecurityClient, IVS_USER_INFO* pUserInfo)
{  
    IVS_DEBUG_TRACE("");
	CHECK_POINTER(pUserInfo, IVS_OPERATE_MEMORY_ERROR);

    if(!xml.FindElemEx("Content/UserList/UserInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User Info Parse XML", "Can't find 'Content/UserList/UserInfo'");
        return IVS_XML_INVALID;
    }
    
    const IVS_CHAR* szElemValue = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, pUserInfo->uiUserID, xml);
    GET_ELEM_VALUE_CHAR("LoginName", szElemValue, pUserInfo->cLoginName, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_CHAR("LoginPWD", szElemValue, pUserInfo->cPWD, IVS_PWD_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("UserType", szElemValue, pUserInfo->uiUserType, xml);
    GET_ELEM_VALUE_CHAR("UserDomain", szElemValue, pUserInfo->cUserDomain, IVS_DOMAIN_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("Status", szElemValue, pUserInfo->uiStatus, xml);
    GET_ELEM_VALUE_CHAR("RegisterDate", szElemValue, pUserInfo->cRegiterDate, IVS_TIME_LEN, xml);
    // GET_ELEM_VALUE_CHAR("UserDesc", szElemValue, pUserInfo->cUserDesc, IVS_DESCRIBE_LEN, xml);
    GET_ELEM_VALUE_POINTER("UserDesc", szElemValue, xml);

	std::string strEncryptedData;
    if (NULL != szElemValue)
    {
        oSecurityClient.DecryptString(szElemValue, strEncryptedData);
        char* pDataANSI = CToolsHelp::UTF8ToANSI(strEncryptedData.c_str());
        if  (NULL  != pDataANSI)
        {
            strncpy(pUserInfo->cUserDesc, pDataANSI, IVS_DESCRIBE_LEN);
			free(pDataANSI);
			pDataANSI = NULL;
        }
    }
    GET_ELEM_VALUE_NUM_FOR_UINT("ControlLevel", szElemValue, pUserInfo->uiPTZLevel, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("MaxVideoCNT", szElemValue, pUserInfo->uiMaxVideoCnt, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("MaxSessionCNT", szElemValue, pUserInfo->uiMaxSessionCnt, xml);

    // GET_ELEM_VALUE_CHAR("UserName", szElemValue, pUserInfo->cUserName, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_POINTER("UserName", szElemValue, xml);
    if (NULL != szElemValue)
    {
        oSecurityClient.DecryptString(szElemValue, strEncryptedData);
        char* pDataANSI = CToolsHelp::UTF8ToANSI(strEncryptedData.c_str());
        if  (NULL  != pDataANSI)
        {
            strncpy(pUserInfo->cUserName, pDataANSI, IVS_NAME_LEN);
			free(pDataANSI);
			pDataANSI = NULL;
        }
    }
    // GET_ELEM_VALUE_CHAR("Email", szElemValue, pUserInfo->cEmail, IVS_EMAILE_LEN, xml);
    GET_ELEM_VALUE_POINTER("Email", szElemValue, xml);
    if (NULL != szElemValue)
    {
        oSecurityClient.DecryptString(szElemValue, strEncryptedData);
        strncpy(pUserInfo->cEmail, strEncryptedData.c_str(), IVS_EMAILE_LEN);
    }
    // GET_ELEM_VALUE_CHAR("Phone", szElemValue, pUserInfo->cPhone, IVS_PHONE_NUMBER_LEN, xml);
    GET_ELEM_VALUE_POINTER("Phone", szElemValue, xml);
    if (NULL != szElemValue)
    {
        oSecurityClient.DecryptString(szElemValue, strEncryptedData);
        strncpy(pUserInfo->cPhone, strEncryptedData.c_str(), IVS_PHONE_NUMBER_LEN);
    }
    GET_ELEM_VALUE_NUM_FOR_UINT("Sex", szElemValue, pUserInfo->uiSex, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("UserGroupID", szElemValue, pUserInfo->uiGroupID, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("RoleID", szElemValue, pUserInfo->uiRoleID, xml);

    // add by wanglei 00165153:2013.10.22 用户有效期特性
    GET_ELEM_VALUE_NUM_FOR_UINT("ValidDateFlag", szElemValue, pUserInfo->uiValidDateFlag, xml);
    GET_ELEM_VALUE_CHAR("ValidDateStart", szElemValue, pUserInfo->cValidDateStart, IVS_TIME_LEN, xml);
    GET_ELEM_VALUE_CHAR("ValidDateEnd", szElemValue, pUserInfo->cValidDateEnd, IVS_TIME_LEN, xml);
    // end add

    return IVS_SUCCEED;
}

IVS_VOID CUserInfoMgrXMLProcess::LockUserGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_BOOL bLock, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("UserList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)uiUserID)).c_str());
    (void)xml.AddElem("Status");
    //0-解锁，1-锁定smu需要的用户状态是：用户状态： 1：激活 2：停用
    (void)xml.SetElemValue(bLock ? "2" : "1");

    xml.OutOfElem();

    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::ChangePWDGetXML(const IVS_CHAR* cLoginID, const IVS_CHAR* pOldPWD, const IVS_CHAR* pNewPWD, CXml &xml)
{
    CHECK_POINTER_VOID(cLoginID);
    CHECK_POINTER_VOID(pOldPWD);
    CHECK_POINTER_VOID(pNewPWD);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommXmlHead(xml);

    (void)xml.AddChildElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("OldPWD");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pOldPWD);
    (void)xml.AddElem("NewPWD");
    (void)xml.SetElemValue(pNewPWD);
    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::ResetPWDGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_CHAR* pNewPWD, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    CHECK_POINTER_VOID(pNewPWD);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserID))).c_str());
    (void)xml.AddElem("LoginPWD");
    (void)xml.SetElemValue(pNewPWD);
    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::CheckPWDByRoleGetXML(const IVS_CHAR* pCode, IVS_UINT32 uiID, const IVS_CHAR* pLoginName, const IVS_CHAR* pPWD, CXml &xml)
{
    CHECK_POINTER_VOID(pCode);
    CHECK_POINTER_VOID(pPWD);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pCode, xml);

    (void)xml.AddElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("RoleID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiID))).c_str());
    (void)xml.AddElem("LoginName");
    (void)xml.SetElemValue(pLoginName);
    (void)xml.AddElem("LoginPWD");
    (void)xml.SetElemValue(pPWD);
    xml.OutOfElem();

    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::SignOutUserGetXML(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("UserList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserID))).c_str());
    xml.OutOfElem();
    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::SignOutOnlineUserGetXML(const IVS_CHAR* pSmuDomainCode, const IVS_UINT32 uiUserOnlineID, CXml &xml)
{
    CHECK_POINTER_VOID(pSmuDomainCode);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pSmuDomainCode, xml);

    (void)xml.AddElem("UserList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("ID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserOnlineID))).c_str());//在线用户信息记录ID
    xml.OutOfElem();
    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::SetUserDevListGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pUserDomainCode, IVS_UINT32 uiUserID, const std::list<IVS_USER_DEVICE_INFO *> &pUserDevList, CXml &xml)
{
    IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);

    (void)xml.AddElem("UserInfo");
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserID))).c_str());
    (void)xml.AddElem("UserDomainCode");
    (void)xml.SetElemValue(pUserDomainCode);
    xml.OutOfElem();

    const IVS_USER_DEVICE_INFO* pUserDev;
    (void)xml.AddElem("DevList");
    (void)xml.IntoElem();
    std::list<IVS_USER_DEVICE_INFO *>::const_iterator iter = pUserDevList.begin();
    IVS_UINT32 i = 0;
    for (; iter != pUserDevList.end(); iter++)
    {
        pUserDev = (IVS_USER_DEVICE_INFO*)(*iter);
        if(i > 0)
        {
            (void)xml.AddElem("DevInfo");
        }
        else
        {
            (void)xml.AddChildElem("DevInfo");
            (void)xml.IntoElem();
        }

        (void)xml.AddChildElem("DevGroupCode");
        (void)xml.IntoElem();
		// 拆掉域编码
		IVS_CHAR szDevGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
		IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};

		CHECK_POINTER_VOID(pUserDev);
		(void)CXmlProcess::ParseDevCode(pUserDev->cDevGroupCode, szDevGroupCode, szDomainCode);
        (void)xml.SetElemValue(szDevGroupCode);

        (void)xml.AddElem("DevCode");
        IVS_CHAR cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cDevCode, IVS_DEV_CODE_LEN, pUserDev->cDevCode, IVS_DEV_CODE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserDev->cDevCode", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cDevCode);
        (void)xml.AddElem("DevDomainCode");
        IVS_CHAR cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDev->cDevDomainCode, IVS_DOMAIN_CODE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserDev->cDevDomainCode", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cDevDomainCode);
        (void)xml.AddElem("IsExDomain");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pUserDev->bIsExDomain).c_str());
        (void)xml.AddElem("IsTotalGroup");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pUserDev->bIsTotalGroup).c_str());

        xml.OutOfElem();
        i++;
    }
    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::GetUserDeviceListGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_INDEX_RANGE* pIndexRange, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    CHECK_POINTER_VOID(pIndexRange);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommQueryXml(pDomainCode, pIndexRange, xml);
    (void)xml.AddElem("QueryCond");
    (void)xml.AddChildElem("QueryField");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("Field");
    (void)xml.IntoElem();
    (void)xml.SetElemValue("UserID");
    (void)xml.AddElem("Value");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserID)).c_str());
    (void)xml.AddElem("QueryType");
    (void)xml.SetElemValue("EXACT");
    xml.OutOfElem();

    xml.OutOfElem();
    xml.OutOfElem();
}

IVS_INT32 CUserInfoMgrXMLProcess::UserDeviceListParseXML(CXml& xml, IVS_USER_DEVICE_INFO_LIST* pUserDevList)
{
    CHECK_POINTER(pUserDevList,IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    const IVS_CHAR* szElemValue = NULL;

    if (!xml.FindElemEx("Content/PageInfo"))
    {
        return IVS_XML_INVALID;
    }
    xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("RealNum", szElemValue, pUserDevList->uiTotal, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", szElemValue, pUserDevList->stIndexRange.uiFromIndex, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", szElemValue, pUserDevList->stIndexRange.uiToIndex, xml);
    xml.OutOfElem();

    if (!xml.FindElemEx("Content/DevList"))
    {
        return IVS_XML_INVALID;
    }

    IVS_INT32 iSize = 0;
    if (xml.FindElemEx("Content/DevList/DevInfo"))
    {
        do
        {
            (void)xml.IntoElem();

            IVS_USER_DEVICE_INFO &stUserDev = pUserDevList->stUserDeviceInfo[iSize];
            memset(&stUserDev, 0, sizeof(IVS_USER_DEVICE_INFO));

			GET_ELEM_VALUE_CHAR("DevGroupCode", szElemValue, stUserDev.cDevGroupCode, IVS_DEVICE_GROUP_LEN, xml);
            GET_ELEM_VALUE_CHAR("DevCode", szElemValue, stUserDev.cDevCode, IVS_DEV_CODE_LEN, xml);
            GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, stUserDev.cDevDomainCode, IVS_DOMAIN_CODE_LEN, xml);
            GET_ELEM_VALUE_NUM("IsExDomain", szElemValue, stUserDev.bIsExDomain, xml);
            GET_ELEM_VALUE_NUM("IsTotalGroup", szElemValue, stUserDev.bIsTotalGroup, xml);
			IVS_CHAR szDevGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
			(void)CToolsHelp::Memcpy(szDevGroupCode, IVS_DEVICE_GROUP_LEN, stUserDev.cDevGroupCode, IVS_DEVICE_GROUP_LEN);
			std::string strDevGroupCode = szDevGroupCode;
			IVS_CHAR szDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
			(void)CToolsHelp::Memcpy(szDevDomainCode, IVS_DOMAIN_CODE_LEN, stUserDev.cDevDomainCode, IVS_DOMAIN_CODE_LEN);
			strDevGroupCode.append("#").append(szDevDomainCode);
			(void)CToolsHelp::Memcpy(stUserDev.cDevGroupCode,IVS_DEVICE_GROUP_LEN,strDevGroupCode.c_str(),strDevGroupCode.length());
            xml.OutOfElem();

            iSize++;
        }while(xml.NextElem());
    }

    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_VOID CUserInfoMgrXMLProcess::GetRoleXML(const IVS_CHAR* pDomainCode, const IVS_ROLE_INFO* pRoleInfo, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    CHECK_POINTER_VOID(pRoleInfo);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("RoleInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("RoleID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->uiRoleID)).c_str());
    (void)xml.AddElem("RoleName");
    IVS_CHAR cRoleName[IVS_NAME_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cRoleName, IVS_NAME_LEN, pRoleInfo->cRoleName, IVS_NAME_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pRoleInfo->cRoleName", "Memcpy failed");
        //return IVS_ALLOC_MEMORY_ERROR;
    }
    (void)xml.SetElemValue(cRoleName);
    (void)xml.AddElem("RoleDesc");
    IVS_CHAR cRoleDescribe[IVS_DESCRIBE_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cRoleDescribe, IVS_DESCRIBE_LEN, pRoleInfo->cRoleDescribe, IVS_DESCRIBE_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pRoleInfo->cRoleDescribe", "Memcpy failed");
        //return IVS_ALLOC_MEMORY_ERROR;
    }
    (void)xml.SetElemValue(cRoleDescribe);
    (void)xml.AddElem("RoleOPInfo");
    (void)xml.AddChildElem("OPInfo");
    (void)xml.IntoElem();

    IVS_CHAR cRight[IVS_MAX_OPERATION_RIGHT + 1] = {0};
    if (!CToolsHelp::Memcpy(cRight, IVS_MAX_OPERATION_RIGHT, pRoleInfo->cRight, IVS_MAX_OPERATION_RIGHT))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pRoleInfo->cRight", "Memcpy failed");
        //return IVS_ALLOC_MEMORY_ERROR;
    }
    (void)xml.SetElemValue(cRight);
    xml.OutOfElem();

    (void)xml.AddElem("RoleRuleInfo");
    (void)xml.AddChildElem("IsEnable");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bEnable)).c_str());

    (void)xml.AddElem("PWDConsist");
    (void)xml.AddChildElem("PWDConsistCapital");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bCapital)).c_str());
    (void)xml.AddElem("PWDConsistLowercase");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bLowercase)).c_str());
    (void)xml.AddElem("PWDConsistFigure");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bFigure)).c_str());
    (void)xml.AddElem("PWDConsistSpecial");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bSpecialChar)).c_str());
    xml.OutOfElem();

    (void)xml.AddElem("ACCFirstModify");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bFirstModify)).c_str());
    (void)xml.AddElem("ACCCheckOldPWD");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bCheckOldPwd)).c_str());
    (void)xml.AddElem("PWDAccountNotFit");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bAccountNotFit)).c_str());
    (void)xml.AddElem("PWDAccountBackNotFit");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bAccountBackNotFit)).c_str());
    (void)xml.AddElem("DomainACCCheckPWD");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bDomainAccCheckPwd)).c_str());
    (void)xml.AddElem("PWDMinLength");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRoleInfo->stAccountRule.uiPwdMinLen))).c_str());
    (void)xml.AddElem("PWDDiferentNumber");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRoleInfo->stAccountRule.uiPwdDifferentNum))).c_str());
    (void)xml.AddElem("ACCLockNumber");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRoleInfo->stAccountRule.uiAccountLockNum))).c_str());
    (void)xml.AddElem("ACCLockElapse");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRoleInfo->stAccountRule.uiAccountLockElapse))).c_str());

    (void)xml.AddElem("ACCLockInterval");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRoleInfo->stAccountRule.uiAccountLockInterval))).c_str());
    (void)xml.AddElem("PWDExpireTime");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRoleInfo->stAccountRule.uiPWDExpireTime))).c_str());
    (void)xml.AddElem("PWDExpireNotifyTime");
    (void)xml.SetElemValue((CToolsHelp::Int2Str(static_cast<IVS_INT32>(pRoleInfo->stAccountRule.uiPWDExpireNotifyTime))).c_str());
    (void)xml.AddElem("PWDExpireForceNotify");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bPWDExpireForceNotify).c_str());
}

IVS_INT32 CUserInfoMgrXMLProcess::AddRoleParseXML(CXml &xml, IVS_UINT32* &uiRoleID)
{
    IVS_DEBUG_TRACE("");
    if(!xml.FindElemEx("Content/RoleInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add Role Parse XML ", "RoleInfo not found");
        return IVS_XML_INVALID;
    }
    const IVS_CHAR* szElemValue = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("RoleID", szElemValue, *uiRoleID, xml);
    xml.OutOfElem();

    return IVS_SUCCEED;

}

IVS_INT32 CUserInfoMgrXMLProcess::AddUserGroupParseXML(CXml& xml, IVS_UINT32* puiUserGroupID)
{
    IVS_DEBUG_TRACE("");

    if(!xml.FindElemEx("Content/GroupList/GroupInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add User Group Parse XML", "Can't find 'Content/GroupList/GroupInfo'");
        return IVS_XML_INVALID;
    }    
    const IVS_CHAR* szElemValue = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", szElemValue, *puiUserGroupID, xml);
    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_INT32 CUserInfoMgrXMLProcess::GetRoleListParseXML(CXml &xml, IVS_ROLE_INFO_LIST* pRoleList)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER(pRoleList, IVS_OPERATE_MEMORY_ERROR);

    const char* szElemValue = NULL;
    if(xml.FindElemEx("Content/PageInfo"))
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_NUM_FOR_UINT("RealNum", szElemValue, pRoleList->uiTotal, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", szElemValue, pRoleList->stIndexRange.uiFromIndex, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", szElemValue, pRoleList->stIndexRange.uiToIndex, xml);
        xml.OutOfElem();
    }
    if (!xml.FindElem("RoleList"))
    {
        return IVS_XML_INVALID;
    }
    xml.IntoElem();
    int iSize = 0;
    do{
        if (!xml.FindElemValue("RoleInfo"))
        {
            break;
        }
        (void)xml.IntoElem();

        IVS_ROLE_INFO &stRoleInfo = pRoleList->stRoleInfo[iSize];
        memset(&stRoleInfo, 0, sizeof(IVS_ROLE_INFO));

        GET_ELEM_VALUE_NUM_FOR_UINT("RoleID", szElemValue, stRoleInfo.uiRoleID, xml);
        GET_ELEM_VALUE_CHAR("RoleName", szElemValue, stRoleInfo.cRoleName, IVS_NAME_LEN, xml);
        GET_ELEM_VALUE_CHAR("RoleDesc", szElemValue, stRoleInfo.cRoleDescribe, IVS_DESCRIBE_LEN, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("RoleType", szElemValue, stRoleInfo.uiRoleType, xml);

        if (xml.FindElem("RoleOPInfo"))
        {
            (void)xml.IntoElem();
            GET_ELEM_VALUE_CHAR("OPInfo", szElemValue, stRoleInfo.cRight,IVS_MAX_OPERATION_RIGHT, xml);
            xml.OutOfElem();
        }       

        if(xml.FindElem("RoleRuleInfo"))
        {
            (void)xml.IntoElem();
            GET_ELEM_VALUE_NUM("IsEnable", szElemValue, stRoleInfo.stAccountRule.bEnable, xml);
            if(xml.FindElem("PWDConsist"))
            {
                (void)xml.IntoElem();
                GET_ELEM_VALUE_NUM("PWDConsistCapital", szElemValue, stRoleInfo.stAccountRule.stPswConsist.bCapital, xml);
                GET_ELEM_VALUE_NUM("PWDConsistLowercase", szElemValue, stRoleInfo.stAccountRule.stPswConsist.bLowercase, xml);
                GET_ELEM_VALUE_NUM("PWDConsistFigure", szElemValue, stRoleInfo.stAccountRule.stPswConsist.bFigure, xml);
                GET_ELEM_VALUE_NUM("PWDConsistSpecial", szElemValue, stRoleInfo.stAccountRule.stPswConsist.bSpecialChar, xml);
                xml.OutOfElem();
            }

            GET_ELEM_VALUE_NUM("ACCFirstModify", szElemValue, stRoleInfo.stAccountRule.bFirstModify, xml);
            GET_ELEM_VALUE_NUM("ACCCheckOldPWD", szElemValue, stRoleInfo.stAccountRule.bCheckOldPwd, xml);
            GET_ELEM_VALUE_NUM("PWDAccountNotFit", szElemValue, stRoleInfo.stAccountRule.bAccountNotFit, xml);
            GET_ELEM_VALUE_NUM("PWDAccountBackNotFit", szElemValue, stRoleInfo.stAccountRule.bAccountBackNotFit, xml);
            GET_ELEM_VALUE_NUM("DomainACCCheckPWD", szElemValue, stRoleInfo.stAccountRule.bDomainAccCheckPwd, xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("PWDMinLength", szElemValue, stRoleInfo.stAccountRule.uiPwdMinLen, xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("PWDDiferentNumber", szElemValue, stRoleInfo.stAccountRule.uiPwdDifferentNum, xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockNumber", szElemValue, stRoleInfo.stAccountRule.uiAccountLockNum, xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockElapse", szElemValue, stRoleInfo.stAccountRule.uiAccountLockElapse, xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockInterval", szElemValue, stRoleInfo.stAccountRule.uiAccountLockInterval, xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireTime", szElemValue, stRoleInfo.stAccountRule.uiPWDExpireTime, xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireNotifyTime", szElemValue, stRoleInfo.stAccountRule.uiPWDExpireNotifyTime, xml);
            GET_ELEM_VALUE_NUM("PWDExpireForceNotify", szElemValue, stRoleInfo.stAccountRule.bPWDExpireForceNotify, xml);
            xml.OutOfElem(); 
        }

        xml.OutOfElem();  // RoleInfo

        iSize ++;
    }while (xml.NextElem());

    return IVS_SUCCEED;
}

IVS_INT32 CUserInfoMgrXMLProcess::GetDeleteUserGroupXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserGroupID, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("GroupList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("GroupInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("GroupID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserGroupID)).c_str());

    (void)xml.AddElem("ParentGroupID");
    (void)xml.AddElem("GroupName");
    xml.OutOfElem();

    xml.OutOfElem();

    (void)xml.AddElem("TargetGroupID");

    return IVS_SUCCEED;
}

IVS_VOID CUserInfoMgrXMLProcess::GetUserGroupXML(const IVS_CHAR* pDomainCode, const IVS_USER_GROUP* pUserGroup, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    CHECK_POINTER_VOID(pUserGroup);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("GroupList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("GroupInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("GroupID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pUserGroup->uiID)).c_str());
    (void)xml.AddElem("ParentGroupID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pUserGroup->uiParentID)).c_str());
    (void)xml.AddElem("GroupName");
    (void)xml.IntoElem();
    IVS_CHAR cName[IVS_NAME_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cName, IVS_NAME_LEN, pUserGroup->cName, IVS_NAME_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserGroup->cName", "Memcpy failed");
        //return IVS_ALLOC_MEMORY_ERROR;
    }
    (void)xml.SetElemValue(cName);
    (void)xml.AddElem("GroupDesc");
    (void)xml.IntoElem();
    IVS_CHAR cDesc[IVS_MAX_USER_GROUP_DESC_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cDesc, IVS_MAX_USER_GROUP_DESC_LEN, pUserGroup->cGroupDesc, IVS_MAX_USER_GROUP_DESC_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserGroup->cGroupDesc", "Memcpy failed");
        //return IVS_ALLOC_MEMORY_ERROR;
    }
    (void)xml.SetElemValue(cDesc);
    xml.OutOfElem();

    xml.OutOfElem();

    (void)xml.AddElem("TargetGroupID");
}

IVS_INT32 CUserInfoMgrXMLProcess::MoveUserGroupGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiTargetDevGroupID, const IVS_USER_GROUP_LIST* pDeviceGroupList, CXml& xml)
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDeviceGroupList, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);

    (void)xml.AddElem("GroupList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("GroupInfo");
    (void)xml.IntoElem();

    for (unsigned int i=0; i<pDeviceGroupList->uiTotal; i++)
    {
        if (i != 0)
        {
            (void)xml.AddElem("GroupInfo");
        }
        (void)xml.AddChildElem("GroupID");
        (void)xml.IntoElem();
        (void)xml.SetElemValue((CToolsHelp::Int2Str((int)pDeviceGroupList->stUserGroup[i].uiID)).c_str());

        (void)xml.AddElem("ParentGroupID");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((int)pDeviceGroupList->stUserGroup[i].uiParentID)).c_str());

        (void)xml.AddElem("GroupName");
        (void)xml.SetElemValue(pDeviceGroupList->stUserGroup[i].cName);

        (void)xml.AddElem("GroupDesc");
        (void)xml.SetElemValue(pDeviceGroupList->stUserGroup[i].cGroupDesc);

        xml.OutOfElem();
    }
    xml.OutOfElem();

    (void)xml.AddElem("TargetGroupID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)uiTargetDevGroupID).c_str());
    xml.OutOfElem();

    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_INT32 CUserInfoMgrXMLProcess::GetUserGroupListParseXML(CXml &RsqXml, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_GROUP_LIST* pUserGroupList)
{
    CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserGroupList, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    // 查找Content/PageInfo
    if (!RsqXml.FindElemEx("Content/PageInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User Group List Parse XML", "no Content PageInfo element in xml");
        return IVS_XML_INVALID;
    }
    const IVS_CHAR* pTemp = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("RealNum", pTemp, pUserGroupList->uiTotal, RsqXml); 
    GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", pTemp, pUserGroupList->stIndexRange.uiFromIndex, RsqXml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", pTemp, pUserGroupList->stIndexRange.uiToIndex, RsqXml);
    RsqXml.OutOfElem();

    //判断是否内存分配不足
    IVS_UINT32 iReqNum = (pIndexRange->uiToIndex - pIndexRange->uiFromIndex) + 1;
    IVS_UINT32 iRspNum = (pUserGroupList->stIndexRange.uiToIndex - pUserGroupList->stIndexRange.uiFromIndex) + 1;
    if (iReqNum < iRspNum)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User Group List Parse XML", "memory not enough");
        return IVS_XML_INVALID;
    }

    // 查找GroupList
    if(!RsqXml.FindElemEx("Content/GroupList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get UserList ParseXML", "FALSE == xml.FindElem('GroupList')");
        return IVS_XML_INVALID;
    }

    IVS_INT32 iSize = 0;
    do 
    {
        if (!RsqXml.FindElemValue("GroupInfo"))
        {
            break;
        }

        (void)RsqXml.IntoElem();
        IVS_USER_GROUP &stQueryInfo = pUserGroupList->stUserGroup[iSize];
        memset(&stQueryInfo, 0, sizeof(IVS_USER_GROUP));

        GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", pTemp, stQueryInfo.uiID, RsqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ParentGroupID", pTemp, stQueryInfo.uiParentID, RsqXml); 
        GET_ELEM_VALUE_CHAR("GroupName", pTemp, stQueryInfo.cName, IVS_NAME_LEN, RsqXml);
        GET_ELEM_VALUE_CHAR("GroupDesc", pTemp, stQueryInfo.cGroupDesc, IVS_MAX_USER_GROUP_DESC_LEN, RsqXml);
        GET_ELEM_VALUE_NUM_FOR_UINT("Level", pTemp, stQueryInfo.uiLevel, RsqXml);

        RsqXml.OutOfElem();        
        iSize ++;
    }while(RsqXml.NextElem());

    RsqXml.OutOfElem();
    RsqXml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_VOID CUserInfoMgrXMLProcess::GetRoleInfoByRoleIDGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("RoleInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("RoleID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRoleID)).c_str());
    xml.OutOfElem();
}

IVS_VOID CUserInfoMgrXMLProcess::GetAutoCreatePWDGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("RoleID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRoleID)).c_str());
    (void)xml.AddElem("LoginName");
    (void)xml.SetElemValue(pLoginName);
    xml.OutOfElem();
}

IVS_INT32 CUserInfoMgrXMLProcess::GetRoleInfoParseXML(CXml &xml, IVS_ROLE_INFO* pRoleInfo)
{
    CHECK_POINTER(pRoleInfo, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    if(!xml.FindElemEx("Content/RoleInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User Info Parse XML", "Can't find 'Content/RoleInfo'");
        return IVS_XML_INVALID;
    }

    (void)xml.IntoElem();
    const IVS_CHAR* szElemValue = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("RoleID", szElemValue, pRoleInfo->uiRoleID, xml);
    GET_ELEM_VALUE_CHAR("RoleName", szElemValue, pRoleInfo->cRoleName, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_CHAR("RoleDesc", szElemValue, pRoleInfo->cRoleDescribe, IVS_DESCRIBE_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("RoleType", szElemValue, pRoleInfo->uiRoleType, xml);

    if (xml.FindElem("RoleOPInfo"))
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_CHAR("OPInfo", szElemValue, pRoleInfo->cRight,IVS_MAX_OPERATION_RIGHT, xml);
        xml.OutOfElem();
    }  
    xml.OutOfElem();

    if(xml.FindElem("RoleRuleInfo"))
    {
        (void)xml.IntoElem();
    }
    GET_ELEM_VALUE_NUM("IsEnable", szElemValue, pRoleInfo->stAccountRule.bEnable, xml);
    if(xml.FindElem("PWDConsist"))
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_NUM("PWDConsistCapital", szElemValue, pRoleInfo->stAccountRule.stPswConsist.bCapital, xml);
        GET_ELEM_VALUE_NUM("PWDConsistLowercase", szElemValue, pRoleInfo->stAccountRule.stPswConsist.bLowercase, xml);
        GET_ELEM_VALUE_NUM("PWDConsistFigure", szElemValue, pRoleInfo->stAccountRule.stPswConsist.bFigure, xml);
        GET_ELEM_VALUE_NUM("PWDConsistSpecial", szElemValue, pRoleInfo->stAccountRule.stPswConsist.bSpecialChar, xml);
        xml.OutOfElem();
    }

    GET_ELEM_VALUE_NUM("ACCFirstModify", szElemValue, pRoleInfo->stAccountRule.bFirstModify, xml);
    GET_ELEM_VALUE_NUM("ACCCheckOldPWD", szElemValue, pRoleInfo->stAccountRule.bCheckOldPwd, xml);
    GET_ELEM_VALUE_NUM("PWDAccountNotFit", szElemValue, pRoleInfo->stAccountRule.bAccountNotFit, xml);
    GET_ELEM_VALUE_NUM("PWDAccountBackNotFit", szElemValue, pRoleInfo->stAccountRule.bAccountBackNotFit, xml);
    GET_ELEM_VALUE_NUM("DomainACCCheckPWD", szElemValue, pRoleInfo->stAccountRule.bDomainAccCheckPwd, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDMinLength", szElemValue, pRoleInfo->stAccountRule.uiPwdMinLen, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDDiferentNumber", szElemValue, pRoleInfo->stAccountRule.uiPwdDifferentNum, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockNumber", szElemValue, pRoleInfo->stAccountRule.uiAccountLockNum, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockElapse", szElemValue, pRoleInfo->stAccountRule.uiAccountLockElapse, xml);

    GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockInterval", szElemValue, pRoleInfo->stAccountRule.uiAccountLockInterval, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireTime", szElemValue, pRoleInfo->stAccountRule.uiPWDExpireTime, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireNotifyTime", szElemValue, pRoleInfo->stAccountRule.uiPWDExpireNotifyTime, xml);
    GET_ELEM_VALUE_NUM("PWDExpireForceNotify", szElemValue, pRoleInfo->stAccountRule.bPWDExpireForceNotify, xml);
    xml.OutOfElem();

    xml.OutOfElem(); 

    return IVS_SUCCEED;
}

IVS_INT32 CUserInfoMgrXMLProcess::GetAutoCreatePWDParseXML(CXml &xml, IVS_CHAR* pPWD)
{
    CHECK_POINTER(pPWD, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    if(!xml.FindElemEx("Content/PWDInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Auto Create PWD ParseXML", "Can't find 'Content/PWDInfo'");
        return IVS_XML_INVALID;
    }
    const IVS_CHAR* szElemValue = NULL;
    GET_ELEM_VALUE_CHAR("LoginPWD", szElemValue, pPWD, IVS_PWD_LEN, xml);

    return IVS_SUCCEED;

}

IVS_INT32 CUserInfoMgrXMLProcess::GetDefaultRoleAccountRuleParseXML(CXml &xml, IVS_ROLE_ACCOUNT_RULE* pRoleAccountRule)
{
    CHECK_POINTER(pRoleAccountRule, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    if(!xml.FindElemEx("Content/RoleRuleInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User Info Parse XML", "Can't find 'Content/RoleRuleInfo'");
        return IVS_XML_INVALID;
    }
    const IVS_CHAR* szElemValue = NULL;
    //(void)xml.IntoElem();
    GET_ELEM_VALUE_NUM("IsEnable", szElemValue, pRoleAccountRule->bEnable, xml);
    if(xml.FindElem("PWDConsist"))
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_NUM("PWDConsistCapital", szElemValue, pRoleAccountRule->stPswConsist.bCapital, xml);
        GET_ELEM_VALUE_NUM("PWDConsistLowercase", szElemValue, pRoleAccountRule->stPswConsist.bLowercase, xml);
        GET_ELEM_VALUE_NUM("PWDConsistFigure", szElemValue, pRoleAccountRule->stPswConsist.bFigure, xml);
        GET_ELEM_VALUE_NUM("PWDConsistSpecial", szElemValue, pRoleAccountRule->stPswConsist.bSpecialChar, xml);
        xml.OutOfElem();
    }

    GET_ELEM_VALUE_NUM("ACCFirstModify", szElemValue, pRoleAccountRule->bFirstModify, xml);
    GET_ELEM_VALUE_NUM("ACCCheckOldPWD", szElemValue, pRoleAccountRule->bCheckOldPwd, xml);
    GET_ELEM_VALUE_NUM("PWDAccountNotFit", szElemValue, pRoleAccountRule->bAccountNotFit, xml);
    GET_ELEM_VALUE_NUM("PWDAccountBackNotFit", szElemValue, pRoleAccountRule->bAccountBackNotFit, xml);
    GET_ELEM_VALUE_NUM("DomainACCCheckPWD", szElemValue, pRoleAccountRule->bDomainAccCheckPwd, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDMinLength", szElemValue, pRoleAccountRule->uiPwdMinLen, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDDiferentNumber", szElemValue, pRoleAccountRule->uiPwdDifferentNum, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockNumber", szElemValue, pRoleAccountRule->uiAccountLockNum, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockElapse", szElemValue, pRoleAccountRule->uiAccountLockElapse, xml);

    GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockInterval", szElemValue, pRoleAccountRule->uiAccountLockInterval, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireTime", szElemValue, pRoleAccountRule->uiPWDExpireTime, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireNotifyTime", szElemValue, pRoleAccountRule->uiPWDExpireNotifyTime, xml);
    GET_ELEM_VALUE_NUM("PWDExpireForceNotify", szElemValue, pRoleAccountRule->bPWDExpireForceNotify, xml);
    //xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_INT32 CUserInfoMgrXMLProcess::GetOperationRightGroupParseXML(CXml &xml, IVS_OPERATION_RIGHT_GROUP_LIST* pRightGroupList)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER(pRightGroupList, IVS_OPERATE_MEMORY_ERROR);

    IVS_UINT32 uiSize = 0;
	IVS_CHAR chTemp[IVS_MAX_OPERATION_RIGHT+1] = {0};
    if(xml.FindElemEx("Content/OPGroupList/OPGroupInfo"))
    {
        do{
            (void)xml.IntoElem();

            IVS_OPERATION_RIGHT_GROUP &stRightGroup = pRightGroupList->stOperationRightGroup[uiSize];
            memset(&stRightGroup, 0, sizeof(IVS_OPERATION_RIGHT_GROUP));
            const IVS_CHAR* szElemValue = NULL;
            GET_ELEM_VALUE_CHAR("OPGroupName", szElemValue, stRightGroup.cRightGroupName, IVS_MAX_RIGHT_GROUP_NAME_LEN, xml);
            memset(chTemp, 0, IVS_MAX_OPERATION_RIGHT+1);
            GET_ELEM_VALUE_CHAR("OPInfo", szElemValue, chTemp, IVS_MAX_OPERATION_RIGHT, xml);
            if (!CToolsHelp::Memcpy(stRightGroup.cRight, IVS_MAX_OPERATION_RIGHT, chTemp, IVS_MAX_OPERATION_RIGHT))
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "stRightGroup.cRight", "Memcpy failed");
                return IVS_ALLOC_MEMORY_ERROR;
            }
            xml.OutOfElem();

            uiSize ++;
        }while (xml.NextElem());
    }

    pRightGroupList->uiTotal = uiSize;
    pRightGroupList->stIndexRange.uiFromIndex = 1;
    pRightGroupList->stIndexRange.uiToIndex = uiSize;

    return IVS_SUCCEED;
}

IVS_VOID CUserInfoMgrXMLProcess::GetPWDExpireInfoGetXML(const IVS_CHAR* pLoginID, CXml &xml)
{
    CHECK_POINTER_VOID(pLoginID);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommXmlHead(xml);

    (void)xml.AddChildElem("LoginInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("LoginID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pLoginID);
    xml.OutOfElem();
}

IVS_INT32 CUserInfoMgrXMLProcess::GetPWDExpireInfoParseXML(CXml &xml, IVS_PWD_EXPIRE_INFO* pPWDExpireInfo)
{
    CHECK_POINTER(pPWDExpireInfo, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    if(!xml.FindElemEx("Content"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get PWD Expire Info ParseXML", "Can't find 'Content'");
        return IVS_XML_INVALID;
    }
    const IVS_CHAR* szElemValue = NULL;
    GET_ELEM_VALUE_NUM("IsNeedWarning", szElemValue, pPWDExpireInfo->bIsNeedWarning, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("RemainDays", szElemValue, pPWDExpireInfo->uiRemainDays, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ExpiredDays", szElemValue, pPWDExpireInfo->uiExpiredDays, xml);

    return IVS_SUCCEED;
}

IVS_VOID CUserInfoMgrXMLProcess::CommXmlHead(CXml &xml)
{
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
}

IVS_VOID CUserInfoMgrXMLProcess::CommDomainCodeXml(const IVS_CHAR* pDomainCode, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommXmlHead(xml);

    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);
}

IVS_VOID CUserInfoMgrXMLProcess::CommQueryXml(const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, CXml &xml)
{
    CHECK_POINTER_VOID(pDomainCode);
    CHECK_POINTER_VOID(pIndexRange);
    IVS_DEBUG_TRACE("");

    CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

    (void)xml.AddElem("PageInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("FromIndex");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiFromIndex)).c_str());
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiToIndex)).c_str());
}


//构造根据摄像机Code，查询该摄像机正在给哪些用户提供视频业务的请求xml
IVS_INT32 CUserInfoMgrXMLProcess::GetStreamListByCamGetXML(const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);

	CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

	(void)xml.AddElem("CamInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	IVS_CHAR cTmpCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Strncpy(cTmpCameraCode, IVS_DEV_CODE_LEN + 1, pCameraCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByCamGetXML", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTmpCameraCode);

	return IVS_SUCCEED;
}


//解析根据摄像机Code，查询该摄像机正在给哪些用户提供视频业务的响应xml
IVS_INT32 CUserInfoMgrXMLProcess::GetStreamListByCamParseXML(CXml &xml,
	IVS_UINT32 uiBufSize,
	IVS_STREAM_LIST_BY_CAM* pStreamListByCam)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pStreamListByCam, IVS_PARA_INVALID);

	if (sizeof(IVS_STREAM_LIST_BY_CAM) + (IVS_MAX_STREAM_USER_INFO_NUM -1) * sizeof(IVS_STREAM_USER_INFO)
		!= uiBufSize)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"GetStreamListByCamParseXML", "uiBufSize should equal %d",
			sizeof(IVS_STREAM_LIST_BY_CAM) + (IVS_MAX_STREAM_USER_INFO_NUM -1) * sizeof(IVS_STREAM_USER_INFO));
		return IVS_PARA_INVALID;
	}

	memset(pStreamListByCam, 0, uiBufSize);

	if(!xml.FindElemEx("Content/CamInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetStreamListByCamParseXML", "Can't find 'CamInfo'");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* szElemValue = NULL;
	GET_ELEM_VALUE_CHAR("CameraCode", szElemValue, pStreamListByCam->cCameraCode, IVS_DEV_CODE_LEN, xml);

	if(xml.FindElemEx("Content/StreamList/StreamInfo"))
	{
		pStreamListByCam->uiTotal = 0;
		do{
			(void)xml.IntoElem();

			IVS_STREAM_USER_INFO stStreamUserInfo = {0};
			GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, stStreamUserInfo.uiUserID, xml);
			GET_ELEM_VALUE_CHAR("UserName", szElemValue, stStreamUserInfo.cUserName, IVS_NAME_LEN, xml);
			GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, stStreamUserInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("StreamType", szElemValue, stStreamUserInfo.uiStreamType, xml);

			if (!CToolsHelp::Memcpy(&(pStreamListByCam->stStreamUserInfo[pStreamListByCam->uiTotal]), sizeof(IVS_STREAM_USER_INFO), 
				&stStreamUserInfo, sizeof(IVS_STREAM_USER_INFO)))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByCamParseXML", "Memcpy failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			pStreamListByCam->uiTotal ++;

			xml.OutOfElem();
		}while (xml.NextElem());
	}

	return IVS_SUCCEED;
}


//构造根据用户编码，查询该用户正在给调用哪些摄像机的视频业务的请求xml
IVS_INT32 CUserInfoMgrXMLProcess::GetStreamListByUserGetXML(const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiUserID,
	CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);

	CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

	(void)xml.AddElem("UserInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("UserID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(uiUserID).c_str());

	return IVS_SUCCEED;
}


//解析根据用户编码，查询该用户正在给调用哪些摄像机的视频业务的响应xml
IVS_INT32 CUserInfoMgrXMLProcess::GetStreamListByUserParseXML(CXml &xml,
	IVS_UINT32 uiBufSize,
	IVS_STREAM_LIST_BY_USER* pStreamListByUser)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pStreamListByUser, IVS_PARA_INVALID);

	if (sizeof(IVS_STREAM_LIST_BY_USER) + (IVS_MAX_STREAM_CAM_INFO_NUM -1) * sizeof(IVS_STREAM_CAM_INFO)
		!= uiBufSize)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"GetStreamListByUserParseXML", "uiBufSize should equal %d",
			sizeof(IVS_STREAM_LIST_BY_USER) + (IVS_MAX_STREAM_CAM_INFO_NUM -1) * sizeof(IVS_STREAM_CAM_INFO));
		return IVS_PARA_INVALID;
	}

	memset(pStreamListByUser, 0, uiBufSize);

	if(!xml.FindElemEx("Content/UserInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetStreamListByUserParseXML", "Can't find 'UserInfo'");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* szElemValue = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, pStreamListByUser->uiUserID, xml);

	if(xml.FindElemEx("Content/StreamList/StreamInfo"))
	{
		pStreamListByUser->uiTotal = 0;
		do{
			(void)xml.IntoElem();

			IVS_STREAM_CAM_INFO stStreamCamInfo = {0};
			GET_ELEM_VALUE_CHAR("CameraCode", szElemValue, stStreamCamInfo.cCameraCode, IVS_DEV_CODE_LEN, xml);
			GET_ELEM_VALUE_CHAR("CameraName", szElemValue, stStreamCamInfo.cCameraName, IVS_NAME_LEN, xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("StreamType", szElemValue, stStreamCamInfo.uiStreamType, xml);

			if (!CToolsHelp::Memcpy(&(pStreamListByUser->stStreamCamInfo[pStreamListByUser->uiTotal]), sizeof(IVS_STREAM_CAM_INFO), 
				&stStreamCamInfo, sizeof(IVS_STREAM_CAM_INFO)))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByUserParseXML", "Memcpy failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			pStreamListByUser->uiTotal ++;

			xml.OutOfElem();
		}while (xml.NextElem());
	}

	return IVS_SUCCEED;
}


//构造根据摄像机Code和用户ID，关闭该用户正在使用的视频业务的响应xml
IVS_INT32 CUserInfoMgrXMLProcess::ShutdownStreamByUserGetXML(const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiUserID, 
	const IVS_CHAR* pCameraCode,
	IVS_INT32 iStreamType,
	IVS_INT32 iDuration,
	CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);

	CUserInfoMgrXMLProcess::CommDomainCodeXml(pDomainCode, xml);

	(void)xml.AddElem("UserID");
	std::string strUserID = CToolsHelp::Int2Str(uiUserID);
	(void)xml.SetElemValue(strUserID.c_str());

	(void)xml.AddElem("CameraCode");
	IVS_CHAR cTmpCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Strncpy(cTmpCameraCode, IVS_DEV_CODE_LEN + 1, pCameraCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "ShutdownStreamByUserGetXML", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTmpCameraCode);

	(void)xml.AddElem("StreamType");
	std::string strStreamType = CToolsHelp::Int2Str(iStreamType);
	(void)xml.SetElemValue(strStreamType.c_str());
	(void)xml.AddElem("Duration");
	std::string strDuration = CToolsHelp::Int2Str(iDuration);
	(void)xml.SetElemValue(strDuration.c_str());

	return IVS_SUCCEED;
}


//构造验证登录用户名和密码的响应xml
IVS_INT32 CUserInfoMgrXMLProcess::PasswdValidationGetXML(const std::string &strLoginName,
	const std::string &strPasswd,
	CXml &xml)
{
	IVS_DEBUG_TRACE("");
	if (strLoginName.empty() || strPasswd.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Passwd Validation GetXML failed", 
			"LoginName: %s, PassWD: %s", strLoginName.c_str(), strPasswd.c_str());
		return IVS_PARA_INVALID;
	}

	CUserInfoMgrXMLProcess::CommXmlHead(xml);

	(void)xml.AddChildElem("LoginName");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strLoginName.c_str());

	(void)xml.AddElem("LoginPWD");
	(void)xml.SetElemValue(strPasswd.c_str());

	return IVS_SUCCEED;
}