/********************************************************************
	filename	: 	SDKConfig.h
	author		:	x00192614
	created		:	2012/11/24
	description	:	SDKConfig操作类
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/1/10 初始版本
*********************************************************************/
#ifndef __SDKCONFIG_H__
#define __SDKCONFIG_H__

#include "hwsdk.h"
#include "ivs_xml.h"
class CSDKConfig
{
public:
    CSDKConfig(void);
    ~CSDKConfig(void);

    static CSDKConfig& instance();
    
    // 日志根路径获取设置方法
    std::string GetLogPath() { return m_LogPath; }; //lint !e1036 !e1536
    void SetLogPath(std::string strLogPath) { m_LogPath = strLogPath; }; //lint !e1036 !e1536
    /******************************************************************
     function   : LoadConfigFile
     description: 加载配置文件
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 LoadConfigFile();

    /******************************************************************
     function   : GetTimeOutValue
     description: 获取发送消息超时时间
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 GetTimeOutValue()const { return m_uiTimeOut; }

    /******************************************************************
     function   : GetBackDataInterval
     description: 获取备份用户配置文件时间间隔
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 GetBackUpInterval()const { return m_uiBackUpInterval; }
    
	/******************************************************************
     function   : GetIAVideoDelayValue
     description: 获取平台智能分析轨迹叠加实况延时
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 GetPlatformIAVideoDelayValue()const { return m_uiPlatformIAVideoDelayValue; }

    /******************************************************************
     function   : GetIAVideoDelayValue
     description: 获取前端智能分析轨迹叠加实况延时
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 GetPUIAVideoDelayValue()const { return m_uiPUIAVideoDelayValue; }

	/******************************************************************
     function   : GetDefaultLanguageValue
     description: 获取默认语言
     output     : NA
     return     : std::string
    *******************************************************************/
    std::string GetDefaultLanguageValue()const { return m_strDefaultLanguageValue; }

    /******************************************************************
     function   : GetMediaBufferLimit
     description: 获取缓冲区的上下限百分比
     input      : IVS_UINT32 & uiLowerLimit 下限
     input      : IVS_UINT32 & uiUpperLimit 上限
     output     : NA
     return     : void
    *******************************************************************/
    void GetMediaBufferLimit(IVS_UINT32& uiLowerLimit, IVS_UINT32& uiUpperLimit) const
    {
        uiLowerLimit = m_uiLowerLimit;
        uiUpperLimit = m_uiUpperLimit;
    }

	/******************************************************************
     function   : GetFontFamilyValue
     description: 获取字体
     output     : NA
     return     : std::string
    *******************************************************************/
    std::string GetFontFamilyValue()const { return m_strFontFamily; }

	/******************************************************************
     function   : GetFontSizeValue
     description: 获取字体大小
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 GetFontSizeValue()const { return m_uiFontSize; }

	/******************************************************************
     function   : GetRtspLogLevel
     description: 获取rtsp日志级别
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 GetRtspLogLevel()const { return m_uiRtspLogLevel; }

    IVS_INT32 GetUtcTimeSpan(std::string& strStartUtcTime, std::string& strEndUtcTime, const char *pFormat, IVS_INT32 iSecond);

    IVS_INT32 ModifyLogLevel(IVS_ULONG ulLogLevel);
    IVS_INT32 SetSDKDebugLogSwitch(CBB::CXml& oCXml, bool bSwitch);
private:
    /******************************************************************
     function   : InitMember
     description: 初始化配置项
     input      : CBB::CXml & oCXml 配置项CBB::CBB::CXml文件
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 InitMember(CBB::CXml& oCXml);

    /******************************************************************
     function   : SetTimeOutValue
     description: 设置TimeOut
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SetTimeOutValue(CBB::CXml& oCXml);

    /******************************************************************
     function   : SetTimeOutValue
     description: 设置BackUpInterval
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SetBackUpInterval(CBB::CXml& oCXml);

	/******************************************************************
     function   : SetPUIAVideoDelayValue
     description: 设置前端智能分析轨迹叠加实况延时
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SetPUIAVideoDelayValue(CBB::CXml& oCXml);

    /******************************************************************
     function   : SetPlatformIAVideoDelayValue
     description: 设置平台智能分析轨迹叠加实况延时
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SetPlatformIAVideoDelayValue(CBB::CXml& oCXml);

	/******************************************************************
     function   : SetDefaultLanguageValue
     description: 设置默认语言
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
	IVS_INT32 SetDefaultLanguageValue(CBB::CXml& oCXml);

    /******************************************************************
     function   : SetMediaBufferLimit
     description: 设置媒体缓冲区上下限
     input      : CBB::CXml & oCXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SetMediaBufferLimit(CBB::CXml& oCXml);

	/******************************************************************
     function   : SetFontFamilyValue
     description: 设置字体
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
	IVS_INT32 SetFontFamilyValue(CBB::CXml& oCXml);
	
	/******************************************************************
     function   : SetFontSizeValue
     description: 设置字体大小
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SetFontSizeValue(CBB::CXml& oCXml);

	/******************************************************************
     function   : SetRtspLogLevel
     description: 设置rtsp日志级别
     input      : CBB::CXml& oCXml 配置文件CBB::CBB::CXml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 SetRtspLogLevel(CBB::CXml& oCXml);

private:
    IVS_UINT32 m_uiTimeOut;                  // 发送消息超时时间，单位（秒）
    IVS_UINT32 m_uiBackUpInterval;           // 用户文件上传时间间隔，单位（分钟）
    IVS_UINT32 m_uiPlatformIAVideoDelayValue;// 平台智能分析轨迹叠加实况延时，单位（毫秒）
    IVS_UINT32 m_uiPUIAVideoDelayValue;      // 前端智能分析轨迹叠加实况延时，单位（毫秒）
	IVS_UINT32 m_uiFontSize;                 // 字体大小
	std::string m_strFontFamily;             // 字体
	std::string m_strDefaultLanguageValue;   // 默认语言
    std::string m_LogPath;                   // 日志根路径
	bool bInit;

    IVS_UINT32 m_uiLowerLimit;              // 媒体缓冲区下限
    IVS_UINT32 m_uiUpperLimit;              // 媒体缓冲区上限
    IVS_UINT32 m_uiRtspLogLevel;            // rtsp日志级别

};
#endif

