/********************************************************************
filename : DeviceParaConfig.h
author : gwx151720
created : 2012/12/03
description : 
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/
#ifndef __DEVICE_PARA_CONFIG_H__
#define __DEVICE_PARA_CONFIG_H__

#include "hwsdk.h"
#include "SDKDef.h"
#include "SDKSecurityClient.h"

#define SENDRET 0
#define SENDSTR 1

class CUserMgr;
class CDeviceParaConfig
{
public:
	CDeviceParaConfig();
	~CDeviceParaConfig();
public:
    void SetUserMgr(CUserMgr *pUserMgr);
	 
	// 设置设备参数
    IVS_INT32 SetDeviceConfig(const IVS_CHAR* pDevCode, 
                              IVS_UINT32 uiConfigType,
                              const IVS_VOID* pBuffer,
                              IVS_UINT32 uiBufSize)const;
	// 获取语音子设备详细信息
    IVS_INT32 GetAudioDeviceConfig(const IVS_CHAR* pDevCode, 
		                      const IVS_CHAR* pAudioDevCode, 
                              IVS_UINT32 uiConfigType,
                              IVS_VOID* pBuffer,
                              IVS_UINT32 uiBufSize)const;

	// 获取设备参数
	IVS_INT32 GetDeviceConfig(const IVS_CHAR* pDevCode, 
		IVS_UINT32 uiConfigType,
		IVS_VOID* pBuffer,
		IVS_UINT32 uiBufSize)const;
	// 设置主设备参数
    IVS_INT32 SetMainDevConfig(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

private:
    CUserMgr *m_pUserMgr;

	// 校验内存大小
    IVS_INT32 CheckBufferSize(IVS_UINT32 uiConfigType, IVS_UINT32 uiBufSize)const;

	// 查询摄像机基本参数
	IVS_INT32 GetCameraBriefCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode)const;

	// 修改摄像机基本参数
	IVS_INT32 SetCameraBriefCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode)const;

	//查询摄像机码流参数
	IVS_INT32 GetCameraStreamCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询摄像机基本图像参数
	IVS_INT32 GetCameraBasicImgCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode)const;

	// 修改摄像机基本图像参数
	IVS_INT32 SetCameraBasicImgCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode)const;
	
	// 获取云台参数
	IVS_INT32 GetCameraPTZCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode)const;

	// 设置云台参数
	IVS_INT32 SetCameraPTZCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode)const;

	// 查询音频参数(语音设备通信)
	IVS_INT32 GetAudioCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pCameraCode,const IVS_CHAR* pAudioDevCode, const IVS_CHAR* pDomainCode)const;
	
	// 查询音频参数
	IVS_INT32 GetAudioCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode)const;

	// 设置音频参数
	IVS_INT32 SetAudioCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode)const;

	//修改摄像机码流参数
	IVS_INT32 SetCameraStreamCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode) const;

	//修改摄像机码流参数（包含事件触发图像质量调整信息）
	IVS_INT32 SetCameraStreamCfgEventEx(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode) const;

	//查询主设备基本参数
	IVS_INT32 GetMainDevCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 修改告警输入子设备
	IVS_INT32 SetAlarmInCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询告警输入子设备
	IVS_INT32 GetAlarmInCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 修改告警输出子设备
	IVS_INT32 SetAlarmOutCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;
    
	// 查询告警输出子设备
	IVS_INT32 GetAlarmOutCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询OSD参数
	IVS_INT32 GetOSDPara(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode) const;

	// 查询多行OSD参数
	IVS_INT32 GetMultOSDPara(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode) const;

	// 修改多行OSD参数
	IVS_INT32 SetMultOSDPara(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询隐私保护
	IVS_INT32 GetVideoMask(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 修改隐私保护
	IVS_INT32 SetVideoMaskCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 修改OSD参数
	IVS_INT32 SetOsgPara(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 修改图像遮挡
	IVS_INT32 SetVideoHideAlarm(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询图像遮挡
	IVS_INT32 GetVideoHideAlarm(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 设置前段移动监测
	IVS_INT32 SetMotionDetec(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询前端移动监测
	IVS_INT32 GetMotionDetec(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 修改主设备网络参数
	IVS_INT32 SetDevNetConfig(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询主设备网络参数
	IVS_INT32 GetDevNetConfig(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 修改主设备时间参数
	IVS_INT32 SetDevTimeConfig(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询主设备时间参数
	IVS_INT32 GetDevTimeConfig(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 设置串口子设备详细信息
	IVS_INT32 SetSerialChannelConfig(const IVS_VOID* pBuffer, const IVS_CHAR* pSerialChannelCode, const IVS_CHAR* pDomainCode)const;

	// 查询串口子设备详细信息
	IVS_INT32 GetSerialChannelConfig(IVS_VOID*& pBuffer, const IVS_CHAR* pSerialChannelCode, const IVS_CHAR* pDomainCode)const;

	// 设置摄像机扩展图像参数
	IVS_INT32 SetCameraImgCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询摄像机扩展图像参数
	IVS_INT32 GetCameraImgCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 设置摄像机扩展参数
	IVS_INT32 SetCameraExCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询摄像机扩展参数
	IVS_INT32 GetCameraExCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 设置摄像机抓拍参数设置
	IVS_INT32 SetCameraSnapshotCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询摄像机抓拍参数设置
	IVS_INT32 GetCameraSnapshotCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 设置摄像机前端录像码流参数
	IVS_INT32 SetCameraRecordStreamCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询摄像机前端录像码流参数
	IVS_INT32 GetCameraRecordStreamCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 设置摄像机FEC参数
	IVS_INT32 SetCameraFECCfg(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	// 查询摄像机FEC参数
	IVS_INT32 GetCameraFECCfg(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const;

	typedef int (__thiscall CDeviceParaConfig::*FnSetDevicePara)(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const; 

	typedef int (__thiscall CDeviceParaConfig::*FnGetDevicePara)(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode)const; 

	typedef struct
	{
		IVS_UINT32 reqID;
		FnSetDevicePara fnSetDevicePara;
		FnGetDevicePara fnGetDevicePara;
	} REQUEST_NAME;
	static REQUEST_NAME m_reqNames[];
	FnSetDevicePara GetFnSetDevicePara(IVS_UINT32 enReqID) const;
	FnGetDevicePara GetFnGetDevicePara(IVS_UINT32 enReqID) const;
	int HandleSetDevicePara(const IVS_VOID* pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, IVS_UINT32 uiConfigType)const;
	int HandleGetDevicePara(IVS_VOID*& pBuffer, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, IVS_UINT32 uiConfigType)const;

	// 设置主设备参数消息模式
	IVS_INT32 SetMainDevConfigMsg(const IVS_DEVICE_CFG* pMainDevCfg, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const char* pszDestModule, const char* pszDeviceCode, enum BP::DOMAIN_TYPE enDomainType = BP::DOMAIN_NONE)const;

	// 查询主设备参数消息模式
	IVS_INT32 GetMainDevCfgMsg(IVS_DEVICE_CFG* pMainDevCfg, const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, enum BP::DOMAIN_TYPE enDomainType = BP::DOMAIN_NONE)const;

};


#endif
