/********************************************************************
filename : OCXUserInfoMgr.h
author : s00191067
created : 2013/01/14
description : 用户管理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 初始版本
*********************************************************************/


#ifndef __OCX_USERINFO_MGR_H_
#define __OCX_USERINFO_MGR_H_

#include "IVSCommon.h"
#include "hwsdk.h"

class COCXUserInfoMgr
{
public:
    COCXUserInfoMgr(void);
    ~COCXUserInfoMgr(void);

public:
    // 添加用户信息
    static CString AddUser(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 删除用户
    static LONG DeleteUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // 设置用户信息
    static LONG ModifyUser(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // 查询用户信息
    static CString GetUserInfo(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // 锁定/解锁用户
    static LONG LockUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID, ULONG ulLock);

    // 查询用户列表
    static CString GetUserList(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery);

    // 查询在线用户列表
    static CString GetOnlineUser(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery);

    // 修改密码
    static LONG ChangePWD(IVS_INT32 iSessionID, LPCTSTR pOldPWD, LPCTSTR pNewPWD);

    // 重置密码
    static LONG ResetPWD(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID, LPCTSTR pNewPWD);

    // 校验输入的密码是否符合账号规则
    static LONG CheckPWDByRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName, LPCTSTR pPWD);

    // 根据角色获取符合账号规则的密码
    static CString GetAutoCreatePWD(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName);

    // 获取密码强度
    static CString GetPWDStrength(LPCTSTR pPWD);

    // 获取密码过期信息
    static CString GetPWDExpireInfo(IVS_INT32 iSessionID);

    // 设置用户设备列表
    static LONG SetUserDeviceList(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    //获取用户设备列表
    static CString GetUserDeviceList(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery);

    // 签退在线用户会话
    static LONG SignOutOnlineUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserOnlineID);

    // 根据用户ID签退在线用户会话
    static LONG SignOutUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // 根据角色ID签退在线用户会话
    static LONG SignOutUserByRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID);

};

#endif
