#include "UserInfoMgr.h"
#include "UserMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "ToolsHelp.h"
#include "XmlProcess.h"
#include "UserInfoMgrXMLProcess.h"
#include "SDKSecurityClient.h"
#include "IVS_Trace.h"

#ifndef MAX_USER_DEV_NUM
#define MAX_USER_DEV_NUM 300
#endif

CUserInfoMgr::CUserInfoMgr(void)
    :m_UserMgr(NULL)
{
    memset(&m_stRoleInfo, 0, sizeof(IVS_ROLE_INFO));
}

CUserInfoMgr::~CUserInfoMgr(void)
{
    m_UserMgr = NULL;
}

void CUserInfoMgr::SetUserMgr(CUserMgr *pUserMgr)
{
    m_UserMgr = pUserMgr;
}

// 添加用户
IVS_INT32 CUserInfoMgr::AddUser(const IVS_USER_INFO* pUserInfo, IVS_UINT32* puiUserID)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserInfo, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;

    std::string strDomainCode;
    // 防止域编码达到临界值无结束符“\0” modify by xiongfeng
    char cDomainCodeTmp[IVS_DOMAIN_CODE_LEN + 1] = {0};
    (void)CToolsHelp::Memcpy(cDomainCodeTmp, IVS_DOMAIN_CODE_LEN, pUserInfo->cDomainCode, IVS_DOMAIN_CODE_LEN);
    strDomainCode = cDomainCodeTmp;
    if (strDomainCode.empty())
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    // modify end

    /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_UserMgr, strDomainCode, strLinkID);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Add User", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }

    // 加密数据后拼装
    CXml xmlReq;
	CUserInfoMgrXMLProcess::GetAddModUserInfoXML(strDomainCode.c_str(), pUserInfo, oSecurityClient, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

    //创建要发送的CMD，拼装了NSS消息头
    CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_ADD_USER_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
    HW_DELETE(pCmdRsp);
    if(iRet == IVS_SUCCEED)
    {
        CHECK_POINTER(pRsp, IVS_XML_INVALID);
        CXml xmlRsp;
        if(!xmlRsp.Parse(pRsp))
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add User", "xml parsing error");
            IVS_DELETE(pRsp, MUILI);
            return IVS_XML_INVALID;
        }
        iRet = CUserInfoMgrXMLProcess::AddUserParseXML(xmlRsp, puiUserID);
    }
    IVS_DELETE(pRsp, MUILI);
    return iRet;
}

// 修改用户
IVS_INT32 CUserInfoMgr::ModifyUser(const IVS_USER_INFO* pUserInfo)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserInfo, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    // 防止域编码达到临界值无结束符“\0” modify by xiongfeng
    char cDomainCodeTmp[IVS_DOMAIN_CODE_LEN + 1] = {0};
    (void)CToolsHelp::Memcpy(cDomainCodeTmp, IVS_DOMAIN_CODE_LEN, pUserInfo->cDomainCode, IVS_DOMAIN_CODE_LEN);
    strDomainCode = cDomainCodeTmp;
    if (strDomainCode.empty())
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    // modify end

    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
	IVS_INT32 iRet = IVS_FAIL;

    iRet = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_UserMgr, strDomainCode, strLinkID);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Modify User", "oSecurityClient Init failed");
        return iRet;
    }

    CXml xmlReq;
  
    CUserInfoMgrXMLProcess::GetAddModUserInfoXML(strDomainCode.c_str(), pUserInfo, oSecurityClient, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();
    // 生成修改用户请求消息
    CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_MOD_USER_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
    HW_DELETE(pCmdRsp);

    return iRet;
}

// 删除用户
IVS_INT32 CUserInfoMgr::DeleteUser(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    // CUserInfoMgrXMLProcess::GetAddUserInfoXML(strDomainCode.c_str(), &pUserInfo, NULL, xmlReq);
    CUserInfoMgrXMLProcess::GetDeleteUserXML(strDomainCode.c_str(), uiUserID, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_DEL_USER_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 查询用户信息
IVS_INT32 CUserInfoMgr::GetUserInfo(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_USER_INFO* pUserInfo)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

	std::string strDomainCode;
	if(NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		m_UserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
	IVS_INT32 iRet = IVS_FAIL;

    iRet = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_UserMgr, strDomainCode, strLinkID);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get User Info", "oSecurityClient Init failed");
        return iRet;
    }
    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

    CXml xmlReq;
    CUserInfoMgrXMLProcess::GetUserInfoGetXML(strDomainCode.c_str(), uiUserID, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);


    CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_GET_USER_BY_ID_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

    // 发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);
	//add by zwx211831 如果失败，返回服务器响应的错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ParseCmd2XML fail", "NA");
		return iRet;
	}
    CHECK_POINTER(pRsp, IVS_OPERATE_MEMORY_ERROR);

    CXml xmlRsp;
    if(!xmlRsp.Parse(pRsp))
    {
		IVS_DELETE(pRsp, MUILI);
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User Info", "xml parsing error");
        return IVS_XML_INVALID;
    }
	IVS_DELETE(pRsp, MUILI);
    iRet = CUserInfoMgrXMLProcess::GetUserInfoParseXML(xmlRsp, oSecurityClient, pUserInfo); 

    return iRet;
}

// 锁定/解锁用户
IVS_INT32 CUserInfoMgr::LockUser(const IVS_CHAR* pDomainCode, IVS_INT32 iSessionID, IVS_UINT32 uiUserID, IVS_BOOL bLock)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::LockUserGetXML(strDomainCode.c_str(), uiUserID, bLock, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_LOCK_USER_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 查询用户列表
IVS_INT32 CUserInfoMgr::GetUserList(const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_USER_INFO_LIST* pUserList, IVS_UINT32 /*uiBufferSize*/)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUnifiedQuery, IVS_PARA_INVALID);
    CHECK_POINTER(pUserList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    // 密钥交换
    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
	IVS_INT32 iRet = IVS_FAIL;
    iRet = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_UserMgr, strDomainCode, strLinkID);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get User List", "oSecurityClient Init failed");
        return iRet;
    }
    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

    CXml xmlReq;
    iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery, xmlReq, strDomainCode.c_str());
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get User List", "Get Unified Format Query XML is fail");
        return iRet;
    }
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_GET_USER_LIST_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

    // 发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);
	//add by zwx211831 如果失败，返回服务器响应的错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ParseCmd2XML fail", "NA");
		return iRet;
	}
    CHECK_POINTER(pRsp, IVS_XML_INVALID);

    CXml xmlRsp;
    if(!xmlRsp.Parse(pRsp))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User List", "xml parsing error");
		IVS_DELETE(pRsp, MUILI);
        return IVS_XML_INVALID;
    }
	IVS_DELETE(pRsp, MUILI);
    iRet = CUserInfoMgrXMLProcess::GetUserListParseXML(xmlRsp, oSecurityClient, pUserList);
    return iRet;
}

// 查询在线用户列表
IVS_INT32 CUserInfoMgr::GetOnlineUser(const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ONLINE_USER_LIST* pOnlineUserList, IVS_UINT32 uiBufferSize)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUnifiedQuery, IVS_PARA_INVALID);
    CHECK_POINTER(pOnlineUserList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery, xmlReq, strDomainCode.c_str());
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Online User", "Get Unified Format Query XML is fail");
        return iRet;
    }
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_GET_ONLINE_USER_LIST_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "SendCmd to SMU Return Failed", "NA");
        return  iRet;
    }
    CHECK_POINTER(strRsp.c_str(), IVS_XML_INVALID);

    CXml xmlRsp;
    if(!xmlRsp.Parse(strRsp.c_str()))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Online User", "xml parsing error");
        return IVS_XML_INVALID;
    }
    iRet = CUserInfoMgrXMLProcess::GetOnlineUserListParseXML(xmlRsp, pOnlineUserList);

    return iRet;
}

// 设置用户设备列表
IVS_INT32 CUserInfoMgr::SetUserDeviceList(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 /*uiBufferSize*/)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserDevList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;

    /*
    CU
    1、CU显示管理员的设备树
    -CU只显示管理员所在域本域及下级域的设备，剔除平级域的设备（包含上级域）
    -如果管理员没有外域管理权限，需要剔除外部域的设备
    2、CU从用户所在域拉用户的设备权限
    -CU将用户的设备挂到剔除后的管理员设备树上
    3、CU为用户配置权限，并将配置后的设备权限发给SDK，（列表1）


    SDK
    1、SDK从用户所在域拉用户的设备权限,（列表2）

    2、SDK从管理员所在域拉管理员的设备权限，（列表3）
    -SDK需要判断管理员是否有外域管理权限，如果没有，则需要剔除权限中所有外部域的设备权限 
    -如果有外域管理权限，则管理员默认拥有所有下级域的设备权限，SDK需要删除【列表3】中所有的下级域设备/组，再将所有下级域的根组权限添加到【列表3】中
    -SDK将管理员本域及下级域关联整组的权限，解开为关联到所有下级组的整组权限 

    //过滤用户设备权限，获取管理员没有权限的设备列表：上级域和平级域设备；本域及下级域，但是管理员没有权限的设备
    3、SDK将列表2中的管理员已有权限过滤掉
    -SDK需要先将列表2中的管理员所在域的平级域设备全部保存*****（列表a）平级域及上级域设备    <+>
    -刚才保存后，剩下来得列表，作为列表b（本域及下级域设备，以及外部域设备）
    -SDK再将列表b，与列表3做过滤***（得到列表c）本域及下级域但是管理员没权限的设备 
    -备注：把【列表b】中管理员拥有的设备/组删掉，即把【列表b】【列表3】中相同的设备/组删除。完成后【列表c】剩下的为管理员没有的设备/组

    //校验分配的权限是否合法
    4、分配的设备权限校验
    -SDK将CU传来的设备权限（列表1）与管理员的设备权限（列表3）做校验，查看列表1中的权限是否都包含在列表3中

    //获取需要存到最小公共祖先节点的列表
    5、SDK将列表1和列表c做合并，成为管理员本域及下级域的设备权限列表（列表0，不包括平级域的）

    //获取所有的设备列表
    6、SDK将用户的设备列表（列表0+列表a）保存到用户所在域

    7、SDK遍历【管理员本域及下级域 域路由】，使用用户所在域和域路由中的域编码查找最小公共祖先节点
    -将列表0整体存到最小公共祖先节点（会有不必要的数据）
    -备注：用户所在域和设备所在域的最小公共祖先节点，肯定是用户所在域的上级域
    ----备注-----------------------------------------------------------------------------------
    第3步【列表b】【列表3】去重的判断依据是：

    1.所有字段完全相同，判为重复，去掉【列表b】中项
    DevGroupCode
    DomainCode
    DevCode
    DevDomainCode
    IsTotalGroup

    2.管理员有组权限，用户有组下的单个设备权限，判为重复，去掉【列表b】中单个设备权限
    第5步【列表1】+【列表c】
    1.用户已经有组权限，又有组下单个设备权限，去掉单个设备权限，保留组权限
    */

    //获取管理员(当前用户)拥有的设备权限
    IVS_USER_DEVICE_INFO_LIST *pAdminUserDevList = NULL;
    iRet = CUserInfoMgr::GetCurrentUserDev(pAdminUserDevList);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pAdminUserDevList, MUILI);
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Current User Device List", "failure");
        return iRet;
    }

    //获取管理员(当前用户)所在域和有权限的下级域列表
    DOMAIN_CODE_VEC domainCodeList;
    m_UserMgr->GetDomainRouteMgr().GetAllDomainCode(domainCodeList);

    //当前域编码
    std::string strAdminDomainCode;
    m_UserMgr->GetDomainCode(strAdminDomainCode);

    //获取管理员(当前用户)是否有外域管理权限
    if(m_stRoleInfo.uiRoleID == 0)
    {
        //如果没有缓存用户角色信息，查询一次
        m_UserMgr->GetRoleMgr().GetRoleInfoByUserID(strAdminDomainCode.c_str(), m_UserMgr->GetUserID(), &m_stRoleInfo);
    }

    IVS_BOOL hasPower = m_stRoleInfo.cRight[OR_MANAGER_OUT_DOMAIN - 1];

    std::vector<IVS_USER_DEVICE_INFO> User_Dev_List_3;
    if(hasPower)
    {
        //把所有下级域设备的权限先删掉
        IVS_UINT32 uiNum = (pAdminUserDevList->stIndexRange.uiToIndex - pAdminUserDevList->stIndexRange.uiFromIndex) + 1;
        for (IVS_UINT32 i = 0; i < uiNum; i++)
        {
            IVS_USER_DEVICE_INFO *pUserDevInfo = &pAdminUserDevList->stUserDeviceInfo[i];
            char cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDevInfo->cDevDomainCode, IVS_DOMAIN_CODE_LEN);
            if(0 == strcmp(strAdminDomainCode.c_str(), cDevDomainCode))
            {
                IVS_USER_DEVICE_INFO pDevInfo;
                memset(&pDevInfo, 0, sizeof(IVS_USER_DEVICE_INFO));
                pDevInfo.bIsTotalGroup = pUserDevInfo->bIsTotalGroup;
                pDevInfo.bIsExDomain = pUserDevInfo->bIsExDomain;
                if (!CToolsHelp::Memcpy(pDevInfo.cDevCode, IVS_DEV_CODE_LEN, pUserDevInfo->cDevCode, IVS_DEV_CODE_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevCode", "CToolsHelp::Memcpy failed");
                }
                if (!CToolsHelp::Memcpy(pDevInfo.cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDevInfo->cDevDomainCode, IVS_DOMAIN_CODE_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevDomainCode", "CToolsHelp::Memcpy failed");
                }
                if (!CToolsHelp::Memcpy(pDevInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, pUserDevInfo->cDevGroupCode, IVS_DEVICE_GROUP_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevGroupCode", "CToolsHelp::Memcpy failed");
                }
                User_Dev_List_3.push_back(pDevInfo);
            }
        }

        //然后把每个域根组(根组编码为0)加上
        IVS_UINT32 iSize = domainCodeList.size();
        for(IVS_UINT32 j = 0; j < iSize; j++)
        {
            std::string strDomainCode = domainCodeList.at(j);
            if(0 != strcmp(strAdminDomainCode.c_str(), strDomainCode.c_str()))
            {
                IVS_USER_DEVICE_INFO pDevInfo;
                memset(&pDevInfo, 0, sizeof(IVS_USER_DEVICE_INFO));
                pDevInfo.bIsTotalGroup = 1;
                pDevInfo.bIsExDomain = 1;
                if (!CToolsHelp::Memcpy(pDevInfo.cDevDomainCode, IVS_DOMAIN_CODE_LEN, strDomainCode.c_str(), strDomainCode.length()))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevDomainCode", "CToolsHelp::Memcpy failed");
                }
				std::string strDevGroupCode = "0#" + strDomainCode;
                if (!CToolsHelp::Memcpy(pDevInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, strDevGroupCode.c_str(), strDevGroupCode.length()))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevDomainCode", "CToolsHelp::Memcpy failed");
                }
                User_Dev_List_3.push_back(pDevInfo);
            }
        }
    }
    else
    {
        //去掉所有外域设备/组
        IVS_UINT32 uiNum = (pAdminUserDevList->stIndexRange.uiToIndex - pAdminUserDevList->stIndexRange.uiFromIndex) + 1;
        for (IVS_UINT32 i = 0; i < uiNum; i++)
        {
            IVS_USER_DEVICE_INFO *pUserDevInfo = &pAdminUserDevList->stUserDeviceInfo[i];
            if(!pUserDevInfo->bIsExDomain)
            {
                IVS_USER_DEVICE_INFO pDevInfo;
                memset(&pDevInfo, 0, sizeof(IVS_USER_DEVICE_INFO));
                pDevInfo.bIsTotalGroup = pUserDevInfo->bIsTotalGroup;
                pDevInfo.bIsExDomain = pUserDevInfo->bIsExDomain;
                if (!CToolsHelp::Memcpy(pDevInfo.cDevCode, IVS_DEV_CODE_LEN, pUserDevInfo->cDevCode, IVS_DEV_CODE_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevCode", "CToolsHelp::Memcpy failed");
                }
                if (!CToolsHelp::Memcpy(pDevInfo.cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDevInfo->cDevDomainCode, IVS_DOMAIN_CODE_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevDomainCode", "CToolsHelp::Memcpy failed");
                }
                if (!CToolsHelp::Memcpy(pDevInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, pUserDevInfo->cDevGroupCode, IVS_DEVICE_GROUP_LEN))
                {
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevInfo->cDevDomainCode", "CToolsHelp::Memcpy failed");
                }
                User_Dev_List_3.push_back(pDevInfo);
            }
        }
    }

    // 有父组权限，默认就有下面所有子组的权限，这里要把父组下的子组全部查出来，放入列表3中
    std::vector<IVS_USER_DEVICE_INFO> User_Dev_List_3A;
    IVS_UINT32 iSize1 = User_Dev_List_3.size();
    IVS_USER_DEVICE_INFO *pUserDev3 = NULL;
    for(IVS_UINT32 i = 0; i < iSize1; i++)
    {
        pUserDev3 = &User_Dev_List_3.at(i);
        if(pUserDev3->bIsTotalGroup)
        {
            char cDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, pUserDev3->cDevDomainCode, IVS_DOMAIN_CODE_LEN);
            iRet = CUserInfoMgr::GetDeviceGroup(cDomainCode, pUserDev3->cDevGroupCode, pUserDev3->bIsExDomain, User_Dev_List_3A);
            if(IVS_SUCCEED != iRet)
            {
				//add by zwx211831,Date:20140715,删除开辟的内存空间，防止内存泄露.
				IVS_DELETE(pAdminUserDevList, MUILI);
				//end
                BP_RUN_LOG_ERR(IVS_FAIL, "Get Device Group", "failure");
                return iRet;
            }
        }
    }

    iSize1 = User_Dev_List_3A.size();
    IVS_USER_DEVICE_INFO *pUserDevInfo3A = NULL;
    for(IVS_UINT32 i = 0; i < iSize1; i++)
    {
        pUserDevInfo3A = &User_Dev_List_3A.at(i);

        User_Dev_List_3.push_back(*pUserDevInfo3A);
    }

    //分配的设备权限校验
    //-SDK将CU传来的设备权限（列表1）与管理员的设备权限（列表3）做校验，查看列表1中的权限是否都包含在列表3中
    IVS_UINT32 uiNum1 = pUserDevList->uiTotal;
    for (IVS_UINT32 i = 0; i < uiNum1; i++)
    {
        const IVS_USER_DEVICE_INFO *pUserDevInfo1 = &pUserDevList->stUserDeviceInfo[i];
        IVS_BOOL isExist = FALSE;
        IVS_UINT32 iSize = User_Dev_List_3.size();
        for(IVS_UINT32 j = 0; j < iSize; j++)
        {
            IVS_USER_DEVICE_INFO *pUserDevInfo3 = &User_Dev_List_3.at(j);
            //1.所有字段完全相同，判为重复
            char cDevDomainCode1[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCode1, IVS_DOMAIN_CODE_LEN, pUserDevInfo1->cDevDomainCode, IVS_DOMAIN_CODE_LEN);
            char cDevDomainCode3[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCode3, IVS_DOMAIN_CODE_LEN, pUserDevInfo3->cDevDomainCode, IVS_DOMAIN_CODE_LEN);

            char cDevCode1[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCode1, IVS_DEV_CODE_LEN, pUserDevInfo1->cDevCode, IVS_DEV_CODE_LEN);
            char cDevCode3[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCode3, IVS_DEV_CODE_LEN, pUserDevInfo3->cDevCode, IVS_DEV_CODE_LEN);

            IVS_BOOL isAllEqual = (0 == strcmp(pUserDevInfo1->cDevGroupCode, pUserDevInfo3->cDevGroupCode) &&
                0 == strcmp(cDevDomainCode1, cDevDomainCode3) &&
                0 == strcmp(cDevCode1, cDevCode3) &&
                pUserDevInfo1->bIsExDomain == pUserDevInfo3->bIsExDomain &&
                pUserDevInfo1->bIsTotalGroup == pUserDevInfo3->bIsTotalGroup);

            //2.管理员有组权限，用户有组下的单个设备权限，判为重复，去掉【pUserDeviceInfo】中单个设备权限
            IVS_BOOL isInclude = (pUserDevInfo3->bIsTotalGroup && 
                0 == strcmp(pUserDevInfo1->cDevGroupCode, pUserDevInfo3->cDevGroupCode) &&
                0 == strcmp(cDevDomainCode1, cDevDomainCode3));

            isExist = (isAllEqual || isInclude);

            if(isExist)
            {
                break;
            }
        }
		//问题单号：DTS2013070207722，如果设备为PCG对接的外部设备，则均赋予其权限（和内部域的外域权限分配原则保持一致）
		if(hasPower && 1 == pUserDevInfo1->bIsExDomain)
		{
			isExist = TRUE;
		}
        //如果有没有权限的设备，返回错误
        if(!isExist)
        {
			//add by zwx211831,Date:20140715,删除开辟的内存空间，防止内存泄露.
			IVS_DELETE(pAdminUserDevList, MUILI);
			//end
            BP_RUN_LOG_INF("Administrator not some equipment permissions", "DevCode:%s,cDevGroupCode:%s,ret:%d", 
				           pUserDevInfo1->cDevCode, pUserDevInfo1->cDevGroupCode, IVS_NO_ENOUGH_RIGHT);
            return IVS_NO_ENOUGH_RIGHT;
        }
    }

    //获取用户已经拥有的设备    
    IVS_USER_DEVICE_INFO_LIST *pUserDevList2 = NULL;
    iRet = CUserInfoMgr::GetUserDevList(pDomainCode, uiUserID, pUserDevList2);
    if(IVS_SUCCEED != iRet)
    {
		//add by zwx211831,Date:20140715,删除开辟的内存空间，防止内存泄露.
		IVS_DELETE(pAdminUserDevList, MUILI);
		//end
        IVS_DELETE(pUserDevList2, MUILI);
        BP_RUN_LOG_ERR(IVS_FAIL, "Get User Device List", "failure");
        return iRet;
    }

    //-SDK需要先将列表2中的管理员所在域的平级域设备全部保存*****（列表a）管理员平级域及上级域设备    <+>
    //-刚才保存后，剩下来得列表，作为列表b（管理员本域及下级域设备）
    std::list<IVS_USER_DEVICE_INFO *> User_Dev_List_A;
    std::list<IVS_USER_DEVICE_INFO *> User_Dev_List_B;

    IVS_UINT32 uiOldDevNum = (pUserDevList2->stIndexRange.uiToIndex - pUserDevList2->stIndexRange.uiFromIndex) + 1;
    for (IVS_UINT32 i = 0; i < uiOldDevNum; i++)
    {
        IVS_USER_DEVICE_INFO *pUserDevInfo = &pUserDevList2->stUserDeviceInfo[i];
        IVS_BOOL isExist = FALSE;
        IVS_UINT32 iSize = domainCodeList.size();
        for(IVS_UINT32 j = 0; j < iSize; j++)
        {
            std::string strDomainCode = domainCodeList.at(j);
            char cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDevInfo->cDevDomainCode, IVS_DOMAIN_CODE_LEN);
            if(0 == strcmp(cDevDomainCode, strDomainCode.c_str()))
            {
                isExist = TRUE;
                break;
            }
        }
        if(isExist)
        {
            User_Dev_List_B.push_back(pUserDevInfo);
        }
        else
        {
            User_Dev_List_A.push_back(pUserDevInfo);
        }
    }

    //把管理员已经有的设备权限去掉-SDK再将列表b，与列表3做过滤***（得到列表c）管理员本域及下级域但是没权限的设备 
    std::list<IVS_USER_DEVICE_INFO *> User_Dev_List_C;
    std::list<IVS_USER_DEVICE_INFO *>::iterator iterB = User_Dev_List_B.begin();
    for (; iterB != User_Dev_List_B.end(); iterB++)
    {
        IVS_USER_DEVICE_INFO *pUserDevInfo = (IVS_USER_DEVICE_INFO*)(*iterB);
        IVS_BOOL isExist = FALSE;
        IVS_UINT32 iSize = User_Dev_List_3.size();
        for(IVS_UINT32 j = 0; j < iSize; j++)
        {
            IVS_USER_DEVICE_INFO *pUserDevInfo3 = &User_Dev_List_3.at(j);
            //1.所有字段完全相同，判为重复
            char cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDevInfo->cDevDomainCode, IVS_DOMAIN_CODE_LEN);
            char cDevDomainCode3[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCode3, IVS_DOMAIN_CODE_LEN, pUserDevInfo3->cDevDomainCode, IVS_DOMAIN_CODE_LEN);

            char cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCode, IVS_DEV_CODE_LEN, pUserDevInfo->cDevCode, IVS_DEV_CODE_LEN);
            char cDevCode3[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCode3, IVS_DEV_CODE_LEN, pUserDevInfo3->cDevCode, IVS_DEV_CODE_LEN);

            IVS_BOOL isAllEqual = (0 == strcmp(pUserDevInfo->cDevGroupCode, pUserDevInfo3->cDevGroupCode) &&
                0 == strcmp(cDevDomainCode, cDevDomainCode3) &&
                0 == strcmp(cDevCode, cDevCode3) &&
                pUserDevInfo->bIsExDomain == pUserDevInfo3->bIsExDomain &&
                pUserDevInfo->bIsTotalGroup == pUserDevInfo3->bIsTotalGroup);

            //2.管理员有组权限，用户有组下的单个设备权限，判为重复，去掉【pUserDeviceInfo】中单个设备权限
            IVS_BOOL isInclude = (pUserDevInfo3->bIsTotalGroup && 
                0 == strcmp(pUserDevInfo->cDevGroupCode, pUserDevInfo3->cDevGroupCode) &&
                0 == strcmp(cDevDomainCode, cDevDomainCode3));

            isExist = (isAllEqual || isInclude);

            if(isExist)
            {
                break;
            }
        }
        if(!isExist && 1 != pUserDevInfo->bIsExDomain)
        {
            User_Dev_List_C.push_back(pUserDevInfo);
        }
    }

    //把设备编码（设备编码#域编码）中的域编码去掉
    std::list<IVS_USER_DEVICE_INFO *>::iterator iterC = User_Dev_List_C.begin();
    //for (; iterC != User_Dev_List_C.end(); iterC++)
    //{
        //IVS_USER_DEVICE_INFO* pDev = (IVS_USER_DEVICE_INFO*)(*iterC);

        //IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
        //IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
        ////主设备编码
        //(void)CXmlProcess::ParseDevCode(pDev->cDevCode, chDevCode, chDomaCode);
        //if (!CToolsHelp::Memcpy(pDev->cDevCode, IVS_DEV_CODE_LEN, chDevCode, IVS_DEV_CODE_LEN))
        //{
        //    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "chDevCode memcpy fail", "NA");
        //    return IVS_ALLOC_MEMORY_ERROR;
        //}
    //}

    //SDK将列表1和列表c做合并--列表0
    std::list<IVS_USER_DEVICE_INFO *> User_Dev_List_0 = User_Dev_List_C;
    IVS_UINT32 uiDevNum = pUserDevList->uiTotal;
    IVS_USER_DEVICE_INFO* pUserDevInfo;
    for (IVS_UINT32 i = 0; i < uiDevNum; i++)
    {
        pUserDevInfo = const_cast<IVS_USER_DEVICE_INFO*>(&pUserDevList->stUserDeviceInfo[i]);
        IVS_BOOL isExist = FALSE;
        for (iterC = User_Dev_List_C.begin(); iterC != User_Dev_List_C.end(); iterC++)
        {
            IVS_USER_DEVICE_INFO* pDev = (IVS_USER_DEVICE_INFO*)(*iterC);
            char cDevDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCode, IVS_DOMAIN_CODE_LEN, pUserDevInfo->cDevDomainCode, IVS_DOMAIN_CODE_LEN);
            char cDevDomainCodeC[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCodeC, IVS_DOMAIN_CODE_LEN, pDev->cDevDomainCode, IVS_DOMAIN_CODE_LEN);

            char cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCode, IVS_DEV_CODE_LEN, pUserDevInfo->cDevCode, IVS_DEV_CODE_LEN);
            char cDevCodeC[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCodeC, IVS_DEV_CODE_LEN, pDev->cDevCode, IVS_DEV_CODE_LEN);

            //1.所有字段完全相同，判为重复
            IVS_BOOL isAllEqual = (0 == strcmp(pUserDevInfo->cDevGroupCode, pDev->cDevGroupCode) &&
                0 == strcmp(cDevDomainCode, cDevDomainCodeC) &&
                0 == strcmp(cDevCode, cDevCodeC) &&
                pUserDevInfo->bIsExDomain == pDev->bIsExDomain &&
                pUserDevInfo->bIsTotalGroup == pDev->bIsTotalGroup);

            //pUserDevList有组权限，用户又有组下单个设备权限，去掉单个设备权限，保留组权限
            IVS_BOOL isInclude = (pUserDevInfo->bIsTotalGroup && 
                0 == strcmp(pUserDevInfo->cDevGroupCode, pDev->cDevGroupCode) &&
                0 == strcmp(cDevDomainCode, cDevDomainCodeC));

            //用户已经有组权限，pUserDevList又有组下单个设备权限，去掉单个设备权限，保留组权限
            IVS_BOOL isUserInclude = (pDev->bIsTotalGroup && 
                0 == strcmp(pDev->cDevGroupCode, pUserDevInfo->cDevGroupCode) &&
                0 == strcmp(cDevDomainCode, cDevDomainCodeC));

            isExist = (isAllEqual || isInclude || isUserInclude);

            if(isExist)
            {
                break;
            }
        }
        if(!isExist)
        {
            User_Dev_List_0.push_back(pUserDevInfo);
        }
    }

    //SDK将用户的设备列表（列表0+列表a）保存到用户所在域
    std::list<IVS_USER_DEVICE_INFO *> User_Dev_List_All = User_Dev_List_0;
    std::list<IVS_USER_DEVICE_INFO *>::iterator iter0;
    std::list<IVS_USER_DEVICE_INFO *>::iterator iterA = User_Dev_List_A.begin();
    for (; iterA != User_Dev_List_A.end(); iterA++)
    {
        pUserDevInfo = (IVS_USER_DEVICE_INFO*)(*iterA);//const_cast<IVS_USER_DEVICE_INFO*>(&pUserDevList->stUserDeviceInfo[i]);
        IVS_BOOL isExist = FALSE;
        for (iter0 = User_Dev_List_0.begin(); iter0 != User_Dev_List_0.end(); iter0++)
        {
            IVS_USER_DEVICE_INFO* pDev = (IVS_USER_DEVICE_INFO*)(*iter0);
            char cDevDomainCodeA[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCodeA, IVS_DOMAIN_CODE_LEN, pUserDevInfo->cDevDomainCode, IVS_DOMAIN_CODE_LEN);
            char cDevDomainCodeC[IVS_DOMAIN_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevDomainCodeC, IVS_DOMAIN_CODE_LEN, pDev->cDevDomainCode, IVS_DOMAIN_CODE_LEN);

            char cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCode, IVS_DEV_CODE_LEN, pUserDevInfo->cDevCode, IVS_DEV_CODE_LEN);
            char cDevCodeC[IVS_DEV_CODE_LEN + 1] = {0};
            (void)CToolsHelp::Memcpy(cDevCodeC, IVS_DEV_CODE_LEN, pDev->cDevCode, IVS_DEV_CODE_LEN);

            //1.所有字段完全相同，判为重复
            IVS_BOOL isAllEqual = (0 == strcmp(pUserDevInfo->cDevGroupCode, pDev->cDevGroupCode) &&
                0 == strcmp(cDevDomainCodeA, cDevDomainCodeC) &&
                0 == strcmp(cDevCode, cDevCodeC) &&
                pUserDevInfo->bIsExDomain == pDev->bIsExDomain &&
                pUserDevInfo->bIsTotalGroup == pDev->bIsTotalGroup);

            //pUserDevList有组权限，用户又有组下单个设备权限，去掉单个设备权限，保留组权限
            IVS_BOOL isInclude = (pUserDevInfo->bIsTotalGroup && 
                0 == strcmp(pUserDevInfo->cDevGroupCode, pDev->cDevGroupCode) &&
                0 == strcmp(cDevDomainCodeA, cDevDomainCodeC));

            //用户已经有组权限，pUserDevList又有组下单个设备权限，去掉单个设备权限，保留组权限
            IVS_BOOL isUserInclude = (pDev->bIsTotalGroup && 
                0 == strcmp(pDev->cDevGroupCode, pUserDevInfo->cDevGroupCode) &&
                0 == strcmp(cDevDomainCodeC, cDevDomainCodeA));

            isExist = (isAllEqual || isInclude || isUserInclude);

            if(isExist)
            {
                break;
            }
        }
        if(!isExist)
        {
            User_Dev_List_All.push_back(pUserDevInfo);
        }
    }

    iRet = CUserInfoMgr::SetUserDeviceListInUserDomain(pDomainCode, uiUserID, User_Dev_List_All);

    if(IVS_SUCCEED != iRet)
    {
		//add by zwx211831,Date:20140715,删除开辟的内存空间，防止内存泄露.
		IVS_DELETE(pAdminUserDevList, MUILI);
		IVS_DELETE(pUserDevList2, MUILI);
		//end
        BP_RUN_LOG_ERR(IVS_FAIL, "Set User Device List", "To save the user device to the user domain failure");
        return iRet;
    }

    //遍历用户域和所有下级域
    CmdMap cmdMap;
	CmdMap tmpCmdMap;  //域编码作为唯一值

    std::string strPublicDomainCode;
    IVS_UINT32 iSize = domainCodeList.size();

    for(IVS_UINT32 j = 0; j < iSize; j++)
    {
        std::string strDomainCode = domainCodeList.at(j);

		//过滤掉上级域,留下本域和下级域
		if (!m_UserMgr->IsInDomainTree(strDomainCode))
		{
			BP_RUN_LOG_INF("domain code out domain tree","DomainCode = %s",strDomainCode.c_str());
			continue;
		}

        // 寻找两个域的最小公共祖先节点
        iRet = m_UserMgr->GetSameSuperDomainCode(pDomainCode, strDomainCode.c_str(), strPublicDomainCode);
        // 如果用户域和(设备域和用户域的最小公共节点)是一样的，那就不需要重新保存
        if(0 == strcmp(pDomainCode, strPublicDomainCode.c_str()))
        {
            continue;
        }
        if(IVS_SUCCEED != iRet)
        {
			//add by zwx211831,Date:20140715,删除开辟的内存空间，防止内存泄露.
			IVS_DELETE(pAdminUserDevList, MUILI);
			IVS_DELETE(pUserDevList2, MUILI);
			//end
            BP_RUN_LOG_ERR(IVS_FAIL, "Get Same Super DomainCode", "Not found the minimum public ancestors");
            return iRet;
        }

        CXml xmlReq;
        CUserInfoMgrXMLProcess::SetUserDevListGetXML(strPublicDomainCode.c_str(), pDomainCode, uiUserID, User_Dev_List_0, xmlReq);
        unsigned int xmlLen = 0;
        const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
        CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

        // 构造带域的请求消息，并发送
        CSendNssMsgInfo sendNssMsgInfo;
        sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
        sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
        sendNssMsgInfo.SetReqID(NSS_SET_USER_DEV_REQ);
        sendNssMsgInfo.SetReqData(pReq);	
        sendNssMsgInfo.SetDomainCode(strPublicDomainCode);

        CCmd *pCmd = m_UserMgr->BuildCmd(sendNssMsgInfo);
		if (NULL == pCmd)
		{
			BP_RUN_LOG_INF("build cmd error","cmd req id=%d, domain code=%s",NSS_SET_USER_DEV_REQ,strPublicDomainCode.c_str());
			continue;
		}

		if (tmpCmdMap.empty())
		{
			(void)tmpCmdMap.insert(std::make_pair(strPublicDomainCode, pCmd));
			(void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
		}
		else
		{
			// 判断需要发送的消息的域编码是否相同，不同则插入;
			CmdMapIterator IpIter = tmpCmdMap.find(strPublicDomainCode);
			if (IpIter != tmpCmdMap.end())
			{
				BP_RUN_LOG_INF("Set User Device List","DomainCode[%s] is already in cmdmap.",strPublicDomainCode.c_str());
				continue;
			}
			else
			{
				(void)tmpCmdMap.insert(std::make_pair(strPublicDomainCode, pCmd));
				(void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
			}
		}
    }

    if (!cmdMap.empty())
    {
        IVS_UINT32 uiTimeout = CSDKConfig::instance().GetTimeOutValue();
        iRet = CDispatcher::instance().dispatcherSyncMsgOutBatch(cmdMap, uiTimeout);
        if(IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "Set User Device List", "To save the user device to the ancestors domain failure");
        }
    }
	else
	{
		BP_RUN_LOG_INF("Set User Device List"," cmdmap is null.");
	}

    IVS_DELETE(pAdminUserDevList, MUILI);
    IVS_DELETE(pUserDevList2, MUILI);

    return iRet;
}

// 获取用户设备列表
IVS_INT32 CUserInfoMgr::GetUserDeviceList(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 /*uiBufferSize*/)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
    CHECK_POINTER(pUserDevList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::GetUserDeviceListGetXML(strDomainCode.c_str(), uiUserID, pIndexRange, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_GET_USER_DEV_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "SendCmd to SMU Return Failed", "NA");
        return  iRet;
    }

    CHECK_POINTER(strpRsp.c_str(), IVS_OPERATE_MEMORY_ERROR);

    CXml xmlRsp;
    if(!xmlRsp.Parse(strpRsp.c_str()))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get User Device List", "xml parsing error");
        return IVS_XML_INVALID;
    }
    iRet = CUserInfoMgrXMLProcess::UserDeviceListParseXML(xmlRsp, pUserDevList);  

    return iRet;
}

// 修改密码
IVS_INT32 CUserInfoMgr::ChangePWD(const IVS_CHAR* pOldPWD, const IVS_CHAR* pNewPWD)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pOldPWD, IVS_PARA_INVALID);
    CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    // 获取本域编码
    std::string strDomainCode;
    m_UserMgr->GetDomainCode(strDomainCode);
    // 获取本域LinkID
    CLinkInfo oLinkIDInfo;
    bool bRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(m_UserMgr->GetSessionID(), strDomainCode, oLinkIDInfo);
    if (!bRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Change PWD", "Get SmuLinkIDInfo By DomainCode failed");
        return IVS_FAIL;
    }
    // 初始化密钥交换类对象,完成密钥交换
    CSDKSecurityClient oSecurityClient;
    int iInitSecurityClient = oSecurityClient.Init(NET_ELE_SMU_NSS, oLinkIDInfo.GetStrLinkId());
    if (IVS_SUCCEED != iInitSecurityClient)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Change PWD", "oSecurityClient Init failed");
        return IVS_FAIL;
    }
    // 加密新、旧密码
    std::string strOldPwdTemp;
    std::string strNewPwdTemp;
    iRet = oSecurityClient.EncryptString(pOldPWD, strOldPwdTemp);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Change PWD", "Encrypt pOldPWD failed");
        return iRet;
    }
    iRet = oSecurityClient.EncryptString(pNewPWD, strNewPwdTemp);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Change PWD", "Encrypt pNewPWD failed");
        return iRet;
    }
    ////创建要发送的CMD，拼装了NSS消息头
    CXml xmlReq;
    CUserInfoMgrXMLProcess::ChangePWDGetXML(m_UserMgr->GetLoginId(), strOldPwdTemp.c_str(), strNewPwdTemp.c_str(), xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();
    CCmd *pCmd = CNSSOperator::instance().BuildCmd(oLinkIDInfo.GetStrLinkId(), NET_ELE_SMU_NSS, NSS_MOD_PASSWORD_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
    //发送nss协议
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
    HW_DELETE(pCmdRsp);
    return iRet;
}

// 重置密码
IVS_INT32 CUserInfoMgr::ResetPWD(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, const IVS_CHAR* pNewPWD)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_UserMgr, strDomainCode, strLinkID);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Reset PWD", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
    // 加密密码
    std::string strNewPwdTemp;
    iRet = oSecurityClient.EncryptString(pNewPWD, strNewPwdTemp);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Reset PWD", "Encrypt String failed");
        return IVS_FAIL;
    }
    // 构造请求
    CXml xmlReq;
    CUserInfoMgrXMLProcess::ResetPWDGetXML(strDomainCode.c_str(), uiUserID, strNewPwdTemp.c_str(), xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();
    CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_RESET_PASSWORD_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
    //发送nss消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
    HW_DELETE(pCmdRsp);
    return iRet;
}

// 校验输入的密码是否符合账号规则
IVS_INT32 CUserInfoMgr::CheckPWDByRole(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, const IVS_CHAR* pPWD)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pPWD, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }
    /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_UserMgr, strDomainCode, strLinkID);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Check PWD ByRole", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
    // 加密密码
    std::string strNewPwdTemp;
    iRet = oSecurityClient.EncryptString(pPWD, strNewPwdTemp);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Check PWD ByRole", "Encrypt String failed");
        return IVS_FAIL;
    }
    // 创建要发送的CMD，拼装了NSS消息头
    CXml xmlReq;
    CUserInfoMgrXMLProcess::CheckPWDByRoleGetXML(strDomainCode.c_str(), uiRoleID, pLoginName, strNewPwdTemp.c_str(), xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();
    CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_CHECK_PWD_RULE_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
    //发送nss消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
    HW_DELETE(pCmdRsp);
    return iRet;
}

// 根据角色获取符合账号规则的密码
IVS_INT32 CUserInfoMgr::GetAutoCreatePWD(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, IVS_CHAR* pNewPWD, IVS_UINT32 /*uiPWDLen*/)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }
    /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_UserMgr, strDomainCode, strLinkID);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Auto CreatePWD", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }

    //创建要发送的CMD，拼装了NSS消息头
    CXml xmlReq;
    CUserInfoMgrXMLProcess::GetAutoCreatePWDGetXML(strDomainCode.c_str(), uiRoleID, pLoginName, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();
    CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_GET_AUTOCREATE_PWD_REQ, pReq, strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
    HW_DELETE(pCmdRsp);
    if(iRet == IVS_SUCCEED)
    {
        CHECK_POINTER(pRsp, IVS_OPERATE_MEMORY_ERROR);
        CXml xmlRsp;
        if(!xmlRsp.Parse(pRsp))
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Auto CreatePWD", "xml parsing error");
            IVS_DELETE(pRsp, MUILI);
            return IVS_XML_INVALID;
        }
        iRet = CUserInfoMgrXMLProcess::GetAutoCreatePWDParseXML(xmlRsp, pNewPWD); 
        // 解密数据后拼装
        std::string strNewPWD;
        if (IVS_SUCCEED != oSecurityClient.DecryptString(pNewPWD, strNewPWD))
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Auto Create PWD", "Decrypt PWD fail");
            IVS_DELETE(pRsp, MUILI);
            return IVS_FAIL;
        }
        memset(pNewPWD, 0, IVS_PWD_LEN);
        if (!CToolsHelp::Memcpy(pNewPWD, IVS_PWD_LEN, strNewPWD.c_str(), strlen(strNewPWD.c_str())))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy PWD", "Memcpy failed");
            IVS_DELETE(pRsp, MUILI);
            return IVS_ALLOC_MEMORY_ERROR;
        }
    }
    IVS_DELETE(pRsp, MUILI);
    return iRet;
}

// 获取密码过期信息
IVS_INT32 CUserInfoMgr::GetPWDExpireInfo(IVS_PWD_EXPIRE_INFO* pPWDExpireInfo)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pPWDExpireInfo, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    CXml xmlReq;
    CUserInfoMgrXMLProcess::GetPWDExpireInfoGetXML(m_UserMgr->GetLoginId(), xmlReq);
    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
    //创建要发送的CMD，拼装了NSS消息头
    CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_PWD_EXPIRE_INFO_REQ, pReq,strSMULinkID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

    //发送消息,同步获取响应
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_OPERATE_MEMORY_ERROR);

    //const IVS_CHAR* pRsp转换为XML并转换为数据结构
    IVS_INT32 iRet = IVS_FAIL;
    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);
	CHECK_POINTER(pRsp, IVS_OPERATE_MEMORY_ERROR);
    if(IVS_SUCCEED == iRet)
    {
        //CHECK_POINTER(pRsp, IVS_XML_INVALID);
        CXml xmlRsp;
        if(!xmlRsp.Parse(pRsp))
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get PWD Expire Info", "xml parsing error");
            IVS_DELETE(pRsp, MUILI);
            return IVS_XML_INVALID;
        }
        iRet = CUserInfoMgrXMLProcess::GetPWDExpireInfoParseXML(xmlRsp, pPWDExpireInfo);
    }
	IVS_DELETE(pRsp, MUILI);

    return iRet;
}

// 根据用户ID签退用户的所有会话
IVS_INT32 CUserInfoMgr::SignOutUser(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::SignOutUserGetXML(strDomainCode.c_str(), uiUserID, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_SIGNOUT_ONLINE_USER_BYID_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 根据用户ID签退用户的所有会话
IVS_INT32 CUserInfoMgr::SignOutUserByRole(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::GetRoleInfoByRoleIDGetXML(strDomainCode.c_str(), uiRoleID, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_SIGNOUT_ONLINE_USER_BYROLEID_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 签退在线用户会话
IVS_INT32 CUserInfoMgr::SignOutOnlineUser(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserOnlineID)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    std::string strDomainCode;
    if(NULL == pDomainCode || '\0' == pDomainCode[0])
    {
        m_UserMgr->GetDomainCode(strDomainCode);
    }
    else
    {
        strDomainCode = pDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::SignOutOnlineUserGetXML(strDomainCode.c_str(), uiUserOnlineID, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_SIGNOUT_ONLINE_USER_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(strDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 在用户所在域设置用户设备列表
IVS_INT32 CUserInfoMgr::SetUserDeviceListInUserDomain(const IVS_CHAR* pUserDomainCode, IVS_UINT32 uiUserID, const std::list<IVS_USER_DEVICE_INFO *> &pUserDevList)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlReq;
    CUserInfoMgrXMLProcess::SetUserDevListGetXML(pUserDomainCode, pUserDomainCode, uiUserID, pUserDevList, xmlReq);
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    // 构造带域的请求消息，并发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_SET_USER_DEV_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    sendNssMsgInfo.SetDomainCode(pUserDomainCode);
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    return iRet;
}

// 查询设备组列表
IVS_INT32 CUserInfoMgr::GetDeviceGroup(const IVS_CHAR* pDomainCode, const IVS_CHAR* cDevGroupCode, IVS_BOOL bIsExDomain, std::vector<IVS_USER_DEVICE_INFO> &userDevList)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    IVS_DEVICE_GROUP_LIST *pDevGroupList = NULL;
    IVS_UINT32 iBufferSize = 0;

    iRet = CUserInfoMgr::AllocateDevGroupSpace(pDevGroupList, &iBufferSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pDevGroupList, MUILI);
        BP_RUN_LOG_ERR(iRet, "Allocate memory space", "failure");
        return iRet;
    }

    // 获取list
    iRet = m_UserMgr->GetDeviceGroupMgr().GetDeviceGroup(pDomainCode, cDevGroupCode, pDevGroupList, iBufferSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pDevGroupList, MUILI);
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Device Group", "failure");
        return iRet;
    }

    // 把设备组转换为用户设备(组)IVS_DEVICE_GROUP -- IVS_USER_DEVICE_INFO
    IVS_UINT32 uiNum = pDevGroupList->uiTotal;/*(pDevGroupList->stIndexRange.uiToIndex - pDevGroupList->stIndexRange.uiFromIndex) + 1;*/
	// 判断大小
	IVS_UINT32 uiRetSize = sizeof(IVS_DEVICE_GROUP_LIST) + (uiNum-1) * sizeof(IVS_DEVICE_GROUP);
	if (uiRetSize > iBufferSize)
	{
		IVS_DELETE(pDevGroupList, MUILI);
		BP_RUN_LOG_ERR(iRet, "Allocate memory space", "failure");
		return IVS_OPERATE_MEMORY_ERROR; 
	}
    for (IVS_UINT32 i = 0; i < uiNum; i++)
    {
        IVS_DEVICE_GROUP *pDevGroup = &pDevGroupList->stDeviceGroup[i];

        IVS_USER_DEVICE_INFO pUserDevInfo;
        memset(&pUserDevInfo, 0, sizeof(IVS_USER_DEVICE_INFO));

        pUserDevInfo.bIsTotalGroup = 1;
        pUserDevInfo.bIsExDomain = bIsExDomain;
		//modify by zwx211831, Date:20140804, 将子组的设备组编码加上“#域编码”
		std::string strDevGroupCode(pDevGroup->cGroupCode);
		if (strDevGroupCode.compare(0, 2, "0#"))
		{
			strDevGroupCode += "#";
			strDevGroupCode += pDomainCode;
		}
        if (!CToolsHelp::Memcpy(pUserDevInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, strDevGroupCode.c_str(), IVS_DEVICE_GROUP_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserDevInfo->cDevGroupCode", "CToolsHelp::Memcpy failed");
        }
		//end
        if (!CToolsHelp::Memcpy(pUserDevInfo.cDevDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pUserDevInfo->cDevDomainCode", "CToolsHelp::Memcpy failed");
        }

        userDevList.push_back(pUserDevInfo);
    }

	IVS_DELETE(pDevGroupList, MUILI);

    return iRet;
}

// 查询设备组列表 -- 分配内存
IVS_INT32 CUserInfoMgr::AllocateDevGroupSpace(IVS_DEVICE_GROUP_LIST* &pDevGroupList, IVS_UINT32* iBufferSize) const
{
    // 分配返回列表数据结构空间
    IVS_UINT32 iItemNum = IVS_MAX_DEV_GROUP_NUM - 1;
    *iBufferSize = sizeof(IVS_DEVICE_GROUP_LIST) + sizeof(IVS_DEVICE_GROUP) * iItemNum;

    IVS_VOID *pBuffer = new IVS_CHAR[*iBufferSize];
    CHECK_POINTER(pBuffer, IVS_ALLOC_MEMORY_ERROR);
    memset(pBuffer, 0, *iBufferSize);
    pDevGroupList = reinterpret_cast<IVS_DEVICE_GROUP_LIST*>(pBuffer);
    return IVS_SUCCEED;
}

// 查询管理员（当前用户）的设备权限
IVS_INT32 CUserInfoMgr::GetCurrentUserDev(IVS_USER_DEVICE_INFO_LIST* &pUserDevList)
{
    CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    IVS_INDEX_RANGE stIndexRange;
    memset(&stIndexRange, 0, sizeof(IVS_INDEX_RANGE));

    IVS_UINT32 iBufferSize = 0;

    iRet = CUserInfoMgr::AllocateUserDevListSpace(MAX_USER_DEV_NUM, &stIndexRange, pUserDevList, &iBufferSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pUserDevList, MUILI);
        BP_RUN_LOG_ERR(iRet, "Allocate memory space", "failure");
        return iRet;
    }

    // 获取list
    std::string strDomainCode;
    m_UserMgr->GetDomainCode(strDomainCode);
    iRet = CUserInfoMgr::GetUserDeviceList(strDomainCode.c_str(), m_UserMgr->GetUserID(), &stIndexRange, pUserDevList, iBufferSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pUserDevList, MUILI);
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Current User Device List", "failure");
        return iRet;
    }

    IVS_UINT32 uiTotal = pUserDevList->uiTotal;    
    // 如果没有查到所有的记录，再查一次 
    if(uiTotal > MAX_USER_DEV_NUM)
    {
        IVS_DELETE(pUserDevList, MUILI);
        iRet = CUserInfoMgr::AllocateUserDevListSpace(uiTotal, &stIndexRange, pUserDevList, &iBufferSize);
        if(IVS_SUCCEED != iRet)
        {
            IVS_DELETE(pUserDevList, MUILI);
            BP_RUN_LOG_ERR(iRet, "Allocate memory space", "failure");
            return iRet;
        }

        // 获取list
        iRet = CUserInfoMgr::GetUserDeviceList(strDomainCode.c_str(), m_UserMgr->GetUserID(), &stIndexRange, pUserDevList, iBufferSize);
        if(IVS_SUCCEED != iRet)
        {
			//add by zwx211831,Date:20140715,删除开辟的内存空间，防止内存泄露.
			IVS_DELETE(pUserDevList, MUILI);
			//end
            BP_RUN_LOG_ERR(IVS_FAIL, "Get Current User Device List", "failure");
            return iRet;
        }
    }
    return iRet;
}

IVS_INT32 CUserInfoMgr::GetUserDevList(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_USER_DEVICE_INFO_LIST* &pUserDevList)
{
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;

    IVS_INDEX_RANGE stIndexRange;
    memset(&stIndexRange, 0, sizeof(IVS_INDEX_RANGE));

    IVS_UINT32 iBufferSize = 0;

    iRet = CUserInfoMgr::AllocateUserDevListSpace(MAX_USER_DEV_NUM, &stIndexRange, pUserDevList, &iBufferSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pUserDevList, MUILI);
        BP_RUN_LOG_ERR(iRet, "Allocate memory space", "failure");
        return iRet;
    }

    // 获取list
    iRet = CUserInfoMgr::GetUserDeviceList(pDomainCode, uiUserID, &stIndexRange, pUserDevList, iBufferSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pUserDevList, MUILI);
        BP_RUN_LOG_ERR(IVS_FAIL, "Get User Dev List", "failure");
        return iRet;
    }

    IVS_UINT32 uiTotal = pUserDevList->uiTotal;
    // 如果没有查到所有的记录，再查一次 
    if(uiTotal > MAX_USER_DEV_NUM)
    {
        IVS_DELETE(pUserDevList, MUILI);
        iRet = CUserInfoMgr::AllocateUserDevListSpace(uiTotal, &stIndexRange, pUserDevList, &iBufferSize);
        if(IVS_SUCCEED != iRet)
        {
            IVS_DELETE(pUserDevList, MUILI);
            BP_RUN_LOG_ERR(iRet, "Allocate memory space", "failure");
            return iRet;
        }

        // 获取list
        iRet = CUserInfoMgr::GetUserDeviceList(pDomainCode, uiUserID, &stIndexRange, pUserDevList, iBufferSize);
        if(IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "Get User Dev List", "failure");
            return iRet;
        }
    }
    return iRet;
}

IVS_INT32 CUserInfoMgr::AllocateUserDevListSpace(IVS_UINT32 uiToIndex, IVS_INDEX_RANGE* pIndexRange, IVS_USER_DEVICE_INFO_LIST* &pUserDevList, IVS_UINT32* iBufferSize) const
{
    IVS_DEBUG_TRACE("");

    pIndexRange->uiFromIndex = 1;
    pIndexRange->uiToIndex = uiToIndex;

    // 分配返回列表数据结构空间
    IVS_UINT32 iItemNum = (pIndexRange->uiToIndex - pIndexRange->uiFromIndex);
    *iBufferSize = sizeof(IVS_USER_DEVICE_INFO_LIST) + sizeof(IVS_USER_DEVICE_INFO) * iItemNum;

    IVS_VOID *pBuffer = new IVS_CHAR[*iBufferSize];
    CHECK_POINTER(pBuffer, NULL);
    memset(pBuffer, 0, *iBufferSize);
    pUserDevList = reinterpret_cast<IVS_USER_DEVICE_INFO_LIST*>(pBuffer);
    return IVS_SUCCEED;
}


//根据摄像机Code，查询该摄像机正在给哪些用户提供视频业务
IVS_INT32 CUserInfoMgr::GetStreamListByCam(const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	IVS_UINT32 uiBufSize,
	IVS_STREAM_LIST_BY_CAM* pStreamListByCam)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pStreamListByCam, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);

	std::string strDomainCode;
	if(NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		m_UserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "ParseDevCode Failed");
		return iRet;
	}

	CXml xmlReq;
	iRet = CUserInfoMgrXMLProcess::GetStreamListByCamGetXML(chDomaCode, chDevCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetStreamListByCam", "GetStreamListByCamGetXML failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	if (NULL == pReq)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetStreamListByCam", "xmlReq.GetXMLStream failed");
		return IVS_XML_INVALID;
	}

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_STREAM_LIST_BY_CAM_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetStreamListByCam", "SendCmd failed");
		return iRet;
	}
	CHECK_POINTER(strpRsp.c_str(), IVS_OPERATE_MEMORY_ERROR);

	CXml xmlRsp;
	if(!xmlRsp.Parse(strpRsp.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetStreamListByCam", "xmlRsp.Parse failed");
		return IVS_XML_INVALID;
	}

	iRet = CUserInfoMgrXMLProcess::GetStreamListByCamParseXML(xmlRsp, uiBufSize, pStreamListByCam);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetStreamListByCam", "GetStreamListByCamParseXML failed");
	}

	return iRet;
}


//根据用户编码，查询该用户正在给调用哪些摄像机的视频业务
IVS_INT32 CUserInfoMgr::GetStreamListByUser(const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiUserID,
	IVS_UINT32 uiBufSize,
	IVS_STREAM_LIST_BY_USER* pStreamListByUser)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pStreamListByUser, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);

	std::string strLocalDomainCode;
	m_UserMgr->GetDomainCode(strLocalDomainCode);
	if (strLocalDomainCode.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetStreamListByUser", "GetDomainCode failed, LocalDomainCode is NULL");
		return IVS_FAIL;
	}

	CXml xmlReq;
	IVS_INT32 iRet = CUserInfoMgrXMLProcess::GetStreamListByUserGetXML(pDomainCode, uiUserID, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetStreamListByUser", "GetStreamListByUserGetXML failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	if (NULL == pReq)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetStreamListByUser", "xmlReq.GetXMLStream failed");
		return IVS_XML_INVALID;
	}

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_STREAM_LIST_BY_USER_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(strLocalDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetStreamListByUser", "SendCmd failed");
		return iRet;
	}
	CHECK_POINTER(strpRsp.c_str(), IVS_OPERATE_MEMORY_ERROR);

	CXml xmlRsp;
	if(!xmlRsp.Parse(strpRsp.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetStreamListByUser", "xmlRsp.Parse failed");
		return IVS_XML_INVALID;
	}

	iRet = CUserInfoMgrXMLProcess::GetStreamListByUserParseXML(xmlRsp, uiBufSize, pStreamListByUser);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetStreamListByUser", "GetStreamListByUserParseXML failed");
	}

	return iRet;
}

//关闭指定用户视频业务
IVS_INT32 CUserInfoMgr::ShutdownStreamByUser(const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiUserID,  
	const IVS_CHAR* pCameraCode,
	IVS_INT32 iStreamType,
	IVS_INT32 iDuration)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_UserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "ParseDevCode Failed");
		return iRet;
	}

	//获取请求xml流
	CXml xmlReq;
	iRet = CUserInfoMgrXMLProcess::ShutdownStreamByUserGetXML(chDomaCode, uiUserID, 
		chDevCode, iStreamType, iDuration, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ShutdownStreamByUser", "ShutdownStreamByUserGetXML failed");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	if (NULL == pReq)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "ShutdownStreamByUser", "xmlReq.GetXMLStream failed");
		return IVS_XML_INVALID;
	}

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SHUTDOWN_STREAM_BY_USER_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_UserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ShutdownStreamByUser", "SendCmd failed");
	}

	return iRet;
}


//用户名、密码验证
IVS_INT32 CUserInfoMgr::PasswdValidation(const IVS_CHAR* pLoginName,
	const IVS_CHAR *pPasswd)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pLoginName, IVS_PARA_INVALID);
	CHECK_POINTER(pPasswd, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);

	//获取本域编码
	std::string strLocalDomainCode;
	m_UserMgr->GetDomainCode(strLocalDomainCode);

	//获取本域SMU的LinkID，用以加密用户名和密码
	std::string strLocalDomainLinkID;
	IVS_INT32 iRet = m_UserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strLocalDomainLinkID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "PasswdValidation", "GetLocalDomainLinkID failed");
		return IVS_FAIL;
	}

	// 初始化信息安全客户端对象 TODO 与SMU SCU一起提交
	CSDKSecurityClient oSecurityClient;
	int iInitSecurityClient = oSecurityClient.Init(NET_ELE_SMU_NSS, strLocalDomainLinkID);
	if (IVS_SUCCEED != iInitSecurityClient)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "PasswdValidation", "oSecurityClient Init failed");
		return IVS_FAIL;
	}

	//加密用户名和密码
	std::string strLoignCipherName;
	std::string strLoignCipherPWD;
	int iEncryptRet = oSecurityClient.EncryptString(pLoginName, strLoignCipherName);
	iEncryptRet += oSecurityClient.EncryptString(pPasswd, strLoignCipherPWD);
	if (IVS_SUCCEED != iEncryptRet)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "PasswdValidation", "EncryptString failed");
		return IVS_FAIL;
	}

	//获取请求xml流
	CXml xmlReq;
	iRet = CUserInfoMgrXMLProcess::PasswdValidationGetXML(strLoignCipherName, strLoignCipherPWD, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "PasswdValidation", "PasswdValidationGetXML failed");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	if (NULL == pReq)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "PasswdValidation", "xmlReq.GetXMLStream failed");
		return IVS_XML_INVALID;
	}

	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLocalDomainLinkID, NET_ELE_SMU_NSS, NSS_PASSWD_VALIDATION_REQ, pReq, strSecurityTransID);
	if (NULL == pCmd)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "PasswdValidation", "Build SMUCmd failed");
		return IVS_FAIL;
	}

	//发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	if(NULL == pCmdRsp)
	{
		BP_RUN_LOG_ERR(IVS_NET_RECV_TIMEOUT, "PasswdValidation", "SendSyncCmd but no response or time is out");
		return IVS_NET_RECV_TIMEOUT;
	}

	//响应无xml消息体时，ParseCmd2XML返回NULL，不会分配堆内存，不用delete pRspXml
	char* pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp,iRet);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "PasswdValidation", "ParseCmd2XML failed");
	}

	IVS_DELETE(pRspXml);
	HW_DELETE(pCmdRsp);
	return iRet;
}