// PlayTimerBar.cpp : implementation file
//

#include "stdafx.h"
//#include "IVS_OCXPlayer.h"
#include "PlayTimerBar.h"
#include "afxdialogex.h"
//#include "TranslucentUtility.h"

// CPlayTimerBar dialog

IMPLEMENT_DYNAMIC(CPlayTimerBar, CTranslucentDialog)

	CPlayTimerBar::CPlayTimerBar(LPCTSTR lpszFile, CWnd* pParent)
	: CTranslucentDialog(CPlayTimerBar::IDD, lpszFile, pParent)
{
	memset(&m_stStartTime,0,sizeof(ST_TIME));
	memset(&m_stStartTime,0,sizeof(ST_TIME));
	memset(&m_stEndTime,0,sizeof(ST_TIME));
	memset(&m_stLayerTime,0,sizeof(ST_LAYERTIME));
	memset(&m_stCurrentTime,0,sizeof(ST_TIME));
	m_iLayerType = -1;
}//lint !e830

CPlayTimerBar::CPlayTimerBar(UINT nImgID, LPCTSTR lpszType, HINSTANCE hResourceModule, CWnd* pParent)
	: CTranslucentDialog(CPlayTimerBar::IDD, nImgID, lpszType, hResourceModule, pParent)
{
	memset(&m_stStartTime,0,sizeof(ST_TIME));
	memset(&m_stStartTime,0,sizeof(ST_TIME));
	memset(&m_stEndTime,0,sizeof(ST_TIME));
	memset(&m_stLayerTime,0,sizeof(ST_LAYERTIME));
	memset(&m_stCurrentTime,0,sizeof(ST_TIME));
	m_iLayerType = -1;
}//lint !e830


CPlayTimerBar::~CPlayTimerBar()
{
}

void CPlayTimerBar::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIMER, m_IvsTimer);
}


BEGIN_MESSAGE_MAP(CPlayTimerBar, CTranslucentDialog)
	ON_WM_PAINT()
	ON_MESSAGE(WM_TIMERBAR_TIME_MOVE,OnCurrrentTimer)
	ON_MESSAGE(WM_TIMERBAR_STOP_PLAY,OnStopRecordPlay)
    ON_MESSAGE(WM_TIMERBAR_START_PLAY, OnInstantReplayStart)
	ON_MESSAGE(WM_TIMERBAR_PLAYBACK_STOP, OnInstantReplaySegmentStop)
	ON_WM_KEYUP()
END_MESSAGE_MAP()//lint !e751


// CPlayTimerBar message handlers


BOOL CPlayTimerBar::OnInitDialog()
{
	CTranslucentDialog::OnInitDialog();
	m_IvsTimer.SetPlayTimerBar(this);
	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPlayTimerBar::OnInitChildrenWnds()
{
	RegisterTranslucentWnd(&m_IvsTimer);
}


void CPlayTimerBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);
	
	//m_IvsTimer.SetBarTime(m_stStartTime,m_stEndTime);//lint !e534
	//m_IvsTimer.SetCurrentTime(m_stCurrentTime);//lint !e534

	m_IvsTimer.MoveWindow(rect);
	m_IvsTimer.ShowWindow(SW_SHOW);
	m_IvsTimer.Invalidate(TRUE);
}

void CPlayTimerBar::SetBarTime(const ST_TIME &stStarttime,const ST_TIME &stEndtime)
{
	m_stStartTime.iHour = stStarttime.iHour;
	m_stStartTime.iMinute = stStarttime.iMinute;
	m_stEndTime.iHour = stEndtime.iHour;
	m_stEndTime.iMinute = stEndtime.iMinute;
}

void CPlayTimerBar::SetCurrentTime(const ST_TIME &time)
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	m_stCurrentTime.iHour = time.iHour;
	m_stCurrentTime.iMinute = time.iMinute;
	m_stCurrentTime.iScond = time.iScond;
}

void CPlayTimerBar::AddLayeTimeRange(int iType,const ST_LAYERTIME &stLayerTime)
{
	m_IvsTimer.AddLayeTimeRange(iType,stLayerTime);
}

void CPlayTimerBar::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}

void CPlayTimerBar::SetCurrentRangeAndType(const ST_LAYERTIME &stLayerTime,int iType)
{
	m_stLayerTime.stStartTime.iHour = stLayerTime.stStartTime.iHour;
	m_stLayerTime.stStartTime.iMinute = stLayerTime.stStartTime.iMinute;
	m_stLayerTime.stStartTime.iScond = stLayerTime.stStartTime.iScond;
	m_stLayerTime.stEndTime.iHour = stLayerTime.stEndTime.iHour;
	m_stLayerTime.stEndTime.iMinute = stLayerTime.stEndTime.iMinute;
	m_stLayerTime.stEndTime.iScond = stLayerTime.stEndTime.iScond;
	m_iLayerType = iType;
}

LRESULT CPlayTimerBar::OnCurrrentTimer(WPARAM wParam,LPARAM pParam)
{
	/*ST_TIMER_MSG* pTimer = new ST_TIMER_MSG;
	memset(pTimer,0,sizeof(ST_TIMER_MSG));
	pTimer->iType = m_iLayerType;
	pTimer->stStartTime.iHour = m_stLayerTime.stStartTime.iHour;
	pTimer->stStartTime.iMinute = m_stLayerTime.stStartTime.iMinute;
	pTimer->stStartTime.iScond = m_stLayerTime.stStartTime.iScond;
	pTimer->stEndTime.iHour = m_stLayerTime.stEndTime.iHour;
	pTimer->stEndTime.iMinute = m_stLayerTime.stEndTime.iMinute;
	pTimer->stEndTime.iScond = m_stLayerTime.stEndTime.iScond;
	pTimer->stCurrentTime.iHour = m_stCurrentTime.iHour;
	pTimer->stCurrentTime.iMinute = m_stCurrentTime.iMinute;
	pTimer->stCurrentTime.iScond = m_stCurrentTime.iScond;*/

	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER(pVideoPane,-1);

	pVideoPane->PostMessage(WM_TIMERBAR_TIME_MOVE,0,(LPARAM)pParam);
	return 0;
}//lint !e715

LRESULT CPlayTimerBar::OnStopRecordPlay(WPARAM wParam,LPARAM pParam)
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER(pVideoPane,-1);

	(void)pVideoPane->StopRecordPlay();
	return 0;
}//lint !e715

void CPlayTimerBar::InitTimeRange()
{
	m_IvsTimer.InitTimeRange();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	m_IvsTimer.SetVideoPane(pVideoPane);
}
BOOL CPlayTimerBar::IsCanPlayBack(ST_TIMER_MSG &stTimerMsg)
{
	return m_IvsTimer.IsCanPlayBack(stTimerMsg);
}

LONG CPlayTimerBar::StartRecordPlay()
{
	//判断下当前的时间返回内是否可以回放录像,失败返回1
	m_IvsTimer.StartRecordPlayTimer();
	return 0;
}

LONG CPlayTimerBar::StopRecordPlay()
{
	m_IvsTimer.StopRecordPlayTime();
	return 0;
}

void CPlayTimerBar::StopVideoPlay()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	(void)pVideoPane->StopRecordPlay();
}

void CPlayTimerBar::ResetTimer()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	//m_IvsTimer.InitTimeRange();
	m_IvsTimer.ResetTimer();
	//pVideoPane->ResetBarPosition();
}

void CPlayTimerBar::SetPlayCurrentTime(const ST_TIME &stCurrenttime)
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	(void)m_IvsTimer.SetCurrentTime(stCurrenttime);
}

void CPlayTimerBar::ResetRange()
{
	m_IvsTimer.Clear();
	m_IvsTimer.ClearGreenList();
}

// 调用接口获取当前回放时间
void CPlayTimerBar::GetPlayTime(ST_TIME& PlayTime)
{
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);

    pVideoPane->GetPlayBackCurrentTime(PlayTime);
}

// 设置播放起始时间
void CPlayTimerBar::SetStartPlayTime()
{
    ST_TIME stCurrenttime = m_IvsTimer.GetCurrentTime();
    m_IvsTimer.SetStartPlayTime(stCurrenttime);
}
LRESULT CPlayTimerBar::OnInstantReplayStart(WPARAM wParam, LPARAM lParam)
{
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane,-1);

    pVideoPane->PostMessage(WM_TIMERBAR_START_PLAY, wParam, lParam);
    return 0;
}

LRESULT CPlayTimerBar::OnInstantReplaySegmentStop( WPARAM wParam, LPARAM lParam )
{
	m_IvsTimer.SetCurStreamStop();
    return 0;
}//lint !e715


bool CPlayTimerBar::FindNextPlayRange(const ST_TIME &CurTime, ST_TIME &NextTime)
{
    return m_IvsTimer.FindNextPlayRange(CurTime, NextTime);
}

void CPlayTimerBar::GetTimeMsg(const ST_TIME& stTime, ST_TIMER_MSG &stTimerMsg)
{
    return m_IvsTimer.GetTimeMsg(stTime, stTimerMsg);
}

void CPlayTimerBar::SetBarPictrue(LPCTSTR lpszFile)
{
	TCHAR wsFile[MAX_PATH] = {0};
	if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)lpszFile,strnlen(lpszFile,MAX_PATH)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("Translucent Dialog Failed"));
	}
	WCHAR* pcharFileName = CTranslucentUtility::ttowc(wsFile);
	Gdiplus::Image* pImage = Gdiplus::Image::FromFile(pcharFileName);
	if (pImage==NULL)
	{
		if (NULL != pcharFileName)
		{
			delete []pcharFileName;
			pcharFileName = NULL;
		}
		return;
	}

	SetBackGroudPictrue(pImage);
	OnInitChildrenWnds();
	if (NULL != pcharFileName)
	{
		delete []pcharFileName;
		pcharFileName = NULL;
	}
}


void CPlayTimerBar::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	//CTranslucentDialog::OnOK();
}


void CPlayTimerBar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (VK_RETURN == nChar)
	{
		return;
	}

	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER_VOID(pVideoPaneMgr);

	if (pVideoPaneMgr->GetActivePaneID()!=pVideoPane->GetPaneID())
	{
		return;
	}

	CWnd *pWnd = pVideoPaneMgr->GetParentWnd();
	CHECK_POINTER_VOID(pWnd);

	::PostMessage(pWnd->GetSafeHwnd(),WM_KEYUP,static_cast<LONG>(nChar),0);//lint !e732
	//CTranslucentDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}//lint !e715


