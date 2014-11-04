/********************************************************************
filename : RoleMgr.h
author : pkf57481
created : 2013/01/14
description : 角色管理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 初始版本
*********************************************************************/


#ifndef __ROLE_MGR_H_
#define __ROLE_MGR_H_

#include "hwsdk.h"
#include "IVSCommon.h"

class CUserMgr;
class CRoleMgr
{
public:
    CRoleMgr(void);
    ~CRoleMgr(void);

    void SetUserMgr(CUserMgr *pUserMgr);

public:
    // 增加角色
    IVS_INT32 AddRole(const IVS_ROLE_INFO* pRoleInfo, IVS_UINT32* puiRoleID);

    // 删除角色
    IVS_INT32 DeleteRole(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID);

    // 修改角色
    IVS_INT32 ModifyRole(const IVS_ROLE_INFO* pRoleInfo);

    // 查询角色列表
    IVS_INT32 GetRoleList(const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_ROLE_INFO_LIST* pRoleInfoList, IVS_UINT32 /*uiBufferSize*/);

    // 根据角色ID获取角色详细信息
    IVS_INT32 GetRoleInfoByRoleID(const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, IVS_ROLE_INFO* pRoleInfo);

    // 根据用户ID获取角色详细信息
    IVS_INT32 GetRoleInfoByUserID(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_ROLE_INFO* pRoleInfo);

    // 获取默认账号规则
    IVS_INT32 GetDefaultRoleAccountRule(const IVS_CHAR* pDomainCode, IVS_ROLE_ACCOUNT_RULE* pRoleAccountRule);

    // 获取用户权限信息
    IVS_INT32 GetOperationRightGroup(const IVS_CHAR* pDomainCode, IVS_OPERATION_RIGHT_GROUP_LIST* pRightGroupList);

private:
    CUserMgr *m_UserMgr;
};

#endif
