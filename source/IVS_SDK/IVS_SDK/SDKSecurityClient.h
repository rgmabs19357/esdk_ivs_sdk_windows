/********************************************************************
 filename    :    SDKSecurityClient.h
 author      :    x00192614
 created     :    2013/01/11
 description :    SDK信息安全客户端
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/11 初始版本
*********************************************************************/
#ifndef __SDKSECURITY_CLIENT_H__
#define __SDKSECURITY_CLIENT_H__

#include "hwsdk.h"
#include "bp_security_client.h"


class CUserMgr;
class CSDKSecurityClient
{
public:
    CSDKSecurityClient(void);
    ~CSDKSecurityClient(void);
    
    /******************************************************************
     function   : Init
     description: 初始化，完成信息安全客户端对象初始化和密钥交换
     input      : int iNetElemType 网元类型
     input      : std::string & strLinkID 连接标识
     output     : NA
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_INT32 Init(int iNetElemType, const std::string& strLinkID , 
        const char * pszCameraCode = "", 
        const char * pszDomainCode = "", 
        const char * pszDestModule = "",
        const char * pszDeviceCode = "",
		const char * pszNVRCode = "",
		enum BP::DOMAIN_TYPE enDomainType = BP::DOMAIN_NONE);

    /******************************************************************
     function   : EncryptString
     description:  加密数据
     input      : const char * pszInBuff 明文输入
     output      : std::string & strCipher 密文返回
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_INT32 EncryptString(const char* pszInBuff, std::string& strCipher) const;

    /******************************************************************
     function   : DecryptString
     description: 解密数据
     input      : const char * pszInBuff 密文输入
     output      : std::string & strPlain 明文返回
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 DecryptString(const char* pszInBuff, std::string& strPlain) const;

    /******************************************************************
     function   : GetSecurityDataTransID
     description: 获取密钥交换后可用的事务ID
     return     : const std::string& 事务ID
    *******************************************************************/
    const std::string& GetSecurityDataTransID() const;

    /******************************************************************
     function   : MultiDomainExchangeKey
     description: 多级多域下的密钥交换
     input      : int iNetElemType
     input      : CUserMgr * pUserMgr
     input      : std::string & strDomainCode
     output     : std::string& strLinkID
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 MultiDomainExchangeKey(int iNetElemType, CUserMgr* pUserMgr, const std::string& strDomainCode, 
        std::string& strLinkID, const char* pszDestModule = "", const char* pszDeviceCode = "", const char* pszNVRCode = "", enum BP::DOMAIN_TYPE enDomainType = BP::DOMAIN_NONE);
private:
    BP::CSecurityClient m_oSecurityClient; // 信息安全客户端对象
};
#endif


