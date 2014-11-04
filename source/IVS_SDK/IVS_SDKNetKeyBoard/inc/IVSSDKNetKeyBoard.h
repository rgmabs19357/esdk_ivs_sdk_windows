/********************************************************************
	filename	: 	IVSSDKNetKeyBoard.h
	author		:	s00191067
	created		:	2011/10/28	
	description	:	IVS提供给第三方使用的接口
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28
*********************************************************************/
#ifndef IVS_SDK_NET_KEY_BOARD_H
#define IVS_SDK_NET_KEY_BOARD_H

/*!< 公共宏、结构体头文件 */
#include "IVSSDKNetKeyBoardDef.h"


#ifdef __cplusplus
extern "C"
{
#endif

    /**************************************************************************
    * name       : SetServerInfo
    * description: 设置服务器IP和端口
    * input      : pszIP 服务器IP地址 iPort 端口
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) int __stdcall  SCCSetServerInfo(const char* pszIP, int iPort);

    /**************************************************************************
    * name       : SetLoginID
    * description: 设置登录ID
    * input      : pszLoginID 登录ID
    * output     : void
    * return     : RET_TRUE 成功，RET_FALSE 失败
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) int __stdcall SetLoginID(const char *pszLoginID);


    /**************************************************************************
    /**************************************************************************
    * name       : SetListenAddr
    * description: 设置服务器端监听IP和端口号
    * input      : iPort服务器端监听端口号
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) void __stdcall IVS_SDK_KB_SetNetKeyBoardListenAddr(u_short iPort);

    /**************************************************************************
    * name       : StartListenThread
    * description: 启动监听线程
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : g_hThread
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) int __stdcall IVS_SDK_KB_StartNetKeyBoardListenThread(u_short ulNetKeyBoardVendor, u_short ulPort, LPCallBack lpCallBack);

    /**************************************************************************
    * name       : ReleaseThread
    * description: 释放线程
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) void __stdcall IVS_SDK_KB_ReleaseThread();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_IVS_SDK_H_
