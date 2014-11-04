/********************************************************************
filename : DeviceParaConfigXMLProcess.h
author : gwx151720
created : 2012/12/03
description : 
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/
#ifndef __DEVICE_PARA_CONFIG_XML_PROCESS_H__
#define __DEVICE_PARA_CONFIG_XML_PROCESS_H__

#include "hwsdk.h"
#include "ivs_xml.h"
#include "SDKDef.h"


class CDeviceParaConfigXMLProcess
{
public:
	// 查询主设备参数解析XML
    static IVS_INT32 GetMainDevConfigParseXml(IVS_DEVICE_CFG* pMainDevCfg, CXml& xml);

	// 设置主设备参数拼接XML
    static IVS_INT32 SetMainParaConfigGetXml( 
                                              const IVS_CHAR* pDomainCode, 
											  const IVS_CHAR* pDevCode, 
											  const IVS_DEVICE_CFG* pDevCfg,
											  const IVS_CHAR* pDevPWD,
											  const IVS_CHAR* pDevRegPWD,
                                              CXml& xmlTemp );
	
	// 查询OSD参数解析XML
	static IVS_INT32 GetOSDParaParseXML(IVS_CAMERA_OSD_CFG* pOSDConfig, CXml& xml);

	// 解析“查选摄像机基本参数”响应
    static IVS_INT32 ParseCameraBriefInfoRsp(CXml& xml,
        IVS_CAMERA_CFG* pCameraCfg);

	// 解析“查选摄像机云台参数”响应
	static IVS_INT32 ParseCameraPTZCfgRsp(CXml& xml,
		IVS_PTZ_CFG* pCameraPTZCfg, const IVS_CHAR* pDomainCode);

	// 解析“查选语音子设备参数”响应
	static IVS_INT32 ParseAudioCfgRsp(CXml& xml,
		IVS_AUDIO_CFG* pAudioCfg);

	// “修改摄像机基本参数”请求xml
	static IVS_INT32 SetCameraBriefCfgGetXml(
		const IVS_CHAR* pDomainCode, 
		const IVS_CHAR* pCameraCode, 
		const IVS_CAMERA_CFG* pCameraCfg,
		CXml& xmlTemp );

	// 解析“查选摄像机基本图像参数”响应
    static IVS_INT32 ParseCameraDispalyCfgRsp(CXml& xml,
        IVS_CAMERA_DISPLAY_CFG* pCameraBriefInfo);

	// "修改摄像机基本图像"请求xml
	static IVS_INT32 SetCameraBasicImgCfgGetXml(
		const IVS_CHAR* pDomainCode, 
		const IVS_CHAR* pCameraCode, 
		const IVS_CAMERA_DISPLAY_CFG* pCameraBasicImgCfg,
		CXml& xmlTemp );

	// "修改云台参数"请求xml
	static IVS_INT32 SetCameraPTZCfgGetXml(
		const IVS_CHAR* pDomainCode, 
		const IVS_CHAR* pCameraCode, 
		const IVS_PTZ_CFG* pCameraBasicImgCfg,
		CXml& reqXml );

	// "修改语音子设备参数"请求xml
	static IVS_INT32 SetAudioCfgGetXml( 
		const IVS_CHAR* pDomainCode, 
		const IVS_CHAR* pCameraCode, 
		const IVS_AUDIO_CFG* pAudioCfg,
		CXml& reqXml );

	// 查询摄像机码流参数解析XML
	static IVS_INT32 CameraStreamCfgParseXML(IVS_CAMERA_STREAM_CFG* pCameraStrCfg, CXml &xml);
	
	// 设置摄像机码流参数拼接XML
	static IVS_INT32 SetCameraStreamCfgGetXML(const IVS_CAMERA_STREAM_CFG* pCameraStrCfg, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, CXml &xml);

	// 设置摄像机码流参数拼接XML（包含事件触发图像质量调整信息）
	static IVS_INT32 SetCameraStreamCfgEventExGetXML(const IVS_CAMERA_STREAM_EVENTEX_CFG* pCameraStrCfgEx, 
		const IVS_CHAR* pDomainCode, 
		const IVS_CHAR* pDevCode, 
		CXml &xml);

	// 设置告警输入子设备拼接XML
	static IVS_INT32 SetAlarmInCfgGetXML(const IVS_ALARM_IN_CFG* pAlarmInCfg, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, CXml &xml);

	// 查询告警输入子设备解析XML
	static IVS_INT32 GetAlarmInCfgParseXML(IVS_ALARM_IN_CFG* pAlarmInCfg, CXml &xml);

	// 设置告警输出子设备拼接XML
	static IVS_INT32 SetAlarmOutCfgGetXML(const IVS_ALARM_OUT_CFG* pAlarmOutCfg, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, CXml &xml);

	// 查询告警输出子设备解析XML
	static IVS_INT32 GetAlarmOutCfgParseXML(IVS_ALARM_OUT_CFG* pAlarmOutCfg, CXml &xml);

	//解析隐私保护的xml
	static IVS_INT32 GetVideoMaskConfigParseXml(IVS_VIDEO_MASK* pVideoMask, CXml& xml);

	//拼装隐私保护XML
	static IVS_INT32 SetVideoMaskCfgGetXML(const IVS_VIDEO_MASK* pVideoMask, const IVS_CHAR* pDomainCode, const IVS_CHAR* pCameraCode, CXml &xml);
	
	// 设置OSD参数拼接XML
	static IVS_INT32 SetCameraOsgCfgGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, const IVS_CAMERA_OSD_CFG* pCameraOsgCfg, CXml &xml);

	// 设置多行OSD参数拼接XML
	static IVS_INT32 SetMultOSDCfgGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, const IVS_CAMERA_MULTI_OSD_CFG* pMultOsgCfg, CXml &xml);

	// 查询多行OSD参数解析XML
	static IVS_INT32 GetMultOSDCfgParseXML(IVS_CAMERA_MULTI_OSD_CFG* pMultOSDConfig, CXml& xml);

	// 设置图像遮挡拼接XML
	static IVS_INT32 SetVideoHideAlarmGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, const IVS_VIDEO_HIDE_ALARM* pVideoHideAlarm, CXml &xml);

	// 查询图像遮挡解析XML
	static IVS_INT32 GetVideoHideAlarmParseXML(IVS_VIDEO_HIDE_ALARM* pVideoHideAlarm, CXml &xml);

	// 解析查询移动侦测响应xml
	static IVS_INT32 ParseMotionDetecXML(CXml& xml, IVS_MOTION_DETECTION* pMotionDetection);

	// 计算运动检测的响应消息中AreaInfo的条数
	static IVS_INT32 GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum);

	// 拼装设置移动侦测请求xml
	static IVS_INT32 SetMotionDetecGetXML(const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, const IVS_MOTION_DETECTION* pMotionDetection, CXml& xml);

	// 修改主设备网络参数拼接XML
	static IVS_INT32 SetDevNetCfgGetXML(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_DEVICE_NET_CFG* pDevNetCfg, CXml &xml);

	// 修改主设备网络参数解析XML
	static IVS_INT32 GetDevNetCfgParseXML(IVS_DEVICE_NET_CFG* pDevNetCfg, CXml &xml);

	// 修改主设备时间参数拼接XML
	static IVS_INT32 SetDevTimeCfgGetXML(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_DEVICE_TIME_CFG* pDevTimeCfg, CXml &xml);

	// 查询主设备时间参数解析XML
	static IVS_INT32 GetDevTimeCfgParseXML(IVS_DEVICE_TIME_CFG* pDevTimeCfg, CXml &xml);

	// 设置串口子设备拼接XML
	static IVS_INT32 SetSerialChannelCfgGetXML(const IVS_CHAR* pSerialCode, const IVS_CHAR* pDomainCode, const IVS_SERIAL_CHANNEL_CFG* pSerialChannelCfg, CXml &xml);
    
	// 查询串口子设备解析XML
	static IVS_INT32 GetSerialChannelCfgParseXML(IVS_SERIAL_CHANNEL_CFG* pSerialChannelCfg, CXml &xml);

	// 设置摄像机扩展图像参数拼接XML
	static IVS_INT32 SetCameraImgCfgGetXML(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CAMERA_IMAGING_CFG* pCameraImgCfg, CXml &xml);

	// 查询摄像机扩展图像参数解析XML
	static IVS_INT32 GetCameraImgCfgParseXML(IVS_CAMERA_IMAGING_CFG* pCameraImgCfg, CXml &xml);

	// 设置摄像机扩展参数拼接XML
	static IVS_INT32 SetCameraExCfgGetXML(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CAMERA_EXTEND_CFG* pCameraExCfg, CXml &xml);

	// 查询摄像机扩展参数解析XML
	static IVS_INT32 GetCameraExCfgParseXML(IVS_CAMERA_EXTEND_CFG* pCameraExCfg, CXml &xml);


	// 摄像机抓拍参数设置拼装XML
	static IVS_INT32 SetCameraSnapshotCfgGetXML(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CAMERA_SNAPSHOT_CFG* pCameraSnapshotCfg, CXml &xml);

	// 摄像机抓拍参数设置解析XML
	static IVS_INT32 GetCameraSnapshotCfgParseXML(IVS_CAMERA_SNAPSHOT_CFG* pCameraSnapshotCfg, CXml &xml);

	// 摄像机前端录像码流参数拼装XML
	static IVS_INT32 SetCameraRecordStreamCfgGetXML(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, IVS_UINT32 uiStreamType, CXml &xml);

	// 摄像机前端录像码流参数设置解析XML
	static IVS_INT32 GetCameraRecordStreamCfgParseXML(IVS_UINT32* pStreamType, CXml &xml);

	// 摄像机FEC参数拼装XML
	static IVS_INT32 SetCameraFECCfgGetXML(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CAMERA_FEC_CFG* pCameraFEC, CXml &xml);

	// 摄像机FEC参数设置解析XML
	static IVS_INT32 GetCameraFECCfgParseXML(IVS_CAMERA_FEC_CFG* pCameraFEC, CXml &xml);

	
private: 
    CDeviceParaConfigXMLProcess();

};

#endif
