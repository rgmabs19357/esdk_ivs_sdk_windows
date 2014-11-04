#include "stdafx.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "OCXUserInfoMgrXMLProcess.h"
#include "IVS_Trace.h"


COCXUserInfoMgrXMLProcess::COCXUserInfoMgrXMLProcess()
{

}

COCXUserInfoMgrXMLProcess::~COCXUserInfoMgrXMLProcess()
{

}

IVS_INT32 COCXUserInfoMgrXMLProcess::GetUserInfoParseXml(LPCTSTR cstrRet, IVS_USER_INFO &pUserInfo)
{
    IVS_DEBUG_TRACE("");

    CXml xml;
    if(!xml.Parse(cstrRet))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get UserInfo ParseXml", "xml parsing error");
        return IVS_XML_INVALID;
    }
    const char* szElemValue = NULL;
    if(xml.FindElemEx("Content/DomainCode"))
    {
        GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pUserInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, xml);
    }
    if(!xml.FindElemEx("Content/UserList/UserInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get UserInfo ParseXml", "Can't find 'Content/UserList/UserInfo'");
        return IVS_XML_INVALID;
    }
    xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, pUserInfo.uiUserID, xml);
    GET_ELEM_VALUE_CHAR("LoginName", szElemValue, pUserInfo.cLoginName, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_CHAR("LoginPWD", szElemValue, pUserInfo.cPWD, IVS_PWD_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("UserType", szElemValue, pUserInfo.uiUserType, xml);
    GET_ELEM_VALUE_CHAR("UserDomain", szElemValue, pUserInfo.cUserDomain, IVS_DOMAIN_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("Status", szElemValue, pUserInfo.uiStatus, xml);
    GET_ELEM_VALUE_CHAR("RegisterDate", szElemValue, pUserInfo.cRegiterDate, IVS_TIME_LEN, xml);
    GET_ELEM_VALUE_CHAR("UserDesc", szElemValue, pUserInfo.cUserDesc, IVS_DESCRIBE_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ControlLevel", szElemValue, pUserInfo.uiPTZLevel, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("MaxVideoCNT", szElemValue, pUserInfo.uiMaxVideoCnt, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("MaxSessionCNT", szElemValue, pUserInfo.uiMaxSessionCnt, xml);
    GET_ELEM_VALUE_CHAR("UserName", szElemValue, pUserInfo.cUserName, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_CHAR("Email", szElemValue, pUserInfo.cEmail, IVS_EMAILE_LEN, xml);
    GET_ELEM_VALUE_CHAR("Phone", szElemValue, pUserInfo.cPhone, IVS_PHONE_NUMBER_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("Sex", szElemValue, pUserInfo.uiSex, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("UserGroupID", szElemValue, pUserInfo.uiGroupID, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("RoleID", szElemValue, pUserInfo.uiRoleID, xml);

    // add by wanglei 00165153:2013.10.22 用户有效期特性
    GET_ELEM_VALUE_NUM_FOR_UINT("ValidDateFlag", szElemValue, pUserInfo.uiValidDateFlag, xml);
    GET_ELEM_VALUE_CHAR("ValidDateStart", szElemValue, pUserInfo.cValidDateStart, IVS_TIME_LEN, xml);
    GET_ELEM_VALUE_CHAR("ValidDateEnd", szElemValue, pUserInfo.cValidDateEnd, IVS_TIME_LEN, xml);
    // end add

    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetUserInfoGetXml(const IVS_USER_INFO* pUserInfo, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserInfo);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("DomainCode");
    IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, pUserInfo->cDomainCode, IVS_DOMAIN_CODE_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cDomainCode", "Memcpy failed");
        //return IVS_ALLOC_MEMORY_ERROR;
    }
    (void)xml.SetElemValue(cDomainCode);
    (void)xml.AddElem("UserList");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiUserID)).c_str());    
    (void)xml.AddElem("LoginPWD");
    IVS_CHAR cPWD[IVS_PWD_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cPWD, IVS_PWD_LEN, pUserInfo->cPWD, IVS_PWD_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cPWD", "Memcpy failed");
    }
    (void)xml.SetElemValue(cPWD);
    (void)xml.AddElem("LoginName");
    IVS_CHAR cLoginName[IVS_NAME_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cLoginName, IVS_NAME_LEN, pUserInfo->cLoginName, IVS_NAME_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cLoginName", "Memcpy failed");
    }
    (void)xml.SetElemValue(cLoginName);    
    (void)xml.AddElem("UserDomain");
    IVS_CHAR cUserDomain[IVS_DOMAIN_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cUserDomain, IVS_DOMAIN_LEN, pUserInfo->cUserDomain, IVS_DOMAIN_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cUserDomain", "Memcpy failed");
    }
    (void)xml.SetElemValue(cUserDomain);
    (void)xml.AddElem("UserType");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiUserType)).c_str());
    (void)xml.AddElem("Status");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiStatus)).c_str());
    (void)xml.AddElem("RegisterDate");
    IVS_CHAR cRegiterDate[IVS_TIME_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cRegiterDate, IVS_TIME_LEN, pUserInfo->cRegiterDate, IVS_TIME_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cRegiterDate", "Memcpy failed");
    }
    (void)xml.SetElemValue(cRegiterDate);
    (void)xml.AddElem("UserDesc");
    IVS_CHAR cUserDesc[IVS_DESCRIBE_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cUserDesc, IVS_DESCRIBE_LEN, pUserInfo->cUserDesc, IVS_DESCRIBE_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cUserDesc", "Memcpy failed");
    }
    (void)xml.SetElemValue(cUserDesc);
    (void)xml.AddElem("ControlLevel");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiPTZLevel)).c_str());
    (void)xml.AddElem("MaxVideoCNT");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiMaxVideoCnt)).c_str());
    (void)xml.AddElem("MaxSessionCNT");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiMaxSessionCnt)).c_str());
    (void)xml.AddElem("Email");
    IVS_CHAR cEmail[IVS_EMAILE_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cEmail, IVS_EMAILE_LEN, pUserInfo->cEmail, IVS_EMAILE_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cEmail", "Memcpy failed");
    }
    (void)xml.SetElemValue(cEmail);
    (void)xml.AddElem("UserName");
    IVS_CHAR cUserName[IVS_NAME_LEN + 1] = {0};;
    if (!CToolsHelp::Memcpy(cUserName, IVS_NAME_LEN, pUserInfo->cUserName, IVS_NAME_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cUserName", "Memcpy failed");
    }
    (void)xml.SetElemValue(cUserName);    
    (void)xml.AddElem("Phone");
    IVS_CHAR cPhone[IVS_PHONE_NUMBER_LEN + 1] = {0};
    if (!CToolsHelp::Memcpy(cPhone, IVS_PHONE_NUMBER_LEN , pUserInfo->cPhone, IVS_PHONE_NUMBER_LEN))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cPhone", "Memcpy failed");
    }
    (void)xml.SetElemValue(cPhone);
    (void)xml.AddElem("Sex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiSex)).c_str());
    (void)xml.AddElem("UserGroupID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiGroupID)).c_str());
    (void)xml.AddElem("RoleID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiRoleID)).c_str());

    // add by wanglei 00165153:2013.10.22 用户有效性特性
    (void)xml.AddElem("ValidDateFlag");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiValidDateFlag).c_str());

    if (strlen(pUserInfo->cValidDateStart))
    {
        (void)xml.AddElem("ValidDateStart");
        (void)xml.SetElemValue(pUserInfo->cValidDateStart);
    }
    if (strlen(pUserInfo->cValidDateStart))
    {
        (void)xml.AddElem("ValidDateEnd");
        (void)xml.SetElemValue(pUserInfo->cValidDateEnd);
    }
    // end add

    xml.OutOfElem();
    xml.OutOfElem();
    xml.OutOfElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::AddUserGetXml(IVS_UINT32 uiUserID, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());

    (void)xml.AddElem("UserInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((int)uiUserID)).c_str());
    xml.OutOfElem();

    xml.OutOfElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetUserListGetXml(const IVS_USER_INFO_LIST *pUserInfoList, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserInfoList);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("PageInfo");
    (void)xml.IntoElem();
    //头部分页信息
    (void)xml.AddChildElem("RealNum");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfoList->uiTotal).c_str());
    (void)xml.AddElem("FromIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfoList->stIndexRange.uiFromIndex).c_str());
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfoList->stIndexRange.uiToIndex).c_str());
    xml.OutOfElem();

    const IVS_USER_INFO* pUserInfo;
    (void)xml.AddElem("UserList");
    (void)xml.IntoElem();
    IVS_INT32 iNum = (IVS_INT32)(pUserInfoList->stIndexRange.uiToIndex - pUserInfoList->stIndexRange.uiFromIndex) + 1; //查询的范围
    for (IVS_INT32 i = 0; i < iNum; i++)
    {
        pUserInfo = &pUserInfoList->stUserInfo[i];
        if(i > 0)
        {
            (void)xml.AddElem("UserInfo");
        }
        else
        {
            (void)xml.AddChildElem("UserInfo");
        }
        (void)xml.IntoElem();
        (void)xml.AddChildElem("UserID");
        (void)xml.IntoElem();
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiUserID)).c_str());
        (void)xml.AddElem("LoginName");
        IVS_CHAR cLoginName[IVS_NAME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cLoginName, IVS_NAME_LEN, pUserInfo->cLoginName, IVS_NAME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cLoginName", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cLoginName);
        (void)xml.AddElem("LoginPWD");
        IVS_CHAR cPWD[IVS_PWD_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cPWD, IVS_PWD_LEN, pUserInfo->cPWD, IVS_PWD_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cPWD", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cPWD);
        (void)xml.AddElem("UserType");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiUserType)).c_str());
        (void)xml.AddElem("UserDomain");
        IVS_CHAR cUserDomain[IVS_DOMAIN_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cUserDomain, IVS_DOMAIN_LEN, pUserInfo->cUserDomain, IVS_DOMAIN_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cUserDomain", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cUserDomain);
        (void)xml.AddElem("Status");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiStatus)).c_str());
        (void)xml.AddElem("UserDesc");
        IVS_CHAR cUserDesc[IVS_DESCRIBE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cUserDesc, IVS_DESCRIBE_LEN, pUserInfo->cUserDesc, IVS_DESCRIBE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cUserDesc", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cUserDesc);
        (void)xml.AddElem("RegisterDate");
        IVS_CHAR cRegiterDate[IVS_TIME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cRegiterDate, IVS_TIME_LEN, pUserInfo->cRegiterDate, IVS_TIME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cRegiterDate", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cRegiterDate);
        (void)xml.AddElem("ControlLevel");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiPTZLevel)).c_str());
        (void)xml.AddElem("MaxSessionCNT");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiMaxSessionCnt)).c_str());
        (void)xml.AddElem("MaxVideoCNT");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiMaxVideoCnt)).c_str());
        (void)xml.AddElem("UserName");
        IVS_CHAR cUserName[IVS_NAME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cUserName, IVS_NAME_LEN, pUserInfo->cUserName, IVS_NAME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cUserName", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cUserName);
        (void)xml.AddElem("Email");
        IVS_CHAR cEmail[IVS_EMAILE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cEmail, IVS_EMAILE_LEN, pUserInfo->cEmail, IVS_EMAILE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cEmail", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cEmail);
        (void)xml.AddElem("Phone");
        IVS_CHAR cPhone[IVS_PHONE_NUMBER_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cPhone, IVS_PHONE_NUMBER_LEN, pUserInfo->cPhone, IVS_PHONE_NUMBER_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserInfo->cPhone", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cPhone);
        (void)xml.AddElem("UserGroupID");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiGroupID).c_str());
        (void)xml.AddElem("Sex");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiSex).c_str());
        (void)xml.AddElem("RoleID");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiRoleID).c_str());

        // add by wanglei 00165153:2013.10.22 用户有效性特性
        (void)xml.AddElem("ValidDateFlag");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserInfo->uiValidDateFlag).c_str());

        if (strlen(pUserInfo->cValidDateStart))
        {
            (void)xml.AddElem("ValidDateStart");
            (void)xml.SetElemValue(pUserInfo->cValidDateStart);
        }
        if (strlen(pUserInfo->cValidDateStart))
        {
            (void)xml.AddElem("ValidDateEnd");
            (void)xml.SetElemValue(pUserInfo->cValidDateEnd);
        }
        // end add

        xml.OutOfElem();
    }	

    xml.OutOfElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetOnlineUserListGetXml(const IVS_ONLINE_USER_LIST *pOnlieUserInfo, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pOnlieUserInfo);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("PageInfo");
    (void)xml.AddChildElem("RealNum");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pOnlieUserInfo->uiTotal)).c_str());
    (void)xml.AddElem("FromIndex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pOnlieUserInfo->stIndexRange.uiFromIndex)).c_str());
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pOnlieUserInfo->stIndexRange.uiToIndex)).c_str());
    xml.OutOfElem();

    const IVS_ONLINE_USER* pOnlineUser;
    (void)xml.AddElem("LoginList");
    (void)xml.IntoElem();
    IVS_INT32 iNum = (IVS_INT32)(pOnlieUserInfo->stIndexRange.uiToIndex - pOnlieUserInfo->stIndexRange.uiFromIndex) + 1;
    for (IVS_INT32 i = 0; i < iNum; i++)
    {
        pOnlineUser = &pOnlieUserInfo->stOnlineUser[i];
        if(i > 0)
        {
            (void)xml.AddElem("LoginInfo");
        }
        else
        {
            (void)xml.AddChildElem("LoginInfo");
        }
        (void)xml.IntoElem();
        (void)xml.AddChildElem("ID");
        (void)xml.IntoElem();
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pOnlineUser->uiUserOnlineID)).c_str());
        (void)xml.AddElem("UserID");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pOnlineUser->uiUserID)).c_str());
        (void)xml.AddElem("LoginName");
        IVS_CHAR cUserName[IVS_NAME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cUserName, IVS_NAME_LEN, pOnlineUser->cUserName, IVS_NAME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pOnlineUser->cUserName", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cUserName);
        (void)xml.AddElem("DomainName");
        IVS_CHAR cDomainName[IVS_DOMAIN_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cDomainName, IVS_DOMAIN_LEN, pOnlineUser->cDomainName, IVS_DOMAIN_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pOnlineUser->cDomainName", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cDomainName);
        (void)xml.AddElem("ClientType");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pOnlineUser->uiClientType)).c_str());
        (void)xml.AddElem("IPAddress");
        IVS_CHAR cIP[IVS_IP_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cIP, IVS_IP_LEN, pOnlineUser->stIP.cIP, IVS_IP_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pOnlineUser->stIP.cIP", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cIP);
        (void)xml.AddElem("MachineName");
        IVS_CHAR cMachineName[IVS_MACHINE_NAME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cMachineName, IVS_MACHINE_NAME_LEN, pOnlineUser->cMachineName, IVS_MACHINE_NAME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pOnlineUser->cMachineName", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cMachineName);
        (void)xml.AddElem("LoginTime");
        IVS_CHAR cLoginTime[IVS_TIME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cLoginTime, IVS_TIME_LEN, pOnlineUser->cLoginTime, IVS_TIME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pOnlineUser->cLoginTime", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cLoginTime);        
        xml.OutOfElem();
    }	

    xml.OutOfElem();
}

IVS_INT32 COCXUserInfoMgrXMLProcess::SetUserDevListParseXml(LPCTSTR cstrRet, IVS_UINT32 &uiUserID, IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_CHAR* pDomainCode)
{
    IVS_DEBUG_TRACE("");
    CHECK_POINTER(pUserDevList, IVS_PARA_INVALID);

    CXml xml;
    if(!xml.Parse(cstrRet))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set User Dev List ParseXml", "xml parsing error");
        return IVS_XML_INVALID;
    }
    const char* szElemValue = NULL;
    if(!xml.FindElemEx("Content/UserInfo"))
    {
        BP_RUN_LOG_INF("Set User Dev List ParseXml", "Can't find 'Content/UserInfo'");
        return IVS_XML_INVALID;
    }
    (void)xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("UserID", szElemValue, uiUserID, xml);
    GET_ELEM_VALUE_CHAR("UserDomainCode", szElemValue, pDomainCode, IVS_DOMAIN_CODE_LEN, xml);
    xml.OutOfElem();

    if (xml.FindElemEx("Content/DevList/DevInfo"))
    {
        int iSize = 0;
        do{
            (void)xml.IntoElem();
            IVS_USER_DEVICE_INFO &stUserDev = pUserDevList->stUserDeviceInfo[iSize];
            memset(&stUserDev, 0, sizeof(IVS_USER_DEVICE_INFO));

            GET_ELEM_VALUE_CHAR("DevGroupCode", szElemValue, stUserDev.cDevGroupCode, IVS_DEVICE_GROUP_LEN, xml);
            GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, stUserDev.cDevDomainCode, IVS_DOMAIN_CODE_LEN, xml);
            GET_ELEM_VALUE_NUM("IsExDomain", szElemValue, stUserDev.bIsExDomain, xml);
            GET_ELEM_VALUE_NUM("IsTotalGroup", szElemValue, stUserDev.bIsTotalGroup, xml);

            if(!stUserDev.bIsTotalGroup)
            {
                const IVS_INT32 maxLen = IVS_DEV_CODE_LEN;
                IVS_CHAR chTemp[maxLen] = {0};
                IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
                IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
                //主设备编码
                GET_ELEM_VALUE_CHAR("DevCode", szElemValue, chTemp, maxLen, xml);
                (void)COCXXmlProcess::ParseDevCode(chTemp, chDevCode, chDomaCode);
                if (!CToolsHelp::Memcpy(stUserDev.cDevCode, IVS_DEV_CODE_LEN, chDevCode, IVS_DEV_CODE_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "chDevCode memcpy fail", "NA");
                    return IVS_ALLOC_MEMORY_ERROR;
                }
                if (!CToolsHelp::Memcpy(stUserDev.cDevDomainCode, IVS_DOMAIN_CODE_LEN, chDomaCode, IVS_DOMAIN_CODE_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "chDevCode memcpy fail", "NA");
                    return IVS_ALLOC_MEMORY_ERROR;
                }
            }
            
            xml.OutOfElem();
            iSize++;
        }while (xml.NextElem());
    }

    return IVS_SUCCEED;
}

IVS_INT32 COCXUserInfoMgrXMLProcess::GetUserDevListGetXml(const IVS_USER_DEVICE_INFO_LIST *pUserDevList, IVS_INT32 iRet, CXml &xml)
{
    CHECK_POINTER(pUserDevList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("PageInfo");
    (void)xml.AddChildElem("RealNum");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserDevList->uiTotal)).c_str());
    (void)xml.AddElem("FromIndex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserDevList->stIndexRange.uiFromIndex)).c_str());
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserDevList->stIndexRange.uiToIndex)).c_str());
    xml.OutOfElem();

    const IVS_USER_DEVICE_INFO* pUserDev;
    (void)xml.AddElem("DevList");
    (void)xml.IntoElem();
    IVS_UINT32 iNum = (pUserDevList->stIndexRange.uiToIndex - pUserDevList->stIndexRange.uiFromIndex) + 1;//查询的范围
    for (IVS_UINT32 i = 0; i < iNum; i++)
    {
        pUserDev = &pUserDevList->stUserDeviceInfo[i];
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
        IVS_CHAR cDevGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cDevGroupCode, IVS_DEVICE_GROUP_LEN, pUserDev->cDevGroupCode, IVS_DEVICE_GROUP_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserDev->cDevGroupCode", "Memcpy failed");
            return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cDevGroupCode);
        (void)xml.AddElem("DevDomainCode");
        IVS_CHAR cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDev->cDevDomainCode, IVS_DOMAIN_CODE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserDev->cDevDomainCode", "Memcpy failed");
            return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cDevDomainCode);
        (void)xml.AddElem("DevCode");
        std::string strBigDevCode;
        IVS_CHAR cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cDevCode, IVS_DEV_CODE_LEN, pUserDev->cDevCode, IVS_DEV_CODE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserDev->cDevCode", "Memcpy failed");
            return IVS_ALLOC_MEMORY_ERROR;
        }
        strBigDevCode.append(cDevCode).append("#").append(cDevDomainCode);
        (void)xml.SetElemValue(strBigDevCode.c_str());
        (void)xml.AddElem("IsExDomain");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pUserDev->bIsExDomain).c_str());
        (void)xml.AddElem("IsTotalGroup");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pUserDev->bIsTotalGroup).c_str());

        xml.OutOfElem();
    }
    xml.OutOfElem();

    return IVS_SUCCEED;
}


IVS_INT32 COCXUserInfoMgrXMLProcess::GetRoleInfoParseXML(LPCTSTR cstrRet, IVS_ROLE_INFO &pRoleInfo)
{
    IVS_DEBUG_TRACE("");

    CXml xml;
    if(!xml.Parse(cstrRet))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get RoleInfo ParseXML", "xml parsing error");
        return IVS_XML_INVALID;
    }
    const char* szElemValue = NULL;
    if(xml.FindElemEx("Content/DomainCode"))
    {
        GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pRoleInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, xml);
    }
    if(!xml.FindElemEx("Content/RoleInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get UserInfo ParseXml", "Can't find 'Content/RoleInfo'");
        return IVS_XML_INVALID;
    }
    xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("RoleID", szElemValue, pRoleInfo.uiRoleID, xml);
    GET_ELEM_VALUE_CHAR("RoleName", szElemValue, pRoleInfo.cRoleName, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_CHAR("RoleDesc", szElemValue, pRoleInfo.cRoleDescribe, IVS_DESCRIBE_LEN, xml);

    if(xml.FindElem("RoleOPInfo"))
    {
        xml.IntoElem();
        GET_ELEM_VALUE_CHAR("OPInfo", szElemValue, pRoleInfo.cRight, IVS_MAX_OPERATION_RIGHT, xml);
        xml.OutOfElem();
    }
    if(xml.FindElem("RoleRuleInfo"))
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_NUM("IsEnable", szElemValue, pRoleInfo.stAccountRule.bEnable, xml);
        if(xml.FindElem("PWDConsist"))
        {
            (void)xml.IntoElem();
            GET_ELEM_VALUE_NUM("PWDConsistCapital", szElemValue, pRoleInfo.stAccountRule.stPswConsist.bCapital, xml);
            GET_ELEM_VALUE_NUM("PWDConsistLowercase", szElemValue, pRoleInfo.stAccountRule.stPswConsist.bLowercase, xml);
            GET_ELEM_VALUE_NUM("PWDConsistFigure", szElemValue, pRoleInfo.stAccountRule.stPswConsist.bFigure, xml);
            GET_ELEM_VALUE_NUM("PWDConsistSpecial", szElemValue, pRoleInfo.stAccountRule.stPswConsist.bSpecialChar, xml);
            xml.OutOfElem();
        }
        GET_ELEM_VALUE_NUM("ACCFirstModify", szElemValue, pRoleInfo.stAccountRule.bFirstModify, xml);
        GET_ELEM_VALUE_NUM("ACCCheckOldPWD", szElemValue, pRoleInfo.stAccountRule.bCheckOldPwd, xml);
        GET_ELEM_VALUE_NUM("PWDAccountNotFit", szElemValue, pRoleInfo.stAccountRule.bAccountNotFit, xml);
        GET_ELEM_VALUE_NUM("PWDAccountBackNotFit", szElemValue, pRoleInfo.stAccountRule.bAccountBackNotFit, xml);
        GET_ELEM_VALUE_NUM("DomainACCCheckPWD", szElemValue, pRoleInfo.stAccountRule.bDomainAccCheckPwd, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("PWDMinLength", szElemValue, pRoleInfo.stAccountRule.uiPwdMinLen, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("PWDDiferentNumber", szElemValue, pRoleInfo.stAccountRule.uiPwdDifferentNum, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockNumber", szElemValue, pRoleInfo.stAccountRule.uiAccountLockNum, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockElapse", szElemValue, pRoleInfo.stAccountRule.uiAccountLockElapse, xml);

        GET_ELEM_VALUE_NUM_FOR_UINT("ACCLockInterval", szElemValue, pRoleInfo.stAccountRule.uiAccountLockInterval, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireTime", szElemValue, pRoleInfo.stAccountRule.uiPWDExpireTime, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("PWDExpireNotifyTime", szElemValue, pRoleInfo.stAccountRule.uiPWDExpireNotifyTime, xml);
        GET_ELEM_VALUE_NUM("PWDExpireForceNotify", szElemValue, pRoleInfo.stAccountRule.bPWDExpireForceNotify, xml);
        xml.OutOfElem();                       
    }
    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetRoleListGetXml(const IVS_ROLE_INFO_LIST *pRoleList, IVS_INT32 iCode, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pRoleList);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iCode)).c_str());
    (void)xml.AddElem("PageInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("RealNum");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pRoleList->uiTotal)).c_str());
    (void)xml.AddElem("FromIndex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pRoleList->stIndexRange.uiFromIndex)).c_str());
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pRoleList->stIndexRange.uiToIndex)).c_str());
    xml.OutOfElem();

    const IVS_ROLE_INFO* pRole;
    (void)xml.AddElem("RoleList");
    (void)xml.IntoElem();
    IVS_INT32 iNum = (IVS_INT32)(pRoleList->stIndexRange.uiToIndex - pRoleList->stIndexRange.uiFromIndex) + 1; //查询的范围
    for (IVS_INT32 i = 0; i < iNum; i++)
    {
        pRole = &pRoleList->stRoleInfo[i];
        if(i > 0)
        {
            (void)xml.AddElem("RoleInfo");
        }
        else
        {
            (void)xml.AddChildElem("RoleInfo");
        }

        (void)xml.IntoElem();
        (void)xml.AddChildElem("RoleID");
        (void)xml.IntoElem();
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pRole->uiRoleID)).c_str());

        (void)xml.AddElem("RoleName");
        IVS_CHAR cRoleName[IVS_NAME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cRoleName, IVS_NAME_LEN, pRole->cRoleName, IVS_NAME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pRole->cRoleName", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cRoleName);

        (void)xml.AddElem("RoleDesc");
        IVS_CHAR cRoleDescribe[IVS_DESCRIBE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cRoleDescribe, IVS_DESCRIBE_LEN, pRole->cRoleDescribe, IVS_DESCRIBE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pRole->cRoleDescribe", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cRoleDescribe);

        (void)xml.AddElem("RoleType");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pRole->uiRoleType)).c_str());

        (void)xml.AddElem("RoleOPInfo");
        (void)xml.AddChildElem("OPInfo");
        (void)xml.IntoElem();
        IVS_CHAR cRight[IVS_MAX_OPERATION_RIGHT + 1] = {0};
        if (!CToolsHelp::Memcpy(cRight, IVS_MAX_OPERATION_RIGHT, pRole->cRight, IVS_MAX_OPERATION_RIGHT))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pRole->cRight", "Memcpy failed");
            //return IVS_ALLOC_MEMORY_ERROR;
        }
        (void)xml.SetElemValue(cRight);
        xml.OutOfElem();

        (void)xml.AddElem("RoleRuleInfo");
        (void)xml.AddChildElem("IsEnable");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.bEnable).c_str());

        (void)xml.AddElem("PWDConsist");
        (void)xml.AddChildElem("PWDConsistCapital");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.stPswConsist.bCapital).c_str());
        (void)xml.AddElem("PWDConsistLowercase");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.stPswConsist.bLowercase).c_str());
        (void)xml.AddElem("PWDConsistFigure");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.stPswConsist.bFigure).c_str());
        (void)xml.AddElem("PWDConsistSpecial");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.stPswConsist.bSpecialChar).c_str());
        xml.OutOfElem();

        (void)xml.AddElem("ACCFirstModify");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.bFirstModify).c_str());
        (void)xml.AddElem("ACCCheckOldPWD");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.bAccountNotFit).c_str());
        (void)xml.AddElem("PWDAccountBackNotFit");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.bCheckOldPwd).c_str());
        (void)xml.AddElem("PWDAccountNotFit");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.bAccountBackNotFit).c_str());
        (void)xml.AddElem("DomainACCCheckPWD");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.bDomainAccCheckPwd).c_str());
        (void)xml.AddElem("PWDMinLength");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRole->stAccountRule.uiPwdDifferentNum).c_str());
        (void)xml.AddElem("ACCLockNumber");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRole->stAccountRule.uiPwdMinLen).c_str());
        (void)xml.AddElem("PWDDiferentNumber");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRole->stAccountRule.uiAccountLockNum).c_str());
        (void)xml.AddElem("ACCLockElapse");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRole->stAccountRule.uiAccountLockElapse).c_str());

        (void)xml.AddElem("PWDExpireTime");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRole->stAccountRule.uiPWDExpireTime).c_str());
        (void)xml.AddElem("ACCLockInterval");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRole->stAccountRule.uiAccountLockInterval).c_str());
        (void)xml.AddElem("PWDExpireNotifyTime");
        (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRole->stAccountRule.uiPWDExpireNotifyTime).c_str());
        (void)xml.AddElem("PWDExpireForceNotify");
        (void)xml.SetElemValue(CToolsHelp::Int2Str(pRole->stAccountRule.bPWDExpireForceNotify).c_str());
        xml.OutOfElem();
        xml.OutOfElem();
    }	

    xml.OutOfElem();
}


IVS_VOID COCXUserInfoMgrXMLProcess::GetUserGroupListGetXML(const IVS_USER_GROUP_LIST* pUserGroupList, IVS_INT32 iResult, CXml &xml)
{    
    CHECK_POINTER_VOID(pUserGroupList);
    IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iResult)).c_str());
    //分页信息
    (void)xml.AddElem("PageInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("RealNum");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserGroupList->uiTotal).c_str());
    (void)xml.AddElem("FromIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserGroupList->stIndexRange.uiFromIndex).c_str());
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pUserGroupList->stIndexRange.uiToIndex).c_str());
    xml.OutOfElem();

    const IVS_USER_GROUP* pUserGroup;
    (void)xml.AddElem("GroupList");
    (void)xml.IntoElem();
    IVS_UINT32 iNum = (pUserGroupList->stIndexRange.uiToIndex - pUserGroupList->stIndexRange.uiFromIndex) + 1; //查询的范围
    for (IVS_UINT32 i = 0; i < iNum; i++)
    {
        pUserGroup = &pUserGroupList->stUserGroup[i];
        if(i > 0)
        {
            (void)xml.AddElem("GroupInfo");
        }
        else
        {
            (void)xml.AddChildElem("GroupInfo");
        }
        (void)xml.IntoElem();
        (void)xml.AddChildElem("GroupID");
        (void)xml.IntoElem();
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserGroup->uiID)).c_str());
        (void)xml.AddElem("ParentGroupID");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserGroup->uiParentID)).c_str());
        (void)xml.AddElem("GroupName");
        IVS_CHAR cName[IVS_NAME_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cName, IVS_NAME_LEN, pUserGroup->cName, IVS_NAME_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserGroup->cName", "Memcpy failed");
        }
        (void)xml.SetElemValue(cName);
        (void)xml.AddElem("GroupDesc");
        IVS_CHAR cGroupDesc[IVS_DESCRIBE_LEN + 1] = {0};
        if (!CToolsHelp::Memcpy(cGroupDesc, IVS_DESCRIBE_LEN, pUserGroup->cGroupDesc, IVS_MAX_USER_GROUP_DESC_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserGroup->cGroupDesc", "Memcpy failed");
        }
        (void)xml.SetElemValue(cGroupDesc);
        (void)xml.AddElem("Level");
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pUserGroup->uiLevel)).c_str());
        xml.OutOfElem();
    }
    xml.OutOfElem();
}

IVS_INT32 COCXUserInfoMgrXMLProcess::UserGroupParseXML(LPCTSTR cstrRet, IVS_USER_GROUP &pUserGroup)
{
    IVS_DEBUG_TRACE("");

    CXml xml;
    if(!xml.Parse(cstrRet))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "User Group ParseXML", "xml parsing error");
        return IVS_XML_INVALID;
    }
    const char* szElemValue = NULL;
    if(xml.FindElemEx("Content/DomainCode"))
    {
        GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pUserGroup.cDomainCode, IVS_DOMAIN_CODE_LEN, xml);
    }
    if(!xml.FindElemEx("Content/GroupList/GroupInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get GroupInfo ParseXml", "Can't find 'Content/GroupList/GroupInfo'");
        return IVS_XML_INVALID;
    }
    xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", szElemValue, pUserGroup.uiID, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ParentGroupID", szElemValue, pUserGroup.uiParentID, xml);
    GET_ELEM_VALUE_CHAR("GroupName", szElemValue, pUserGroup.cName, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_CHAR("GroupDesc", szElemValue, pUserGroup.cGroupDesc, IVS_MAX_USER_GROUP_DESC_LEN, xml);
    xml.OutOfElem();

    return IVS_SUCCEED;
}

IVS_INT32 COCXUserInfoMgrXMLProcess::UserGroupSetXML(CXml& xml, IVS_UINT32& puiUserGroupID, const IVS_INT32& iRetValue)
{
    IVS_DEBUG_TRACE("");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRetValue)).c_str());
    xml.OutOfElem();

    if (IVS_SUCCEED == iRetValue)
    {
        (void)xml.IntoElem();
        (void)xml.AddElem("GroupList");
        (void)xml.IntoElem();
        (void)xml.AddChildElem("GroupInfo");
        (void)xml.IntoElem();
        (void)xml.AddChildElem("GroupID");
        (void)xml.IntoElem();
        (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)puiUserGroupID)).c_str());
    }

    return IVS_SUCCEED;
}

IVS_INT32 COCXUserInfoMgrXMLProcess::MoveUserGroupPraseXML(IVS_CHAR* pCameraCode, IVS_UINT32& uiTargetDevGroupID, IVS_USER_GROUP_LIST* &pUserGroupList, CXml& xml)
{
    // CHECK_POINTER(pUserGroupList,IVS_FAIL);
    IVS_DEBUG_TRACE("");

    if (!xml.FindElem("Content"))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "no 'Content' elemenet");
        return IVS_FAIL;
    }
    (void)xml.IntoElem();

    const char* szElemValue = NULL;
    if(xml.FindElemEx("Content/DomainCode"))
    {
        GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pCameraCode, IVS_DOMAIN_CODE_LEN, xml);
    }

    // 解析GroupList
    if (!xml.FindElem("GroupList"))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "no 'GroupList' elemenet");
        return IVS_FAIL;
    }
    (void)xml.IntoElem(); // 到DevInfo节点 

    typedef std::list<IVS_USER_GROUP> USER_GROUP_LIST;                
    typedef USER_GROUP_LIST::iterator USER_GROUP_LIST_ITER;
    USER_GROUP_LIST groupCameraInfoList;
    groupCameraInfoList.clear();
    while (xml.FindElemValue("GroupInfo"))
    {
        (void)xml.IntoElem();

        IVS_USER_GROUP groupInfo;
        memset(&groupInfo, 0, sizeof(IVS_USER_GROUP));

        // DomainCode
        if (!xml.FindElem("GroupID"))
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "no 'GroupID' elemenet");
            return IVS_FAIL;
        }
        const char* pszGroupID = xml.GetElemValue();
        if (NULL == pszGroupID)
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "pszGroupID = null");
            // 出错处理
            return IVS_FAIL;
        }
        groupInfo.uiID = (IVS_UINT32)atoi(pszGroupID);

        // GroupID
        if (!xml.FindElem("ParentGroupID"))
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "no 'ParentGroupID' elemenet");
            return IVS_FAIL;
        }
        const char* psParentGroupID = xml.GetElemValue();
        if (NULL == psParentGroupID)
        {
            // 出错处理
            BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "psParentGroupID = null");
            return IVS_FAIL;
        }
        groupInfo.uiParentID = (IVS_UINT32)atoi(psParentGroupID);

        // GroupName
        if (!xml.FindElem("GroupName"))
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "no 'GroupName' elemenet");
            return IVS_FAIL;
        }
        const char* pszGroupName = xml.GetElemValue();
        if (NULL == pszGroupName)
        {
            // 出错处理
            BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "pszGroupName = null");
            return IVS_FAIL;
        }
        strncpy(groupInfo.cName, pszGroupName, IVS_NAME_LEN - 1);

        // GroupDesc
        if (xml.FindElem("GroupDesc"))
        {
			const char* pDesc = xml.GetElemValue();
            if (NULL != pDesc)
            {
                (void)CToolsHelp::Memcpy(groupInfo.cGroupDesc,IVS_MAX_USER_GROUP_DESC_LEN,pDesc,strlen(pDesc));
            }
			
        }

        groupCameraInfoList.push_back(groupInfo);

        xml.OutOfElem();

        if (!xml.NextElem())
        {
            break;
        }
    }


    // 得到list了,现在开始分配内存吧 //check!!!
    IVS_UINT32 uiBufferSize = sizeof(IVS_USER_GROUP_LIST) + sizeof(IVS_USER_GROUP)*(groupCameraInfoList.size() - 1);//lint !e826

	IVS_CHAR* pBuffer = IVS_NEW(pBuffer, uiBufferSize);
    memset(pBuffer, 0, uiBufferSize);
    pUserGroupList = (IVS_USER_GROUP_LIST*)pBuffer;//lint !e826
	CHECK_POINTER(pUserGroupList, IVS_OPERATE_MEMORY_ERROR);

    pUserGroupList->uiTotal = groupCameraInfoList.size();
    USER_GROUP_LIST_ITER iter = groupCameraInfoList.begin();
    for (unsigned int i=0; i<groupCameraInfoList.size(); i++)//lint !e574 !e737 !e605
    {
        (void)CToolsHelp::Memcpy(&pUserGroupList->stUserGroup[i], sizeof(IVS_USER_GROUP), &(*iter), sizeof(IVS_USER_GROUP));
        iter++;
    }

    BP_RUN_LOG_INF("Move UserGroup PraseXML", "INFO", "pUserGroupList size is : %d ",groupCameraInfoList.size() );

    xml.OutOfElem();

    // 解析TargetGroupInfo下的TargetDomainCode、TargetGroupID
    if (!xml.FindElem("TargetGroupID"))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Move UserGroup PraseXML", "faill", "no 'TargetGroupID' elemenet");
        return IVS_FAIL;
    }
    const  char* pszTargetGroupID = xml.GetElemValue();
	CHECK_POINTER(pszTargetGroupID, IVS_XML_INVALID);
    uiTargetDevGroupID = (IVS_UINT32)atoi(pszTargetGroupID);

    xml.OutOfElem();
    return IVS_SUCCEED;
}//lint !e574 !e737 !e605

//拼接添加角色的响应xml
IVS_VOID COCXUserInfoMgrXMLProcess::AddRoleGetXML(IVS_UINT32 &uiRoleID, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    (void)xml.AddDeclaration("1.0", "UTF-8", "");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());

    (void)xml.AddElem("RoleInfo");
    (void)xml.AddChildElem("RoleID");
    xml.IntoElem();
    xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiRoleID).c_str());
    xml.OutOfElem();

    xml.OutOfElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetRoleInfoGetXml(const IVS_ROLE_INFO* pRoleInfo, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pRoleInfo);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
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

    (void)xml.AddElem("RoleType");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->uiRoleType)).c_str());

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
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bEnable).c_str());

    (void)xml.AddElem("PWDConsist");
    (void)xml.AddChildElem("PWDConsistCapital");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bCapital).c_str());
    (void)xml.AddElem("PWDConsistLowercase");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bLowercase).c_str());
    (void)xml.AddElem("PWDConsistFigure");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bFigure).c_str());
    (void)xml.AddElem("PWDConsistSpecial");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.stPswConsist.bSpecialChar).c_str());
    xml.OutOfElem();

    (void)xml.AddElem("ACCFirstModify");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bFirstModify).c_str());
    (void)xml.AddElem("ACCCheckOldPWD");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bCheckOldPwd).c_str());
    (void)xml.AddElem("PWDAccountNotFit");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bAccountNotFit).c_str());
    (void)xml.AddElem("PWDAccountBackNotFit");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bAccountBackNotFit).c_str());
    (void)xml.AddElem("DomainACCCheckPWD");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bDomainAccCheckPwd).c_str());
    (void)xml.AddElem("PWDMinLength");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->stAccountRule.uiPwdMinLen).c_str());
    (void)xml.AddElem("PWDDiferentNumber");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->stAccountRule.uiPwdDifferentNum).c_str());
    (void)xml.AddElem("ACCLockNumber");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->stAccountRule.uiAccountLockNum).c_str());
    (void)xml.AddElem("ACCLockElapse");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->stAccountRule.uiAccountLockElapse).c_str());

    (void)xml.AddElem("ACCLockInterval");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->stAccountRule.uiAccountLockInterval).c_str());
    (void)xml.AddElem("PWDExpireTime");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->stAccountRule.uiPWDExpireTime).c_str());
    (void)xml.AddElem("PWDExpireNotifyTime");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleInfo->stAccountRule.uiPWDExpireNotifyTime).c_str());
    (void)xml.AddElem("PWDExpireForceNotify");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleInfo->stAccountRule.bPWDExpireForceNotify).c_str());
    xml.OutOfElem();
    xml.OutOfElem();
    xml.OutOfElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetAutoCreatePWDGetXml(const IVS_CHAR *pPWD, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("PWDInfo");
    (void)xml.AddChildElem("LoginPWD");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pPWD);
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetPWDStrengthGetXml(IVS_UINT32 uiPWDStrength, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("PWDStrength");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(GetPWDStrength(uiPWDStrength));
    xml.OutOfElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetDefaultRoleAccountRuleGetXml(const IVS_ROLE_ACCOUNT_RULE* pRoleAccountRule, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pRoleAccountRule);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("RoleRuleInfo");
    (void)xml.AddChildElem("IsEnable");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->bEnable).c_str());

    (void)xml.AddElem("PWDConsist");
    (void)xml.AddChildElem("PWDConsistCapital");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->stPswConsist.bCapital).c_str());
    (void)xml.AddElem("PWDConsistLowercase");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->stPswConsist.bLowercase).c_str());
    (void)xml.AddElem("PWDConsistFigure");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->stPswConsist.bFigure).c_str());
    (void)xml.AddElem("PWDConsistSpecial");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->stPswConsist.bSpecialChar).c_str());
    xml.OutOfElem();

    (void)xml.AddElem("ACCFirstModify");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->bFirstModify).c_str());
    (void)xml.AddElem("ACCCheckOldPWD");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->bCheckOldPwd).c_str());
    (void)xml.AddElem("PWDAccountNotFit");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->bAccountNotFit).c_str());
    (void)xml.AddElem("PWDAccountBackNotFit");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->bAccountBackNotFit).c_str());
    (void)xml.AddElem("DomainACCCheckPWD");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->bDomainAccCheckPwd).c_str());
    (void)xml.AddElem("PWDMinLength");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleAccountRule->uiPwdMinLen).c_str());
    (void)xml.AddElem("PWDDiferentNumber");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleAccountRule->uiPwdDifferentNum).c_str());
    (void)xml.AddElem("ACCLockNumber");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleAccountRule->uiAccountLockNum).c_str());
    (void)xml.AddElem("ACCLockElapse");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleAccountRule->uiAccountLockElapse).c_str());

    (void)xml.AddElem("ACCLockInterval");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleAccountRule->uiAccountLockInterval).c_str());
    (void)xml.AddElem("PWDExpireTime");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleAccountRule->uiPWDExpireTime).c_str());
    (void)xml.AddElem("PWDExpireNotifyTime");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRoleAccountRule->uiPWDExpireNotifyTime).c_str());
    (void)xml.AddElem("PWDExpireForceNotify");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pRoleAccountRule->bPWDExpireForceNotify).c_str());
    xml.OutOfElem();
    xml.OutOfElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetOperationRightGroupGetXml(const IVS_OPERATION_RIGHT_GROUP_LIST *pOpRightGroupList, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pOpRightGroupList);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("OPGroupList");
    (void)xml.IntoElem();

    const IVS_OPERATION_RIGHT_GROUP* pOpRightGroup;

    IVS_INT32 iNum = (IVS_INT32)(pOpRightGroupList->stIndexRange.uiToIndex - pOpRightGroupList->stIndexRange.uiFromIndex) + 1; //查询的范围
    for (IVS_INT32 i = 0; i < iNum; i++)
    {
        pOpRightGroup = &pOpRightGroupList->stOperationRightGroup[i];

        if(i > 0)
        {
            (void)xml.AddElem("OPGroupInfo");
        }
        else
        {
            (void)xml.AddChildElem("OPGroupInfo");
            (void)xml.IntoElem();
        }
        
        (void)xml.AddChildElem("OPGroupName");
        (void)xml.IntoElem();
        {
            IVS_CHAR cTemp[IVS_MAX_RIGHT_GROUP_NAME_LEN + 1] = {0};
            if (!CToolsHelp::Memcpy(cTemp, IVS_MAX_RIGHT_GROUP_NAME_LEN, pOpRightGroup->cRightGroupName, IVS_MAX_RIGHT_GROUP_NAME_LEN))
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pOpRightGroup->cRightGroupName", "Memcpy failed");
                //return IVS_ALLOC_MEMORY_ERROR;
            }
            (void)xml.SetElemValue(cTemp);
        }
        (void)xml.AddElem("OPInfo");
        {
            IVS_CHAR cTemp[IVS_MAX_OPERATION_RIGHT + 1] = {0};
            if (!CToolsHelp::Memcpy(cTemp, IVS_MAX_OPERATION_RIGHT, pOpRightGroup->cRight, IVS_MAX_OPERATION_RIGHT))
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pOpRightGroup->cRight", "Memcpy failed");
                //return IVS_ALLOC_MEMORY_ERROR;
            }
            (void)xml.SetElemValue(cTemp);
        }

        xml.OutOfElem();
    }	
    if(iNum > 0)
    {
        xml.OutOfElem();
    }
    xml.IntoElem();
}

IVS_VOID COCXUserInfoMgrXMLProcess::GetPWDExpireInfoGetXml(const IVS_PWD_EXPIRE_INFO* pPWDExpireInfo, IVS_INT32 iRet, CXml &xml)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pPWDExpireInfo);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
    (void)xml.AddElem("IsNeedWarning");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pPWDExpireInfo->bIsNeedWarning).c_str());
    (void)xml.AddElem("RemainDays");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pPWDExpireInfo->uiRemainDays).c_str());
    (void)xml.AddElem("ExpiredDays");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pPWDExpireInfo->uiExpiredDays).c_str());
    xml.OutOfElem();
}

IVS_UINT32 COCXUserInfoMgrXMLProcess::GetXmlListItemNum(CXml &xml, const IVS_CHAR* pElemPath)
{
    IVS_UINT32 iNum = 0;
    if (xml.FindElemEx(pElemPath))
    {
        do{
            xml.IntoElem();
            iNum++;
            xml.OutOfElem();
        }while (xml.NextElem());
    }
    return iNum;
}

// 获取密码强度类型
const IVS_CHAR* COCXUserInfoMgrXMLProcess::GetPWDStrength(IVS_UINT32 uiStrength)
{
    const IVS_CHAR* result = "";
    switch(uiStrength)
    {
    case KEY_WEAK:
        result = "KEY_WEAK";
        break;
    case KEY_MIDDLE:
        result = "KEY_MIDDLE";
        break;
    case KEY_STRONG:
        result = "KEY_STRONG";
        break;
    case KEY_STRONGER:
        result = "KEY_STRONGER";
        break;
    default:
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "PWD Strength fail", "NA");
            result = NULL;
        }
        break;
    }
    return result;
}


IVS_INT32 COCXUserInfoMgrXMLProcess::GetStreamListByCamGetXml(const IVS_STREAM_LIST_BY_CAM* pStreamList, 
	IVS_INT32 iRet, 
	CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pStreamList, IVS_PARA_INVALID);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
	(void)xml.AddElem("StreamNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pStreamList->uiTotal).c_str());

	(void)xml.AddElem("CamInfo");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	IVS_CHAR cTmpCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Strncpy(cTmpCameraCode, IVS_DEV_CODE_LEN + 1, pStreamList->cCameraCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByCamGetXml", "Strncpy Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTmpCameraCode);
	xml.OutOfElem();

	(void)xml.AddElem("StreamList");
	const IVS_STREAM_USER_INFO* pStreamUserInfo;
	for (IVS_UINT32 i = 0; i < pStreamList->uiTotal; i++)
	{
		pStreamUserInfo = &pStreamList->stStreamUserInfo[i];

		if(i > 0)
		{
			(void)xml.AddElem("StreamInfo");
		}
		else
		{
			(void)xml.AddChildElem("StreamInfo");
			(void)xml.IntoElem();
		}

		(void)xml.AddChildElem("UserID");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pStreamUserInfo->uiUserID).c_str());

		(void)xml.AddElem("UserName");
		{
			IVS_CHAR cTmpUserName[IVS_NAME_LEN + 1] = {0};
			if (!CToolsHelp::Strncpy(cTmpUserName, IVS_NAME_LEN + 1, pStreamUserInfo->cUserName, IVS_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByCamGetXml", "Strncpy failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cTmpUserName);
		}

		(void)xml.AddElem("DomainCode");
		{
			IVS_CHAR cTmpDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
			if (!CToolsHelp::Strncpy(cTmpDomainCode, IVS_DOMAIN_CODE_LEN + 1, pStreamUserInfo->cDomainCode, IVS_DOMAIN_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByCamGetXml", "Strncpy failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cTmpDomainCode);
		}

		(void)xml.AddElem("StreamType");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pStreamUserInfo->uiStreamType).c_str());

		xml.OutOfElem();
	}	

	xml.OutOfElem();

	return IVS_SUCCEED;
}


IVS_INT32 COCXUserInfoMgrXMLProcess::GetStreamListByUserGetXml(const IVS_STREAM_LIST_BY_USER* pStreamList, 
	IVS_INT32 iRet, 
	CXml &xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pStreamList, IVS_PARA_INVALID);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue((CToolsHelp::Int2Str(iRet)).c_str());
	(void)xml.AddElem("StreamNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pStreamList->uiTotal).c_str());

	(void)xml.AddElem("UserInfo");
	(void)xml.AddChildElem("UserID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pStreamList->uiUserID).c_str());
	xml.OutOfElem();

	(void)xml.AddElem("StreamList");
	const IVS_STREAM_CAM_INFO* pStreamCamInfo;
	for (IVS_UINT32 i = 0; i < pStreamList->uiTotal; i++)
	{
		pStreamCamInfo = &pStreamList->stStreamCamInfo[i];

		if(i > 0)
		{
			(void)xml.AddElem("StreamInfo");
		}
		else
		{
			(void)xml.AddChildElem("StreamInfo");
			(void)xml.IntoElem();
		}

		(void)xml.AddChildElem("CameraCode");
		(void)xml.IntoElem();
		IVS_CHAR cTmpCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Strncpy(cTmpCameraCode, IVS_DEV_CODE_LEN + 1, pStreamCamInfo->cCameraCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByUserGetXml", "Strncpy Failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTmpCameraCode);

		(void)xml.AddElem("CameraName");
		{
			IVS_CHAR cTmpCameraName[IVS_NAME_LEN + 1] = {0};
			if (!CToolsHelp::Strncpy(cTmpCameraName, IVS_NAME_LEN + 1, pStreamCamInfo->cCameraName, IVS_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetStreamListByUserGetXml", "Strncpy failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cTmpCameraName);
		}

		(void)xml.AddElem("StreamType");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pStreamCamInfo->uiStreamType).c_str());

		xml.OutOfElem();
	}	

	xml.OutOfElem();

	return IVS_SUCCEED;
}