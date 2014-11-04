/*****************************************************************
 filename    :    OCXDeviceParaConfigXMLProcess.h
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#ifndef __OCX_DEVICE_PARA_CONFIG_XML_PROCESS_H__
#define __OCX_DEVICE_PARA_CONFIG_XML_PROCESS_H__

#pragma once

#include "SDKDef.h"

//sdk内部域编码
#define IVS_SDK_DEV_CODE_LEN 32

class COCXDeviceParaConfigXMLProcess
{
public:
	COCXDeviceParaConfigXMLProcess();
	~COCXDeviceParaConfigXMLProcess();
   
	// 设置主设备基本参数解析XML
    IVS_INT32 SetMainDevConfigParseXml(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询主设备基本参数拼接XML
    IVS_INT32 GetMainDevConfigGetXML(IVS_VOID* &pBuffer, const IVS_CHAR *pDevCode, CXml& xml)const;

	// 查询摄像机码流参数拼接XML
	IVS_INT32 GetCameraStreamCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置摄像机码流参数解析XML
	IVS_INT32 SetCameraStreamCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询摄像机图像显示参数拼接XML
	IVS_INT32 GetCameraDisplayCfgGetXml(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置摄像机图像显示参数解析XML
	IVS_INT32 SetCameraDisplayCfgParseXml(IVS_VOID*& pBuffer, CXml& xml)const;
	
	// 查询摄像机基本参数拼接XML
	IVS_INT32 GetCameraCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;
	
	// 设置摄像机基本参数解析xml
	IVS_INT32 SetCameraCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 获取云台参数拼接XML
	IVS_INT32 GetPTZCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 查询语音子设备详细信息拼接XML
	IVS_INT32 GetAudioCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置云台参数解析XML
	IVS_INT32 SetCameraPTZCfgParseXml(IVS_VOID*& pBuffer, CXml &xml)const;

	// 设置语音子设备参数解析XML
	IVS_INT32 SetAudioCfgParseXml(IVS_VOID*& pBuffer, CXml &xml)const;

	// 解析DevCode
	IVS_INT32 ParseDevCode(const IVS_CHAR *pDevBigCode,
		IVS_CHAR *pDevCode,
		IVS_CHAR* pDomainCode)const;

	// 设置告警输入子设备解析XML
	IVS_INT32 SetAlarmInCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询告警输入子设备拼接XML
	IVS_INT32 GetAlarmInCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置告警输出子设备解析XML
	IVS_INT32 SetAlarmOutCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询告警输出子设备拼接XML
	IVS_INT32 GetALarmOutCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 查询OSD参数拼接XML
	IVS_INT32 GetOSDConfigGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 拼装隐私保护xml
	IVS_INT32 GetVideoMaskConfigGetXml(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 解析修改隐私保护xml
	IVS_INT32 SetVideoMaskConfigParseXml(IVS_VOID*& pBuffer, CXml &xml)const;
	
	// 设置OSD参数解析XML
	IVS_INT32 SetCameraOsgCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 设置图像遮挡解析XML
	IVS_INT32 SetVideHideAlarmParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询图像遮挡拼接XML
	IVS_INT32 GetVideoHideAlarmGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置前端移动侦测
	IVS_INT32 SetMotionDetecParseXML(IVS_VOID*& pBuffer, CXml& xml)const;

	// 查询移动侦测区域个数
	IVS_INT32 GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum)const;

	// 查询前端移动监测拼接XML
	IVS_INT32 GetMotioinDetecGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 修改设备网络参数解析XML
	IVS_INT32 SetDevNetCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询主设备网络参数拼接XML
	IVS_INT32 GetDevNetCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 修改主设备时间参数解析XML
	IVS_INT32 SetDevTimeCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询主设备时间参数拼接XML
	IVS_INT32 GetDevTimeCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 修改串口子设备参数解析XML
	IVS_INT32 SetSerialChannelCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询串口子设备详细信息拼接XML
	IVS_INT32 GetSerialChannelCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置摄像机扩展图像参数解析XML
	IVS_INT32 SetCameraImgCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询摄像机扩展图像参数拼接XML
	IVS_INT32 GetCameraImgCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置摄像机扩展参数解析XML
	IVS_INT32 SetCameraExCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询摄像机扩展参数拼接XML
	IVS_INT32 GetCameraExCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置摄像机抓拍参数设置XML
	IVS_INT32 SetCameraSnapshotCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询摄像机抓拍参数XML
	IVS_INT32 GetCameraSnapshotCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置设备参数统一解析XML
	IVS_INT32 SetDeviceConfigParseXml(IVS_VOID* &pBuffer, IVS_UINT32 uiConfigType, CXml &xml);

	// 查询设备参数统一拼接XML
	IVS_INT32 GetDeviceConfigGetXml(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, IVS_UINT32 uiConfigType, CXml &xml);

	// 设置摄像机前端录像码流参数设置XML
	IVS_INT32 SetCameraRecordStreamCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询摄像机前端录像码流参数XML
	IVS_INT32 GetCameraRecordStreamCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

	// 设置摄像机FEC参数设置XML
	IVS_INT32 SetCameraFECCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const;

	// 查询摄像机FEC参数XML
	IVS_INT32 GetCameraFECCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const;

private:

    

	typedef int (__thiscall COCXDeviceParaConfigXMLProcess::*FnSetDeviceParaParseXML)(IVS_VOID*& pBuffer, CXml &xml)const; 

	typedef int (__thiscall COCXDeviceParaConfigXMLProcess::*FnGetDeviceParaGetXML)(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const; 

	typedef struct
	{
		uint32_t reqID;
		FnSetDeviceParaParseXML fnSetDevicePara;
		FnGetDeviceParaGetXML fnGetDevicePara;
	} REQUEST_NAME;
	static REQUEST_NAME m_reqNames[];
	FnSetDeviceParaParseXML GetFnSetDevicePara(uint32_t enReqID)const;
	FnGetDeviceParaGetXML GetFnGetDevicePara(uint32_t enReqID)const;

};


#endif
