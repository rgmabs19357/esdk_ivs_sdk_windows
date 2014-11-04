/********************************************************************
 filename    :    TVWallMgr.h
 author      :    g00209332
 created     :    2012/12/19
 description :    电视墙管理类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 初始版本
*********************************************************************/
#pragma once
#include "hwsdk.h"
#include "SDKDef.h"
#include "ivs_xml.h"
#include "DecoderChannel.h"
#include "vos.h"
#include "SDKTimer.h"
#include "SDKDecoderMgr.h"
#include "AutoResumeTrigger.h"

//摄像头ID
#define TVWALL_CAMERA_CODE_LEN  128

//回调信息
typedef struct 
{
	PCUMW_CALLBACK_NOTIFY pCallBackFunc;		//回调函数
	void* pUserData;							//回调数据
}TVWALL_CALLBACK_INFO;

//自动重连信息
typedef struct 
{
    CTVWallMgr* pTVWallMgr;
    CDecoderChannel* pCh;
    CUMW_BREAKDOWN_REASON emReason;
}TVWALL_ADD_AUTO_RESUME_INFO;

//自动重连的相关信息
typedef struct 
{
    CTVWallMgr* pTVWallMgr;
    AUTO_RESUME_PARAM stAutoResumePara;
}AUTO_RESUME_TREAD_PARAM;

class CUserMgr;
class CTVWallMgr
{
	typedef std::map<std::string, CUMW_NOTIFY_REMOTE_DECODER_STATUS*> REMOTE_DECODER_MAP;	   //解码器信息链表
	typedef std::map<IVS_ULONG, CUMW_NOTIFY_INIT_DECODER_INFO*> REMOTE_DECODER_INIT_INFO_MAP;//解码器初始化信息MAP
	typedef std::map<std::string, CDecoderChannel*> TVWALL_DEC_CH_MAP;	//解码器MAP
    typedef std::map<std::string, AUTO_RESUME_PARAM*> AUTO_RESUME_MAP;//自动恢复参数MAP
	typedef std::map<IVS_UINT64, TVWALL_CALLBACK_INFO> TVWALL_CALLBACK_MAP;
    typedef std::map<time_t, CDecoderChannel*> TVWALL_IDLE_CH_MAP;
public:
    // 初始化电视上墙模块
    IVS_INT32 InitTVWall(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

    // 释放电视上墙模块
    IVS_INT32 CleanupTVWall();
	// 获取解码器列表
	IVS_INT32 GetDecoderList(const IVS_INDEX_RANGE* pIndexRange,
								CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList,
								IVS_UINT32 uiBufferSize);
	// 启动电视墙实时浏览
	IVS_INT32 StartRealPlayTVWall(const IVS_CHAR* pCameraCode,
									const IVS_REALPLAY_PARAM* pRealplayParam,
									const IVS_TVWALL_PARAM* pTVWallParam,
									IVS_BOOL bAutoResume=FALSE);	
	// 启动电视墙实时浏览
	IVS_INT32 StopRealPlayTVWall(const IVS_TVWALL_PARAM* pTVWallParam);

	// 启动电视墙录像回放
    IVS_INT32 StartPlayBackTVWall(const IVS_CHAR* pCameraCode,
                                            const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                            const IVS_TVWALL_PARAM* pTVWallParam,
											REPLAY_TYPE iReplayType,
											const IVS_CHAR* pDomainCode=NULL,
											const IVS_CHAR* pNVRCode=NULL);

	// 停止电视墙录像回放
	IVS_INT32 StopPlayBackTVWall(const IVS_TVWALL_PARAM* pTVWallParam);
	// 暂定电视墙录像回放
	IVS_INT32 PlayBackPauseTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// 恢复电视墙录像回放
	IVS_INT32 PlayBackResumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam);

	// 单帧快进
	IVS_INT32 PlaybackFrameStepForwardTVWall(const IVS_TVWALL_PARAM& stTVWallParam);

	// 播放声音
	IVS_INT32 PlaySoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// 停止声音
	IVS_INT32 StopSoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam);
	// 设置音量
	IVS_INT32 SetVolumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam, IVS_UINT32 uiVolumeValue);
	// 增加一个回调信息
	IVS_INT32 AddCallBack(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
	// 移除一个回调信息
	IVS_INT32 RemoveCallBack(void* pUserData);

    //删除空闲通道对象
    void DelIdleCh();

public:
	CTVWallMgr(void);
	~CTVWallMgr(void);

	void SetUserMgr(CUserMgr* pUserMgr);
	//从SCU获取媒体URL
	IVS_INT32 GetURL(const char *pszAuthHeader, ST_URL_MEDIA_INFO& stUrlMediaInfo, ST_MEDIA_RSP& stMediaRsp);
	//获取解码器通道初始化信息
	IVS_INT32 GetDecoderInitInfo(IVS_ULONG ulWndID, CUMW_NOTIFY_INIT_DECODER_INFO* pInitDecoderInfo);
	//增加解码器通道初始化信息
	void AddDecoderInitInfo(IVS_ULONG ulWndID, const CUMW_NOTIFY_INIT_DECODER_INFO& initDecoderInfo);
	//删除指定窗格的解码器通道初始化信息
	void DeleteDecoderInitInfo(IVS_ULONG ulWndID);
    //自动恢复处理,由触发器触发调用
    void HandleAutoResume();
    //异步操作返回通知
    void NotifyAsyncReturn(const CUMW_NOTIFY_ASYNC_RET_EX& stAsyncRet);
	//事件通知上层
	void NotifyEvent(CUMW_NOTIFY_INFO* pstNotifyInfo);
private:
	//获取指定解码器通道对象，没有就创建一个
    CDecoderChannel* GetDecoderChannel(const char* pDecoderID, IVS_UINT32 uiChannel);
    //获取指定解码器通道对象，只是获取
    CDecoderChannel* GetDecoderChannel(const std::string& key);
    //获取指定解码器通道对象，只是获取
	CDecoderChannel* GetDecoderChannel(IVS_ULONG ulAsyncSeq);
	//生成MAP的Key
	std::string MakeKey(const char* pDecoderID, IVS_UINT32 uiChannel) const;
	//增加一个解码器信息
	void AddRemoteDecoder(const CUMW_NOTIFY_REMOTE_DECODER_STATUS& remoteDecoder);
	//移除一个解码器
	void RemoveRemoteDecoder(const IVS_CHAR* remoteDecoderID);
    //查找前端录像文件名称 
    IVS_INT32 GetPUPlayBackFileName(const char* pCameraCode, const IVS_TIME_SPAN* pTimeSpan, IVS_RECORD_INFO &stRecordInfo);
    //自动恢复
    IVS_INT32 AutoResumeEx(const AUTO_RESUME_PARAM* pResumeParam);
public:
	//增加自动恢复
	void AddAutoResume(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON iReason);
private:
	//检查是否有自动重连数据.有，关闭RTSP，返回true;无,返回false.
	bool RemoveAutoResume(const char* szDecoderCode, unsigned int uiChannel);
	//电视墙回调函数
	static IVS_INT32 __stdcall HandleNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData);
	//处理初始化解码器通道的返回数据
	void InitDecoderNotify(const CUMW_NOTIFY_INFO* pstNotifyInfo, const void* pUserData);
    //查询指定的解码器是否在线
    bool IsOnline(const char* pDecoderID);
	//查询指定的解码器是否在线，是否有指定的通道
	IVS_INT32 CheckDecoder(const char* pDecoderID, IVS_UINT32 uiChannel);
	//解码器收流异常处理
	IVS_INT32 DecoderStreamError(const char* pDecoderID, IVS_UINT32 uiChannel);
	//上报自动恢复事件 bStartResume 是否是启动恢复 TRUE-启动恢复 FALSE-恢复完成，结果看iErrCode
	void ReportAutoResumeEvnet(const AUTO_RESUME_PARAM* pResumeParam, IVS_INT32 iErrCode = IVS_SUCCEED, IVS_BOOL bStartResume=TRUE);
    //取得当前系统时间，以秒为单位
    time_t GetCurrentTime() const;
    //把通道对象指针放到空闲链表中
    void AddToIdleList(CDecoderChannel* pDecoderChannel);

    //检查重连链表里是否还存在指定的键值的重连参数
    bool IsAutoResumeParaExist(const std::string& key);
    //删除重连链表里的数据
    void DelAutoResumePara(const std::string& key);

    //解码器流异常，增加自动重连
    void DecoderStreamError(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON emReason);
    //增加自动重连的线程函数
    static void __cdecl AddAutoResumeThread(LPVOID lpvoid);

    //开始硬解实况线程函数
    static void __cdecl AutoResumeThreadFun(LPVOID lpvoid);

private:
	//////////////////////////////////////////////////////////////////////////
	//CU使用多个OCX，导致initTVWall只能注册一个ocx获取事件
	//为了使电视墙相关ocx都能获取到事件，使用Map存放订阅事件的OCX信息
	//PCUMW_CALLBACK_NOTIFY m_pFunCallBack;       //回调函数指针
	//void* m_pUserData;                          //上层调用传进的参数，一般是this指针
	TVWALL_CALLBACK_MAP m_callbackMap;			//上层回调信息MAP
	VOS_Mutex* m_pCallBackMapMutex;					   //解码器初始化Map锁
	//////////////////////////////////////////////////////////////////////////

	CUserMgr* m_pUserMgr;

	REMOTE_DECODER_INIT_INFO_MAP m_remoteDecodeInitInfoMap;//远程解码器初始化信息表
	VOS_Mutex* m_pInitDecoderMapMutex;					   //解码器初始化Map锁

	TVWALL_DEC_CH_MAP m_tvwallDecChMap;			//电视墙解码器通道MAP
	VOS_Mutex* m_pDecChMapMutex;					//通道MAP锁;

	REMOTE_DECODER_MAP m_onlineDecoderMap;	    //在线解码器信息
    VOS_Mutex* m_pOnlineDecoderMapMutex;		//在线解码器信息MAP锁;

    AUTO_RESUME_MAP m_autoResumeMap;            //自动恢复参数MAP，需要自动恢复的播放参数都保存在此MAP中
    VOS_Mutex* m_pAutoResumeMapMutex;           //自动恢复MAP锁
    CNVSTimer m_timer;                          //自动恢复定时器
    CAutoResumeTrigger m_AutoResumeTigger;    //自动恢复触发器
    bool m_bTimerInit;                          //定时器是否已经初始化

	IVS_ULONG m_ulAsyncSeq;						//与电视墙模块通信异步业务序号

	//CSDKDecoderMgr m_SDKDecoderMgr;				//SDK解码器管理对象

	//解码器上墙成功的通道列表
	typedef std::map<IVS_ULONG,CDecoderChannel*> CHANNEL_MAP;
	typedef CHANNEL_MAP::iterator CHANNEL_MAP_ITER;
	CHANNEL_MAP m_mapPlayChannel;//实况列表

    TVWALL_IDLE_CH_MAP m_IdleChMap;            //空闲解码器通道对象链表
    VOS_Mutex* m_pIdleChMapMutex;              //空闲解码器通道对象链表的锁

public:
	CDecoderChannel* GetChannel(IVS_ULONG ulHandle);
	IVS_ULONG GetTVWallHandlebyRtspHandle(IVS_ULONG ulHandle);
	void DeleteChannel(IVS_ULONG ulHandle);//删除某通道

    VOS_Mutex* m_pHandleMutex;		          //句柄锁;
};

