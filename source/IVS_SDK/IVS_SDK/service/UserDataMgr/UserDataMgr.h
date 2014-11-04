/********************************************************************
 filename    :    UserDataMgr.h
 author      :    x00192614
 created     :    2013/01/17
 description :    用户数据上传下载
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/17 初始版本
*********************************************************************/

#ifndef __USER_DATA_MGR_H__
#define __USER_DATA_MGR_H__

#include "ivs_error.h"
#include "hwsdk.h"
#include "NSSOperator.h"
#include "SDKDef.h"

class CUserMgr;
class CUserDataMgr
{
public:
    CUserDataMgr(void);
    ~CUserDataMgr(void);

    /******************************************************************
     function   : BackupUserData
     description: 上传用户数据到服务器
     input      : const IVS_CHAR * pFileName 用户数据路径
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 BackupUserData(const IVS_CHAR* pFileName)const;

    /******************************************************************
     function   : RestoreUserData
     description:  从服务器下载用户数据
     input      : const IVS_CHAR * pFileName 用户数据路径
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 RestoreUserData(const IVS_CHAR* pFileName);

	/******************************************************************
	 function   : GetUserDataResp
	 description: 从服务器下载用户数据请求XML
	 input      : const IVS_CHAR * pFileName 
	 input      : const IVS_CHAR * pResp
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetUserDataResp(const IVS_CHAR* pFileName, const IVS_CHAR* pResp) const;

    /******************************************************************
     function   : SetUserMgr
     description: 设置用户指针
     input      : CUserMgr * pUserMgr
     output     : NA
     return     : void
    *******************************************************************/
    void SetUserMgr(CUserMgr *pUserMgr);

	/******************************************************************
     function   : UploadLogo
     description: 上传logo图片
     input      : uiLogoType logo类型
				  pLogoFile logo路径
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_INT32 UploadLogo(IVS_UINT32 uiLogoType, const IVS_CHAR* pLogoFile)const;

	/******************************************************************
     function   : GetLogo
     description: 下载logo图片
     input      : uiLogoType logo类型
				  pLogoMD5 MD5值
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_INT32 GetLogo(IVS_UINT32 uiLogoType, const IVS_CHAR* pLogoMD5)const;

	/******************************************************************
     function   : UploadStatement
     description: 上传申明信息
     input      : pLanguageType 语言类型
				  pStatementFile 申明信息路径
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_INT32 UploadStatement(const IVS_CHAR* pLanguageType,const IVS_CHAR* pStatementFile)const;

	/******************************************************************
     function   : GetStatement
     description: 获取申明信息
     input      : pLanguageType 语言类型
				  pStatement 申明信息返回值
     return     : IVS_INT32 成功返回0，失败返回错误码
    *******************************************************************/
	IVS_INT32 GetStatement(const IVS_CHAR* pLanguageType,IVS_CHAR** pStatement);
private:
    CUserMgr *m_pUserMgr;   // 用户指针

    /******************************************************************
     function   : GetRestoreUserDataReqXml
     description: 下载用户数据请求xml
     input      : const IVS_CHAR * cLoginId
     input      : CXml & reqXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetRestoreUserDataReqXml(const IVS_CHAR* cLoginId, CXml &reqXml) const;

    /******************************************************************
     function   : GetRestoreUserDataParseRspXml
     description: 解析下载用户数据
     input      : const string & strFailMd5
     input      : const IVS_CHAR * pFileName
     input      : CXml & rspXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    //IVS_INT32 GetRestoreUserDataParseRspXml(const string &strFailMd5, const IVS_CHAR* pFileName, CXml &rspXml) const;

	/******************************************************************
	 function   : GetFileMd5
	 description: 获取文件MD5值
	 input      : const IVS_CHAR * pFileName
	 output     : NA
	 return     : std::string
	*******************************************************************/
	std::string GetFileMd5(const IVS_CHAR *pFileName) const;

};
#endif


