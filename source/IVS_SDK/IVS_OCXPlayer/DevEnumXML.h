/*****************************************************************
 filename    :    DevEnumXML.h
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备组;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#ifndef __DEV_ENUM_H__
#define __DEV_ENUM_H__

#pragma once
#include "hwsdk.h"


class CDevEnumXML
{
private:
    CDevEnumXML();
public:
    // 根据类型获取
    static IVS_INT32 GetDevTypeEnum(const IVS_CHAR* pDeviceType);

    // 根据类型获取
    static const IVS_CHAR* GetDevType(IVS_INT32 iDeviceType);

    // 获取视频制式
    static IVS_INT32 GetVideoFormatEnum(const IVS_CHAR* pProtocolType);
    
	// 获取视频制式的枚举型
    static const IVS_CHAR* GetVideoFormatType( const IVS_INT32 iFormatType );
  
    // 获取通用状态
    static IVS_BOOL GetCommStatusBool(const IVS_CHAR* pCommStatus);

	// 获取通用状态
    static const IVS_CHAR* GetCommStatusType( IVS_BOOL bEnable );

    //获取排序的枚举型
    static IVS_BOOL GetFieldEnum(const IVS_CHAR* pFieldType);

	// 获取告警类型
    static const IVS_BOOL GetAlamTypeEnum(const IVS_CHAR* cAlam );

	// 获取告警类型
	static const IVS_CHAR* GetAlamTypeCH(  IVS_BOOL bAlam );

	// 获取正反向控制
    static const IVS_CHAR *GetPosNegDirCtlCH(bool bPosNegDirCtl);

	// 获取镜头码流分辨率
    static const IVS_CHAR *GetResolutionCH(IVS_INT32 resolution);

	// 获取编码格式
    static const IVS_CHAR *GetEnCodeModeCH(IVS_INT32 enCodeMode);

	// 获取码率类型
    static const IVS_CHAR *GetBitRateTypeCH(IVS_INT32 bitRateType);

	// 获取编码格式为H264的Profile
    static const IVS_CHAR *GetH264ProfileCH(IVS_INT32 profile);

	// 获取编码格式为MJPEG的Profile
    static const IVS_CHAR *GetMjpegProfileCH(IVS_INT32 profile);

	// 获取主设备协议类型
	static const IVS_CHAR *GetDevProtocolTypeCH(IVS_INT32 typeCode);

	// 获取主设备提供商类型
	static const IVS_CHAR *GetDevVendorTypeCH(IVS_INT32 typeCode);

	// 获取通道类型
    static IVS_VOID  GetChannelType( IVS_INT32 iNum ,IVS_CHAR *pchTemp);

	// 获取正反向控制
    static IVS_INT32 GetPosNegDirCtlEnum(const IVS_CHAR* posNegDirectControl);

	// 获取镜头码流分辨率
    static IVS_INT32 GetResolutionEnum(const IVS_CHAR* resolution);

	// 获取编码格式
    static IVS_INT32 GetEnCodeModeEnum(const IVS_CHAR* enCodeMode);

	// 获取码率类型
    static IVS_INT32 GetBitRateTypeEnum(const IVS_CHAR* bitRateType);

	// 获取编码格式为H264的Profile
    static IVS_INT32 GetH264ProfileEnum(const IVS_CHAR* profile);

	// 获取编码格式为MJPEG的Profile
    static IVS_INT32 GetMjpegProfileEnum(const IVS_CHAR* profile);

	// 获取排序的枚举型
    static IVS_BOOL GetOrderTypeBool( const IVS_CHAR* pLoginType );

	// 获取主设备协议类型
	static IVS_INT32 GetDevProtocolTypeEnum( const IVS_CHAR* typeCode );

	// 获取主设备提供商类型
	static IVS_INT32 GetDevVendorTypeEnum( const IVS_CHAR* typeCode );

	//获取串口类型
	static IVS_INT32 GetSerialTypeEnum( const IVS_CHAR* typeCode );

	//获取串口类型对应的字符串
	static const IVS_CHAR *GetSerialTypeCH(IVS_INT32 typeCode);

	// 获取可靠传输类型从字符串到整形
	static IVS_INT32 GetReliableTransTypeINT(const IVS_CHAR* pTransType);

	// 获取可靠传输类型从整形到字符串
	static const IVS_CHAR* GetReliableTransTypeCHAR(IVS_INT32 iTransType);

	// 获取数据格式类型从字符串到整形
	static IVS_INT32 GetDataFormatINT(const IVS_CHAR* pDataFormat);

	// 获取数据格式类型从整形到字符串
	static const IVS_CHAR* GetDataFormatCHAR(IVS_INT32 iDataFormat);

	// 获取摄像机类型
	static const IVS_CHAR* GetCameraTypeEnum(IVS_INT32 iCameraType);

	// 获取通用是否存在
	static const IVS_CHAR* GetCommIsOrNotType(IVS_INT32 iType);

	// 获取网络类型
	static const IVS_CHAR* GetNetType(IVS_INT32 iNetType);

	// 获取随路音频
	static const IVS_CHAR* GetVoiceInEnable(IVS_INT32 iVoiceInEnable);

	// 获取设备配置大小
	static IVS_UINT32 GetSizeOfConfigType(IVS_UINT32 uiConfigType);

	// 获取通用模式
	static IVS_INT32 GetCommMode(const IVS_CHAR* pCommMode);

	// 获取曝光优先
	static IVS_INT32 GetExposurePriority(const IVS_CHAR* pExposurePriority);

	// 获取通用过滤模式
	static IVS_INT32 GetCommExImgMode(const IVS_CHAR* pCommExImgMode);

	// 获取曝光优先
	static const IVS_CHAR* GetExposurePriorityStr(IVS_INT32 iExposurePriority);

	// 获取通用模式
	static const IVS_CHAR* GetCommModeStr(IVS_INT32 iCommMode);

	// 获取通用过滤模式
	static const IVS_CHAR* GetCommExImgModeStr(IVS_INT32 iCommExImgMode);
};

#endif
