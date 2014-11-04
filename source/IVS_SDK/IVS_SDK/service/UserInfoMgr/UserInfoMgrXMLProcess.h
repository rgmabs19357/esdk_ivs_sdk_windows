/********************************************************************
filename : UserInfoMgrXMLProcess.h
author : pkf57481
created : 2012/12/03
description : 用户管理xml处理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/


#ifndef __USERINFO_MGR_XML_PROCESS_H__
#define __USERINFO_MGR_XML_PROCESS_H__

#include "ToolsHelp.h"
#include "XmlProcess.h"
#include <string.h>
#include "QueryFiled.h"
#include "hwsdk.h"

class CSDKSecurityClient;
class CUserInfoMgrXMLProcess
{
private:
    CUserInfoMgrXMLProcess();
public:
    ~CUserInfoMgrXMLProcess();
public:

    static IVS_VOID GetDeleteUserXML(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, CXml& xml);

    static IVS_VOID GetAddModUserInfoXML(const IVS_CHAR* pDomainCode, const IVS_USER_INFO* pUserInfo, const CSDKSecurityClient& oSecurityClient, CXml &xml);

    static IVS_INT32 AddUserParseXML(CXml &xml, IVS_UINT32* uiUserID);

    static IVS_INT32 GetUserListParseXML(CXml &ReqXml, const CSDKSecurityClient& oSecurityClient, IVS_USER_INFO_LIST* pUserInfoList);

    static IVS_INT32 GetOnlineUserListParseXML(CXml &ReqXml, IVS_ONLINE_USER_LIST* pOlUserList);

    static IVS_VOID GetUserInfoGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, CXml &xml);

    static IVS_INT32 GetUserInfoParseXML(CXml &xml, const CSDKSecurityClient& oSecurityClient, IVS_USER_INFO* pUserInfo);

    static IVS_VOID LockUserGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_BOOL bLock, CXml &xml);

    static IVS_VOID ChangePWDGetXML(const IVS_CHAR* cLoginID, const IVS_CHAR* pOldPWD, const IVS_CHAR* pNewPWD, CXml &xml);

    static IVS_VOID ResetPWDGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_CHAR* pNewPWD, CXml &xml);

    static IVS_VOID CheckPWDByRoleGetXML(const IVS_CHAR* pCode, IVS_UINT32 uiID, const IVS_CHAR* pLoginName, const IVS_CHAR* pPWD, CXml &xml);

    static IVS_VOID SignOutUserGetXML(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, CXml &xml);

    static IVS_VOID SignOutOnlineUserGetXML(const IVS_CHAR* pSmuDomainCode, const IVS_UINT32 uiUserOnlineID, CXml &xml);

    static IVS_VOID SetUserDevListGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pUserDomainCode, IVS_UINT32 uiUserID, const std::list<IVS_USER_DEVICE_INFO *> &pUserDevList, CXml &xml);

    static IVS_VOID GetUserDeviceListGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_INDEX_RANGE* pIndexRange, CXml &xml);

    static IVS_INT32 UserDeviceListParseXML(CXml& xml, IVS_USER_DEVICE_INFO_LIST* pUserDevList);

    static IVS_VOID GetRoleXML(const IVS_CHAR* pDomainCode, const IVS_ROLE_INFO* pRoleInfo, CXml &xml);

    static IVS_INT32 AddRoleParseXML(CXml &xml, IVS_UINT32* &uiRoleID);

    static IVS_VOID GetUserGroupXML(const IVS_CHAR* pDomainCode, const IVS_USER_GROUP* pUserGroup, CXml &xml);

    static IVS_INT32 GetDeleteUserGroupXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserGroupID, CXml &xml);

    static IVS_INT32 GetRoleListParseXML(CXml &xml, IVS_ROLE_INFO_LIST* pRoleList);

    static IVS_INT32 AddUserGroupParseXML(CXml& xml, IVS_UINT32* puiUserGroupID);

    static IVS_INT32 MoveUserGroupGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiTargetDevGroupID, const IVS_USER_GROUP_LIST* pDeviceGroupList, CXml& xml);

    static IVS_VOID GetUserGroupListGetXML(const IVS_CHAR * pDomainCode, const IVS_INDEX_RANGE* pIndexRange, CXml &xml);

    static IVS_INT32 GetUserGroupListParseXML(CXml &Rspxml, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_GROUP_LIST* pUserGroupList);

    static IVS_VOID GetRoleInfoByRoleIDGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, CXml &xml);

    static IVS_INT32 GetRoleInfoParseXML(CXml &xml, IVS_ROLE_INFO* pRoleInfo);

    static IVS_INT32 GetAutoCreatePWDParseXML(CXml &xml, IVS_CHAR* pPWD);
    
    //static IVS_INT32 GetRoleInfoByUidParseXML(CXml &xml, IVS_ROLE_INFO* pRoleInfo);

    static IVS_VOID GetAutoCreatePWDGetXML(const IVS_CHAR* cLoginID, IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, CXml &xml);

    static IVS_INT32 GetDefaultRoleAccountRuleParseXML(CXml &xml, IVS_ROLE_ACCOUNT_RULE* pRoleAccountRule);

    static IVS_INT32 GetOperationRightGroupParseXML(CXml &xml, IVS_OPERATION_RIGHT_GROUP_LIST* pRightGroupList);

    static IVS_VOID GetPWDExpireInfoGetXML(const IVS_CHAR* pLoginID, CXml &xml);

    static IVS_INT32 GetPWDExpireInfoParseXML(CXml &xml, IVS_PWD_EXPIRE_INFO* pPWDExpireInfo);

    static IVS_VOID CommXmlHead(CXml &xml);

    static IVS_VOID CommDomainCodeXml(const IVS_CHAR* pDomainCode, CXml &xml);

    static IVS_VOID CommQueryXml(const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, CXml &xml);

	/******************************************************************
   function		: GetStreamListByCamGetXML
   description: 构造根据摄像机Code，查询该摄像机正在给哪些用户提供视频业务的请求xml
   input			: pDomainCode 域编码
   input			: pCameraCode 摄像机编码
   output			: CXml & xml
   return			: 成功返回0，失败返回负数错误码
   *******************************************************************/   
	static IVS_INT32 GetStreamListByCamGetXML(const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
		CXml &xml);

	/******************************************************************
   function		: GetStreamListByCamParseXML
   description: 解析根据摄像机Code，查询该摄像机正在给哪些用户提供视频业务的响应xml
   input			: xml 响应xml
   input			: uiBufSize 申请内存大小
   output			: pStreamListByCam 摄像机开启的媒体业务列表
   return			: 成功返回0，失败返回负数错误码
   *******************************************************************/   
	static IVS_INT32 GetStreamListByCamParseXML(CXml &xml,
		IVS_UINT32 uiBufSize,
		IVS_STREAM_LIST_BY_CAM* pStreamListByCam);

	/******************************************************************
   function		: GetStreamListByUserGetXML
   description: 构造根据用户编码，查询该用户正在给调用哪些摄像机的视频业务的请求xml
   input			: pDomainCode 域编码
   input			: uiUserID 用户ID
   output			: CXml & xml
   return			: 成功返回0，失败返回负数错误码
   *******************************************************************/   
	static IVS_INT32 GetStreamListByUserGetXML(const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiUserID,
		CXml &xml);

	/******************************************************************
   function		: GetStreamListByUserParseXML
   description: 解析根据用户编码，查询该用户正在给调用哪些摄像机的视频业务的响应xml
   input			: xml 响应xml
   input			: uiBufSize 申请内存大小
   output			: pStreamListByUser 用户正在调用的媒体业务列表
   return			: 成功返回0，失败返回负数错误码
   *******************************************************************/   
	static IVS_INT32 GetStreamListByUserParseXML(CXml &xml,
		IVS_UINT32 uiBufSize,
		IVS_STREAM_LIST_BY_USER* pStreamListByUser);

	/******************************************************************
   function		: ShutdownStreamByUserGetXML
   description: 构造根据摄像机Code和用户ID，关闭该用户正在使用的视频业务的响应xml
   input			: uiUserID 用户ID
   input			: pCameraCode 摄像机编码
   input			: iStreamType 视频业务类型 1-实况，2-回放
   input			: pCameraCode 关闭时长，在此时间之内该用户不能再次点播，单位为秒，0为不控制关闭时长
   output			: xml
   return			: 成功返回0，失败返回负数错误码
   *******************************************************************/   
	static IVS_INT32 ShutdownStreamByUserGetXML(const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiUserID, 
		const IVS_CHAR* pCameraCode,
		IVS_INT32 iStreamType,
		IVS_INT32 iDuration,
		CXml &xml);

	/******************************************************************
   function		: PasswdValidationGetXML
   description: 构造验证登录用户名和密码的响应xml
   input			: strLoginName 登录用户名
   input			: strPasswd 密码
   output			: xml
   return			: 成功返回0，失败返回负数错误码
   *******************************************************************/   
	static IVS_INT32 PasswdValidationGetXML(const std::string &strLoginName,
		const std::string &strPasswd,
		CXml &xml);

};

#endif

