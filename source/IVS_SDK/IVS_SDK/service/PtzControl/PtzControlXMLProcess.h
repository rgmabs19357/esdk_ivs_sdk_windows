/********************************************************************
	filename	: 	PtzControlXMLProcess.h
	author		:	z00201790
	created		:	2012/12/13
	description	:	定义云台管理类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/13 初始版本;
*********************************************************************/

#ifndef __PTZCONTROL_XML_PROCESS_H__
#define __PTZCONTROL_XML_PROCESS_H__

#include "XmlProcess.h"

class PtzControlProcess
{
public:
	/***********************************************************************************
    * function   : GetPtzPositionGetXML
	* description: 拼装云镜位置的xml
	* input      : pCameraCode 摄像机编码
	* output     : xml  云镜控制的xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetPtzPositionGetXML(const IVS_CHAR* pCameraCode, CXml &xml);

	/**************************************************************************
    * name       : GetPtzPositionParseXML
    * description: 解析云台位置的xml
    * input      : xmlRsq  响应xml
	* output     : pPTZPosInfo 保存响应信息类
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetPtzPositionParseXML(const std::string& strRsp, IVS_PTZ_ABSPOSITION* pPTZPosInfo);

	/***********************************************************************************
    * function       : PtzControlGetXML
	* description: 拼装云镜控制的xml
	* input      : pLoginID 登录ID
	               pCameraCode 摄像机编码
				   iControlCode	云镜控制码
				   pControlPara1 云镜参数1: 1-点动模式  2-连续模式
				   pControlPara2 云镜参数2: 1-步长  2-速度
	* output     : xml  云镜控制的xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 PtzControlGetXML(const IVS_CHAR* pCameraCode,IVS_INT32 iControlCode,const IVS_CHAR* pControlPara1,const IVS_CHAR* pControlPara2,CXml &xml);

	/**************************************************************************
    * name       : PtzControlParseXML
    * description: 解析云镜控制的xml
    * input      : xmlRsq  响应xml
	* output     :   PtzControlInfo 保存响应信息类
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 PtzControlParseXML(const std::string& strRsp, IVS_PTZ_CONTROL_INFO* pPtzControlInfo);

	/**************************************************************************
    * name       : AddPTZPresetGetXML
    * description: 增加预置位的xml
    * input      : pLoginID 登录ID
				   pCameraCode  摄像机编码
				   pPresetName  预置位名称
	* output     : xml  增加预置位的xml
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 AddPTZPresetGetXML(const IVS_CHAR* pCameraCode, const IVS_CHAR* pPresetName,CXml &xml);

	/**************************************************************************
    * name       : AddPTZPresetParseXML
    * description: 解析增加预置位的xml
    * input      : strRsp  响应xml
				   uiPresetIndex  预置位索引
	* output     : 
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 AddPTZPresetParseXML(const std::string& strRsp, IVS_UINT32* uiPresetIndex);

	/**************************************************************************
    * name       : DelPTZPresetGetXML
    * description: 增加预置位的xml
    * input      : pCameraCode  摄像机编码
				   pPresetName  预置位名称
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 DelPTZPresetGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex, CXml &xml);

	/**************************************************************************
    * name       : ModPTZPresetGetXML
    * description: 修改预置位的xml
    * input      : pCameraCode  摄像机编码
				   pPTZPreset  预置位信息
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 ModPTZPresetGetXML(const IVS_CHAR* pCameraCode, const IVS_PTZ_PRESET* pPTZPreset, CXml &xml);

	/**************************************************************************
    * name       : GetPTZPresetListGetXML
    * description: 获取预置位列表的请求xml
    * input      : pCameraCode  摄像机编码
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetPTZPresetListGetXML(const IVS_CHAR* pCameraCode, CXml &xml);

	/**************************************************************************
    * name       : GetPTZPresetListParseXML
    * description: 获取预置位列表的响应xml
    * input      : strRsp  响应XML
				   pPTZPresetList  预置位列表
				   uiBufferSize  大小
				   uiPTZPresetNum 预置位个数
	* output     : 
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetPTZPresetListParseXML(const std::string& strRsp, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 /*uiBufferSize*/, IVS_UINT32* uiPTZPresetNum);

	/**************************************************************************
    * name       : SetGuardPosGetXML
    * description: 设置看守位的请求xml
    * input      : pCameraCode  摄像机编码
				   pGuardPosInfo  看守位信息
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 SetGuardPosGetXML(const IVS_CHAR* pCameraCode, const IVS_GUARD_POS_INFO* pGuardPosInfo, CXml &xml);


		/**************************************************************************
    * name       : DelGuardPosGetXML
    * description: 删除看守位的请求xml
    * input      : pCameraCode  摄像机编码
				   pGuardPosInfo  看守位信息
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 DelGuardPosGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex, CXml &xml);


	/**************************************************************************
    * name       : GetGuardPosGetXML
    * description: 获取看守位的请求xml
    * input      : pCameraCode  摄像机编码
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetGuardPosGetXML(const IVS_CHAR* pCameraCode, CXml &xml);

	/**************************************************************************
    * name       : GetGuardPosParseXML
    * description: 获取看守位的响应xml
    * input      : strRsp 响应xml
				   pGuardPosInfo  看守位信息
	* output     : 
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetGuardPosParseXML(const std::string& strRsp, IVS_GUARD_POS_INFO* pGuardPosInfo);

	/**************************************************************************
    * name       : AddCruiseTrackGetXML
    * description: 增加巡航轨迹的请求xml
    * input      : pCameraCode  摄像机编码
				   pCuriseTrackInfo  轨迹信息
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 AddCruiseTrackGetXML(const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, CXml &xml);

	/**************************************************************************
    * name       : AddCruiseTrackGetParseXML
    * description: 增加巡航轨迹的响应xml
    * input      : strRsp 响应报文
				   uiTrackIndex  轨迹索引
	* output     : xml
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 AddCruiseTrackGetParseXML(const std::string& strRsp, IVS_UINT32* uiTrackIndex);

	/**************************************************************************
    * name       : DelCruiseTrackGetXML
    * description: 删除轨迹xml
    * input      : pCameraCode  摄像机编码
				   uiTrackIndex  轨迹索引
				   uiCruiseType  轨迹类型
	* output     : xml  增加预置位的xml
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 DelCruiseTrackGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiTrackIndex, IVS_UINT32 uiCruiseType, CXml &xml);

	/**************************************************************************
    * name       : ModCruiseTrackGetXML
    * description: 修改轨迹的xml
    * input      : pCameraCode  摄像机编码
				   pCuriseTrackInfo  轨迹信息
	* output     : xml  增加预置位的xml
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 ModCruiseTrackGetXML(const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, CXml &xml);

	/**************************************************************************
    * name       : GetCruiseTrackListGetXML
    * description: 获取轨迹列表请求xml
    * input      : pCameraCode  摄像机编码
	* output     : xml  增加预置位的xml
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetCruiseTrackListGetXML(const IVS_CHAR* pCameraCode, CXml &xml);

	/**************************************************************************
    * name       : GetCruiseTrackListParseXML
    * description: 获取轨迹列表响应xml
    * input      : strRsp 响应报文
				   pCruiseTrackList  内存首地址
				   uiBufferSize  内存大小
				   uiCruiseTrackNum 轨迹个数
	* output     : 
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetCruiseTrackListParseXML(const std::string& strRsp, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 /*uiBufferSize*/, IVS_UINT32* uiCruiseTrackNum);

	/**************************************************************************
    * name       : GetCruiseTrackGetXML
    * description: 获取巡航轨迹的请求xml
    * input      : pCameraCode  摄像机编码
				   uiTrackIndex  轨迹索引
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetCruiseTrackGetXML(const IVS_CHAR* pCameraCode, IVS_INT32 uiTrackIndex, CXml &xml);

	/**************************************************************************
    * name       : GetCruiseTrackParseXML
    * description: 获取巡航轨迹的响应xml
    * input      : strRsp  响应XML
				   pCruiseTrackInfo 轨迹信息
	* output     :   
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetCruiseTrackParseXML(const std::string& strRsp, IVS_CRUISE_TRACK* pCruiseTrackInfo);

	/**************************************************************************
    * name       : SetCruisePlanGetXML
    * description: 设置巡航计划的请求xml
    * input      : pCameraCode  摄像机编码
				   pCruisePlan  巡航计划
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 SetCruisePlanGetXML(const IVS_CHAR* pCameraCode, const IVS_CRUISE_PLAN* pCruisePlan, CXml &xml);

	/**************************************************************************
    * name       : GetCruisePlanGetXML
    * description: 获取巡航计划的请求xml
    * input      : pCameraCode  摄像机编码
	* output     : xml  
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetCruisePlanGetXML(const IVS_CHAR* pCameraCode, CXml &xml);

	/**************************************************************************
    * name       : GetCruisePlanParseXML
    * description: 获取巡航计划的响应xml
    * input      : strRsp  响应xml
				   pCruisePlan 巡航计划
	* output     :   
    * return     : 成功返回IVS_SUCCESSED，失败返回错误码
    **************************************************************************/
	static IVS_INT32 GetCruisePlanParseXML(const std::string& strRsp, IVS_CRUISE_PLAN* pCruisePlan);
};
#endif //__PTZCONTROL_XML_PROCESS_H__
