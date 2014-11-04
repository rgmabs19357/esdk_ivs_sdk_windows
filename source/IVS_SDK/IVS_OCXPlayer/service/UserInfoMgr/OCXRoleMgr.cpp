#include "stdafx.h"
#include "SDKDef.h"
#include "IVS_OCXPlayerCtrl.h"
#include "VideoPaneMgr.h"
#include "hwsdk.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "OCXRoleMgr.h"
#include "OCXUserInfoMgrXMLProcess.h"
#include "IVS_Trace.h"

COCXRoleMgr::COCXRoleMgr(void)
{
}

COCXRoleMgr::~COCXRoleMgr(void)
{
}

// 增加角色
CString COCXRoleMgr::AddRole(IVS_INT32 iSessionID, LPCTSTR pRoleInfo)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_FAIL;
    IVS_ROLE_INFO stRoleInfo;
    memset(&stRoleInfo, 0, sizeof(IVS_ROLE_INFO));

    iRet = COCXUserInfoMgrXMLProcess::GetRoleInfoParseXML(pRoleInfo, stRoleInfo);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get RoleInfo ParseXML", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return strResult;
    }

    IVS_UINT32 uiRoleId = 0;    
    iRet = IVS_SDK_AddRole(iSessionID, &stRoleInfo, &uiRoleId);
    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::AddRoleGetXML(uiRoleId, iRet, rspXml);
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
    }
    return strResult;
}

// 删除角色
LONG COCXRoleMgr::DeleteRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    iRet = IVS_SDK_DeleteRole(iSessionID, pDomainCode, ulRoleID);
    return iRet;
}

// 修改角色
LONG COCXRoleMgr::ModifyRole(IVS_INT32 iSessionID, LPCTSTR pRoleInfo)
{
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;
    IVS_ROLE_INFO stRoleInfo;
    memset(&stRoleInfo, 0, sizeof(IVS_ROLE_INFO));
    iRet = COCXUserInfoMgrXMLProcess::GetRoleInfoParseXML(pRoleInfo, stRoleInfo);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get RoleInfo ParseXML", "Error");
        return iRet;
    }

    iRet = IVS_SDK_ModifyRole(iSessionID, &stRoleInfo);
    return iRet;
}

// 查询角色列表
CString COCXRoleMgr::GetRoleList(IVS_INT32 iSessionID, LPCTSTR pDomainCode)
{
    IVS_DEBUG_TRACE("");

    CString strResult;
    IVS_INT32 iRet = IVS_FAIL;

    IVS_INDEX_RANGE stIndexRange;
    memset(&stIndexRange, 0, sizeof(IVS_INDEX_RANGE));
    stIndexRange.uiFromIndex = 1;
    stIndexRange.uiToIndex = IVS_MAX_ROLE_INFO_NUM;

    // 分配返回列表数据结构空间
    IVS_UINT32 iBufferSize = sizeof(IVS_ROLE_INFO_LIST) + sizeof(IVS_ROLE_INFO) * IVS_MAX_ROLE_INFO_NUM;

    IVS_CHAR* pBuffer = IVS_NEW(pBuffer,iBufferSize);
    memset(pBuffer, 0, iBufferSize);
    IVS_ROLE_INFO_LIST *pRoleList = reinterpret_cast<IVS_ROLE_INFO_LIST*>(pBuffer);//lint !e826

    // 获取list    
    iRet = IVS_SDK_GetRoleList(iSessionID, pDomainCode, &stIndexRange, pRoleList, iBufferSize);
    if(iRet == IVS_SUCCEED)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetRoleListGetXml(pRoleList, iRet, rspXml);
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
    }     

    IVS_DELETE(pBuffer, MUILI);
    return strResult;
}

// 根据角色ID获取角色详细信息
CString COCXRoleMgr::GetRoleInfoByRoleID(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_SUCCEED;
    IVS_ROLE_INFO stRoleInfo;
    memset(&stRoleInfo, 0, sizeof(IVS_ROLE_INFO));

    iRet = IVS_SDK_GetRoleInfoByRoleID(iSessionID, pDomainCode, ulRoleID, &stRoleInfo);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetRoleInfoGetXml(&stRoleInfo, iRet, rspXml);     
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get RoleInfo By RoleID", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return strResult;
}

// 根据用户ID获取角色详细信息
CString COCXRoleMgr::GetRoleInfoByUserID(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_SUCCEED;
    IVS_ROLE_INFO stRoleInfo;
    memset(&stRoleInfo, 0, sizeof(IVS_ROLE_INFO));

    iRet = IVS_SDK_GetRoleInfoByUserID(iSessionID, pDomainCode, ulUserID, &stRoleInfo);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetRoleInfoGetXml(&stRoleInfo, iRet, rspXml);     
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get RoleInfo By UserID", "ErrorCode");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return strResult;
}

// 获取默认账号规则
CString COCXRoleMgr::GetDefaultRoleAccountRule(IVS_INT32 iSessionID, LPCTSTR pDomainCode)
{
    IVS_DEBUG_TRACE("");

    CString strResult = NULL;
    IVS_INT32 iRet = IVS_SUCCEED;
    IVS_ROLE_ACCOUNT_RULE stRoleAccountRule;
    memset(&stRoleAccountRule, 0, sizeof(IVS_ROLE_ACCOUNT_RULE));

    iRet = IVS_SDK_GetDefaultRoleAccountRule(iSessionID, pDomainCode, &stRoleAccountRule);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetDefaultRoleAccountRuleGetXml(&stRoleAccountRule, iRet, rspXml);     
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get Default Role Account Rule", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return strResult;
}

// 获取用户权限信息
CString COCXRoleMgr::GetOperationRightGroup(IVS_INT32 iSessionID, LPCTSTR pDomainCode)
{
    IVS_DEBUG_TRACE("");

    CString strResult;
    IVS_INT32 iRet = IVS_SUCCEED;

    // 分配返回列表数据结构空间
    IVS_UINT32 iBufferSize = sizeof(IVS_OPERATION_RIGHT_GROUP_LIST) + sizeof(IVS_OPERATION_RIGHT_GROUP) * (IVS_MAX_RIGHT_GROUP_NUM-1);

    IVS_CHAR* pBuffer = IVS_NEW(pBuffer, iBufferSize);
    memset(pBuffer, 0, iBufferSize);
    IVS_OPERATION_RIGHT_GROUP_LIST* pRightGroupList = reinterpret_cast<IVS_OPERATION_RIGHT_GROUP_LIST*>(pBuffer);//lint !e826

    iRet = IVS_SDK_GetOperationRightGroup(iSessionID, pDomainCode, pRightGroupList, iBufferSize);

    if(IVS_SUCCEED == iRet)
    {
        CXml rspXml;
        COCXUserInfoMgrXMLProcess::GetOperationRightGroupGetXml(pRightGroupList, iRet, rspXml);     
        IVS_UINT32 iLen = 0;
        strResult = rspXml.GetXMLStream(iLen);
    }
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get Operation Right Group", "Error");
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    IVS_DELETE(pBuffer,MUILI);
    return strResult;
}

