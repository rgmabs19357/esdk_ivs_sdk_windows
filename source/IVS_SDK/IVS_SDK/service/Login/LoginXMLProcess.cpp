/********************************************************************
filename : LoginXMLProcess.cpp
author : x00192614
created : 2012/12/03
description : 登录xml拼装解析
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/
#include "LoginXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "SDKSecurityClient.h"
#include "IVS_Trace.h"
/*!< socket头文件 */
#ifdef WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#else
#include <iconv.h>
#endif


CLoginXMLProcess::CLoginXMLProcess(void)
{
}

CLoginXMLProcess::~CLoginXMLProcess(void)
{
}

// 获取登录XML
IVS_INT32 CLoginXMLProcess::GetLoginXML(const IVS_LOGIN_INFO* pLoginReqInfo, const std::string& strLoginName, const std::string& strLoginPWD,CUserMgr *pUserObj, CXml& xmlReq )
{
    IVS_DEBUG_TRACE("");
    CHECK_POINTER(pLoginReqInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserObj, IVS_OPERATE_MEMORY_ERROR);
	std::string strLocalIp = GetLocalIp(pLoginReqInfo->stIP.cIP, static_cast<int>(pLoginReqInfo->uiPort));
	if (strLocalIp.empty())
	{
		BP_RUN_LOG_INF("Leave CLogin::GetLoginXML", "CToolsHelp::GetLocalIp fail");
		return IVS_NET_RECV_TIMEOUT;
	}

     //保存本地IP，供启动实况播放使用
    CUserInfo *userInfo = NULL;
    userInfo = pUserObj->GetUserInfoEx();
    CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
    userInfo->SetUserLocalIp(strLocalIp);   

    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    (void)xmlReq.IntoElem();

    //xmlReq.AddChildElem("LoginID");
    //xmlReq.IntoElem();
    //xmlReq.SetElemValue("0");

    //xmlReq.AddElem("LoginName");
    //xmlReq.SetElemValue(sLoginParas.cUserName);

    (void)xmlReq.AddChildElem("LoginName");
    (void)xmlReq.IntoElem();

//     char cTempUserName[IVS_NAME_LEN + 1] = {0};
//     memcpy(cTempUserName, pLoginReqInfo->cUserName, IVS_NAME_LEN);
    (void)xmlReq.SetElemValue(strLoginName.c_str());

	(void)xmlReq.AddElem("LoginPWD");
//     char cTempPassword[IVS_PWD_LEN + 1] = {0};
//     memcpy(cTempPassword, pLoginReqInfo->pPassword, IVS_PWD_LEN);
	(void)xmlReq.SetElemValue(strLoginPWD.c_str());

	(void)xmlReq.AddElem("LoginIP");
	(void)xmlReq.SetElemValue(strLocalIp.c_str()); // 本地IP

	(void)xmlReq.AddElem("LoginPort");            // 本地端口
	(void)xmlReq.SetElemValue("0");

	(void)xmlReq.AddElem("LoginType");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str(static_cast<int>(pLoginReqInfo->uiLoginType)).c_str());

	(void)xmlReq.AddElem("DomainName");
    char cTempDomainName[IVS_DOMAIN_LEN + 1] = {0};
    memcpy(cTempDomainName, pLoginReqInfo->cDomainName, IVS_DOMAIN_LEN);
	(void)xmlReq.SetElemValue(cTempDomainName);

	(void)xmlReq.AddElem("MachineName");
    char cTempMachineName[IVS_MACHINE_NAME_LEN + 1] = {0};
    memcpy(cTempMachineName, pLoginReqInfo->cMachineName, IVS_MACHINE_NAME_LEN);
	(void)xmlReq.SetElemValue(cTempMachineName);

    (void)xmlReq.AddElem("ClientType");
    (void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfo->uiClientType).c_str());

    (void)xmlReq.AddElem("ServerIP");
    char cTempSerIp[IVS_IP_LEN + 1] = {0};
    memcpy(cTempSerIp, pLoginReqInfo->stIP.cIP, IVS_IP_LEN);
    (void)xmlReq.SetElemValue(cTempSerIp);

    (void)xmlReq.AddElem("ServerPort");
    (void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfo->uiPort).c_str());

	xmlReq.OutOfElem();

	BP_RUN_LOG_INF("Leave CLogin::GetLoginXML", "ok");
	return IVS_SUCCEED;
}


//获取单点登录请求xml;
IVS_INT32 CLoginXMLProcess::GetLoginByTicketXML(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, CUserMgr *pUserObj, CXml& xmlReq )
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pLoginReqInfoEx, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUserObj, IVS_OPERATE_MEMORY_ERROR);

	std::string strLocalIp = GetLocalIp(pLoginReqInfoEx->stIP.cIP, static_cast<int>(pLoginReqInfoEx->uiPort));
	if (strLocalIp.empty())
	{
		BP_RUN_LOG_INF("Leave CLogin::GetLoginXML", "CToolsHelp::GetLocalIp fail");
		return IVS_NET_RECV_TIMEOUT;
	}

	//保存本地IP，供启动实况播放使用
	CUserInfo *userInfo = NULL;
	userInfo = pUserObj->GetUserInfoEx();
	CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
	userInfo->SetUserLocalIp(strLocalIp);   

	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.IntoElem();

	(void)xmlReq.AddChildElem("TicketID");
	(void)xmlReq.IntoElem();
	IVS_CHAR cTmpTicketID[IVS_TICKET_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cTmpTicketID, IVS_TICKET_LEN + 1, pLoginReqInfoEx->cTicketID, IVS_TICKET_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "GetLoginByTicketXML", "Copy TicketID Failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xmlReq.SetElemValue(cTmpTicketID);

	(void)xmlReq.AddElem("LoginIP");
	(void)xmlReq.SetElemValue(strLocalIp.c_str()); // 本地IP

	(void)xmlReq.AddElem("LoginPort");            // 本地端口;
	(void)xmlReq.SetElemValue("0");

	(void)xmlReq.AddElem("LoginType");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str(static_cast<int>(pLoginReqInfoEx->uiLoginType)).c_str());

	(void)xmlReq.AddElem("DomainName");
	char cTempDomainName[IVS_DOMAIN_LEN + 1] = {0};
	memcpy(cTempDomainName, pLoginReqInfoEx->cDomainName, IVS_DOMAIN_LEN);
	(void)xmlReq.SetElemValue(cTempDomainName);

	(void)xmlReq.AddElem("MachineName");
	char cTempMachineName[IVS_MACHINE_NAME_LEN + 1] = {0};
	memcpy(cTempMachineName, pLoginReqInfoEx->cMachineName, IVS_MACHINE_NAME_LEN);
	(void)xmlReq.SetElemValue(cTempMachineName);

	(void)xmlReq.AddElem("ClientType");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfoEx->uiClientType).c_str());

	(void)xmlReq.AddElem("ServerIP");
	char cTempSerIp[IVS_IP_LEN + 1] = {0};
	memcpy(cTempSerIp, pLoginReqInfoEx->stIP.cIP, IVS_IP_LEN);
	(void)xmlReq.SetElemValue(cTempSerIp);

	(void)xmlReq.AddElem("ServerPort");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfoEx->uiPort).c_str());

	xmlReq.OutOfElem();

	BP_RUN_LOG_INF("Leave CLogin::GetLoginByTicketXML", "ok");
	return IVS_SUCCEED;
}


IVS_INT32 CLoginXMLProcess::GetReLoginXML(const std::string& strLoginName, const std::string& strLoginPWD, CUserInfo *pUserInfo, CXml& reqXml )
{
    IVS_DEBUG_TRACE("");
    CHECK_POINTER(pUserInfo, IVS_OPERATE_MEMORY_ERROR);
    std::string strLocalIp = pUserInfo->GetUserLocalIp();

    (void)reqXml.AddDeclaration("1.0","UTF-8","");
    (void)reqXml.AddElem("Content");
    (void)reqXml.IntoElem();

    //xmlReq.AddChildElem("LoginID");
    //xmlReq.IntoElem();
    //xmlReq.SetElemValue("0");

    //xmlReq.AddElem("LoginName");
    //xmlReq.SetElemValue(sLoginParas.cUserName);

    (void)reqXml.AddChildElem("LoginName");
    (void)reqXml.IntoElem();
    (void)reqXml.SetElemValue(strLoginName.c_str());

    (void)reqXml.AddElem("LoginPWD");
    (void)reqXml.SetElemValue(strLoginPWD.c_str());

    (void)reqXml.AddElem("LoginIP");
    (void)reqXml.SetElemValue(strLocalIp.c_str()); // 本地IP

    (void)reqXml.AddElem("LoginPort");            // 本地端口
    (void)reqXml.SetElemValue("0");

    (void)reqXml.AddElem("LoginType");
    (void)reqXml.SetElemValue(CToolsHelp::Int2Str(pUserInfo->GetLoginType()).c_str());

    (void)reqXml.AddElem("DomainName");
    (void)reqXml.SetElemValue(pUserInfo->GetDomainName().c_str());

    (void)reqXml.AddElem("MachineName");
    (void)reqXml.SetElemValue(pUserInfo->GetMachineName().c_str());

    (void)reqXml.AddElem("ClientType");
    (void)reqXml.SetElemValue(CToolsHelp::Int2Str(pUserInfo->GetClientType()).c_str());

    (void)reqXml.AddElem("ServerIP");
    (void)reqXml.SetElemValue(pUserInfo->GetServerIP().c_str());

    (void)reqXml.AddElem("ServerPort");
    (void)reqXml.SetElemValue(CToolsHelp::Int2Str(pUserInfo->GetServerPort()).c_str());

    reqXml.OutOfElem();
    return IVS_SUCCEED;
}
// 获取摄像机定位XML
IVS_INT32 CLoginXMLProcess::GetFineMainDevXML(const std::string& strDevCode, CXml& xmlReq )
{
	IVS_DEBUG_TRACE("");
	if (strDevCode.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get FineMain Dev XML", "DevCode is empty");
		return IVS_FAIL;
	}
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.AddChildElem("CameraInfo");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("CameraCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(strDevCode.c_str());
	xmlReq.OutOfElem();
	return IVS_SUCCEED;
}

// 解析NvrCode信息
IVS_INT32 CLoginXMLProcess::ParseNvrCodeXML(CXml& xmlRsq, char* pNvrCode)
{
	const char*AttriValue = NULL;   //临时存储单个节点值
	 if (xmlRsq.FindElemEx("Content/NVRInfo"))
	 {
		 (void)xmlRsq.IntoElem();
		 GET_ELEM_VALUE_CHAR("NVRCode",AttriValue, pNvrCode,IVS_NVR_CODE_LEN,xmlRsq);
		 return IVS_SUCCEED;
	 }
// 	if (!xmlRsq.FindElemEx("Content"))
// 	{
// 		BP_RUN_LOG_INF("Leave CLoginXMLProcess::ParseNvrCodeXML", "Content is null");
// 		return IVS_FAIL;
// 	}
// 	(void)xmlRsq.IntoElem();
// 	if (!xmlRsq.FindElemEx("IPCInfo"))
// 	{
// 		BP_RUN_LOG_INF("Leave CLoginXMLProcess::ParseNvrCodeXML", "IPCInfo is null");
// 		return IVS_FAIL;
// 	}
// 	(void)xmlRsq.IntoElem();
// 	GET_ELEM_VALUE_CHAR("IPCCode",AttriValue,pNvrCode,IVS_DEV_CODE_LEN,xmlRsq);
	 BP_RUN_LOG_ERR(IVS_FAIL, "Parse NvrCode XML", "Get NVRCode error");
	return IVS_FAIL;
}

// 用户下线通知
IVS_INT32 CLoginXMLProcess::ParseUserOffLineXML(CXml& xmlRsq, const IVS_CHAR* /*pLoginID*/, IVS_USER_OFFLINE_INFO *stOffLineInfo)
{
    //memset(&stOffLineInfo, 0, sizeof(stOffLineInfo));
    IVS_DEBUG_TRACE("");
    const char *AttriValue = NULL;   //临时存储单个节点值

    if (!xmlRsq.FindElemEx("/"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "CLoginXMLProcess::ParseUserOffLineXML", "Content is null");
        return IVS_XML_INVALID;
    }
    //GET_ELEM_VALUE_CHAR("LoginID", AttriValue, pLoginID, LOGIN_ID_LEN, xmlRsq);
    GET_ELEM_VALUE_NUM_FOR_UINT("OfflineType", AttriValue, stOffLineInfo->uiOfflineType, xmlRsq);
    return IVS_SUCCEED;
}

// 解析登录响应XML
IVS_INT32 CLoginXMLProcess::ParseLoginXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRsp)
{
	IVS_DEBUG_TRACE("");
	memset(&stSRULoginRsp, 0, sizeof(ST_SRU_LOGIN_RSP));
	const char* AttriValue = NULL;   //临时存储单个节点值
	if (!xmlRsq.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "Node Content not found");
		return IVS_FAIL;
	}
	(void)xmlRsq.IntoElem();
    if (!xmlRsq.FindElem("LoginID"))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "Node LoginID not found");
        return IVS_FAIL;
    }
    std::string strLoginID;
    AttriValue = xmlRsq.GetElemValue();
    if (NULL == AttriValue)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "LoginID is NUll");
        return IVS_FAIL;
    }
    if (IVS_SUCCEED != oSecurityClient.DecryptString(AttriValue, strLoginID))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "DecryptString LoginID failed");
        return IVS_FAIL;
    }
    if(!CToolsHelp::Memcpy(stSRULoginRsp.szLoginID, LOGIN_ID_LEN, strLoginID.c_str(), strLoginID.length()))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Parse LoginXML", "Memcpy LoginID failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
    //GET_ELEM_VALUE_CHAR("LoginID",AttriValue,stSRULoginRsp.szLoginID,LOGIN_ID_LEN,xmlRsq);
    GET_ELEM_VALUE_CHAR("SessionID",AttriValue,stSRULoginRsp.szSessionID,SESSION_ID_LEN,xmlRsq);
    GET_ELEM_VALUE_NUM("KeepAliveTime",AttriValue,stSRULoginRsp.iKeepAliveTime,xmlRsq);
    GET_ELEM_VALUE_NUM("KeepAliveCnt",AttriValue,stSRULoginRsp.iKeepAliveCount,xmlRsq);
    GET_ELEM_VALUE_CHAR("TimeZone",AttriValue,stSRULoginRsp.szTimeZone,TIME_ZONE_LEN,xmlRsq);
    GET_ELEM_VALUE_CHAR("Time",AttriValue,stSRULoginRsp.szTime,TIME_LEN,xmlRsq);
    GET_ELEM_VALUE_CHAR("PlayType",AttriValue,stSRULoginRsp.cPlatformVersion,PLATFORM_VERSION_LEN,xmlRsq);
    GET_ELEM_VALUE_NUM("OMUPort",AttriValue,stSRULoginRsp.iOmuPort,xmlRsq);
    GET_ELEM_VALUE_NUM_FOR_UINT("UserID",AttriValue,stSRULoginRsp.uiUserID,xmlRsq);
	return IVS_SUCCEED;
}


//解析单点登录响应xml;
IVS_INT32 CLoginXMLProcess::ParseLoginByTicketXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRspEx)
{
	IVS_DEBUG_TRACE("");
	memset(&stSRULoginRspEx, 0, sizeof(ST_SRU_LOGIN_RSP));

	const char* AttriValue = NULL;   //临时存储单个节点值;
	if (!xmlRsq.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "Node Content not found");
		return IVS_FAIL;
	}
	(void)xmlRsq.IntoElem();
	if (!xmlRsq.FindElem("LoginID"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "Node LoginID not found");
		return IVS_FAIL;
	}
	std::string strLoginID;
	AttriValue = xmlRsq.GetElemValue();
	if (NULL == AttriValue)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "LoginID is NUll");
		return IVS_FAIL;
	}
	if (IVS_SUCCEED != oSecurityClient.DecryptString(AttriValue, strLoginID))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "DecryptString LoginID failed");
		return IVS_FAIL;
	}
	if(!CToolsHelp::Memcpy(stSRULoginRspEx.szLoginID, LOGIN_ID_LEN, strLoginID.c_str(), strLoginID.length()))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Parse LoginXML By Ticket", "Memcpy LoginID failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	GET_ELEM_VALUE_CHAR("SessionID",AttriValue,stSRULoginRspEx.szSessionID,SESSION_ID_LEN,xmlRsq);
	GET_ELEM_VALUE_NUM_FOR_UINT("UserID",AttriValue,stSRULoginRspEx.uiUserID,xmlRsq);
	GET_ELEM_VALUE_NUM("KeepAliveTime",AttriValue,stSRULoginRspEx.iKeepAliveTime,xmlRsq);
	GET_ELEM_VALUE_NUM("KeepAliveCnt",AttriValue,stSRULoginRspEx.iKeepAliveCount,xmlRsq);
	GET_ELEM_VALUE_CHAR("TimeZone",AttriValue,stSRULoginRspEx.szTimeZone,TIME_ZONE_LEN,xmlRsq);
	GET_ELEM_VALUE_CHAR("Time",AttriValue,stSRULoginRspEx.szTime,TIME_LEN,xmlRsq);
	GET_ELEM_VALUE_CHAR("PlayType",AttriValue,stSRULoginRspEx.cPlatformVersion,PLATFORM_VERSION_LEN,xmlRsq);
	GET_ELEM_VALUE_NUM("OMUPort",AttriValue,stSRULoginRspEx.iOmuPort,xmlRsq);
	
	return IVS_SUCCEED;
}


// 解析响应，解密获取LoginID明文
IVS_INT32 CLoginXMLProcess::ParseLoginID(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, std::string &strLoginID)
{
    IVS_DEBUG_TRACE("");
    const char* AttriValue = NULL;   //临时存储单个节点值
    if (!xmlRsq.FindElemEx("Content/LoginID"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse LoginXML", "Node LoginID not found");
        return IVS_XML_INVALID;
    }
    (void)xmlRsq.IntoElem();
    AttriValue = xmlRsq.GetElemValue();
    if (NULL == AttriValue)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse LoginXML", "LoginID is NUll");
        return IVS_XML_INVALID;
    }
    if (IVS_SUCCEED != oSecurityClient.DecryptString(AttriValue, strLoginID))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "DecryptString LoginID failed");
        return IVS_FAIL;
    }
    return IVS_SUCCEED;
}

// 获取用户注销XML
IVS_INT32 CLoginXMLProcess::GetLogoutXML(CXml& xmlReq)
{
    IVS_DEBUG_TRACE("");
    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    return IVS_SUCCEED;
}

// 用户登陆OMU请求XML，重连step1也使用,用户登陆SCU请求XML
IVS_INT32 CLoginXMLProcess::GetLoginOmuXML(const std::string &strLoginID, CXml& xmlReq)
{
    if (strLoginID.empty())
    {
        return IVS_FAIL;
    }
    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    (void)xmlReq.IntoElem();
    (void)xmlReq.AddChildElem("LoginID");
    (void)xmlReq.IntoElem();
    (void)xmlReq.SetElemValue(strLoginID.c_str());
    xmlReq.OutOfElem();
    return IVS_SUCCEED;
}

// 用户登陆无鉴权SMU请求XML
IVS_INT32 CLoginXMLProcess::GetGeneralLoginSmuXML(const char* pLoginID, const std::string &strDomainCode, CXml& xmlReq)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pLoginID, IVS_OPERATE_MEMORY_ERROR);
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("LoginID");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(pLoginID); // TODO 待加密
	(void)xmlReq.AddElem("DomainCode");
	(void)xmlReq.SetElemValue(strDomainCode.c_str());
	xmlReq.OutOfElem();
	return IVS_SUCCEED;
}

// 获取本地与服务器连接可用的IP地址;
std::string CLoginXMLProcess::GetLocalIp( const char* pSerIP, int iSerPort)
{
    //IVS_LOG(LOG_INFO, "Enter %s",__FUNCTION__);

    //已经获取过，直接返回当前值
    char szLocalIp[IVS_IP_LEN] = {0};  //本地IP

    long lSockFd = (long)socket(AF_INET, SOCK_STREAM, 0);
    if (lSockFd < 0)
    {
        //IVS_LOG(LOG_ERROR, "create socket Failed."); 
        return "";
    }

    unsigned long iRet = inet_addr(pSerIP);
    if (INADDR_NONE == iRet)
    {
        //IVS_LOG(LOG_ERROR, "get scc ip Failed."); 
#if VOS_APP_OS == VOS_OS_WIN32
		(void)::closesocket((SOCKET)lSockFd); //modi by zkf71716 at 2012-8-17  
#elif VOS_APP_OS == VOS_OS_LINUX
		close(lSockFd);
#endif
        return "";
    }

    struct sockaddr_in peerAddr;
    memset((char *)&peerAddr, 0, (long)sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_addr.s_addr = (unsigned int)iRet;
    peerAddr.sin_port = htons((u_short)iSerPort);

#if VOS_APP_OS == VOS_OS_WIN32  //begin modi by zkf71716 at 2012-8-17
    unsigned long ul = 1; 
    if(SOCKET_ERROR == ioctlsocket((SOCKET)lSockFd, (long)FIONBIO, (unsigned long*)&ul))
    {
		(void)::closesocket((SOCKET)lSockFd);
        return "";   
    }
#elif VOS_APP_OS == VOS_OS_LINUX
    int flags = fcntl(lSockFd,F_GETFL,0);
    if(0 > flags)
    {
        perror("fcntl F_GETFL");
		close(lSockFd);
        return "";
    }
    if (0 > fcntl(lSockFd, F_SETFL, flags| O_NONBLOCK))
    {
		close(lSockFd);
        return "";
    }

#endif  //end modi by zkf71716 at 2012-8-17


#ifndef WIN32
    errno = 0;
#endif

    (void)::connect((unsigned int)lSockFd, (struct sockaddr*)&peerAddr,
        sizeof(peerAddr));

    struct timeval timeout ;   
    fd_set r;   

    FD_ZERO(&r);   
    FD_SET((unsigned int)lSockFd, &r);   
    timeout.tv_sec = CONNET_TIME_OUT;   // 连接超时8秒   
    timeout.tv_usec =0; 

    // modify by wanglei 00165153 2012.8.22 : select的第一个参数由0改为lSockFd + 1，WIN32下会忽略该参数，但LINUX必须
    int ret = select(lSockFd + 1, 0, &r, 0, &timeout);   

#ifndef WIN32
    // add by wanglei 00165153 2012.8.31
    // select机制在端口错误或IP不可达情况下，WIN32会等待超时后返回错误；而LINUX会马上返回并告知有数据可读，可以通过errno识别错误原因
    if (ECONNREFUSED == errno)
    {
		close(lSockFd);
        return "";
    }
    else if(EHOSTUNREACH == errno)
    {
		close(lSockFd);
        return "";
    }
#endif


    if(ret<=0)   
    {   
#if VOS_APP_OS == VOS_OS_WIN32
        (void)::closesocket((SOCKET)lSockFd); //modi by zkf71716 at 2012-8-17  
#elif VOS_APP_OS == VOS_OS_LINUX
        close(lSockFd);
#endif
        return "";   
    }  

    //重新获取本机地址
    sockaddr_in stAddr = {0};
    socklen_t  nAddrLen =  sizeof(stAddr);
    int nResult = getsockname((unsigned int)lSockFd, (struct sockaddr *)&stAddr, &nAddrLen);   
#if VOS_APP_OS == VOS_OS_WIN32	
    if (SOCKET_ERROR == nResult)
    {
        int nLastError = WSAGetLastError();
		(void)::closesocket((SOCKET)lSockFd);
        //IVS_LOG(LOG_ERR, "GetLocalIp Failed. socket = %ld. ErrorCode = %d.", lSockFd, nLastError); 
        return "";
    }//lint !e529
#elif VOS_APP_OS == VOS_OS_LINUX
    if(-1 == nResult)
    {
        int nLastError = errno;
        IVS_LOG(LOG_ERR, "GetLocalIp Failed. socket = %ld. ErrorCode = %d.", lSockFd, nLastError);
		close(lSockFd);
        return "";
    }
#endif
    char* pszLocalIp = inet_ntoa(stAddr.sin_addr);
    if (NULL == pszLocalIp)
    {
        //IVS_LOG(LOG_ERR, "inet_ntoa Failed On GetLocalIp. socket = %ld.", lSockFd); 
#if VOS_APP_OS == VOS_OS_WIN32//modi by zkf71716 at 2012-8-17
		(void)::closesocket((SOCKET)lSockFd);   
#elif VOS_APP_OS == VOS_OS_LINUX
		close(lSockFd);
#endif
        return "";
    }

    //保存并返回
    //strncpy_s(szLocalIp, pszLocalIp, sizeof(szLocalIp) - 1);
    strncpy(szLocalIp,pszLocalIp,sizeof(szLocalIp)-1);//modi by zkf71716 at 2012-8-17

#if VOS_APP_OS == VOS_OS_WIN32//modi by zkf71716 at 2012-8-17
    (void)::closesocket((SOCKET)lSockFd);   
#elif VOS_APP_OS == VOS_OS_LINUX
    close(lSockFd);
#endif
    return szLocalIp;
}
