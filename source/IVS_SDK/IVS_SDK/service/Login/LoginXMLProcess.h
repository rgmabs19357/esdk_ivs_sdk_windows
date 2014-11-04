/********************************************************************
filename : LoginXMLProcess.h
author : x00192614
created : 2012/12/03
description : 登录xml拼装解析
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/

#ifndef __LOGIN_XML_PROCESS_H__
#define __LOGIN_XML_PROCESS_H__

#include "SDKDef.h"

class CUserMgr;
class CUserInfo;
class CSDKSecurityClient;
class CLoginXMLProcess
{
private:
    CLoginXMLProcess();
public:
    ~CLoginXMLProcess();

public:
    /******************************************************************
     function   : GetLoginXML
     description: 获取登录本域XML
     input      : const IVS_LOGIN_INFO * pLoginReqInfo 输入参数结构体
     input      : const std::string & strLoginName 用户名
     input      : const std::string & strLoginPWD 密码
     input      : CUserMgr * pUserObj 用户指针
     output      : CXml & xmlReq 请求xml类
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginXML(const IVS_LOGIN_INFO* pLoginReqInfo, const std::string& strLoginName, const std::string& strLoginPWD, CUserMgr *pUserObj, CXml& xmlReq );

	/******************************************************************
     function   : GetLoginByTicketXML
     description: 获取单点登录本域XML
     input      : const IVS_LOGIN_INFO_EX * pLoginReqInfoEx 输入参数结构体
     input      : CUserMgr * pUserObj 用户指针
     output     : CXml & xmlReq 请求xml类;
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginByTicketXML(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, CUserMgr *pUserObj, CXml& xmlReq );

    /******************************************************************
     function   : GetReLoginXML
     description: 获取重登陆XML
     input      : const std::string & strLoginName 用户名
     input      : const std::string & strLoginPWD 密码
     input      : CUserInfo * pUserInfo 用户信息指针
     output      : CXml & xmlReq 请求XML
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetReLoginXML(const std::string& strLoginName, const std::string& strLoginPWD, CUserInfo *pUserInfo, CXml& reqXml );
	
	/******************************************************************
	 function   : GetFineMainDevXML
	 description: 获取摄像机定位XML
	 input      : const string & strDevCode 摄像机编码
	 output      : CXml & xmlReq 请求xml类
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetFineMainDevXML(const std::string& strDevCode, CXml& xmlReq );

	/******************************************************************
	 function   : ParseNvrCodeXML
	 description: 解析NvrCode信息
	 input      : CXml & xmlRsq 响应xml类
	 output      : char * pNvrCode NVR编码
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 ParseNvrCodeXML(CXml& xmlRsq, char* pNvrCode);

    /******************************************************************
     function   : ParseLoginXML
     description: 解析登录响应XML
     input      : CXml & xmlRsq 登录响应xml类
     input      : const CSDKSecurityClient & oSecurityClient 密钥交换类
     output      : ST_SRU_LOGIN_RSP & stSRULoginRsp 保存响应信息类
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseLoginXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRsp);


	/******************************************************************
     function   : ParseLoginByTicketXML
     description: 解析单点登录响应XML
     input      : CXml & xmlRsq 登录响应xml类;
	 input      : const CSDKSecurityClient & oSecurityClient 密钥交换类;
     output     : ST_SRU_LOGIN_RSP & stSRULoginRspEx 保存响应信息类;
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseLoginByTicketXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRspEx);


    /******************************************************************
     function   : ParseLoginID
     description: 解析重登录响应，获取LoginID
     input      : CXml & xmlRsq
     input      : CSDKSecurityClient & oSecurityClient
     input      : std::string & strLoginID
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseLoginID(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, std::string &strLoginID);

    /******************************************************************
     function   : GetLoginOmuXML
     description: 用户登陆OMU请求XML
     input      : const string & pLoginID 登录本域SMU成功返回
     output     : CXml & xmlReq 请求xml类
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginOmuXML(const std::string &pLoginID, CXml& xmlReq);

    /******************************************************************
     function   : GetLogoutXML
     description: 用户注销XML
     input      : CXml & xmlReq 请求xml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLogoutXML(CXml& xmlReq);

    /******************************************************************
     function   : GetGeneralLoginSmuXML
     description: 用户登陆无鉴权SMU请求XML
     input      : const char * pLoginID 登录成功返回的标识
     input      : const std::string & strDomainCode 域编码
     output      : CXml & xmlReq 请求xml类
     return     : IVS_INT32
	*******************************************************************/
    static IVS_INT32 GetGeneralLoginSmuXML(const char* pLoginID, const std::string &strDomainCode, CXml& xmlReq);
   
    /******************************************************************
     function   : ParseUserOffLineXML
     description: 用户下线通知
     input      : CXml & xmlRsq 下线通知xml
     input      : IVS_CHAR * pLoginID 一次登录的标识
     output      : IVS_USER_OFFLINE_INFO * stOffLineInfo 离线通知结构体
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseUserOffLineXML(CXml& xmlRsq, const IVS_CHAR* /*pLoginID*/, IVS_USER_OFFLINE_INFO *stOffLineInfo);

private:

	/******************************************************************
	 function   : GetLocalIp
	 description: 获取本地IP地址
	 input      : const char * pSerIP 服务端IP
	 input      : int iSerPort 服务端PORT
	 output     : NA
	 return     : string 本机IP
	*******************************************************************/
	static std::string GetLocalIp(const char* pSerIP, int iSerPort);
};
#endif

