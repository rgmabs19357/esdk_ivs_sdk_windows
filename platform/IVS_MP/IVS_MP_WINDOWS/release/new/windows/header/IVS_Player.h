/********************************************************************
filename    :    IVS_Player.h
author      :    s00191067
created     :    2012/11/12
description :    IVS播放库接口
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/12 初始版本
*********************************************************************/
#ifndef IVS_PLAYER_H
#define IVS_PLAYER_H

#include "IVSPlayerDataType.h"

#ifdef IVS_PLAYER_EXPORTS
#define IVS_PLAYER_API extern "C" __declspec(dllexport)
#else
#define IVS_PLAYER_API extern "C" __declspec(dllimport)
#endif

/*****************************************************************************
 函数名称：IVS_PLAY_Init
 功能描述：初始化;
 输入参数：const char* pLogPath = NULL
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_Init(const char* pLogPath = NULL);

/*****************************************************************************
 函数名称：IVS_PLAY_UnInit
 功能描述：清除库;
 输入参数：NA
 输出参数：NA
 返 回 值：NA
*****************************************************************************/
IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_UnInit();

/*****************************************************************************
 函数名称：IVS_PLAY_GetChannel
 功能描述：获取可用的媒体通道号;
 输入参数：NA
 输出参数：pChannel  通道号;
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetChannel(IVS_ULONG* pChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_FreeChannel
 功能描述：释放指定的媒体通道号;
 输入参数：ulChannel  通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_FreeChannel(IVS_ULONG ulChannel);

/***************************************************************************
 函 数 名 : IVS_PLAY_SetExceptionCallBack
 功能描述 : 设置异常回调函数;
 输入参数 : cbExceptionCallBack         异常回调函数;
            pUser                       用户数据;
 输出参数 : NA
 返 回 值 : 成功:IVS_SUCCEED;失败:IVS_FAIL;
***************************************************************************/
IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_SetExceptionCallBack(
	                                  PLAYER_EXCEPTION_CALLBACK cbExceptionCallBack, 
                                      void* pUser
   								    );

/*****************************************************************************
 函数名称：IVS_PLAY_SetVideoDecodeType
 功能描述：设置视频解码类型;
 输入参数：ulChannel  通道号;
           uiVideoDecoderType 视频解码类型;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetVideoDecodeType(IVS_ULONG ulChannel, IVS_UINT32 uiVideoDecoderType);

/*****************************************************************************
 函数名称：IVS_PLAY_GetVideoDecodeType
 功能描述：获取视频解码类型;
 输入参数：ulChannel  通道号;                
 输出参数：pVideoDecoderType 视频解码类型;
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetVideoDecodeType(IVS_ULONG ulChannel, IVS_UINT32* pVideoDecoderType);

/*****************************************************************************
 函数名称：IVS_PLAY_SetAudioDecodeType()
 功能描述：设置音频解码类型;
 输入参数：ulChannel  通道号;
		   uiAudioDecoderType 音频解码类型;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetAudioDecodeType(IVS_ULONG ulChannel, IVS_UINT32 uiAudioDecoderType);

/*****************************************************************************
 函数名称：IVS_PLAY_GetAudioDecodeType()
 功能描述：获取音频解码类型;
 输入参数：ulChannel  通道号;                
 输出参数：uiAudioDecoderType 音频解码类型;
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetAudioDecodeType(IVS_ULONG ulChannel, IVS_UINT32* pAudioDecoderType);

/*****************************************************************************
 函数名称：IVS_PLAY_OpenStream
 功能描述：打开流通道;
 输入参数：ulChannel  通道号;
           uiPlayeMode  播放模式;
           uiStreamType 输入流方式;
           bHardwareDec 启动硬解标志，true启动，false不启动
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_OpenStream(IVS_ULONG ulChannel, IVS_UINT32 uiPlayeMode, IVS_UINT32 uiSourceType);

/*****************************************************************************
 函数名称：IVS_PLAY_CloseStream
 功能描述：关闭流通道;
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_CloseStream(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_StartPlay
 功能描述：启动播放;
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StartPlay(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_StopPlay
 功能描述：停止播放;
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StopPlay(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_PausePlay
 功能描述：暂停播放
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_PausePlay(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_ResumePlay
 功能描述：恢复播放
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_ResumePlay(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_SetPlaySpeed
 功能描述：恢复播放
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetPlaySpeed(IVS_ULONG ulChannel, double dSpeed);

/*****************************************************************************
 函数名称：IVS_PLAY_StepPalyForward
 功能描述：单步播放前进
 输入参数：ulChannel 通道号;
 输出参数：pRemain 剩余帧数
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StepPlayForward(IVS_ULONG ulChannel, IVS_ULONG *pRemain);

/*****************************************************************************
 函数名称：IVS_PLAY_StepPalyBack
 功能描述：单步播放后退
 输入参数：ulChannel 通道号;
 输出参数：pRemain 剩余帧数
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StepPlayBackward(IVS_ULONG ulChannel, IVS_ULONG *pRemain);

/*****************************************************************************
 函数名称：IVS_PLAY_StartPlayAudio
 功能描述：启动音频播放;
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_PlaySound(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_StopSound
 功能描述：停止音频播放;
 输入参数：ulChannel 通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StopSound(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_SetVolume
 功能描述：设置音量;
 输入参数：ulChannel 通道号;
           uiVolumeValue 音量
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetVolume(IVS_ULONG ulChannel,
                                                    IVS_UINT32 uiVolumeValue);

/*****************************************************************************
 函数名称：IVS_PLAY_GetVolume
 功能描述：获取音量;
 输入参数：ulChannel 通道号;
 输出参数：uiVolumeValue 音量
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetVolume(IVS_ULONG ulChannel, IVS_UINT32* puiVolumeValue);

/*****************************************************************************
 函数名称：IVS_PLAY_SetRawFrameCallBack
 功能描述：设置裸码流回调;
 输入参数：ulChannel           通道号;
           cbReassmCallBack  回调函数指针;
           pUser             用户数据;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetRawFrameCallBack(IVS_ULONG ulChannel, REASSM_CALLBACK cbReassmCallBack, void *pUser);

/*****************************************************************************
 函数名称：IVS_PLAY_SetFrameCallBack
 功能描述：设置解码帧数据回调;
 输入参数：ulChannel           通道号;
           cbReassmCallBack    回调函数指针;
           pUser               用户数据;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetFrameCallBack(IVS_ULONG ulChannel, DECODER_CALLBACK fnDecoderCB, void *pUser);

/*****************************************************************************
 函数名称：IVS_PLAY_SetPlayWindow
 功能描述：设置播放窗口句柄
 输入参数：ulChannel 通道号;
           IVS_HANDLE： 渲染通道句柄
	       hWnd：          渲染窗口
 输出参数：NA
 返 回 值：int
****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetPlayWindow(IVS_ULONG ulChannel, HWND hWnd);

/*****************************************************************************
 函数名称：IVS_PLAY_AddPlayWindow
 功能描述：设置播放窗口句柄
 输入参数：ulChannel 通道号;
	       IVS_HANDLE： 渲染通道句柄
	       hWnd：       窗口句柄
	       nType：      渲染布局类型
	       pLayout：    窗口布局参数
 输出参数：NA
 返 回 值：int
****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_AddPlayWindow(IVS_ULONG ulChannel, HWND hWnd, int nType, void *pLayout);

/*****************************************************************************
 函数名称：IVS_PLAY_AddPlayWindowPartial
 功能描述：增加局部图像播放窗口
 输入参数：ulChannel 通道号;
	      IVS_HANDLE： 渲染通道句柄
	      hWnd：       窗口句柄
	      pPartial：   局部图片参数
 输出参数：NA
 返 回 值：int
****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_AddPlayWindowPartial(IVS_ULONG ulChannel, HWND hWnd, LAYOUT_DRAW_PARTIAL *pPartial);

/*****************************************************************************
 函数名称：IVS_PLAY_UpdateWindowPartial
 功能描述：更新局部放大源矩形
 输入参数：ulChannel 通道号
	     IVS_HANDLE： 渲染通道句柄
	     hWnd：       窗口句柄
	     pPartial：   局部参数
 输出参数：NA
 返 回 值：int
****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_UpdateWindowPartial(IVS_ULONG ulChannel, HWND hWnd, LAYOUT_DRAW_PARTIAL *pPartial);

/*****************************************************************************
 函数名称：IVS_PLAY_DelPlayWindow
 功能描述：删除播放窗口句柄
 输入参数：ulChannel 通道号;
	       hWnd：    窗口句柄
 输出参数：NA
 返 回 值：int
****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_DelPlayWindow(IVS_ULONG ulChannel, HWND hWnd);

/*****************************************************************************
 函数名称：IVS_PLAY_SetDisplayAspectRatio
 功能描述：按通道和窗口设置视频显示比例
 输入参数：ulChannel:    通道号;
	     IVS_HANDLE：   渲染通道句柄
	     hWnd：         窗口句柄
		 uiScale:       显示比例
 输出参数：NA
 返 回 值：IVS_INT32
****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetDisplayAspectRatio(IVS_ULONG ulChannel, IVS_UINT32 uiScale);

/*****************************************************************************
 函数名称：IVS_PLAY_InputRtpData
 功能描述：向播放库输入RTP流;
 输入参数：ulChannel    通道号;
           pBuf         缓存;
		   ulSize       缓存大小;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_InputRtpData(IVS_ULONG ulChannel, IVS_CHAR *pBuf, IVS_UINT32 uiSize);

/*****************************************************************************
 函数名称：IVS_PLAY_InputFrameData
 功能描述：向播放库输入帧数据(音视频);
 输入参数：ulChannel    通道号;
           pstRawFrameInfo 未解码帧数据信息;
		   pFrame       帧数据
		   uFrameSize   帧数据长度
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_InputFrameData(IVS_ULONG ulChannel, IVS_RAW_FRAME_INFO* pstRawFrameInfo, IVS_CHAR* pFrame, IVS_UINT32 uFrameSize);

/*****************************************************************************
 函数名称：IVS_OpenFileStream
 功能描述：打开文件;
 输入参数：ulChannel    通道号;
 输入参数：pFileName    文件名;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_OpenFile(IVS_ULONG ulChannel, const IVS_CHAR* pFileName);

/*****************************************************************************
 函数名称：IVS_PLAY_SetFileTimeRange
 功能描述：设置文件播放时间范围
 输入参数：ulChannel    通道号;
 输入参数：pFileName    文件名;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetFileTimeRange(IVS_ULONG ulChannel, 
	unsigned int beg, unsigned int end);

/*****************************************************************************
 函数名称：IVS_SetWaterMarkVerify()
 功能描述：设置开启/停止校验水印
 输入参数：ulChannel 通道号:  
         ：iStart 0-开始校验，1-停止校验
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetWaterMarkVerify(IVS_ULONG ulChannel, IVS_INT32 iStart);

/*****************************************************************************
 函数名称：IVS_PLAY_GetMediaInfo()
 功能描述：获取视频质量参数
 输入参数：ulChannel 通道号:  
 输出参数：pMediaInfo 视频质量参数
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetMediaInfo(IVS_ULONG ulChannel,IVS_MEDIA_INFO* pMediaInfo);

/*****************************************************************************
 函数名称：IVS_PLAY_GetLostPacketRate()
 功能描述：获取当前播放时间
 输入参数：ulChannel 通道号
 输出参数：puPlayTime 当前播放时间
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetPlayedTime(IVS_ULONG ulChannel,IVS_UINT64 *puPlayTime);

/*****************************************************************************
 函数名称：IVS_PLAY_GetCurrentFrameTick
 功能描述：获取当前帧时间戳
 输入参数：ulChannel 通道号
 输出参数：pTick 当前时间戳
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetCurrentFrameTick(IVS_ULONG ulChannel, IVS_UINT64 *pTick);

IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetPlayTime(IVS_ULONG ulChannel, time_t tm);

IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetDownLoadTimeSpan(IVS_ULONG ulChannel, time_t tmBeg, time_t tmEnd, IVS_BOOL bReportFinsh);


/*****************************************************************************
 函数名称：IVS_PLAY_SetPlayQualityMode()
 功能描述：设置实况播放模式：清晰、流畅
 输入参数：ulChannel 通道号
		   iPlayMode 播放模式 参考：IVS_PLAY_MODE
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetPlayQualityMode(IVS_ULONG ulChannel,IVS_INT32 iPlayQualityMode);

/*****************************************************************************
 函数名称：IVS_PLAY_HasHwDecAbility()
 功能描述：是否支持GPU硬解加速
 输入参数：NA
 输出参数：NA
 返 回 值：支持:TRUE;不支持:FALSE;
 *****************************************************************************/
IVS_PLAYER_API VIDEO_DECODE_MODE __stdcall IVS_PLAY_GetDecodeAbility();

/*****************************************************************************
 函数名称：IVS_PLAY_HasHwDecAbility()
 功能描述：设置硬解标志
 输入参数：bHardwareDec：硬解标志。开启:TRUE;关闭:FALSE;
 输出参数：NA
 返 回 值：NA
 *****************************************************************************/
IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_SetHardwareDec(IVS_BOOL bHardwareDec);

/*****************************************************************************
 函数名称：IVS_PLAY_ShowPlayStatis()
 功能描述：播放界面显示统计信息
 输入参数：NA
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_ShowPlayStatis(IVS_ULONG ulChannel, IVS_BOOL enable);

/*****************************************************************************
 函数名称：IVS_PLAY_ResetData
 功能描述：清空数据;
 输入参数：ulChannel    通道号;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_ResetData(IVS_ULONG ulChannel);

IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_SetResetFlag(IVS_ULONG ulChannel, bool bIsReset);
/*****************************************************************************
 函数名称：IVS_PLAY_StartRecord
 功能描述：开始录像
 输入参数：NA
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 IVS_PLAY_StartRecord(IVS_ULONG ulChannel, const char *pCamCode, const char *pCamName, 
	      const char *pName, const IVS_LOCAL_RECORD_PARAM *pParameter, bool bAssociatedAudio);

/*****************************************************************************
函数名称：IVS_PLAY_StopRecord
功能描述：停止录像
输入参数：NA
输出参数：NA
返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_StopRecord(IVS_ULONG ulChannel);

/*****************************************************************************
函数名称：IVS_PLAY_GetRecordStatis
功能描述：获取录像信息
输入参数：NA
输出参数：NA
返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 IVS_PLAY_GetRecordStatis(IVS_ULONG ulChannel, 
	        IVS_UINT32 *pTime, IVS_UINT32 *pSpeed, IVS_UINT64 *pTotalSz);

/*****************************************************************************
 函数名称：IVS_PLAY_LocalSnapshot
 功能描述：本地抓拍单张图片
 输入参数：pSavePath			图片保存路径；
		 ulSnapshotFormat	图片保存类型
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_LocalSnapshot(IVS_ULONG ulChannel, const char *pSavePath, IVS_ULONG ulSnapshotFormat);

/*****************************************************************************
 函数名称：IVS_PLAY_LocalSnapshotRect
 功能描述：本地抓拍单张图片
 输入参数：pSavePath			图片保存路径；
		 ulSnapshotFormat	图片保存类型
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_LocalSnapshotRect(IVS_ULONG ulChannel, const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT *Rct);


/*****************************************************************************
函数名称：IVS_PLAY_StopDecryptData
功能描述：停止视频解密
输入参数：ulChannel   通道号;
输出参数：NA
返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_StopDecryptData(IVS_ULONG ulChannel);

/*****************************************************************************
函数名称：IVS_PLAY_SetSecretKey
功能描述：设置视频解密密钥（统一针对RTP包数据进行解密）
输入参数：ulChannel   通道号;
		  ulDecType   解密方法
					  0:扰码(包头密文)
					  1:扰码(包头明文)
					  2:AES256(全码流)
					  3:AES256(部分码流)
		  pSecretKey  密钥
输出参数：NA
返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
*****************************************************************************/
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_SetSecretKey(IVS_ULONG ulChannel,IVS_INT32 iDecType,const IVS_CHAR* pSecretKey);

/*****************************************************************************
 函数名称：IVS_PLAY_SetDrawCallBack
 功能描述：设置绘图回调
 输入参数：NA
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetDrawCallBack(IVS_ULONG ulChannel, 
	                     DRAW_CALLBACK callback, void *pUser);

IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetDrawCallBackEx(IVS_ULONG ulChannel,
	                     HWND hWnd, DRAW_CALLBACK callback, void *pUser);

/*****************************************************************************
 函数名称：IVS_PLAY_SetDelayFrameNum
 功能描述：设置延时解码的帧数
 输入参数：ulChannel      通道号;
           uDelayFrameNum 延时解码的帧数;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetDelayFrameNum(IVS_ULONG ulChannel, 
	                                                IVS_UINT32 uDelayFrameNum);

/*****************************************************************************
 函数名称：IVS_PLAY_SetVoiceVolume
 功能描述：设置语音对讲的音量
 输入参数：ulVolumeValue      音量值，范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetVoiceVolume(IVS_ULONG ulVolumeValue);

/*****************************************************************************
 函数名称：IVS_PLAY_GetVoiceVolume
 功能描述：获取语音对讲的音量
 输入参数：无
 输出参数：pVolumeValue       音量值，范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetVoiceVolume(IVS_UINT32* pVolumeValue);

/*****************************************************************************
 函数名称：IVS_PLAY_SetMicVolume
 功能描述：设置麦克风的音量
 输入参数：ulVolumeValue       音量值， 范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetMicVolume(IVS_UINT32 ulVolumeValue);

/*****************************************************************************
 函数名称：IVS_PLAY_GetMicVolume
 功能描述：获取麦克风的音量
 输入参数：无
 输出参数：pVolumeValue    音量值， 范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetMicVolume(IVS_UINT32* pVolumeValue);

/*****************************************************************************
 函数名称：IVS_PLAY_SetVoiceProtocolType
 功能描述：设置语音传输协议
 输入参数：ulVoiceProtocolType             语音传输协议
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetVoiceProtocolType(IVS_UINT32 uVoiceProtocolType);

/*****************************************************************************
 函数名称：IVS_PLAY_GetRecvVoiceAddr
 功能描述：获取接收语音的地址（IP和端口号）
 输入参数：ulPort          通道号
 输出参数：pRecvVoiceIp    接收语音的IP
		   pRecvVoicePort  接收语音的端口号
 返 回 值： ULONG
*****************************************************************************/
//IVS_PLAYER_API IVS_INT32 IVS_PLAY_GetRecvVoiceAddr(IVS_ULONG* pRecvVoicePort);

/*****************************************************************************
 函数名称：IVS_PLAY_FreeRecvVoiceAddr
 功能描述：释放接收语音的地址
 输入参数：ulPort             通道号
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
//IVS_PLAYER_API IVS_INT32 IVS_PLAY_FreeRecvVoiceAddr();

/*****************************************************************************
 函数名称：IVS_PLAY_SetVoiceDecoderType
 功能描述：设置语音解码类型
 输入参数：ulVoiceDecoderType             语音解码类型
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetVoiceDecoderType(IVS_ULONG ulVoiceDecoderType);

/*****************************************************************************
 函数名称：IVS_PLAY_StartVoiceTalkback
 功能描述：开始语音对讲         
 输入参数：pLocalIp     本地语音的IP地址
		   ulLocalPort  本地语音的端口
		   pRemoteIp    远端语音的IP地址
		   ulRemotePort 远端语音的端口
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StartVoiceTalkback(IVS_CHAR* pLocalIp, IVS_ULONG ulLocalPort, 
                                                IVS_CHAR* pRemoteIp, IVS_ULONG ulRemotePort);

/*****************************************************************************
 函数名称：IVS_PLAY_StopVoiceTalkback
 功能描述：停止语音对讲         
 输入参数：无
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StopVoiceTalkback();

/*****************************************************************************
 函数名称：IVS_PLAY_StartVoiceBroadcast()
 功能描述：开始语音广播        
 输入参数：pLocalIp        本地语音的IP地址
           ulLocalPort     本地语音的端口
           ulBroadcastType 语音广播类型(0为实时广播，1为文件广播)
           pFileName       语音广播文件名(绝对路径)
           bCycle          是否循环播放(0不循环播放 1循环播放,实时广播不支持循环)
 输出参数：无
 返 回 值： IVS_INT32
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StartVoiceBroadcast(
                                                    const IVS_CHAR* pLocalIp,
                                                    IVS_ULONG ulLocalPort, 
								                    const IVS_CHAR* pRemoteIp,
                                                    IVS_ULONG ulRemotePort,
                                                    IVS_ULONG ulBroadcastType,
                                                    const IVS_CHAR* pFileName,
                                                    bool bCycle);

/*****************************************************************************
 函数名称：IVS_PLAY_StopVoiceBroadcast()
 功能描述：停止语音广播         
 输入参数：pRemoteIp    语音设备IP
           ulRemotePort 语音设备端口号
 输出参数：无
 返 回 值： IVS_INT32
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_StopVoiceBroadcast(
                                                    IVS_CHAR* pRemoteIp,
                                                    IVS_ULONG ulRemotePort);

/*****************************************************************************
 函数名称：IVS_PLAY_GetSourceBufferRemain()
 功能描述：获取未解码帧缓冲区剩余的帧数         
 输入参数：无
 输出参数：无
 返 回 值：IVS_INT32
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_GetSourceBufferRemain(
                                                    IVS_ULONG ulChannel,
                                                    IVS_UINT32 &uBufferCount);

/***************************************************************************
 函 数 名 : IVS_PLAY_SetEventCallBack
 功能描述 : 设置事件回调函数;
 输入参数 : cbEventCallBack         事件回调函数;
            pUser                   用户数据;
 输出参数 : NA
 返 回 值 : 成功:IVS_SUCCEED;失败:IVS_FAIL;
***************************************************************************/
IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_SetEventCallBack(PLAYER_EVENT_CALLBACK cbEventCallBack, void* pUser);

/*****************************************************************************
 函数名称：IVS_PLAY_SetMediaBufferLimit
 功能描述：设置媒体缓冲区的上下限百分比
 输入参数：uiLowerLimit 下限百分比
           uiUpperLimit 上限百分比
 输出参数：无
 返 回 值： IVS_VOID
*****************************************************************************/
IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_SetMediaBufferLimit(IVS_UINT32 uiLowerLimit, 
                                                               IVS_UINT32 uiUpperLimit);

/*****************************************************************************
 函数名称：IVS_PLAY_StopNotifyBuffEvent
 功能描述：停止通知缓冲区事件
 输入参数：NA
 输出参数：无
 返 回 值： IVS_VOID
*****************************************************************************/
IVS_PLAYER_API IVS_VOID __stdcall IVS_PLAY_StopNotifyBuffEvent(IVS_ULONG ulChannel);


/*****************************************************************************
 函数名称：IVS_PLAY_SetMeidaAttribute
 功能描述：设置媒体信息
 输入参数：NA
 输出参数：无
 返 回 值： IVS_VOID
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetMediaAttribute(IVS_ULONG ulChannel,
	                                                          const MEDIA_ATTR *pVideoAttr, 
                                                              const MEDIA_ATTR *pAudioAttr);

/*****************************************************************************
 函数名称：IVS_PLAY_SetWaitForVertiBlank
 功能描述：设置垂直同步使能
 输入参数：NA
 输出参数：无
 返 回 值： IVS_INT32
*****************************************************************************/
IVS_PLAYER_API  IVS_INT32 __stdcall IVS_PLAY_SetWaitForVertiBlank(IVS_ULONG ulChannel, int bEnable);

/*****************************************************************************
 函数名称：IVS_PLAY_ReSetFlowContrlFlag
 功能描述：清空流控标志
 输入参数：NA
 输出参数：无
 返 回 值： IVS_VOID
*****************************************************************************/
IVS_PLAYER_API  IVS_VOID __stdcall IVS_PLAY_ReSetFlowContrlFlag(IVS_ULONG ulChannel);

/*****************************************************************************
 函数名称：IVS_PLAY_SetVideoEffect
 功能描述：设置视频质量
 输入参数：NA
 输出参数：无
 返 回 值：IVS_INT32
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __stdcall IVS_PLAY_SetVideoEffect(IVS_ULONG ulChannel, IVS_LONG lBrightValue, 
														   IVS_LONG lContrastValue, IVS_LONG lSaturationValue, 
														   IVS_LONG lHueValue);
#endif //IVS_PLAYER_H
