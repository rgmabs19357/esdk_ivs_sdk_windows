/********************************************************************
	filename	: 	PlaybackMgr.h
	author		:	s00193168
	created		:	2012/12/06	
	description	:	定义回放业务管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本;
*********************************************************************/
#ifndef __PLAYBACK_MGR_H__
#define __PLAYBACK_MGR_H__


#include "Playback.h"
#include "UserInfo.h"
#include "hwsdk.h"
#include "SDKDef.h"


typedef struct stVodPlayInfo
{
	IVS_UINT32              ulPlayType;			    //回放类型 added by l0076138
	HWND                    hWnd;					//播放句柄
	IVS_UINT32              ulPort;                 //端口号
	IVS_BOOL                bPause;                 //是否处于暂停回放状态
	IVS_BOOL                bFirstGetTime;		    //是否是第一次get time(1 是第一次 0 不是第一次)
	IVS_BOOL                bUserFirstGetTime;		//是否是用户第一次get time(1 是第一次 0 不是第一次)
	IVS_FLOAT               fPlaySpeed;             //回放速度
	//CVodPlayTime *m_pVodPlayTime;                 //获取回放的第一帧的时间戳
	IVS_DOUBLE              dVedioStartTime;        //用于保存录像文件最开始时间，用于录像拖拽
	IVS_DOUBLE              dStartTime;             //录像文件的开始时间，取外部传进来的时间
	IVS_DOUBLE              dEndTime;               //录像文件的开始时间，取外部传进来的时间
	IVS_DOUBLE              dPlayTime;              //当前播放时间(绝对时间)
	IVS_UINT32              ulFirstTime;			//最开始获得的时间戳
	IVS_UINT32              ulLastTime;             //最后获取的时间戳
	time_t t;					                    //硬解上墙时间标示
	IVS_INT32               iHandle;                //成功返回的句柄
	//PlayBackCallBackCB      fCallBackFun;           //裸码流回调函数
	//PlayBackCallBackYUV     fCallBackYUVFun;        //YUV回调函数
	void*                   pUserDataCB;            //用户数据(裸码流)
	void*                   pUserDataYUV;           //用户数据(YUV)
	SourcePlayType          iStartType;             // 打开方式(有句柄/CB/YUV)
	IVS_CHAR                szCameraCode[33];		//摄像机ID add by wKF71044
	IVS_CHAR                szLocalPlayFileName[MAX_PATH];	  //本地录像回放文件名
}ST_VOD_PLAY_INFO;

//窗口句柄到端口号的映射表定义
typedef std::map<HWND, ST_VOD_PLAY_INFO*> MAP_HWND_TO_PORT;
typedef std::pair<HWND, ST_VOD_PLAY_INFO*> PAIR_HWND_TO_PORT;
typedef std::map<int, ST_VOD_PLAY_INFO*> MAP_PLAYHWND_TO_PORT;
typedef std::pair<int, ST_VOD_PLAY_INFO*> PAIR_PLAYHWND_TO_PORT;


class CPlaybackMgr
{
public:
	CPlaybackMgr(void);
	virtual ~CPlaybackMgr(void);
public:
	void SetUserMgr(CUserMgr* pUserMgr);

	/******************************************************************
	function : StartPlatformPlayBack
	description: 开始平台录像回放;
	input : pPlaybackParam,回放参数；pCameraCode，摄像机编号；hWnd，窗体句柄；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 StartPlatformPlayBack(const IVS_CHAR* pCameraCode,
                                    const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                    HWND hWnd,
                                    IVS_ULONG* pHandle);

	IVS_INT32 StartPlatformPlayBackCB(const IVS_CHAR* pCameraCode,
									  const IVS_PLAYBACK_PARAM* pPlaybackParam,
									  PlayBackCallBackRaw fPlayBackCallBackRaw,
									  void* pUserData, 
									  IVS_ULONG* pHandle);

	IVS_INT32 StartPlatformPlayBackCBFrame(const IVS_CHAR* pCameraCode,
										   const IVS_PLAYBACK_PARAM* pPlaybackParam,
										   PlayBackCallBackFrame fPlayBackCallBackFrame,
										   void* pUserData, 
										   IVS_ULONG* pHandle);

	/******************************************************************
	function : StartbackupPlayBack
	description: 开始备份录像回放;
	input : pPlaybackParam,回放参数；pCameraCode，摄像机编号；hWnd，窗体句柄；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 StartBackupPlayBack(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,
                                    const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                    HWND hWnd,
                                    IVS_ULONG* pHandle);

	/******************************************************************
	function : CheckParam
	description: 检验回放参数是否合法;
	input : pTimeSpan,时间段信息；pCameraCode，摄像机编号；uiProtocolType，协议类型；fSpeed，速率；stTimeSpanInter，时间片结构体信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 CheckParam(const IVS_CHAR* pCameraCode,const IVS_TIME_SPAN* pTimeSpan,IVS_UINT32 uiProtocolType, IVS_FLOAT fSpeed, TIME_SPAN_INTERNAL& stTimeSpanInter);

	/******************************************************************
	function : StopPlatformPlayBack
	description: 停止平台录像回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StopPlatformPlayBack(IVS_ULONG ulHandle);

    IVS_INT32 StopDisasterRecoveryPlayBack(IVS_ULONG ulHandle);
    IVS_INT32 StartDisasterRecoveryPlayBack(const IVS_CHAR* pCameraCode,const IVS_PLAYBACK_PARAM* pPlaybackParam,HWND hWnd,IVS_ULONG* pHandle);

	/******************************************************************
	function : PlayBackFrameStepForward
	description: 平台录像单步前进回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 PlayBackFrameStepForward(IVS_ULONG ulPlayHandle);

	/******************************************************************
	function : PlayBackFrameStepBackWard
	description: 平台录像单步后退回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 PlayBackFrameStepBackward(IVS_ULONG ulPlayHandle);

	/******************************************************************
	function : PlayBackPause
	description: 暂停回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 PlayBackPause(IVS_ULONG ulHandle);

	/******************************************************************
	function : PlayBackResume
	description: 恢复回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 PlayBackResume(IVS_ULONG ulHandle);

	/******************************************************************
	 function   : GetPlayBackStatus
	 description: 获取回放状态;
	 input      : IVS_ULONG ulPlayHandle
	 output     : IVS_BOOL& bPause
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetPlayBackStatus(IVS_ULONG ulPlayHandle, IVS_BOOL& bPause);

	/******************************************************************
	function : SetPlayBackSpeed
	description: 设置播放速度;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 SetPlayBackSpeed(IVS_ULONG ulHandle, float fSpeed);

	/******************************************************************
	 function   : GetPlayBackSpeed
	 description: 获取播放速度
	 input      : IVS_ULONG ulHandle
	 output     : float & fSpeed
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetPlayBackSpeed(IVS_ULONG ulHandle, float& fSpeed);

	/******************************************************************
	function : GetPlayBackTime
	description: 获取回放时间;
	input : ulHandle,句柄；pPlayBackTime，播放时间
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 GetPlayBackTime(IVS_ULONG ulPlayHandle, IVS_PLAYBACK_TIME* pPlayBackTime);
	IVS_INT32 GetCurrentFrameTick(IVS_ULONG ulPlayHandle, IVS_UINT64* pTick);

	/******************************************************************
	function : SetPlayBackTime
	description: 设置录像回放播放时间;
	input : ulHandle,句柄；uiTime，播放时间
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 SetPlayBackTime(IVS_ULONG ulHandle ,IVS_UINT32 uiTime);

	/******************************************************************
	function : GetFreePlayback
	description: 根据句柄获取空闲对象;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	CPlayback* GetFreePlayback(IVS_ULONG& ulHandle);

	/******************************************************************
	function : GetPlayBack
	description: 根据句柄获取对象;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    CPlayback* GetPlayBack(IVS_ULONG ulHandle);

	/******************************************************************
	function : GetPlayHandlebyRtspHandle
	description: 根据Rtsp句柄获取播放句柄;
	input : iRtspHandle，Rtsp句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_ULONG GetPlayHandlebyRtspHandle(int iRtspHandle);

	/******************************************************************
	function : GetPlayHandleByNetSourceChannel
	description: 获取播放句柄;
	input : ulNetSourceChannel，NetSource信道
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_ULONG GetPlayHandleByNetSourceChannel(unsigned long ulNetSourceChannel);

	/******************************************************************
	function : GetPlayHandleByPlayChannel
	description: 根据播放信道号获取播放句柄;
	input : ulPlayChannel，播放信道号
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_ULONG GetPlayHandleByPlayChannel(unsigned long ulPlayChannel);

	/******************************************************************
	function : GetPlayerChannelByHandle
	description: 根据句柄获取播放信道;
	input : ulHandle，句柄
	output : NA
	return : 播放信道号
	*******************************************************************/
	unsigned int GetPlayerChannelByHandle(unsigned long ulHandle);

	/******************************************************************
	function : *GetMediaBase
	description: 根据句柄获取meidiaBase对象指针;
	input : ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	CMediaBase *GetMediaBase(IVS_ULONG ulHandle);
	/******************************************************************
	function : PlaySound
	description: 播放随路语音;
	input : uiPlayHandle，播放句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 PlayAudioSound(unsigned int uiPlayHandle);

	/******************************************************************
	function : StopSound
	description: 停止播放随路语音;
	input : uiPlayHandle，播放句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 StopAudioSound(unsigned int uiPlayHandle);

	/******************************************************************
	function : SetVolume
	description: 设置音量;
	input : uiPlayHandle,播放句柄；puiVolumeValue，音量
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 SetVolume(unsigned int uiPlayHandle, unsigned int uiVolumeValue);

	/******************************************************************
	function : GetVolume
	description: 获取音量;
	input : uiPlayHandle,播放句柄；puiVolumeValue，音量
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 GetVolume(unsigned int uiPlayHandle, unsigned int* puiVolumeValue);

    /******************************************************************
	function : StartPUPlayBack
	description: 开始前端录像回放;
	input : pPlaybackParam,回放参数；pCameraCode，摄像机编号；hWnd，窗体句柄；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 StartPUPlayBack(const IVS_CHAR* pCameraCode,
                              const IVS_PLAYBACK_PARAM* pPlaybackParam,
                              HWND hWnd,
                              IVS_ULONG* pHandle);

	/******************************************************************
	function : StopPUPlayBack
	description: 停止前端录像回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StopPUPlayBack(IVS_ULONG ulHandle);

    IVS_INT32 GetPUPlayBackFileName(const char* pCameraCode,
                                    const IVS_TIME_SPAN* pTimeSpan,
                                    IVS_RECORD_INFO &stRecordInfo);

    /******************************************************************
     function   : NotifyStreamBufferState
     description: 通知流缓冲区状态
     input      : IVS_ULONG ulPlayHandle
     input      : IVS_INT32 iBufferState
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 NotifyStreamBufferState(IVS_ULONG ulPlayHandle, IVS_INT32 iBufferState);

	IVS_VOID  StopNotifyBufEvent(IVS_ULONG ulPlayHandle);

	/******************************************************************
	function : StopBackupPlayBack
	description: 停止备份录像回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StopBackupPlayBack(IVS_ULONG ulHandle);

	/******************************************************************
	function : StartLocalPlayBack
	description: 开始本地录像回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StartLocalPlayBack (
		const IVS_CHAR* pFileName,
		const IVS_LOCAL_PLAYBACK_PARAM* pPlaybackParam,
		HWND hWnd, 
		IVS_ULONG* pHandle);

	/******************************************************************
	function : StartLocalPlayBack
	description: 停止本地录像回放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StopLocalPlayBack(IVS_ULONG ulPlayHandle);

	/******************************************************************
	function : AddPlayWindow
	description: 添加播放窗口;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 AddPlayWindow(IVS_ULONG ulHandle, HWND hWnd);
	/******************************************************************
	function : DelPlayWindow
	description: 删除播放窗口;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 DelPlayWindow(IVS_ULONG ulHandle, HWND hWnd);

	IVS_INT32 AddPlayWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial);
	IVS_INT32 UpdateWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial);

	//停止所有回放
	void StopAllPlayBack();

	/*****************************************************************************
	函数名称：SetVideoEffect
	功能描述：设置视频质量
	输入参数：ulPlayHandle 播放句柄
					：lBrightValue 亮度
					：lContrastValue 对比度
					：lSaturationValue 饱和度
					：lHueValue 色调
	输出参数：NA
	返 回 值：int
	****************************************************************************/
	IVS_INT32 SetVideoEffect(IVS_ULONG ulPlayHandle, IVS_LONG lBrightValue, 
		IVS_LONG lContrastValue, IVS_LONG lSaturationValue, 
		IVS_LONG lHueValue);

private:
	/******************************************************************
	function : GetHandleByHwnd
	description: 根据窗体句柄获得句柄号
	input : hWnd,窗体句柄
	output : NA
	return : 句柄号
	*******************************************************************/
    IVS_ULONG GetHandleByHwnd(HWND hWnd);

private:
    CUserMgr *m_pUserMgr;

	typedef std::map<unsigned long, CPlayback*> HandlePlayBackMap;
	typedef HandlePlayBackMap::iterator HandlePlayBackMapIter;
	HandlePlayBackMap m_HanlePlayBackMap;

	VOS_Mutex* m_pHandlePlayBackMutex;	
};

#endif
