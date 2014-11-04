/********************************************************************
filename	: 	IVS_SDKNetKeyBoard.h 
author		:	x00192614
created		:	2012/12/21	
description	:	SDK对外提供网络键盘相关接口定义
                因为网络键盘接口不对外开放，只供OCX使用，所以不放在IVS_SDK.h中
copyright	:	Copyright (C) 2011-2015
history		:	2012/12/21 初始版本;
*********************************************************************/
#ifndef __IVS_SDK_NETKEYBOARD_H__
#define __IVS_SDK_NETKEYBOARD_H__

#include "hwsdk.h"
#include "IVSSDKNetKeyBoardDef.h"

#ifdef WIN32
#ifdef  IVS_SDK_EXPORTS
#define IVS_SDK_API __declspec(dllexport)
#else
#define IVS_SDK_API __declspec(dllimport)
#endif
#else
//#define IVS_SDK_API __attribute__((visibility))
#define IVS_SDK_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    /*********************************************************************************************
    * function       : SetListenAddr
    * description: 设置服务器端监听IP和端口号
    * input      : iPort服务器端监听端口号
    * output     : NA
    * return     : NA
    * remark     : NA
    *********************************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetNetKeyBoardPort(ULONG ulPort);

    /*********************************************************************************************
    * name       : StartListenThread
    * description: 启动监听线程
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : g_hThread
    * remark     : NA
    *********************************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartNetKeyBoard(ULONG ulNetKeyBoardVendor, ULONG ulPort, LPCallBack lpCallBack);

    /*********************************************************************************************
    * name       : ReleaseThread
    * description: 释放线程
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    *********************************************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopNetKeyBoard();
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__IVS_SDK_NETKEYBOARD_H__
