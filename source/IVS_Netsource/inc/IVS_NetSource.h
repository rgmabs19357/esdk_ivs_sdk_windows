/********************************************************************
	filename	: 	IVS_NetSource.h
	author		:	s00191067
	created		:	2011/10/28	
	description	:	IVS网络收流库接口
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28
*********************************************************************/
#ifndef IVS_NETSOURCE_H
#define IVS_NETSOURCE_H

#include "hwsdk.h"
#include "ivs_error.h"
#include "NetSourceCommon.h"

#ifdef IVS_NETSOURCE_EXPORTS
#define IVS_NETSOURCE_API extern "C" __declspec(dllexport)
#else
#define IVS_NETSOURCE_API extern "C" __declspec(dllimport)
#endif

/*****************************************************************************
 函数名称：IVS_NETSOURCE_Init
 功能描述：初始化;
 输入参数：const char* pLogPath = NULL
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;
           失败:IVS_FAIL,PLAYER_RET_WSA_START_ERROR;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_Init(const char* pLogPath);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_UnInit
 功能描述：清空收流库;
 输入参数：NA
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_VOID __stdcall IVS_NETSOURCE_UnInit();

/*****************************************************************************
 函数名称：IVS_NETSOURCE_GetChannel
 功能描述：获取可用通道;
 输入参数：NA
 输出参数：pChannel 
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_GetChannel(IVS_ULONG* pChannel);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_FreeChannel
 功能描述：释放指定通道;
 输入参数：ulChannel 
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_FreeChannel(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_SetProtocolType
 功能描述：设置媒体流传输协议类型;
 输入参数：ulChannel        通道号
           uiProtocolType 协议类型;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_SetProtocolType(IVS_ULONG ulChannel, IVS_UINT32 uiProtocolType);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_GetProtocolType
 功能描述：获得媒体流传输协议类型;
 输入参数：ulChannel  通道号;              
 输出参数：pProtocolType 协议类型;
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_GetProtocolType(IVS_ULONG ulChannel, IVS_UINT32* pProtocolType);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_SetLocalRecvStreamIP
 功能描述：设置本地接收码流的Ip地址;
 输入参数：ulChannel  通道号;   
           pLocalRecvStreamIp  接收码流的ip;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_SetLocalRecvStreamIP(IVS_ULONG ulChannel, IVS_CHAR* pLocalRecvStreamIp);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_GetLocalRecvStreamIP
 功能描述：设置本地接收码流的Ip地址;
 输入参数：ulChannel  通道号;   
 输出参数：pLocalRecvStreamIp  接收码流的ip;
 返 回 值：NA
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_GetLocalRecvStreamIP(IVS_ULONG ulChannel, IVS_CHAR* pLocalRecvStreamIp);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_GetLocalRecvStreamAddr
 功能描述：获取本地接收码流的地址IP和端口号;
 输入参数：ulChannel 通道号;
 输出参数：pLocalRecvStreamVideoRtpPort 接收视频码流的RTP端口号;
           pLocalRecvStreamAudioRtpPort 接收音频码流的RTP端口号;
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_GetLocalRecvStreamPort(IVS_ULONG ulChannel, 
                                                                           IVS_UINT32* pLocalRecvStreamVideoRtpPort, 
                                                                           IVS_UINT32* pLocalRecvStreamAudioRtpPort);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_SetLocalRecvStreamAddr
 功能描述：设置本地接收码流的地址IP和端口号;
 输入参数：ulChannel 通道号;
		   uiLocalRecvStreamVideoRtpPort 接收视频码流的RTP端口号;
           uiLocalRecvStreamAudioRtpPort 接收音频码流的RTP端口号;
 输出参数：NULL
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_SetLocalRecvStreamPort(IVS_ULONG ulChannel, 
                                                                           IVS_UINT32 uiLocalRecvStreamVideoRtpPort, 
                                                                           IVS_UINT32 uiLocalRecvStreamAudioRtpPort);


/*****************************************************************************
 函数名称：IVS_NETSOURCE_OpenNetStream
 功能描述：打开网络流;
 输入参数：ulChannel                         通道号;
		   pRemoteSendStreamIp               源端发送码流IP;
		   uiRemoteSendStreamVideoPort       源端发送视频码流RTP端口号;
           uiRemoteSendStreamAudioPort       源端发送音频码流RTP端口号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_OpenNetStream(IVS_ULONG ulChannel, IVS_CHAR* pRemoteSendStreamIp, 
                                                                  IVS_UINT32 uiRemoteSendStreamVideoPort, 
                                                                  IVS_UINT32 uiRemoteSendStreamAudioPort);

/*****************************************************************************
 函数名称：IVS_NETSOURCE_CloseNetStream
 功能描述：关闭网络流;
 输入参数：ulChannel             通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_CloseNetStream(IVS_ULONG ulChannel);


/*****************************************************************************
 函数名称：IVS_NETSOURCE_StartRecvStream
 功能描述：启动收流;
 输入参数：ulChannel             通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED 失败:IVS_FAIL;
*****************************************************************************/
IVS_NETSOURCE_API IVS_INT32 __stdcall IVS_NETSOURCE_StartRecvStream(IVS_ULONG ulChannel);


/***************************************************************************
 函 数 名 : IVS_NETSOURCE_SetExceptionCallBack
 功能描述 : 设置异常回调函数;
 输入参数 : cbExceptionCallBack         异常回调函数;
            pUser                       用户参数，回调函数中返回;
 输出参数 : NA
 返 回 值 : NA
***************************************************************************/
IVS_NETSOURCE_API IVS_VOID __stdcall IVS_NETSOURCE_SetExceptionCallBack(NET_EXCEPTION_CALLBACK cbExceptionCallBack, 
                                                                        IVS_VOID* pUser);

/***************************************************************************
 函 数 名 : IVS_NETSOURCE_SetDataCallBack
 功能描述 : 设置媒体流回调函数;
 输入参数 : ulChannel          通道号;
            cbDataCallBack    媒体流回调函数;
            pUser             用户参数，回调函数中返回;
 输出参数 : NA
 返 回 值 : NA
***************************************************************************/
IVS_NETSOURCE_API IVS_VOID __stdcall IVS_NETSOURCE_SetDataCallBack(IVS_ULONG ulChannel, 
                                                                   NET_DATA_CALLBACK cbDataCallBack, 
                                                                   IVS_VOID* pUser);

/***************************************************************************
 函 数 名 : IVS_NETSOURCE_SetSessionInfo
 功能描述 : 设置SessionUrl和SessionId;
 输入参数 : ulChannel          通道号;
           pSessionUrl       URL;
           pSessionId        ID值;
 输出参数 : NA
 返 回 值 : NA
***************************************************************************/
IVS_NETSOURCE_API IVS_VOID __stdcall IVS_NETSOURCE_SetSessionInfo(IVS_ULONG ulChannel, 
                                                                  IVS_CHAR* pSessionUrl, 
                                                                  IVS_CHAR* pSessionId);
#endif // IVS_NETSOURCE_H
