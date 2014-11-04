/********************************************************************
filename	: 	IVS_TVWall.h
author		:	z00201790
created		:	2012/12/14
description	:	定义 DLL 应用程序的导出函数;
copyright	:	Copyright (C) 2011-2015
history		:	2012/10/23 初始版本;
*********************************************************************/

#ifndef IVS_TVWALL_H
#define IVS_TVWALL_H

#include "hwsdk.h"
#include "ivs_error.h"
#include "IVS_SDKTVWall.h"

#ifdef IVS_IVSSDKTVWALL_EXPORTS
#define IVS_IVSSDKTVWALL_API __declspec(dllexport)
#else
#define IVS_IVSSDKTVWALL_API __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	/*****************************************************************************
	函数名称：IVS_TVWall_Init
	功能描述：初始化;
	输入参数：NA
	输出参数：NA
	返 回 值：成功:IVS_SUCCEED;
	失败:错误码;
	*****************************************************************************/
	IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_Init(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

	/*****************************************************************************
	函数名称：IVS_TVWall_Release
	功能描述：初始化;
	输入参数：NA
	输出参数：NA
	返 回 值：成功:IVS_SUCCEED;
	失败:错误码;
	*****************************************************************************/
	IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_Release();
	
    /**************************************************************************
    * name       : IVS_TVWall_InitBusiness
    * description: 初始化解码器
    * input      : pParaInfo   初始化参数
    * output     : NA
    * return     : long.       返回码
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_InitBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

	/**************************************************************************
    * name       : IVS_TVWall_StartBusiness
    * description: 命令远程解码器开始解码
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : 成功:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

    /**************************************************************************
    * name       : IVS_TVWall_StopBusiness
    * description: 命令远程解码器停止解码
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : 成功:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo);

    /**************************************************************************
    * name       : IVS_TVWall_PlayAudio
    * description: 播放/停止播放声音
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : 成功:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    //IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_PlayAudio(IPI_AUDIO_CONTROL_REQ *pParaInfo);

    /**************************************************************************
    * name       : IVS_TVWall_SetVolume
    * description: 设置音量
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : 成功:IVS_SUCCEED;
    * remark     : NA
    **************************************************************************/
    //IVS_IVSSDKTVWALL_API IVS_INT32 __stdcall IVS_TVWall_SetVolume(IPI_TUNEUP_VOLUME_REQ* pParaInfo);
}

#endif //IVS_TVWALL_H
