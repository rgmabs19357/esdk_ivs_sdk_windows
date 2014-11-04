/********************************************************************
    filename	: 	UserInfo.cpp
    author		:	x00192614
    created		:	2011/11/1
    description	:	用户信息
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/5/20 初始版本
 *********************************************************************/

#ifndef __USER_INFO_H__
#define __USER_INFO_H__

#include "hwsdk.h"
#include "vos.h"   // 保护锁头文件
#include "SDKDef.h"


class CUserInfo
{
public:
	CUserInfo(void);
	virtual ~CUserInfo(void);

public:
	/******************************************************************
	 function   : Init
	 description: 初始化用户信息
	 input      : IVS_INT32 iSdkSessionID
	 input      : const IVS_LOGIN_INFO * pLoginReqInfo
	 input      : const ST_SRU_LOGIN_RSP & stSRULoginRsp
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 Init(IVS_INT32 iSdkSessionID, const IVS_LOGIN_INFO* pLoginReqInfo, const ST_SRU_LOGIN_RSP &stSRULoginRsp);

	/******************************************************************
	 function   : Init
	 description: 初始化用户信息;
	 input      : IVS_INT32 iSdkSessionID
	 input      : const IVS_LOGIN_INFO_EX * pLoginReqInfoEx
	 input      : const IVS_CHAR * pUserName
	 input      : const ST_SRU_LOGIN_RSP & stSRULoginRspEx
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 Init(IVS_INT32 iSdkSessionID, const IVS_CHAR *pUserName, const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, const ST_SRU_LOGIN_RSP &stSRULoginRspEx);

	/******************************************************************
	 function   : GetServerIP
	 description: 获取登录IP
	 output     : NA
	 return     : string
	*******************************************************************/
	virtual std::string GetServerIP() {return m_strIP;}

	/******************************************************************
	 function   : GetSerSessionID
	 description:  获取登录服务器SessionID
	 output     : NA
	 return     : string
	*******************************************************************/
	virtual std::string GetSerSessionID() {return m_stSRULoginRsp.szSessionID;}

	/******************************************************************
	 function   : GetLoginRspInfo
	 description: 获取登录响应信息
	 input      : ST_SRU_LOGIN_RSP & stSRULoginRsp
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetLoginRspInfo(ST_SRU_LOGIN_RSP &stSRULoginRsp) const;

	/******************************************************************
	 function   : GetServerPort
	 description: 获取服务器SMU端口
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetServerPort() const{ return m_iSmuPort; }

	/******************************************************************
	 function   : GetLoginName
	 description: 获取用户名
	 output     : NA
	 return     : string
	*******************************************************************/
	std::string GetLoginName() const { return m_strLoginName; }

	/******************************************************************
	 function   : GetTicketID
	 description: 获取单点登录的TicketID
	 output     : NA
	 return     : string
	*******************************************************************/
	std::string GetTicketID() const { return m_strTicketID; }

	/******************************************************************
	 function   : GetPassword
	 description: 获取用户密码
	 output     : NA
	 return     : string
	*******************************************************************/
	std::string GetPWD() const;

	/******************************************************************
	 function   : GetOrgCode
	 description: 获取组织编号
	 output     : NA
	 return     : string
	*******************************************************************/
	std::string GetOrgCode() const { return m_stSRULoginRsp.szOrgCode; }

	/******************************************************************
	 function   : GetSdkSessionID
	 description: 获取SDK的SessionID
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetSdkSessionID() const { return m_iSdkSessionID; };

    /******************************************************************
     function   : GetClientType
     description: 获取SDK的m_iClientType
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetClientType() const { return m_iClientType; };

    /******************************************************************
     function   : GetUserLocalIp
     description: 获取SDK的m_strLocalIp
     output     : NA
     return     : string
    *******************************************************************/
    std::string GetUserLocalIp() const { return m_strLocalIp; };

	/******************************************************************
	 function   : GetDomainName
	 description: 获取SDK的m_strDomainName
	 output     : NA
	 return     : string
	*******************************************************************/
	std::string GetDomainName() const { return m_strDomainName; };

    /******************************************************************
     function   : GetMachineName
     description: 获取机器名
     output     : NA
     return     : std::string
    *******************************************************************/
    std::string GetMachineName() const { return m_strMachineName; };

    /******************************************************************
     function   : SetUserLocalIp
     description: 设置SDK的m_strLocalIp
     input      : const string & strIp
     output     : NA
     return     : void
    *******************************************************************/
    void SetUserLocalIp(const std::string& strIp) { m_strLocalIp = strIp; };

    /******************************************************************
     function   : GetLoginType
     description: 获取登录类型
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetLoginType() const { return m_loginType; };
    
    /******************************************************************
     function   : GetLoginId
     description: 获取LoginId
     output     : NA
     return     : char*
    *******************************************************************/
    char* GetLoginId();

    /******************************************************************
     function   : SetLoginID
     description: 设置LoginID
     input      : std::string & strLoginID
     output     : NA
     return     : void
    *******************************************************************/
    void SetLoginID(const std::string &strLoginID);
    
    /******************************************************************
     function   : GetUserID
     description: 获取UserID
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 GetUserID() const;

	/******************************************************************
	 function   : Clear
	 description: 重置用户信息
	 output     : NA
	 return     : void
	*******************************************************************/
	void Clear();

private:
	ST_SRU_LOGIN_RSP m_stSRULoginRsp;  // 登录用户响应信息

	std::string    m_strLoginName;           // 登录用户名
	std::string	m_strTicketID;			//单点登录的TicketID;
	std::string    m_strPWD;            // 登录用户密码
	std::string    m_strIP;                  // 登录SMU地址
	IVS_INT32 m_iSmuPort;             // 登录SMU端口
	IVS_INT32 m_loginType;				// 登陆类型 0-基本，1-windows，2-windows其他
	std::string		m_strDomainName;			//域名
	std::string		m_strMachineName;			//机器名
    IVS_INT32 m_iClientType;             //客户端类型
	IVS_INT32 m_iSdkSessionID;         // 本用户SessionID;
    std::string    m_strLocalIp;                      //本机IP，实况业务使用

	VOS_Mutex* m_pConfigMutex;   //保护锁
};

#endif
