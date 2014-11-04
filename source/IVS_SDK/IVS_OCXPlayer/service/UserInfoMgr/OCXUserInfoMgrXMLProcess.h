/********************************************************************
filename : OCXUserInfoMgrXMLProcess.h
author : s00191067
created : 2013/01/14
description : 用户管理相关接口报文拼装解析类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 初始版本
*********************************************************************/


#ifndef __OCX_USERINFO_MGR_XML_PROCESS_H__
#define __OCX_USERINFO_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"


class COCXUserInfoMgrXMLProcess
{
private:
    COCXUserInfoMgrXMLProcess(void);
public:
    ~COCXUserInfoMgrXMLProcess(void);

public:
    static IVS_INT32 GetUserInfoParseXml(LPCTSTR cstrRet, IVS_USER_INFO &pUserInfo);

    static IVS_VOID AddUserGetXml(IVS_UINT32 uiUserID, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetUserListGetXml(const IVS_USER_INFO_LIST *pUserInfoList, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetUserInfoGetXml(const IVS_USER_INFO* pUserInfo, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetOnlineUserListGetXml(const IVS_ONLINE_USER_LIST *pOnlieUserInfo, IVS_INT32 iRet, CXml &xml);

    static IVS_INT32 SetUserDevListParseXml(LPCTSTR cstrRet, IVS_UINT32 &uiUserID, IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_CHAR* pDomainCode);

    static IVS_INT32 GetUserDevListGetXml(const IVS_USER_DEVICE_INFO_LIST *pUserDevList, IVS_INT32 iRet, CXml &xml);


    static IVS_VOID GetUserGroupListGetXML(const IVS_USER_GROUP_LIST* pUserGroupList, IVS_INT32 iResult, CXml &xml);

    static IVS_INT32 UserGroupParseXML(LPCTSTR cstrRet, IVS_USER_GROUP &pUserGroup);

    static IVS_INT32 UserGroupSetXML(CXml& xml, IVS_UINT32& puiUserGroupID, const IVS_INT32& iRetValue);

    static IVS_INT32 MoveUserGroupPraseXML(IVS_CHAR* pCameraCode, IVS_UINT32& uiTargetDevGroupID,IVS_USER_GROUP_LIST* &pUserGroupList, CXml& xml);


    static IVS_INT32 GetRoleInfoParseXML(LPCTSTR cstrRet, IVS_ROLE_INFO &pUserInfo);

    static IVS_VOID GetRoleListGetXml(const IVS_ROLE_INFO_LIST *pRoleList, IVS_INT32 iCode, CXml &xml);

    static IVS_VOID AddRoleGetXML(IVS_UINT32 &uiRoleID, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetRoleInfoGetXml(const IVS_ROLE_INFO* pRoleInfo, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetAutoCreatePWDGetXml(const IVS_CHAR *pPWD, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetPWDStrengthGetXml(IVS_UINT32 uiPWDStrength, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetDefaultRoleAccountRuleGetXml(const IVS_ROLE_ACCOUNT_RULE* pRoleAccountRule, IVS_INT32 iRet, CXml &xml);

    static IVS_VOID GetOperationRightGroupGetXml(const IVS_OPERATION_RIGHT_GROUP_LIST *pOpRightGroupList, IVS_INT32 iRet, CXml &xml);

    static IVS_UINT32 GetXmlListItemNum(CXml &xml, const IVS_CHAR* pElemPath);

    static IVS_VOID GetPWDExpireInfoGetXml(const IVS_PWD_EXPIRE_INFO* pPWDExpireInfo, IVS_INT32 iRet, CXml &xml);

	static IVS_INT32 GetStreamListByCamGetXml(const IVS_STREAM_LIST_BY_CAM* pStreamList, IVS_INT32 iRet, CXml &xml);

	static IVS_INT32 GetStreamListByUserGetXml(const IVS_STREAM_LIST_BY_USER* pStreamList, IVS_INT32 iRet, CXml &xml);

private:
    // 获取密码强度类型
    static const IVS_CHAR* GetPWDStrength(IVS_UINT32 uiStrength);

};

#endif // __OCX_USERINFO_MGR_XML_PROCESS_H__
