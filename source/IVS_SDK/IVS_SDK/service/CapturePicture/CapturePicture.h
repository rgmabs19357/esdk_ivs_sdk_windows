//#pragma once
/********************************************************************
	filename	: 	CapturePicture.h
	author		:	s00193168
	created		:	2013/1/24	
	description	:	定义抓拍管理类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2013/1/24 初始版本;
*********************************************************************/
#ifndef __CAPTUREPICTURE_H__
#define __CAPTUREPICTURE_H__
#include "hwsdk.h"
#include "ivs_error.h"

class CUserMgr;

class CCapturePicture
{
public:
	CCapturePicture(void);
	~CCapturePicture(void);
	// 设置用户对象类
	void SetUserMgr(CUserMgr* pUserMgr);

	/**************************************************************************
    * name       : PlatformSnapshot
    * description: 平台手动抓拍
    * input      : pCameraCode  摄像机编码
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng s00193168
	* remark     : NA
	* history    : 2013-1-24
    **************************************************************************/
	IVS_INT32  PlatformSnapshot(const IVS_CHAR* pCameraCode)const;

	/**************************************************************************
    * name       : SetSnapshotPlan
    * description: 设置平台抓拍计划
    * input      : pCameraCode  摄像机编码
	               pSnapShotPlan 抓拍计划信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng s00193168
	* remark     : NA
	* history    : 2013-1-24
    **************************************************************************/
	IVS_INT32  SetSnapshotPlan(const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan)const;

	/**************************************************************************
    * name       : GetSnapshotPlan
    * description: 查询平台抓拍计划
    * input      : pCameraCode  摄像机编码
	               pSnapShotPlan 抓拍计划信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：songjianfeng s00193168
	* remark     : NA
	* history    : 2013-1-24
    **************************************************************************/
	IVS_INT32  GetSnapshotPlan(const IVS_CHAR* pCameraCode,IVS_SNAPSHOT_PLAN* pSnapShotPlan)const;
	
	// 获取平台抓拍图像列表
	IVS_INT32 GetSnapshotList(const IVS_CHAR* pCameraCode, const IVS_QUERY_SNAPSHOT_PARAM* pQueryParam, IVS_SNAPSHOT_INFO_LIST* pSnapshotList, IVS_UINT32 uiBufSize) const;

	// 删除平台抓拍计划
	IVS_INT32 DelSnapshotPlan(const IVS_CHAR* pCameraCode) const;

	// 删除平台抓拍图片
	IVS_INT32 DeleteSnapshot(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID)const;
private:
	CUserMgr *m_pUserMgr; //lint !e830 //不需要//
};

#endif

