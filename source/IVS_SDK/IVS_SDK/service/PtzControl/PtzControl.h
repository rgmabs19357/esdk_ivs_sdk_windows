/********************************************************************
	filename	: 	PtzControl.h
	author		:	z00201790
	created		:	2012/12/06	
	description	:	定义云台管理类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/06 初始版本;
*********************************************************************/
#ifndef __PTZCONTROL_H__
#define __PTZCONTROL_H__
#include "hwsdk.h"
#include "ivs_error.h"

class CUserMgr;
class CPtzControl
{
public:
	CPtzControl(void);
	~CPtzControl(void);

public:
	// 设置用户对象类
	void SetUserMgr(CUserMgr* pUserMgr);

	/**************************************************************************
    * name       : GetPtzAbsPosition
    * description: 获取云台当前位置
    * input      : pCameraCode  摄像机编码
	* output     : pPTZPosInfo  云台位置信息
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuohaofan zwx211831
	* remark     : Add
	* history    : 2014-3-15
    **************************************************************************/
	IVS_INT32 GetPtzAbsPosition(const IVS_CHAR* pCameraCode, IVS_PTZ_ABSPOSITION* pPTZPosInfo);

	// 云镜控制
	IVS_INT32 PtzControl(const IVS_CHAR* pCameraCode, IVS_INT32 iControlCode,const IVS_CHAR* pControlPara1,const IVS_CHAR* pControlPara2, IVS_PTZ_CONTROL_INFO* pPtzControlInfo);

	/**************************************************************************
    * name       : AddPTZPreset
    * description: 增加预置位
    * input      : pCameraCode  摄像机编码
				   pPresetName  预置位名称
				   uiPresetIndex 预置位索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  AddPTZPreset (const IVS_CHAR* pCameraCode, const IVS_CHAR* pPresetName, IVS_UINT32* uiPresetIndex);

	/**************************************************************************
    * name       : DelPTZPreset
    * description: 删除预置位
    * input      : pCameraCode  摄像机编码
				   uiPresetIndex  预置位索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  DelPTZPreset (const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex);

	/**************************************************************************
    * name       : ModPTZPreset
    * description: 修改预置位
    * input      : pCameraCode  摄像机编码
				   pPTZPreset  预置位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  ModPTZPreset (const IVS_CHAR* pCameraCode, const IVS_PTZ_PRESET* pPTZPreset);

	/**************************************************************************
    * name       : GetPTZPresetList
    * description: 查询预置位列表
    * input      : pCameraCode  摄像机编码
				   pPTZPresetList 预置位信息列表
				   uiBufferSize 缓存大小
				   uiPTZPresetNum 实际预置位个数
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  GetPTZPresetList (const IVS_CHAR* pCameraCode, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiPTZPresetNum);

	/**************************************************************************
    * name       : SetGuardPos
    * description: 设置看守位
    * input      : pCameraCode  摄像机编码
				   pGuardPosInfo  看守位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  SetGuardPos (const IVS_CHAR* pCameraCode, const IVS_GUARD_POS_INFO* pGuardPosInfo);

	/**************************************************************************
    * name       : DelGuardPos
    * description: 删除看守位
    * input      : pCameraCode  摄像机编码
				   pGuardPosInfo  看守位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：xusida WX168257
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  DelGuardPos (const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex);

	/**************************************************************************
    * name       : GetGuardPos
    * description: 获取看守位
    * input      : pCameraCode  摄像机编码
				   pGuardPosInfo  看守位信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  GetGuardPos (const IVS_CHAR* pCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo);

	/**************************************************************************
    * name       : AddCruiseTrack
    * description: 增加巡航轨迹
    * input      : pCameraCode  摄像机编码
				   pCuriseTrackInfo  巡航轨迹信息
				   uiTrackIndex 轨迹索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  AddCruiseTrack (const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, IVS_UINT32* uiTrackIndex);

	/**************************************************************************
    * name       : DelCruiseTrack
    * description: 删除巡航轨迹
    * input      : pCameraCode  摄像机编码
				   uiTrackIndex  轨迹索引
				   uiCruiseType 轨迹类型
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  DelCruiseTrack (const IVS_CHAR* pCameraCode, IVS_UINT32 uiTrackIndex, IVS_UINT32 uiCruiseType);

	/**************************************************************************
    * name       : ModCruiseTrack 
    * description: 修改巡航轨迹
    * input      : pCameraCode  摄像机编码
				   pCuriseTrackInfo  巡航轨迹信息
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  ModCruiseTrack (const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo);

	/**************************************************************************
    * name       : GetCruiseTrackList
    * description: 查询巡航轨迹列表
    * input      : pCameraCode  摄像机编码
				   pCruiseTrackInfoList 巡航轨迹列表
				   uiBufferSize 缓存大小
				   uiCruiseTrackNum 实际查询到的巡航轨迹个数
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  GetCruiseTrackList (const IVS_CHAR* pCameraCode, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiCruiseTrackNum);

	/**************************************************************************
    * name       : GetCruiseTrack
    * description: 查询巡航轨迹信息
    * input      : pCameraCode  摄像机编码
				   uiTrackIndex  轨迹索引
				   pCruiseTrackInfo 轨迹索引
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  GetCruiseTrack (const IVS_CHAR* pCameraCode, IVS_INT32 uiTrackIndex, IVS_CRUISE_TRACK* pCruiseTrackInfo);

	/**************************************************************************
    * name       : SetCruisePlan
    * description: 设置巡航计划
    * input      : pCameraCode  摄像机编码
				   pCruisePlan  巡航计划
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  SetCruisePlan (const IVS_CHAR* pCameraCode, const IVS_CRUISE_PLAN* pCruisePlan);

	/**************************************************************************
    * name       : GetCruisePlan
    * description: 查询巡航计划
    * input      : pCameraCode  摄像机编码
				   pCruisePlan  巡航计划
	* output     : NA
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    * author     ：zhuwenbin z00193167
	* remark     : NA
	* history    : 2013-1-22
    **************************************************************************/
	IVS_INT32  GetCruisePlan (const IVS_CHAR* pCameraCode, IVS_CRUISE_PLAN* pCruisePlan);

private:
	CUserMgr *m_pUserMgr; //lint !e830 //不需要//
};

#endif
