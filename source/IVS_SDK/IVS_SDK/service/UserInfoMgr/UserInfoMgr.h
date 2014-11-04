/********************************************************************
filename : UserInfoMgr.h
author : pkf57481
created : 2012/12/03
description : 用户管理
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/


#ifndef __USERINFO_MGR_H_
#define __USERINFO_MGR_H_

#include "IVSCommon.h"
#include "hwsdk.h"
#include "SDKDef.h"

class CUserMgr;
class CUserInfoMgr
{
public:
    CUserInfoMgr(void);
    ~CUserInfoMgr(void);

    void SetUserMgr(CUserMgr* pUserMgr);

public:
    // 添加用户信息
    IVS_INT32 AddUser(const IVS_USER_INFO* pUserInfo, IVS_UINT32* puiUserID);

    // 设置用户信息
    IVS_INT32 ModifyUser(const IVS_USER_INFO* pUserInfo);

    // 删除用户
    IVS_INT32 DeleteUser(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID);

    // 查询用户信息
    IVS_INT32 GetUserInfo(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_USER_INFO* pUserInfo);

    // 锁定/解锁用户
    IVS_INT32 LockUser(const IVS_CHAR* pDomainCode, IVS_INT32 /*iSessionID*/, IVS_UINT32 uiUserID, IVS_BOOL bLock);

    // 查询用户列表
    IVS_INT32 GetUserList(const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_USER_INFO_LIST* pUserList, IVS_UINT32 /*uiBufferSize*/);

    // 查询在线用户列表
    IVS_INT32 GetOnlineUser(const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ONLINE_USER_LIST* pOnlineUserList, IVS_UINT32 /*uiBufferSize*/);

    // 设置用户设备列表
    IVS_INT32 SetUserDeviceList(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 /*uiBufferSize*/);

    //获取用户设备列表
    IVS_INT32 GetUserDeviceList(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 /*uiBufferSize*/);

    // 修改密码
    IVS_INT32 ChangePWD(const IVS_CHAR* pOldPWD, const IVS_CHAR* pNewPWD);

    // 重置密码
    IVS_INT32 ResetPWD(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, const IVS_CHAR* pNewPWD);

    // 校验输入的密码是否符合账号规则
    IVS_INT32 CheckPWDByRole(const IVS_CHAR* pDomainCode, const IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, const IVS_CHAR* pPWD);

    // 根据角色获取符合账号规则的密码
    IVS_INT32 GetAutoCreatePWD(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, IVS_CHAR* pNewPWD, IVS_UINT32 /*uiPWDLen*/);

    // 获取密码过期信息
    IVS_INT32 GetPWDExpireInfo(IVS_PWD_EXPIRE_INFO* pPWDExpireInfo);

    // 根据用户ID签退用户的所有会话
    IVS_INT32 SignOutUser(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID);

    // 根据角色ID签退用户的所有会话
    IVS_INT32 SignOutUserByRole(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID);

    // 签退在线用户会话
    IVS_INT32 SignOutOnlineUser(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserOnlineID);

	/******************************************************************
	function			: GetStreamListByCam
	description	: 根据摄像机Code，查询该摄像机正在给哪些用户提供视频业务
	author			: zwx211831
	input				: pDomainCode 域编码
	input				: pCameraCode 摄像机编码
	input				: uiBufSize 申请内存大小
	output			: pStreamListByCam 摄像机开启的媒体业务列表
	return				: 成功返回0，失败返回错误码
	*******************************************************************/
	IVS_INT32 GetStreamListByCam(const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
		IVS_UINT32 uiBufSize,
		IVS_STREAM_LIST_BY_CAM* pStreamListByCam)const;

	/******************************************************************
	function			: GetStreamListByUser
	description	: 根据用户编码，查询该用户正在给调用哪些摄像机的视频业务
	author			: zwx211831
	input				: pDomainCode 域编码
	input				: uiUserID 用户ID
	input				: uiBufSize 申请内存大小
	output			: pStreamListByUser 用户正在调用的媒体业务列表
	return				: 成功返回0，失败返回错误码
	*******************************************************************/
	IVS_INT32 GetStreamListByUser(const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiUserID,
		IVS_UINT32 uiBufSize,
		IVS_STREAM_LIST_BY_USER* pStreamListByUser)const;

	/******************************************************************
	function			: ShutdownStreamByUser
	description	: 根据摄像机Code和用户ID，关闭该用户正在使用的视频业务
	author			: zwx211831
	input				: pDomainCode 域编码
	input				: uiUserID 用户ID
	input				: pCameraCode 摄像机编码
	input				: iStreamType 视频业务类型 1-实况，2-回放
	input				: iDuration 关闭时长，在此时间之内该用户不能再次点播，单位为秒，0为不控制关闭时长
	output			: NA
	return				: 成功返回0，失败返回错误码
	*******************************************************************/
	IVS_INT32 ShutdownStreamByUser(const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiUserID,  
		const IVS_CHAR* pCameraCode,
		IVS_INT32 iStreamType,
		IVS_INT32 iDuration)const;

	/******************************************************************
	function			: PasswdValidation
	description	: 用户名、密码验证
	author			: zwx211831
	input				: pLoginName 登录用户名
	input				: pPasswd 密码
	output			: NA
	return				: 成功返回0，失败返回错误码
	*******************************************************************/
	IVS_INT32 PasswdValidation(const IVS_CHAR* pLoginName,
		const IVS_CHAR *pPasswd)const;

private:
    IVS_INT32 SetUserDeviceListInUserDomain(const IVS_CHAR* pUserDomainCode, IVS_UINT32 uiUserID, const std::list<IVS_USER_DEVICE_INFO *> &pUserDevList);

    // 查询设备组列表
    IVS_INT32 GetDeviceGroup(const IVS_CHAR* pDomainCode, const IVS_CHAR* cDevGroupCode, IVS_BOOL bIsExDomain, std::vector<IVS_USER_DEVICE_INFO> &userDevList);

    // 查询设备组列表 -- 分配内存
    IVS_INT32 AllocateDevGroupSpace(IVS_DEVICE_GROUP_LIST* &pDevGroupList, IVS_UINT32* iBufferSize)const;

    IVS_INT32 GetCurrentUserDev(IVS_USER_DEVICE_INFO_LIST* &userDevList);

    IVS_INT32 GetUserDevList(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_USER_DEVICE_INFO_LIST* &pUserDevList);

    IVS_INT32 AllocateUserDevListSpace(IVS_UINT32 uiToIndex, IVS_INDEX_RANGE* pIndexRange, IVS_USER_DEVICE_INFO_LIST* &pUserDevList, IVS_UINT32* iBufferSize)const;


private:
    CUserMgr* m_UserMgr;

    IVS_ROLE_INFO m_stRoleInfo; 
};

#endif
