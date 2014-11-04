/********************************************************************
	filename	: 	RecordBackup.h
	author		:	tkf71032
	created		:	2013/1/23	
	description	:	定义录像书签管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/1/23 初始版本;
*********************************************************************/

#ifndef __RECORD_BACKUP_H__
#define __RECORD_BACKUP_H__

#include "hwsdk.h"
#include "ivs_error.h"
#include "IVSCommon.h"
#include "ToolsHelp.h"
#include "SDKConfig.h"
//#include "ivs_xml.h"

class CUserMgr;
 class CRecordBackupMgr
 {
 public:
	 CRecordBackupMgr(void);
	 ~CRecordBackupMgr(void);

 public:
	 // 设置用户对象类
	 void SetUserMgr(void* pUserMgr);

	 // 设置摄像机备份参数
	 IVS_INT32 SetCameraBackupCfg(const IVS_CHAR* pBackupXml);

	 // 获取摄像机备份参数
	 IVS_INT32 GetCameraBackupCfg(const IVS_CHAR* pCameraCode, IVS_CHAR** pBackConfigXml);

	 // 重设摄像机编码
	 IVS_INT32 SetCameraCodeValue(CXml& xml, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode) const;

	 // 拼装获取摄像机备份参数的xml
	 IVS_INT32 GetCameraBackupCfgGetXml(CXml& xml, const IVS_CHAR *pDevCode, const IVS_CHAR* pDomainCode)const;

	 // 添加录像备份计划
	 IVS_INT32 AddRecordBackupPlan(const IVS_CHAR* pBackupPlan);

	 // 修改录像备份计划
	 IVS_INT32 ModifyRecordBackupPlan(const IVS_CHAR* pBackupPlan);

	 // 修改录像备份计划
	 IVS_INT32 DeleteRecordBackupPlan(const IVS_CHAR* pBackupPlan);

	 // 查询录像备份计划
	 IVS_INT32 GetRecordBackupPlan(const IVS_CHAR* pQueryInfo, IVS_CHAR** pBackupPlan);

	 // 新增录像备份任务
	 IVS_INT32 AddRecordBackupTask(const IVS_CHAR* pBackupTask);

	 /*****************************************************************
	      function   : RecordBackupPlan
	      description: 录像备份计划公共发送接口
	      input      : NSS_MSG_TYPE_E msgType  发送的消息类型
					   const IVS_CHAR * pBackupPlan   录像备份计划XML
	      return     : 成功返回0；失败返回错误码
	 *****************************************************************/
	 IVS_INT32 RecordBackupPlan(NSS_MSG_TYPE_E msgType, const IVS_CHAR* pBackupPlan);

	 // 重设添加录像备份计划的摄像机编码并添加domianCode
	 /*****************************************************************
	      function   : SetNodeCameraCodeValue
	      description: 解析cameraCode，并拼装domianCode字段进xml
	      output     : CXml & xml  处理后的xml 
	                   IVS_CHAR * pDevCode  解析后的设备编码
			           IVS_CHAR * pDomainCode   解析出的域编码
	      return     : 成功返回0；失败返回错误码
	 *****************************************************************/
	 IVS_INT32 SetNodeCameraCodeValue(CXml& xml, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode) const;

	 /*****************************************************************
	      function   : SetNodeFileNameValue
	      description: 解析cameraCode，并拼装domianCode字段进xml
	      output     : CXml & xml  处理后的xml 
	                   IVS_CHAR * pDevCode  解析后的设备编码
			           IVS_CHAR * pDomainCode   解析出的域编码
	      return     : 成功返回0；失败返回错误码
	 *****************************************************************/
	 IVS_INT32 SetNodeFileNameValue(CXml& xml) const;

	 /*****************************************************************
	      function   : StartRecordBackupTask
	      description: 启动/继续录像备份任务
	      input     : pQueryInfo  备份任务参数信息
	      return     : 成功返回0；失败返回错误码
	 *****************************************************************/
	 IVS_INT32 StartRecordBackupTask(const IVS_CHAR* pQueryInfo);

	 /*****************************************************************
	      function   : StopRecordBackupTask
	      description: 暂停/停止录像备份任务
	      input     : pQueryInfo  备份任务参数信息
	      return     : 成功返回0；失败返回错误码
	 *****************************************************************/
	 IVS_INT32 StopRecordBackupTask(const IVS_CHAR* pQueryInfo);

	 /*****************************************************************
	      function   : GetRecordBackupTaskList
	      description: 查询录像备份任务列表
	      input     : pQueryInfo  备份任务参数信息
	      return     : 成功返回0；失败返回错误码
	 *****************************************************************/
	 IVS_INT32 GetRecordBackupTaskList(const IVS_CHAR* pQueryInfo, IVS_CHAR** pRspXml);

	 /*****************************************************************
	      function   : DeleteRecordBackupTask
	      description: 删除录像备份任务
	      input     : pQueryInfo  备份任务参数信息
	      return     : 成功返回0；失败返回错误码
	 *****************************************************************/
	 IVS_INT32 DeleteRecordBackupTask(const IVS_CHAR* pQueryInfo);

 private:
	 CUserMgr *m_pUserMgr;   
 };

#endif
