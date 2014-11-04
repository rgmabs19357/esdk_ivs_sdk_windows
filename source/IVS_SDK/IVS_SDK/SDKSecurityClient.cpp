/********************************************************************
 filename    :    SDKSecurityClient.cpp
 author      :    x00192614
 created     :    2013/01/11
 description :    SDK信息安全客户端
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/11 初始版本
*********************************************************************/
#include "SDKSecurityClient.h"
#include "NSSOperator.h"
#include "IVSCommon.h"
#include "SDKDef.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
CSDKSecurityClient::CSDKSecurityClient(void)
{
}

CSDKSecurityClient::~CSDKSecurityClient(void)
{
}

// 与制定的连接密钥交换
IVS_INT32 CSDKSecurityClient::Init(int iNetElemType, const std::string& strLinkID, 
    const char* pszCameraCode, 
    const char* pszDomainCode, 
    const char* pszDestModule,
    const char* pszDeviceCode,
	const char* pszNVRCode,
	enum BP::DOMAIN_TYPE enDomainType)
{
    CHECK_POINTER(pszCameraCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pszDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pszDestModule, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pszDeviceCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pszNVRCode, IVS_OPERATE_MEMORY_ERROR);
    IVS_INFO_TRACE("iNetElemType:%d, strLinkID:%s", iNetElemType, strLinkID.c_str());

    IVS_INT32 iRet = IVS_FAIL;
    // 初始化m_oSecurityClient
	iRet = m_oSecurityClient.RequestExchangeKey(SERVICE_COMMU, iNetElemType, strLinkID.c_str(), pszCameraCode, pszDomainCode, pszDestModule, pszDeviceCode, pszNVRCode, enDomainType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Security Client Init", "Request ExchangeKey failed");
        return iRet;
    }
    return IVS_SUCCEED;
}

// 使用密钥交换后的CSecurityClient对象加密数据
IVS_INT32 CSDKSecurityClient::EncryptString(const char* pszInBuff, std::string& strCipher) const
{
    IVS_INT32 iEncryptedLen = CSecurityClient::GetStringEncryptedBufferLen(pszInBuff);
    IVS_CHAR* pCipherTemp = IVS_NEW(pCipherTemp, (IVS_UINT32)iEncryptedLen);
    memset(pCipherTemp, 0, (IVS_UINT32)iEncryptedLen);
    bool bEncrypt = m_oSecurityClient.EncryptString(pszInBuff, pCipherTemp, iEncryptedLen);
    if (!bEncrypt)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Encrypt String", "EncryptString failed");
        IVS_DELETE(pCipherTemp, MUILI);
        return IVS_FAIL;
    }
    strCipher = pCipherTemp;
    IVS_DELETE(pCipherTemp, MUILI);
    return IVS_SUCCEED;
}

// 使用密钥交换后的CSecurityClient对象解密数据
IVS_INT32 CSDKSecurityClient::DecryptString(const char* pszInBuff, std::string& strPlain) const
{
    // 明文长度
    IVS_INT32 iDecryptedLen = CSecurityClient::GetStringDecryptedBufferLen(pszInBuff);
    IVS_CHAR* pPlainTemp = IVS_NEW(pPlainTemp, (IVS_UINT32)iDecryptedLen);
    memset(pPlainTemp, 0, (IVS_UINT32)(iDecryptedLen));
    bool bDecryptRet = m_oSecurityClient.DecryptString(pszInBuff, pPlainTemp, iDecryptedLen);
    if(!bDecryptRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Encrypt String", "DecryptString fail");
        IVS_DELETE(pPlainTemp, MUILI);
        return IVS_FAIL;
    }
    strPlain = pPlainTemp;
    IVS_DELETE(pPlainTemp, MUILI);
    return IVS_SUCCEED;
}

// 获取可传输加密数据的事务ID
const std::string& CSDKSecurityClient::GetSecurityDataTransID() const
{
    return m_oSecurityClient.GetSecurityDataTransID();
}

// 多域下的密钥交换
IVS_INT32 CSDKSecurityClient::MultiDomainExchangeKey(int iNetElemType, CUserMgr* pUserMgr, const std::string& strDomainCode, 
    std::string& strLinkID, const char* pszDestModule, const char* pszDeviceCode, const char* pszNVRCode, enum BP::DOMAIN_TYPE enDomainType)
{
    CHECK_POINTER(pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pszDeviceCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pszDestModule, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pszNVRCode, IVS_OPERATE_MEMORY_ERROR);
    IVS_INFO_TRACE("iNetElemType:%d, strDomainCode:%s", iNetElemType, strDomainCode.c_str());
    // 本域编码
    std::string strLocalDomainCode;
    int iSessionID = pUserMgr->GetSessionID();
    pUserMgr->GetDomainCode(strLocalDomainCode);
    CLinkInfo oLinkIdInfo;
    //根据域编码查找目标域和代理域映射表，查找代理域
    std::string strSendDomainCode;
    bool bRet = pUserMgr->GetDomainRouteMgr().FindProxyDomainCode(strDomainCode, strSendDomainCode);
    // 如果未找到代理域，使用目标域代替;
    if (!bRet)
    {
        strSendDomainCode = strDomainCode;
    }

    // 域编码为空，或者目标域为本域时，未找到连接，返回失败（本域不重新登录）
    if (strDomainCode.empty() || 0 == strSendDomainCode.compare(strLocalDomainCode))
    {
        bool bGetSmuLinkRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(iSessionID, strDomainCode, oLinkIdInfo);
        if (!bGetSmuLinkRet)
        {
            BP_RUN_LOG_ERR(IVS_FAIL,"Multi Domain ExchangeKey","GetSmuLinkIDInfo By DomainCode failed");
            return IVS_FAIL;
        }
        // 连接是断开状态，直接返回连接已断开，没有必要继续发送业务请求
        if (LINKID_STATUS_OPENED != oLinkIdInfo.GetLinkStatus())
        {
            BP_RUN_LOG_ERR(IVS_NET_CONNECT_ERROR,"Multi Domain ExchangeKey","Link is Closed");
            return IVS_NET_CONNECT_ERROR;
        }
        if (IVS_SUCCEED != Init(iNetElemType, oLinkIdInfo.GetStrLinkId(),"","",pszDestModule, pszDeviceCode, pszNVRCode, enDomainType))
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "Security Client Init", "Request ExchangeKey failed");
			// modify by limingjie kf175483:2013.7.15 
			// 密钥交换出错，返回相应错误码
			return IVS_SDK_EXCHANGE_KEY_ERR_GET_KEY_FAIL;
            //return IVS_FAIL;
        }
        strLinkID = oLinkIdInfo.GetStrLinkId();
        return IVS_SUCCEED;
    }
   
    // 根据域编码查找LinkRouteMgr找SMU的LinkID
    bool bGetLinkRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(iSessionID, strSendDomainCode, oLinkIdInfo);
    
    // 若未找到，登录并获取LinkID
    if (!bGetLinkRet)
    {
        BP_RUN_LOG_ERR(bGetLinkRet, "send cmd,", "domain code:[%s] is not exist in route list cache.", strSendDomainCode.c_str());
    }
        //int iCreateLinkRet = pUserMgr->GetLoginMgr().GeneralLoginSmu(strSendDomainCode, strLinkID);
        //// BP建立可能返回-1的错误码
        //if (IVS_SUCCEED != iCreateLinkRet)
        //{
        //    BP_RUN_LOG_ERR(iCreateLinkRet,"Multi Domain ExchangeKey","Login Smu failed");

        //    IVS_DOMAIN_ROUTE stDomainRouteInfo;
        //    memset(&stDomainRouteInfo, 0, sizeof(IVS_DOMAIN_ROUTE));
        //    if (IVS_SUCCEED == pUserMgr->GetDomainRouteMgr().GetDomainRoutebyCode(strDomainCode, stDomainRouteInfo))
        //    {
        //        (void)pUserMgr->AddLinkIdToBusinessResumeList(strLinkID,NET_ELE_SMU_NSS,stDomainRouteInfo.stIP.cIP,DEFAULT_SMU_PORT,TYPE_ADD_RELINK);
        //    }
        //    return iCreateLinkRet;
        //}
    //}
    // 找到，直接使用LinkID进行密钥交换
    //else
    //{
    //    strLinkID = oLinkIdInfo.GetStrLinkId();
    //}

    IVS_INT32 iRet = IVS_FAIL;
    if (LINKID_STATUS_ENABLED == oLinkIdInfo.GetLinkStatus())
    {
        iRet = pUserMgr->ProcrssEnableLinkNode(oLinkIdInfo);
    }

    if (LINKID_STATUS_OPENED != oLinkIdInfo.GetLinkStatus())
    {
        return iRet;
    }

    strLinkID = oLinkIdInfo.GetStrLinkId();

    if (IVS_SUCCEED != Init(iNetElemType,strLinkID,"","",pszDestModule, pszDeviceCode, pszNVRCode, enDomainType))
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Multi Domain ExchangeKey","Send Cmd Error");
		// modify by limingjie kf175483:2013.6.27 
		// 密钥交换出错，返回相应错误码
        return IVS_SDK_EXCHANGE_KEY_ERR_GET_KEY_FAIL;
		// return IVS_FAIL
    }
    return IVS_SUCCEED;
}
