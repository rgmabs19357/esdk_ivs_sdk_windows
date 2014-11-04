/********************************************************************
    filename	: 	UserInfo.h
    author		:	x00192614
    created		:	2011/11/1
    description	:	用户信息
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/5/20 初始版本
 *********************************************************************/

#include "UserInfo.h"
#include "LockGuard.h"
#include "bp_log.h"
#include "bp_def.h"
#include "ivs_error.h"
#include "IVS_Trace.h"
#include "ToolsHelp.h"
#include "Aes256Operator.h"

CUserInfo::CUserInfo(void)
    :m_iSmuPort(SMU_SERVER_DEF_PORT),
    m_iClientType(CLIENT_PC),
	m_iSdkSessionID(SDK_SESSIONID_INIT),    
    m_strLocalIp("")
{
	memset(&m_stSRULoginRsp, 0, sizeof(m_stSRULoginRsp));
	m_pConfigMutex = VOS_CreateMutex();    //创建保护锁

    m_loginType = 0;
}


CUserInfo::~CUserInfo(void)
{
    try
    {
        Clear();
    }
    catch (...)
    {
    }

	//删除保护锁
	if(NULL != m_pConfigMutex)
	{
		(void)VOS_DestroyMutex(m_pConfigMutex);
		m_pConfigMutex = NULL;
	}
}

// 初始化用户信息
IVS_INT32 CUserInfo::Init(IVS_INT32 iSdkSessionID, const IVS_LOGIN_INFO* pLoginReqInfo, const ST_SRU_LOGIN_RSP &stSRULoginRsp )
{
	IVS_DEBUG_TRACE("");
    CHECK_POINTER(pLoginReqInfo, IVS_FAIL);
	m_iSdkSessionID   = iSdkSessionID;
	m_strLoginName    = pLoginReqInfo->cUserName;
    // 密文128位够用，此处保存密码密文
    char cPWDtemp[IVS_PWD_LEN + 1] = {0};
    (void)CToolsHelp::Memcpy(cPWDtemp, IVS_PWD_LEN, pLoginReqInfo->pPWD,IVS_PWD_LEN);
    char cPWDCipher[128] = {0};
    (void)Aes256Operator::instance().Aes256CbcEncode(cPWDtemp, cPWDCipher, (IVS_UINT64)128);
	memset(cPWDtemp,0,IVS_PWD_LEN + 1);
	m_strPWD = cPWDCipher;
	m_strIP = pLoginReqInfo->stIP.cIP;
	m_iSmuPort      = static_cast<int>(pLoginReqInfo->uiPort);
	m_loginType = static_cast<int>(pLoginReqInfo->uiLoginType);
	m_strDomainName = pLoginReqInfo->cDomainName;
	m_strMachineName = pLoginReqInfo->cMachineName;
    m_iClientType = static_cast<int>(pLoginReqInfo->uiClientType);
    if (!CToolsHelp::Memcpy(&m_stSRULoginRsp, sizeof(ST_SRU_LOGIN_RSP), &stSRULoginRsp, sizeof(ST_SRU_LOGIN_RSP)))
    {
        return IVS_FAIL;
    }
	//memcpy(&m_stSRULoginRsp, &stSRULoginRsp, sizeof(ST_SRU_LOGIN_RSP));
	return IVS_SUCCEED;
}


//初始化用户信息;
IVS_INT32 CUserInfo::Init(IVS_INT32 iSdkSessionID, const IVS_CHAR *pUserName, const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, const ST_SRU_LOGIN_RSP &stSRULoginRspEx)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pLoginReqInfoEx, IVS_FAIL);

	m_iSdkSessionID   = iSdkSessionID;
	m_strTicketID	  = pLoginReqInfoEx->cTicketID;
	m_strLoginName    = pUserName;

	m_strIP = pLoginReqInfoEx->stIP.cIP;
	m_iSmuPort      = static_cast<int>(pLoginReqInfoEx->uiPort);
	m_loginType = static_cast<int>(pLoginReqInfoEx->uiLoginType);
	m_strDomainName = pLoginReqInfoEx->cDomainName;
	m_strMachineName = pLoginReqInfoEx->cMachineName;
	m_iClientType = static_cast<int>(pLoginReqInfoEx->uiClientType);

	if (!CToolsHelp::Memcpy(&m_stSRULoginRsp, sizeof(ST_SRU_LOGIN_RSP), &stSRULoginRspEx, sizeof(ST_SRU_LOGIN_RSP)))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "CToolsHelp::Memcpy failed", "NA");
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}

// 获取登录响应信息
IVS_INT32 CUserInfo::GetLoginRspInfo( ST_SRU_LOGIN_RSP &stSRULoginRsp )const
{
    IVS_INT32 iRet = IVS_FAIL;
    if (CToolsHelp::Memcpy(&stSRULoginRsp, sizeof(ST_SRU_LOGIN_RSP), &m_stSRULoginRsp, sizeof(ST_SRU_LOGIN_RSP)))
    {
        iRet = IVS_SUCCEED;
    }
	//memcpy(&stSRULoginRsp, &m_stSRULoginRsp, sizeof(ST_SRU_LOGIN_RSP));
	return iRet;
}

// 获取LoginId
char* CUserInfo::GetLoginId()
{
    return m_stSRULoginRsp.szLoginID;//lint !e1536
}

// 设置LoginID
void CUserInfo::SetLoginID(const std::string &strLoginID)
{
    (void)CToolsHelp::Memcpy(m_stSRULoginRsp.szLoginID, LOGIN_ID_LEN, strLoginID.c_str(),strlen(strLoginID.c_str()));
}

// 获取UserID
IVS_UINT32 CUserInfo::GetUserID() const
{
    return m_stSRULoginRsp.uiUserID;//lint !e1536
}

std::string CUserInfo::GetPWD() const
{
    std::string strPlainPWD;
    // 明文128位够用，解密密码，返回明文
    char cPWDtemp[128] = {0};
    //解密
    (void)Aes256Operator::instance().Aes256CbcDecode(m_strPWD.c_str(),cPWDtemp, (IVS_UINT64)128);
    strPlainPWD = cPWDtemp;
    return strPlainPWD; //lint !e1036 !e1536
}

// 重置用户信息
void CUserInfo::Clear()
{
    memset(&m_stSRULoginRsp, 0, sizeof(m_stSRULoginRsp));
	m_iSdkSessionID = SDK_SESSIONID_INIT;
	m_strLoginName  = "";         
    m_strPWD   = "";        
	m_iSmuPort    = SMU_SERVER_DEF_PORT;  
    m_strLocalIp = "";
    m_strIP = "";
    m_strMachineName = "";
}

