#include "UserGroupMgr.h"
#include "UserMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "ToolsHelp.h"
#include "UserInfoMgrXMLProcess.h"
#include "IVS_Trace.h"

CUserGroupMgr::CUserGroupMgr()
    :m_pUserMgr(NULL)
{
}

CUserGroupMgr::~CUserGroupMgr()
{
    m_pUserMgr = NULL;
}

void CUserGroupMgr::SetUserMgr( CUserMgr *pUserMgr)
{
    m_pUserMgr = pUserMgr;
}

// 增加用户组
IVS_INT32 CUserGroupMgr::AddUserGroup(const IVS_USER_GROUP* pUserGroup, IVS_UINT32* puiUserGroupID)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserGroup, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    // 防止域编码达到临界值无结束符“\0” modify by xiongfeng
    char cDomainCodeTmp[IVS_DOMAIN_CODE_LEN + 1] = {0};
    (void)CToolsHelp::Memcpy(cDomainCodeTmp, IVS_DOMAIN_CODE_LEN, pUserGroup->cDomainCode, IVS_DOMAIN_CODE_LEN);
    strDomainCode = cDomainCodeTmp;
    if (strDomainCode.empty())
    {
        m_pUserMgr->GetDomainCode(strDomainCode);
    }
    // modify end

//     strDomainCode.assign(pUserGroup->cDomainCode, IVS_DOMAIN_CODE_LEN);
//     if(pUserGroup->cDomainCode[0] == '\0')
//     {
//         m_pUserMgr->GetDomainCode(strDomainCode);
//     }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::GetUserGroupXML(strDomainCode.c_str(), pUserGroup, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_ADD_USERGROUP_REQ);
    sendNssMsgInfo.SetReqData(pReq);
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add User Group","SendCmd to SMU Return Failed");
        return  iRet;
    }

    CHECK_POINTER(strpRsp.c_str(), IVS_XML_INVALID); 

    CXml xmlRsp;
    if(!xmlRsp.Parse(strpRsp.c_str()))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add User Group", "xml parsing error");
        return IVS_XML_INVALID;
    }
    iRet = CUserInfoMgrXMLProcess::AddUserGroupParseXML(xmlRsp, puiUserGroupID);

    return iRet;
}

// 删除用户组
IVS_INT32 CUserGroupMgr::DeleteUserGroup(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserGroupID)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_pUserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    (void)CUserInfoMgrXMLProcess::GetDeleteUserGroupXML(strDomainCode.c_str(), uiUserGroupID, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_DEL_USERGROUP_REQ);
    sendNssMsgInfo.SetReqData(pReq);
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 修改用户组
IVS_INT32 CUserGroupMgr::ModifyUserGroup(const IVS_USER_GROUP* pUserGroup)
{
    CHECK_POINTER(pUserGroup, IVS_PARA_INVALID);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    // 防止域编码达到临界值无结束符“\0” modify by xiongfeng
    char cDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    (void)CToolsHelp::Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, pUserGroup->cDomainCode, IVS_DOMAIN_CODE_LEN);
    strDomainCode = cDomainCode;
    if (strDomainCode.empty())
    {
        m_pUserMgr->GetDomainCode(strDomainCode);
    }
    // modify end

//     strDomainCode.assign(pUserGroup->cDomainCode, IVS_DOMAIN_CODE_LEN);
//     if(pUserGroup->cDomainCode[0] == '\0')
//     {
//         m_pUserMgr->GetDomainCode(strDomainCode);
//     }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::GetUserGroupXML(strDomainCode.c_str(), pUserGroup, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_MOD_USERGROUP_REQ);
    sendNssMsgInfo.SetReqData(pReq);
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 移动用户组
IVS_INT32 CUserGroupMgr::MoveUserGroup(const IVS_CHAR* pDomainCode, IVS_UINT32 uiTargetGroupID, const IVS_USER_GROUP_LIST* pUserGroupList)
{
    CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserGroupList,IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_pUserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    // 拼装请求xml报文 TODO
    (void)CUserInfoMgrXMLProcess::MoveUserGroupGetXML(strDomainCode.c_str(), uiTargetGroupID, pUserGroupList, xmlReq);

    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_MOVE_USERGROUP_REQ);
    sendNssMsgInfo.SetReqData(pReq);
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 查询用户组列表
IVS_INT32 CUserGroupMgr::GetUserGroupList(const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_GROUP_LIST* pUserGroupList, IVS_UINT32 /*uiBufferSize*/)//lint !e830
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
    CHECK_POINTER(pUserGroupList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_pUserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    //组装xml
    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::CommQueryXml(strDomainCode.c_str(), pIndexRange, xmlReq);
    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_GET_USERGROUP_LIST_REQ);
    sendNssMsgInfo.SetReqData(pReq);
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get User Group List","SendCmd to SMU Return Failed");
        return  iRet;
    }

    CHECK_POINTER(strRsp.c_str(), IVS_XML_INVALID); 

    CXml xmlRsp;
    if(!xmlRsp.Parse(strRsp.c_str()))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get UserGroup List", "xml parsing error");
        return IVS_XML_INVALID;
    }
    iRet = CUserInfoMgrXMLProcess::GetUserGroupListParseXML(xmlRsp, pIndexRange, pUserGroupList); 

    return iRet;
}

