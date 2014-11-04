/********************************************************************
    filename	: 	LoginMgr.h
    author		:	x00192614
    created		:	2011/11/1
    description	:	用户登录管理类
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/5/20 初始版本
 *********************************************************************/

#ifndef __LOGIN_MNG_H__
#define __LOGIN_MNG_H__

#include "ivs_error.h"
#include "hwsdk.h"
#include "UserInfo.h"
#include "bp_def.h"
// #include <map>
#include "SDKDef.h"
#include "bp_security_client.h"

class CUserMgr;

class CLoginMgr	
{
public:
	CLoginMgr(void);
	~CLoginMgr(void);

    /******************************************************************
     function   : SetUserMgr
     description: 设置登录模块的用户指针
     input      : CUserMgr * pUserMgr 用户指针
     output     : NA
     return     : void
    *******************************************************************/
    void SetUserMgr(CUserMgr *pUserMgr);

    /**************************************************************************
    * name       : Login
    * description: 用户登录
    * input      : IVS_LOGIN_INFO 用户登录信息
    * output     : NA
    * return     : 发送消息成功返回SMU返回码，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_INT32 Login(const IVS_LOGIN_INFO* pLoginReqInfo);

	/**************************************************************************
    * name       : LoginByTicket
    * description: 用户单点登录;
    * input      : IVS_LOGIN_INFO_EX 用户单点登录信息;
    * output     : NA
    * return     : 发送消息成功返回SMU返回码，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_INT32 LoginByTicket(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx);

	/******************************************************************
	 function   : GetInitData
	 description:  获取域路由，NVRList和摄像机列表
	 output     : NA
	 return     : void
	*******************************************************************/
	void GetInitData();

	/******************************************************************
	 function   : GetLoginRspInfo
	 description: 获取登录响应信息
	 input      : LOGIN_RSP_INFO * pLoginRspInfo SDK保存的登录响应信息
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	//IVS_INT32 GetLoginRspInfo(LOGIN_RSP_INFO* pLoginRspInfo);

	/******************************************************************
	 function   : Logout
	 description: 用户注销
	 input      : bool bSendLogoutCmd 是否发送注销请求，默认为true
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 Logout(bool bSendLogoutCmd = true);

	/******************************************************************
	 function   : GetUserInfo
	 description: 获取用户信息
	 output     : NA
	 return     : CUserInfo* 保存用户信息的指针
	*******************************************************************/
	CUserInfo* GetUserInfo();

    /******************************************************************
     function   : GetLoginId
     description: 获取用户登录成功返回LoginID
     output     : NA
     return     : char* 登录成功返回的LoginID
    *******************************************************************/
    char* GetLoginId();

    /******************************************************************
     function   : GetUserID
     description:  获取用户登录成功返回UserID
     output     : NA
     return     : IVS_UINT32 登录成功返回UserID
    *******************************************************************/
    IVS_UINT32 GetUserID() const;

	/******************************************************************
     function   : GetLoginType
     description:  获取用户的登录类型，单点登录或普通登录
     output     : NA
     return     : IVS_LOGIN_TYPE_E 登录类型
    *******************************************************************/
    IVS_LOGIN_TYPE_E GetLoginType() const;

    /******************************************************************
     function   : LoginOmuThreadEntry
     description:  登陆Omu线程入口
     input      : void * pArg
     output     : NA
     return     : long __stdcall
    *******************************************************************/
    static long __SDK_CALL LoginOmuThreadEntry(void* pArg);

    /******************************************************************
     function   : ReLogin
     description: SMU恢复后重新登录到SMU
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 ReLogin(std::string& strLinkID);

    IVS_INT32 ReLoginSMU(const std::string& strIP, int iPort);

    IVS_INT32 ReLoginOmu(const std::string& strIP, int iPort);

public:
    /******************************************************************
     function   : LoginSmu
     description:  用户登录到本域SMU
     input      : const IVS_LOGIN_INFO * pLoginReqInfo 登录请求参数
     output      : ST_SRU_LOGIN_RSP & stSRULoginRsp 登录返回信息
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 LoginSmu(const IVS_LOGIN_INFO* pLoginReqInfo, ST_SRU_LOGIN_RSP &stSRULoginRsp);

	/******************************************************************
     function   : LoginSmuByTicket
     description:  用户单点登录到本域SMU;
     input      : const IVS_LOGIN_INFO_EX * pLoginReqInfoEx 单点登录请求参数;
     output      : ST_SRU_LOGIN_RSP & stSRULoginRspEx 单点登录返回信息;
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 LoginSmuByTicket(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, ST_SRU_LOGIN_RSP &stSRULoginRspEx);

    /******************************************************************
     function   : LoginScu
     description: 用户登录指定的SCU
     input      : const string & strDomainCode 域编码
     input      : const string & strNVRCode NVR编码
     input      : string & strLinkID 与SCU的连接标识
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_INT32 LoginScu(const std::string &strDomainCode,const std::string &strNVRCode, std::string &strLinkID);

    /******************************************************************
     function   : GeneralLoginSmu
     description: 用户登陆到SMU
     input      : const std::string & strDomainCode 域编码
     input      : std::string & strLinkID 与SMU的连接标识
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_INT32 GeneralLoginSmu(const std::string &strDomainCode, std::string &strLinkID);

    /******************************************************************
     function   : LoginOmu
     description: 登陆OMU
     output     : NA
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_INT32 LoginOmu();

    /******************************************************************
     function   : LogoutSmu
     description: 用户注销
     input      : const std::string & sSmuLinkId 本域SMU的连接标识
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_INT32 LogoutSmu(const std::string& sSmuLinkId);

private:
    /******************************************************************
     function   : SendGeneralAuthCmd
     description: 发送通用业务认证消息
     input      : int iNetElementType
     input      : const std::string & strLinkId 连接标识
     input      : CXml & xml 请求xml
     input      : const std::string & strTransID 事务ID
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_INT32 SendGeneralAuthCmd(int iNetElementType, const std::string& strLinkId, CXml& xml, const std::string& strTransID) const;

    /******************************************************************
     function   : SendReConnectCmd
     description: 发送重连SMU请求（含密钥交换，loginID解密）
     input      : const std::string & strLinkID
     output      : std::string & strLoginID
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SendReConnectCmd(const std::string &strLinkID, std::string &strLoginID);

    /******************************************************************
     function   : SendLoginCmd
     description: 重新发送登录请求
     input      : const std::string & strLinkID
     output     : std::string & strLoginID
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SendLoginCmd(const std::string &strLinkID, std::string &strLoginID);

    /******************************************************************
     function   : RefreshLoginInfo
     description: 重连成功后，更新loginID和LinkID
     input      : const std::string strLoginID 重新获取的LoginID
     input      : int iSessionID
     input      : const std::string strLocalDomainCode 
     input      : const string strLinkID 重新登录成功的LinkID
     output     : NA
     return     : void
    *******************************************************************/
    void RefreshLoginInfo(const std::string strLoginID, int iSessionID, const std::string strLocalDomainCode, const std::string strLinkID );

    IVS_INT32 SendReLoginOMUCmd(const std::string& strLinkID);

public:
    bool m_bLoginOmuThreadRun; //登陆OMU的线程在运行

private:

	CUserInfo  m_userInfo;   // 用户信息
    VOS_Thread*  m_pLoginOMUThread; // 登录OMU线程指
    CUserMgr   *m_pUserMgr; // 用户指针
	IVS_LOGIN_TYPE_E m_enLoginType;	//用户登录类型，0 正常登录;1 单点登录;
};

#endif
