#include "OCXLoginXMLProcess.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"
COCXLoginXMLProcess::COCXLoginXMLProcess()
{
}

COCXLoginXMLProcess::~COCXLoginXMLProcess()
{
}

// 生成登录请求结构体
IVS_INT32 COCXLoginXMLProcess::GetLoginReqInfo(LPCTSTR pUserName, LPCTSTR pPWD, LPCTSTR pServerIP,LPCTSTR pPort, ULONG ulClientType, IVS_LOGIN_INFO &pLoginInfo)
{
    IVS_DEBUG_TRACE("");
    CHECK_POINTER(pUserName,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pPWD,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pServerIP,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pPort,IVS_OPERATE_MEMORY_ERROR);
    if (!CToolsHelp::Memcpy(pLoginInfo.cUserName, IVS_NAME_LEN, pUserName, strlen(pUserName)))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get LoginReqInfo", "copy pUserName failed");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    //memcpy(pLoginInfo.cUserName, pUserName, strlen(pUserName));
    if (!CToolsHelp::Memcpy(pLoginInfo.pPWD, IVS_PWD_LEN, pPWD, strlen(pPWD)))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get LoginReqInfo", "copy pPWD failed");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    //memcpy(pLoginInfo.pPassword, pPWD, strlen(pPWD));
    pLoginInfo.stIP.uiIPType = static_cast<IVS_UINT32>(IP_V4);
    if (!CToolsHelp::Memcpy(pLoginInfo.stIP.cIP, IVS_IP_LEN, pServerIP, strlen(pServerIP)))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get LoginReqInfo", "copy pServerIP failed");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    //memcpy(pLoginInfo.stIP.cIP, pServerIP, strlen(pServerIP));
    pLoginInfo.uiPort = static_cast<IVS_UINT32>(atoi(pPort));
    pLoginInfo.uiClientType = static_cast<IVS_UINT32>(ulClientType);
    return IVS_SUCCEED;
}

//生成单点登录结构体
IVS_INT32 COCXLoginXMLProcess::GetLoginByTicketReqInfo(LPCTSTR pTicketID, 
	LPCTSTR pServerIP,
	LPCTSTR pPort, 
	ULONG ulClientType, 
	IVS_LOGIN_INFO_EX &pLoginInfoEx)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pTicketID,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pServerIP,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPort,IVS_OPERATE_MEMORY_ERROR);

	if (!CToolsHelp::Memcpy(pLoginInfoEx.cTicketID, IVS_TICKET_LEN, pTicketID, strlen(pTicketID)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get LoginByTicketReqInfo", "copy pTicketID failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	pLoginInfoEx.stIP.uiIPType = static_cast<IVS_UINT32>(IP_V4);
	if (!CToolsHelp::Memcpy(pLoginInfoEx.stIP.cIP, IVS_IP_LEN, pServerIP, strlen(pServerIP)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get LoginReqInfo", "copy pServerIP failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	pLoginInfoEx.uiPort = static_cast<IVS_UINT32>(atoi(pPort));
	pLoginInfoEx.uiClientType = static_cast<IVS_UINT32>(ulClientType);
	return IVS_SUCCEED;
}

// 拼装获取用户ID响应xml
void COCXLoginXMLProcess::GetUserIDRspXML(IVS_INT32 iRet, IVS_UINT32 uiUserID, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

    (void)xml.AddElem("UserID");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<int>(uiUserID)).c_str());
    xml.OutOfElem();
}

// 拼装用户下线通知xml
IVS_INT32 COCXLoginXMLProcess::UserOfflineNotifyGetXML(const IVS_USER_OFFLINE_INFO* pOfflineNotify, CXml &xml)
{
    IVS_DEBUG_TRACE("");
    CHECK_POINTER(pOfflineNotify, IVS_OPERATE_MEMORY_ERROR);
    
    (void)xml.AddDeclaration("1.0", "UTF-8", "");
    (void)xml.AddElem("Content");

    /*(void)xml.AddChildElem("LoginID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pOfflineNotify->cLoginId);*/

    (void)xml.AddChildElem("OfflineType");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pOfflineNotify->uiOfflineType)).c_str());
    xml.OutOfElem();

    return IVS_SUCCEED;
}
