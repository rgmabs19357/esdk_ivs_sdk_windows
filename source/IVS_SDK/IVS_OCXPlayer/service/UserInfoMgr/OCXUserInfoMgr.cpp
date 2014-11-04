#include "stdafx.h"
#include "SDKDef.h"
#include "VideoPaneMgr.h"
#include "hwsdk.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "OCXUserInfoMgr.h"
#include "OCXUserInfoMgrXMLProcess.h"
#include "IVS_Trace.h"

COCXUserInfoMgr::COCXUserInfoMgr(void)
{
}

COCXUserInfoMgr::~COCXUserInfoMgr(void)
{
}

// 添加用户信息
CString COCXUserInfoMgr::AddUser(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");
    CString strResult = NULL;
    IVS_INT32 iRet = IVS_FAIL;
    IVS_USER_INFO stUserInfo;
    memset(&stUserInfo, 0, sizeof(IVS_USER_INFO));

    iRet = COCXUserInfoMgrXMLProcess::GetUserInfoParseXml(pReqXml, stUserInfo);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get UserInfo ParseXml", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }

    IVS_UINT32 iUserId = 0;    
    iRet = IVS_SDK_AddUser(iSessionID, &stUserInfo, &iUserId);        
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add User", "ErrorCode");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }

    CXml rspXml;
    COCXUserInfoMgrXMLProcess::AddUserGetXml(iUserId, iRet, rspXml);
    IVS_UINT32 iLen = 0;
    strResult = rspXml.GetXMLStream(iLen); 
    return strResult;
}

// 删除用户
LONG COCXUserInfoMgr::DeleteUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_DeleteUser(iSessionID, pDomainCode, ulUserID);
    return iRet;
}

// 设置用户信息
LONG COCXUserInfoMgr::ModifyUser(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    IVS_USER_INFO stUserInfo;
    memset(&stUserInfo, 0, sizeof(IVS_USER_INFO));

    iRet = COCXUserInfoMgrXMLProcess::GetUserInfoParseXml(pReqXml, stUserInfo);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get UserInfo ParseXml", "Error");
        return iRet;
    }

    iRet = IVS_SDK_ModifyUser(iSessionID, &stUserInfo);
    return iRet;
}

// 查询用户信息
CString COCXUserInfoMgr::GetUserInfo(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_SUCCEED;
    IVS_USER_INFO stUserInfo;
    memset(&stUserInfo, 0, sizeof(IVS_USER_INFO));

    iRet = IVS_SDK_GetUserInfo(iSessionID, pDomainCode, ulUserID, &stUserInfo);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetUserInfoGetXml(&stUserInfo, iRet, rspXml);     
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get User Info", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }
    return strResult;
}

// 锁定/解锁用户
LONG COCXUserInfoMgr::LockUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID, ULONG ulLock)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_LockUser(iSessionID, pDomainCode, ulUserID, (IVS_INT32)ulLock);
    return iRet;
}

// 查询用户列表
CString COCXUserInfoMgr::GetUserList(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery)
{
    IVS_DEBUG_TRACE("");

    CString strResult;
    IVS_INT32 iRet = IVS_FAIL;

    if(NULL == pUnifiedQuery)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get User List", "pUnifiedQuery is null");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult;
    }

    // 判断是否解析正确
    CXml reqXml;
    if (!reqXml.Parse(pUnifiedQuery))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Xml Parse", "error");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult;
    }

    // 调用通用的方法:解析并分配通用查询请求结构体空间
    IVS_VOID *pReqBuffer = NULL;
    IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};
    IVS_INT32 iNum = COCXXmlProcess::GetFieldNum(reqXml);
    iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXMLEx(pReqBuffer, reqXml, iNum, cDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pReqBuffer, MUILI);
        BP_RUN_LOG_ERR(iRet, "Get Unified Format Query ParseXML", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }
    IVS_QUERY_UNIFIED_FORMAT *pQuery = reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(pReqBuffer);

	if(NULL == pQuery)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get User List", "pQuery is null");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return strResult;
	}

	// 分配返回列表数据结构空间
    IVS_UINT32 iItemNum = (pQuery->stIndex.uiToIndex - pQuery->stIndex.uiFromIndex) + 1;
    IVS_UINT32 iBufferSize = sizeof(IVS_USER_INFO_LIST) + sizeof(IVS_USER_INFO) * iItemNum;

    IVS_CHAR *pBuffer = IVS_NEW(pBuffer, iBufferSize);
	if (NULL == pBuffer)
	{
		IVS_DELETE(pReqBuffer, MUILI);
		return NULL;
	}
   
    memset(pBuffer, 0, iBufferSize);
    IVS_USER_INFO_LIST *pUserInfoList = reinterpret_cast<IVS_USER_INFO_LIST*>(pBuffer);//lint !e826

    // 获取list    
    iRet = IVS_SDK_GetUserList(iSessionID, cDomainCode, pQuery, pUserInfoList, iBufferSize);
    if(iRet == IVS_SUCCEED)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetUserListGetXml(pUserInfoList, iRet, rspXml);
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get User List", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }     

    IVS_DELETE(pReqBuffer, MUILI);
    IVS_DELETE(pBuffer, MUILI);
    return strResult;
}

// 查询在线用户列表
CString COCXUserInfoMgr::GetOnlineUser(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery)
{
    IVS_DEBUG_TRACE("");
    
    IVS_INT32 iRet = IVS_SUCCEED;
    CString strResult;
    if(NULL == pUnifiedQuery)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get Online User", "pUnifiedQuery is null");
        COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
        return strResult;
    }

    // 判断是否解析正确
    CXml reqXml;
    if (!reqXml.Parse(pUnifiedQuery))
    {
        //记录日志
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "CXml Parse", "xml parsing error");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult;
    }

    // 调用通用的方法:解析并分配通用查询请求结构体空间
    IVS_VOID *pReqBuffer = NULL;
    IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};
    IVS_INT32 iFieldNum = COCXXmlProcess::GetFieldNum(reqXml);
    iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXMLEx(pReqBuffer, reqXml, iFieldNum, cDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        //记录日志
        IVS_DELETE(pReqBuffer, MUILI);
        BP_RUN_LOG_ERR(iRet, "Get Unified Format Query ParseXML", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }
    IVS_QUERY_UNIFIED_FORMAT *pQueryFormat = reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(pReqBuffer);

    // 分配返回列表数据结构空间
    IVS_UINT32 iItemNum = (pQueryFormat->stIndex.uiToIndex - pQueryFormat->stIndex.uiFromIndex) + 1;
    IVS_UINT32 iBufferSize = sizeof(IVS_ONLINE_USER_LIST) + sizeof(IVS_ONLINE_USER) * iItemNum;

	IVS_CHAR *pBuffer = IVS_NEW(pBuffer, iBufferSize);
	if (NULL == pBuffer)
	{
		IVS_DELETE(pReqBuffer, MUILI);
		return NULL;
	}
    memset(pBuffer, 0, iBufferSize);
    IVS_ONLINE_USER_LIST *pOnlineUserList = reinterpret_cast<IVS_ONLINE_USER_LIST*>(pBuffer);//lint !e826

    // 获取list    
    iRet = IVS_SDK_GetOnlineUser(iSessionID, cDomainCode, pQueryFormat, pOnlineUserList, iBufferSize);
    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetOnlineUserListGetXml(pOnlineUserList, iRet, rspXml);        
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get Online User", "ErrorCode");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }     

    IVS_DELETE(pReqBuffer, MUILI);
    IVS_DELETE(pBuffer, MUILI);
    return strResult;
}

// 修改密码
LONG COCXUserInfoMgr::ChangePWD(IVS_INT32 iSessionID, LPCTSTR pOldPWD, LPCTSTR pNewPWD)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_ChangePWD(iSessionID, pOldPWD, pNewPWD);
    return iRet;
}

// 重置密码
LONG COCXUserInfoMgr::ResetPWD(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID, LPCTSTR pNewPWD)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_ResetPWD(iSessionID, pDomainCode, ulUserID, pNewPWD);
    return iRet;
}

// 校验输入的密码是否符合账号规则
LONG COCXUserInfoMgr::CheckPWDByRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName, LPCTSTR pPWD)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_CheckPWDByRole(iSessionID, pDomainCode, ulRoleID, pLoginName, pPWD);
    return iRet;
}

// 根据角色获取符合账号规则的密码
CString COCXUserInfoMgr::GetAutoCreatePWD(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_SUCCEED;
    IVS_CHAR szPWD[IVS_PWD_LEN+1]={0};

    iRet = IVS_SDK_GetAutoCreatePWD(iSessionID, pDomainCode, ulRoleID, pLoginName, szPWD, IVS_PWD_LEN+1);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetAutoCreatePWDGetXml(szPWD, iRet, rspXml);
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get Auto Create PWD", "ErrorCode");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return strResult;
}

// 获取密码强度
CString COCXUserInfoMgr::GetPWDStrength(LPCTSTR pPWD)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_FAIL;

    IVS_UINT32 uiPWDStrength = 0;
    iRet = IVS_SDK_GetPWDStrength(pPWD, &uiPWDStrength);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetPWDStrengthGetXml(uiPWDStrength, iRet, rspXml);     
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get PWD Strength", "ErrorCode");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return strResult;
}

// 获取密码过期信息
CString COCXUserInfoMgr::GetPWDExpireInfo(IVS_INT32 iSessionID)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_FAIL;

    IVS_PWD_EXPIRE_INFO stPWDExpireInfo;
    memset(&stPWDExpireInfo, 0, sizeof(IVS_PWD_EXPIRE_INFO));
    iRet = IVS_SDK_GetPWDExpireInfo(iSessionID, &stPWDExpireInfo);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetPWDExpireInfoGetXml(&stPWDExpireInfo, iRet, rspXml);     
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get PWD Strength", "ErrorCode");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return strResult;
}

// 设置用户设备列表
LONG COCXUserInfoMgr::SetUserDeviceList(IVS_INT32 iSessionID, LPCTSTR pReqXml)
{
    IVS_DEBUG_TRACE("");

    //获取有几条记录
    IVS_UINT32 iNum = 0;
    CXml xml;
    if (!xml.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "CXml Parse", "xml parsing error");
        return IVS_XML_INVALID;
    }
    iNum = COCXUserInfoMgrXMLProcess::GetXmlListItemNum(xml, "Content/DevList/DevInfo");

    //分配内存
    IVS_UINT32 iBufferSize = sizeof(IVS_USER_DEVICE_INFO_LIST) + sizeof(IVS_USER_DEVICE_INFO) * iNum;

	IVS_CHAR *pBuffer = IVS_NEW(pBuffer, iBufferSize);
	CHECK_POINTER(pBuffer, NULL);
    memset(pBuffer, 0, iBufferSize);
    IVS_USER_DEVICE_INFO_LIST *pUserDevList = reinterpret_cast<IVS_USER_DEVICE_INFO_LIST*>(pBuffer);//lint !e826

    pUserDevList->uiTotal = iNum;

    IVS_CHAR cUserDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};

    IVS_UINT32 uiUserId = 0;
    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgrXMLProcess::SetUserDevListParseXml(pReqXml, uiUserId, pUserDevList, cUserDomainCode);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pBuffer, MUILI);
        BP_RUN_LOG_ERR(iRet, "Set User Dev List ParseXml", "Error");
        return iRet;
    }

    iRet = IVS_SDK_SetUserDeviceList(iSessionID, cUserDomainCode, uiUserId, pUserDevList, iBufferSize);

    IVS_DELETE(pBuffer, MUILI);
    return iRet;
}

// 获取用户设备列表
CString COCXUserInfoMgr::GetUserDeviceList(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery)
{
    IVS_DEBUG_TRACE("");

    CString strResult;
    IVS_INT32 iRet = IVS_FAIL;

    if(NULL == pUnifiedQuery)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get User Device List", "pUnifiedQuery is null");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult;
    }

    // 判断是否解析正确
    CXml reqXml;
    if (!reqXml.Parse(pUnifiedQuery))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "CXml Parse method", "xml parsing error");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult;
    }

    // 调用通用的方法:解析并分配通用查询请求结构体空间
    IVS_VOID *pReqBuffer = NULL;
    IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};
    IVS_INT32 iFieldNum = COCXXmlProcess::GetFieldNum(reqXml);
    iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXMLEx(pReqBuffer, reqXml, iFieldNum, cDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Unified Format Query ParseXML", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }
    IVS_QUERY_UNIFIED_FORMAT *pQueryFormat = reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(pReqBuffer);
	if (NULL == pQueryFormat)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get Unified Format Query ParseXML", "pQueryFormat is null");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return strResult;
	}

    IVS_UINT32 uiUserID = 0;
    for (int i = 0;i < iFieldNum; i++)
    {
        // 获取userID
        if(QUERY_USER_ID == pQueryFormat->stQueryField[i].eFieID)
        {
            uiUserID = (IVS_UINT32)atoi(pQueryFormat->stQueryField[i].cValue);
        }
    }
    
    // 分配返回列表数据结构空间
    IVS_UINT32 iItemNum = (pQueryFormat->stIndex.uiToIndex - pQueryFormat->stIndex.uiFromIndex) + 1;
    IVS_UINT32 iBufferSize = sizeof(IVS_USER_DEVICE_INFO_LIST) + sizeof(IVS_USER_DEVICE_INFO) * iItemNum;

	IVS_CHAR *pBuffer = IVS_NEW(pBuffer, iBufferSize);
	if (NULL == pBuffer)
	{
		IVS_DELETE(pReqBuffer, MUILI);
		return NULL;
	}
    memset(pBuffer, 0, iBufferSize);
    IVS_USER_DEVICE_INFO_LIST *pUserDevList = reinterpret_cast<IVS_USER_DEVICE_INFO_LIST*>(pBuffer);//lint !e826

    // 获取list
    iRet = IVS_SDK_GetUserDeviceList(iSessionID, cDomainCode, uiUserID, &pQueryFormat->stIndex, pUserDevList, iBufferSize);
    if(iRet == IVS_SUCCEED)
    {
        CXml rspXml;
        (void)COCXUserInfoMgrXMLProcess::GetUserDevListGetXml(pUserDevList, iRet, rspXml);
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get User Device List", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }     

    IVS_DELETE(pReqBuffer, MUILI);
    IVS_DELETE(pBuffer, MUILI);
    return strResult;
}

// 签退在线用户会话
LONG COCXUserInfoMgr::SignOutOnlineUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserOnlineID)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_SignOutOnlineUser(iSessionID, pDomainCode, (IVS_UINT32)ulUserOnlineID);
    return iRet;
}

// 根据用户ID签退在线用户会话
LONG COCXUserInfoMgr::SignOutUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_SignOutUser(iSessionID, pDomainCode, (IVS_UINT32)ulUserID);
    return iRet;
}

// 根据角色ID签退在线用户会话
LONG COCXUserInfoMgr::SignOutUserByRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_SignOutUserByRole(iSessionID, pDomainCode, (IVS_UINT32)ulRoleID);
    return iRet;
}

