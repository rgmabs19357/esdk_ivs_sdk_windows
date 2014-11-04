/*****************************************************************
 filename    :    VideoPaneMgr.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    窗体管理类
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#include "Lock.h"
#include "common_struct.h"
#include "VideoPane.h"
#include "ParentWnd.h"
#include "OCXTVWallXMLProcess.h"

/*lint –elib(129) */

// using namespace std;

#define INTERNATIONAL_WORD_LEN   (100) // 国际化单词长度
const double RATIO_PTZ_GRID = 0.35;
const LONG INTERNATIONAL_WORD_NUM = 27;   // 国际化单词个数

class CVideoPaneMgr
{
public:
    CVideoPaneMgr(void);
    ~CVideoPaneMgr(void);
private:

private:
    // 主窗口句柄，用于创建窗格时设定其父窗口
    CWnd *m_pParentWnd;
    COleControl* m_pOcxCtrl;       //Ctrl的指针
public:
    const COleControl* GetOcxCtrl() const { return m_pOcxCtrl; }
    ULONG SetOcxCtrl(COleControl* val);
	CWnd *GetParentWnd(){return m_pParentWnd;}
private:    // 窗口类型
    WND_TYPE m_ulWndType ;
public:
    ENUMDEF::WND_TYPE GetWndType() const { return m_ulWndType; } 
    void SetWndType(ENUMDEF::WND_TYPE val){ m_ulWndType = val; }  
    bool isAnalysisWndType()const;
private:
	IVS_WIND_LAYOUT m_windLayOut;
	IVS_ULONG m_ulTVWallID;			//当前关联的电视墙编号
public:
	//返回当前关联的电视墙ID
	IVS_ULONG GetCurTVWallID()const {return m_ulTVWallID;}
public:
	IVS_WIND_LAYOUT GetWindLayOut() const { return m_windLayOut; }
	void SetWindLayOut(IVS_WIND_LAYOUT val) { m_windLayOut = val; }
private:

private:
    // 显示的窗格个数
    ULONG m_ulShowWndNum;
public:
	ULONG GetNumOfDisplayPane() const { return m_ulShowWndNum; }
	void SetNumOfDisplayPane(ULONG val) { m_ulShowWndNum = val; }
public:
    // 是否放大
    LONG m_ulIsMagnify;

    //窗格map表
    MAP_VIDEOPANE m_videoPaneMap;
    //窗格map表的锁
    CIVSMutex m_csVideoPaneLock;

	ULONG SetWarnState( ULONG ulWndID, ULONG ulWarnState, ULONG ulWarnSeconds );

    //底部工具栏按钮权限
    TCHAR  m_strToolbarStatus[33];

public:
	ULONG GetToolbarBtnNum() const { return m_uToolbarBtnNum; }

    // 获取放大状态
    LONG GetMagnifyState() const { return m_ulIsMagnify; }
    // 获取全屏状态
    ULONG GetFullScreenState() const { return m_ulIsFullScreen; }
	void SetFullScreenState(ULONG ulFullScreen) { m_ulIsFullScreen = ulFullScreen; }
    void SetColor(ENUMDEF::COLOR_MODE ulColor);
private:
    // 当前活动窗格ID，默认为1,范围是1-64
    ULONG m_ulActivePaneID;
public:
	ULONG GetActivePaneID() const { return m_ulActivePaneID; }
	//设置活动窗体ID
	void SetActivePaneID(ULONG val) { m_ulActivePaneID = val; }
private:
    // 当前操作的窗格ID
    ULONG m_ulOperatePaneID;
    // 客户端类型
    IVS_CLIENT_TYPE m_ulClientType;
public:
    // 获得客户端类型
    IVS_CLIENT_TYPE GetClientType() const { return m_ulClientType; }
    // 设置客户端类型
    LONG SetClientType(IVS_CLIENT_TYPE ulClientType);
private:
    //是否全屏;
    ULONG m_ulIsFullScreen;
    // 是否显示标题工具栏
    bool m_IsShowTitleBar;
	bool m_isShowToolBar;
	bool m_isChangeToolBar;	//设置要显示的工具栏是否有变化
public:
	// 是否显示工具具栏
	ULONG SetToolbar(ULONG ulToolBar);
    //设置智能分析的toolbar
    ULONG SetIAToolbar( ULONG ulToolBar );
    // 获取标题栏显示状态
    bool GetTitleBarShowState() const { return m_IsShowTitleBar; }
	bool GetToolBarShowState() const { return m_isShowToolBar; }
	bool GetShowToolBar() const { return (m_isShowToolBar && ((0 != m_uToolbarBtnNum )|| (0 != m_uAnalysisBtnNum))); }
	void SetShowToolBar(bool val) { m_isShowToolBar = val; }
private:
    // 工具栏显示状态
    ULONG m_uToolbarBtnNum;
    ULONG m_uAnalysisBtnNum;
    // 显示比例
    ULONG m_ulDispalyAspectRatio;
private:
    // 是否自动变换布局
    LONG m_lAutoChangeLayout;
	// 保存被放大时的布局，避免缩放等操作导致退出
	IVS_WIND_LAYOUT m_MagnifyLayOut;
	// 1_1布局的pane指针，用于单窗格无法操作的保存
	CVideoPane* m_pVideoPane_1_1;
	//是否允许窗格交换
	ULONG ulEnableExchangePaneStatus;
	ULONG m_ulParentHanlde;
	ULONG m_ulOldParentHanlde;
	//当前皮肤类型
	ULONG m_ulSkinType;
public:
    LONG GetAutoChangeLayout() const { return m_lAutoChangeLayout; }
    void SetAutoChangeLayout(LONG val) { m_lAutoChangeLayout = val; }

	//设置皮肤类型
	ULONG GetParentHwnd()const {return m_ulParentHanlde;}
	void SetSkin(ULONG val);
	ULONG GetSkinType()const{return m_ulSkinType;}
public:
    LONG Init(void);
    //  放大或缩小，参数为是否正在播放
	void ChangeSize( ULONG ulIsPlaying );
    // 缩小窗格
    void Minify(void);
    // 放大窗格
    void Magnify(CRect& rect);
    // 检查是否可以交换窗格

    ULONG CheckIfCanExchange(const CVideoPane * pFirstPane) const;

    // 删除所有SoundCtrl和Talkback对话框删除Replay声音对话框
    void DeleteSoundCtrl(void);
    // 删除SoundCtrl、Talkback对话框, 不对活动窗格处理删除Replay声音对话框
    void DeleteSoundCtrlForOther(void);
    //  交换窗格，参数为捕捉到松开鼠标左键的窗格

    void ExchangePane(ULONG ulMouseUpPaneID);

    // 获取活动窗格指针
    CVideoPane* GetActiveWndPointer(void);
    // 获取所有窗格

    ULONG GetAllVideoPane(MAP_VIDEOPANE & videoPaneMap) const;

    // 根据句柄获取窗口ID
	ULONG GetWindIDByHWND( const HWND &ulWnd );
    // 根据鼠标坐标获取窗口ID
	ULONG GetWndIDByMouse( void );
    // 获取空闲的窗口(非告警窗口)
	ULONG GetWndIDFree( void );
    // 获取指定窗格指针
    CVideoPane* GetWndPointerByHandle(long lHandle);
    // 获取指定窗格指针
	CVideoPane* GetWndPointerByHWND( const HWND hWnd );
    // 获取指定窗格指针
	CVideoPane* GetWndPointerByID(ULONG ulWndID);
	// 根据设备编码获取指定窗格指针
	CVideoPane* GetWndPointerByCameraCode(const std::string& strCameraCode);
	// 获取局部放大窗口
	ULONG GetMagnifyFreeWnd(ULONG ulPaneID);
	// 获取哪个窗口开启了局部放大
	bool GetMagnifyOpenWnd(ULONG& ulWndID);
    void HideVideoPane(void);
	// 取消所有窗口的局部放大状态
	void CancleAllMagnifyWnd();
	void SetMagnifyLayOut(IVS_WIND_LAYOUT layout){ m_MagnifyLayOut = layout; }
	IVS_WIND_LAYOUT GetMagnifyLayOut()const{ return m_MagnifyLayOut; }
    // 判断是否有语音对讲
    BOOL IsTalkback(void);
    // 隐藏自适应对话框
    void OnCurrentLocker(const CPoint &point)const;
    void OnHideAllLocker(void);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    void OnLButtonDown( UINT nFlags, CPoint point );
    BOOL OnMouseWheel(UINT nFlags, short zDelta, const CPoint& pt);
    void RefreshAllWindow(void)const;
    // 恢复全屏
    void ResetFullScreen(void);
    // 复位窗格信息

    ULONG Set3DPTZState(ULONG ulWndID, ULONG ulIsStart3D);

    // 由窗格调用，用于设置选定的边框颜色
	void SetActivatePane( ULONG PaneID );
	ULONG SetAddAnalysisResult(ULONG ulWinID, bool bState);
	ULONG SetAlarmState(ULONG ulWndID, bool bIsOpen);

    // 设置按钮状态（按钮置灰）
	void SetButtonStatusOff(ULONG ulWinID, LONG ulButtonType);
    // 设置按钮状态（按钮点亮）
	void SetButtonStatusOn( ULONG ulWinID, LONG ulButtonType);
	ULONG SetCaptureState( ULONG ulWndID, bool bIsOpen );
    // 设置设备名称

    ULONG SetDevName(ULONG ulWndID, LPCSTR pDevName);
	IVS_INT32 SetDisplayScale(IVS_INT32 iSessionID, ULONG ulDisplayScale);
	ULONG     GetDispalyScale() const {return m_ulDisplayScale;}

    void SetFullScreen(void);
    // 设置语言
    LONG SetLanguage(LPCTSTR pLanguage);

    //可以平均分配的布局（1,4,9等）

    void SetLayoutAsAverage( const CRect &rect, ULONG ulLayOutNum ) ;
    ULONG SetManifyWindow(ULONG ulWndID, ULONG ulManifyState);
    ULONG SetMicVolume(ULONG ulVolumeValue) const;
	ULONG GetMicVolume(void) const;

    // 更新布局前重新计算或记录下一些参数
	ULONG SetParameter(ULONG ulLayoutType);
	ULONG SetRecordState(ULONG ulWndID, bool bIsOpen);
	ULONG SetSoundState( ULONG ulWndID, bool bIsOpen );
	ULONG SetSoundStateToFalse(ULONG ulWndID);
    // 设置工具栏权限
    LONG SetToolbarEnableStatus( ULONG ulWndID ,ULONGLONG dwToolbarBtn, ULONGLONG lButtonStatus );
    // 根据传入的位置数据布局视频播放窗格与标题栏或者画矩形
	void SetVideoPanePosition(CRect& rect) ;
    // 设置语音对讲音量
    LONG SetVoiceTalkVolume(LONG nCoefficient) const;
    // 调节随路音频音量
    LONG SetVoiceVolume(LONG ulVolumeValue) const;
    // 设置警告状态
	ULONG SetWarnStateSetWarnState( ULONG ulWndID, ULONG ulWarnState, ULONG ulWarnSeconds );
    // 调节随路音频音量

    ULONG SetWaveAudio(ULONG ulWndID, ULONG ulVolumeValue);

    // 设置窗格布局，输入参数为位置数据与布局类型
	LONG SetWndLayout(CRect rect, ULONG ulLayoutType);
	// 根据电视墙编号设置布局
	LONG SetTVWallWndLayout(const CRect& rect, IVS_ULONG ulTVWallID);
    // 设置窗格自定义布局，输入参数为位置数据与布局类型
	LONG SetCustomLayout( CRect rect, RECTLIST &videoRects, const IVS_UINT32 uX,const IVS_UINT32 uY );
    // 设置窗格阶梯式布局, 

    ULONG SetLadderLayout(CRect rect,ULONG ulPaneNUm );
    // 显示/隐藏标题工具栏
	void ShowTitleBar(  bool isShow );
	// 显示/隐藏工具栏
	void ShowToolBar( bool isShow);

    ULONG StartVoiceTalkback(const TCHAR* pVoiceCode, ULONG ulDecoderType, ULONG ulStreamType);
    ULONG StopAllRealPlay(void);
    ULONG StopAllSound(ENUMDEF::WND_TYPE val);

    // 关闭随路语音
    void StopSoundCtrlForOther(bool bCloseAll);
    // 关闭语音对讲,不对活动窗格处理
    void StopTalkbackSoundForOther(void);

    ULONG StopVoiceTalkback(void) const;

	//设置轮训状态
	ULONG SetTurnCruiseStyle(ULONG ulWndID,ULONG ulStyle=0);

	//隐藏所有透明窗体
	void HideAllTranslucenWnd(void);
private:

    // 全屏前的屏幕大小
    WINDOWPLACEMENT m_struOldWndpl;
	ULONG           m_ulDisplayScale;

public:
    //颜色配置参数
    COLORREF m_VideoPaneBKGColor;
    COLORREF m_ActiveEdgeColor;
    COLORREF m_NotActiveEdgeColor;
    COLORREF m_TitleBarBKGColor;
    COLORREF m_BottomBarBKGColor;
    COLORREF m_RectangleBKGColor;
    COLORREF m_TextColor;
    LONG m_ulShadeStartRGB;
    LONG m_ulActiveEdgeRed;
    LONG m_ulActiveEdgeGreen;
    LONG m_ulActiveEdgeBlue;

    void ResetBarPosition(void);

	bool isPaneExistByID(ULONG ulPaneId);

private:
	ULONGLONG  m_ullToolBarBtnVisableStatus;
public:
	ULONGLONG GetToolBarVisableStatus() const { return m_ullToolBarBtnVisableStatus; }
	void SetToolBarVisableStatus(ULONGLONG val);

    void HideAllBarOnce();
	void SetAllBarWindowsLong(ULONG ulHandle);
    IVS_INT32 GetHwndbyPlayHandle(unsigned long ulPlayHandle, HWND& hWnd);
    bool IsLegalityWndID( ULONG lWndID )const;
public:
	/******************************************************************
	function : SetTVWallPaneChannelStatus
	description: 设置电视墙窗格通道状态
	input : ULONG ulWndID
	input : ULONG ulStatus
	output : NA
	return : void
	*******************************************************************/
	void SetTVWallPaneChannelStatus(ULONG ulWndID,ULONG ulStatus);

	/******************************************************************
	function : SetTVWallPaneStatus
	description: 设置电视墙窗格状态
	input : ULONG ulWndID
	input : ULONG ulPaneStatus
	output : NA
	return : void
	*******************************************************************/
	void SetTVWallPaneStatus(ULONG ulWndID,ULONG ulPaneStatus);

	void StartSerialCaptureAll();
private:
	WND_TYPE m_TVWallWndType;
public:
	void SetTVWallWndType(WND_TYPE wndType) {m_TVWallWndType = wndType;}
	WND_TYPE GetTVWallWndType()const {return m_TVWallWndType;}
	void ResetTVWallStatus();
	void DoButtonFilter(LONG& ulWndID);
public:
	typedef enum{
		SPLIT_1 = 1,
		SPLIT_4,
		SPLIT_9,
		SPLIT_16
	}SPLIT_WND_TYPE;
	typedef std::map<ULONG,CRect> PaneIDRectMap;//需要分裂的窗格映射表
	PaneIDRectMap m_MapID1Rect;
	PaneIDRectMap m_MapID4Rect;
	PaneIDRectMap m_MapID9Rect;
	PaneIDRectMap m_MapID16Rect;
	RECTLIST m_SplitTo1RectList;//拆分为1个窗格坐标列表
	RECTLIST m_SplitTo4RectList;//拆分为4个窗格坐标列表
	RECTLIST m_SplitTo9RectList;//拆分为9个窗格的坐标列表
	RECTLIST m_SplitTo16RectList;//拆分为16个窗格的坐标列表
	typedef std::map<ULONG,TVWall_VPANEID> WNDID_MAP;//窗格对应关系表
	WNDID_MAP m_MapWindowPaneID;
	LONG SplitTVWallWnd();//拆分窗格
	void AddSplitRectMap(const SPLIT_WND_TYPE& emType,const CRect &rc);//增加拆分窗口的ID到映射表
	bool ResetVideoPanePositon(const ULONG& ulPaneID,const CRect& rc);
	void GetRealTVwallPaneID(const TVWall_VPANEID_LIST& listPaneID,std::vector<ULONG>& vecRealPaneID);//根据虚拟窗格ID列表获取真实的窗格列表
	void GetRealTVwallPaneIDByParentID(const std::vector<ULONG>& vecPaneID,std::vector<ULONG>& vecRealPaneID);//根据父窗格ID获取所有的真实窗格
	bool GetVirtualIDByRealID(const ULONG& ulPaneID,TVWall_VPANEID& vID);//根据真实的窗格ID获取虚拟的窗格id
	ULONG GetRealIDByVirtualID(const TVWall_VPANEID& stPaneID);//根据虚拟的窗格ID获取真实的窗格id
	void UpdateTVWallPaneStatus();//刷新电视墙窗格一些子控件的显示，删除未绑定解码器的电视墙图标
	void SetALLMagnifyBtnStatus(const CVideoPane* pVedioPane,ULONGLONG lButtonStatus);//// 禁用除当前窗口外的所有其他窗口
	void EnableExchangePane(ULONG ulStatus){ulEnableExchangePaneStatus = ulStatus;} //是否允许窗格交换
	ULONG MoveOCXToRect(CRect& rect);
	ULONG FullScreenKeyUp(ULONG ulMsgKey);
    void NotifyOtherSoundOpen(ULONG ulWinID, bool isOpen);
    bool IsSoundStart(const CString& strCameraCode);
	void HideAllTitileBar();
	void HideAllToolBar();
	void HideShellTrayWnd(BOOL bShow)const;
    void UpdateTVWallOnlineStatus();
    ULONG GetTVWallPaneStatus(ULONG ulWndID);
	void DragMoveOutofOCX();
};

