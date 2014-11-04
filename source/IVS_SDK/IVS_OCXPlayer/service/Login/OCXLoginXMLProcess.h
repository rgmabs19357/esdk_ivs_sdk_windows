/********************************************************************
filename    :    OCXLoginXMLProcess.h
author      :    x00192614
created     :    2012/12/3
description :    ivs 错误码定义
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/12/3 初始版本
*********************************************************************/
#ifndef __OCX_LOGIN_XML_PROCESS_H__
#define __OCX_LOGIN_XML_PROCESS_H__
#include "OCXXmlProcess.h"

class COCXLoginXMLProcess
{
private:
    COCXLoginXMLProcess();
public:
    ~COCXLoginXMLProcess();

    /******************************************************************
     function   : GetLoginReqInfo
     description: 根据登陆请求XML，获取登陆信息结构体
     input      : LPCTSTR pUserName             用户名
     input      : LPCTSTR pPassward             密码
     input      : LPCTSTR pServerIP             服务端IP
     input      : LPCTSTR pPort                 服务端PORT
     input      : LONG uiClientType             客户端类型
     output      : IVS_LOGIN_INFO & pLoginInfo   入参结构体
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginReqInfo(LPCTSTR pUserName, LPCTSTR pPWD, LPCTSTR pServerIP,LPCTSTR pPort, ULONG ulClientType, IVS_LOGIN_INFO &pLoginInfo);


	/******************************************************************
     function   : GetLoginByTicketReqInfo
     description: 根据单点登陆请求XML，获取单点登陆信息结构体
     input      : LPCTSTR pTicketID             TicketID
     input      : LPCTSTR pServerIP             服务端IP
     input      : LPCTSTR pPort                 服务端PORT
     input      : LONG uiClientType             客户端类型
     output      : IVS_LOGIN_INFO_EX & pLoginInfoEx   入参结构体
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginByTicketReqInfo(LPCTSTR pTicketID, 
		LPCTSTR pServerIP,
		LPCTSTR pPort, 
		ULONG ulClientType, 
		IVS_LOGIN_INFO_EX &pLoginInfoEx);


    /******************************************************************
     function   : UserOfflineNotifyGetXML
     description: 用户下线通知构造xml
     input      : const IVS_USER_OFFLINE_INFO * pOfflineNotify 下线通知结构体
     output      : CXml & xml 下线通知xml
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 UserOfflineNotifyGetXML(const IVS_USER_OFFLINE_INFO* pOfflineNotify, CXml &xml);    
    
    /******************************************************************
     function   : GetUserIDRspXML
     description: 拼装GetUserID响应消息
     input      : IVS_INT32 iRet 获取UserID返回码
     input      : IVS_UINT32 uiUserID 用户ID
     output      : CXml & xml 响应xml
     return     : void
    *******************************************************************/
    static void GetUserIDRspXML(IVS_INT32 iRet, IVS_UINT32 uiUserID, CXml &xml);
};

#endif // __OCX_LOGIN_XML_PROCESS_H__
