/*****************************************************************
 filename    :    VideoPane.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    窗体类
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#include "Resource.h"
#include "VideoStatic.h"
#include "TranVideoRealtimeBar.h"
#include "TranAnalysisToolBar.h"
#include "TranVideoTitleBar.h"
#include "TranTVWallStatusBar.h"
#include "IVS_IADrawAPI.h"
#include "IVS_IATrackAPI.h"
#include "TurnCruisePane.h"
#include "DitalPtzDlg.h"
#include "TranZoomDirctDlg.h"
#include "TranZoomBar.h"
#include "TranZoomTitleBar.h"
#include "PlayTimerBar.h"

const short  PANE_STATE_NOT_ACTIVATE         =  0  ;
const short  PANE_STATE_ACTIVATE             =  1  ;

const short  PANE_STATE_WARNING              =  10 ;
const short  WARN_TIMER_ID                   =  1  ;
const short  MAGNIFY_TIMER_ID                =  2  ;

const short  VIDEO_PLAY_STATE_NOT_PLAYING    =  0  ;
const short  VIDEO_PLAY_STATE_REALTIME       =  1  ;
const short  VIDEO_PLAY_STATE_VOD            =  2  ;
const short  VIDEO_PLAY_STATE_LOCALFILE      =  3  ;
const short  VIDEO_PLAY_STATE_RESTARTING     =  4  ;
const short  VIDEO_PLAY_STATE_STARTING       =  5  ;
const short  VIDEO_PLAY_STATE_EMAP           =  6  ;

const short  CIF_WIDTH                       =  352;
const short  CIF_HEIGHT                      =  288;

typedef enum 
{
	TVWALL_STATUS_NONE = 0,				//未上墙
	TVWALL_STATUS_START = 1,			//开始上墙
	TVWALL_STATUS_PLAY = 2,				//正在播放
	TVWALL_STATUS_VIDEO_BROKEN = 3,		//视频断流
	TVWALL_STATUS_DEC_DISCONN = 4		//解码器断连
}TVWALL_STATUS;

typedef enum
{
	TVWALL_BAR_STAUS_HIDE = 0,//隐藏
	TVWALL_BAR_STAUS_OFFLINE = 1,//离线
	TVWALL_BAR_STAUS_ONLINE = 2//在线
}TVWALLBAR_STATUS;

typedef struct
{
	HWND window;
	int x;
	int y;
	int cx;
	int cy;
}WindowMoveInfo;


// CVideoPane dialog

#ifndef _CVIDEOPANEMGR_DECLARATION_
#define _CVIDEOPANEMGR_DECLARATION_
class CVideoPaneMgr;
#endif

#ifndef _CIVS_OCXPLAYERCTRL_DECLARATION_
#define _CIVS_OCXPLAYERCTRL_DECLARATION_
class CIVS_OCXPlayerCtrl;
#endif

class CVideoPane : public CDialog
{
	DECLARE_DYNAMIC(CVideoPane)

public:
	CVideoPane(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVideoPane();

    CVideoPaneMgr * m_pVideoPaneMgr;
    COleControl * m_pOcxCtrl;
    std::vector<WindowMoveInfo> m_LISTMOVE;
// Dialog Data
	enum { IDD = IDD_DLG_VIDEOPANE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    void BeginPaneDrag(void)const;
    //  
    void ChangePaneSize(void)const;
    void ResetPaneSize();

    // 创建标题栏和底部工具栏
    void CreateBar(void);
    void CreateAnalysisBar();
	void CreateBarAnalysisBar();
    void CreateRealTimeBar();
	void CreateTVWallStatusBar()const;
	void CreatemTurnCruisePane();
	void DeleteTurnCruisePane();
	void CreateDigtalPtz();
	void DeleteDigtalPtz();
	void CreateZoomDirctDlg();
	void DeleteZoomDirctDlg();
	void CreateZoomBar();
	void DeleteZoomBar();
	void CreateZoomTitleBar();
	void CreateTimerBar();
	void DeleteZoomTitleBar();
	void DeleteTimerBar();

    // 删除声音控件
    void DeleteSoundCtrl( const CPoint &point ) const; 
	void DeleteTalkbackSound( const CPoint &point ) const;
private:
    // 窗格ID
    ULONG m_ulPaneID;
public:
    ULONG GetPaneID() const { return m_ulPaneID; }
    void SetPaneID(ULONG val) { m_ulPaneID = val; }
private:
    // 是否是活动窗格
    ULONG m_ulIsActivate;
public:
    void SetNotActivate()   { m_ulIsActivate = PANE_STATE_NOT_ACTIVATE;}
    void SetActivate()      { m_ulIsActivate = PANE_STATE_ACTIVATE; }
private:
    // 播放状态：实况、回放、本地文件、未播放
    ULONG m_ulPlayState;
public:
    ULONG GetPlayState() const { return m_ulPlayState; }
    void SetPlayState(ULONG val) { m_ulPlayState = val; }

    void GetPlayBackCurrentTime(ST_TIME& PlayTime);

private:
    // 警告时用于计算边框闪动多少次
    ULONG m_ulWarnCounter;
    // 警告时边框的最大闪动次数
    ULONG m_ulMaxWarnCount;
    // 记录用户是否开启随路音频
    bool m_bIsSoundOn;
public:
    bool IsSoundOn() const { return m_bIsSoundOn; }
    void IsSoundOn(bool val) { m_bIsSoundOn = val; }
private:
    // 记录当前窗格随路音频音量
    ULONG m_ulAudioVolume;
public:
    ULONG GetAudioVolume() const { return m_ulAudioVolume; }
    void SetAudioVolume(ULONG val) { m_ulAudioVolume = val; }

	void IsDrag(ULONG val) { m_ulIsDrag = val; }
private:
    // 标题对象
    CTranVideoTitleBar *m_pRealTimeTitleBar;
    // 实况工具栏对象
    CTranVideoRealtimeBar *m_pRealTimeToolBar;
    // 智能分析工具栏
    CTranAnalysisToolBar *m_pAnalysisToolBar;
	// 电视墙通道状态栏
	CTranTVWallStatusBar *m_pTVWallStatusBar;
	bool m_bIsShowTVWallBar;

	//轮训按钮界面
	CTurnCruisePane *m_pTurnCruisePane;
	//数字云台
	CDitalPtzDlg *m_pDigtalDlg;
	// 放大的移动
	CTranZoomDirctDlg *m_pZoomDirctDlg;
	// 放大工具条
	CTranZoomBar* m_pZoomBar;
	// 放大标题栏
	CTranZoomTitleBar* m_pZoomTitleBar;
	// 即时回放按钮
	CPlayTimerBar* m_pPlayTimerBar;

	Gdiplus::Image* m_pTVWallOnlineImage;
	Gdiplus::Image* m_pTVWallOfflineImage;

	//设置是否进行Drag
	ULONG m_ulIsDrag;
	//对双击关闭按钮进行的规避操作
	bool m_ulIsStopButtonDrag;
public:
	void SetStopDrag(bool bFlag) {m_ulIsStopButtonDrag = bFlag;}
public:
	ULONG GetWndDragStatus()const{return m_ulIsDrag;}
	bool GetTVWallStatusBarStatus()const {return m_bIsShowTVWallBar;}
public:
    CTranVideoTitleBar* GetVideoTitleBar() const { return m_pRealTimeTitleBar; }			//lint !e1763
    CTranVideoRealtimeBar* GetVideoRealtimeBar() const { return m_pRealTimeToolBar; }	//lint !e1763
    CTranAnalysisToolBar* GetAnalysisToolBar() const { return m_pAnalysisToolBar; }	//lint !e1763
	CTranTVWallStatusBar* GetTVWallStatusBar() const { return m_pTVWallStatusBar; } //lint !e1763
	CTurnCruisePane* GetTurnCruisePane() const { return m_pTurnCruisePane;}//lint !e1763
	CDitalPtzDlg* GetDigtalPtzDlg() const {return m_pDigtalDlg;} //lint !e1763
	CTranZoomDirctDlg* GetZoomDirctDlg() const {return m_pZoomDirctDlg;} //lint !e1763
	CTranZoomBar* GetZoomBar() const {return m_pZoomBar;} //lint !e1763
	CTranZoomTitleBar* GetZoomTitleBar() const {return m_pZoomTitleBar;} //lint !e1763
	CPlayTimerBar* GetPlayTimerBar() const {return m_pPlayTimerBar;} //lint !e1763
public:
	void SetTVWallStatusBarStatus(ULONG ulBarStatus);//设置电视墙通道状态
	void SetTVWallStatus(ULONG ulPaneStatus);//设置电视墙窗格状态
private:
    // 拖拽时候用于判断是否是第一次进入窗格
    bool m_isFirstEnter;
    // 记录窗格在ParentWnd中所占的位置
    CRect m_PaneRect;
public:
    void GetPaneRect( CRect * paneRect )const;
    void SetPaneRect(const CRect &paneRect );
private:
    // 记录窗格在ParentWnd中所占的位置（整个窗格，包含标题栏和底部工具栏）
    CRect m_PaneRectFull;
public:
    void SetPaneRectFull(const CRect &paneRect);
    void GetPaneRectFull( CRect * paneRect )const;
 private:
    // 显示比例
    ULONG m_ulDisplayScale;
public:
    ULONG GetDisplayScale() const { return m_ulDisplayScale; }
    void SetDisplayScale(ULONG val) { m_ulDisplayScale = val; }
private:
    // 3D云台定位的第一个点
    CPoint m_3DPTZStartPoint;
    // 3D云台是否开启
    ULONG m_ulIs3DPTZOpen;
public:
    ULONG Get3DPTZState() const { return m_ulIs3DPTZOpen; }
    void Set3DPTZState(ULONG val);
	void Set3DPTZStateOnly(ULONG val) { m_ulIs3DPTZOpen = val; }
private:
    // 是否正在录像
    ULONG m_ulIsRealtimeRecording;
public:
    ULONG GetRealtimeRecordState() const { return m_ulIsRealtimeRecording; }
    void SetRealtimeRecordState(ULONG val) { m_ulIsRealtimeRecording = val; }
private:
    // 是否正在抓拍
    ULONG m_ulIsRealtimeCapturing;
public:
    ULONG GetRealtimeCaptureState() const { return m_ulIsRealtimeCapturing; }
    void SetRealtimeCaptureState(ULONG val) { m_ulIsRealtimeCapturing = val; }
private:
    // 是否正在注册标签
    ULONG m_ulIsRealtimeLabling;
public:
    ULONG GetRealtimeLabelState() const { return m_ulIsRealtimeLabling; }
    void SetRealtimeLabelState(ULONG val) { m_ulIsRealtimeLabling = val; }
private:
    // 是否正在语音对讲
    ULONG m_ulIsRealtimeTalkbacking;
public:
    ULONG GetRealtimeTalkbackState() const { return m_ulIsRealtimeTalkbacking; }
    void SetRealtimeTalkbackState(ULONG val) { m_ulIsRealtimeTalkbacking = val; }
private:
    // 是否正在播放声音
    ULONG m_ulIsRealtimeSounding;
public:
    ULONG GetRealtimeSoundState() const { return m_ulIsRealtimeSounding; }
    void SetRealtimeSoundState(ULONG val) { m_ulIsRealtimeSounding = val; }
	
	//设置时候为刷新透明Bar
	void SetTranslucentStatus(BOOL val) {m_bIsHideTranslucentBar = val;}
private:
    // 是否正在局部放大,源窗口
    ULONG m_ulIsRealtimeMagnifying;
	// 目的窗口放大状态
	ULONG m_ulIsDstMagnify;

	// 放大框个数
	ULONG m_MagnifyNum;
	// 显示名字的计数,从1-3循环使用
	ULONG m_MagnifyNameCount[MAGNIFY_MAX_NUM];
	std::string m_strZoomName;
	// 框的颜色
	IA_COLOR m_IAColor;
	// 源窗口
	CVideoPane* m_pMagnifySrcPane;
	// 被占用为放大窗口的之前的状态，用于恢复
	ULONG m_ulPrvePlayState;
	// 保存源窗口被放大的坐标百分比
	IVS_RECT m_IArectPercent;
	// 被覆盖前的handle
	ULONG m_ulPrvePlayHandle;
	// 被快速录像回放覆盖的实况的句柄
	ULONG m_ulInstanstRealPlayHandle;
	// 快速录像回放的句柄
	ULONG m_ulInstanstPlayBackHandle;
	// 是否已经在快速录像回放
	BOOL m_bIsInstanstPlayBack;
	// 记录保存的工具栏的状态
	ULONGLONG m_ulToolBarStatus;
	// 是否是暂停状态
	BOOL m_bIsInstansPlayPause;
	// 绘图的结构体
	IA_GRAPHICS m_szGraphics[MAGNIFY_MAX_NUM];
	// 用于恢复的个数
	ULONG m_ResumeMagnifyNum;
	// 是否开启过局部放大，用于需要恢复的场景
	bool m_bOpendMagnifyStatus;
	// 是否开启3D被局部放大占用
	bool m_bOpend3DPTZStatus;
	// 曾开启分辨率
	SIZE m_szOpendMagnifySize;
	// 计算一次拖拽操作中移动如果没成功过，就放弃这此绘图
	ULONG m_DrawMoveSuccessSum;
	// 保存画图完成后的鼠标，用于模拟点击事件
	POINT m_MagnifyLastPoint;
	// 窗口类型
	TYPE_WND m_ulPrveTypeWnd;

	// 在放大情况下关闭实况需要恢复的摄像机编码
	std::string m_strOpendMagnifyCameraID;
	// 分辨率
	LONG m_DrawHeight;
	LONG m_DrawWidth;
public:
	afx_msg LRESULT OnMagnifyCloseAll(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMagnifyClose(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMagnifyRateChange(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMagnifyRectMove(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnInstantReplayMove(WPARAM wParam,LPARAM pParam);
	afx_msg LRESULT OnDeleteTurnCruise(WPARAM wParam,LPARAM pParam);
    afx_msg LRESULT OnInstantReplayStart(WPARAM wParam, LPARAM pParam);
	afx_msg LRESULT onInstantReplaySegStop(WPARAM wParam, LPARAM pParam);


	IVS_INT32 MoveSrcGraphicRect(int iDirect, ULONG ulGraphicGroup);
	IVS_INT32 ChangeSrcGraphicRect(int iDirect, ULONG ulGraphicGroup);
	void UpdataMagnifyWindow(ULONG ulGraphicGroup);
	void TransIARect2Percent(const CRect& rect, CRect& percentRect);
	void SortIARect2Point(POINT& p1, POINT& p2, POINT& p3, POINT& p4, const CRect& changeRect)const;
	IVS_INT32 GetIARectByGroup(ULONG uiGraphicGroup, CRect& rect);
	void GetIARectByGraphics(const IA_GRAPHICS& graphicsTemp, CRect& rect)const;
    ULONG GetRealtimeMagnifyState() const { return m_ulIsRealtimeMagnifying; }
    // 设置实况工具栏局部放大状态
    ULONG SetRealtimeMagnifyState(ULONG ulState);
	void SetMagnifySrcPane(CVideoPane* pSrc) { m_pMagnifySrcPane = pSrc;}
	CVideoPane* GetMagnifySrcPane() { return m_pMagnifySrcPane;}
	IVS_INT32 GetMediaSize(SIZE& size);// 获取图像的大小
	void MagnifyMouseDown( const CPoint &point );
	void MagnifyMouseUp( const CPoint &point );
	void StartMagnifyByGraphicPoint(const CPoint &point);
	void FinishMagnifyByGraphicPoint(const CPoint &point);
	ULONG GetDstMagnifyState() const { return m_ulIsDstMagnify; }
	void SetDstMagnifyState(ULONG val) { m_ulIsDstMagnify = val; }
	ULONG GetPrvePlayState() const { return m_ulPrvePlayState; }
	void SetPrvePlayState(ULONG val) { m_ulPrvePlayState = val; }
	TYPE_WND GetPrveTypeWnd() const { return m_ulPrveTypeWnd; }
	void SetPrveTypeWnd(TYPE_WND val) { m_ulPrveTypeWnd = val; }
	ULONG GetMagnifyPlayHandle() const { return m_ulPrvePlayHandle; }
	void SetMagnifyPlayHandle(ULONG val) { m_ulPrvePlayHandle = val; }
	bool GetOpendMagnifyStatus() const { return m_bOpendMagnifyStatus; }
	void SetOpendMagnifyStatus(bool val) { m_bOpendMagnifyStatus = val; }
	SIZE GetOpendMagnifySize() const { return m_szOpendMagnifySize; }
	void SetOpendMagnifySize(const SIZE& sz) { m_szOpendMagnifySize = sz; }
	const std::string& GetOpendMagnifyCameraID() const{ return m_strOpendMagnifyCameraID; }
	void SetOpendMagnifyCameraID( const std::string& strCameraID ) { m_strOpendMagnifyCameraID = strCameraID; }
	bool GetOpend3DPTZStatus() const { return m_bOpend3DPTZStatus; }
	void SetOpend3DPTZStatus(bool val) { m_bOpend3DPTZStatus = val; }
	int GetFreeNameNum(ULONG ulGraphicGroup);
	// 根据组id释放到占用的数字
	void FreeNameNum(ULONG ulGraphicGroup);
	void ResumeAllMagnifyRect();
	void ResumeMagnifyRectByGraphic(const IA_GRAPHICS& graphicsTemp);
	// 保存窗口的名称
	void SetZoomName(const std::string& strZoomName) { m_strZoomName = strZoomName; }
	std::string& GetZoomName() { return m_strZoomName; }//lint !e1536
	// 刷新tip
	void UpdateAllTipText();
	IVS_RECT GetIArectPercent() const { return m_IArectPercent; }
	void SetIArectPercent(const CRect& val){
		m_IArectPercent.top = val.top;
		m_IArectPercent.bottom = val.bottom;
		m_IArectPercent.left = val.left;
		m_IArectPercent.right = val.right;
	};
	int GetMagnifyRateByIARect(const CRect& IArect);
	LONG GetDrawHeight() const { return m_DrawHeight; }
	void SetDrawHeight(LONG val) { m_DrawHeight = val; }
	LONG GetDrawWidth() const { return m_DrawWidth; }
	void SetDrawWidth(LONG val) { m_DrawWidth = val; }
	void HideZoomWnd();
private:
    // 是否正在云台控制
    ULONG m_ulIsRealtimePtzing;
public:
    ULONG GetRealtimePtzState() const { return m_ulIsRealtimePtzing; }
    void SetRealtimePtzState(ULONG val);
private:
    // 是否正在告警
    ULONG m_ulIsRealtimeAlarming; //lint !e1764
public:
    ULONG GetRealtimeAlarmState() const { return m_ulIsRealtimeAlarming; }
    void SetRealtimeAlarmState(ULONG val) { m_ulIsRealtimeAlarming = val; }
private:
    // 是否正在视频上墙
    ULONG m_ulIsRealtimeWalling;
public:
    ULONG GetRealtimeWallState() const { return m_ulIsRealtimeWalling; }
    void SetRealtimeWallState(ULONG val) { m_ulIsRealtimeWalling = val; }
private:
    // 是否正在智能分析结果叠加
    ULONG m_ulIsRealtimeAnalysising;
public:
    ULONG GetRealtimeAnalysisState() const { return m_ulIsRealtimeAnalysising; }
    void SetRealtimeAnalysisState(ULONG val) { m_ulIsRealtimeAnalysising = val; }
private:
    // 是否正在显示实况工具栏
    ULONG m_ulIsRealtimeBarShowing;
public:
    ULONG GetRealtimeBarState() const { return m_ulIsRealtimeBarShowing; }
    void SetRealtimeBarState(ULONG val) { m_ulIsRealtimeBarShowing = val; }
private:
    // 是否正在录像回放页面中播放
    ULONG m_ulIsRecordPlaying;
public:
    ULONG GetRecordPlayState() const { return m_ulIsRecordPlaying; }
    void SetRecordPlayState(ULONG val) { m_ulIsRecordPlaying = val; }
private:
    // 是否正在录像回放页面中抓拍
    ULONG m_ulIsRecordCaptureing;
public:
    ULONG GetRecordCaptureState() const { return m_ulIsRecordCaptureing; }
    void SetRecordCaptureState(ULONG val) { m_ulIsRecordCaptureing = val; }
private:
    // 是否正在录像回放页面中标签
    ULONG m_ulIsRecordLabeling;
public:
    ULONG GetRecordLabelState() const { return m_ulIsRecordLabeling; }
    void SetRecordLabelState(ULONG val) { m_ulIsRecordLabeling = val; }
private:
    // 是否正在录像回放页面中声音
    ULONG m_ulIsRecordSounding;
public:
    ULONG GetRecordSoundState() const { return m_ulIsRecordSounding; }
    void SetRecordSoundState(ULONG val) { m_ulIsRecordSounding = val; }
private:
    // 是否正在录像回放页面中放大
    ULONG m_ulIsRecordMagnifying;
public:
    ULONG GetRecordMagnifyState() const { return m_ulIsRecordMagnifying; }
    void SetRecordMagnifyState(ULONG val) { m_ulIsRecordMagnifying = val; }
private:
    // 是否正在录像回放页面中视频上墙
    ULONG m_ulIsRecordWalling;
public:
    ULONG GetRecordWallState() const { return m_ulIsRecordWalling; }
    void SetRecordWallState(ULONG val) { m_ulIsRecordWalling = val; }
private:
    // 是否正在录像回放页面中智能分析结果叠加
    ULONG m_ulIsRecordAnalysising;
public:
    ULONG GetRecordAnalysisState() const { return m_ulIsRecordAnalysising; }
    void SetRecordAnalysisState(ULONG val) { m_ulIsRecordAnalysising = val; }
private:
    // 是否正在显示录像回放工具栏
    ULONG m_ulIsRecordBarShowing;
public:
    ULONG GetRecordBarState() const { return m_ulIsRecordBarShowing; }
    void SetRecordBarState(ULONG val) { m_ulIsRecordBarShowing = val; }
private:
    // 是否正在显示智能分析工具栏
    ULONG m_ulIsAnalysisBarShowing;
public:
    void SetAnalysisBarState(ULONG val) { m_ulIsAnalysisBarShowing = val; }
    ULONG GetAnaylysisBarState() const { return m_ulIsAnalysisBarShowing; }

private:
	bool m_bSyncMode;
public:
	void SetSyncPlayBackMode(bool bSync) {  m_bSyncMode = bSync; }
	bool IsSyncPlayBack()const { return m_bSyncMode; } 

private:
	//是否轮训窗口
	bool m_bIsTurnCruise;
	//轮训状态
	ULONG m_ulTurnCruiseStyle;
	//上一次状态
	ULONG m_ulPrveTurnCruiseStyle;
	//窗口类型
	TYPE_WND m_ulTypeWnd;
	//即时回放Bar状态
	ULONG m_ulInstantReplayStyle;
	//是否展示所有Bar
	ULONG m_ulAllBarStatus;
public:
	//设置轮训窗口
	void SetTurnCruiseStatus(bool val) {m_bIsTurnCruise = val;}
	bool GetTurnCruiseStatus()const {return m_bIsTurnCruise;}
	LONG DeleteTurnCruiseStatus();
	//设置轮训状态
	void SetTurnCruiseStyle(ULONG val) {m_ulTurnCruiseStyle = val;}
	ULONG GetTurnCruiseStyle() const { return m_ulTurnCruiseStyle; }
	// 保存轮训状态
	ULONG GetPrveTurnCruiseStyle() const { return m_ulPrveTurnCruiseStyle; }
	void SetPrveTurnCruiseStyle(ULONG val) { m_ulPrveTurnCruiseStyle = val; }

	void InvalidateCruiseBar();
	//设置窗口类型
	void SetTypeWnd(TYPE_WND val) {m_ulTypeWnd = val;}
	TYPE_WND GetTypeWnd()const {return m_ulTypeWnd;}
	//设置即时回放
	void ShowInstantReplayBar(ULONG val);
	ULONG GetShowInstantStatus()const{return m_ulInstantReplayStyle;}
	//恢复实况
	void ResumeRealPlay();
	//开始录像回放
	LONG StartRecordPlay();
	//停止录像回放
	LONG StopRecordPlay();
	// 暂停录像回放
	LONG PauseRecordPlay();
	//设置所有Bar的展示状态
	void SetAllBarStatus(ULONG val){m_ulAllBarStatus = val;}
	//隐藏所有Bar
	void HideAllBar();
	//隐藏上下两个Bar
	void HideTwoBar();
private:

    // 是否正在回访页面中声音
    ULONG m_ulIsReplaySounding;
public:
    ULONG GetReplaySoundState() const { return m_ulIsReplaySounding; }
    void SetReplaySoundState(ULONG val) { m_ulIsReplaySounding = val; }
	void SetBarWindowsLong()const;
private:
    // 是否开始智能分析绘图
    bool m_bIsAnalysisDrawing;
    // 鼠标状态
    int m_ulCursorState;
public:
    int GetCursorState() const { return m_ulCursorState; }
    void SetCursorState(int val) { m_ulCursorState = val; }
private:
    //  
    CPoint m_startPoint;
    //  
    CPoint m_lastPoint;

    // 用于记录鼠标按下后是否移动过，屏蔽双击时框选
    bool m_bIsMoving;
    // 左键处理按下状态
    bool m_bIsButtonDown;
    // 初次操作
    bool m_bIsFirst;
    // 修改智能分析，可移动标志位
    bool m_bIsMovable;
    // 当前面板的窗口类型
    WND_TYPE m_ulWndType;
public:
    WND_TYPE GetWndType() const { return m_ulWndType; }
    void SetWndType(WND_TYPE val) { m_ulWndType = val; }
private:
    // 客户端类型
    IVS_CLIENT_TYPE m_ulClientType;
public:
    IVS_CLIENT_TYPE GetClientType() const { return m_ulClientType; }
    void SetClientType(IVS_CLIENT_TYPE val) { m_ulClientType = val; }
private:
    //  
    bool m_bIsLocalRecord;

	// 是否是本窗口启动了本地录像，
	// 流复用情况下，关闭窗口时 判断是否是本窗口触发的启动录像
    bool m_bLocalRecordTrigger; 
public:
    bool GetIsLocalRecord() const { return m_bIsLocalRecord; }
    void SetIsLocalRecord(bool val) { m_bIsLocalRecord = val; }
	bool GetIsLocalRecordTrigger() const { return m_bLocalRecordTrigger; }
	void SetIsLocalRecordTrigger(bool b) { m_bLocalRecordTrigger = b; }

private:
    //  
    bool m_bIsPlatRecord;
public:
    bool GetIsPlatRecord() const { return m_bIsPlatRecord; }
    void SetIsPlatRecord(bool val) { m_bIsPlatRecord = val; }
private:
    //  
    bool m_bIsPuRecord;
public:
    bool GetIsPuRecord() const { return m_bIsPuRecord; }
    void SetIsPuRecord(bool val) { m_bIsPuRecord = val; }
private:
    //
    TCHAR m_szCameraID[IVS_DEV_CODE_LEN + 1];
public:
    const TCHAR* GetCameraID() const{ return m_szCameraID; }
	void SetCameraID( const TCHAR* szCameraID );

private:
    // 是否显示重连标志
    bool m_bIsResume;

    //是否显示上墙背景字幕标志
    bool m_bIsTvWallBg;

	//是否电视墙
	bool m_bIsTvWall;
public:
    bool GetResumeState() const { return m_bIsResume; }
    void SetResumeState(bool val) { m_bIsResume = val; }
public:
    bool GetTvWallBgState() const { return m_bIsTvWallBg; }
    void SetTvWallBgState(bool val) { m_bIsTvWallBg = val; }
public:
	bool GetTvWallState() const { return m_bIsTvWall; }
	void SetTvWallState(bool val) { m_bIsTvWall = val; }

private:
    // 是否改变窗格布局
    ULONG m_ulIsWndLayoutChange;
public:
    ULONG GetIsWndLayoutChange() const { return m_ulIsWndLayoutChange; }
    void SetIsWndLayoutChange(ULONG val) { m_ulIsWndLayoutChange = val; }
private:
    // 摄像机类型
    ULONG m_ulCameraType;
public:
    ULONG GetCameraType() const { return m_ulCameraType; }
    void SetCameraType(ULONG val) { m_ulCameraType = val; }
private:
    // 是否在轮巡
    TCHAR m_szToolbarStatus[33];
    // 是否在轮巡
    bool m_bIsTurning;
    // 上一段录像中声音状态
    bool m_bLastSound;
public:
    void SetLastSoundState(bool val) { m_bLastSound = val; }
private:
    //  
    bool m_bStartPlayState;
public:
    bool GetStartPlayState() const { return m_bStartPlayState; }
    void SetStartPlayState(bool val) { m_bStartPlayState = val; }
private:
    //  
    ULONG m_ulPlayHandle;
public:
    ULONG GetPlayHandle() const { return m_ulPlayHandle; }
    void SetPlayHandle(ULONG val) { m_ulPlayHandle = val; }

public:
    void SetServiceType(IVS_SERVICE_TYPE enServiceType){m_enServiceType = enServiceType;}
    IVS_SERVICE_TYPE GetServiceType()const{ return m_enServiceType;}
private:
    IVS_SERVICE_TYPE m_enServiceType;
public:
	void PTZ3DMouseUp( const CRect &mainRect);
	void SetBackGroundBitmap(const CRect &rect);
	void InvalidateBar();
	void PTZ3DMouseDown( const CPoint &point );
	BOOL PointIsVideoRect( const CPoint &point );
	void GetDisplayScaleRect( CRect &mainRect );

	virtual BOOL OnInitDialog();
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnPaint();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg LRESULT OnSyncPlayBackMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnResumeRealPlayOKMsg(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnAsyncStreamMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnAsyncEventMsg(WPARAM wParam,LPARAM lParam);
	
public:
    bool IsAnalysisDrawing() const { return m_bIsAnalysisDrawing; }
    void IsAnalysisDrawing(bool val) { m_bIsAnalysisDrawing = val; }

public:
    // 判断鼠标指针是否在屏幕中心
	bool IsInCenter( const CPoint &point )const;
    void HideVideo(void);
	BOOL IsPtInWnd( const CWnd* pWnd, const CPoint &point )const;
	bool IsPtNotInVideoWnd( const CPoint &point )const;
	bool MoveEagleEye()const;
	void MoveVideo();
	void MoveVideoPane();

	//电视墙多进一出，将一个窗格拆分为4个子窗口,用虚线分割
	//void SplitTVWallPaneToFour();
	//显示电视墙多进一出 分割线
	//void ShowTVWallSplitLine();

    HWND GetVideoWnd(void) const{ return m_Video.GetSafeHwnd(); }
    // 设置工具栏状态
	void SetToolbarEnableStatus( ULONGLONG dwToolbarBtn, ULONGLONG lButtonStatus );
	void SetToolBarBtnVisableStatus(ULONGLONG dwToolbarBtn);
	ULONGLONG GetToolBarBtnVisableStatus(void)const;
	void ReSetToolBarStatus();
    // 设置为活动窗体
    void SetActivePane(void);
    // 边框设置与复位
    void SetBorder(ULONG ulBorderState);

    void SetActiveBorder();

    //设置警告的状态
    ULONG SetWarnState(ULONG ulWarnState,ULONG ulWarnSeconds);
    // 设置云台控制鼠标图片
	void SetPtzCursor( const CPoint &point )const;
    // 设置云台控制动作
	void SetPtzControl(const CPoint &point, const TCHAR *pCameraCode, const TCHAR *pMode, const TCHAR *pSpeed);
    void SetPtzControl(const CPoint &point);
    //
    void OnRealtimeLButtonUp( const CPoint &point);
    // 设置实况工具栏中按钮状态
    void SetToolBarBtnState(BOOL bState)const;
    // 设置标题栏按钮状态图片
    void SetTitleButtonPic(bool bState)const;
    // 设置实况工具栏中播放状态图片
    void SetRealtimePlayPic(ULONG ulState)const;

    void ResetPane();

    ULONG SetPartMagnifyState( ULONG ulState );

	void OnRealtimePaneMouseMove( const CPoint &point, const CRect &mainRect );

    void ResetBottomButton();

	//设置水印状态
	void SetWaterMarkStatus(ULONG ulStatus);
private:
    // 主视频窗口
    CVideoStatic m_Video;
	CVideoStatic m_EagleEye;
    CRect m_TitleBarRect;
    CRect m_ToolBarRect;
	CRect m_TVWallStatusBarRect;//电视墙通道状态图矩形
	CRect m_TurnCruisePaneRect;
	CRect m_DigtalRect;
	CRect m_pZoomDirctRect;
	CRect m_ZoomBarRect;
	CRect m_ZoomTitleBarRect;
	CRect m_TimerBarRect;
public:
    void ResetBarPosition(void);
    void SetBarRect();

	void DeleteBar();
	CPoint GetInfoButtonRightBottomPosition()const;

    CIVS_OCXPlayerCtrl *GetOCXPlayerCtrl();
    CVideoPaneMgr *GetVideoPaneMgr();

    bool IsExistPlayHandle(unsigned long ulPlayHandle)const;

/************************************************************************/
/* draw  begin  add by zhangshufan00227377                              */
/************************************************************************/
private:
	std::string m_strDevName;

public:
	void		 SetDevName(const std::string &strDevName) {m_strDevName = strDevName; }
	std::string& GetDevName() {return m_strDevName; }//lint !e1536
	ULONG GetDrawHandle() const {return m_ulDrawHandle; }
private:

	ULONG			m_ulCurrentGraphicType;		// 工具条选中的图形类型
	ULONG			m_ulCurrentGraphicGroup;	// 当前传入的图形组
	ULONG			m_ulGraphicState;		// 图形状态（画图、修改）
	bool			m_bIsDrawing;			// 是否正在画图，未完成
	IA_GRAPHICS		m_CurrentGraphic;		// 存储CU设置进来的开始画图的属性
	ULONG			m_ulGroupDrawMaxNum;	// 该组图形允许画最大图形个数
    bool            m_bDrawEnabled;
	bool            m_bOverlayEnabled;
	ULONG			m_ulDrawHandle;			// 智能分析画图句柄
	ULONG			m_ulCursorType;

	void DrawLButtonDown(const CPoint &point);
	void DrawLButtonUp(const CPoint &point) ;
	void DrawMouseMove(const CPoint &point) ;
	void DrawRButtonDown(const CPoint &point);
	void DrawRButtonUp(const CPoint &point);

public:
	// 开始画图设置图形属性
	LONG SetCurrentGraphicAttribute(const IA_GRAPHICS *pGraphicAttribute, ULONG ulGroupDrawMaxNum);
	
	// 设置当前图形类型
	VOID SetCurrentGraphicType(ULONG ulGraphicType);

	// 设置当前图形组编号
	VOID SetCurrentGraphicGroup(ULONG ulGraphicGroup);

	// 设置当前绘图状态
	VOID SetCurrentGraphicState(ULONG ulGraphicState);

	// 获取当前绘图状态
	ULONG GetCurrentGraphicState()const;

	// 判断是否允许删除
	BOOL CheckAllowDelete();
	
	// 设置删除控件状态
	VOID SetDeleteButtonState(BOOL bAllowDelete);

	// 开启智能分析画图
	IVS_INT32 EnableDraw(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);

	// 关闭智能分析画图
	IVS_INT32 DisableDraw(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);

	// 设置图形
	IVS_INT32 SetGraphics(const IA_GRAPHICS *pGraphics, ULONG ulNum, BOOL bPersent);

	//设置智能分析图形组之间的关系
	IVS_INT32 SetRelationShip(ULONG ulGraphicGroupA, ULONG ulGraphicGroupB, RELATION_SHIP_TYPE ulRelationShipType);

	// 获取图形
	IVS_INT32 GetGraphics(IA_GRAPHICS *pGraphics, ULONG *pulNum);
	
	// 获取选中的智能分析图形
	IVS_INT32 GetGraphicSelected(IA_GRAPHICS *pGraphic);

	// 按组获取智能分析图形
	IVS_INT32 GetGraphicsByGroup(ULONG ulGraphicGroup, IA_GRAPHICS *pGraphics, ULONG *pulNum);

	// 删除选中图形
	IVS_INT32 DeleteGraphicsSelected();

	// 删除指定图形组的所有图形
	IVS_INT32 DeleteGraphicsByGroup(ULONG ulGraphicGroup);

	// 删除所有图形
	IVS_INT32 DeleteGraphics();

	//智能分析结束某图形组的画图（若存在正在画的该组图形，直接丢弃）
	IVS_INT32 StopDrawGraphics(ULONG ulGraphicGroup);
	IVS_INT32 StopDrawGraphicWhileDraw(ULONG ulGraphicGroup);

	//选中指定图形组的所有图形，准备修改
	IVS_INT32 SelectGraphicByGroup(ULONG ulGraphicGroup);

	// 按点选中图形
	IVS_INT32 SelectGraphicByPoint(const POINT &CurrentPoint);

	//取消选中智能分析图形
	IVS_INT32 SelectCancel();
    
    //取消选中智能分析图形
    IVS_INT32 SelectCancelWhileDraw();

	// 获取当前选中图形ID
	ULONG GetCurrentGraphicGroup()const{ return m_ulCurrentGraphicGroup; }
	
	VOID GetCursorType(const POINT &Point);
	VOID SetCursorType()const;

private:

	ULONG	m_ulOverlayHandle;		// 智能分析轨迹叠加句柄
	ULONG	m_ulRequestID;			// 轨迹请求ID

public:

	// 开启智能分析轨迹叠加
	IVS_INT32 EnableOverlay(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, ULONG ulRuleType);

	// 关闭智能分析轨迹叠加
	IVS_INT32 DisableOverlay(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle);
	
	// 智能分析轨迹叠加插入轨迹消息
	IVS_INT32 InsertIALayoutMessage(const IA_LAYOUT_MESSAGE &LayoutMessage);

	// 获取当前轨迹信息数量
	IVS_INT32 GetCurrentElementNum();


/************************************************************************/
/*  draw end                                                            */
/************************************************************************/
	
/************************************************************************/
/* Capture  begin  add by zhangshufan00227377                           */
/************************************************************************/
public:
	IVS_INT32 LocalSnapshot(IVS_INT32 iSessionID, IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam);
	IVS_INT32 LocalSnapshotToDir(IVS_INT32 iSessionID, IVS_UINT32 uiPictureFormat, const IVS_CHAR *pFileName);
	IVS_VOID StopSerialSnapshot();

	//设置录像视频质量
	IVS_INT32 SetVideoEffect(IVS_INT32 iSessionID, IVS_LONG lBrightValue,
		IVS_LONG lContrastValue, IVS_LONG lSaturationValue,
		IVS_LONG lHueValue);

private:
	IVS_INT32 CheckLocalCaptureParam(const IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam)const;
	IVS_INT32 GetTime(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam)const;
	IVS_INT32 BuildFileName(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam);
	IVS_INT32 BuildFullPath(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam);
	IVS_INT32 SingleCapture(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam);
	IVS_INT32 StartSerialCapture(IVS_INT32 iSessionID, const IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam);
	IVS_INT32 CaptureRetToCU(IVS_INT32 iResult, IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam);
	static IVS_INT32 ReachTimeInterval(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, const IVS_LOCAL_CAPTURE_PARAM &LocalCaptureParam, BOOL &bReachTimeInterval);
	static IVS_UINT32 __stdcall CaptureThreadProc(void *pParameter);
	
	IVS_LOCAL_CAPTURE_PARAM m_LocalCaptureParam;
	HANDLE					m_CaptureThreadHandle;
	bool					m_bCaptureThreadRun;
	IVS_INT32				m_iSessionID;

/************************************************************************/
/*  Capture end				                                            */
/************************************************************************/

	HBITMAP m_hPaneBKGBitmap;
    HBITMAP m_hPaneALARMBitmap;
public:
	void EnableALLButton(BOOL isEnable);

/*
 * add by w00210470
 * 播放实况情况下保存摄像头信息，进行流复用检测
 */
public:
	bool VerifySameStream(const char *szCamID, unsigned int type)const;
	void SetRealStreamInfo(const char *szCamID, unsigned int type);

    //缩放图片的同时要保持比例(图片比例为4:3）
    int m_nZoomWidth;
    int m_nZoomHeigth;

    void SetAnalysisBarIsStartPlay(BOOL IsStartPlay);
	void HideTranslucenWnd(void);
private:
	char         m_CamReal[IVS_DEV_CODE_LEN + 1];
	unsigned int m_uiStreamType;
public:
    const char *GetCamReal() const{ return m_CamReal; }


/* 
 * 实况异步启动停止
 */
public:
     
	LONG StartRealSteamAsync(IVS_REALPLAY_PARAM &realplayParam, LPCTSTR pCameraID, 
		BOOL bLayoutChange, ULONG ulStreamOpenMode, unsigned long tranid);

	int StopRealStreamAsync();
	int StopRealStreamSync();
	void StopRealStream();

	static unsigned int __stdcall BegRealPlayProc(LPVOID lpParameter);

private:

	enum MSG_TYPE {
		ASY_MSG_NONE          = 0,
		ASY_MSG_STOP_SUCCESS  = 1,
		ASY_MSG_START_SUCCESS = 2,
		ASY_MSG_START_FAIL    = 3
	};

    enum THREAD_STATE {
		THREAD_RUNNING = 0,
		THREAD_EXITING = 1
	};

	class  REALPLAY_CMD {
	public:
		unsigned long transid;
		bool bStop;
		char CamID[IVS_DEV_CODE_LEN + 1];
		IVS_REALPLAY_PARAM Param;

		REALPLAY_CMD()
		{
			transid = 0;
			bStop  = 0;
			CamID[0] = '\0';
			memset(&Param, 0, sizeof(Param));
		}

		REALPLAY_CMD (const REALPLAY_CMD& rhs)
		{
			transid = rhs.transid;
			bStop   = rhs.bStop;
			memcpy(CamID, rhs.CamID, sizeof(CamID));
			memcpy(&Param, &rhs.Param, sizeof(Param));
		}
	};

	typedef std::list<REALPLAY_CMD> REALPLY_CMD_LIST;

	CIVSMutex        m_CmdLock;
	unsigned int     m_uCmdSeq;
	REALPLY_CMD_LIST m_CmdList;
    HANDLE  m_hWorkThread;
	int     m_ThreadState;

public:
	//virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);//lint !e1768
	BOOL TransStrToTime(const CString &str,ST_TIME* pTime)const;
	void SetInstansPlayStatus(BOOL val){m_bIsInstanstPlayBack = val;}
	BOOL GetInstansPlayStatus()const{return m_bIsInstanstPlayBack;}
	void ShowInstantBar();
private:
	unsigned long long m_ullWinAlarmFlag;//是否对告警按钮置灰
public:
	void SetAlarmButtonStatus(unsigned long long istatus);
	//设置书签按钮状态
	void SetBookmarkButtonStatus(unsigned long long istatus);
    void SetStartPlayTime();
	void SetSkin();
	void SetTitleBarStatus(ULONG ulStatusType, ULONG ulShow);
	void SetTitleBarInitStatus();
    ULONG GetTVWallStatus()const;
private:
	BOOL m_bIsMoveVideoWindow;
	BOOL m_bIsHideTranslucentBar;
	bool m_bIsBarCreated;
	bool m_bIsRefresheed;
	ULONG m_ulTVBarStatus;

	ULONG m_ulShowCruiseTrackStatus;
	ULONG m_ulShowAlarmStatus;
	ULONG m_ulShowRecordStatus;
	ULONG m_ulShowSingleFrameStatus;
};
