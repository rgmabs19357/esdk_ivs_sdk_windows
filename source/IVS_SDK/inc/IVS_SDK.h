/********************************************************************
filename	: 	IVSSDK.cpp
author		:	z00201790
created		:	2012/10/23	
description	:	定义 DLL 应用程序的导出函数;
copyright	:	Copyright (C) 2011-2015
history		:	2012/10/23 初始版本;
*********************************************************************/

#ifndef IVS_SDK_H
#define IVS_SDK_H

#include "hwsdk.h"
#include "ivs_error.h"
//#include "SDKDef.h"

#ifdef WIN32
#ifdef  IVS_SDK_EXPORTS
#define IVS_SDK_API __declspec(dllexport)
#else
#define IVS_SDK_API __declspec(dllimport)
#endif
#else
//#define IVS_SDK_API __attribute__((visibility))
#define IVS_SDK_API __attribute__((visibility("default")))
#endif

#ifdef WIN32
#define __SDK_CALL __stdcall
#else
#define __SDK_CALL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	/**************************************************************************
	* name       : IVS_SDK_Init
	* description: 初始化SDK
	* input      : NA
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_Init();

	/**************************************************************************
	* name       : IVS_SDK_Cleanup()
	* description: 释放SDK
	* input      : NA
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_Cleanup();

	/**************************************************************************
	* name       : IVS_SDK_GetVersion()
	* description: 获取SDK版本号，当前版本号为2.1.0.1
	* input      : NA
	* output     : NA
	* return     : 成功返回SDK版本号，高16位表示主版本号，低16位表示次版本号，失败返回负数错误码;
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetVersion();


    /**************************************************************************
	* name       : IVS_SDK_GetCUVersion()
	* description: 获取CU版本号
	* input      : NA
	* output     : NA
	* return     : NA
	* remark     : NA
	**************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCUVersion (IVS_INT32 iSessionID, const IVS_CU_INFO* pCUInfo, IVS_UPGRADE_INFO* pUpgradeInfo);


	/**************************************************************************
	* name       : IVS_SDK_SetEventCallBack()
	* description: 设置事件回调函数
	* input      : iSessionID        IVS_SDK_Login返回的会话ID	
	*              fEventCallBack    用户回调函数;
	*              pUserData		 用户数据;
	* output     : NA
	* return     : 成功返回SDK版本号，高16位表示主版本号，低16位表示次版本号，失败返回负数错误码;
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetEventCallBack(IVS_INT32 iSessionID, EventCallBack fEventCallBack = NULL, void *pUserData = NULL);

	/**************************************************************************
	* name       : IVS_SDK_Login
	* description: 用户登录
	* input      : IVS_LOGIN_INFO 登陆入参结构体
                       （ pUserName 用户名
	                    pPassword 密码
	                    pIP       服务器IP地址
	                    iPort     服务器端口
	                    loginType 登陆类型	：		0 –基本身份验证 	1 –Windows域身份验证—当前用户 	2 –Windows域身份验证—其他用户
	                    pDomainName   登录域名信息
	                    pMachineName 客户端登录的机器名）
    * output     : pSessionId  返回sessionId SessionID由SDK内部产生，大于0的整数，
                        作为其他接口的入参，用来区分访问的服务器，最多登录128个服务器，
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	**************************************************************************/

    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_Login(IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId);


	/**************************************************************************
	* name       : IVS_SDK_LoginByTicket
	* description: 用户单点登录;
	* input      : IVS_LOGIN_INFO_EX 登陆入参结构体;
					(cTicketID		用户的票据，和用户唯一对应;
					stIP			服务器IP地址;
					uiPort			服务器端口;
					uiLoginType		登陆类型: 0 –基本身份验证;
					cDomainName		登录域名信息;
					cMachineName	客户端登录的机器名;
					uiClientType	客户端类型：参考 IVS_CLIENT_TYPE 0-PC客户端, 1-web浏览器, 
						2-移动客户端, 3-PC客户端子进程，4-电视墙进程;);
    * output     : pSessionId  返回sessionId SessionID由SDK内部产生，大于0的整数，
                        作为其他接口的入参，用来区分访问的服务器，最多登录128个服务器;
	* return     : 成功返回0，失败返回错误码;
	* remark     : NA
	**************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LoginByTicket(IVS_LOGIN_INFO_EX* pLoginReqInfoEx, IVS_INT32* pSessionId);


    /**************************************************************************
	* name       : IVS_SDK_WinUserLogin
	* description: 用户登录
	* input      : IVS_LOGIN_INFO 登陆入参结构体
	                   （ pUserName 用户名
	                      pPassword 密码
	                      pIP       服务器IP地址
	                      iPort     服务器端口
	                      loginType 登陆类型	：		0 –基本身份验证 	1 –Windows域身份验证—当前用户 	2 –Windows域身份验证—其他用户
	                      pDomainName   登录域名信息
	                      pMachineName 客户端登录的机器名）
    * output     : pSessionId  返回sessionId SessionID由SDK内部产生，大于0的整数，
                        作为其他接口的入参，用来区分访问的服务器，最多登录128个服务器，
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	**************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_WinUserLogin(IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId);

    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_BackupUserData(IVS_INT32 iSessionID, const IVS_CHAR* pUserDataFile);

	/**************************************************************************
	* name       : IVS_SDK_GetLoginRspInfo
	* description: 获取登录响应信息
	* input      : iSessionID     登录返回的SessionID
	* output     : pLoginRspInfo  登录成功返回的信息，详见登录成功响应信息结构体
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	//IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetLoginRspInfo(IVS_INT32 iSessionID, LOGIN_RSP_INFO* pLoginRspInfo);

    /**************************************************************************
	* name       : IVS_SDK_GetUserID
	* description: 获取登陆用户的用户ID
	* input      : iSessionID     登录返回的SessionID
	* output     : puiUserID  用户ID
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	**************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserID(IVS_INT32 iSessionID, IVS_UINT32* puiUserID);

	/**************************************************************************
	* name       : IVS_SDK_Logout
	* description: 用户注销
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_Logout(IVS_INT32 iSessionID);

    /******************************************************************
    function : IVS_SDK_AddUser
    description: 添加用户
    input : IVS_INT32 iSessionID                登录成功后的会话ID
    input : const IVS_USER_INFO * pUserInfo     用户信息结构体
    output : IVS_UINT32 * puiUserID             添加成功后返回用户ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddUser(IVS_INT32 iSessionID, const IVS_USER_INFO* pUserInfo, IVS_UINT32* puiUserID);

    /******************************************************************
    function : IVS_SDK_ModifyUser
    description: 修改用户
    input : IVS_INT32 iSessionID                登录成功后的会话ID
    input : const IVS_USER_INFO * pUserInfo     用户信息结构体
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyUser(IVS_INT32 iSessionID, const IVS_USER_INFO* pUserInfo);

    /******************************************************************
    function : IVS_SDK_DeleteUser
    description: 删除用户
    input : IVS_INT32 iSessionID               登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode       域编码
    output : IVS_UINT32 uiUserID               用户ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID);

    /******************************************************************
    function : IVS_SDK_GetUserList
    description: 查询用户列表
    input : IVS_INT32 iSessionID                            登录成功后的会话ID
    input : const IVS_QUERY_UNIFIED_FORMAT * pUnifiedQuery  通用查询请求
    output : IVS_USER_INFO_LIST * pUserList                 用户信息列表结构体
    input : IVS_UINT32 uiBufferSize                         缓存大小
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_USER_INFO_LIST* pUserList, IVS_UINT32 uiBufferSize);

    /******************************************************************
    function : IVS_SDK_GetOnlineUser
    description: 查询在线用户列表
    input : IVS_INT32 iSessionID                            登录成功后的会话ID
    input : const IVS_QUERY_UNIFIED_FORMAT * pUnifiedQuery  通用查询请求
    output : IVS_ONLINE_USER_LIST * pOnlineUserList         在线用户信息列表结构体
    input : IVS_UINT32 uiBufferSize                         缓存大小
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetOnlineUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ONLINE_USER_LIST* pOnlineUserList, IVS_UINT32 uiBufferSize);

    /******************************************************************
    function : IVS_SDK_GetUserInfo
    description: 查询用户信息
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiUserID             用户ID
    output : IVS_USER_INFO * pUserInfo      用户信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserInfo(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_USER_INFO* pUserInfo);

    /******************************************************************
    function : IVS_SDK_LockUser
    description: 锁定/解锁用户
    input : IVS_INT32 iSessionID        	登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiUserID         	用户ID
    input : IVS_BOOL bLock              	是否锁定用户：0-解锁，1-锁定
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LockUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_BOOL bLock);

    /******************************************************************
    function : IVS_SDK_SetUserDeviceList
    description: 设置用户设备列表
    input : IVS_INT32 iSessionID                            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode                    域编码
    input : IVS_UINT32 uiUserID                             用户ID
    input : const IVS_USER_DEVICE_INFO_LIST * pUserDevList  用户列表结构体
    input : IVS_UINT32 uiBufferSize                         缓存大小
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetUserDeviceList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 uiBufferSize);

    /******************************************************************
    function : IVS_SDK_GetUserDeviceList
    description: 获取用户设备列表
    input : IVS_INT32 iSessionID                            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode                    域编码
    input : IVS_UINT32 uiUserID                             用户ID
    input : const IVS_INDEX_RANGE * pIndexRange             分页信息
    output : IVS_USER_DEVICE_INFO_LIST * pUserDevList       用户列表结构体
    input : IVS_UINT32 uiBufferSize                         缓存大小
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserDeviceList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 uiBufferSize);

    /******************************************************************
    function : IVS_SDK_ChangePWD
    description: 修改密码
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pOldPWD   旧密码
    input : const IVS_CHAR * pNewPWD   新密码
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ChangePWD(IVS_INT32 iSessionID, const IVS_CHAR* pOldPWD, const IVS_CHAR* pNewPWD);

    /******************************************************************
    function : IVS_SDK_ResetPWD
    description: 重置密码
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : const IVS_UINT32 iUserID        用户ID
    input : const IVS_CHAR * pNewPWD        用户新密码
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ResetPWD(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, const IVS_CHAR* pNewPWD);

    /******************************************************************
    function : IVS_SDK_CheckPWDByRole
    description: 校验密码是否满足账号规则
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : const IVS_UINT32 uiRoleID       角色ID
    input : const IVS_CHAR * pPWD           用户设置的密码
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_CheckPWDByRole(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, const IVS_CHAR* pPWD);

    /******************************************************************
    function : IVS_SDK_GetAutoCreatePWD
    description: 根据角色获取符合账号规则的密码
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiRoleID             角色ID
    input : IVS_CHAR* pLoginName            用户登录名
    output : IVS_CHAR* pNewPWD              密码字符串
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAutoCreatePWD(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, IVS_CHAR* pNewPWD, IVS_UINT32 uiPWDLen);

    /******************************************************************
    function : IVS_SDK_GetPWDStrength
    description: 获取密码强度
    input : cosnt IVS_CHAR * pPWD       密码
    input : IVS_UINT32 * uiPWDStrength  密码强度
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPWDStrength(const IVS_CHAR* pPWD, IVS_UINT32* uiPWDStrength);

    /******************************************************************
    function : IVS_SDK_GetPWDExpireInfo
    description: 获取密码过期信息
    output : IVS_PWD_EXPIRE_INFO * pPWDExpireInfo    密码过期信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPWDExpireInfo(IVS_INT32 iSessionID, IVS_PWD_EXPIRE_INFO* pPWDExpireInfo);

    /******************************************************************
    function : IVS_SDK_SignOutUser
    description: 根据用户ID签退用户的所有会话
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiUserID             用户ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SignOutUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID);

    /******************************************************************
    function : IVS_SDK_SignOutUserByRole
    description: 根据角色ID签退用户的所有会话
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiRoleID             角色ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SignOutUserByRole(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID);

    /******************************************************************
    function : IVS_SDK_SignOutOnlineUser
    description: 签退在线用户
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiUserOnlineID       在线用户的会话ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SignOutOnlineUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserOnlineID);

    /******************************************************************
    function : IVS_SDK_AddUserGroup
    description: 增加用户组
    input : IVS_INT32 iSessionID                登录成功后的会话ID
    input : const IVS_USER_GROUP * pUserGroup   用户组信息结构体
    input : IVS_UINT32 * puiUserGroupID          用户组ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddUserGroup(IVS_INT32 iSessionID, const IVS_USER_GROUP* pUserGroup, IVS_UINT32* puiUserGroupID);

    /******************************************************************
    function : IVS_SDK_DeleteUserGroup
    description: 删除用户组
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiUserGroupID        用户组ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteUserGroup(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserGroupID);

    /******************************************************************
    function : IVS_SDK_ModifyUserGroup
    description: 修改用户组
    input : IVS_INT32 iSessionID                登录成功后的会话ID
    input : const IVS_USER_GROUP * pUserGroup   用户组信息结构体
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyUserGroup(IVS_INT32 iSessionID, const IVS_USER_GROUP* pUserGroup);

    /******************************************************************
    function : IVS_SDK_MoveUserGroup
    description: 移动用户组
    input : IVS_INT32 iSessionID                        登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode                域编码
    input : IVS_UINT32 uiTargetGroupID                  移动到的用户组ID
    input : const IVS_USER_GROUP_LIST * pUserGroupList  用户组列表结构体
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_MoveUserGroup(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiTargetGroupID, const IVS_USER_GROUP_LIST* pUserGroupList);

    /******************************************************************
    function : IVS_SDK_GetUserGroupList
    description: 查询用户组列表
    input : IVS_INT32 iSessionID                    登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode            域编码
    input : const IVS_INDEX_RANGE * pIndexRange     分页索引结构体
    output : IVS_USER_GROUP_LIST * pUserGroupList   用户组列表结构体
    input : IVS_UINT32 uiBufferSize                 缓存大小
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserGroupList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_GROUP_LIST* pUserGroupList, IVS_UINT32 uiBufferSize);

    /******************************************************************
    function : IVS_SDK_AddRole
    description: 增加角色
    input : IVS_INT32 iSessionID                登录成功后的会话ID
    input : const IVS_ROLE_INFO * pRoleInfo     角色信息结构体
    input : IVS_UINT32 * puiRoleID              角色ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRole(IVS_INT32 iSessionID, const IVS_ROLE_INFO* pRoleInfo, IVS_UINT32* puiRoleID);

    /******************************************************************
    function : IVS_SDK_DeleteRole
    description: 删除角色
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiRoleID             角色ID
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRole(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID);

    /******************************************************************
    function : IVS_SDK_ModifyRole
    description: 修改角色
    input : IVS_INT32 iSessionID                登录成功后的会话ID
    input : const IVS_ROLE_INFO * pRoleInfo     角色信息结构体
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRole(IVS_INT32 iSessionID, const IVS_ROLE_INFO* pRoleInfo);

    /******************************************************************
    function : IVS_SDK_GetRoleList
    description: 查询角色列表
    input : IVS_INT32 iSessionID                    登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode            域编码
    input : const IVS_INDEX_RANGE * pIndexRange     分页索引结构体
    output : IVS_ROLE_INFO_LIST * pRoleInfoList     角色列表结构体
    input : IVS_UINT32 uiBufferSize                 缓存大小
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRoleList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_ROLE_INFO_LIST* pRoleInfoList, IVS_UINT32 uiBufferSize);

    /******************************************************************
    function : IVS_SDK_GetRoleInfoByRoleID
    description: 根据角色ID获取角色详细信息
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiRoleID             角色ID
    output : IVS_ROLE_INFO * pRoleInfo      角色信息结构体
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRoleInfoByRoleID(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, IVS_ROLE_INFO* pRoleInfo);

    /******************************************************************
    function : IVS_SDK_GetRoleInfoByUserID
    description: 根据用户ID获取角色详细信息
    input : IVS_INT32 iSessionID            登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode    域编码
    input : IVS_UINT32 uiUserID             角色ID
    output : IVS_ROLE_INFO * pRoleInfo      角色信息结构体
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRoleInfoByUserID(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_ROLE_INFO* pRoleInfo);

    /******************************************************************
    function : IVS_SDK_GetDefaultRoleAccountRule
    description: 获取默认账户规则
    input : IVS_INT32 iSessionID                        登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode                域编码
    output : IVS_ROLE_ACCOUNT_RULE * pRoleAccountRule   账户规则
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDefaultRoleAccountRule(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_ROLE_ACCOUNT_RULE* pRoleAccountRule);

    /******************************************************************
    function : IVS_SDK_GetOperationRightGroup
    description: 获取用户权限信息
    input : IVS_INT32 iSessionID                                登录成功后的会话ID
    input : const IVS_CHAR * pDomainCode                        域编码
    output : IVS_OPERATION_RIGHT_GROUP_LIST * pRightGroupList   权限组列表
    input : IVS_UINT32 uiBufferSize                             缓存大小
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetOperationRightGroup(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_OPERATION_RIGHT_GROUP_LIST* pRightGroupList, IVS_UINT32 uiBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_AddPlatformRecordPlan
	* description: 新增平台录像计划
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRecordPlan(IVS_INT32 iSessionID,IVS_UINT32 uiDeviceNum,
		const IVS_DEVICE_CODE* pDeviceList,const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_AddPlatformRecordPlan
	* description: 修改平台录像计划
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRecordPlan(IVS_INT32 iSessionID,IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
		const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_AddPlatformRecordPlan
	* description: 删除平台录像计划
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRecordPlan(IVS_INT32 iSessionID,IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
		IVS_UINT32 uiRecordMethod,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_AddPlatformRecordPlan
	* description: 查询平台录像计划
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordPlan(IVS_INT32 iSessionID,const char* pCameraCode,IVS_UINT32 uiRecordMethod,IVS_RECORD_PLAN *pRecordPlan);

    /******************************************************************
    function : IVS_SDK_GetCameraPlanInfo
    description: 查询摄像机计划设置信息
    input : IVS_INT32 iSessionID                             登录成功后的会话ID
    input : IVS_UINT32 uiPlanType                            录像类型
    output : IVS_CHAR** pRspXml                              响应消息
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCameraPlanInfo(IVS_INT32 iSessionID, IVS_UINT32 uiPlanType, IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_AddBookmark
	* description: 新增录像书签
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,const IVS_CHAR* pNVRCode,
		const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID);

	/*************************************************************************
	* name       : IVS_SDK_ModifyBookmark
	* description: 修改录像书签
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName);
	/*************************************************************************
	* name       : IVS_SDK_DeleteBookmark
	* description: 删除录像书签
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID);

		/*************************************************************************
	* name       : IVS_SDK_AddRealPlayBookmark
	* description: 新增实况书签
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRealPlayBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,
		const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID);

	/*************************************************************************
	* name       : IVS_SDK_ModifyRealPlayBookmark
	* description: 修改实况书签
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRealPlayBookmark(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName);
	/*************************************************************************
	* name       : IVS_SDK_DeleteRealPlayBookmark
	* description: 删除实况书签
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRealPlayBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID);

	/*************************************************************************
	* name       : IVS_SDK_GetBookmarkList
	* description: 查询录像书签列表
	* input      : iSessionID 登录成功后的会话ID
	               pUnifiedQuery 通用查询结构
				   pBookmarkList 返回录像书签查询列表
				   uiBufSize 分配内存大小
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetBookmarkList(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
		IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiBufSize);

	/*************************************************************************
	* name       : IVS_SDK_QueryRecordList
	* description: 查询录像列表
	* input      : iSessionID 登录成功后的会话ID
	               pUnifiedQuery 通用查询结构
				   pRecordInfoList 返回录像查询结构列表
				   uiBufferSize 分配内存大小
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryRecordList(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
		IVS_RECORD_INFO_LIST* pRecordInfoList,IVS_UINT32 uiBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_GetRecordPolicyByTime
	* description: 查询平台录像策略
	* input      : pCameraCode 摄像机ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordPolicyByTime(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_RECORD_POLICY_TIME* pRecordPolicy);

	/*************************************************************************
	* name       : IVS_SDK_SetRecordPolicyByTime
	* description: 修改录像策略
	* input      : pRecordPolicy 摄像机ID
	               iSessionID  登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetRecordPolicyByTime(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,const IVS_RECORD_POLICY_TIME* pRecordPolicy);

	/*************************************************************************
	* name       : IVS_SDK_GetRecordList
	* description: 查询录像列表
	* input      : iSessionID 登录成功后的会话ID
	               pCameraCode 摄像机ID
				   iRecordMethod 录像存储位置
				   pTimeSpan 时间段信息
				   pIndexRange 分页信息
				   pRecordList 返回录像列表
				   iBufSize 内存分配空间大小
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* remark     : NA

	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordList(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_INT32 iRecordMethod,const IVS_TIME_SPAN* pTimeSpan,
		const IVS_INDEX_RANGE* pIndexRange,IVS_RECORD_INFO_LIST* pRecordList,IVS_UINT32 uiBufSize);


	/*************************************************************************
	* name       : IVS_SDK_SubscribeAlarmQuery
	* description: 查看告警订阅列表
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SubscribeAlarmQuery(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_SubscribeAlarm
	* description: 新增告警订阅
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SubscribeAlarm(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*************************************************************************
	* name       : IVS_SDK_AddHelpInfo
	* description: 新增告警帮助
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddHelpInfo(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_HELP_INFO *pHelpInfo);

	/*************************************************************************
	* name       : IVS_SDK_ModifyHelpInfo
	* description: 修改告警帮助
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyHelpInfo(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_HELP_INFO *pHelpInfo);

	/*************************************************************************
	* name       : IVS_SDK_DeleteHelpInfo
	* description: 删除告警帮助
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteHelpInfo(IVS_INT32 iSessionID, IVS_UINT32 uiAlarmHelpSourceType,IVS_UINT32 uiAlarmHelpSourceId, const IVS_CHAR* pDomainCode);

	/*************************************************************************
	* name       : IVS_SDK_QueryHelpInfo
	* description: 查询告警帮助详情
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryHelpInfo(IVS_INT32 iSessionID, IVS_UINT32 uiAlarmHelpSourceType,IVS_UINT32 uiAlarmHelpSourceId,const IVS_CHAR* pDomainCode, IVS_HELP_INFO* pHelpInfoResult);

    /******************************************************************
     function   : IVS_SDK_GetHelpInfoText
     description: 查询帮助信息正文
     input      : IVS_INT32 iSessionID
     input      : const IVS_CHAR * pAlarmInCode
     input      : const IVS_CHAR * pAlarmType
     input      : IVS_CHAR * pHelpInfo
     input      : IVS_UINT32 uiBufferSize
     output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetHelpInfoText(IVS_INT32 iSessionID, const IVS_CHAR* pAlarmInCode, const IVS_CHAR* pAlarmType, IVS_CHAR* pHelpInfo, IVS_UINT32 uiBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmTypeList
	* description: 查询自定义告警类型列表
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmTypeList (IVS_INT32 iSessionID, 
        const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ALARM_TYPE_LIST* pAlarmTypeList, IVS_UINT32 uiBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_SetAlarmTypeLevel
	* description: 查询告警类型级别
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetAlarmTypeLevel(IVS_INT32 iSessionID,  
        const IVS_ALARM_TYPE_LIST* pAlarmTypeList);


	/*************************************************************************
	* name       : IVS_SDK_AddAlarmLevel
	* description: 新增告警级别
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddAlarmLevel(IVS_INT32 iSessionID, const IVS_ALARM_LEVEL *pAlarmLevel,IVS_UINT32* pAlarmLevelID);

	/*************************************************************************
	* name       : IVS_SDK_ModifyAlarmLevel
	* description: 修改告警级别
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyAlarmLevel(IVS_INT32 iSessionID, const IVS_ALARM_LEVEL *pAlarmLevel);

	/*************************************************************************
	* name       : IVS_SDK_DeleteAlarmLevel
	* description: 删除告警级别
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteAlarmLevel(IVS_INT32 iSessionID, const IVS_UINT32 uiAlarmLevelId);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmLevelList
	* description: 查询告警级别列表
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmLevelList(IVS_INT32 iSessionID,IVS_UINT32 uiMaxAlarmLevelNum,IVS_UINT32* puiAlarmLevelNum,IVS_ALARM_LEVEL* pAlarmLevel,IVS_UINT32 uiBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_AddAlarmArea
	* description: 新增防区
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddAlarmArea(IVS_INT32 iSessionID, const IVS_CHAR* pReq, IVS_CHAR** pRsp);

	/*************************************************************************
	* name       : IVS_SDK_ModifyAlarmArea
	* description: 修改防区
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyAlarmArea(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*************************************************************************
	* name       : IVS_SDK_DeleteAlarmArea
	* description: 删除防区
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteAlarmArea(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmAreaList
	* description: 查看防区列表
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmAreaList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmAreaInfo
	* description: 查看防区详情
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmAreaInfo(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId,IVS_CHAR** pAlarmArea);

	/*************************************************************************
	* name       : IVS_SDK_StartAlarmAreaGuard
	* description: 手动布防
	* input      : iSessionID 登录成功后的会话ID;uiAlarmAreaId,防区ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartAlarmAreaGuard(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId);

	/*************************************************************************
	* name       : IVS_SDK_SubscribeAlarmQuery
	* description: 手动撤防
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopAlarmAreaGuard(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId);

	/*************************************************************************
	* name       : IVS_SDK_AllocAreaGuardPlan
	* description: 新增布防计划
	* input      : iSessionID 登录成功后的会话ID；pAlarmAreaPlan，防区布防计划结构体
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     : wulixia
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AllocAreaGuardPlan(IVS_INT32 iSessionID,const IVS_CHAR* pReqXml);

	/*************************************************************************
	* name       : IVS_SDK_ModifyAreaGuardPlan
	* description: 修改布防计划
	* input      : iSessionID 登录成功后的会话ID;  pAlarmAreaPlan，防区布防计划结构体
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyAreaGuardPlan(IVS_INT32 iSessionID,	const IVS_CHAR* pReqXml);

	/*************************************************************************
	* name       : IVS_SDK_DeleteAreaGuardPlan
	* description: 删除布防计划
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteAreaGuardPlan(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId);

	/*************************************************************************
	* name       : IVS_SDK_GetAreaGuardPlan
	* description: 查看布防计划
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAreaGuardPlan(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId, IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_AddAlarmLinkage
	* description: 新增告警联动策略
	* input      : iSessionID 登录成功后的会话ID;pReqXml，请求消息
	* output     : pRspXml,响应消息，包含有联动策略ID
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddAlarmLinkage(IVS_INT32 iSessionID,	const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_ModifyAlarmLinkage
	* description: 修改告警联动策略
	* input      : iSessionID 登录成功后的会话ID；pReqXml，请求消息
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyAlarmLinkage(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*************************************************************************
	* name       : IVS_SDK_DeleteAlarmLinkage
	* description: 删除告警联动策略
	* input      : iSessionID 登录成功后的会话ID;uiAlarmLinkageID，告警联动策略ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteAlarmLinkage(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmlinkageList
	* description: 查看告警联动策略列表
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmlinkageList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmLinkage
	* description: 查询告警联动策略
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmLinkage (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_ConfirmAlarm
	* description: 告警确认
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmConfirm(IVS_INT32 iSessionID, IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo);

	/*************************************************************************
	* name       : IVS_SDK_CancelAlarm
	* description: 告警撤销
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmCancel(IVS_INT32 iSessionID, IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo);

	/*************************************************************************
	* name       : IVS_SDK_AlarmCommission
	* description: 告警授权
	* input      : iSessionID 登录成功后的会话ID;pAlarmCommission,告警授权结构体
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmCommission (IVS_INT32 iSessionID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_COMMISSION* pAlarmCommission);

	/*************************************************************************
	* name       : IVS_SDK_UserAlarmAuthorization
	* description: 授权告警用户认证
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UserAuthentication(IVS_INT32 iSessionID, const IVS_CHAR *pPWD);

	/*************************************************************************
	* name       : IVS_SDK_AlarmReport
	* description: 告警信息上报（告警推送）
	* input      : iSessionID 登录成功后的会话ID；pAlarmReport， 信息上报结构体
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmReport(IVS_INT32 iSessionID, const IVS_ALARM_REPORT* pAlarmReport);

	/*************************************************************************
	* name       : IVS_SDK_ManualAlarm
	* description: 第三方告警上报
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ManualAlarm(IVS_INT32 iSessionID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp);

	/*************************************************************************
	* name       : IVS_SDK_StartAlarmOut
	* description: 启动开关量输出告警
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartAlarmOut(IVS_INT32 iSessionID, const IVS_CHAR* pAlarmOutCode);

	/*************************************************************************
	* name       : IVS_SDK_StopAlarmOut
	* description: 停止开关量输出告警
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopAlarmOut(IVS_INT32 iSessionID, const IVS_CHAR* pAlarmOutCode);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmEventInfo
	* description: 查询告警信息
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmEventInfo(IVS_INT32 iSessionID, IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, IVS_ALARM_EVENT* pAlarmEvent);

	/*************************************************************************
	* name       : IVS_SDK_GetAlarmEventList
	* description: 查询历史告警信息列表
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmEventList(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ALARM_EVENT_LIST* pAlarmEventList, IVS_UINT32 uiBufferSize);


	/*************************************************************************
	* name       : IVS_SDK_AddDevice
	* description: 增加设备
	* input      : iSessionID 登录成功后的会话ID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-02
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDevice(IVS_INT32 iSessionID, 
                                                       const IVS_CHAR* pDomainCode, 
                                                       IVS_UINT32 uiDeviceNum, 
                                                       const IVS_DEVICE_OPER_INFO* pDevList, 
                                                       IVS_DEVICE_OPER_RESULT_LIST* pResultList, 
                                                       IVS_UINT32 uiResultBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_DeleteDevice
	* description: 删除设备
	* input      : iSessionID 登录成功后的会话ID, pDomainCode域编码 uiDeviceNum 删除的设备数目
	               pDevCodeList 设备编码列表 pResultList 设备批量操作结果列表 uiResultBufferSize 缓存大小
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：wulixia
	* remark     : NA
	* history    : 2012-11-21
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteDevice(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,IVS_UINT32 uiDeviceNum,
		const IVS_DEVICE_CODE* pDevCodeList, IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_QueryDeviceList
	* description: 查询设备列表
	* input      : iSessionID 登录成功后的会话ID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	               2013-1-10 恢复pDomainCode
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryDeviceList(IVS_INT32 iSessionID,
		const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiDeviceType, 
		const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat, 
		IVS_VOID* pBuffer, 
		IVS_UINT32 uiBufferSize);
    
	/******************************************************************
	function : IVS_SDK_GetDeviceList
	description: 获取设备列表
	author: fengjiaji f00152768
	input : IVS_INT32 iSessionID 登录成功后的会话ID
	input : IVS_UINT32 uiDeviceType 设备类型，值参考IVS_DEVICE_TYPE
	input : const IVS_INDEX_RANGE * pIndexRange 分页信息
	output : IVS_VOID * pDeviceList 设备列表指针
	input : IVS_UINT32 uiBufferSize 缓存大小
	return : IVS_SDK_API IVS_INT32
	*******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceList(
		IVS_INT32 iSessionID,
		IVS_UINT32 uiDeviceType,
		const IVS_INDEX_RANGE* pIndexRange,
		IVS_VOID* pDeviceList,
		IVS_UINT32 uiBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_GetDeviceInfo
	* description: 查询主设备详细信息
	* input      : 
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：gWX151718
	* remark     : NA
	* history    : 2012-11-21
	*************************************************************************/		
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceInfo(IVS_INT32 iSessionID, 
                                                           const IVS_CHAR* pDomainCode,
                                                           const IVS_CHAR* pDevCode, 
                                                           IVS_DEVICE_INFO* pDeviceInfo);

	/*************************************************************************
	* name       : IVS_SDK_AddDeviceGroup
	* description: 新增设备组
	* input      : iSessionID 登录成功后的会话ID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDeviceGroup (IVS_INT32 iSessionID, const IVS_DEV_GROUP* pDevGroup,IVS_UINT32* puiDevGroupID);
	
	/*************************************************************************
	* name       : IVS_SDK_GetDeviceGroup
	* description: 查询设备组列表
	* input      : iSessionID 登录成功后的会话ID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/		
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceGroup(IVS_INT32 iSessionID,
                                                            const IVS_CHAR* pDomainCode,
                                                            const IVS_CHAR* pDevGroupCode,
                                                            IVS_DEVICE_GROUP_LIST* pDeviceGroupList,
                                                            IVS_UINT32 uiBufferSize);

    /******************************************************************
    function : IVS_SDK_AddDeviceToGroup
    description: 新增设备组下的设备
    input : IVS_INT32 iSessionID
            const IVS_CHAR * pTargetDomainCode
            IVS_UINT32 uiTargetDevGroupID
            const IVS_DEV_GROUP_CAMERA_LIST * pGroupCameraList
            IVS_UINT32 uiBufferSize
    output : NA
    return : IVS_INT32
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDeviceToGroup( IVS_INT32 iSessionID, 
                                        const IVS_CHAR* pTargetDomainCode,
                                        IVS_UINT32 uiTargetDevGroupID,
                                        IVS_UINT32 uiDeviceNum,
                                        const IVS_DEV_GROUP_CAMERA* pGroupCameraList);

    /*************************************************************************
    * name       : DeleteDeviceGroup
    * description: 删除设备组
    * input      :  iSessionID 登陆成功返回的sessionId，用于标识唯一用户
                       pDeviceGroupList, 设备组列表
                      uiGroupNum  设备组Num
                      uiResultBufferSize 缓存大小
    * output     : pResultList 删除结果
    * return     : 成功返回0，失败返回负数错误码
    * author     ：xiongfeng 00192614
    * remark     : NA
    * history    : 2012-11-06
    *************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteDeviceGroup( IVS_INT32 iSessionID,
                                                                const IVS_CHAR* pDomainCode,
                                                                IVS_UINT32 uiDevGroupID);

	/*************************************************************************
	* name       : IVS_SDK_GetDeviceConfig
	* description: 查询设备参数
	* input      : iSessionID 登录成功后的会话ID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
	//IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceConfig(IVS_INT32 iSessionID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp);

    /**************************************************************************
    * name       : IVS_SDK_StartRealPlay
    * description: 开始实时浏览，本地流复用时可多次调用, 
                   在看第一个实况的时候，拖第二个摄像机到该窗口，
                   会主动停止第一个实况，再播放第二个摄像头的实况，接口需要实现视频关键帧申请。
    * input      : iSessionID 登录成功后的会话ID
                   pRealplayPara 实况参数，详见媒体参数结构体
                   pCamerCode 摄像头编码
                   hWnd       窗口句柄
    * output     : pHandle    播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng00193168
	* remark     : NA
	* history    : 2012-11-09
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlay(IVS_INT32 iSessionID, IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, HWND hWnd, IVS_ULONG* pHandle);

	/**************************************************************************
    * name       : IVS_SDK_StartRealPlayCBRaw
    * description: 开始实时浏览，本地流复用时可多次调用, 
                   在看第一个实况的时候，拖第二个摄像机到该窗口，
                   会主动停止第一个实况，再播放第二个摄像头的实况，接口需要实现视频关键帧申请。
    * input      : iSessionID           登录成功后的会话ID
                   pRealplayPara        实况参数，详见实况参数结构体
                   pCamerCode           摄像头编码
                   fRealPlayCallBackRaw 实时浏览回调函数
				   pUserData            用户数据
    * output     : pHandle              播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng00193168
	* remark     : NA
	* history    : 2012-11-12
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayCBRaw(IVS_INT32 iSessionID, IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, 
                                                                RealPlayCallBackRaw fRealPlayCallBackRaw, void* pUserData, IVS_ULONG* pHandle );

	/**************************************************************************
    * name       : IVS_SDK_StartRealPlayCBFrame
    * description: 开始实时浏览，本地流复用时可多次调用, 
                   在看第一个实况的时候，拖第二个摄像机到该窗口，
                   会主动停止第一个实况，再播放第二个摄像头的实况，接口需要实现视频关键帧申请。
    * input      : iSessionID               登录成功后的会话ID
                   pRealplayPara            实况参数，详见实况参数结构体
                   pCamerCode               摄像头编码
                   fRealPlayCallBackFrame   实时浏览回调函数
				   pUserData                用户数据
    * output     : pHandle                  播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng00193168
	* remark     : NA
	* history    : 2012-11-12
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayCBFrame(IVS_INT32 iSessionID, IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, 
                                                                  RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData, IVS_ULONG* pHandle);

	/**************************************************************************
    * name       : IVS_SDK_StopRealPlay
    * description: 停止实时浏览。
    * input      : iSessionID 登录成功后的会话ID
	               ulHandle   播放句柄
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng00193168
	* remark     : NA
	* history    : 2012-11-12
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlay( IVS_INT32 iSessionID, IVS_ULONG ulHandle);

    /**************************************************************************
    * name       : IVS_SDK_AddPlayWnd
    * description: 
    * input      : iSessionID 登录成功后的会话ID
	               ulPlayHandle   播放句柄
                   hWnd       窗口句柄
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：
	* remark     : NA
	* history    : 2012-12-15
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddPlayWnd(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd);

	/**************************************************************************
    * name       : IVS_SDK_DelPlayWnd
    * description: 
    * input      : iSessionID 登录成功后的会话ID
	               ulPlayHandle   播放句柄
                   hWnd       窗口句柄
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：
	* remark     : NA
	* history    : 2012-12-15
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelPlayWnd(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd);

	/**************************************************************************
    * name       : IVS_SDK_AddPlayWindowPartial
    * description: 
    * input      : iSessionID 登录成功后的会话ID
	               ulPlayHandle   播放句柄
                   hWnd       窗口句柄
				   pPartial 放大窗口的百分比
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：
	* remark     : NA
	* history    : 2013-3-8
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddPlayWindowPartial(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd, void *pPartial);

	/**************************************************************************
    * name       : IVS_SDK_UpdateWindowPartial
    * description: 
    * input      : iSessionID 登录成功后的会话ID
	               ulPlayHandle   播放句柄
                   hWnd       窗口句柄
				   pPartial 放大窗口的百分比
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：
	* remark     : NA
	* history    : 2013-3-8
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UpdateWindowPartial(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd, void *pPartial);

	/**************************************************************************
    * name       : IVS_SDK_GetOperationLog
    * description: 查询操作日志。
    * input      : iSessionID 登录成功后的会话ID
	               iPlayHandle 播放句柄
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：
	* remark     : NA
	* history    : 2012-11-14
    **************************************************************************/

	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetOperationLog(IVS_INT32 iSessionID,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_OPERATION_LOG_LIST* pResOperLogList, IVS_UINT32 uiBufSize);


	/**************************************************************************
    * name			: IVS_SDK_GetOperationLogEx
    * description: 扩展查询操作日志
    * input			: iSessionID 登录成功后的会话ID
	* input			: pUnifiedQuery 统一通用查询条件
	* input			: pOptLogQueryEx 扩展查询操作日志条件
	* input			: uiBufSize 事件内容大小
    * output			: pResOperLogList 操作日志查询查询结果
    * return			: 成功返回IVS_SUCCESSED，失败返回错误码
    * author			: zwx211831 zhuohaofan
	* remark		: NA
	* history			: 2014-5-6
    **************************************************************************/

	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetOperationLogEx(IVS_INT32 iSessionID,
		const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
		const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
		IVS_OPERATION_LOG_LIST* pResOperLogList,
		const IVS_UINT32 uiBufSize);


	/**************************************************************************
    * name       : IVS_SDK_StartPlatformPlayBack
    * description: 开始客户端平台录像回放
    * input      : iSessionID      会话ID
				   pCameraCode     摄像机编码
                   pPlaybackParam  回放参数
    * output     : pHandle         播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBack(IVS_INT32 iSessionID,
                                                                   const IVS_CHAR* pCameraCode,
                                                                   const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                                   HWND hWnd,
                                                                   IVS_ULONG* pHandle);

   /**************************************************************************
    * name       : IVS_SDK_StopPlatformPlayBack
    * description: 停止客户端平台录像回放
    * input      : iSessionID      会话ID
				   ulPlayHandle    播放句柄
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformPlayBack(IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle);

	/**************************************************************************
    * name       : IVS_SDK_StartPlatformPlayBackByIP
    * description: 开始客户端平台录像回放
    * input      : iSessionID      会话ID
				   pCameraCode     摄像机编码
                   pPlaybackParam  回放参数
    * output     : pHandle         播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackByIP(IVS_INT32 iSessionID,
																	const IVS_CHAR* pCameraCode,
																	const IVS_PLAYBACK_PARAM* pPlaybackParam,
																	const IVS_MEDIA_ADDR* pMediaAddrDst,
																	IVS_MEDIA_ADDR* pMediaAddrSrc,
																	IVS_ULONG* pHandle);

   /**************************************************************************
    * name       : IVS_SDK_StopPlatformPlayBackByIP
    * description: 停止客户端平台录像回放
    * input      : iSessionID      会话ID
				   ulPlayHandle    播放句柄
    * output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformPlayBackByIP(IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle);

	/**************************************************************************
    * name			: IVS_SDK_PlatformPlayBackPauseByIP
    * description: 暂停平台录像回放
    * input			: iSessionID	会话ID
							: ulPlayHandle	播放句柄
    * output			: NA
    * return			: 成功返回IVS_SUCCESSED，失败返回错误码
    * remark		: NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlatformPlayBackPauseByIP(IVS_INT32 iSessionID,
																	IVS_ULONG ulPlayHandle);

	/**************************************************************************
    * name			: IVS_SDK_PlatformPlayBackResumeByIP
    * description: 恢复平台录像回放
    * input			: iSessionID	会话ID
							: ulPlayHandle	播放句柄
    * output			: NA
    * return			: 成功返回0（IVS_SUCCEED），失败返回错误码
    * remark		: NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlatformPlayBackResumeByIP(IVS_INT32 iSessionID,
																	IVS_ULONG ulPlayHandle);

	 /**************************************************************************
    * name			: IVS_SDK_SetPlayBackSpeedByIP
    * description: 设置平台录像码流回放速度
    * input			: iSessionID	会话ID
							: ulPlayHandle	播放句柄
							: fSpeed	播放速度
    * output			: NA
    * return			: 成功返回0（IVS_SUCCEED），失败返回错误码
    * remark		: NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackSpeedByIP(IVS_INT32 iSessionID, 
		IVS_ULONG ulPlayHandle, 
		IVS_FLOAT fSpeed);

	 /**************************************************************************
    * name			: IVS_SDK_SetPlayBackTimeByIP
    * description: 设置平台录像码流回放时间
    * input			: iSessionID	会话ID
							: ulPlayHandle	播放句柄
							: uiTime	拖动尺度，单位秒，正整数，绝对时间
    * output			: NA
    * return			: 成功返回0（IVS_SUCCEED），失败返回错误码
    * remark		: NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackTimeByIP(IVS_INT32 iSessionID, 
		IVS_ULONG ulPlayHandle, 
		IVS_UINT32 uiTime);

	/**************************************************************************
    * name       : IVS_SDK_StartPlatformPlayBack
    * description: 开始客户端平台录像回放
    * input      : iSessionID      会话ID
				   pCameraCode     摄像机编码
                   pPlaybackParam  回放参数
    * output     : pHandle         播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 IVS_SDK_StartLocalPlayBack (
		                       IVS_INT32 iSessionID,
		                       const IVS_CHAR* pFileName,
		                       const IVS_LOCAL_PLAYBACK_PARAM* pPlaybackParam,
		                       HWND hWnd, 
							   IVS_ULONG* pHandle);
	
	/**************************************************************************
    * name       : IVS_SDK_StartPlatformPlayBack
    * description: 开始客户端平台录像回放
    * input      : iSessionID      会话ID
				   pCameraCode     摄像机编码
                   pPlaybackParam  回放参数
    * output     : pHandle         播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 IVS_SDK_StopLocalPlayBack(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);


    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartDisasterRecoveryPlayBack (
        IVS_INT32 iSessionID,
        const IVS_CHAR* pCameraCode,
        const IVS_PLAYBACK_PARAM* pPlaybackParam,
        HWND hWnd,
        IVS_ULONG* pHandle);

    IVS_SDK_API IVS_INT32  __SDK_CALL  IVS_SDK_StopDisasterRecoveryPlayBack(
        IVS_INT32 iSessionID,
        IVS_ULONG ulPlayHandle);


    /**************************************************************************
    * name       : IVS_SDK_StartPlatformPlayBackCBRaw
    * description: 开始平台录像回放(裸码流回调)
    * input      : iSessionID           登录成功后的会话ID
                   pMediaPara           媒体参数
                   pCameraCode          摄像机编码
                   pTimeSpan            回放时间段
                   fSpeed               播放速率
                   fPlayBackCallBackRaw 录像回放裸码流回调函数
                   pUserData            用户数据
    * output     : pHandle              播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackCBRaw(IVS_INT32 iSessionID,
																		const IVS_CHAR* pCameraCode,
																		const IVS_PLAYBACK_PARAM* pPlaybackParam,
																		PlayBackCallBackRaw fPlayBackCallBackRaw, 
																		void* pUserData,
																		IVS_ULONG* pHandle);


	/**************************************************************************
    * name       : IVS_SDK_StartPlatformPlayBackCBFrame
    * description: 开始平台录像回放(帧码流回调)
    * input      : iSessionID             登录成功后的会话ID
                   pMediaPara             媒体参数
                   pCameraCode            摄像机编码
                   pTimeSpan              回放时间段
                   fSpeed               播放速率

                   fPlayBackCallBackFrame 录像回放解码后帧码流回调函数
                   pUserData              用户数据
    * output     : pHandle                播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackCBFrame(IVS_INT32 iSessionID,
		   																  const IVS_CHAR* pCameraCode,
																		  const IVS_PLAYBACK_PARAM* pPlaybackParam,
																		  PlayBackCallBackFrame fPlayBackCallBackFrame, 
																		  void* pUserData,
																		  IVS_ULONG* pHandle);


    /******************************************************************
     function   : IVS_SDK_PlayBackPause
     description: 暂停回放
     input      : IVS_ULONG ulPlayHandle
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackPause(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);

    /******************************************************************
     function   : IVS_SDK_PlayBackResume
     description: 恢复回放
     input      : IVS_ULONG ulPlayHandle
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackResume(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);

	/******************************************************************
	 function   : IVS_SDK_GetPlayBackStatus
	 description: 获取播放状态
	 input      : IVS_INT32 iSessionID
	 input      : IVS_ULONG ulPlayHandle
	 output     : IVS_BOOL * pbPause
	 return     : IVS_SDK_API IVS_INT32 __SDK_CALL
	*******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPlayBackStatus(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_BOOL* pbPause);

    /******************************************************************
     function   : IVS_SDK_PlayBackFrameStepForward
     description: 单帧快进
     input      : IVS_ULONG ulPlayHandle
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackFrameStepForward(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);


	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackFrameStepBackward(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);

    /******************************************************************
     function   : IVS_SDK_SetPlayBackSpeed
     description: 设置回放速度
     input      : IVS_ULONG ulPlayHandle
     input      : IVS_FLOAT fSpeed
     output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackSpeed(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_FLOAT fSpeed);

	/******************************************************************
	 function   : IVS_SDK_GetPlayBackSpeed
	 description: 获取回放速度
	 input      : IVS_ULONG ulPlayHandle	播放句柄;
	 output     : IVS_FLOAT * pfSpeed		当前播放速度;
	 return     : IVS_INT32
	*******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPlayBackSpeed(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_FLOAT* pfSpeed);

    /******************************************************************
     function   : IVS_SDK_SetPlayBackTime
     description: 回放拖动
     input      : IVS_ULONG ulPlayHandle
     input      : IVS_UINT32 uiTime
     output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackTime(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_UINT32 uiTime);

    /******************************************************************
     function   : IVS_SDK_GetPlayBackTime
     description: 获取当前回放时间
     input      : IVS_ULONG ulPlayHandle
     input      : IVS_PLAYBACK_TIME * pPlayBackTime
     output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPlayBackTime(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_PLAYBACK_TIME* pPlayBackTime);
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCurrentFrameTick(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_UINT64* pTick);

	/******************************************************************
     function   : IVS_SDK_StartPlatformDownload
     description: 开始平台录像下载
     input      : IVS_ULONG ulPlayHandle
     input      : IVS_PLAYBACK_TIME * pPlayBackTime
	 input      : IVS_CHAR* pCameraCode
	 input      : IVS_DOWNLOAD_PARAM* pDownloadParam
	 input      : IVS_ULONG*  pulHandle
     output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformDownload(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode,
		const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pulHandle);

	/******************************************************************
     function   : IVS_SDK_StopPlatformDownload
     description: 停止平台录像下载
     input      : IVS_INT32 iSessionID
	 input      : IVS_ULONG ulHandle
	 output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformDownload(IVS_INT32 iSessionID, IVS_ULONG ulHandle);

    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartDisasterRecoveryDownload (
        IVS_INT32 iSessionID,
        const IVS_CHAR* pCameraCode,
        const IVS_DOWNLOAD_PARAM* pDownloadParam,
        IVS_ULONG* pulHandle);

    IVS_SDK_API IVS_INT32 __SDK_CALL  IVS_SDK_StopDisasterRecoveryDownload (
        IVS_INT32 iSessionID,
        IVS_ULONG ulHandle);

	/******************************************************************
     function   : IVS_SDK_StartPUDownload
     description: 开始前端录像下载
     input      : IVS_INT32 iSessionID
	 input      : IVS_CHAR* pCameraCode
	 input      : IVS_DOWNLOAD_PARAM* pDownloadParam
	 input      : IVS_ULONG* pulHandle
	 output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPUDownload(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode,
					const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pulHandle);

	/******************************************************************
     function   : IVS_SDK_StopPUDownload
     description: 停止平台录像下载
     input      : IVS_INT32 iSessionID
	 input      : IVS_ULONG ulHandle
	 output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPUDownload(IVS_INT32 iSessionID, IVS_ULONG ulHandle);

	/******************************************************************
     function   : IVS_SDK_DownloadPause
     description: 暂停录像下载
     input      : IVS_INT32 iSessionID
	 input      : IVS_ULONG ulHandle
	 output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DownloadPause(IVS_INT32 iSessionID, IVS_ULONG ulHandle);

	/******************************************************************
     function   : IVS_SDK_DownloadResume
     description: 恢复录像下载
     input      : IVS_INT32 iSessionID
	 input      : IVS_ULONG ulHandle
	 output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DownloadResume(IVS_INT32 iSessionID,IVS_ULONG ulHandle);

	/******************************************************************
     function   : IVS_SDK_GetDownloadInfo
     description: 获取下载信息
     input      : IVS_INT32 iSessionID
	 input      : IVS_ULONG ulHandle
	 output     : pDownloadInfo
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDownloadInfo(IVS_INT32 iSessionID, 
		              IVS_ULONG ulHandle, IVS_DOWNLOAD_INFO* pDownloadInfo);

	/**************************************************************************
    * name       : 
    * description: 开始客户端前端录像回放
    * input      : iSessionID      会话ID
				   pCameraCode     摄像机编码
                   pPlaybackParam  回放参数
				   hWnd            窗口句柄
                   pHandle         回放句柄
    * output     : NA
    * return     : 成功返回播放句柄，失败返回负数错误码
    * remark     : NA
    **************************************************************************/
#ifdef WIN32
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPUPlayBack(
                                    IVS_INT32 iSessionID,
                                    const IVS_CHAR* pCameraCode,
                                    const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                    HWND hWnd,
                                    IVS_ULONG* pHandle);
#endif
   /**************************************************************************
    * name       : 
    * description: 停止客户端前端录像回放
    * input      : iSessionID      会话ID
				   ulPlayHandle    播放句柄
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPUPlayBack(IVS_INT32 iSessionID,
                                                       IVS_ULONG ulPlayHandle);

    /**************************************************************************
    * name       : IVS_SDK_ModifyDeviceGroup
    * description: 修改设备组
    * input      : iSessionID         登录成功后的会话ID
                   pDevGroupInfo      设备组
    * return     : 成功返回0；失败返回负数错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyDeviceGroupName(IVS_INT32 iSessionID, 
                                                        const IVS_CHAR* pDomainCode,
                                                        IVS_UINT32 uiDevGroupID,
                                                        const IVS_CHAR* pNewDevGroupName);

	/*************************************************************************
	* name       : IVS_SDK_MoveDeviceGroup
	* description: 移动设备组下设备 
	* input      : iSessionID 登录成功后的会话ID, const IVS_CHAR* pReq, IVS_CHAR* &pRsp
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_MoveDeviceGroup(IVS_INT32 iSessionID, 
                                                 const IVS_CHAR* pDomainCode,
                                                 IVS_UINT32 uiNewParentDevGroupID,
                                                 IVS_UINT32 uiDevGroupID);

    /**************************************************************************
    * name       : IVS_SDK_GetDecodeAbility
    * description: 获取解码能力
    * input      : NA
    * output     : NA
    * return     : VIDEO_DECODE_MODE
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_BOOL __SDK_CALL IVS_SDK_GetGPUDecodeAbility();

    /**************************************************************************
    * name       : IVS_SDK_EnableGPU
    * description: 开启/关闭GPU加速
    * input      : bEnable：硬解标志。开启:TRUE;关闭:FALSE;
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_VOID __SDK_CALL IVS_SDK_EnableGPU(IVS_BOOL bEnable);
	
	/**************************************************************************
    * name       : IVS_SDK_GetMediaInfo
    * description: 获取媒体信息
    * input      : iSessionID        登录成功后的会话ID
                   ulHandle           业务句柄，包括实况、回放
    * output     : pMediaInfo        媒体信息
    * return     : 成功返回0；失败返回负数错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetMediaInfo(IVS_INT32 iSessionID, IVS_ULONG ulHandle, IVS_MEDIA_INFO* pMediaInfo);

	/**************************************************************************
    * name       : IVS_SDK_GetPlayedTime
    * description: 获取已播放时间
    * input      : iSessionID        登录成功后的会话ID
                   ulHandle           业务句柄，包括实况、回放
    * output     : puPlayedTime       当前播放时间(毫秒)
    * return     : 成功返回0；失败返回负数错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPlayedTime(IVS_INT32 iSessionID, IVS_ULONG ulHandle, IVS_UINT32* puPlayedTime);

	/**************************************************************************
    * name       : IVS_SDK_SetPlayQualityMode
    * description: 设置实况播放模式：清晰、流畅
    * input      : iSessionID        登录成功后的会话ID
                   ulHandle          业务句柄，包括实况、回放
				   iPlayQualityMode  播放模式 参考：IVS_PLAY_QUALITY_MODE
    * output     : NA
    * return     : 成功返回0；失败返回负数错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetPlayQualityMode(IVS_INT32 iSessionID, IVS_ULONG ulHandle,IVS_INT32 iPlayQualityMode);

	/**************************************************************************
	* name       : IVS_SDK_GetVideoNumAndResource
	* description: 查询MU的资源预算是否达到上限及当前摄像机视频流是否被引用
	* input      : iSessionID        登录成功后的会话ID
				   pCameraCode       摄像机编码
	* output     : bReachLimit       是否达到上限：0-未达到 1-已达到
				   bWhetherCited     是否被引用：0-没有引用 1-有被引用
	* return     : 成功返回0；失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetVideoNumAndResource (IVS_INT32 iSessionID, 
																	 const IVS_CHAR* pCameraCode,
																	 IVS_BOOL* bReachLimit,
																	 IVS_BOOL* bWhetherCited);

	/**************************************************************************
	* name       : IVS_SDK_SetWaterMarkVerify
	* description: 设置开启/停止校验水印
	* input      : iSessionID        登录成功后的会话ID
				   ulHandle          业务句柄，包括实况、回放
				   bIsStart          1：开启水印 0：停止水印
	* output     : NA
	* return     : 成功返回0；失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetWaterMarkVerify(IVS_INT32 iSessionID, IVS_ULONG ulHandle,IVS_BOOL bIsStart);

	/**************************************************************************
	* name       : IVS_SDK_WatermarkAlarmConfirm
	* description: 水印告警确认
	* input      : iSessionID        登录成功后的会话ID
				   pConfirmInfo     水印告警确认信息
	* output     : NA
	* return     : 成功返回0；失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_WatermarkAlarmConfirm (IVS_INT32 iSessionID, const IVS_WATERMARK_ALARM_CONFIRM* pConfirmInfo);

	/**************************************************************************
	* name       : IVS_SDK_QueryWatermarkAlarmList
	* description: 水印告警查询
	* input      : iSessionID       登录成功后的会话ID
				   pQueryFormat     水印告警确认信息
	* output     : NA
	* return     : 成功返回0；失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryWatermarkAlarmList(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat, 
																			IVS_VOID* pBuffer,IVS_UINT32 uiBufferSize);


	/**************************************************************************
	* name       : IVS_SDK_WatermarkAlarmCancel
	* description: 水印告警撤销
	* input      : iSessionID       登录成功后的会话ID
				   pCancelInfo      水印告警撤销信息
	* output     : NA
	* return     : 成功返回0；失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_WatermarkAlarmCancel(IVS_INT32 iSessionID, const IVS_WATERMARK_ALARM_CANCEL* pCancelInfo);

	/**************************************************************************
	* name       : IVS_SDK_SetDisplayScale
	* description: 设置播放显示比例
	* input      : iSessionID        登录成功后的会话ID
				   ulHandle          业务句柄，包括实况、回放
				   iDisplayScale     显示比例值，1-原始比例显示 2-满屏显示 
	* output     : NA
	* return     : 成功返回0；失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDisplayScale(IVS_INT32 iSessionID, IVS_ULONG ulHandle,IVS_INT32 iDisplayScale);

	/**************************************************************************
    * name       : IVS_SDK_GetNVRList
    * description: 查询网元列表
    * input      : iSessionID        登录成功后的会话ID
                   pDomainCode       域代码
				   uiServerType      服务类型
				   pNvrList          网元类型列表结构指针
				   uiBufferSize      结构指针大小
    * output     : pMediaInfo        
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetNVRList(IVS_INT32 iSessionID, 
		                                                const IVS_CHAR* pDomainCode,
		                                                IVS_UINT32 uiServerType,
		                                                const IVS_INDEX_RANGE* pIndexRange,
		                                                IVS_DEVICE_BRIEF_INFO_LIST* pNvrList,
		                                                IVS_UINT32 uiBufferSize);


    /******************************************************************
    function : IVS_SDK_DeleteDeviceFromGroup
    description: 删除设备组的摄像机
    input : IVS_INT32 iSessionID
			const IVS_CHAR * pTargetDomainCode
			IVS_UINT32 uiDeviceNum
			const IVS_DEV_GROUP_CAMERA * pGroupCameraList
    output : NA
    return : 成功返回0；失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteDeviceFromGroup( IVS_INT32 iSessionID, 
                                                                    const IVS_CHAR* pTargetDomainCode,
                                                                    IVS_UINT32 uiDeviceNum,
                                                                    const IVS_DEV_GROUP_CAMERA* pGroupCameraList);



	/**************************************************************************
    * name       : IVS_SDK_SetDeviceConfig
    * description: 更改主设备，子设备参数
    * input      : iSessionID        登录成功后的会话ID
                   pDomainCode       域代码
				   uiConfigType      配置设备类型
				   pDevCode          设备代码
                   pBuffer           参数结构体指针
				   uiBufferSize      参数结构体大小
    * output     : NA        
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDeviceConfig(IVS_INT32 iSessionID,
                                                             const IVS_CHAR* pDevCode, 
                                                             IVS_UINT32 uiConfigType,
                                                             IVS_VOID* pBuffer,
                                                             IVS_UINT32 uiBufSize);

	/**************************************************************************
    * name       : IVS_SDK_GetDeviceConfig
    * description: 更改主设备，子设备参数
    * input      : iSessionID        登录成功后的会话ID
                   pDomainCode       域代码
				   uiConfigType      配置设备类型
				   pDevCode          设备代码
                   pBuffer           返回参数结构体指针
				   uiBufferSize      参数结构体大小
    * output     : NA       
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceConfig(IVS_INT32 iSessionID,
                                                             const IVS_CHAR* pDevCode,
                                                             IVS_UINT32 uiConfigType,
                                                             IVS_VOID* pBuffer,
                                                             IVS_UINT32 uiBufSize);


    /******************************************************************
    function : IVS_SDK_GetChannelList
    description: 根据主设备查询子设备列表
    input : IVS_INT32 iSessionID
             const IVS_CHAR * pDevCode 
             IVS_UINT32 uiBufferSize
    output : IVS_DEV_CHANNEL_BRIEF_INFO_LIST * pChannelList
    return : IVS_SDK_API IVS_INT32
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetChannelList(IVS_INT32 iSessionID, 
                                                 const IVS_CHAR* pDevCode,
                                                 IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList,
                                                 IVS_UINT32 uiBufferSize);

	/**************************************************************************
    * name       : IVS_SDK_GetAlarmLevel
    * description: 查询告警级别详情
    * input      : iSessionID        登录成功后的会话ID
                   uiAlarmLevelID    告警级别ID
    * output     : pAlarmLevel       告警级别详情 
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmLevel(IVS_INT32 iSessionID,IVS_UINT32 uiAlarmLevelID,IVS_ALARM_LEVEL* pAlarmLevel);

	/**************************************************************************
    * name       : IVS_SDK_Aes256CbcEncode
    * description: AES256加密
    * input      : pszPlain        明文     
                      ullLenCipher 分配给pszCipher的空间大小
    * output     : pszCipher      密文 
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
	**************************************************************************/
   IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_Aes256CbcEncode(const IVS_CHAR* pszPlain, IVS_CHAR* pszCipher, IVS_UINT64 ullLenCipher);
   
	/**************************************************************************
    * name       : IVS_SDK_Aes256CbcEncode
    * description: AES256解密
    * input      : pszCipher      密文      ullLenCipher 分配给pszPlain的空间大小
    * output     :   pszPlain        明文  
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
	**************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_Aes256CbcDecode(const IVS_CHAR* pszCipher, IVS_CHAR* pszPlain, IVS_UINT64 ullLenPlain);

  /**************************************************************************
    * name       : IVS_SDK_StartPlatformRecord
    * description: 开始平台录像
    * input      : iSessionID      登录成功后的会话ID
	               pCameraCode     开始平台录像的摄像机编码
				   iRecordTime     录像时长
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
	**************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformRecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime);

  /**************************************************************************
    * name       : IVS_SDK_StopPlatformRecord
    * description: 停止平台录像
    * input      : iSessionID      登录成功后的会话ID
	               pCameraCode     停止平台录像的摄像机编码
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
  **************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformRecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode);

  /**************************************************************************
  * name       : IVS_SDK_StartPURecord(
  * description: 开始前端录像
  * input      : iSessionID      登录成功后的会话ID
  pCameraCode     开始前端录像的摄像机编码
  iRecordTime     录像时长
  * return     : 成功返回0；失败返回错误码
  * remark     : NA
  **************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPURecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime);

  /**************************************************************************
    * name       : IVS_SDK_StopPURecord
    * description: 停止前端录像
    * input      : iSessionID      登录成功后的会话ID
	               pCameraCode     停止平台录像的摄像机编码
    * return     : 成功返回0；失败返回错误码
    * remark     : NA
  **************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPURecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode);

  /**************************************************************************
   * name       : IVS_SDK_StartLocalRecord
   * description: 开始本地录像
   * input      : iSessionID     登录成功后的会话ID
	              pRecordParam   本地录像参数
				  ulPlayHandle   播放句柄
				  pSaveFileName  保存文件名
   * return     : 成功返回0；失败返回错误码
   * remark     : NA
   **************************************************************************/
   IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartLocalRecord(IVS_INT32 iSessionID,
	                                                         const IVS_LOCAL_RECORD_PARAM* pRecordParam, 
                                                             IVS_ULONG ulPlayHandle, 
                                                             const IVS_CHAR* pSaveFileName);

  /**************************************************************************
   * name       : IVS_SDK_StopLocalRecord
   * description: 停止本地录像
   * input      : iSessionID      登录成功后的会话ID
	              ulPlayHandle    播放句柄
   * return     : 成功返回0； 失败返回错误码
   * remark     : NA
   **************************************************************************/
   IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopLocalRecord(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);

  /******************************************************************
  function : IVS_SDK_GetDomainRoute
  description: 查询域路由信息
  input : IVS_INT32 iSessionID
          IVS_UINT32 uiBufferSize
  output : IVS_DOMAIN_ROUTE_LIST * pDomainRouteList
  return : 
  *******************************************************************/  
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDomainRoute( IVS_INT32 iSessionID, 
                          IVS_DOMAIN_ROUTE_LIST* pDomainRouteList,
                          IVS_UINT32 uiBufferSize);

  /******************************************************************
  function : IVS_SDK_ReleaseBuffer
  description: 释放SDK内部申请的内存
  input : IVS_INT32 iSessionID
  output : NA
  return : 成功返回0；失败返回错误码
  *******************************************************************/  
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ReleaseBuffer(IVS_CHAR *pBuffer);



  /******************************************************************
  function : IVS_SDK_IA
  description: 智能分析统一透传接口
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  IVS_UINT32		uiInterfaceType 接口类型；
		  const IVS_CHAR*	pReqXml			请求CBB::CBB::CXml；
  output : IVS_CHAR**		pRspXml			响应CBB::CBB::CXml
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_IA(IVS_INT32 iSessionID, IVS_UINT32 uiInterfaceType, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

  /******************************************************************
  function : IVS_SDK_IA_EX
  description: 智能分析统一透传接口
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  IVS_UINT32		uiInterfaceType 接口类型；
		  const IVS_CHAR*	pReqXml			请求CBB::CBB::CXml；
		  const IVS_CHAR*   pTransID		请求消息号
  output : IVS_CHAR**		pRspXml			响应CBB::CBB::CXml
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_IA_EX(IVS_INT32 iSessionID, IVS_UINT32 uiInterfaceType, IVS_CHAR* pTransID, 
												 IVS_UINT32 uiTransIDLen, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

  /******************************************************************
  function : IVS_SDK_SetDrawCallBack
  description: 设置画图显示的回调接口
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  IVS_ULONG			ulPlayHandle	播放句柄；
		  DRAW_PROC 		callback		画图回调函数指针；
		  void*·			pUserData		用户数据指针
  output : NA
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDrawCallBack(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, DRAW_PROC callback, void *pUserData);
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDrawCallBackEx(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd, DRAW_PROC callback, void *pUserData);
   
  /******************************************************************
  function : IVS_SDK_SetDelayFrameNum
  description: 设置延时播放帧数
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  IVS_ULONG			ulPlayHandle	播放句柄；
		  IVS_UINT32 		uDelayFrameNum	延时帧数；
  output : NA
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDelayFrameNum(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_UINT32 uDelayFrameNum);

  /******************************************************************
  function   : IVS_SDK_GetRecordStatus
  description: 云镜控制
  input      : iSessionID 登录成功后返回的会话ID
               pCameraCode 摄像机编码
			   uiRecordMethod 录像方式
  output     : NA
  return     : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordStatus(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_UINT32 uiRecordMethod,IVS_UINT32* pRecordState);

  /******************************************************************
  function   : IVS_SDK_GetPTZAbsPosition
  description: 获取云台当前位置
  input      : iSessionID 登录成功后返回的会话ID
			   pDomainCode 域编码
               pCameraCode 摄像机编码
  output     : pPTZPosInfo 云台位置信息
  return     : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPTZAbsPosition(IVS_INT32 iSessionID,
	  const IVS_CHAR* pDomainCode, const IVS_CHAR* pCameraCode, IVS_PTZ_ABSPOSITION *pPtzAbsPosition);

  /******************************************************************
  function   : IVS_SDK_PtzControl
  description: 云镜控制
  input      : iSessionID 登录成功后返回的会话ID
               pCameraCode 摄像机编码
			   iControlCode 云台控制码
			   pControlPara1 参数1
			   pControlPara2 参数2
  output     : NA
  return     : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PtzControl(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode, IVS_INT32 iControlCode,const IVS_CHAR* pControlPara1,const IVS_CHAR* pControlPara2, IVS_UINT32* pLockStatus);

  /******************************************************************
  function   : IVS_SDK_PtzControlWithLockerInfo
  description: 云镜控制
  input      : iSessionID 登录成功后返回的会话ID
               pCameraCode 摄像机编码
			   iControlCode 云台控制码
			   pControlPara1 参数1
			   pControlPara2 参数2
  output     : pPtzControlInfo 云台控制信息
  return     : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PtzControlWithLockerInfo(IVS_INT32 iSessionID,
															  const IVS_CHAR* pCameraCode, 
															  IVS_INT32 iControlCode,
															  const IVS_CHAR* pControlPara1,
															  const IVS_CHAR* pControlPara2, 
															  IVS_PTZ_CONTROL_INFO* pPtzControlInfo);

	/*************************************************************************
	* name       : IVS_SDK_SetMainDeviceName
	* description: 修改主设备名称
	* input      : iSessionID 登录成功后的会话ID, 
				   pDevCode 主设备编码
				   pNewDevName 主设备新的名称
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：fengjiaji f00152768
	* remark     : NA
	* history    : 2012-12-14 11:12[add]
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetMainDeviceName(	IVS_INT32 iSessionID,
																const IVS_CHAR* pDevCode,
																const IVS_CHAR* pNewDevName);


	/*************************************************************************
	* name       : IVS_SDK_SetCameraName
	* description: 修改摄像机名称
	* input      : iSessionID 登录成功后的会话ID, 
				   pCameraCode 摄像机编码
				   pNewCameraName 摄像机新的名称
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：fengjiaji f00152768
	* remark     : NA
	* history    : 2012-12-14 11:21[add]
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetCameraName(	IVS_INT32 iSessionID,
															const IVS_CHAR* pCameraCode,
															const IVS_CHAR* pNewCameraName);
	
    /*************************************************************************
	* name       : IVS_SDK_PlaySound
	* description: 播放随路语音
	* input      : iSessionID   登录成功后的会话ID, 
				   uiPlayHandle 播放句柄
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2012-12-20[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlaySound(IVS_INT32 iSessionID, IVS_UINT32 uiPlayHandle);

    /*************************************************************************
	* name       : IVS_SDK_StopSound
	* description: 停止播放随路语音
	* input      : iSessionID   登录成功后的会话ID, 
				   uiPlayHandle 播放句柄
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2012-12-20[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopSound(IVS_INT32 iSessionID, IVS_UINT32 uiPlayHandle);


    /*************************************************************************
	* name       : IVS_SDK_SetVolume
	* description: 设置音量
	* input      : iSessionID   登录成功后的会话ID, 
				   uiPlayHandle 播放句柄
                   uiVolumeValue音量
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2012-12-25[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetVolume(IVS_INT32 iSessionID,
                                                       IVS_UINT32 uiPlayHandle, 
                                                       IVS_UINT32 uiVolumeValue);


    /*************************************************************************
	* name       : IVS_SDK_GetVolume
	* description: 获取音量
	* input      : iSessionID   登录成功后的会话ID, 
				   uiPlayHandle 播放句柄
	* output     : uiVolumeValue音量
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2012-12-25[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetVolume(IVS_INT32 iSessionID,
                                                       IVS_UINT32 uiPlayHandle, 
                                                       IVS_UINT32* puiVolumeValue);

	/*************************************************************************
	* name       : IVS_SDK_SetUserData
	* description: 备份用户数据
	* input      : iSessionID 登录成功后的会话ID, 
				   pFileName 用户数据压缩文件
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：tanjibao
	* remark     : NA
	* history    : 2012-12-25
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetUserData (IVS_INT32 iSessionID, const IVS_CHAR* pFileName);

	/*************************************************************************
	* name       : IVS_SDK_GetUserData
	* description: 恢复用户数据
	* input      : iSessionID 登录成功后的会话ID, 
				   pFileName 用户数据压缩文件
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：tanjibao
	* remark     : NA
	* history    : 2012-12-25
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserData (IVS_INT32 iSessionID, const IVS_CHAR* pFileName);


	/**************************************************************************
    * name       : IVS_SDK_GetDeviceUserList
    * description: 获取拥有指定设备权限的用户列表
    * input      : iSessionID 登录成功后的会话ID
				   pDevCode   设备编码
				   pIndexRange 分页信息
				   pUserDevList 用户设备列表结构
				   uiBufferSize 缓存大小
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhougang zWX155350
	* remark     : NA
	* history    : 2012-12-27
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL  IVS_SDK_GetDeviceUserList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, IVS_UINT32 uiIsUserOnline, const IVS_INDEX_RANGE* pIndexRange, IVS_DEVICE_USER_INFO_LIST* pDevUserList, IVS_UINT32 uiBufferSize);


	/**************************************************************************
    * name       : IVS_SDK_RebootDevice
    * description: 重启前段设备
    * input      : iSessionID 登录成功后的会话ID
				   pDeviceCode  主设备编码
				   uiRebootType 重启类型：0-重启 1-恢复默认配置并重启
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：tanjibao tkf71032
	* remark     : NA
	* history    : 2013-1-6
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_RebootDevice (IVS_INT32 iSessionID, const IVS_CHAR* pDeviceCode, IVS_UINT32 uiRebootType);

	/**************************************************************************
    * name       : IVS_SDK_GetConfigInfo
    * description: 获取平台配置信息
    * input      : iSessionID 登录成功后的会话ID
				   pReqXml  请求信息
				   pRspXml  返回信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhougang zWX155350
	* remark     : NA
	* history    : 2013-1-6
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetConfigInfo (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

		/**************************************************************************
    * name       : IVS_SDK_GetAbilityInfo 
    * description: 查询平台能力集信息
    * input      : iSessionID 登录成功后的会话ID 
	               uiAbility  平台支持的能力值，与或值表示
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：xusida WX168257
	* remark     : NA
	* history    : 2013-1-6
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAbilityInfo  (IVS_INT32 iSessionID,IVS_UINT32* uiAbility);
	/**************************************************************************
    * name       : IVS_SDK_LocalSnapshot
    * description: 本地抓拍单张图片
    * input      : iSessionID 登录成功后的会话ID
				   ulPlayHandle  播放句柄
				   uiPictureFormat  图片格式
				   pFileName	图片文件名称
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhangshufan z00227377
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LocalSnapshot(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_UINT32 uiPictureFormat, const IVS_CHAR* pFileName);
	
	/**************************************************************************
    * name       : IVS_SDK_LocalSnapshotRect
    * description: 本地局部放大抓拍单张图片
    * input      : iSessionID 登录成功后的会话ID
				   ulPlayHandle  播放句柄
				   uiPictureFormat  图片格式
				   pFileName	图片文件名称
				   pRect		局部放大范围
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhangshufan z00227377
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LocalSnapshotRect(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, 
																IVS_UINT32 uiPictureFormat, const IVS_CHAR* pFileName, const IVS_RECT* pRect);

	/**************************************************************************
    * name       : IVS_SDK_GetLogo
    * description: Logo检验
    * input      : iSessionID 登录成功后的会话ID
				   uiLogoType  客户端的Logo的类型: 0-左上角 1-登录/关于
				   pLogoFile  Logo文件全路径
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：z00201790
	* remark     : NA
	* history    : 2013-1-23
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetLogo (IVS_INT32 iSessionID, IVS_UINT32 uiLogoType, const IVS_CHAR* pLogoFile);

	/**************************************************************************
    * name       : IVS_SDK_UploadLogo
    * description: Logo下载
    * input      : iSessionID 登录成功后的会话ID
				   uiLogoType  客户端的Logo的类型: 0-左上角 1-登录/关于 2-移动PAD 3-移动Phone 4-BS左上角 5BS登录/关于
				   pLogoFile  Logo文件全路径
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：z00201790
	* remark     : NA
	* history    : 2013-1-23
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UploadLogo(IVS_INT32 iSessionID, IVS_UINT32 uiLogoType, const IVS_CHAR* pLogoFile);

	/**************************************************************************
    * name       : IVS_SDK_GetStatement
    * description: 下载声明信息
    * input      : iSessionID 登录成功后的会话ID
				   pLanguageType 语言类型  
	* output     : pStatement  声明信息
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：z00201790
	* remark     : NA
	* history    : 2013-1-23
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetStatement(IVS_INT32 iSessionID,const IVS_CHAR* pLanguageType,IVS_CHAR** pStatement);

	/**************************************************************************
    * name       : IVS_SDK_UploadStatement
    * description: 上传声明信息
    * input      : iSessionID 登录成功后的会话ID
				   pLanguageType 语言类型  
				   pStatementFile  声明信息文件
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：z00201790
	* remark     : NA
	* history    : 2013-1-23
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UploadStatement(IVS_INT32 iSessionID,const IVS_CHAR* pLanguageType,const IVS_CHAR* pStatementFile);

	/**************************************************************************
    * name       : IVS_SDK_AddPTZPreset
    * description: 增加预置位
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pPTZPreset  预置位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddPTZPreset (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CHAR* pPresetName, IVS_UINT32* uiPresetIndex);

	/**************************************************************************
    * name       : IVS_SDK_DelPTZPreset
    * description: 删除预置位
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   uiPresetIndex  预置位索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelPTZPreset (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex);

	/**************************************************************************
    * name       : IVS_SDK_ModPTZPreset
    * description: 修改预置位
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pPTZPreset  预置位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModPTZPreset (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_PTZ_PRESET* pPTZPreset);

	/**************************************************************************
    * name       : IVS_SDK_GetPTZPresetList
    * description: 查询预置位列表
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pIndexRange  索引范围
				   pPTZPresetList 预置位信息列表
				   uiBufferSize 缓存大小
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPTZPresetList (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiPTZPresetNum);

	/**************************************************************************
    * name       : IVS_SDK_SetGuardPos
    * description: 设置看守位
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pGuardPosInfo  看守位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetGuardPos (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_GUARD_POS_INFO* pGuardPosInfo);

	/**************************************************************************
    * name       : IVS_OCX_DelGuardPos
    * description: 删除看守位
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   uiPresetIndex  预置位索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelGuardPos (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex);

	/**************************************************************************
    * name       : IVS_SDK_GetGuardPos
    * description: 获取看守位
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pGuardPosInfo  看守位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetGuardPos (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo);

	/**************************************************************************
    * name       : IVS_SDK_AddCruiseTrack
    * description: 增加巡航轨迹
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pCuriseTrackInfo  巡航轨迹信息
				   uiTrackIndex 轨迹索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, IVS_UINT32* uiTrackIndex);

	/**************************************************************************
    * name       : IVS_SDK_DelCruiseTrack
    * description: 删除巡航轨迹
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   uiTrackIndex  轨迹索引
				   uiCruiseType 轨迹类型
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiTrackIndex, IVS_UINT32 uiCruiseType);

	/**************************************************************************
    * name       : IVS_SDK_ModCruiseTrack 
    * description: 修改巡航轨迹
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pCuriseTrackInfo  巡航轨迹信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo);

	/**************************************************************************
    * name       : IVS_SDK_GetCruiseTrackList
    * description: 查询巡航轨迹列表
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pCruiseTrackInfoList 巡航轨迹列表
				   uiBufferSize 缓存大小
				   uiCruiseTrackNum 实际查询到的巡航轨迹个数
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCruiseTrackList (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiCruiseTrackNum);

	/**************************************************************************
    * name       : IVS_SDK_GetCruiseTrack
    * description: 查询巡航轨迹信息
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   uiTrackIndex  轨迹索引
				   pCruiseTrackInfo 轨迹索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_INT32 uiTrackIndex, IVS_CRUISE_TRACK* pCruiseTrackInfo);

	/**************************************************************************
    * name       : IVS_SDK_SetCruisePlan
    * description: 设置巡航计划
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pCruisePlan  巡航计划
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetCruisePlan (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CRUISE_PLAN* pCruisePlan);

	/**************************************************************************
    * name       : IVS_SDK_GetCruisePlan
    * description: 查询巡航计划
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pCruisePlan  巡航计划
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCruisePlan (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_CRUISE_PLAN* pCruisePlan);
   /**************************************************************************
    * name       : IVS_SDK_GetDeviceAlarmList
    * description: 查询OMU告警事件列表
				   pUnifiedQuery  请求信息结构体
				   pAlarmEventList  响应消息结构体
    * author     ：wulixia wWX155351
	* history    : 2013-1-23
	/**************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceAlarmList(IVS_INT32 iSessionID, 
																const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
																IVS_DEVICE_ALARM_EVENT_LIST* pAlarmEventList,
																IVS_UINT32 uiBufferSize);

	/*****************************************************************
	     function   : IVS_SDK_SetCameraBackupConfig
	     description: 设置摄像机备份参数     
	     input      : IVS_INT32 iSessionID        登录成功后的会话ID
					  const IVS_CHAR * pBackupXml 录像备份参数XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetCameraBackupConfig(IVS_INT32 iSessionID, const IVS_CHAR* pBackupXml);

	/*****************************************************************
	     function   : IVS_SDK_GetCameraBackupConfig
	     description: 获取摄像机备份参数
	     output     : IVS_INT32 iSessionID       登录成功后的会话ID
					  const IVS_CHAR * pCameraCode 摄像机编码
	     output     : IVS_CHAR * * pBackConfigXml  备份参数xml
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCameraBackupConfig(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_CHAR** pBackConfigXml);

	/**************************************************************************
    * name       : IVS_SDK_PlatformSnapshot
    * description: 平台抓拍 
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng s00193168
	* remark     : NA
	* history    : 2013-1-24
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlatformSnapshot(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode);

	/**************************************************************************
    * name       : IVS_SDK_SetSnapshotPlan
    * description: 设置平台抓拍计划 
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pSnapShotPlan 抓拍计划
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng s00193168
	* remark     : NA
	* history    : 2013-1-24
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetSnapshotPlan(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan);

	/**************************************************************************
    * name       : IVS_SDK_GetSnapshotPlan 
    * description: 查询平台抓拍计划 
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pSnapShotPlan 抓拍计划
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng s00193168
	* remark     : NA
	* history    : 2013-1-24
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetSnapshotPlan(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_SNAPSHOT_PLAN* pSnapShotPlan);

	/*****************************************************************
	     function   : IVS_SDK_GetSnapshotList
	     description: 获取平台抓拍图片列表              
	     input     : IVS_INT32 iSessionID                           登录成功后的会话ID
		 input     :  const IVS_CHAR* pCameraCode
	                  const IVS_QUERY_SNAPSHOT_PARAM * pQueryParam   抓拍图片查询参数
	     output    : IVS_SNAPSHOT_INFO_LIST * pSnapshotList         抓拍图片列表信息
	     input     : IVS_UINT32 uiBufSize                           缓存大小
	     return    : 成功返回0；失败返回错误码   
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetSnapshotList(IVS_INT32 iSessionID,
															 const IVS_CHAR* pCameraCode,
															 const IVS_QUERY_SNAPSHOT_PARAM* pQueryParam,
															 IVS_SNAPSHOT_INFO_LIST* pSnapshotList,
															 IVS_UINT32 uiBufSize);

	/*****************************************************************
	     function   : IVS_SDK_GetExDomainList
	     description: 查询外域列表
	     intput     : IVS_INT32 iSessionID 登录成功后的会话ID
		 intput     : pQueryInfo 通用查询      
	     output     : pRspXml 外域列表信息XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetExDomainList (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo,IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_ShareCatalog
	     description: 目录共享
	     intput     : IVS_INT32 iSessionID         登录成功后的会话ID
	     output     : pShareInfo 目录共享信息XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ShareCatalog (IVS_INT32 iSessionID,
														   const IVS_CHAR* pShareInfo);

	/*****************************************************************
	     function   : IVS_SDK_GetShareCatalogList
	     description: 查询目录共享信息列表
	     intput     : IVS_INT32 iSessionID         登录成功后的会话ID
					  IVS_CHAR pQueryInfo		   查询条件
	     output     : pRspXml 目录共享信息列表XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetShareCatalogList (IVS_INT32 iSessionID,
																  const IVS_CHAR* pQueryInfo,
																  IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_StartSearchDevice
	     description: 启动搜索前端设备
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
					  const IVS_CHAR * pSearchInfo 设备搜索信息XML
	     output     : NA
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo);

	/*****************************************************************
	     function   : IVS_SDK_StopSearchDevice
	     description: 停止搜索前端设备
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
					  const IVS_CHAR * pSearchInfo 设备搜索信息XML
	     output     : NA
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo);

	/*****************************************************************
	     function   : IVS_SDK_VerifyDevice
	     description: 验证前端设备
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
					  const IVS_CHAR * pVerifyInfo 设备验证信息XML
		 output     : IVS_CHAR** pRspXml           验证结果XML 
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_VerifyDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_GetDevLog
	     description: 查询前端日志
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
					  const IVS_CHAR * pQueryXml   查询条件XML
		 output     : IVS_CHAR** pRspXml           验证结果XML 
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDevLog(IVS_INT32 iSessionID, const IVS_CHAR* pQueryXml, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_GetDomainList
	     description: 查询设备数列表
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
		 output     : IVS_CHAR** pDomainList       设备数列表XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDomainList(IVS_INT32 iSessionID, IVS_CHAR** pDomainList);

	/*****************************************************************
	     function   : IVS_SDK_GetCPUMemory
	     description: CPU/内存性能查询
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
					  const IVS_CHAR * pQueryXml   查询XML
		 output     : IVS_CHAR** pRspXml           响应XML 
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCPUMemory(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance);

    /*************************************************************************
	* name       : IVS_SDK_StartVoiceTalkback
	* description: 开始语音对讲
	* input      : iSessionID   登录成功后的会话ID
				   pTalkbackParam 语音对讲参数（注：目前只支持UDP方式）
                   pCameraCode 摄像机编码
	* output     : pHandle 对讲句柄
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartVoiceTalkback(
                                IVS_INT32 iSessionID,
                                const IVS_VOICE_TALKBACK_PARAM* pTalkbackParam,
                                const IVS_CHAR* pCameraCode,
                                IVS_ULONG* pHandle);

    /*************************************************************************
	* name       : IVS_SDK_StopVoiceTalkback
	* description: 停止语音对讲
	* input      : iSessionID   登录成功后的会话ID
				   ulHandle 对讲句柄
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopVoiceTalkback(
                                                          IVS_INT32 iSessionID,
                                                          IVS_ULONG ulHandle);

    /**************************************************************************
    * name       : IVS_SDK_SetMicVolume
    * description: 设置麦克风音量
    * input      : uiVolumeValue 音量值，范围0~100
    * output     ：NA
    * return     : 成功返回0；失败返回负数错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetMicVolume(IVS_UINT32 uiVolumeValue);

    /**************************************************************************
    * name       : IVS_SDK_GetMicVolume
    * description: 获取麦克风音量
    * input      : NA
    * output     ：NA
    * return     : 音量值，范围0~100
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_UINT32 __SDK_CALL IVS_SDK_GetMicVolume();

    /*************************************************************************
	* name       : IVS_SDK_StartRealBroadcast
	* description: 开始实时语音广播
	* input      : iSessionID   登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealBroadcast(IVS_INT32 iSessionID);

    /*************************************************************************
	* name       : IVS_SDK_StopRealBroadcast
	* description: 停止实时语音广播
	* input      : iSessionID   登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealBroadcast(IVS_INT32 iSessionID);

    /*************************************************************************
	* name       : IVS_SDK_StartFileBroadcast
	* description: 开始文件语音广播
	* input      : iSessionID 登录成功后的会话ID, 
				   pFileName  音频文件名,
                   bCycle     是否循环播放：0-不循环，1-循环
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartFileBroadcast(
                                                    IVS_INT32 iSessionID,
                                                    const IVS_CHAR* pFileName,
                                                    IVS_BOOL bCycle);

    /*************************************************************************
	* name       : IVS_SDK_StopFileBroadcast
	* description: 停止文件语音广播
	* input      : iSessionID   登录成功后的会话ID, 
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopFileBroadcast(IVS_INT32 iSessionID);

    /*************************************************************************
	* name       : IVS_SDK_AddBroadcastDevice
	* description: 添加广播设备
	* input      : iSessionID 登录成功后的会话ID, 
				   pCameraCode 摄像机编码
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddBroadcastDevice(
                                                   IVS_INT32 iSessionID,
                                                   const IVS_CHAR* pCameraCode);

    /*************************************************************************
	* name       : IVS_SDK_DeleteBroadcastDevice
	* description: 删除广播设备
	* input      : iSessionID 登录成功后的会话ID, 
				   pCameraCode 摄像机编码
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：zhouqiming z90003203
	* remark     : NA
	* history    : 2013-1-29[add]
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteBroadcastDevice (
                                                   IVS_INT32 iSessionID,
                                                   const IVS_CHAR* pCameraCode);


	/*****************************************************************
	     function   : IVS_SDK_DelSnapshotPlan
	     description: 删除平台抓拍机会
	     output     : IVS_INT32 iSessionID    登录成功后的会话ID
	     output     : const IVS_CHAR * pCameraCode  摄像机编码
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/	
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelSnapshotPlan(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode);

	/*****************************************************************
	     function   : IVS_SDK_AddRecordBackupPlan
	     description: 添加录像备份计划
	     input      : IVS_INT32 iSessionID   登录成功后的会话ID
					  const IVS_CHAR * pBackupPlan  添加录像备份计划XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan);


	/*****************************************************************
	     function   : IVS_SDK_ModifyRecordBackupPlan
	     description: 修改录像备份计划
	     input      : IVS_INT32 iSessionID   登录成功后的会话ID
			          const IVS_CHAR * pBackupPlan  修改录像备份计划XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan);

	/*****************************************************************
	     function   : IVS_SDK_DeleteRecordBackupPlan
	     description: 删除录像备份计划
	     input      : IVS_INT32 iSessionID   登录成功后的会话ID
	                  const IVS_CHAR * pBackupPlan  删除录像备份计划XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan);


	/*****************************************************************
	     function   : IVS_SDK_GetRecordBackupPlan
	     description: 查询录像备份计划
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
				      IVS_CHAR * pQueryInfo     查询录像备份计划XML
	     output     : IVS_CHAR * * pBackupPlan  录像备份计划返回信息XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pQueryInfo, IVS_CHAR** pBackupPlan);

	/*****************************************************************
	     function   : IVS_SDK_GetIACPUMemory
	     description: 智能分析CPU/内存性能查询
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
					  const IVS_CHAR * pQueryXml   查询XML
		 output     : IVS_CHAR** pRspXml           响应XML 
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetIACPUMemory(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance);

	/*****************************************************************
	     function   : IVS_SDK_QueryPerformance
	     description: 网元性能查询
	     output     : IVS_INT32 iSessionID         登录成功后的会话ID
					  const IVS_CHAR * pQueryXml   查询XML
		 output     : IVS_CHAR** pRspXml           响应XML 
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryPerformance(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance);
	
	/*****************************************************************
	     function   : IVS_SDK_GetRecordTask
	     description: 查询当前用户的手动录像任务
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     output     : IVS_CHAR * * pRspXml  查询当前用户的手动录像任务返回信息XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordTask(IVS_INT32 iSessionID, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_GetRecordBackupTaskList
	     description: 查询录像备份任务列表
	     input      : IVS_INT32 iSessionID 登录成功后的会话ID;
	     output     : IVS_CHAR * * pRspXml  查询录像备份任务列表返回信息XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordBackupTaskList(IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo,IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_StartRecordBackupTask
	     description: 启动/继续录像备份任务
	     input      : pQueryInfo
	     output     : NA
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL  IVS_SDK_StartRecordBackupTask (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo);

	/*****************************************************************
	     function   : IVS_SDK_StopRecordBackupTask
	     description: 暂停/停止录像备份任务
	     input      : pQueryInfo
	     output     : NA
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRecordBackupTask (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo);

	/*****************************************************************
	     function   : IVS_SDK_DeleteRecordBackupTask
	     description: 删除录像备份任务
	     input      : pQueryInfo
	     output     : NA
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRecordBackupTask (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo);

	/*****************************************************************
	     function   : IVS_SDK_AddRecordBackupTask
	     description: 新增录像备份任务
	     input      : IVS_INT32 iSessionID     登录成功后的会话ID
					  const IVS_CHAR * pBackupTask  新增录像备份任务xml
	     return     : 
	*****************************************************************/	
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRecordBackupTask(IVS_INT32 iSessionID, const IVS_CHAR* pBackupTask);

	/**************************************************************************
    * name       : IVS_SDK_StartRealPlayByIP
    * description: 开始实时浏览
    * input      : iSessionID 登录成功后的会话ID
				   pCameraCode  摄像机编码
				   pMediaAddrDst  媒体流目标地址
	* output     : pMediaAddrSrc 媒体流源地址
				   pHandle 播放句柄（唯一指定此播放通道）
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：gaoshuming g00209332
	* remark     : NA
	* history    : 2013-3-5
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayByIP(IVS_INT32 iSessionID, 
															const IVS_CHAR* pCameraCode,
															const IVS_MEDIA_ADDR* pMediaAddrDst,
															IVS_MEDIA_ADDR* pMediaAddrSrc,
															IVS_ULONG* pHandle);
	/**************************************************************************
    * name       : IVS_SDK_StopRealPlayByIP
    * description: 停止实时浏览
    * input      : iSessionID 登录成功后的会话ID
				   ulHandle 播放句柄（唯一指定此播放通道）
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：gaoshuming g00209332
	* remark     : NA
	* history    : 2013-3-5
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlayByIP(IVS_INT32 iSessionID, IVS_ULONG ulHandle);

	/**************************************************************************
    * name			: IVS_SDK_StartRealPlayByIPEx
    * description	: 开始实时浏览
    * input			: iSessionID 登录成功后的会话ID
						  pRealplayParam  播放参数
						  pCameraCode  摄像机编码
						  pMediaAddrDst  媒体流目标地址
	* output		: pMediaAddrSrc 媒体流源地址
						  pHandle 播放句柄（唯一指定此播放通道）
    * return		: 成功返回IVS_SUCCESSED，失败返回错误码
    * author		: zhangrui z00233055
	* remark		: NA
	* history		: 2013-12-24
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayByIPEx(IVS_INT32 iSessionID, 
															const IVS_REALPLAY_PARAM  *pRealplayParam,
															const IVS_CHAR* pCameraCode,
															const IVS_MEDIA_ADDR* pMediaAddrDst,
															IVS_MEDIA_ADDR* pMediaAddrSrc,
															IVS_ULONG* pHandle);

	/**************************************************************************
    * name			: IVS_SDK_StopRealPlayByIPEx
    * description	: 停止实时浏览
    * input			: iSessionID 登录成功后的会话ID
						  ulHandle 播放句柄（唯一指定此播放通道）
	* output		: NA
    * return		: 成功返回IVS_SUCCESSED，失败返回错误码
    * author		: zhangrui z00233055
	* remark		: NA
	* history		: 2013-12-24
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlayByIPEx(IVS_INT32 iSessionID, IVS_ULONG ulHandle);

	/*****************************************************************
	     function   : IVS_SDK_GetAlarmLinkageAction
	     description: 查询告警联动动作详情
	     input      : IVS_INT32 iSessionID     登录成功后的会话ID
					  const IVS_CHAR * pReqXml  请求xml
					  IVS_CHAR** pRsqXml  响应xml
	     return     : 
	*****************************************************************/	
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmLinkageAction(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRsqXml);

	/******************************************************************
    function : IVS_SDK_LockRecord
    description: 锁定录像
    input :  iSessionID                            登录成功后的会话ID
	input : pCameraCode                                     摄像机编码
	input :  pRecordLockInfo                                录像锁定信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LockRecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo);

	/******************************************************************
    function : IVS_SDK_ModLockRecord
    description: 修改录像锁定信息
    input :  iSessionID                            登录成功后的会话ID
	input : pCameraCode                                     摄像机编码
	input :  pRecordLockInfo                                录像锁定信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModLockRecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo);


	/******************************************************************
    function : IVS_SDK_UnlockRecord
    description: 解锁录像
    input :  iSessionID                            登录成功后的会话ID
	input : pCameraCode                                     摄像机编码
	input :  pRecordLockInfo                                录像锁定信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UnLockRecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_RECORD_LOCK_INFO* pRecordLockInfo);

	/******************************************************************
    function : IVS_SDK_LockBackupRecord
    description: 锁定录像(备份服务器)
    input :  iSessionID                            登录成功后的会话ID
	input : pCameraCode                                     摄像机编码
	input :  pRecordLockInfo                                录像锁定信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LockBackupRecord(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo);

	/******************************************************************
    function : IVS_SDK_UnlockBackupRecord
    description: 解锁录像(备份服务器)
    input :  iSessionID                            登录成功后的会话ID
	input : pCameraCode                                     摄像机编码
	input :  pRecordLockInfo                                录像锁定信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UnlockBackupRecord(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_RECORD_LOCK_INFO* pRecordLockInfo);

	/******************************************************************
    function : IVS_SDK_ModLockBackupRecord
    description: 修改录像锁定信息(备份服务器)
    input :  iSessionID                            登录成功后的会话ID
	input : pCameraCode                                     摄像机编码
	input :  pRecordLockInfo                                录像锁定信息
    return : 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModLockBackupRecord(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_RECORD_LOCK_INFO* pRecordLockInfo);


	/*************************************************************************
	* name       : IVS_SDK_GetMotionRangeDataPlatform
	* description: 查询平台运动检测幅度数据
	* input      : iSessionID 登录成功后的会话ID, 
				   pCameraCode 摄像机编码
				   pMotionRangeData 平台运动检测幅度数据结构体
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：songjianfeng
	* remark     : NA
	* history    : 2013-3-7 
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetMotionRangeDataPlatform(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,const IVS_TIME_SPAN* pTimeSpan,const IVS_INDEX_RANGE* pIndexRange,
		IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_UINT32 uiBufferSize);

	/*************************************************************************
	* name       : IVS_SDK_AddBookmarkForBackup
	* description: 新增录像书签(备份服务器)
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddBookmarkForBackup(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,
		const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime);

	/*************************************************************************
	* name       : IVS_SDK_ModifyBookmarkForBackup
	* description: 修改录像书签(备份服务器)
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyBookmarkForBackup(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName);
	/*************************************************************************
	* name       : IVS_SDK_DeleteBookmarkForBackup
	* description: 删除录像书签(备份服务器)
	* input      : iSessionID 登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteBookmarkForBackup(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID);

	/*************************************************************************
	* name       : IVS_SDK_GetBookmarkListForBackup
	* description: 查询录像书签列表(备份服务器)
	* input      : iSessionID 登录成功后的会话ID
	               pUnifiedQuery 通用查询结构
				   pBookmarkList 返回录像书签查询列表
				   uiBufSize 分配内存大小
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetBookmarkListForBackup(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
		IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiBufSize);

    /******************************************************************
    function : IVS_SDK_OMUQueryDevReport
    description: 实时报表查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryDevReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryDevReportDetail
    description: 实时报表详情查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryDevReportDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryRealtimeReport
    description: 实时报表查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRealtimeReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryRealtimeReportDetail
    description: 实时报表详情查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRealtimeReportDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryQDReport
    description: 视频质量诊断查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryQDReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryQDReportDetail
    description: 视频质量诊断详情查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryQDReportDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryRecordPlanReport
    description: 录像执行计划报表查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRecordPlanReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryRecordPlanReportDetail
    description: 录像执行计划报表详情查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRecordPlanReportDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryRecordIntegralityReport
    description: 录像完整性报表查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRecordIntegralityReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryRecordIntegralityReportDetail
    description: 录像完整性报表详情查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRecordIntegralityReportDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryPacketLossRateReport
    description: 丢包率报表查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryPacketLossRateReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryPacketLossRateReportDetail
    description: 丢包率报表详情查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryPacketLossRateReportDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

    /******************************************************************
    function : IVS_SDK_OMUQueryDiskUsageRateReport
    description: 磁盘存储占用率报表查询
    input : IVS_INT32 iSessionID        登录成功后的会话ID
    input : const IVS_CHAR * pReqXml    查询条件
    input : IVS_CHAR * * pRspXml        查询结果
    output : NA
    return : 成功返回0，失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryDiskUsageRateReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	 /******************************************************************
    function			: IVS_SDK_OMUQueryDiskProperty
    description	: 查询磁盘信息接口
    input				: iSessionID 登录成功后的会话ID
    output			: pRspXml 结果XML
	return				: 成功返回0，失败返回错误码
	author			: zhuohaofan zwx211831
    remark			: pRspXml是SDK内部申请内存，使用完应调用IVS_SDK_ReleaseBuffer释放。
    return				: NA
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryDiskProperty(IVS_INT32 iSessionID, 
		IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_GetPrerecordCameraList
	     description: 查询开启预录的摄像机列表
	     input      : IVS_INT32 iSessionID 登录成功后的会话ID;
	     output     : IVS_CHAR * * pRspXml  查询开启预录的摄像机列表返回信息XML
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPrerecordCameraList(IVS_INT32 iSessionID, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_SetDevGroupRemark
	     description: 设置设备组备注信息
	     input      : IVS_INT32 iSessionID 登录成功后的会话ID;
	     input      : const IVS_CHAR* pDomainCode  设备组所属域编码
		 input      : IVS_UINT32 uiDevGroupID  设备组ID
		 input      : const IVS_CHAR* pDevGroupRemark 设备组备注
		 output     : NA
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDevGroupRemark(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, const IVS_CHAR* pDevGroupRemark);

	/*****************************************************************
	     function   : IVS_SDK_GetDevGroupRemark
	     description: 查询设备组备注信息
	     input      : IVS_INT32 iSessionID 登录成功后的会话ID;
	     input      : const IVS_CHAR* pDomainCode  设备组所属域编码
		 input      : IVS_UINT32 uiDevGroupID  设备组ID
		 input      : IVS_CHAR* pDevGroupRemark 设备组备注
		 input      : IVS_UINT32 uiRemarkLen 备注字符长度（最大324字节）
		 output     : NA
	     return     : 成功返回0；失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDevGroupRemark(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, IVS_CHAR* pDevGroupRemark, IVS_UINT32 uiRemarkLen);

	/*****************************************************************
	     function   : IVS_SDK_OMUAddAlarmLinkageRule
	     description: 新增告警通知规则接口
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml  请求内容XML
	     output     : IVS_CHAR * * pRspXml      结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUAddAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_OMUModAlarmLinkageRule
	     description: 修改告警通知规则接口
	     input      : IVS_INT32 iSessionID       登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml   请求内容XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUModAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*****************************************************************
	     function   : IVS_SDK_OMUDelAlarmLinkageRule
	     description: 删除告警通知规则接口
	     input      : IVS_INT32 iSessionID        登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml    请求内容XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUDelAlarmLinkageRule(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

	/*****************************************************************
	     function   : IVS_SDK_OMUGetAlarmLinkageRuleList
	     description: 获取告警通知规则列表接口
	     input      : IVS_INT32 iSessionID       登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml   查询条件
	     output     : IVS_CHAR * * pRspXml       结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUGetAlarmLinkageRuleList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_OMUGetAlarmLinkageRuleDetail
	     description: 获取告警通知规则详情接口
	     input      : IVS_INT32 iSessionID       登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml   请求内容XML
	     output     : IVS_CHAR * * pRspXml        结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUGetAlarmLinkageRuleDetail(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_OMUGetDevTypeByNodeCode
	     description: 根据网元获取设备类型接口   
	     output     : IVS_INT32 iSessionID       登录成功后的会话ID
	     output     : const IVS_CHAR * pReqXml   请求内容XML
	     output     : IVS_CHAR * * pRspXml       结果XML
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUGetDevTypeByNodeCode(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : IVS_SDK_DeleteSnapshot
	     description: 删除平台抓拍图片
	     output     : IVS_INT32 iSessionID              登录成功后的会话ID
	     output     : const IVS_CHAR * pCameraCode      摄像机编码
	     output     : IVS_UINT32 uiPictureID            图片ID
	     return     : 
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteSnapshot(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID);

	/*************************************************************************
	* name       : IVS_SDK_GetExDomainRecordPolicy
	* description: 查询外域摄像机容灾录像策略
	* input      : pReqXml 请求XML
	* output     : pRspXml 响应XML
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetExDomainRecordPolicy(IVS_INT32 iSessionID,const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_SetExDomainRecordPolicy
	* description: 设置外域摄像机容灾录像策略
	* input      : pReqXml 请求XML
	               iSessionID  登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetExDomainRecordPolicy(IVS_INT32 iSessionID,const IVS_CHAR* pReqXml);

   /*************************************************************************
	* name       : IVS_SDK_SetExDomainRecordPolicy
	* description: 配置外域摄像机容灾录像计划
	* input      : pReqXml 请求XML
	               iSessionID  登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddExDomainRecordPlan(
        IVS_INT32 iSessionID, 
        IVS_UINT32 uiDeviceNum,
        const IVS_DEVICE_CODE* pDeviceList,
        const IVS_RECORD_PLAN* pRecordPlan,
        IVS_RESULT_LIST* pResultList,
        IVS_UINT32 uiResultBufferSize);
    /*************************************************************************
	* name       : IVS_SDK_ModExDomainRecordPlan
	* description: 修改外域摄像机容灾录像计划
	* input      : pReqXml 请求XML
	               iSessionID  登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModExDomainRecordPlan(
        IVS_INT32 iSessionID, 
        IVS_UINT32 uiDeviceNum,
        const IVS_DEVICE_CODE* pDeviceList,
        const IVS_RECORD_PLAN* pRecordPlan,
        IVS_RESULT_LIST* pResultList,
        IVS_UINT32 uiResultBufferSize);

    /*************************************************************************
	* name       : IVS_SDK_DelExDomainRecordPlan
	* description: 删除外域摄像机容灾录像计划
	* input      : pReqXml 请求XML
	               iSessionID  登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelExDomainRecordPlan(
        IVS_INT32 iSessionID,
        IVS_UINT32 uiDeviceNum,
        const IVS_DEVICE_CODE* pDeviceList,
        IVS_UINT32 uiRecordMethod,
        IVS_RESULT_LIST* pResultList,
        IVS_UINT32 uiResultBufferSize);

    /*************************************************************************
	* name       : IVS_SDK_GetExDomainRecordPlan
	* description: 删除外域摄像机容灾录像计划
	* input      : pReqXml 请求XML
	               iSessionID  登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
    
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetExDomainRecordPlan(
        IVS_INT32 iSessionID,
        const IVS_CHAR* pCameraCode,
        IVS_UINT32 uiRecordMethod,
        IVS_RECORD_PLAN* pRecordPlan);


	/*****************************************************************
	     function   : IVS_SDK_GetDeviceCapability
	     description: 查询摄像机码流能力
	     input      : IVS_INT32 iSessionID             登录成功后的会话ID
	     input      : IVS_UINT32 uiCapabilityType      设置类型：IVS_DEVICE_CAPABILITY_TYPE的DEVICE_ENCODE_CAPABILITY
	     input      : const IVS_CHAR* pDevCode         设备编码
		 output     : IVS_CHAR** pRspXml               查询XML
	     return     : 
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceCapability(IVS_INT32 iSessionID, IVS_UINT32 uiCapabilityType, const IVS_CHAR* pDevCode, IVS_CHAR** pRspXml);

    /*********************************************************************************************
    * function       : SetListenAddr
    * description: 设置服务器端监听IP和端口号
    * input      : iPort服务器端监听端口号
    * output     : NA
    * return     : NA
    * remark     : NA
    *********************************************************************************************/
    //IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetNetKeyBoardPort(ULONG ulPort);

    /*********************************************************************************************
    * name       : StartListenThread
    * description: 启动监听线程
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : g_hThread
    * remark     : NA
    *********************************************************************************************/
    //IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartNetKeyBoard(LPCallBack lpCallBack);

    /*********************************************************************************************
    * name       : ReleaseThread
    * description: 释放线程
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    *********************************************************************************************/
    //IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopNetKeyBoard();

    /******************************************************************
     function   : IVS_SDK_SetLogPath
     description: 设置日志根目录
     input      : const IVS_CHAR * pLogPath 日志根目录
     return     : IVS_INT32 成功返回0 失败返回错误码
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetLogPath(const IVS_CHAR* pLogPath);

	/*********************************************************************************************
	     function   : IVS_SDK_SetDeviceGuard
	     description: 设备布撤防
	     input      : IVS_INT32 iSessionID             登录成功后的会话ID
	     input      : IVS_CHAR* pReqXml                请求消息
		 output     : NA
	     return     : 成功返回0， 失败返回错误码
	**********************************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDeviceGuard(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);
	/*****************************************************************
	     function   : IVS_SDK_CheckNewVersion
	     description: 客户端升级检查
	     input      : IVS_INT32 iSessionID      登录成功后的会话ID
	     input      : const IVS_CHAR * pReqXml  升级版本校验xml
	     output     : IVS_CHAR * * pRspXml      外域设备录像策略响应xml
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_CheckNewVersion(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/**************************************************************************
    * name       : IVS_SDK_StopBackupPlayBack
    * description: 停止客户端备份录像回放
    * input      : iSessionID      会话ID
				   ulPlayHandle    播放句柄
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopBackupPlayBack(IVS_INT32 iSessionID,
                                                       IVS_ULONG ulPlayHandle);
	/**************************************************************************
    * name       : IVS_SDK_StartBackupPlayBack
    * description: 开始客户端备份录像回放
    * input      : iSessionID      会话ID
				   pCameraCode     摄像机编码
                   pPlaybackParam  回放参数
    * output     : pHandle         播放句柄
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * remark     : NA
    **************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartBackupPlayBack(IVS_INT32 iSessionID,
		                                                           const IVS_CHAR* pDomainCode,
		                                                           const IVS_CHAR* pNVRCode,
                                                                   const IVS_CHAR* pCameraCode,
                                                                   const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                                   HWND hWnd,
                                                                   IVS_ULONG* pHandle);

  /******************************************************************
  function : IVS_SDK_GetCameraThumbnail
  description: 获取摄像机预览图接口
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  const IVS_CHAR*	pReqXml			请求CBB::CBB::CXml；
  output : IVS_CHAR**		pRspXml			响应CBB::CBB::CXml
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCameraThumbnail(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode, IVS_CHAR* pThumbnailURL,IVS_UINT32 uiBufferSize);


  	/******************************************************************
     function   : IVS_SDK_StartBackupDownload
     description: 开始备份录像下载
     input      : IVS_ULONG ulPlayHandle
     input      : IVS_PLAYBACK_TIME * pPlayBackTime
	 input      : IVS_CHAR* pCameraCode
	 input      : IVS_DOWNLOAD_PARAM* pDownloadParam
	 input      : IVS_ULONG*  pulHandle
     output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartBackupDownload(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,
		const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pulHandle);

	/******************************************************************
     function   : IVS_SDK_StopBackupDownload
     description: 停止备份录像下载
     input      : IVS_INT32 iSessionID
	 input      : IVS_ULONG ulHandle
	 output     : NA
     return     : IVS_SDK_API IVS_INT32 __SDK_CALL
    *******************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopBackupDownload(IVS_INT32 iSessionID, IVS_ULONG ulHandle);

  /******************************************************************
  function : IVS_SDK_SetDevPWD
  description: 设置前端设备密码
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  const IVS_CHAR*	pDevCode		设备编码；
		  const IVS_CHAR* pDevUserName      设备登录用户名
		  const IVS_CHAR* pDevPWD           设备登录密码
  output : NA
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDevPWD(IVS_INT32 iSessionID, const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD);

  /******************************************************************
  function : IVS_SDK_ImportDevCfgFile
  description: 导入前端配置文件
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  const IVS_CHAR*	pDevCode		设备编码
		  const IVS_CHAR*   pCFGFile        配置文件全路径
  output : NA
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ImportDevCfgFile(IVS_INT32 iSessionID, const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFile);

  /******************************************************************
  function : IVS_SDK_ExportDevCfgFile
  description: 导出前端配置文件
  input : IVS_INT32			iSessionID		登录成功后返回的会话ID；
		  const IVS_CHAR*	pDevCode		设备编码
		  const IVS_CHAR*   pCFGFile        配置文件全路径
  output : NA
  return : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ExportDevCfgFile(IVS_INT32 iSessionID, const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFilePath);

  /********************************************************************************************
  function	  : IVS_SDK_ResetAlarm
  description : 报警复位
  input		  : iSessionID		登录成功后返回的会话ID；
			    pReqXml		    请求消息
  output	  : NA
  return	  : 成功返回0；失败返回错误码
  *********************************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ResetAlarm (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

  /********************************************************************************************
  function     : IVS_SDK_ExEventSubscribe
  description  : 外部域告警事件订阅
  input		   : iSessionID		登录成功后返回的会话ID；
		         pReqXml		    请求消息
  output       : NA
  return       : 成功返回0；失败返回错误码
  *********************************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ExEventSubscribe (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

  /********************************************************************************************
  function     : IVS_SDK_GetExEventSubscribe
  description  : 查询外部域告警事件订阅
  input        : iSessionID		登录成功后返回的会话ID；
		         pReqXml		    请求消息
  output       : pRspXml, 响应消息
  return       : 成功返回0；失败返回错误码
  *********************************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetExEventSubscribe (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

  /********************************************************************************************
  function    : IVS_SDK_ExCatalogSubscribe
  description : 外部域设备目录订阅
  input       : iSessionID		登录成功后返回的会话ID；
		        pReqXml		    请求消息
  output      : NA
  return      : 成功返回0；失败返回错误码
  *********************************************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ExCatalogSubscribe (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

  /******************************************************************
  function    : IVS_SDK_GetExCatalogSubscribeList
  description : 查询外部域设备目录订阅
  input       : iSessionID		登录成功后返回的会话ID；
		        pReqXml		    请求消息
  output      : pRspXml, 响应消息
  return      : 成功返回0；失败返回错误码
  *******************************************************************/
  IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetExCatalogSubscribeList (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

  /*********************************************************************************************
    * name       : IVS_OCX_GetAlarmSnapshotList
    * description: 获取告警抓拍图片列表
    * input      : iSessionID, 会话ID，
	               pReqXml， 请求xml
    * output     : pRsqXml, 响应xml
    * return     : 成功返回0， 失败返回错误码
    * remark     : NA
    *********************************************************************************************/
   IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmSnapshotList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRsqXml);

   	/*************************************************************************
	* name       : IVS_SDK_ExGetDeviceStatus
	* description: 设备状态查询
	* input      : pReqXml 请求XML
	* output     : pRspXml 响应XML
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ExGetDeviceStatus(IVS_INT32 iSessionID,const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml);

	/*************************************************************************
	* name       : IVS_SDK_ExGetDeviceInfo
	* description: 设备信息查询
	* input      : pReqXml 请求XML
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ExGetDeviceInfo(IVS_INT32 iSessionID,const IVS_CHAR* pReqXml);

    /*************************************************************************
	* name       : IVS_SDK_RefreshCameraList
	* description: 刷新摄像机列表
	* input      : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_RefreshCameraList(IVS_INT32 iSessionID);

	/*************************************************************************
	* name       : IVS_SDK_GetCameraEncodeCapabilities
	* description: Onvif设备从前端获取能力参数范围
	* input      : pCameraCode 请求摄像机编码
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceEncodeCapability(IVS_INT32 iSessionID,const IVS_CHAR* pDevCode,
		IVS_STREAM_INFO_LIST* pStreamInfoList);

    IVS_SDK_API VOID __SDK_CALL IVS_SDK_SetLogLevel(IVS_ULONG ulLogLevel);
	/**************************************************************************
    * name       : IVS_SDK_OMUQueryRaidGroup
    * description: 磁盘组信息查询
    * input      : iSessionID 登录成功后的会话ID
	* output     : pRspXml  响应XML
    * return     : 成功返回0，失败返回错误码
    * author     ：zhangrui z00233055
	* remark     : NA
	* history    : 2013-12-4
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRaidGroup(IVS_INT32 iSessionID, IVS_CHAR** pRspXml);

	/**************************************************************************
    * name       : IVS_SDK_OMUQueryRaidGroupDisk
    * description: 磁盘信息查询
    * input      : iSessionID 登录成功后的会话ID
						pReqXml 请求XML
	* output     : pRspXml  响应XML
    * return     : 成功返回0，失败返回错误码
    * author     ：zhangrui z00233055
	* remark     : NA
	* history    : 2013-12-4
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRaidGroupDisk(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	/**************************************************************************
    * name			: IVS_SDK_SetReceiveOMUAlarm
    * description: 设置SDK是否接收OMU设备类告警
    * input			: iSessionID 登录成功后的会话ID
	* input			: bReceive 是否接收OMU设备类告警
	* output			: NA
    * return			: 成功返回0，失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: NA
	* history			: 2014-5-8
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetReceiveOMUAlarm(
		IVS_INT32 iSessionID,
		IVS_BOOL bReceive);

	/**************************************************************************
    * name			: IVS_SDK_GetVideoDiagnose
    * description: 查询单台VCN的视频质量诊断计划
    * input			: iSessionID 登录成功后的会话ID
	* input			: pDomainCode 域编码
	* input			: pCameraCode 摄像机编码
	* output			: pRspXml 单台VCN的视频质量诊断计划xml
    * return			: 成功返回0，失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: pRspXml是SDK内部申请内存，使用完应调用IVS_SDK_ReleaseBuffer释放。
	* history			: 2014-6-9
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetVideoDiagnose(IVS_INT32 iSessionID,
		const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
		IVS_CHAR** pRspXml);

	/**************************************************************************
    * name			: IVS_SDK_SetVideoDiagnose
    * description: 设置单台VCN的视频质量诊断计划
    * input			: iSessionID 登录成功后的会话ID
	* input			: pReqXml 单台VCN的视频质量诊断计划xml
	* output			: NA
    * return			: 成功返回0，失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: NA
	* history			: 2014-6-9
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetVideoDiagnose(IVS_INT32 iSessionID, 
		const IVS_CHAR* pReqXml);

	/**************************************************************************
    * name			: IVS_SDK_GetStreamListByCam
    * description: 根据摄像机Code，查询该摄像机正在给哪些用户提供视频业务
    * input			: iSessionID 登录成功后的会话ID
	* input			: pDomainCode 域编码
	* input			: pCameraCode 摄像机编码
	* input			: uiBufSize 申请内存大小
	* output			: pStreamListByCam 摄像机开启的媒体业务列表
    * return			: 成功返回0，失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: C/C++中uiBufSize固定为sizeof(IVS_STREAM_LIST_BY_CAM) + 
								(IVS_MAX_STREAM_USER_INFO_NUM -1) * sizeof(IVS_STREAM_USER_INFO)
	* history			: 2014-6-10
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetStreamListByCam(IVS_INT32 iSessionID, 
		const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
		IVS_UINT32 uiBufSize,
		IVS_STREAM_LIST_BY_CAM* pStreamListByCam);

	/**************************************************************************
    * name			: IVS_SDK_GetStreamListByUser
    * description: 根据用户编码，查询该用户正在给调用哪些摄像机的视频业务
    * input			: iSessionID 登录成功后的会话ID
	* input			: pDomainCode 被查询的用户所在域编码
	* input			: uiUserID 被查询的用户ID
	* input			: uiBufSize 申请内存大小
	* output			: pStreamListByUser 用户正在调用的媒体业务列表
    * return			: 成功返回0，失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: C/C++中uiBufSize固定为sizeof(IVS_STREAM_LIST_BY_USER) + 
								(IVS_MAX_STREAM_CAM_INFO_NUM -1) * sizeof(IVS_STREAM_CAM_INFO)
	* history			: 2014-6-24
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetStreamListByUser(IVS_INT32 iSessionID, 
		const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiUserID,
		IVS_UINT32 uiBufSize,
		IVS_STREAM_LIST_BY_USER* pStreamListByUser);

	/**************************************************************************
    * name			: IVS_SDK_ShutdownStreamByUser
    * description: 根据摄像机Code和用户ID，关闭该用户正在使用的视频业务
    * input			: iSessionID 登录成功后的会话ID
	* input			: pDomainCode 域编码
	* input			: uiUserID 用户ID
	* input			: pCameraCode 摄像机编码
	* input			: iStreamType 视频业务类型 1-实况，2-回放
	* input			: iDuration 关闭时长，在此时间之内该用户不能再次点播，单位为秒，0为不控制关闭时长
	* output			: NA
    * return			: 成功返回0，失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: NA
	* history			: 2014-6-10
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ShutdownStreamByUser(IVS_INT32 iSessionID,
		const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiUserID,
		const IVS_CHAR* pCameraCode,
		IVS_INT32 iStreamType,
		IVS_INT32 iDuration);

	/**************************************************************************
    * name			: IVS_SDK_PasswdValidation
    * description: 用户名、密码验证
    * input			: iSessionID 登录成功后的会话ID
	* input			: pLoginName 登录用户名
	* input			: pPasswd 密码
	* output			: NA
    * return			: 成功返回0，失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: NA
	* history			: 2014-6-27
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PasswdValidation(IVS_INT32 iSessionID,
		const IVS_CHAR* pLoginName,
		const IVS_CHAR *pPasswd);

	/**************************************************************************
    * name			: IVS_SDK_SetVideoEffect
    * description: 录像视频质量调节
	* input			: iSessionID 登录成功后的会话ID
    * input			: ulHandle 播放句柄
	* input			: lBrightValue 亮度
	* input			: lContrastValue 对比度
	* input			: lSaturationValue 饱和度
	* input			: lHueValue 色调
	* output			: NA
    * return			: 成功返回0（IVS_SUCCEED），失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: NA
	* history			: 2014-9-2
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetVideoEffect(IVS_INT32 iSessionID,
		IVS_ULONG ulPlayHandle, IVS_LONG lBrightValue,
		IVS_LONG lContrastValue, IVS_LONG lSaturationValue,
		IVS_LONG lHueValue);

	/**************************************************************************
    * name			: IVS_SDK_GetDomainDeviceList
    * description: 获取指定域所包含的设备列表
	* input			: iSessionID 登录成功后的会话ID
    * input			: pDomainCode 域编码，为需要查询设备的域的编码
	* input			: uiDeviceType 设备类型，值参考IVS_DEVICE_TYPE，支持除主设备，外域告警设备外的其他设备
	* input			: pIndexRange 分页信息
	* output			: pDeviceList 设备列表指针
	* input			: uiBufferSize 缓存大小
    * return			: 成功返回0（IVS_SUCCEED），失败返回错误码
    * author			: zhuohaofan zwx211831
	* remark		: NA
	* history			: 2014-9-9
    **************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDomainDeviceList(IVS_INT32 iSessionID,
		const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiDeviceType,
		const IVS_INDEX_RANGE* pIndexRange,
		IVS_VOID* pDeviceList,
		IVS_UINT32 uiBufferSize);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_IVS_SDK_H_

