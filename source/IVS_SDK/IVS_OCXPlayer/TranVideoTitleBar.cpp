// TranVideoTitleBar.cpp : implementation file
//

#include "stdafx.h"
#include "TranVideoTitleBar.h"
#include "afxdialogex.h"
#include "VideoPane.h"
#include "VideoPaneMgr.h"
#include "Utility.h"
#include "IVS_OCXPlayerCtrl.h"

// CTranVideoTitleBar dialog

IMPLEMENT_DYNAMIC(CTranVideoTitleBar, CTranslucentDialog)

CTranVideoTitleBar::CTranVideoTitleBar(Gdiplus::Image* pImage, CWnd* pParent)
	: CTranslucentDialog(CTranVideoTitleBar::IDD, pImage, pParent)
{
	m_ulTVWallStatus = 0;
	m_ulTurnCruiseStatus = 0;
	m_ulSingleFrameStatus = 0;
	m_ulAlarmStatus = 0;
	m_ulRecordStatus = 0;
	m_ulCruiseTrackStatus = 0;
	m_ulWatermarkStutus = -1;
}

CTranVideoTitleBar::~CTranVideoTitleBar()
{
}//lint !e1540 pVideoPane 由外部传入 无须释放

void CTranVideoTitleBar::DoDataExchange(CDataExchange* pDX)
{
    CTranslucentDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_TITLE_OFF, m_btnOFF);
    DDX_Control(pDX, IDC_BTN_TITLE_INF, m_btnInf);
    DDX_Control(pDX, IDC_BTN_TITLE_TVWALL_HANDLING, m_btnTVWallHandling);
    DDX_Control(pDX, IDC_BTN_TITLE_TVWALL_OK, m_btnTVWallOK);
    DDX_Control(pDX, IDC_BTN_TITLE_TVWALL_VIDEO_BROKEN, m_btnTVWallVideoBroken);
    DDX_Control(pDX, IDC_DEVNAME_TEXT, m_StaticDevName);
    DDX_Control(pDX, IDC_BTN_WATERMARK_STATUS, m_btnWaterMakr);
    DDX_Control(pDX, IDC_BTN_TURNCRUISE, m_btnTurnCruiseStatus);
    DDX_Control(pDX, IDC_BTN_SINGLEFRAME_STATUS, m_btnSingleFrameStatus);
    DDX_Control(pDX, IDC_BTN_ALARM_STATUS, m_btnAlarmStatus);
    DDX_Control(pDX, IDC_BTN_RECORD_STATUS, m_btnRecordStatus);
    DDX_Control(pDX, IDC_BTN_CRUISETRACK_STATUS, m_btnCruiseTrackStatus);
    DDX_Control(pDX, IDC_BTN_RECONNECT_STATUS, m_btnTvWallReconnect);
}

BEGIN_MESSAGE_MAP(CTranVideoTitleBar, CTranslucentDialog)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BTN_TITLE_OFF, &CTranVideoTitleBar::OnBnClickedBtnTitleOff)
    ON_BN_CLICKED(IDC_BTN_TITLE_INF, &CTranVideoTitleBar::OnBnClickedBtnTitleInf)
    ON_WM_SHOWWINDOW()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

void CTranVideoTitleBar::OnInitChildrenWnds()
{
	RegisterTranslucentWnd(&m_StaticDevName);
	SetButtonPicture(m_btnOFF, "Off");
	SetButtonPicture(m_btnInf, "Info");
	SetButtonPicture(m_btnTVWallHandling,"TVWallHandling");
	SetButtonPicture(m_btnTVWallOK,"TVWallOK");
	SetButtonPicture(m_btnTVWallVideoBroken,"TVWallVideoBroken");
	SetButtonPicture(m_btnWaterMakr,"WatermarkGary");
	SetButtonPicture(m_btnTurnCruiseStatus,"TrunCuise");
	SetButtonPicture(m_btnSingleFrameStatus,"SingleFrameStatus");
	SetButtonPicture(m_btnAlarmStatus,"AlarmStatus");
	SetButtonPicture(m_btnRecordStatus,"RecordStatus");
	SetButtonPicture(m_btnCruiseTrackStatus,"CruiseTrackStatus");
    SetButtonPicture(m_btnTvWallReconnect,"TVWallReConnect");
}

// CTranVideoTitleBar message handlers

void CTranVideoTitleBar::OnBnClickedBtnTitleOff()
{
    // 视频窗口右上角关闭事件，迭代一只做实况，后续要区分是实况还是回放
    SetCurrentPaneToActivePane();

    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);

	pVideoPane->StopSerialSnapshot();
	if (pVideoPane->GetShowInstantStatus())
	{
		pVideoPane->ResumeRealPlay();
	}
	pVideoPane->SetResumeState(false);
	pVideoPane->SetStopDrag(true);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	ASSERT(NULL != pCtrl);
	(void)pCtrl->StopRealPlayByPane(pVideoPane);
    pCtrl->PostStopButtonClickMsg(pVideoPane->GetPaneID());
}

void CTranVideoTitleBar::ZeroSize()
{
	CRect _Rect(0,0,0,0);
	m_StaticDevName.MoveWindow(_Rect);
	m_StaticDevName.ShowWindow(SW_HIDE);
    m_btnOFF.MoveWindow(0, 0, 0, 0);
    m_btnOFF.ShowWindow(SW_HIDE);
    m_btnInf.MoveWindow(0, 0, 0, 0);
    m_btnInf.ShowWindow(SW_HIDE);
	m_btnTVWallHandling.MoveWindow(0,0,0,0);
	m_btnTVWallHandling.ShowWindow(SW_HIDE);
	m_btnTVWallOK.MoveWindow(0,0,0,0);
	m_btnTVWallOK.ShowWindow(SW_HIDE);
	m_btnTVWallVideoBroken.MoveWindow(0,0,0,0);
	m_btnTVWallVideoBroken.ShowWindow(SW_HIDE);
	m_btnTurnCruiseStatus.MoveWindow(0,0,0,0);
	m_btnTurnCruiseStatus.ShowWindow(SW_HIDE);
	m_btnSingleFrameStatus.MoveWindow(0,0,0,0);
	m_btnSingleFrameStatus.ShowWindow(SW_HIDE);
	m_btnAlarmStatus.MoveWindow(0,0,0,0);
	m_btnAlarmStatus.ShowWindow(SW_HIDE);
	m_btnRecordStatus.MoveWindow(0,0,0,0);
	m_btnRecordStatus.ShowWindow(SW_HIDE);
	m_btnCruiseTrackStatus.MoveWindow(0,0,0,0);
	m_btnCruiseTrackStatus.ShowWindow(SW_HIDE);
	m_btnWaterMakr.MoveWindow(0,0,0,0);
	m_btnWaterMakr.ShowWindow(SW_HIDE);
    m_btnTvWallReconnect.MoveWindow(0,0,0,0);
    m_btnTvWallReconnect.ShowWindow(SW_HIDE);
}

void CTranVideoTitleBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
    CRect rect;
	pVideoPane->GetClientRect(&rect);

	const long ICON_WIDTH = 23;
	long lWidth = ICON_WIDTH;
	long lRight = rect.right;
	m_StaticDevName.MoveWindow(1,1,lRight - 70, 22);
	m_StaticDevName.ShowWindow(SW_SHOW);

    m_btnOFF.MoveWindow(lRight - lWidth, 1, 22, 22);
    m_btnOFF.ShowWindow(SW_SHOW);
	lWidth += ICON_WIDTH;
    m_btnInf.MoveWindow(lRight - lWidth, 1, 22, 22);
    m_btnInf.ShowWindow(SW_SHOW);

	if(TVWALL_STATUS_START==m_ulTVWallStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnTVWallHandling.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnTVWallHandling.ShowWindow(SW_SHOW);
		m_btnTVWallOK.ShowWindow(SW_HIDE);
		m_btnTVWallVideoBroken.ShowWindow(SW_HIDE);
        m_btnTvWallReconnect.ShowWindow(SW_HIDE);
	}
    if(TVWALL_STATUS_DEC_DISCONN==m_ulTVWallStatus)
    {
        lWidth += ICON_WIDTH;
        m_btnTvWallReconnect.MoveWindow(lRight - lWidth, 3, 16, 16);
        m_btnTvWallReconnect.ShowWindow(SW_SHOW);
        m_btnTVWallOK.ShowWindow(SW_HIDE);
        m_btnTVWallVideoBroken.ShowWindow(SW_HIDE);
    }
	else if(TVWALL_STATUS_PLAY==m_ulTVWallStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnTVWallOK.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnTVWallOK.ShowWindow(SW_SHOW);
		m_btnTVWallHandling.ShowWindow(SW_HIDE);
		m_btnTVWallVideoBroken.ShowWindow(SW_HIDE);
        m_btnTvWallReconnect.ShowWindow(SW_HIDE);
	}
	else if(TVWALL_STATUS_VIDEO_BROKEN==m_ulTVWallStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnTVWallVideoBroken.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnTVWallVideoBroken.ShowWindow(SW_SHOW);
		m_btnTVWallOK.ShowWindow(SW_HIDE);
		m_btnTVWallHandling.ShowWindow(SW_HIDE);
	}
	else
	{
		m_btnTVWallOK.ShowWindow(SW_HIDE);
		m_btnTVWallHandling.ShowWindow(SW_HIDE);
		m_btnTVWallVideoBroken.ShowWindow(SW_HIDE);
        m_btnTvWallReconnect.ShowWindow(SW_HIDE);
	}

	if (1 == m_ulTurnCruiseStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnTurnCruiseStatus.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnTurnCruiseStatus.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnTurnCruiseStatus.ShowWindow(SW_HIDE);
	}

	if (1 == m_ulSingleFrameStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnSingleFrameStatus.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnSingleFrameStatus.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnSingleFrameStatus.ShowWindow(SW_HIDE);
	}

	if (1 == m_ulAlarmStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnAlarmStatus.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnAlarmStatus.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnAlarmStatus.ShowWindow(SW_HIDE);
	}

	if (1 == m_ulRecordStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnRecordStatus.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnRecordStatus.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnRecordStatus.ShowWindow(SW_HIDE);
	}

	if (1 == m_ulCruiseTrackStatus)
	{
		lWidth += ICON_WIDTH;
		m_btnCruiseTrackStatus.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnCruiseTrackStatus.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnCruiseTrackStatus.ShowWindow(SW_HIDE);
	}

	if (-1 == m_ulWatermarkStutus)
	{
		m_btnWaterMakr.ShowWindow(SW_HIDE);
	}
	else
	{
		lWidth += ICON_WIDTH;
		m_btnWaterMakr.MoveWindow(lRight - lWidth, 3, 16, 16);
		m_btnWaterMakr.ShowWindow(SW_SHOW);
	}
	
    UpdateView();
}

ENUMDEF::WND_TYPE CTranVideoTitleBar::GetWndType() const
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane,WND_TYPE_REALTIME);
    return pVideoPane->GetWndType();
}

// 获得媒体信息
void CTranVideoTitleBar::OnBnClickedBtnTitleInf()
{
    SetCurrentPaneToActivePane();

    CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);

	//通知CU，由他来弹出对话框
	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER_VOID(pVideoPaneMgr);
	// 删除自适应大小对话框
	//	pVideoPaneMgr->DeleteRealtimeBarLocker();
	pVideoPaneMgr->DeleteSoundCtrl();

	//没有在实况，事件不触发
	if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
	{
		return;
	}
	ULONG ulWinID = pVideoPane->GetPaneID();
	if (NULL != pVideoPane->GetOCXPlayerCtrl())
	{
		pVideoPane->GetOCXPlayerCtrl()->PostGetPlayInfoEventMsg(ulWinID);
	}
}

IVS_CLIENT_TYPE CTranVideoTitleBar::GetClientMode() const
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane,CLIENT_PC);
    return pVideoPane->GetClientType();
}

std::string CTranVideoTitleBar::GetDevName() const
{
	std::string strDevName;
	TCHAR pszDevName[MAX_PATH] = {0};
	memset(pszDevName,0x0,MAX_PATH);
	m_StaticDevName.GetWindowText(pszDevName,MAX_PATH);
	strDevName.append(pszDevName);
	return strDevName;//lint !e1036
}//lint !e522

void CTranVideoTitleBar::SetDevName( const std::string &strDevName )
{
	if (!strDevName.empty())
	{
        if(::IsWindow(m_StaticDevName.GetSafeHwnd()))
        {
            m_StaticDevName.SetWindowText(strDevName.c_str());
            UpdateView();
        }
	}
}

BOOL CTranVideoTitleBar::OnInitDialog()
{
    CTranslucentDialog::OnInitDialog();
    return TRUE;
}

CPoint CTranVideoTitleBar::GetInfoButtonRightBottomPosition()const
{
	CRect _Rect;
    GetClientRect(_Rect);
	ClientToScreen(_Rect);
	return CPoint(_Rect.right - 25,_Rect.top + 23);
}

void CTranVideoTitleBar::SetButtonPicture( CTranslucentButton &tButton,const std::string &strButtonName )
{
	// 根据名字 获取对应的资源文件中的信息
	std::string strToolTip;
	if (CI18N::GetInstance().GetResourceLanguage(strButtonName, strToolTip))
	{
		tButton.UpdateTipText(strToolTip);
	}
	// 添加到tip中
	(void)m_TipMap.insert(std::make_pair(strButtonName, &tButton));

	CHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]={0x0};

	memset(pcImageList,0x0,256*5);
	if (strButtonName.empty())
	{
		return;
	}
	(void)SkinMgr.TitleBarGetPNGButtonByName(strButtonName,pcImageList);
	(void)tButton.LoadImageList(pcImageList);
	RegisterTranslucentWnd(&tButton);
}

void CTranVideoTitleBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CTranslucentDialog::OnShowWindow(bShow, nStatus);
}

void CTranVideoTitleBar::OnMouseMove(UINT nFlags, CPoint point)
{
    CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);    
    pVideoPane->OnMouseMove(nFlags, point);
    CTranslucentDialog::OnMouseMove(nFlags, point);
}

void CTranVideoTitleBar::SetCurrentPaneToActivePane()
{
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    pVideoPane->SetActivePane();
}


void CTranVideoTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCurrentPaneToActivePane();
    CTranslucentDialog::OnLButtonDown(nFlags, point);
}

void CTranVideoTitleBar::EnableALLButton( BOOL isEnable )
{
    m_btnOFF.EnableWindow(isEnable);
    m_btnInf.EnableWindow(isEnable);
	if (!isEnable)
	{
		m_ulWatermarkStutus = -1;
	}
	UpdateView();
}

void CTranVideoTitleBar::SetWatermarkStatus(ULONG ulStatus)
{
	m_ulWatermarkStutus = (LONG)ulStatus;
	//如果是0就是灰色的关闭
	if (0 == ulStatus)
	{
		SetButtonPicture(m_btnWaterMakr,"WatermarkGary");
	}
	else if (1 == ulStatus)
	{
		SetButtonPicture(m_btnWaterMakr,"WatermarkNomal");
	}
	Invalidate();
}

void CTranVideoTitleBar::UpdateAllButtonSkin()
{
	SetButtonPicture(m_btnOFF, "Off");
	SetButtonPicture(m_btnInf, "Info");
	SetButtonPicture(m_btnTVWallHandling,"TVWallHandling");
	SetButtonPicture(m_btnTVWallOK,"TVWallOK");
	SetButtonPicture(m_btnTVWallVideoBroken,"TVWallVideoBroken");
	SetButtonPicture(m_btnWaterMakr,"WatermarkGary");
	SetButtonPicture(m_btnTurnCruiseStatus,"TrunCuise");
	SetButtonPicture(m_btnSingleFrameStatus,"SingleFrameStatus");
	SetButtonPicture(m_btnAlarmStatus,"AlarmStatus");
	SetButtonPicture(m_btnRecordStatus,"RecordStatus");
	SetButtonPicture(m_btnCruiseTrackStatus,"CruiseTrackStatus");
	SetButtonPicture(m_btnTvWallReconnect,"TVWallReConnect");
	UpdateView();
};


void CTranVideoTitleBar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
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
