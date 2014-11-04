#pragma once
#include "afxwin.h"
#include "TranslucentButton.h"
#include "TranslucentDialog.h"
#include "TranslucentStatic.h"
#include "TimeStatic.h"
// CPlayTimerBar dialog
#define  WM_TIMERBAR_TIME_MOVE  (WM_USER+1099)
#define  WM_TIMERBAR_STOP_PLAY  (WM_USER+1100)
#define  WM_TIMERBAR_START_PLAY (WM_USER+1101)
#define  WM_TIMERBAR_PLAYBACK_STOP (WM_USER + 1102)


class CPlayTimerBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CPlayTimerBar)

public:
	//CPlayTimerBar(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	CPlayTimerBar(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	CPlayTimerBar(UINT nImgID, LPCTSTR lpszType = _T("PNG"), HINSTANCE hResourceModule = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlayTimerBar();

// Dialog Data
	enum { IDD = IDD_DLG_TIMERBAR };

	virtual void OnInitChildrenWnds();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTimeStatic m_IvsTimer;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	void SetBarTime(const ST_TIME &stStarttime,const ST_TIME &stEndtime);
	void SetCurrentTime(const ST_TIME &time);
	void AddLayeTimeRange(int iType,const ST_LAYERTIME &stLayerTime);
	void SetCurrentPaneToActivePane();
	void SetCurrentRangeAndType(const ST_LAYERTIME &stLayerTime,int iType);
	afx_msg LRESULT OnCurrrentTimer(WPARAM wParam,LPARAM pParam);
	afx_msg LRESULT OnStopRecordPlay(WPARAM wParam,LPARAM pParam);
	void InitTimeRange();
	LONG StartRecordPlay();
	LONG StopRecordPlay();
	void StopVideoPlay();
	//判断下当前时间端内是否可以播放录像
	BOOL IsCanPlayBack(ST_TIMER_MSG &stTimerMsg);
	void ResetTimer();
	void SetPlayCurrentTime(const ST_TIME &stCurrenttime);
	void ResetRange();
    void GetPlayTime(ST_TIME& PlayTime);
    void SetStartPlayTime();
    afx_msg LRESULT OnInstantReplayStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInstantReplaySegmentStop(WPARAM wParam, LPARAM lParam);


    bool FindNextPlayRange(const ST_TIME &CurTime, ST_TIME &NextTime);
    void GetTimeMsg(const ST_TIME& stTime, ST_TIMER_MSG &stTimerMsg);
	void SetBarPictrue(LPCTSTR lpszFile);
private:
	ST_TIME m_stCurrentTime;          // 当前时间;//lint !e830
	ST_TIME m_stStartTime;            // 起始时间;
	ST_TIME m_stEndTime;              // 终止时间;

	ST_LAYERTIME m_stLayerTime;       // 当前所在时间范围
	int m_iLayerType;                 // 当前所在范围的类型
	virtual void OnOK(); //lint !e1768
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
}; //lint !e1712
