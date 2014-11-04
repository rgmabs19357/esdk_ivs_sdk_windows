/********************************************************************
filename : UserGroupMgr.h
author : pkf57481
created : 2012/12/03
description : 用户组管理
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/


#ifndef __USERGROUP_MGR_H_
#define __USERGROUP_MGR_H_

#include "hwsdk.h"
#include "SDKDef.h"

class CUserMgr;
class CUserGroupMgr
{
public:
    CUserGroupMgr(void);
    ~CUserGroupMgr(void);

    void SetUserMgr(CUserMgr* pUserMgr);

public:
    // 增加用户组
    IVS_INT32 AddUserGroup(const IVS_USER_GROUP* pUserGroup, IVS_UINT32* puiUserGroupID);

    // 删除用户组
    IVS_INT32 DeleteUserGroup(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserGroupID);

    // 修改用户组
    IVS_INT32 ModifyUserGroup(const IVS_USER_GROUP* pUserGroup);

    // 移动用户组
    IVS_INT32 MoveUserGroup(const IVS_CHAR* pDomainCode, IVS_UINT32 uiTargetGroupID, const IVS_USER_GROUP_LIST* pUserGroupList);

    // 查询用户组列表
    IVS_INT32 GetUserGroupList(const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_GROUP_LIST* pUserGroupList, IVS_UINT32 /*uiBufferSize*/);

private:
    CUserMgr* m_pUserMgr;
};

#endif
