/********************************************************************
	filename	: 	CRecord.h
	author		:	z00201790
	created		:	2012/11/12	
	description	:	定义录像业务类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/12 初始版本;
*********************************************************************/
#ifndef __RECORD_H__
#define __RECORD_H__

#include "RecordXMLProcess.h"
#include "SDKDef.h"

typedef enum
{
	RECORD_IDLE    = 0,
	RECORDING
}RECORD_STATUS;

class CRecord;
typedef struct ST_STOP_RECORD_INFO
{
	unsigned long ulPort;
	CRecord* pRecord;
}STOP_RECORD_INFO;

class CUserMgr;
class CRecord
{
public:
	CRecord(void);
	~CRecord(void);

public:

	int GetPlayHandle()const;     
	void SetPlayHandle(int iPlayHandle);

	//设置用户对象类
	void SetUserMgr(void *pUserMgr);

	RECORD_STATUS GetState()const;                 
	void SetState(RECORD_STATUS RecordStatus);   

	unsigned long GetPort()const;  

	/***********************************************************************************
    * name       : DealException
    * description: 处理MP的流异常回调
    * input      : iPort:端口; iMsgType:消息类型; pHandle:句柄
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	void DealException(IVS_INT32 /*iPort*/, IVS_INT32 /*iMsgType*/,const void* pHandle);

	void Release();

	/***********************************************************************************
    * name       : SetRecordSpace
    * description: 设置录像空间
    * input      : pCameraCode:摄像机编号; pRecordSpace:录像空间信息;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 SetRecordSpace(const IVS_CHAR* pCameraCode, const IVS_RECORD_POLICY_TIME* pRecordSpace) const;

	/***********************************************************************************
    * name       : DeleteRecordSpace
    * description: 删除录像空间
    * input      : pCameraCode:摄像机编号; iRecordMethod:录像方式; 
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 DeleteRecordSpace(const IVS_CHAR* pCameraCode, IVS_INT32 /*iRecordMethod*/) const;

	/***********************************************************************************
    * name       : StartPlatformRecord
    * description: 开始平台录像
    * input      : pCameraCode:摄像机编号; iRecordTime:录像时间;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 StartPlatformRecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime) const;

	/***********************************************************************************
    * name       : StopPlatformRecord
    * description: 停止平台录像
    * input      : pCameraCode:摄像机编号;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 StopPlatformRecord(const IVS_CHAR* pCameraCode) const;

	/***********************************************************************************
    * name       : StartPURecord
    * description: 开始前端录像
    * input      :  pCameraCode:摄像机编号; iRecordTime:录像时间
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 StartPURecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime) const;

	/***********************************************************************************
    * name       : StopPURecord
    * description: 停止前端录像
    * input      :  pCameraCode:摄像机编号;
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 StopPURecord(const IVS_CHAR* pCameraCode) const;

	/***********************************************************************************
    * name       : StartRemoteRecord
    * description: 开始远程录像
    * input      : eRecordType:录像类型; pCameraCode:摄像机编号; iRecordTime:录像时间
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 StartRemoteRecord(RECORD_TYPE eRecordType,const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime)const;

	/***********************************************************************************
    * name       : StopRemoteRecord
    * description: 停止远程录像
    * input      : eRecordType:录像类型; pCameraCode:摄像机编号
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 StopRemoteRecord(RECORD_TYPE eRecordType,const IVS_CHAR* pCameraCode)const;

private:
	int m_iPlayHandle;  //播放句柄

	CUserMgr *m_pUserMgr; //用户管理指针对象
	
	unsigned long m_ulPort; //占用的端口

	RECORD_STATUS m_RecordStatus; //录像状态
};

#endif
