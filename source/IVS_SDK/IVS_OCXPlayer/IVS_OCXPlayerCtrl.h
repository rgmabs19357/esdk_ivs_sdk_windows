/*****************************************************************
 filename    :    IVS_OCXPlayerCtrl.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    OCX控件
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/

#pragma once
#include "LocalConfig.h"
#include "VideoPaneMgr.h"

#ifndef DYNCREATE_VIDEOPANE
#define DYNCREATE_VIDEOPANE
class CVideoPane;  //lint !e763 !e830
#endif

class CGlobalPlayMgr;//lint !e830
class CTranVideoRealtimeBar;//lint !e830
class CSyncPlayBackMgr;//lint !e830

#define  OCX_SKIN_TYPE_BLACK 1
#define  OCX_SKIN_TYPE_WHITE 2
// IVS_OCXPlayerCtrl.h : Declaration of the CIVS_OCXPlayerCtrl ActiveX Control class.

// CIVS_OCXPlayerCtrl : See IVS_OCXPlayerCtrl.cpp for implementation.
#include <objsafe.h>


//开始上墙信息
typedef struct 
{
    ULONG ulWndID;
    ULONG ulTransID;
    ULONG ulTVWallID;
    BOOL bDel;
    BOOL bStopInterfaceSend;

    CIVS_OCXPlayerCtrl* pOcx;

    CString strCameraCode;
    CString strRealplayParam;
}START_TVWALL_INFO;

//上墙的线程信息
typedef struct 
{
    ULONG ulWndID;
    ULONG ulTVWallID;
    CIVS_OCXPlayerCtrl* pOcx;
}TVWALL_THREAD_INFO;

//保存电视墙的参数
typedef struct stTvwallChannelStateInfo
{
    bool bStartThreadRun;  //开启电视上墙线程是否在运行
    bool bStopInterfaceRun;//关闭电视上墙接口是否在运行
	bool bStopThreadWaitRun;//关闭电视上墙接口正在等待运行
    bool bWndPlaying;      //本窗格的电视墙是否正在播放
    bool bCleanupTVWall;   //关闭电视墙的所有业务，并清除内存
    std::list<START_TVWALL_INFO*> StartTvwallInfo;   

    CIVSMutex  TvWallWndLock;
    CIVSMutex  StartTvWallTheadLock; //开启电视墙的线程的锁
    CIVSMutex  StartTvwallInfoLock;  //上墙信息链表的锁

    stTvwallChannelStateInfo()
        :bStartThreadRun(false)
        ,bStopInterfaceRun(false)
		,bStopThreadWaitRun(false)
        ,bWndPlaying(false)
        ,bCleanupTVWall(false)
    {}

}TVWALL_CHANNEL_STATE_INFO;

#define TVWALL_WND_ID_MAX 1000
#define TVWALL_WND_ID_MIN 1



class CIVS_OCXPlayerCtrl : public COleControl
{
	DECLARE_DYNCREATE(CIVS_OCXPlayerCtrl)

	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(ObjSafe, IObjectSafety)   
	STDMETHOD_(HRESULT, GetInterfaceSafetyOptions) (   
		REFIID riid,   
		DWORD __RPC_FAR *pdwSupportedOptions,   
		DWORD __RPC_FAR *pdwEnabledOptions   
		);   

	STDMETHOD_(HRESULT, SetInterfaceSafetyOptions) (   
			REFIID riid,   
			DWORD dwOptionSetMask,   
			DWORD dwEnabledOptions   
			);   
	END_INTERFACE_PART(ObjSafe)

	friend CSyncPlayBackMgr;
	friend CGlobalPlayMgr;
	friend CTranVideoRealtimeBar;
	friend CTranAnalysisToolBar;
	friend CTranZoomBar;
	friend CDitalPtzDlg;
    friend CVideoPane;
	friend CVideoPaneMgr;
	friend CVideoSoundCtrl;
	friend void CALLBACK FunEventCallBack(IVS_INT32 iEventType, IVS_VOID *pEventBuf, 
		IVS_UINT32 iBufSize, IVS_VOID *pUserData);
	// Constructor
public:
	CIVS_OCXPlayerCtrl(CWnd* pParent = NULL);

	// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

    TVWALL_CHANNEL_STATE_INFO m_TvwallChannelState[TVWALL_WND_ID_MAX];


private:
	// 用于标记是否已经初始化，要保证一次Init会且只会对应一次cleanUp
	bool m_bIsInit;
	//视屏窗体管理类的指针
	CVideoPaneMgr* m_pVideoPaneMgr;

	ENUMDEF::COLOR_MODE m_ulColor;

	HWND_WID_MAP m_hwnd2wid;

	std::string m_szCameraCode;	//保存回调过来的CameraCode;
    std::string m_szLogPath;    // 日志根路径
	// session id
	static IVS_INT32 m_iSDKSessionID;

	typedef struct
	{
		CString strCamera;
		CString strPara;
	}OnWallPara;
	typedef std::map<IVS_ULONG,OnWallPara*> TVWall_OnWall_Map;
	TVWall_OnWall_Map m_mapOnWall;//已上墙参数

	static IVS_INT32 m_iOldCallBackKeyValue;           //上一次网络键盘发的云台方向命令
	static char m_cOldSpeed;                           //上一次云台方向速度

public:
    IVS_INT32 GetSDKSessionID() const { return m_iSDKSessionID; }
private:

    //句柄列表锁
	CIVSMutex m_csHandleListLock;            
	//控件与通道号的map表的锁
	CIVSMutex m_csExceptionHandleLock;

	// 摄像机别名本地参数文件锁
	VOS_Mutex* m_pMutexCameraAlias;      
	// GDI+
	GdiplusStartupInput gdiplusStartupInput;//lint !e601
	ULONG_PTR gdiplusToken;

	//读写临时场景的锁
	static CIVSMutex m_csTVWallTempScenarioLock; 
public:
	CLocalConfig m_pLocalConfig;

    static CIVS_OCXPlayerCtrl* m_pTvwallOcx;

	//获取窗口管理类
	CVideoPaneMgr* GetVideoPaneMgr(){ return m_pVideoPaneMgr; }
	// 启动实况时改变窗口布局
	void ChangeLayoutRealPlay(ULONG ulWndID);

	// 内部函数
	CVideoPane *GetVideoPanel(ULONG ulWndID);

	// 上报停止播放事件给客户端
	void ReportStopPlayEvent(ULONG ulWndID);
private:
	//重置所有电视墙窗格
	void ResetAllTVwallPaneStatus();
	//刷新所有在线的电视墙通道
	void RefreshOnlineTVWallStatus();
public:
    void PostTalkbackEvent();
    void PostSoundEvent();
	//  
	//void UpdateSynWndID(ULONG ulActivePaneID, ULONG ulMouseUpPaneID);

	void PostOcxEventMsg( const char * pMsg, ULONG ulMsgSeq, ULONG ulResultCode, const std::string &strCameraCode)const;

	void PostOcxEventMsg( const char * pMsg, ULONG ulMsgSeq, ULONG ulResultCode, ULONG ulWndID)const;

	void PostOcxEventMsgStr( const char * pMsg, const char *pMsgSeq, ULONG ulResultCode, ULONG ulWndID)const;

	void PostGetPlayInfoEventMsg(ULONG ulWinID);
	// 双击窗格放大或缩小
	void PostMagnifyOrMinifyEventMsg(ULONG ulMsgSeq, ULONG ulWinID, ULONG ulIsMagnify);

	void PostActiveWndChangeEventMsg(ULONG ulMsgSeq, ULONG ulWinID);
	// 窗口交换事件上报
	void PostExchangeWndEventMsg(ULONG ulMsgSeq, ULONG ulSrcWnd, ULONG ulDstWnd);

	void PostOcxAddRecordLabel(ULONG ulMsgSeq, char * pCameraCode, char * pTime)const;

	void PostOcxStopRecordReport(ULONG ulWndID,ULONG ulRecordRet)const;

	void PostLeftMouseClickEventMsg(ULONG ulMsgSeq, ULONG ulWinID, ULONG ulX, ULONG ulY);
    
    void PostEnterFullScreenEventMsg(LPCTSTR pMsg, ULONG ulResultCode, ULONG ulWndID);

    void PostExitFullScreenEventMsg(LPCTSTR pMsg, ULONG ulResultCode, ULONG ulWndID);

	void PostRightMouseClickEventMsg(ULONG ulMsgSeq, ULONG ulWinID, ULONG ulX, ULONG ulY);

	void PostEventMsg(char* pMsg, HWND hWnd, ULONG ulMsgType, ULONG ulExceptionCode)const;

	void PostToolBarClickEventMsg(ULONG ulBarType,ULONG ulBtnTtype , ULONG ulWndID,std::string strBtnStatus = _T(""));

	void PostStopButtonClickMsg(ULONG ulWndID);

	void PostTurnCruisePreEventMsg(ULONG ulWinID);

	void PostTurnCruiseNextEventMsg(ULONG ulWinID);

	void PostToolBarEMapClickEventMsg(ULONG ulBarType,ULONG ulBtnTtype , ULONG ulWndID,CRect& rect);

	void PostPointMoveOutofOCX(ULONG ulWinID);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnExceptionCallback(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUserOfflineCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAlarmReportCallBack(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnTalkbackFailedCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBroadcastFailedCallBack(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileBroadcastReportCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAlarmStatusReportCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAlarmLinkageCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMAUEventCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommEventCallback(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLocalPlayBackEvent(WPARAM wParam,LPARAM lParam);
    afx_msg LRESULT OnRemotePlayBackEvent(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFindDevBackEvent(WPARAM wParam,LPARAM lParam);
    afx_msg LRESULT OnDevAlarmNotifyCallBack(WPARAM wParam,LPARAM lParam);
    afx_msg LRESULT OnManualRecordStateCallBack(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnKeepAliveFailedCallBack(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnReLoginFailedCallBack(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnReLoginSuccessCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnModeCruiseRecordOverCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetFullScreenMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnResetSetWndLayoutMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTVWallCallback(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnStopTVWallRealPlayEnd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnStartTVWallRealPlayEnd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnNotifyTVWallStartRes(WPARAM wParam, LPARAM lParam);

	static int g_nNumberOfOcx;

	static std::list<CIVS_OCXPlayerCtrl *> m_OcxList;
	static CIVSMutex m_csOcxListLock;

	static std::map <HWND,CIVS_OCXPlayerCtrl *> m_hWnd2OcxMap;            //控件的map表

	static CIVSMutex m_csHWnd2OcxLock;     //控件与窗口句柄的map表的锁

	static std::map <ULONG ,PORT_SESSIONID_INFO>  m_DownloadHandleMap;
	static CIVSMutex m_csDownloadMapLock;

	static std::map <ULONG,CIVS_OCXPlayerCtrl *>   m_VedioDownloadUlport2OCX;      // 录像下载map表(端口->OCX地址)
	static CIVSMutex m_csVedioDownloadUlport2OCXLock;//录像下载map表锁

	static int mCountPorts;
public:
	static BOOL m_bVoiceTalkbacking;
	static CVideoPane *m_pVoiceTalkbackVideoPane;
	static ULONG m_ulVoiceTalkbackVolume;
	static ULONG m_ulVoiceTalkbackHandle;
	static std::string m_strVoiceTalkbackCameraCode;
	static CIVSMutex m_HandleNotifyLock;        //电视墙向CU发消息的锁

	static CIVS_OCXPlayerCtrl * m_pOCX;
	static CIVS_OCXPlayerCtrl * m_pVoiceTalkbackOCX;                           // 语音对讲的ocx
	static CIVS_OCXPlayerCtrl * m_pVoiceBroadcastOCX;							//语音广播的ocx
    static HHOOK g_hook;
    static bool g_IsInitHook;
	static ULONG g_ulSkinType;

	// 显示自动重连文字
	ULONG SetResumeState(HWND hWnd, BOOL bState);

protected:
	ULONG IVS_OCX_SetColor(ULONG ulColor);

	// Dispatch and event IDs
public:
	enum {
		dispidIVS_OCX_SetVideoEffect = 567L,
		dispidIVS_OCX_LocalSnapshotToDir = 566L,
		dispidIVS_OCX_GetMicVolume = 565L,
		dispidIVS_OCX_SetMicVolume = 564L,
		dispidIVS_OCX_GetStreamListByUser = 563L,
		dispidIVS_OCX_ShutdownStreamByUser = 562L,
		dispidIVS_OCX_GetStreamListByCam = 561L,
		dispidIVS_OCX_SetVideoDiagnose = 560L,
		dispidIVS_OCX_GetVideoDiagnose = 559L,
		dispidIVS_OCX_SetReceiveOMUAlarm = 558L,
		dispidIVS_OCX_LoginByTicket = 557L,
		dispidIVS_OCX_SetLogLevel = 556L,
	    dispidIVS_OCX_StopRealPlay_Async = 555L,
		dispidIVS_OCX_StartRealPlay_Async = 554L,
		dispidIVS_OCX_StopRealPlayTVWall_Async = 553L,
		dispidIVS_OCX_StartRealPlayTVWall_Async = 552L,
		dispidIVS_OCX_GetCUVersion = 551L,
		dispidIVS_OCX_GetDeviceEncodeCapabilities = 550L,
        dispidIVS_OCX_RefreshCameraList = 549L,
		dispidIVS_OCX_ExGetDeviceInfo = 548L,
		dispidIVS_OCX_ExGetDeviceStatus = 547L,
		dispidIVS_OCX_GetBroadcastFileList = 546L,
		dispidIVS_OCX_AddBroadcastFile = 545L,
		dispidIVS_OCX_GetBroadcastCameraList = 544L,
		dispidIVS_OCX_DeleteBroadcastFile = 543L,
		dispidIVS_OCX_ModCameraSwitch = 542L,
		dispidIVS_OCX_SetTitleBarStatus = 541L,
		dispidIVS_OCX_KeyEventNotice = 540L,
		dispidIVS_OCX_GetEmapTempScenario = 539L,
		dispidIVS_OCX_GetRecordTempScenario = 538L,
		dispidIVS_OCX_GetRealTimeTempScenario = 537L,
		dispidIVS_OCX_SetEmapTempScenario = 536L,
		dispidIVS_OCX_SetRecordTempScenario = 535L,
		dispidIVS_OCX_SetRealTimeTempScenario = 534L,
		dispidIVS_OCX_EnableExchangePane = 533L,
		dispidIVS_OCX_GetGraphicSelected = 532L,
		dispidIVS_OCX_GetScenarioRecovery = 531L,
		dispidIVS_OCX_SetScenarioRecovery = 530L,
		dispidIVS_OCX_GetAbilityInfo = 529L,
		dispidIVS_OCX_StopBackuplayBackTVWall = 528L,
		dispidIVS_OCX_StartBackupPlayBackTVWall = 527L,
		dispidIVS_OCX_GetExCatalogSubscribeList = 526L,
		dispidIVS_OCX_ExCatalogSubscribe = 525L,
		dispidIVS_OCX_GetExEventSubscribe = 524L,
		dispidIVS_OCX_ExEventSubscribe = 523L,
		dispidIVS_OCX_ResetAlarm = 522L,
		dispidIVS_OCX_SetSkin = 521L,
		dispidIVS_OCX_SetDevPWD = 520L,
		dispidIVS_OCX_GetPaneWnd = 519L,
		dispidIVS_OCX_GetRecordThumbnail = 518L,
		dispidIVS_OCX_StopPUPlayBackTVWall = 517L,
		dispidIVS_OCX_StartPUPlayBackTVWall = 516L,
		dispidIVS_OCX_PlayBackResumeTVWall = 515L,
		dispidIVS_OCX_PlayBackPauseTVWall = 514L,
		dispidIVS_OCX_ModLockBackupRecord  = 513L,
		dispidIVS_OCX_GetCameraSwitchPlanTVWall = 512L,
		dispidIVS_OCX_GetCameraSwitchPlanListTVWall = 511L,
		dispidIVS_OCX_DelCameraSwitchPlanTVWall = 510L,
		dispidIVS_OCX_AddCameraSwitchPlanTVWall = 509L,
		dispidIVS_OCX_DelGuardPos = 508L,	
        dispidIVS_OCX_PlaybackFrameStepForwardTVWall = 507L,  
        
		dispidIVS_OCX_ExportDevCfgFile = 506L,
        dispidIVS_OCX_ImportDevCfgFile = 505L,

        dispidIVS_OCX_StopDisasterRecoveryPlayBack = 504L,
        dispidIVS_OCX_StartDisasterRecoveryPlayBack = 503L,

	    dispidIVS_OCX_ModNetkeyboardCamera = 502L,
        dispidIVS_OCX_SaveNetkeyboardCamera = 501L,
        dispidIVS_OCX_GetNetkeyboardList = 500L,
        dispidIVS_OCX_GetPWDExpireInfo = 499L,
        dispidIVS_OCX_SetPaneEMap = 498L,
		dispidIVS_OCX_StopSyncPlayTVWall = 497L,
		dispidIVS_OCX_StartSyncPlayTVWall = 496L,
		dispidIVS_OCX_DeleteRealPlayBookmark = 495L,
		dispidIVS_OCX_ModifyRealPlayBookmark = 494L,
		dispidIVS_OCX_AddRealPlayBookmark = 493L,
		dispidIVS_OCX_StopBackupDownload = 492L,
		dispidIVS_OCX_StartBackupDownload = 491L,
        dispidIVS_OCX_StartDisasterRecoveryDownload = 490L,
        dispidIVS_OCX_StopDisasterRecoveryDownload = 489L,
		dispidIVS_OCX_ShowPaneBar = 488L,
		dispidIVS_OCX_ModLockRecord = 487L,
		dispidIVS_OCX_GetAlarmSnapshotList = 486L,
		dispidIVS_OCX_GetCameraThumbnail = 485L,
		dispidIVS_OCX_ShowInstantReplayBar = 484L,
		dispidIVS_OCX_StopBackupPlayBack = 483L,
		dispidIVS_OCX_StartBackupPlayBack = 482L,
        dispidIVS_OCX_SignOutUserByRole = 481L,
        dispidIVS_OCX_SetLogPath = 480L,
		dispidIVS_OCX_SetDeviceGuard = 479L,
		dispidIVS_OCX_CheckNewVersion = 478L,
		dispidIVS_OCX_GetExDomainRecordPlan = 477L,
		dispidIVS_OCX_DelExDomainRecordPlan = 476L,
		dispidIVS_OCX_ModExDomainRecordPlan = 475L,
		dispidIVS_OCX_AddExDomainRecordPlan = 474L,
		dispidIVS_OCX_GetExDomainRecordPolicy = 473L,
		dispidIVS_OCX_SetExDomainRecordPolicy = 472L,
        dispidIVS_OCX_GetCameraPlanInfo = 471L,
        dispidIVS_OCX_SetNetKeyBoardPort = 470L,
        dispidIVS_OCX_StopNetKeyBoard = 469L,
        dispidIVS_OCX_StartNetKeyBoard = 468L,
	    dispidIVS_OCX_GetDeviceCapability = 467L,
		dispidIVS_OCX_StopLocalPlayBack = 466L,
		dispidIVS_OCX_StartLocalPlayBack = 465L,
		dispidIVS_OCX_GetPaneWndID = 464L,
		dispidIVS_OCX_QueryWatermarkAlarmList = 463L,
		dispidIVS_OCX_WatermarkAlarmCancel = 462L,
		dispidIVS_OCX_WatermarkAlarmConfirm = 461L,
		dispidIVS_OCX_VerifyWatermark = 460L,
		dispidIVS_OCX_GetPrerecordCameraList = 458L,
		dispidIVS_OCX_OMUGetDevTypeByNodeCode = 457L,
		dispidIVS_OCX_OMUGetAlarmLinkageRuleDetail = 456L,
		dispidIVS_OCX_OMUGetAlarmLinkageRuleList = 455L,
		dispidIVS_OCX_OMUDelAlarmLinkageRule = 454L,
		dispidIVS_OCX_OMUModAlarmLinkageRule = 453L,
		dispidIVS_OCX_OMUAddAlarmLinkageRule = 452L,
		dispidIVS_OCX_GetDevGroupRemark = 451L,
		dispidIVS_OCX_SetDevGroupRemark = 450L,
		dispidIVS_OCX_OMUQueryDiskUsageRateReport = 449L,
        dispidIVS_OCX_OMUQueryPacketLossRateReportDetail = 448L,
        dispidIVS_OCX_OMUQueryPacketLossRateReport = 447L,
        dispidIVS_OCX_OMUQueryRecordIntegralityReportDetail = 446L,
        dispidIVS_OCX_OMUQueryRecordIntegralityReport = 445L,
        dispidIVS_OCX_OMUQueryRecordPlanReportDetail = 444L,
        dispidIVS_OCX_OMUQueryRecordPlanReport = 443L,
        dispidIVS_OCX_OMUQueryQDReportDetail = 442L,
        dispidIVS_OCX_OMUQueryQDReport = 441L,
        dispidIVS_OCX_OMUQueryRealtimeReportDetail = 440L,
        dispidIVS_OCX_OMUQueryRealtimeReport = 439L,
        dispidIVS_OCX_OMUQueryDevReportDetail = 438L,
        dispidIVS_OCX_OMUQueryDevReport = 437L,
        dispidIVS_OCX_DeleteBookmarkForBackup = 436L,
		dispidIVS_OCX_ModifyBookmarkForBackup = 435L,
		dispidIVS_OCX_GetBookmarkListForBackup = 434L,
		dispidIVS_OCX_AddBookmarkForBackup = 433L,
		dispidIVS_OCX_GetMotionRangeDataPlatform = 432L,
		dispidIVS_OCX_LockBackupRecord = 431L,
		dispidIVS_OCX_UnlockBackupRecord  = 430L,
		dispidIVS_OCX_GetVideoNumAndResource = 429L,
		dispidIVS_OCX_LockRecord = 428L,
		dispidIVS_OCX_UnlockRecord = 427L,
		dispidIVS_OCX_GetCameraSwitchPlan = 426L,
		dispidIVS_OCX_GetCameraSwitchPlanList = 425L,
		dispidIVS_OCX_DelCameraSwitchPlan = 424L,
		dispidIVS_OCX_AddCameraSwitchPlan = 423L,
		dispidIVS_OCX_GetPWDStrength = 422L,
        dispidIVS_OCX_GetAlarmLinkageAction = 421L,
		dispidIVS_OCX_RefreshWnd = 420L,
		dispidIVS_OCX_SetWndDrag = 419L,
		dispidIVS_OCX_SetEventReceiver = 418L,
		dispidIVS_OCX_AddRecordBackupTask = 417L,
		dispidIVS_OCX_DelRecordBackupTask = 416L,
		dispidIVS_OCX_GetTemplateList = 415L,
		dispidIVS_OCX_GetDriverList = 414L,
		dispidIVS_OCX_StopRecordBackupTask = 413L,
		dispidIVS_OCX_StartRecordBackupTask = 412L,
		dispidIVS_OCX_GetRecordBackupTaskList = 411L,
		dispidIVS_OCX_StopPUPlayBack = 410L,
		dispidIVS_OCX_StartPUPlayBack = 409L,
		dispidIVS_OCX_GetDownloadInfo = 408L,
		dispidIVS_OCX_DownloadResume = 407L,
		dispidIVS_OCX_DownloadPause = 406L,
		dispidIVS_OCX_StopPUDownload = 405L,
		dispidIVS_OCX_StartPUDownload = 404L,
		dispidIVS_OCX_StopPlatformDownload = 403L,
		dispidIVS_OCX_StartPlatformDownload = 402L,
		dispidIVS_OCX_SignOutUser = 401L,
		dispidIVS_OCX_QueryPerformance = 400L,
	    dispidIVS_OCX_GetRecordTask = 399L,
		dispidIVS_OCX_StopSyncPlay = 398L,
		dispidIVS_OCX_StartSyncPlay = 397L,
		dispidIVS_OCX_PlaybackFrameStepBackward = 396L,
		dispidIVS_OCX_GetIACPUMemory = 395L,
		dispidIVS_OCX_GetShareCatalogList = 394L,
		dispidIVS_OCX_ShareCatalog = 393L,
		dispidIVS_OCX_GetExDomainList = 392L,
		dispidIVS_OCX_IALocusOverly  = 391L,
		dispidIVS_OCX_GetWndType = 390L,
		dispidIVS_OCX_SetCameraSwitchWnd = 389L,
        dispidIVS_OCX_DeleteBroadcastDevice = 388L,
        dispidIVS_OCX_AddBroadcastDevice = 387L,
        dispidIVS_OCX_StopFileBroadcast = 386L,
        dispidIVS_OCX_StartFileBroadcast = 385L,
        dispidIVS_OCX_StopRealBroadcast = 384L,
        dispidIVS_OCX_StartRealBroadcast = 383L,
		dispidIVS_OCX_GetDomainList = 382L,
		dispidIVS_OCX_DelSnapshotPlan = 381L,
		dispidIVS_OCX_GetRecordBackupPlan = 380L,
		dispidIVS_OCX_DeleteRecordBackupPlan = 379L,
		dispidIVS_OCX_ModifyRecordBackupPlan = 378L,
		dispidIVS_OCX_AddRecordBackupPlan = 377L,
		dispidIVS_OCX_GetCPUMemory = 376L,
		dispidIVS_OCX_GetDeviceAlarmList = 375L,
		dispidIVS_OCX_GetDecoderList = 374L,
		dispidIVS_OCX_GetDevLog = 373L,
		
		dispidIVS_OCX_VerifyDevice = 372L,
		dispidIVS_OCX_StopSearchDevice = 371L,
		dispidIVS_OCX_StartSearchDevice = 370L,
		dispidIVS_OCX_SetLanguage = 369L,
	    dispidIVS_OCX_GetHelpInfoText = 368L,
		dispidIVS_OCX_SetTVWallPaneStatus = 367L,
		dispidIVS_OCX_SetTVWallWndLayout = 366L,
		dispidIVS_OCX_DeleteAlarmWnd = 365L,
		dispidIVS_OCX_SetWndType = 364L,
		dispidIVS_OCX_GetCameraBackupConfig = 363L,
		dispidIVS_OCX_SetCameraBackupConfig = 362L,
		dispidIVS_OCX_StopAlarmOut = 361L,
		dispidIVS_OCX_StartAlarmOut = 360L,
		dispidIVS_OCX_StopPURecord  = 359L,
		dispidIVS_OCX_StartPURecord = 358L,
		dispidIVS_OCX_StopPlatformRecord  = 357L,
		dispidIVS_OCX_StartPlatformRecord = 356L,
		dispidIVS_OCX_SetWarnState = 355L,
		dispidIVS_OCX_UploadStatement = 354L,
		dispidIVS_OCX_GetStatement = 353L,
		dispidIVS_OCX_UploadLogo = 352L,
		dispidIVS_OCX_GetLogo = 351L,
        dispidIVS_OCX_GetSnapshotPlan = 350L,
		dispidIVS_OCX_SetSnapshotPlan = 349L,
		dispidIVS_OCX_DeleteSnapshot = 348L,
		dispidIVS_OCX_DownloadSnapshot = 347L,
		dispidIVS_OCX_GetSnapshotList = 346L,
		dispidIVS_OCX_PlatformSnapshot = 345L,
		dispidIVS_OCX_LocalSnapshot = 344L,
		dispidIVS_OCX_LockUser = 343L,
		dispidIVS_OCX_GetCameraSwitchInfo = 342L,
		dispidIVS_OCX_GetCameraSwitchList = 341L,
		dispidIVS_OCX_DelCameraSwitch = 340L,
		dispidIVS_OCX_AddCameraSwitch = 339L,
		dispidIVS_OCX_GetCruisePlan = 338L,
		dispidIVS_OCX_SetCruisePlan = 337L,
		dispidIVS_OCX_GetCruiseTrack = 336L,
		dispidIVS_OCX_GetCruiseTrackList = 335L,
		dispidIVS_OCX_ModCruiseTrack = 334L,
		dispidIVS_OCX_DelCruiseTrack = 333L,
		dispidIVS_OCX_AddCruiseTrack = 332L,
		dispidIVS_OCX_GetGuardPos = 331L,
		dispidIVS_OCX_SetGuardPos = 330L,
		dispidIVS_OCX_GetPTZPresetList = 329L,
		dispidIVS_OCX_ModPTZPreset = 328L,
		dispidIVS_OCX_DelPTZPreset = 327L,
		dispidIVS_OCX_AddPTZPreset = 326L,
		dispidIVS_OCX_DeleteBookmark = 325L,
		dispidIVS_OCX_ModifyBookmark = 324L,
		dispidIVS_OCX_GetBookmarkList = 323L,
		dispidIVS_OCX_AddBookmark = 322L,

		dispidIVS_OCX_SetGraphicsByPercent = 321L,
		dispidIVS_OCX_GetGraphicsByPercent = 320L,
		dispidIVS_OCX_GetConfigInfo = 319L,
		dispidIVS_OCX_RebootDevice = 318L,
		dispidIVS_OCX_ModifyTurnInspectPlan = 317L,
		dispidIVS_OCX_SetTurnInspectPlan = 316L,
		dispidIVS_OCX_DeleteTVWallTurnInspect = 315L,
		dispidIVS_OCX_ModifyTVWallTurnInspect = 314L,
		dispidIVS_OCX_AddTVWallTurnInspect = 313L,
		dispidIVS_OCX_GetTVWallTurnInspect = 312L,
		dispidIVS_OCX_DeleteTVWallGroupTurnInspect = 311L,
		dispidIVS_OCX_ModifyTVWallGroupTurnInspect = 310L,
		dispidIVS_OCX_AddTVWallGroupTurnInspect = 309L,
		dispidIVS_OCX_GetTVWallGroupTurnInspect = 308L,
		dispidIVS_OCX_DeleteTVWallScenarioFavorite = 306L,
		dispidIVS_OCX_ModifyTVWallScenarioFavorite = 305L,
		dispidIVS_OCX_GetTVWallScenarioFavoriteList = 304L,
		dispidIVS_OCX_AddTVWallScenarioFavorite = 303L,
		dispidIVS_OCX_DeleteTVWallScenario = 302L,
		dispidIVS_OCX_ModifyTVWallScenario = 301L,
		dispidIVS_OCX_AddTVWallScenario = 300L,
		dispidIVS_OCX_GetTVWallScenarioList = 299L,
		dispidIVS_OCX_SetCameraName = 298L,
		dispidIVS_OCX_SetMainDeviceName = 297L,
		dispidIVS_OCX_GetScenarioFavorite = 295L,
		dispidIVS_OCX_GetScenarioFavoriteList = 294L,
		dispidIVS_OCX_DelScenarioFavorite = 293L,
		dispidIVS_OCX_ModScenarioFavorite = 292L,
		dispidIVS_OCX_AddScenarioFavorite = 291L,
		dispidIVS_OCX_GetTempScenario = 290L,
		dispidIVS_OCX_SetTempScenario = 289L,
		dispidIVS_OCX_GetRecordSearchInfo = 288L,
		dispidIVS_OCX_SetRecordSearchInfo = 287L,
		dispidIVS_OCX_SetRelationShip = 286L,
		dispidIVS_OCX_DeleteGraphicsByGroup = 285L,
		dispidIVS_OCX_GetGraphicsByGroup = 284L,
		dispidIVS_OCX_DrawGraphics = 296L,

		dispidIVS_OCX_SelectCancel = 283L,
		dispidIVS_OCX_SelectGraphicByGroup = 282L,
		dispidIVS_OCX_StopDrawGraphics = 281L,
		dispidIVS_OCX_GetTVWallByID = 280L,
		dispidIVS_OCX_GetTVWallTempScenario = 279L,
		dispidIVS_OCX_SetTVWallTempScenario = 278L,
		dispidIVS_OCX_GetDeviceUserList = 277L,
		dispidIVS_OCX_GetDeviceList = 276L,
		dispidIVS_OCX_StopPlatformPlayBackTVWall = 275L,
		dispidIVS_OCX_StartPlatformPlayBackTVWall = 274L,
		dispidIVS_OCX_StopRealPlayTVWall = 273L,
		dispidIVS_OCX_StartRealPlayTVWall = 272L,
		dispidIVS_OCX_PopupFloderDialog = 271L,
		dispidIVS_OCX_DeleteTVWall = 270L,
		dispidIVS_OCX_ModifyTVWallLayout = 269L,
		dispidIVS_OCX_ModifyTVWallInfo = 268L,
		dispidIVS_OCX_AddTVWall = 267L,
		dispidIVS_OCX_GetTVWallList = 266L,
		dispidIVS_OCX_GetGraphics = 265L,
		dispidIVS_OCX_DisableDraw = 264L,
		dispidIVS_OCX_EnableDraw = 263L,
		dispidIVS_OCX_StopLocalRecord  = 262L,
		dispidIVS_OCX_StartLocalRecord = 261L,
		dispidIVS_OCX_DeleteGraphics = 260L,
		dispidIVS_OCX_DeleteGraphicsSelected = 259L,
		dispidIVS_OCX_SetGraphics = 258L,
		dispidIVS_OCX_SetPlayBackTime = 257L,
		dispidIVS_OCX_WinUserLogin = 254L,
		dispidIVS_OCX_SetTVWallStatus = 253L,
		dispidIVS_OCX_CleanupTVWall = 252L,
		dispidIVS_OCX_InitTVWall = 251L,
        dispidIVS_OCX_SetIAToolbar = 250L,
        dispidIVS_OCX_GetPlayBackStatus = 249L,
        dispidIVS_OCX_GetPlayBackTime = 248L,
        dispidIVS_OCX_GetPlayBackSpeed = 247L,
        dispidIVS_OCX_SetPlayBackSpeed = 246L,
        dispidIVS_OCX_PlaybackFrameStepForward = 245L,
        dispidIVS_OCX_PlayBackResume = 244L,
        dispidIVS_OCX_PlayBackPause = 243L,
        dispidIVS_OCX_StopPlatformPlayBack = 242L,
        dispidIVS_OCX_StartPlatformPlayBack = 241L,
        dispidIVS_OCX_GetAutoCreatePWD = 240L,
        dispidIVS_OCX_CheckPWDByRole = 239L,
        dispidIVS_OCX_GetOperationRightGroup = 238L,
        dispidIVS_OCX_GetDefaultRoleAccountRule = 237L,
        dispidIVS_OCX_GetRoleInfoByUserID = 236L,
        dispidIVS_OCX_GetRoleInfoByRoleID =235L,
        dispidIVS_OCX_RestoreUserData = 234L,
        dispidIVS_OCX_BackupUserData = 233L,
        dispidIVS_OCX_GetUserID = 232L,
        dispidIVS_OCX_SetRecordPolicyByTime = 231L,
        dispidIVS_OCX_EnableGPU = 230L,
        dispidIVS_OCX_AlarmReport = 229L,
        dispidIVS_OCX_GetRecordStatus = 228L,
        dispidIVS_OCX_PtzControl = 227L,
        dispidIVS_OCX_AlarmCancel = 226L,
        dispidIVS_OCX_AlarmConfirm = 225L,
        dispidIVS_OCX_GetChannelList = 224L,
        dispidIVS_OCX_GetRecordPolicy = 223L,
        dispidIVS_OCX_AlarmCommission = 222L,
        dispidIVS_OCX_SetPlayQualityMode = 220L,
        dispidIVS_OCX_IA = 219L,
        dispidIVS_OCX_QueryRecordList = 218L,
        dispidIVS_OCX_GetAlarmEventInfo = 217L,
        dispidIVS_OCX_StopAlarmAreaGuard = 215L,
        dispidIVS_OCX_StartAlarmAreaGuard = 214L,
        dispidIVS_OCX_GetAlarmEventList = 213L,
        dispidIVS_OCX_AddDeviceToGroup = 212L,

        dispidIVS_OCX_DeleteAlarmLinkage = 204L,
        dispidIVS_OCX_ModifyAlarmLinkage = 203L,
        dispidIVS_OCX_GetAlarmLinkageList = 202L,
        dispidIVS_OCX_GetAlarmLinkage = 201L,
        dispidIVS_OCX_AddAlarmLinkage = 200L,
        dispidIVS_OCX_UserAuthentication = 194L,
        dispidIVS_OCX_GetAlarmAreaList = 193L,
        dispidIVS_OCX_DeleteAlarmArea = 192L,
        dispidIVS_OCX_ModifyAlarmArea = 191L,
        dispidIVS_OCX_GetAlarmAreaInfo = 199L,
        dispidIVS_OCX_AddAlarmArea = 198L,
        dispidIVS_OCX_GetAreaGuardPlan = 197L,
        dispidIVS_OCX_DeleteAreaGuardPlan = 196L,
        dispidIVS_OCX_ModifyAreaGuardPlan = 195L,
        dispidIVS_OCX_GetDomainRoute = 190L,
        dispidIVS_OCX_AllocAreaGuardPlan = 189L,

        //dispidIVS_OCX_GetFavoriteDevice = 185L,
        dispidIVS_OCX_SubscribeAlarmQuery = 187L,
        dispidIVS_OCX_SubscribeAlarm = 186L,
        dispidIVS_OCX_GetFavoriteDevice = 185L,
        dispidIVS_OCX_MoveFavoriteFolder = 184L,
        dispidIVS_OCX_ModFavoriteFolder = 183L,
        dispidIVS_OCX_DelFavoriteFolder = 182L,
        dispidIVS_OCX_AddFavoriteFolder = 181L,
        dispidIVS_OCX_MoveFavoriteDevice = 180L,
        dispidIVS_OCX_DelFavoriteDevice = 179L,
        dispidIVS_OCX_AddFavoriteDevice = 178L,
        dispidIVS_OCX_SetDisplayScale = 177L,
        dispidIVS_OCX_GetMediaInfo = 175L,
        dispidIVS_OCX_SetAlarmTypeLevel = 176L,
        dispidIVS_OCX_QueryHelpInfo = 174L,
        dispidIVS_OCX_DeleteHelpInfo = 173L,

        dispidIVS_OCX_GetAlarmLevel = 172L,
        dispidIVS_OCX_Aes256CbcDecode = 170L,
        dispidIVS_OCX_Aes256CbcEncode = 169L,
        dispidIVS_OCX_GetAlarmTypeList = 168L,

        dispidIVS_OCX_GetDeviceConfig= 167L,
        dispidIVS_OCX_SetDeviceConfig= 166L,
        dispidIVS_OCX_GetAlarmLevelList = 165L,
        dispidIVS_OCX_DeleteDeviceFromGroup = 164L,    
        dispidIVS_OCX_DeleteDeviceGroup = 162L,
        dispidIVS_OCX_AddDeviceGroup = 161L,
        dispidIVS_OCX_MoveDeviceGroup = 160L,
        dispidIVS_OCX_ModifyDeviceGroupName = 159L,
        dispidIVS_OCX_AddRecordPlan = 158L,
        dispidIVS_OCX_DeleteDevice = 157L,
        dispidIVS_OCX_GetDeviceInfo = 156L,
        dispidIVS_OCX_GetDeviceGroup = 155L,
        dispidIVS_OCX_GetNVRList = 154L,
        dispidIVS_OCX_GetGPUDecodeAbility = 153L,
        dispidIVS_OCX_GetRecordList = 152L,

        dispidIVS_OCX_GetCustomWndLayout = 150L,
        dispidIVS_OCX_SetCustomWndLayout = 149L,
        dispidIVS_OCX_SignOutOnlineUser = 147L,
        dispidIVS_OCX_GetUserDeviceList = 146L,
        dispidIVS_OCX_SetUserDeviceList = 145L,
        dispidIVS_OCX_GetOperationLog = 144L,

        dispidIVS_OCX_ModifyHelpInfo = 142L,
        dispidIVS_OCX_AddHelpInfo = 141L,
        dispidIVS_OCX_GetRecordSpace = 140L,
        dispidIVS_OCX_ModifyRecordSpace = 139L,
        dispidIVS_OCX_DeleteRecordSpace = 138L,
        dispidIVS_OCX_SetRecordSpace = 137L,
        dispidIVS_OCX_DeleteAlarmLevel = 136L,
        dispidIVS_OCX_ModifyAlarmLevel = 135L,
        dispidIVS_OCX_AddAlarmLevel = 134L,
        dispidIVS_OCX_GetOnlineUser = 133L,
        dispidIVS_OCX_GetRoleList = 132L,
        dispidIVS_OCX_ModifyRole = 131L,
        dispidIVS_OCX_DeleteRole = 130L,
        dispidIVS_OCX_AddRole = 129L,
        dispidIVS_OCX_MoveUserGroup = 128L,
        dispidIVS_OCX_ModifyUserGroup = 127L,
        dispidIVS_OCX_DeleteUserGroup = 126L,
        dispidIVS_OCX_AddUserGroup = 125L,
        dispidIVS_OCX_GetKeyword = 124L,
        dispidIVS_OCX_DelKeyword = 123L,
        dispidIVS_OCX_AddKeyword = 122L,
        dispidIVS_OCX_QueryDeviceList = 119L,
        dispidIVS_OCX_AddDevice = 117L,
        dispidIVS_OCX_GetRecordPlan = 116L,
        dispidIVS_OCX_DeleteRecordPlan = 115L,
        dispidIVS_OCX_ModifyRecordPlan = 114L,


        dispidIVS_OCX_ModifyUser = 120L,
        dispidIVS_OCX_DeleteUser = 90L,
        dispidIVS_OCX_GetUserInfo = 106L,
        dispidIVS_OCX_GetUserList = 91L,
        dispidIVS_OCX_ResetPWD = 108L,
        dispidIVS_OCX_ChangePWD = 107L,
        dispidIVS_OCX_GetUserGroupList = 109L,
        dispidIVS_OCX_DelLocalCameraLinkConfig = 105L,
        dispidIVS_OCX_GetLocalCameraLinkConfig = 104L,
        dispidIVS_OCX_SetLocalCameraLinkConfig = 103L,
        dispidIVS_OCX_GetLocalPTZConfig = 102L,
        dispidIVS_OCX_SetLocalPTZConfig = 101L,
        dispidIVS_OCX_GetLocalNetWorkConfig = 100L,
        dispidIVS_OCX_SetLocalNetWorkConfig = 99L,
        dispidIVS_OCX_GetLocalCaptureConfig = 98L,
        dispidIVS_OCX_SetLocalCaptureConfig = 97L,
        dispidIVS_OCX_GetLocalRecordConfig = 96L,
        dispidIVS_OCX_SetLocalRecordConfig = 95L,
        dispidIVS_OCX_GetLocalAlarmConfig = 94L,
        dispidIVS_OCX_SetLocalAlarmConfig = 93L,
        dispidIVS_OCX_Logout = 92L,
        dispidIVS_OCX_AddUser = 89L,
        dispidIVS_OCX_SetToolbar = 88L,
        dispidIVS_OCX_Login = 87L,
        dispidIVS_OCX_SetColor = 86L,
        dispidIVS_OCX_SetWndLayout = 85L,
        dispidIVS_OCX_StopAllRealPlay = 83L,
        dispidIVS_OCX_GetVersion = 82L,
        dispidIVS_OCX_Cleanup = 81L,
        dispidIVS_OCX_Init = 80L,
        dispidIVS_OCX_GetFreeWnd = 74L,
        dispidIVS_OCX_GetMouseWnd = 73L,
        dispidIVS_OCX_StopRealPlay = 64L,
        dispidIVS_OCX_StartRealPlay = 63L,
		dispidIVS_OCX_GetAllCameraAlias = 55L,
		dispidIVS_OCX_GetCameraAlias = 54L,
		dispidIVS_OCX_SetCameraAlias = 53L,
		dispidIVS_OCX_GetTalkbackVolume = 52L,
		dispidIVS_OCX_SetTalkbackVolume = 51L,
		dispidIVS_OCX_StopVoiceTalkback = 50L,
		dispidIVS_OCX_StartVoiceTalkback = 49L,
        dispidIVS_OCX_Set3DPTZ = 48L,
        dispidIVS_OCX_ShowToolbar = 47L,
        dispidIVS_OCX_ShowTitlebar = 46L,
        dispidIVS_OCX_SetToolbarButtonStatus = 45L,
        dispidIVS_OCX_GetCameraByWnd = 44L,
        dispidIVS_OCX_GetSelectWnd = 43L,
        dispidIVS_OCX_SetActiveWnd = 42L,
        dispidIVS_OCX_SetTitlebar = 41L,
		dispidIVS_OCX_GetVolume = 40L,
		dispidIVS_OCX_SetVolume = 39L,
        dispidIVS_OCX_StopSound = 38L,
        dispidIVS_OCX_PlaySound = 37L,
        dispidIVS_OCX_SetDeviceName = 36L,
        dispidIVS_OCX_FullScreenDisplay = 35L,
        dispidIVS_OCX_NormalScreenDisplay = 34L
    };

    enum
    {
        eventidOnNetKeyBoard = 24L,
		eventidOnIVS_OCX_Windows_Event = 23L,
		eventidOnTVWall = 22L,
        eventidOnUserOffline = 26L,
        eventidOnKeepAlive = 21L,
        eventidOnAlarmReporter = 20L,
        eventidOnAlarmLinkage = 19L,
        eventidOnOCXInfoButtonClick = 18L,
        eventidIVS_OCX_Event = 17L
    };

	// Implementation
protected:
	~CIVS_OCXPlayerCtrl();

	BEGIN_OLEFACTORY(CIVS_OCXPlayerCtrl)        //lint !e1768  MFC自动生成的的函数  Class factory and guid
	END_OLEFACTORY(CIVS_OCXPlayerCtrl)

	DECLARE_OLETYPELIB(CIVS_OCXPlayerCtrl)      //lint !e1768  MFC自动生成的的函数 GetTypeInfo
	DECLARE_PROPPAGEIDS(CIVS_OCXPlayerCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CIVS_OCXPlayerCtrl)		// Type name and misc status

	// Message maps
	DECLARE_MESSAGE_MAP()

	// Dispatch maps
	DECLARE_DISPATCH_MAP()

	// Event maps
	DECLARE_EVENT_MAP()

protected:

#pragma region EventDef
	void OnOCXInfoButtonClick(ULONG ulWndID, LPCTSTR strDeviceName, ULONG ulVideoState, ULONG ulAudioState, ULONG ulFPS, ULONG ulCodeRate, ULONG ulVideoFomate)
	{
		FireEvent(eventidOnOCXInfoButtonClick, EVENT_PARAM(VTS_UI4 VTS_BSTR VTS_UI4 VTS_UI4 VTS_UI4 VTS_UI4 VTS_UI4), ulWndID, strDeviceName, ulVideoState, ulAudioState, ulFPS, ulCodeRate, ulVideoFomate);
	}

	void OnKeepAlive(ULONG ulResult)
	{
		FireEvent(eventidOnKeepAlive, EVENT_PARAM(VTS_UI4), ulResult);
	}

	void OnAlarmLinkage(LPCTSTR pAlarmLinkage)
	{
		FireEvent(eventidOnAlarmLinkage, EVENT_PARAM(VTS_BSTR), pAlarmLinkage);
	}
    
    //如果CU希望单独处理该事件而不是笼统的IVS_OCX_Event
    void OnUserOffline(LPCTSTR pEventContent)
    {
        FireEvent(eventidOnUserOffline, EVENT_PARAM(VTS_BSTR), pEventContent);
    }

	void OnAlarmReporter(LPCTSTR pAarmReporter)
	{
		FireEvent(eventidOnAlarmReporter, EVENT_PARAM(VTS_BSTR), pAarmReporter);
	}

	void IVS_OCX_Event(LONG lEventType, LPCTSTR pEventContent)
	{
		FireEvent(eventidIVS_OCX_Event, EVENT_PARAM(VTS_I4 VTS_BSTR), lEventType, pEventContent);
	}

	void IVS_OCX_OnTVWall(LPCTSTR pTVWallMsg)
	{
		FireEvent(eventidOnTVWall, EVENT_PARAM(VTS_BSTR), pTVWallMsg);
	}

	void IVS_OCX_Windows_Event(LONG lEventType, LPCTSTR pEventContent)
	{
		FireEvent(eventidOnIVS_OCX_Windows_Event, EVENT_PARAM(VTS_I4 VTS_BSTR), lEventType, pEventContent);
	}

    void OnNetKeyBoard(LPCTSTR pNetKeyBoardMsg)
    {
        FireEvent(eventidOnNetKeyBoard, EVENT_PARAM(VTS_BSTR), pNetKeyBoardMsg);
    }

#pragma endregion EventDef
public:
	//获取指定类型的ocx
	CIVS_OCXPlayerCtrl* GetOcxPlayerCtrl(ULONG ulWndType)const;
	//通过窗体指针停止实况播放
	ULONG StopRealPlayByPane(CVideoPane * pVideoPane);
	ULONG StopRealPlayByPaneEx(CVideoPane * pVideoPane);

    ULONG StopPlayBackByPane(CVideoPane * pVideoPane, BOOL bPlatForm);
protected:
#pragma region IVS_OCX_Function
    LONG  IVS_OCX_GetMouseWnd(void);
    LONG IVS_OCX_NormalScreenDisplay(void);
    LONG IVS_OCX_FullScreenDisplay(void);
    LONG IVS_OCX_GetVersion(void)const;
    LONG IVS_OCX_Init(void);
    LONG IVS_OCX_Cleanup(void);
    ULONG IVS_OCX_StopAllRealPlay(void);
    LONG IVS_OCX_SetWndLayout(ULONG lWindowsNum);
    LONG IVS_OCX_SetDeviceName(ULONG ulWndID, LPCTSTR pDevName);
    LONG  IVS_OCX_SetTitlebar(LONG lShowStatus)const;
    LONG IVS_OCX_Login(LPCTSTR pUserName, LPCTSTR pPWD, LPCTSTR pServerIP,LPCTSTR pPort, ULONG ulClientType);
    LONG IVS_OCX_StartRealPlay(LPCTSTR pMediaPara, LPCTSTR pCameraCode, LONG lWndID);
    LONG IVS_OCX_StopRealPlay(LONG lWndID);
    BSTR IVS_OCX_AddUser(LPCTSTR pReqXml);
    LONG IVS_OCX_DeleteUser(LPCTSTR pDomainCode, ULONG ulUserID);
    BSTR IVS_OCX_GetUserList(LPCTSTR pUnifiedQuery);
    LONG IVS_OCX_Logout(void);
    BOOL IVS_OCX_GetGPUDecodeAbility(void);
    LONG IVS_OCX_EnableGPU(BOOL bEnable);
    LONG IVS_OCX_SetToolbarButtonStatus(LONG lWndID, ULONG ulToolbarBtn, ULONG ulButtonStatus);
    LONG IVS_OCX_ShowTitlebar(VARIANT_BOOL bShow);
    LONG IVS_OCX_ShowToolbar(VARIANT_BOOL bShow);
    ULONG IVS_OCX_SetActiveWnd(ULONG ulWndID);
    ULONG IVS_OCX_GetSelectWnd(void);
    BSTR IVS_OCX_GetCameraByWnd(ULONG ulWndID);
    ULONG IVS_OCX_PlaySound(LONG lWndID);
    ULONG IVS_OCX_StopSound(LONG lWndID);
    ULONG IVS_OCX_Set3DPTZ(ULONG ulWndID, ULONG ulIsStart3D);
	BSTR IVS_OCX_StartVoiceTalkback(LPCTSTR pTalkParam, LPCTSTR pCameraCode);
	LONG IVS_OCX_StopVoiceTalkback(ULONG ulTalkbackHandle);
	LONG IVS_OCX_SetTalkbackVolume(ULONG ulVolumeValue);
	ULONG IVS_OCX_GetTalkbackVolume(void);
	ULONG IVS_OCX_SetCameraAlias(LPCTSTR pCameraCode, LPCTSTR pAlias);
	BSTR IVS_OCX_GetCameraAlias(LPCTSTR pCameraCode);
	BSTR IVS_OCX_GetAllCameraAlias(void);
#pragma endregion IVS_OCX_Function
private:

public:
	// 开始实况浏览，停止实况浏览
	LONG StartRealSteam(IVS_REALPLAY_PARAM &realplayParam, LPCTSTR pCameraID,
		        CVideoPane *pPanel, BOOL bLayoutChange, ULONG ulStreamOpenMode);

	void IVS_OCX_SetPlayQualityMode(ULONG ulWndID, LONG lpalyQualityMode);
	BSTR IVS_OCX_GetMediaInfo(ULONG ulWndID);

    LONG StartSound(ULONG ulWndID);
    LONG StopSound(ULONG ulWndID);
	ULONG StartTalkback(ULONG ulWinID);
	ULONG StopTalkback(ULONG ulWinID)const;

    LONG SetVolume(ULONG ulWndID, ULONG ulType, unsigned int uiVolumeValue);
    LONG GetVolume(ULONG ulWndID, ULONG ulType, unsigned int* puiVolumeValue);
	ULONG SetAlarmState(HWND paneHandle,BOOL bIsOpen);
	LONG SetWndType(CVideoPane * pWnd,ULONG ulTyle);
	LONG SetNoMalWnd(CVideoPane * pWnd);
	LONG SetAlarmWnd(CVideoPane * pWnd);
	LONG SetTurnCruiseWnd(CVideoPane * pWnd);
	LONG SetEMapWnd(CVideoPane * pWnd);

	void PostLocRecEventGeneric(int code, int PanelID);

	// 停止各种资源，包括局部放大，语音对讲，随路语音等
	void StopPlayResource(CVideoPane* pVideoPane)const;
	void DoButtonFilter(LONG& ulWndID);
	void ResumeMagnifyStatus(CVideoPane* pVideoPane)const;
	//全屏下按键上抛事件
	ULONG FullScreenKeyEvent(ULONG wlMsg);

    void NotifyStartRealPlayTVWallRes(ULONG ulTVWallID, ULONG ulWndID, IVS_INT32 iErrCode, ULONG ulTransID);
    LONG DealStartRealPlayTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pRealplayParam, ULONG ulWndID, BOOL bStopInterfaceSend, ULONG ulTransID);
protected:
	LONG IVS_OCX_GetFreeWnd(void);
public:
	afx_msg void OnMove(int x, int y);
protected:
    LONG IVS_OCX_SetIAToolbar(ULONG ulToolbarFlag);
	LONG IVS_OCX_SetToolbar(ULONG ulToolbarFlag);
	ULONG IVS_OCX_SetLocalAlarmConfig(LPCTSTR pAlarmParm);
	BSTR IVS_OCX_GetLocalAlarmConfig(void);
	ULONG IVS_OCX_SetLocalRecordConfig(LPCTSTR pRecordParm);
	BSTR IVS_OCX_GetLocalRecordConfig(void);
	ULONG IVS_OCX_SetLocalCaptureConfig(LPCTSTR pCaptureParm);
	BSTR IVS_OCX_GetLocalCaptureConfig(void);
	ULONG IVS_OCX_SetLocalNetWorkConfig(LPCTSTR pNetWorkParm);
	BSTR IVS_OCX_GetLocalNetWorkConfig(void);
	ULONG IVS_OCX_SetLocalPTZConfig(LPCTSTR pPTZParm);
	BSTR IVS_OCX_GetLocalPTZConfig(void);
	ULONG IVS_OCX_SetLocalCameraLinkConfig(LPCTSTR pCameraCode, ULONG ulLinkMode);
	ULONG IVS_OCX_GetLocalCameraLinkConfig(LPCTSTR pCameraCode);
	ULONG IVS_OCX_DelLocalCameraLinkConfig(LPCTSTR pCameraCode);
	BSTR IVS_OCX_GetUserGroupList(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetUserInfo(LPCTSTR pDomainCode, ULONG ulUserID);
	LONG IVS_OCX_ChangePWD(LPCTSTR pOldPWD, LPCTSTR pNewPWD);
	LONG IVS_OCX_ResetPWD(LPCTSTR pDomainCode, ULONG ulUserID, LPCTSTR pNewPWD);
    LONG IVS_OCX_CheckPWDByRole(LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName, LPCTSTR pPWD);
	BSTR IVS_OCX_AddRecordPlan( LPCTSTR pRecordPlan);
	BSTR IVS_OCX_ModifyRecordPlan(LPCTSTR pRecordPlan);
	BSTR IVS_OCX_DeleteRecordPlan( LPCTSTR pDelRecPlanXML);
	BSTR IVS_OCX_GetRecordPlan(LPCTSTR pRecordPlanReq);
	BSTR IVS_OCX_GetRecordPolicyByTime(LPCTSTR pCameraCode);
	//BSTR IVS_OCX_SetRecordPolicy(LPCTSTR pRecordPolicyXml);
    BSTR IVS_OCX_AddDevice(LPCTSTR pReqDev);
    BSTR IVS_OCX_QueryDeviceList(ULONG ulDeviceType, LPCTSTR pQueryXml);
    LONG IVS_OCX_ModifyUser(LPCTSTR pReqXml);	
	ULONG IVS_OCX_AddKeyword(LPCTSTR pCameraCode, LPCTSTR pDeviceKeyWord);
	ULONG IVS_OCX_DelKeyword(LPCTSTR pCameraCode);
	BSTR IVS_OCX_GetKeyword(void);
	BSTR IVS_OCX_AddUserGroup(LPCTSTR pUserGroup);
	LONG IVS_OCX_DeleteUserGroup(LPCTSTR pDomainCode, ULONG ulUserGroupID);
	LONG IVS_OCX_ModifyUserGroup(LPCTSTR pUserGroup);
	LONG IVS_OCX_MoveUserGroup(LPCTSTR pUserGroup);
    BSTR IVS_OCX_AddRole(LPCTSTR pRoleInfo);
    LONG IVS_OCX_DeleteRole(LPCTSTR pDomainCode, ULONG ulRoleID);
    LONG IVS_OCX_ModifyRole(LPCTSTR pRoleInfo);
    BSTR IVS_OCX_GetRoleList(LPCTSTR pDomainCode);
    BSTR IVS_OCX_GetRoleInfoByRoleID(LPCTSTR pDomainCode, ULONG ulRoleID);
    BSTR IVS_OCX_GetAutoCreatePWD(LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName);
    BSTR IVS_OCX_GetRoleInfoByUserID(LPCTSTR pDomainCode, ULONG ulUserID);
    BSTR IVS_OCX_GetDefaultRoleAccountRule(LPCTSTR pDomainCode);
    BSTR IVS_OCX_GetOperationRightGroup(LPCTSTR pDomainCode);
    BSTR IVS_OCX_GetOnlineUser(LPCTSTR pUnifiedQuery);
	BSTR IVS_OCX_GetOperationLog(LPCTSTR pUnifiedQuery);
	BSTR IVS_OCX_AddAlarmLevel(LPCTSTR pAlarmLevel);
	LONG IVS_OCX_ModifyAlarmLevel(LPCTSTR pReqXml);
	LONG IVS_OCX_DeleteAlarmLevel(ULONG ulAlarmLevelID);
	LONG IVS_OCX_SetRecordSpace(LPCTSTR pRecordSpace);
	LONG IVS_OCX_DeleteRecordSpace(LPCTSTR pRecordSpace);
	LONG IVS_OCX_ModifyRecordSpace(LPCTSTR pRecordSpace);
	LONG IVS_OCX_GetRecordSpace(LPCTSTR pRecordSpace);
	LONG IVS_OCX_AddHelpInfo(LPCTSTR helpInfo);
	LONG IVS_OCX_ModifyHelpInfo(LPCTSTR helpInfo);

	LONG IVS_OCX_SetUserDeviceList(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetUserDeviceList(LPCTSTR pUnifiedQuery);
	LONG IVS_OCX_SignOutOnlineUser(LPCTSTR pDomainCode, ULONG ulUserOnlineID);
	LONG IVS_OCX_SignOutUser(LPCTSTR pDomainCode, ULONG ulUserID);
	LONG IVS_OCX_SetCustomWndLayout(ULONG ulLayoutID, LPCTSTR pLayoutXml);
	BSTR IVS_OCX_GetCustomWndLayout(ULONG ulLayoutID);

	BSTR IVS_OCX_GetRecordList(LPCTSTR pCameraCode,ULONG lRecordMethod,LPCTSTR pStartTime,LPCTSTR pEndTime,ULONG lFromIndex,ULONG lToIndex);
	BSTR IVS_OCX_QueryRecordList(LPCTSTR pUnifiedQuery);
	LONG IVS_OCX_ModifyDeviceGroupName(LPCTSTR pReqXml);
	LONG IVS_OCX_MoveDeviceGroup(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetNVRList(LPCTSTR pUnifiedQuery);
	BSTR IVS_OCX_GetDeviceGroup(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetDeviceInfo(LPCTSTR pReqXml);

	BSTR IVS_OCX_DeleteDevice(LPCTSTR pReqXml);
	BSTR IVS_OCX_AddDeviceGroup(LPCTSTR pReqXml);
	LONG IVS_OCX_DeleteDeviceFromGroup(LPCTSTR pReqXml);
	LONG IVS_OCX_DeleteDeviceGroup(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetAlarmLevelList(void);
	LONG IVS_OCX_SetDeviceConfig(LONG iConfigType, LPCTSTR pDevCode, LPCTSTR pReqXml);
	BSTR IVS_OCX_GetDeviceConfig(LONG iConfigType, LPCTSTR pDevCode);

	BSTR IVS_OCX_GetAlarmTypeList(LPCTSTR pReqXml);
	BSTR IVS_OCX_Aes256CbcEncode(LPCTSTR pszPlain);
	BSTR IVS_OCX_Aes256CbcDecode(LPCTSTR pszCipher);

	BSTR IVS_OCX_GetAlarmLevel(LONG uiAlarmLevelID);

	LONG IVS_OCX_DeleteHelpInfo(LPCTSTR helpInfo);
	BSTR IVS_OCX_QueryHelpInfo(LPCTSTR helpInfo);
	LONG IVS_OCX_SetAlarmTypeLevel(LPCTSTR pReqXml);
	LONG IVS_OCX_SetDisplayScale(LONG lDisplayScale);
	BSTR IVS_OCX_AddFavoriteDevice(LPCTSTR pReqXml);
	LONG IVS_OCX_DelFavoriteDevice(LPCTSTR pReqXml);
	LONG IVS_OCX_MoveFavoriteDevice(LPCTSTR pReqXml);
	BSTR IVS_OCX_AddFavoriteFolder(LPCTSTR pReqXml);
	LONG IVS_OCX_DelFavoriteFolder(LPCTSTR pReqXml);
	LONG IVS_OCX_ModFavoriteFolder(LPCTSTR pReqXml);
	LONG IVS_OCX_MoveFavoriteFolder(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetFavoriteDevice(void);
	LONG IVS_OCX_AllocAreaGuardPlan(LPCTSTR pReqXml);
	LONG IVS_OCX_SubscribeAlarm(LPCTSTR pReqXml);
	BSTR IVS_OCX_SubscribeAlarmQuery(LPCTSTR pReqXml);

	LONG WM_KEYUPEvent(const MSG* pMsg);
	BSTR IVS_OCX_GetDomainRoute(void);
	LONG IVS_OCX_ModifyAlarmArea(LPCTSTR pReqXml);
	LONG IVS_OCX_DeleteAlarmArea(LPCTSTR pDomainCode,ULONG ulAlarmAreaID);
	BSTR IVS_OCX_GetAlarmAreaList(LPCTSTR pReqXml);
	LONG IVS_OCX_ModifyAreaGuardPlan(LPCTSTR pReqXml);
	LONG IVS_OCX_DeleteAreaGuardPlan(LPCTSTR pDomainCode,ULONG ulAlarmAreaId);
	BSTR IVS_OCX_GetAreaGuardPlan(LPCTSTR pDomainCode,ULONG ulAlarmAreaId);
	BSTR IVS_OCX_AddAlarmArea(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetAlarmAreaInfo(LPCTSTR pDomainCode,ULONG ulAlarmAreaId);
	LONG IVS_OCX_UserAuthentication(LPCTSTR pPWD);
	BSTR IVS_OCX_AddAlarmLinkage(LPCTSTR pReqXml);
	LONG IVS_OCX_ModifyAlarmLinkage(LPCTSTR pReqXml);
	LONG IVS_OCX_DeleteAlarmLinkage(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetAlarmLinkage(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetAlarmLinkageList(LPCTSTR pReqXml);
	LONG IVS_OCX_AddDeviceToGroup(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetAlarmEventList(LPCTSTR pReqXml);
	LONG IVS_OCX_StartAlarmAreaGuard(LPCTSTR pDomainCode,ULONG ulAlarmAreaId);
	LONG IVS_OCX_StopAlarmAreaGuard(LPCTSTR pDomainCode,ULONG ulAlarmAreaId);
	LONG IVS_OCX_AlarmCommission(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetAlarmEventInfo(LONG uiAlarmEventId, LPCTSTR pAlarmInCode);
	BSTR IVS_OCX_IA(ULONG ulInterfaceType, LPCTSTR pReqXml);

    BSTR IVS_OCX_GetChannelList(LPCTSTR pDevCode);

	BSTR IVS_OCX_PtzControl(LPCTSTR pCameraCode,LONG lOperateCode, LPCTSTR pControlPara1, LPCTSTR pControlPara2);
	LONG IVS_OCX_AlarmReport(LPCTSTR pAlarmReport);
	BSTR IVS_OCX_GetRecordStatus(LPCTSTR pCameraCode,ULONG ulRecordMethod);

	LONG IVS_OCX_AlarmConfirm(LPCTSTR pReqXml);
	LONG IVS_OCX_AlarmCancel(LPCTSTR pReqXml);
    BSTR IVS_OCX_GetHelpInfoText(LPCTSTR pAlarmInCode, LPCTSTR pAlarmType);
	LONG IVS_OCX_SetRecordPolicyByTime(LPCTSTR pRecordPolicyXml);
	BSTR IVS_OCX_GetUserID(void);
    LONG IVS_OCX_BackupUserData(void);
    LONG IVS_OCX_RestoreUserData(void);
    LONG IVS_OCX_StartPlatformPlayBack(LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID);
    LONG IVS_OCX_StopPlatformPlayBack(LONG lWndID);

    LONG IVS_OCX_StartDisasterRecoveryPlayBack(LPCTSTR pCameraCode,LPCTSTR pPlaybackParam,LONG lWndID);
    LONG IVS_OCX_StopDisasterRecoveryPlayBack(LONG lWndID);

    LONG IVS_OCX_PlayBackPause(LONG lWndID);
    LONG IVS_OCX_PlayBackResume(LONG lWndID);
    LONG IVS_OCX_PlaybackFrameStepForward(LONG lWndID);
    LONG IVS_OCX_SetPlayBackSpeed(LONG lWndID, FLOAT fSpeed);
    BSTR IVS_OCX_GetPlayBackSpeed(LONG lWndID);
    BSTR IVS_OCX_GetPlayBackTime(LONG lWndID);
    BSTR IVS_OCX_GetPlayBackStatus(LONG lWndID);
    LONG IVS_OCX_SetPlayBackTime(LONG lWndID, ULONG ulTime);
	BSTR IVS_OCX_GetAlarmLinkageAction(LPCTSTR pReqXml);

public:
    afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
protected:
	LONG IVS_OCX_InitTVWall(void);
	LONG IVS_OCX_CleanupTVWall(void);

	LONG IVS_OCX_WinUserLogin(LPCTSTR pUserName, LPCTSTR pPWD, LPCTSTR pServerIP, LPCTSTR pServerPort, LPCTSTR pDomainName, ULONG ulLoginType, ULONG ulClientType);
	BSTR IVS_OCX_GetRecordTask();
	
private:
	//电视墙回调函数
	static IVS_INT32 __stdcall HandleTVWallNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData);//lint !e601

public:
    //网络键盘回调函数
    static IVS_INT32 __stdcall NetKeyBoardCallBack(IVS_INT32 iKeyValue,IVS_INT32 iSpeedFirst,IVS_INT32 iSpeedSecond, bool bTvWall);

    //开始电视墙实况线程函数
    static void __cdecl StartRealPlayTVWallThreadFun(LPVOID lpvoid);
    //停止电视墙实况线程函数
    static void __cdecl StopRealPlayTVWallThreadFun(LPVOID lpvoid);

    //停止电视上墙(同步)
    LONG SynStopRealPlayTVWall(ULONG ulTVWallID, ULONG ulWndID);

    void CleanupTVWall(void);
protected:
	LONG IVS_OCX_SetGraphics(ULONG ulWindowID, LPCTSTR pGraphicsXml);
	LONG IVS_OCX_DeleteGraphicsSelected(ULONG ulWindowID);
	LONG IVS_OCX_DeleteGraphics(ULONG ulWindowID);
	ULONG IVS_OCX_StartLocalRecord(LPCTSTR pRecordPara, LONG lWndID);
	LONG IVS_OCX_StopLocalRecord(LONG lWndID);
	LONG IVS_OCX_EnableDraw(ULONG ulWindowID);
	LONG IVS_OCX_DisableDraw(ULONG ulWindowID);
	BSTR IVS_OCX_GetGraphics(ULONG ulWindowID);
	BSTR IVS_OCX_GetTVWallList(void);
	BSTR IVS_OCX_AddTVWall(LPCTSTR pTVWall);
	LONG IVS_OCX_ModifyTVWallInfo(LPCTSTR pTVWall);
	LONG IVS_OCX_ModifyTVWallLayout(LPCTSTR pTVWall);
	LONG IVS_OCX_DeleteTVWall(ULONG ulTVWallID);
	BSTR IVS_OCX_PopupFloderDialog(void);
	LONG IVS_OCX_StartRealPlayTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pRealplayParam, ULONG ulWndID);
	LONG IVS_OCX_StopRealPlayTVWall(ULONG ulTVWallID, ULONG ulWndID);
	LONG IVS_OCX_StartPlatformPlayBackTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, ULONG ulWndID);
	LONG IVS_OCX_StopPlatformPlayBackTVWall(ULONG ulTVWallID, ULONG ulWndID);
	BSTR IVS_OCX_GetDeviceList(ULONG ulDeviceType, ULONG ulFromIndex, ULONG ulToIndex);
	BSTR IVS_OCX_GetDeviceUserList(LPCTSTR pDomainCode, LPCTSTR pDevCode, ULONG ulIsUserOnline, ULONG ulFromIndex, ULONG ulToIndex);
	LONG IVS_OCX_SetRecordSearchInfo(LPCTSTR pSearchInfo);
	BSTR IVS_OCX_GetRecordSearchInfo(void);
	LONG IVS_OCX_SetTempScenario(LPCTSTR pScenarioXml);
	BSTR IVS_OCX_GetTempScenario(void);
	BSTR IVS_OCX_AddScenarioFavorite(LPCTSTR pScenarioXml);
	LONG IVS_OCX_ModScenarioFavorite(LPCTSTR pScenarioXml);
	LONG IVS_OCX_DelScenarioFavorite(LPCTSTR pScenarioID);
	BSTR IVS_OCX_GetScenarioFavoriteList(void);
	BSTR IVS_OCX_GetScenarioFavorite(LPCTSTR pScenarioID);

	BSTR IVS_OCX_GetGraphicsByGroup(ULONG ulWindowID, ULONG ulGraphicGroup);
	LONG IVS_OCX_DeleteGraphicsByGroup(ULONG ulWindowID, ULONG ulGraphicGroup);
	LONG IVS_OCX_SetRelationShip(ULONG ulWindowID, ULONG ulGraphicGroupA, ULONG ulGraphicGroupB, ULONG ulRelationShipType);
	LONG IVS_OCX_StopDrawGraphics(ULONG ulWindowID, ULONG ulGraphicGroup);
	LONG IVS_OCX_SelectGraphicByGroup(ULONG ulWindowID, ULONG ulGraphicGroup);
	LONG IVS_OCX_SelectCancel(ULONG ulWindowID);

	BSTR IVS_OCX_GetTVWallByID(ULONG ulTVWallID);

	LONG IVS_OCX_DrawGraphics(ULONG ulWindowID, LPCTSTR pGraphicsXml);
	LONG IVS_OCX_SetMainDeviceName(LPCTSTR pDevCode, LPCTSTR pNewDevName);
	LONG IVS_OCX_SetCameraName(LPCTSTR pCameraCode, LPCTSTR pNewCameraName);
	BSTR IVS_OCX_GetTVWallGroupTurnInspect(ULONG ulTVWallID);
	BSTR IVS_OCX_AddTVWallGroupTurnInspect(LPCTSTR pTVWall);
	LONG IVS_OCX_ModifyTVWallGroupTurnInspect(LPCTSTR pTVWall);
	LONG IVS_OCX_DeleteTVWallGroupTurnInspect(ULONG ulTVWallID);
	BSTR IVS_OCX_GetTVWallTurnInspect(ULONG ulTVWallID);
	LONG IVS_OCX_AddTVWallTurnInspect(LPCTSTR pTVWall);
	LONG IVS_OCX_ModifyTVWallTurnInspect(LPCTSTR pTVWall);
	LONG IVS_OCX_DeleteTVWallTurnInspect(ULONG ulTVWallID);
	LONG IVS_OCX_SetTurnInspectPlan(LPCTSTR pTurnInspectPlanInfo);
	LONG IVS_OCX_ModifyTurnInspectPlan(LPCTSTR pTurnInspectPlanInfo);
	LONG IVS_OCX_RebootDevice(LPCTSTR pDeviceCode, ULONG ulRebootType);
	BSTR IVS_OCX_GetConfigInfo(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetGraphicsByPercent(ULONG ulWindowID);
	LONG IVS_OCX_SetGraphicsByPercent(ULONG ulWindowID, LPCTSTR pGraphicsXml);

	BSTR IVS_OCX_AddBookmark(LPCTSTR pNVRCode,LPCTSTR pCameraCode,LPCTSTR pBookmarkName,LPCTSTR pBookmarkTime);
	BSTR IVS_OCX_GetBookmarkList(LPCTSTR pQueryXml);
	LONG IVS_OCX_ModifyBookmark(LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID,LPCTSTR pNewBookmarkName);
	LONG IVS_OCX_DeleteBookmark(LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID);
	BSTR IVS_OCX_AddPTZPreset(LPCTSTR pCameraCode, LPCTSTR pPresetName);
	LONG IVS_OCX_DelPTZPreset(LPCTSTR pCameraCode, ULONG ulPresetIndex);
	LONG IVS_OCX_ModPTZPreset(LPCTSTR pCameraCode, ULONG ulPresetIndex, LPCTSTR pPresetName);
	BSTR IVS_OCX_GetPTZPresetList(LPCTSTR pCameraCode);
	LONG IVS_OCX_SetGuardPos(LPCTSTR pGuardPosInfo);
	BSTR IVS_OCX_GetGuardPos(LPCTSTR pCameraCode);
	BSTR IVS_OCX_AddCruiseTrack(LPCTSTR pCuriseTrackInfo);
	LONG IVS_OCX_DelCruiseTrack(LPCTSTR pCameraCode, ULONG ulTrackIndex, ULONG ulCruiseType);
	LONG IVS_OCX_ModCruiseTrack(LPCTSTR pCuriseTrackInfo);
	BSTR IVS_OCX_GetCruiseTrackList(LPCTSTR pCameraCode);
	BSTR IVS_OCX_GetCruiseTrack(LPCTSTR pCameraCode, ULONG ulTrackIndex);
	LONG IVS_OCX_SetCruisePlan(LPCTSTR pCruisePlan);
	BSTR IVS_OCX_GetCruisePlan(LPCTSTR pCameraCode);
	BSTR IVS_OCX_AddCameraSwitch(LPCTSTR pSwitchInfo);
	LONG IVS_OCX_DelCameraSwitch(LPCTSTR pCameraSwitchID);
	BSTR IVS_OCX_GetCameraSwitchList(void);
	BSTR IVS_OCX_GetCameraSwitchInfo(LPCTSTR pCameraSwitchID);
	LONG IVS_OCX_LockUser(LPCTSTR pDomainCode, ULONG ulUserID, ULONG ulLock);
	LONG IVS_OCX_LocalSnapshot(LONG lWndID);
	LONG IVS_OCX_PlatformSnapshot(LPCTSTR pCameraCode);
	BSTR IVS_OCX_GetSnapshotList(LPCTSTR pQueryParam);
	LONG IVS_OCX_DownloadSnapshot(ULONG ulPictureID, LPCTSTR pFileName)const;
	LONG IVS_OCX_DeleteSnapshot(LPCTSTR pCameraCode, ULONG ulPictureID);
	LONG IVS_OCX_SetSnapshotPlan(LPCTSTR pSnapshotPlan);
	BSTR IVS_OCX_GetSnapshotPlan(LPCTSTR pCameraCode);
    LONG IVS_OCX_StartAlarmOut(LPCTSTR pAlarmOutCode);
	LONG IVS_OCX_StopAlarmOut(LPCTSTR pAlarmOutCode);
	BSTR IVS_OCX_GetDeviceAlarmList(LPCTSTR pReqXml);
	LONG IVS_OCX_GetLogo(ULONG ulLogoType, LPCTSTR pLogoFile);
	LONG IVS_OCX_UploadLogo(ULONG ulLogoType,LPCTSTR pLogoFile);
	BSTR IVS_OCX_GetStatement(LPCTSTR pLanguageType);
	LONG IVS_OCX_UploadStatement(LPCTSTR pLanguageType,LPCTSTR pStatementFile);
	LONG IVS_OCX_SetWarnState(ULONG ulWndID, ULONG ulWarnState, ULONG ulWarnSeconds);
	LONG IVS_OCX_StartPlatformRecord(LPCTSTR pCameraCode,ULONG ulRecordTime);
	LONG IVS_OCX_StopPlatformRecord (LPCTSTR pCameraCode);
	LONG IVS_OCX_StartPURecord(LPCTSTR pCameraCode,ULONG ulRecordTime);
	LONG IVS_OCX_StopPURecord (LPCTSTR pCameraCode);
	LONG IVS_OCX_SetCameraBackupConfig(LPCTSTR pBackupXml);
	BSTR IVS_OCX_GetCameraBackupConfig(LPCTSTR pCameraCode)const;
	LONG IVS_OCX_SetWndType(ULONG ulWindID,ULONG ulTyle);
	LONG IVS_OCX_DeleteAlarmWnd(ULONG ulWindID);
	LONG IVS_OCX_SetTVWallWndLayout(ULONG ulTVWallID);
	LONG IVS_OCX_SetTVWallPaneStatus(ULONG ulTVWallID, ULONG ulPaneID, ULONG ulPaneStatus);
	LONG IVS_OCX_SetLanguage(LPCTSTR pLanguage);
	LONG IVS_OCX_StartSearchDevice(LPCTSTR pDomainCode, LPCTSTR pSearchInfo);
	LONG IVS_OCX_StopSearchDevice(LPCTSTR pDomainCode, LPCTSTR pSearchInfo);
	BSTR IVS_OCX_VerifyDevice(LPCTSTR pDomainCode, LPCTSTR pVerifyInfo);

	BSTR IVS_OCX_GetDevLog(LPCTSTR pQueryXml);
	BSTR IVS_OCX_GetDecoderList(void);
	BSTR IVS_OCX_GetCPUMemory(LPCTSTR pNodeCode);
	LONG IVS_OCX_AddRecordBackupPlan(LPCTSTR pBackupPlan);
	LONG IVS_OCX_ModifyRecordBackupPlan(LPCTSTR pBackupPlan);
	LONG IVS_OCX_DeleteRecordBackupPlan(LPCTSTR pBackupPlan);
	BSTR IVS_OCX_GetRecordBackupPlan(LPCTSTR pQueryInfo)const;
	LONG IVS_OCX_DelSnapshotPlan(LPCTSTR pCameraCode);
	BSTR IVS_OCX_GetDomainList(void);
    LONG IVS_OCX_StartRealBroadcast(void)const;
    LONG IVS_OCX_StopRealBroadcast(void)const;
    LONG IVS_OCX_StartFileBroadcast(LPCTSTR pFileName, ULONG ulCycle)const;
    LONG IVS_OCX_StopFileBroadcast(void)const;
    LONG IVS_OCX_AddBroadcastDevice(LPCTSTR pCameraCode);
    LONG IVS_OCX_DeleteBroadcastDevice(LPCTSTR pCameraCode);
	LONG IVS_OCX_SetCameraSwitchWnd(ULONG ulWndID,ULONG ulStyle);
	LONG IVS_OCX_GetWndType(ULONG ulWndID);
	LONG IVS_OCX_IALocusOverly(ULONG ulWinID, ULONG ulRuleType, ULONG ulStartOverly);
	LONG IVS_OCX_ShareCatalog(LPCTSTR pShareInfo);
	BSTR IVS_OCX_GetShareCatalogList(LPCTSTR pQueryInfo);
	BSTR IVS_OCX_GetIACPUMemory(LPCTSTR pNodeCode);
	BSTR IVS_OCX_GetExDomainList(LPCTSTR pQueryInfo);
	LONG IVS_OCX_PlaybackFrameStepBackward(LONG lWndID);
	LONG IVS_OCX_StartSyncPlay(LPCTSTR pSyncPlayInfo);
	LONG IVS_OCX_StopSyncPlay(void)const;
	BSTR IVS_OCX_QueryPerformance(LPCTSTR pNodeCode);
	BSTR IVS_OCX_StartPlatformDownload(LPCTSTR pCameraCode, LPCTSTR pStartTime, LPCTSTR pEndTime, LPCTSTR pDownloadParamXml);
	LONG IVS_OCX_StopPlatformDownload(ULONG ulDownloadHandle)const;
    BSTR IVS_OCX_StartDisasterRecoveryDownload( LPCTSTR pCameraCode, LPCTSTR pStartTime, LPCTSTR pEndTime, LPCTSTR pDownloadParamXml);
    LONG IVS_OCX_StopDisasterRecoveryDownload(ULONG ulDownloadHandle);
	BSTR IVS_OCX_StartPUDownload(LPCTSTR pCameraCode, LPCTSTR pStartTime, LPCTSTR pEndTime, LPCTSTR pDownloadParamXml)const;
	LONG IVS_OCX_StopPUDownload(ULONG ulDownloadHandle)const;
	LONG IVS_OCX_DownloadPause(ULONG ulDownloadHandle)const;
	LONG IVS_OCX_DownloadResume(ULONG ulDownloadHandle)const;
	BSTR IVS_OCX_GetDownloadInfo(ULONG ulDownloadHandle)const;
	LONG IVS_OCX_StartPUPlayBack(LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID);
	LONG IVS_OCX_StopPUPlayBack(LONG lWndID);
	LONG IVS_OCX_SetTVWallTempScenario(LPCTSTR pScenarioXml);
	BSTR IVS_OCX_GetTVWallTempScenario(ULONG pScenarioID);
	BSTR IVS_OCX_GetRecordBackupTaskList (LPCTSTR pQueryInfo)const;
	LONG IVS_OCX_StartRecordBackupTask (LPCTSTR pQueryInfo);
	LONG IVS_OCX_StopRecordBackupTask (LPCTSTR pQueryInfo);
	BSTR IVS_OCX_GetDriverList(LPCTSTR pReqXml)const;
	BSTR IVS_OCX_GetTemplateList(LPCTSTR pReqXml)const;
	LONG IVS_OCX_DelRecordBackupTask (LPCTSTR pQueryInfo);
	LONG IVS_OCX_AddRecordBackupTask(LPCTSTR pBackupTask);
	void IVS_OCX_SetEventReceiver(void);
	LONG IVS_OCX_SetWndDrag(ULONG ulWindID,ULONG ulEnable);
	void IVS_OCX_RefreshWnd(ULONG ulRefreshType);
    BSTR IVS_OCX_GetPWDStrength(LPCTSTR pPWD);
	LONG IVS_OCX_AddCameraSwitchPlan(LPCTSTR pSwitchPlanInfo);
	LONG IVS_OCX_DelCameraSwitchPlan(LPCTSTR pCameraSwitchID);
	BSTR IVS_OCX_GetCameraSwitchPlanList(void);
	BSTR IVS_OCX_GetCameraSwitchPlan(LPCTSTR pCameraSwitchID);
	LONG IVS_OCX_UnlockRecord(LPCTSTR pUnlockXml);
	LONG IVS_OCX_LockRecord(LPCTSTR pLockXml);
	BSTR IVS_OCX_GetVideoNumAndResource(LPCTSTR pCameraCode)const;
	LONG IVS_OCX_UnlockBackupRecord(LPCTSTR pUnlockXml);
	LONG IVS_OCX_LockBackupRecord(LPCTSTR pLockXml);
	BSTR IVS_OCX_GetMotionRangeDataPlatform(LPCTSTR pCameraCode,LPCTSTR pStartTime,LPCTSTR pEndTime,LONG lFromIndex,LONG lToIndex);
	LONG IVS_OCX_AddBookmarkForBackup(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode,LPCTSTR pBookmarkName,LPCTSTR pBookmarkTime);
	BSTR IVS_OCX_GetBookmarkListForBackup(LPCTSTR pQueryXml);
	LONG IVS_OCX_ModifyBookmarkForBackup(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID,LPCTSTR pNewBookmarkName);
	LONG IVS_OCX_DeleteBookmarkForBackup(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID);

    BSTR IVS_OCX_OMUQueryDevReport(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryDevReportDetail(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryRealtimeReport(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryRealtimeReportDetail(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryQDReport(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryQDReportDetail(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryRecordPlanReport(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryRecordPlanReportDetail(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryRecordIntegralityReport(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryRecordIntegralityReportDetail(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryPacketLossRateReport(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryPacketLossRateReportDetail(LPCTSTR pReqXml);
    BSTR IVS_OCX_OMUQueryDiskUsageRateReport(LPCTSTR pReqXml);
	LONG IVS_OCX_SetDevGroupRemark(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetDevGroupRemark(LPCTSTR pReqXml);
	BSTR IVS_OCX_OMUAddAlarmLinkageRule(LPCTSTR pReqXml);
	LONG IVS_OCX_OMUModAlarmLinkageRule(LPCTSTR pReqXml);
	LONG IVS_OCX_OMUDelAlarmLinkageRule(LPCTSTR pReqXml);
	BSTR IVS_OCX_OMUGetAlarmLinkageRuleList(LPCTSTR pReqXml);
	BSTR IVS_OCX_OMUGetAlarmLinkageRuleDetail(LPCTSTR pReqXml);
	BSTR IVS_OCX_OMUGetDevTypeByNodeCode(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetPrerecordCameraList();
	LONG IVS_OCX_VerifyWatermark(LONG lWndID, ULONG ulVerifyWatermark);
	LONG IVS_OCX_WatermarkAlarmConfirm(LPCTSTR pConfirmInfo);
	LONG IVS_OCX_WatermarkAlarmCancel(LPCTSTR pCancelInfo);
	BSTR IVS_OCX_QueryWatermarkAlarmList(LPCTSTR pQueryXml);
	BSTR IVS_OCX_GetPaneWndID(ULONG ulTVWallID, ULONG ulPaneID);
	LONG IVS_OCX_StartLocalPlayBack(LONG lWndID, LPCTSTR pFileName, LPCTSTR pPlaybackParam);
	LONG IVS_OCX_StopLocalPlayBack(LONG lWndID);
	BSTR IVS_OCX_GetDeviceCapability(LONG lCapabilityType, LPCTSTR pDevCode);	
    LONG IVS_OCX_StartNetKeyBoard(ULONG ulNetKeyBoardVendor, ULONG ulPort);
    LONG IVS_OCX_StopNetKeyBoard(void);
    LONG IVS_OCX_SetNetKeyBoardPort(ULONG ulPort);
    BSTR IVS_OCX_GetCameraPlanInfo(LPCTSTR pReqXml)const;
	LONG IVS_OCX_SetExDomainRecordPolicy(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetExDomainRecordPolicy(LPCTSTR pReqXml)const;
	BSTR IVS_OCX_AddExDomainRecordPlan(LPCTSTR pRecordPlanXml);
	BSTR IVS_OCX_ModExDomainRecordPlan(LPCTSTR pRecordPlan);
	BSTR IVS_OCX_DelExDomainRecordPlan(LPCTSTR pDelRecPlanXML);
	BSTR IVS_OCX_GetExDomainRecordPlan(LPCTSTR pRecordPlanListReq);
	BSTR IVS_OCX_CheckNewVersion(LPCTSTR pReqXml);
	LONG IVS_OCX_SetDeviceGuard(LPCTSTR pReqXml);
	LONG IVS_OCX_SetLogPath(LPCTSTR pLogPath);
    LONG IVS_OCX_SignOutUserByRole(LPCTSTR pDomainCode, ULONG ulRoleID);
	LONG IVS_OCX_StartBackupPlayBack(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID);
	LONG IVS_OCX_StopBackupPlayBack(LONG lWndID);
	LONG IVS_OCX_ShowInstantReplayBar(ULONG ulWndID,ULONG ulStyle);
	BSTR IVS_OCX_GetCameraThumbnail(LPCTSTR pCameraCode);
	BSTR IVS_OCX_GetAlarmSnapshotList(LPCTSTR pQueryParam);
	LONG IVS_OCX_ModLockRecord(LPCTSTR pLockXml);
	LONG IVS_OCX_ShowPaneBar(ULONG ulWndID,ULONG ulStyle);
	BSTR IVS_OCX_StartBackupDownload(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode, LPCTSTR pStartTime, LPCTSTR pEndTime, LPCTSTR pDownloadParamXml);
	LONG IVS_OCX_StopBackupDownload(ULONG ulDownloadHandle)const;
	BSTR IVS_OCX_AddRealPlayBookmark(LPCTSTR pCameraCode,LPCTSTR pBookmarkName,LPCTSTR pBookmarkTime);
	LONG IVS_OCX_ModifyRealPlayBookmark(LPCTSTR pCameraCode,ULONG ulBookmarkID,LPCTSTR pNewBookmarkName);
	LONG IVS_OCX_DeleteRealPlayBookmark(LPCTSTR pCameraCode,ULONG ulBookmarkID);
	LONG IVS_OCX_StartSyncPlayTVWall(ULONG ulTVWallID, LPCTSTR pSyncPlayInfo);
	LONG IVS_OCX_StopSyncPlayTVWall(ULONG ulTVWallID);
	BSTR IVS_OCX_SetPaneEMap(ULONG ulWndID);
    BSTR IVS_OCX_GetPWDExpireInfo(void);
    BSTR IVS_OCX_GetNetkeyboardList(void);
    LONG IVS_OCX_SaveNetkeyboardCamera(LPCTSTR pNetboardCameraList);
    LONG IVS_OCX_ModNetkeyboardCamera(LPCTSTR pNetboardCamera);
	LONG IVS_OCX_ImportDevCfgFile(LPCTSTR pDevCode, LPCTSTR pCFGFile);
	LONG IVS_OCX_ExportDevCfgFile(LPCTSTR pDevCode, LPCTSTR pCFGFilePath);
    LONG IVS_OCX_PlaybackFrameStepForwardTVWall(ULONG ulTVWallID, LONG lWndID);
	LONG IVS_OCX_DelGuardPos(LPCTSTR pGuardPosInfo);
	LONG IVS_OCX_AddCameraSwitchPlanTVWall(LPCTSTR pSwitchPlanInfo);
	LONG IVS_OCX_DelCameraSwitchPlanTVWall(ULONG ulTVWallID,LPCTSTR pCameraSwitchID);
	BSTR IVS_OCX_GetCameraSwitchPlanListTVWall(void);
	BSTR IVS_OCX_GetCameraSwitchPlanTVWall(ULONG ulTVWallID,LPCTSTR pCameraSwitchID);
	LONG IVS_OCX_ModLockBackupRecord(LPCTSTR pLockXml);
	LONG IVS_OCX_PlayBackPauseTVWall(ULONG ulTVWallID, LONG lWndID);
	LONG IVS_OCX_PlayBackResumeTVWall(ULONG ulTVWallID, LONG lWndID);
	LONG IVS_OCX_StartPUPlayBackTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID);
	LONG IVS_OCX_StopPUPlayBackTVWall(ULONG ulTVWallID, LONG lWndID);
	BSTR IVS_OCX_GetRecordThumbnail(LPCTSTR pThumbnailQuery)const;
	ULONG IVS_OCX_GetPaneWnd(ULONG ulWndID);
	LONG IVS_OCX_SetDevPWD(LPCTSTR pReqXml);
	LONG IVS_OCX_SetSkin(ULONG ulSkinType);
	LONG IVS_OCX_ResetAlarm(LPCTSTR pReqXml);
	LONG IVS_OCX_ExEventSubscribe(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetExEventSubscribe(LPCTSTR pReqXml)const;
	LONG IVS_OCX_ExCatalogSubscribe(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetExCatalogSubscribeList(LPCTSTR pReqXml)const;
	LONG IVS_OCX_StartBackupPlayBackTVWall(ULONG ulTVWallID, LPCTSTR pDomainCode, LPCTSTR pNVRCode, LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID);
	LONG IVS_OCX_StopBackuplayBackTVWall(ULONG ulTVWallID, LONG lWndID);
	ULONG IVS_OCX_GetAbilityInfo(void)const;
	LONG IVS_OCX_SetScenarioRecovery(LONG lRecovery)const;
	LONG IVS_OCX_GetScenarioRecovery(void)const;
	BSTR IVS_OCX_GetGraphicSelected(ULONG ulWindowID);
	void IVS_OCX_EnableExchangePane(ULONG ulStatus);
	LONG IVS_OCX_SetRealTimeTempScenario(LPCTSTR pRealTimeScenarioXml, LPCTSTR pScenarioID);
	LONG IVS_OCX_SetRecordTempScenario(LPCTSTR pRecordScenarioXml, LPCTSTR pScenarioID);
	LONG IVS_OCX_SetEmapTempScenario(LPCTSTR pEmapScenarionXml, LPCTSTR pScenarioID);
	BSTR IVS_OCX_GetRealTimeTempScenario(LPCTSTR pScenarioID);
	BSTR IVS_OCX_GetRecordTempScenario(LPCTSTR pScenarioID);
	BSTR IVS_OCX_GetEmapTempScenario(LPCTSTR pScenarioID);
	void IVS_OCX_KeyEventNotice(ULONG ulCtrlKey, ULONG ulKey);
	LONG IVS_OCX_SetVolume(LONG lWndID, ULONG ulVolumeValue);
	ULONG IVS_OCX_GetVolume(LONG lWndID);
	ULONG IVS_OCX_SetTitleBarStatus(ULONG ulWndID, ULONG ulStatusType, ULONG ulShow);
    LONG IVS_OCX_ModCameraSwitch(LPCTSTR pSwitchInfo);
	LONG IVS_OCX_DeleteBroadcastFile(LPCTSTR pFileName);
	BSTR IVS_OCX_GetBroadcastCameraList(void);
	LONG IVS_OCX_AddBroadcastFile(LPCTSTR pFileName);
	BSTR IVS_OCX_GetBroadcastFileList(void);
	BSTR IVS_OCX_ExGetDeviceStatus(LPCTSTR pReqXml)const;
	LONG IVS_OCX_ExGetDeviceInfo(LPCTSTR pReqXml);
    LONG IVS_OCX_RefreshCameraList()const;
    void IVS_OCX_SetLogLevel(ULONG ulLogLevel) const;
    BSTR IVS_OCX_GetDeviceEncodeCapabilities(LPCTSTR pDevCode)const;
	BSTR IVS_OCX_GetCUVersion(LPCTSTR pReqXml);
	LONG IVS_OCX_StartRealPlayTVWall_Async(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pRealplayParam, ULONG ulWndID, ULONG ulTransID);
	LONG IVS_OCX_StopRealPlayTVWall_Async(ULONG ulTVWallID, ULONG ulWndID);
	LONG IVS_OCX_StartRealPlay_Async(LPCTSTR pMediaPara, LPCTSTR pCameraCode, LONG lWndID, ULONG lTransID);
	LONG IVS_OCX_StopRealPlay_Async(LONG lWndID);
	
	
	LONG IVS_OCX_LoginByTicket(LPCTSTR pTicketID, LPCTSTR pServerIP, LPCTSTR pPort, ULONG ulClientType);
	LONG IVS_OCX_SetReceiveOMUAlarm(LONG bReceive);
	BSTR IVS_OCX_GetVideoDiagnose(LPCTSTR pDomainCode, LPCTSTR pCameraCode) const;
	LONG IVS_OCX_SetVideoDiagnose(LPCTSTR pReqXml);
	BSTR IVS_OCX_GetStreamListByCam(LPCTSTR pDomainCode, LPCTSTR pCameraCode);
	LONG IVS_OCX_ShutdownStreamByUser(LPCTSTR pDomainCode, ULONG ulUserID, LPCTSTR pCameraCode, LONG lStreamType, LONG lDuration);
	BSTR IVS_OCX_GetStreamListByUser(LPCTSTR pDomainCode, ULONG ulUserID);
	LONG IVS_OCX_SetMicVolume(ULONG ulVolumeValue);
	ULONG IVS_OCX_GetMicVolume(void);
	LONG IVS_OCX_LocalSnapshotToDir(LONG lWndID, ULONG ulPictureFormat, LPCTSTR pFileName);
	LONG IVS_OCX_SetVideoEffect(LONG lWndID, LONG lBrightValue, LONG lContrastValue, LONG lSaturationValue, LONG lHueValue);
}; //lint !e1712  MFC自动生成的的函数

