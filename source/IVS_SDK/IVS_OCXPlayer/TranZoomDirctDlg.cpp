// DitalPtzDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "IVS_OCXPlayer.h"
#include "TranZoomDirctDlg.h"
#include "afxdialogex.h"
#include "IVS_OCXPlayerCtrl.h"

// CTranZoomDirctDlg dialog

IMPLEMENT_DYNAMIC(CTranZoomDirctDlg, CTranslucentDialog)

CTranZoomDirctDlg::CTranZoomDirctDlg(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CTranZoomDirctDlg::IDD, lpszFile,  pParent)
{

}

CTranZoomDirctDlg::~CTranZoomDirctDlg()
{
}

void CTranZoomDirctDlg::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_BTN_TOP, m_btnTop);
	DDX_Control(pDX, IDC_BTN_RIGHT, m_btnRight);
	DDX_Control(pDX, IDC_BTN_BOTTOM, m_btnBottom);
}

// TODO 这里的事件之后需要改成DOWN和UP，并启用定时器发送消息
BEGIN_MESSAGE_MAP(CTranZoomDirctDlg, CTranslucentDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TRANS_BTN_DOWN, &CTranZoomDirctDlg::OnDitalBnDown)
	ON_MESSAGE(WM_TRANS_BTN_UP, &CTranZoomDirctDlg::OnDitalBnUp)
	ON_BN_CLICKED(IDC_BTN_LEFT, &CTranZoomDirctDlg::OnBnClickedLeft)
	ON_BN_CLICKED(IDC_BTN_TOP, &CTranZoomDirctDlg::OnBnClickedTop)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &CTranZoomDirctDlg::OnBnClickedRight)
	ON_BN_CLICKED(IDC_BTN_BOTTOM, &CTranZoomDirctDlg::OnBnClickedBottom)
END_MESSAGE_MAP()


// CTranZoomDirctDlg message handlers

BOOL CTranZoomDirctDlg::OnInitDialog()
{
	CTranslucentDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTranZoomDirctDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CTranslucentDialog::OnPaint() for painting messages
	//TODO 去掉上下两条的宽度和计算不是全屏时的放大
	CRect rect;
	GetClientRect(rect);

	int iWidth = rect.Width();
	int iHeight = rect.Height();

	m_btnLeft.MoveWindow(rect.left, rect.top+iHeight/2-8, 16, 16);
	m_btnLeft.ShowWindow(SW_SHOW);
	m_btnRight.MoveWindow(rect.right-16, rect.top+iHeight/2-8, 16, 16);
	m_btnRight.ShowWindow(SW_SHOW);
	m_btnTop.MoveWindow(rect.left+iWidth/2-8, rect.top, 16, 16);
	m_btnTop.ShowWindow(SW_SHOW);
	m_btnBottom.MoveWindow(rect.left+iWidth/2-8, rect.top+iHeight-16, 16, 16);
	m_btnBottom.ShowWindow(SW_SHOW);

	UpdateView();
}

void CTranZoomDirctDlg::OnInitChildrenWnds()
{
	SetButtonPicture(m_btnLeft, "Left");
	SetButtonPicture(m_btnTop, "Top");
	SetButtonPicture(m_btnRight, "Right");
	SetButtonPicture(m_btnBottom, "Bottom");
}

void CTranZoomDirctDlg::SetButtonPicture( CTranslucentButton &tButton,const std::string &strButtonName )
{
	CHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]={0x0};

	memset(pcImageList,0x0,256*5);
	if (strButtonName.empty())
	{
		return;
	}
	(void)SkinMgr.DigtalPtzGetPNGButtonByName(strButtonName,pcImageList);
	(void)tButton.LoadImageList(pcImageList);
	RegisterTranslucentWnd(&tButton);
}

void CTranZoomDirctDlg::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}

#define BTN_TIMER_INVA 500
LRESULT CTranZoomDirctDlg::OnDitalBnDown(WPARAM wParam, LPARAM lParam)
{
	// 开启定时器
	if ((HWND)wParam == m_btnLeft.GetSafeHwnd())
	{
		SetTimer(DIRCT_BTN_LEFT, BTN_TIMER_INVA, NULL);
	}
	else if ((HWND)wParam == m_btnTop.GetSafeHwnd())
	{
		SetTimer(DIRCT_BTN_TOP, BTN_TIMER_INVA, NULL);
	}
	else if ((HWND)wParam == m_btnRight.GetSafeHwnd())
	{
		SetTimer(DIRCT_BTN_RIGHT, BTN_TIMER_INVA, NULL);
	}
	else if ((HWND)wParam == m_btnBottom.GetSafeHwnd())
	{
		SetTimer(DIRCT_BTN_BOTTOM, BTN_TIMER_INVA, NULL);
	}
	else
	{
		return 0;
	}

	return 0;
}//lint !e715

LRESULT CTranZoomDirctDlg::OnDitalBnUp(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam == m_btnLeft.GetSafeHwnd())
	{
		// 关闭定时器
		KillTimer(DIRCT_BTN_LEFT);
	}
	else if ((HWND)wParam == m_btnTop.GetSafeHwnd())
	{
		KillTimer(DIRCT_BTN_TOP);
	}
	else if ((HWND)wParam == m_btnRight.GetSafeHwnd())
	{
		KillTimer(DIRCT_BTN_RIGHT);
	}
	else if ((HWND)wParam == m_btnBottom.GetSafeHwnd())
	{
		KillTimer(DIRCT_BTN_BOTTOM);
	}
	else
	{
		return 0;
	}

	return 0;
}//lint !e715

void CTranZoomDirctDlg::OnTimer(UINT_PTR nIDEvent)
{
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
	CHECK_POINTER_VOID(pSrcVideoPane);
	switch (nIDEvent)
	{
	case DIRCT_BTN_LEFT:
		::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_LEFT, (int)m_pVideoPane);
		break;
	case DIRCT_BTN_TOP:
		::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_TOP, (int)m_pVideoPane);
		break;
	case DIRCT_BTN_RIGHT:
		::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_RIGHT, (int)m_pVideoPane);
		break;
	case DIRCT_BTN_BOTTOM:
		::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_BOTTOM, (int)m_pVideoPane);
		break;
	default:
		//KillTimer(nIDEvent);
		break;
	}
}

void CTranZoomDirctDlg::OnBnClickedLeft()
{
	// 给放大窗口发送一个消息
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
	CHECK_POINTER_VOID(pSrcVideoPane);
	::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_LEFT, (int)m_pVideoPane);
}

void CTranZoomDirctDlg::OnBnClickedTop()
{
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
	CHECK_POINTER_VOID(pSrcVideoPane);
	::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_TOP, (int)m_pVideoPane);
}

void CTranZoomDirctDlg::OnBnClickedRight()
{
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
	CHECK_POINTER_VOID(pSrcVideoPane);
	::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_RIGHT, (int)m_pVideoPane);
}

void CTranZoomDirctDlg::OnBnClickedBottom()
{
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
	CHECK_POINTER_VOID(pSrcVideoPane);
	::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RECT_MOVE, DIRCT_BTN_BOTTOM, (int)m_pVideoPane);
}
void CTranZoomDirctDlg::SetBarPictrue(LPCTSTR lpszFile)
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
