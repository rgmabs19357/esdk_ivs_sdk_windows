/*****************************************************************
 filename    :    OCXPtzControlXMLProcess.h
 author      :    z00193167
 created     :    2013/1/22
 description :    实现云台控制XML的拼装和解析;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/1/22 初始版本
*****************************************************************/
#ifndef __OCX_PTZ_CONTROL_PROCESS_H__
#define __OCX_PTZ_CONTROL_PROCESS_H__
#include "stdafx.h"
#include "SDKDef.h"

class COCXPtzControlXMLProcess
{
public:
    ~COCXPtzControlXMLProcess();
	// 新增预置位拼装xml
	static IVS_INT32 AddPTZPresetGetXML(std::string& strCameraCode, IVS_UINT32 uiPresetIndex, IVS_INT32 iRet, CString& strResult);
	// 查询预置位列表拼装xml
	static IVS_INT32 GetPTZPresetListGetXML(std::string& strCameraCode, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 uiPTZPresetNum, IVS_INT32 iRet, CString& strResult);
	// 设置看守位解析xml
	static IVS_INT32 SetGuardPosParseXML(std::string& strGuardPosInfo, std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo);
	// 删除看守位解析xml
	static IVS_INT32 DelGuardPosParseXML(std::string& strGuardPosInfo, std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo);
    // 获取看守位拼装xml
	static IVS_INT32 GetGuardPosGetXML(std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo, IVS_INT32 iRet,CString& strResult);
	// 增加巡航轨迹解析xml
	static IVS_INT32 AddCruiseTrackParseXML(std::string& strCuriseTrackInfo, std::string& strCameraCode, IVS_CRUISE_TRACK* pCuriseTrackInfo);
	// 增加巡航轨迹拼装xml
	static IVS_INT32 AddCruiseTrackGetXML(IVS_UINT32 uiTrackIndex, IVS_INT32 iRet, CString& strResult);
	// 修改巡航轨迹解析xml
	static IVS_INT32 ModCruiseTrackParseXML(std::string& strCuriseTrackInfo, std::string& strCameraCode, IVS_CRUISE_TRACK* pCuriseTrackInfo);
	// 查询巡航轨迹拼装xml
	static IVS_INT32 GetCruiseTrackGetXML(std::string& strCameraCode, IVS_CRUISE_TRACK* pCruiseTrackInfo, IVS_INT32 iRet, CString& strResult);
	// 查询巡航轨迹信息列表拼装xml
	static IVS_INT32 GetCruiseTrackListGetXML(std::string&strCameraCode, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 uiCruiseTrackNum, IVS_INT32 iRet, CString& strResult);
	// 设置巡航计划解析xml
	static IVS_INT32 SetCruisePlanParseXML(std::string& strCruisePlan, std::string& strCameraCode, IVS_CRUISE_PLAN* pCruisePlan);
	// 获取巡航计划拼装xml
	static IVS_INT32 GetCruisePlanGetXML(std::string& strCameraCode, IVS_CRUISE_PLAN* pCruisePlan, IVS_INT32 iRet, CString& strResult);
    // PTZ控制拼装xml
    static IVS_INT32 GetPTZControlRspXML(IVS_PTZ_CONTROL_INFO& stPtzControlInfo, IVS_INT32 iRet, CString& strResult);

private:
    COCXPtzControlXMLProcess();
    
};

#endif // __OCX_PTZ_CONTROL_PROCESS_H__
