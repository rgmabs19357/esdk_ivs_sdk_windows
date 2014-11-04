/********************************************************************
	filename	: 	Playback.h
	author		:	z00201790
	created		:	2012/12/06	
	description	:	定义回放业务类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本;
*********************************************************************/
#ifndef __PLAYBACK_H__
#define __PLAYBACK_H__

#include "MediaBase.h"
#include "vos.h"

class CPlayback: public CMediaBase
{
public:
	CPlayback(void);
	~CPlayback(void);

public:
	void SetHandle(unsigned long ulHandle);

	unsigned long GetHandle();

	/******************************************************************
	function : StartPlatformPlayBack
	description: 开始平台录像回放
	input : uiProtocolType,协议类型；pCameraCode，摄像机编号；hWnd，窗体句柄；stTimeSpanInter，时间段信息 ;playBackServiceType 业务类型
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int StartPlatformOrBackupPlayBack(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode,
                              unsigned int uiProtocolType,
                              const TIME_SPAN_INTERNAL& stTimeSpanInter,
                              HWND hWnd,
                              float fSpeed,IVS_SERVICE_TYPE playBackServiceType = SERVICE_TYPE_PLAYBACK);

	int StartPlatformPlayBackCB(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode,
								unsigned int uiProtocolType,
								const TIME_SPAN_INTERNAL& stTimeSpanInter,
								PlayBackCallBackRaw fPlayBackCallBackRaw,
								void* pUserData, 
								float fSpeed);

	int StartPlatformPlayBackCBFrame(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode,
									unsigned int uiProtocolType,
									const TIME_SPAN_INTERNAL& stTimeSpanInter,
									PlayBackCallBackFrame fPlayBackCallBackFrame,
									void* pUserData, 
									float fSpeed);

	

	/******************************************************************
	function : StopPlatformPlayBack
	description: 停止平台录像回放
	input : 
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int StopPlatformPlayBack();

	/******************************************************************
	function : PlayBackPause
	description: 暂停录像回放
	input : 
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int PlayBackPause();

	/******************************************************************
	function : PlayBackResume
	description: 恢复录像回放
	input : 
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int PlayBackResume();
	

	/******************************************************************
	function : StepForward
	description: 单步前进播放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StepForward();

	/******************************************************************
	function : StepForward
	description: 单步后退播放;
	input : ulHandle,句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StepBackward();

	/******************************************************************
	function   : SetPlayBackSpeed
	description: 设置回复速度
	input : 
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int SetPlayBackSpeed(float fSpeed);

	/******************************************************************
	function : GetPlayBackTime
	description: 获取录像回放时间
	input : stPlayBackTime，录像回放时间信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int GetPlayBackTime(IVS_PLAYBACK_TIME& stPlayBackTime);
	int GetCurrentFrameTick(unsigned long long &tick);

	/******************************************************************
	function : SetPlayBackTime
	description: 设置录像回放时间
	input : uiTime，时间秒数
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int SetPlayBackTime(unsigned int uiTime);

	/******************************************************************
	 function   : GetPlayBackSpeed
	 description: 获取播放速度;
	 output     : NA
	 return     : int 播放速度
	*******************************************************************/
	float GetPlayBackSpeed()const { return m_fSpeed;};

	/******************************************************************
	function : IsExistHWnd
	description: 检查窗体句柄是否存在
	input : hWnd，窗体句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    bool IsExistHWnd(const HWND& hWnd) const;

    /******************************************************************
     function   : NotifyStreamBufferState
     description: 通知流缓冲区状态
     input      : IVS_INT32 iBufferState
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 NotifyStreamBufferState(IVS_INT32 iBufferState);

	IVS_VOID  StopNotifyBufEvent();

    // 开始前端录像回放
    int StartPUPlayBack(const char* pCameraCode,
                        unsigned int uiProtocolType,
                        const TIME_SPAN_INTERNAL& stTimeSpanInter,
                        HWND hWnd,
                        float fSpeed,
                        const char* pFileName);
    
    // 停止前端录像回放
    int StopPUPlayBack();

	/******************************************************************
	function : StopBackupPlayBack
	description: 停止备份录像回放
	input : 
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int StopBackupPlayBack();

    inline IVS_SERVICE_TYPE GetServiceType() const
    {
        return m_enServiceType;
    }

    inline void SetServiceType(IVS_SERVICE_TYPE enServiceType)
    {
        m_enServiceType = enServiceType;
    }

	bool IsPaused() const { return m_bPaused; }

    bool IsControlPaused() const { return m_bControlPause; }
	/******************************************************************
	function :   StartLocalPlayBack
	description: 启动本地录像回放;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StartLocalPlayBack (
			const IVS_CHAR* pFileName,
			const IVS_LOCAL_PLAYBACK_PARAM* pPlaybackParam,
			HWND hWnd);

	/******************************************************************
	function :   StopLocalPlayBack
	description: ;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StopLocalPlayBack();

	/******************************************************************
	function : AddPlayWindow
	description: 添加播放窗口;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int AddPlayWindow(HWND hWnd);
	/******************************************************************
	function : DelPlayWindow
	description: 删除播放窗口;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int DelPlayWindow(HWND hWnd);

	/*****************************************************************************
	函数名称：AddPlayWindowPartial
	功能描述：增加局部图像播放窗口
	输入参数：hWnd：       窗口句柄
			 pPartial：   局部图片参数
	输出参数：NA
	返 回 值：int
	****************************************************************************/
	int AddPlayWindowPartial(HWND hWnd, void* pPartial);

	/*****************************************************************************
	函数名称：UpdateWindowPartial
	功能描述：更新局部放大源矩形
	输入参数：hWnd：       窗口句柄
			 pPartial：   局部参数
	输出参数：NA
	返 回 值：int
	****************************************************************************/
	int UpdateWindowPartial(HWND hWnd, void* pPartial);

	void DoRawFrameCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo, IVS_CHAR* pFrame, IVS_UINT32 uFrameSize);

	void DoFrameCallBack(IVS_INT32 iStreamType, IVS_VOID* pFrameInfo);

	virtual void RetSet();

	/*****************************************************************************
	函数名称：SetVideoEffect
	功能描述：设置视频质量
	输入参数：lBrightValue 亮度
					：lContrastValue 对比度
					：lSaturationValue 饱和度
					：lHueValue 色调
	输出参数：NA
	返 回 值：int
	****************************************************************************/
	IVS_INT32 SetVideoEffect(IVS_LONG lBrightValue, 
		IVS_LONG lContrastValue, IVS_LONG lSaturationValue, 
		IVS_LONG lHueValue);

private:
	/******************************************************************
	function : StartPlay
	description: 开始回放
	input : pstMediaPara，媒体信息；pCameraCode，摄像机编号；stTimeSpanInter，时间片信息；stParam，播放参数
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int StartPlay(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
		          const IVS_MEDIA_PARA* pstMediaPara, 
                  const IVS_CHAR* pCameraCode, 
                  const TIME_SPAN_INTERNAL& stTimeSpanInter,
                  START_PLAY_PARAM& stParam,
                  const char* pFileName);

	/******************************************************************
	function : DoGetUrlPlayBack
	description: 获取回放Url
	input : stTimeSpanInter，时间段信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int DoGetUrlPlayBack(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,IVS_SERVICE_TYPE ServiceType,
                         const TIME_SPAN_INTERNAL& stTimeSpanInter,
                         const char *pFileName);
private:
    HWND m_hWnd;    // 显示窗口;
    //RealPlayCallBackRaw m_fRealPlayCallBackRaw;         // 裸码流的回调函数
    //void* m_pRealPlayUserDataRaw;
    //VOS_Mutex*          m_pRealPlayRawCBMutex;              // 裸码流回调锁;
    TIME_SPAN_INTERNAL m_stTimeSpanInter;                // 播放流的开始结束时间;
    
    unsigned int       m_uiDragPlayTime;                 // 拖动后的回放时间;

	unsigned int       m_FirstRtpStamp;

	bool               m_bPaused;
	bool               m_bLocalFile;

	PlayBackCallBackRaw		m_fPlayBackCallBackRaw;         // 裸码流的回调函数
	void*					m_pPlayBackUserDataRaw;
	PlayBackCallBackFrame	m_fPlayBackCallBackFrame;     // 解码后帧数据的回调函数
	void*					m_pPlayBackUserDataFrame;
	VOS_Mutex*				m_pPlayBackRawCBMutex;              // 裸码流回调锁;
	VOS_Mutex*				m_pPlayBackFrameDecoderCBMutex;   // 解码流回调锁;
	IVS_LONG m_lBrightValue;				//亮度
	IVS_LONG m_lContrastValue;			//对比度
	IVS_LONG m_lSaturationValue;		//饱和度
	IVS_LONG m_lHueValue;					//色调

	//unsigned long       m_ulLastStepBackwardTime;        // 上次单步回退的时间;
};

#endif //__PLAYBACK_H__
