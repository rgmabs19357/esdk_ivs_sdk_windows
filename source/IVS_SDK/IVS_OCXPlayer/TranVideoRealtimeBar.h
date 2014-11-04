/*****************************************************************
 filename    :    TranVideoRealtimeBar.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    实况，录像工具栏
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#include <bitset>
#include "TranslucentDialog.h"
#include "translucentbutton.h"
#include "VideoSoundCtrl.h"
#include "IVSMenu.h"
#include "afxwin.h"

// CTranslucentVideoRealtimeBar dialog
//  MFC Dialog 无需默认构造函数

const ULONG SOUNDCTRL_TIMESPAN_LEN = 10;
const ULONG SOUNDCTRL_SLIDER_INTERVAL = 10; // 滑动条间隔
const ULONG SOUNDCTRL_SLIDER_LEN = 100;     // 滑动条长度
const ULONG SOUNDCTRL_SLIDER_HEIGHT = 145;  // 滑动条高度
const ULONG SOUNDCTRL_WIDTH = 30;           // 声音对话框的宽度
#define  VOLUME_FULL                 0xFFFFFFFF

#define WM_SETSOUND_STATE (WM_USER + 1314)

class CTranVideoRealtimeBar : public CTranslucentDialog
{
    DECLARE_DYNAMIC(CTranVideoRealtimeBar)

public:
    virtual ~CTranVideoRealtimeBar();
	CTranVideoRealtimeBar(Gdiplus::Image* pImage, CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_DLG_TVIDEOREALTIMEBAR};

    virtual void OnInitChildrenWnds();

	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
	CIVSMenu* m_pMenu;
	
    std::vector<CTranslucentButton *> m_btnVector;
#pragma region BTN
    CTranslucentButton m_buttonLocker;
	//抓拍
    CTranslucentButton m_buttonSnapShot;
	//本地录像
    CTranslucentButton m_buttonLocalRecord;
	//书签
    CTranslucentButton m_buttonBookMark;
	//局部放大
    CTranslucentButton m_buttonMagnify;
	//即时回放
	CTranslucentButton m_buttonInstantBackPlay;
	//声音
    CTranslucentButton m_buttonPlaySound;
	//对讲
    CTranslucentButton m_buttonTalkback;
	//视频上墙
    CTranslucentButton m_buttonTVWall;
	//设置告警窗口
	CTranslucentButton m_buttonSetWardWnd;
	//云镜控制
	CTranslucentButton m_buttonPtz;
	//叠加智能分析
	CTranslucentButton m_buttonSuperIntel;
	//打开电子地图
	CTranslucentButton m_buttonEMap;
	//一主多辅
	CTranslucentButton m_buttonMasterMAuxiliary;
	//网速优先/画质优先
	CTranslucentButton m_buttonNetQuality;
    //开启3D云台
    CTranslucentButton m_BtnPtz3D;
	//返回实况按钮
	CTranslucentButton m_btnReturn;
	//开始/暂停录像按钮
	CTranslucentButton m_btnStart;
#pragma endregion BTN

	// 是否扩展显示按钮
	bool m_isExtend;
	//CMenu
    IVS_INT32 m_iNumBtn;
    //实况页面中声音控制
    CVideoSoundCtrl * m_pVideoSoundCtrl;
	ULONG   m_ulSoundCtrlValue;          //记录声音的值
	BOOL    m_bSoundCtrlState;           //声音控制状态 
	BOOL    m_bIsInstansPlay;            //如果是即时回放需要加两按钮
	BOOL    m_bIsStartPlay;              //如果已经启动回放
public:
    BOOL GetSoundCtrlState() const { return m_bSoundCtrlState; }
    void SetSoundCtrlState(BOOL val) { m_bSoundCtrlState = val; }
	void SetInstansPlayBack(BOOL val) {m_bIsInstansPlay = val;}
	void SetIsStartPlay(BOOL val);
	void SetRecordPlayStop(BOOL val);
private:

    BOOL    m_bSoundState;               //声音状态 是否静音 
	bool    m_bRecordStarted;
    bool    m_bPtz3DStarted;             //是否开启3D

    CVideoSoundCtrl * m_pVideoTalkbackSound; //实况页面中语音对讲
    ULONG   m_ulTalkbackSoundValue;          //记录声音的值
	bool	m_bShowTalkbackSoundCtrl; // 是否显示声音控制条

	bool	m_bTalkbackSoundCreated; // 是否已经创建
	bool	m_bSoundCreated; // 是否已经创建
	bool	m_bMenuCreated; // 是否已经创建
public:
	//BOOL GetTalkbackCtrlState() const { return m_bTalkbackCtrlState; }
	void SetTalkbackCtrlState(bool state);
	bool GetShowTalkbackSoundCtrl() const { return m_bShowTalkbackSoundCtrl; }
	void SetShowTalkbackSoundCtrl(bool val) { m_bShowTalkbackSoundCtrl = val; }
	
public:
    ULONG GetTalkbackSoundValue() const { return m_ulTalkbackSoundValue; }
    void SetTalkbackSoundValue(ULONG val) { m_ulTalkbackSoundValue = val; }
private:

    BOOL    m_bTalkbackSoundState;           //声音控制状态
public:
    BOOL GetTalkbackSoundState() const { return m_bTalkbackSoundState; }
    void SetTalkbackSoundState(BOOL val) { m_bTalkbackSoundState = val; }
private:

    BOOL    m_bTalkbackSlienceState;         //静音状态
    BOOL    m_bIsTurning;                    //轮巡状态
//private:
//	//由于现在标题栏的父窗口已经不是窗格，需要记录下对应的窗格
//	CWnd* m_pVideoPane;
//public:
//    CWnd* GetVideoPane() const { return m_pVideoPane; } //lint !e1763  MFC自动生成的的函数
//	void SetVideoPane(CWnd * pVideoPane){m_pVideoPane = pVideoPane;}

private:
    // 音量对话框位置
    ULONG   m_ulSoundCtrlLeft;
public:
    ULONG GetSoundCtrlLeft() const { return m_ulSoundCtrlLeft; }
private:
    ULONG   m_ulSoundCtrlTop;
public:
    ULONG GetSoundCtrlTop() const { return m_ulSoundCtrlTop; }
private:

public:
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButtonRealtimeCapture();
    afx_msg void OnBnClickedButtonRealtimeRecord();
    afx_msg void OnBnClickedButtonRealtimeLabel();
    afx_msg void OnBnClickedButtonRealtimeMagnify();
    afx_msg void OnBnClickedButtonRealtimeSound();
    afx_msg void OnBnClickedButtonRealtimeTalkback();
    afx_msg void OnBnClickedButtonRealtimeExtent();
	afx_msg void OnBnClickedBtnToolRealtimeInstantBackPlay();
	afx_msg void OnBnClickedBtnToolRealtimeTvwall();
	afx_msg void OnBnClickedBtnToolRealtimeSetWarnWnd();
	afx_msg void OnBnClickedBtnToolRealtimePtz();
	afx_msg void OnBnClickedBtnToolRealtimeSuperIntel();
	afx_msg void OnBnClickedBtnToolRealtimeEMap();
	afx_msg void OnBnClickedBtnToolRealtimeNetQuality();
    afx_msg void OnBnClickedBtnToolRealtimePtz3d();
	afx_msg void OnBnClickedBtnToolRealtimeMasterAuxiliary();
	afx_msg void OnBnClickedBtnToolRealtimeReturn();
	afx_msg void OnBnClickedBtnToolRealtimeStart();
	afx_msg	void OnMenuCmd(UINT nID);
    
public:
	virtual BOOL OnInitDialog();
    void CreateSoundCtrl();
    // 删除声音控制对话框
    void DeleteSoundCtrl();
	void DeletePopMenuBar();
    // 获得声音对话框
    const CVideoSoundCtrl* GetSoundCtrl() const{ return m_pVideoSoundCtrl;}
    //把工具栏按钮大小设置为0

    void ZeroSize();
    // 控制音量
    void ChangeSoundCtrlValue(ULONG ulType, ULONG ulValue);
    void InitSoundCtrl(ULONG ulType);
    // 控制声音
    void  StartSound();
    void  StopSound();
    ULONG GetSoundCtrlValue()const{return m_ulSoundCtrlValue;}
    void  SetSoundCtrlValue(ULONG ulValue){m_ulSoundCtrlValue = ulValue;}
    // 静音状态
    void  SetSoundSlienceFlag(BOOL bState);
    BOOL  GetSoundSlienceFlag();
    ULONG GetSoundState();
    void  DrawButton(IVS_INT32 iBtnNum = 6);
    // 获得Client模型
    ULONG GetClientMode() const;
    // Talkback Sound
    // 删除Talkback声音控制对话框
    void DeleteTalkbackCtrl();
	void CreateTalkbackCtrl();
    IVS_INT32 StartTalkbackSound();
    IVS_INT32 StopTalkbackSound();
	void CreatePopToolBar();

	void SetRecordBtnState(bool state);
    // 设置3D定位按钮状态
    void SetPtz3DBtnState(bool state);

	void SetMagnifyBtnState(bool state);
	void SetPtzBtnState(bool state);
	

private:
	//视频播放质量模式：清晰(画质优先)、流畅（网络优先）
	LONG m_lPalyQualityMode;
	LONG GetPalyQualityMode() const {return m_lPalyQualityMode;}
	void SetPalyQualityMode(LONG val) { m_lPalyQualityMode = val;}
	//仅仅用作按钮是否禁用的状态记录
	BOOL m_bIsButtonEnable;
private:
    ULONGLONG  m_ullToolBarBtnVisableStatus;
public:
    ULONGLONG GetToolBarVisableStatus() const { return m_ullToolBarBtnVisableStatus; }
    void SetToolBarVisableStatus(ULONGLONG val) { m_ullToolBarBtnVisableStatus = val; }
public:
    std::bitset<TOOLBARBTNNUM> GetBitsetButtonVisableStatus() const;//lint !e1048
private:
	std::bitset<TOOLBARBTNNUM> m_bitsetButtonUsableStatus;//lint !e1048
	std::bitset<TOOLBARBTNNUM> m_bitsetButtonCompetenceStatus;//lint !e1048
public:
	void SetToolbarEnableStatus( ULONGLONG dwToolbarBtn, ULONGLONG lButtonStatus );
	
protected:
	afx_msg LRESULT OnIdcMenu(WPARAM wParam, LPARAM lParam);

	void RoutineButtonHideMenuEvent( WPARAM wParam );
	void RoutineButtonShowMenuEvent( WPARAM wParam );

	void SetCurrentPaneToActivePane();
public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

    ULONG GetTalkbackSoundState();//lint !e1762
    BOOL IsTalkback();
    void SetSoundPic(ULONG ulState);
    void EnableALLButton(BOOL isEnable);

	//add by w00210470
	void Reset();

	void ResetBottomButton();

	HWND GetMagnifyBtnHwnd()const {return m_buttonMagnify.GetSafeHwnd(); }
	HWND GetPTZBtnHwnd()const {return m_buttonPtz.GetSafeHwnd(); }
	HWND GetTalkbackBtnHwnd()const {return m_buttonTalkback.GetSafeHwnd(); }
    HWND GetReturnBtnHwnd()const {return m_btnReturn.GetSafeHwnd(); }
	HWND GetRecordBtnHwnd()const {return m_buttonLocalRecord.GetSafeHwnd(); }
public:
    afx_msg LRESULT OnSetSoundState(WPARAM wParam, LPARAM lParam);

	void UpdateAllButtonSkin();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};//lint !e1712  MFC自动生成的的函数
