/********************************************************************
filename : OCXUserGroupMgr.h
author : s00191067
created : 2013/01/14
description : 用户组管理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 初始版本
*********************************************************************/


#ifndef __OCX_USERGROUP_MGR_H_
#define __OCX_USERGROUP_MGR_H_

#include "hwsdk.h"

class COCXUserGroupMgr
{
public:
    COCXUserGroupMgr(void);
    ~COCXUserGroupMgr(void);

public:
    // 增加用户组
    static CString AddUserGroup(IVS_INT32 iSessionID, LPCTSTR pUserGroup);

    // 删除用户组
    static LONG DeleteUserGroup(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserGroupID);

    // 修改用户组
    static LONG ModifyUserGroup(IVS_INT32 iSessionID, LPCTSTR pUserGroup);

    // 移动用户组
    static LONG MoveUserGroup(IVS_INT32 iSessionID, LPCTSTR pUserGroup);

    // 查询用户组列表
    static CString GetUserGroupList(IVS_INT32 iSessionID, LPCTSTR pReqXml);

};

#endif
