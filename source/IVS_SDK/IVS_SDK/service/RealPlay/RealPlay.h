/********************************************************************
 filename    :    RealPlay.h
 author      :    s00191067
 created     :    2012/11/19
 description :    实现所有实况业务;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*********************************************************************/
#ifndef __REALPLAY_H__
#define __REALPLAY_H__
// #include <list>
#include "MediaBase.h"
#include "vos.h"
#include "SDKDef.h"
#include "vos_atomic.h"

#include <Windows.h>

typedef std::list<CAMERA_PLAY_INFO*>             CAMERA_PLAY_INFO_LIST;
typedef std::list<CAMERA_PLAY_INFO*>::iterator   CAMERA_PLAY_INFO_LIST_ITER;

typedef enum {
	STATUS_READY   = 0x01,
	STATUS_PLAYING = 0x02,
	STATUS_STOPPED = 0x03,
	STATUS_FINISHED = 0x04
}PLAY_STATUS;

class CRealPlay : public CMediaBase
{
public:
	CRealPlay(void);
	virtual ~CRealPlay(void);
    int StartPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, const START_PLAY_PARAM& stParam);//lint !e1411
	int ReStartStream(const IVS_MEDIA_PARA* pMediaPara, const IVS_CHAR* pCameraCode, const START_PLAY_PARAM& stParam);//lint !e1411

	virtual int StopStream();
public:

    /******************************************************************
	function : StartRealPlay
	description: 开始实况;
	input : pRealplayPara,实况参数信息；pCameraCode，摄像机编号；hWnd，窗体句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StartRealPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode,HWND hWnd);

    /******************************************************************
	function : ReStartRealPlay
	description: 开始实况(码流复用);
	input : pMediaPara,媒体参数信息；pCameraCode，摄像机编号
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int ReStartRealPlay(const IVS_MEDIA_PARA* pMediaPara, const IVS_CHAR* pCameraCode);

	/******************************************************************
	function : StartRealPlayCBRaw
	description: 开始实况(裸码流);
	input : pRealplayPara,实况参数信息；pCameraCode，摄像机编号；fRealPlayCallBackRaw，裸码流回调；pUserData，用户信息；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StartRealPlayCBRaw(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackRaw fRealPlayCallBackRaw, void* pUserData );

	/******************************************************************
	function : StartRealPlayCBFrame
	description: 开始实况(YUV流);
	input : pRealplayPara,实况参数信息；pCameraCode，摄像机编号；fRealPlayCallBackFrame，YUV码流回调；pUserData，用户信息；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StartRealPlayCBFrame(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData );

	/******************************************************************
	function : StopRealPlay
	description: 停止实况;
	input : ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	int StopRealPlay();

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

	/******************************************************************
	function : IsExistHWnd
	description: 判断窗口句柄是否存在;
	input : hWnd，窗体句柄
	output : NA
	return : 存在-True；不存在-False
	*******************************************************************/
	bool IsExistHWnd(const HWND& hwnd);

	/******************************************************************
	function : DelHWndfromList
	description: 删除WndList中的Wnd;;
	input : hWnd，窗体句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int DelHWndfromList(const HWND& hwnd);

	/******************************************************************
	function : GetCameraCode
	description: 获取CameraCode
	input : 无
	output : NA
	return : 摄像机编号
	*******************************************************************/
	char* GetCameraCode();

	void SetHandle(unsigned long ulHandle);

	unsigned long GetHandle();

    void DoRawFrameCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo, IVS_CHAR* pFrame, IVS_UINT32 uFrameSize);

    void DoFrameCallBack(IVS_INT32 iStreamType, IVS_VOID* pFrameInfo);

    /******************************************************************
     function   : ReuseStream
     description: 复用该流
     input      : HWND hWnd 新的窗口句柄;
     output     : NA
     return     : int
    *******************************************************************/
    int ReuseStream(HWND hWnd);

    virtual void RetSet();

    bool IsExistStream(const char* pszCameraID, IVS_STREAM_TYPE eStreamType) const;

	/******************************************************************
	function : PlaySound
	description: 播放随路语音;
	input : uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int PlaySound();

	/******************************************************************
	function : StopSound
	description: 停止播放随路语音;
	input : uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int StopSound();

	/******************************************************************
	function : SetVolume
	description: 设置音量;
	input : uiVolumeValue，音量值；uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int SetVolume(unsigned int uiVolumeValue);

	/******************************************************************
	function : GetVolume
	description: 获取音量;
	input : puiVolumeValue，音量值；uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int GetVolume(unsigned int* puiVolumeValue);
	
	/******************************************************************
	function : GetPlayWindows
	description: 获取播放窗口;
	input : pBuf，窗体句柄；size，窗口总数量
	output : NA
	return : 在播放实况的窗口数目
	*******************************************************************/
	int GetPlayWindows(HWND *pBuf, int size);

	/******************************************************************
	function : GetCameraPlayInfo
	description: 返回播放的参数;
	input : cameraPlayInfo，当前在播放的窗口的信息
	output : NA
	return : 无
	*******************************************************************/
	void GetCameraPlayInfo(CAMERA_PLAY_INFO& cameraPlayInfo)const;

	long GetRef();
	long PutRef();

private:
    /******************************************************************
	 function   : GetUrlRealPlay
	 description: 从SCU获取媒体url;
	 input      : int & iProtocolType
	 input      : ST_MEDIA_RSP * mediaRsp
	 output     : NA
	 return     : int
	*******************************************************************/
	int DoGetUrlRealPlay();

	IVS_INT32 GetLocalIpPort();

private:
	typedef std::list<HWND> WndList;            
	typedef WndList::iterator WndListIter;

    WndList                 m_WndList;          // 渲染窗口列表;
	CAMERA_PLAY_INFO        m_CameraPlayInfo;   // 播放信息
    RealPlayCallBackRaw m_fRealPlayCallBackRaw;         // 裸码流的回调函数
    void* m_pRealPlayUserDataRaw;
    RealPlayCallBackFrame m_fRealPlayCallBackFrame;     // 解码后帧数据的回调函数
    void* m_pRealPlayUserDataFrame;
    VOS_Mutex*          m_pRealPlayRawCBMutex;              // 裸码流回调锁;
    VOS_Mutex*          m_pRealPlayFrameDecoderCBMutex;   // 解码流回调锁;      
#ifdef WIN32
	vos_atomic_t        m_RefCounter;
#else
	atomic_t			m_RefCounter;
#endif

	PLAY_STATUS         m_nStatus;
};

#endif

