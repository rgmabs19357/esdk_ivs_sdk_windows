// TurnCruisePane.cpp : implementation file
//

#include "stdafx.h"
#include "TurnCruisePane.h"
#include "afxdialogex.h"
#include "IVS_OCXPlayerCtrl.h"

// CTurnCruisePane dialog

IMPLEMENT_DYNAMIC(CTurnCruisePane, CTranslucentDialog)

CTurnCruisePane::CTurnCruisePane(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CTurnCruisePane::IDD, lpszFile,  pParent)
    ,m_uiStyle(0)
{
    
}

CTurnCruisePane::CTurnCruisePane(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/,UINT uiStyle)
	: CTranslucentDialog(CTurnCruisePane::IDD, lpszFile,  pParent)
	,m_uiStyle(0)
{
	m_uiStyle = uiStyle;
}

CTurnCruisePane::~CTurnCruisePane()
{
}

void CTurnCruisePane::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PRE, m_BtnPre);
	DDX_Control(pDX, IDC_BTN_NEXT, m_BtnNext);
}


BEGIN_MESSAGE_MAP(CTurnCruisePane, CTranslucentDialog)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_PRE, &CTurnCruisePane::OnBnClickedBtnPre)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CTurnCruisePane::OnBnClickedBtnNext)
	ON_WM_KEYUP()
END_MESSAGE_MAP()


// CTurnCruisePane message handlers


void CTurnCruisePane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CTranslucentDialog::OnPaint() for painting messages
	CRect rect;
	GetClientRect(rect);

	//int iWidth = rect.Width();
	int iHeight = rect.Height();

	switch(m_uiStyle)
	{
	case TYPE_TURNCRUISE_BUTH:
		{
			m_BtnPre.MoveWindow(rect.left, rect.top+iHeight/2-18, 20, 36);
			m_BtnPre.ShowWindow(SW_SHOW);
			m_BtnNext.MoveWindow(rect.right-20, rect.top+iHeight/2-18, 20, 36);
			m_BtnNext.ShowWindow(SW_SHOW);
		}
		break;
	case TYPE_TURNCRUISE_LEFTBUTTON:
		{
			m_BtnPre.MoveWindow(rect.left, rect.top+iHeight/2-18, 20, 36);
			m_BtnPre.ShowWindow(SW_SHOW);
			m_BtnNext.MoveWindow(0, 0, 0, 0);
			m_BtnNext.ShowWindow(SW_HIDE);
		}
		break;
	case TYPE_TURNCRUISE_RIFHTBUTTON:
		{
			m_BtnPre.MoveWindow(0, 0, 0, 0);
			m_BtnPre.ShowWindow(SW_HIDE);
			m_BtnNext.MoveWindow(rect.right-20, rect.top+iHeight/2-18, 20, 36);
			m_BtnNext.ShowWindow(SW_SHOW);
		}
		break;
	default:
		ZeroSize();
		break;
	}

	UpdateView();
}

void CTurnCruisePane::OnInitChildrenWnds()
{
	SetButtonPicture(m_BtnPre, "Left");
	SetButtonPicture(m_BtnNext, "Right");
}

void CTurnCruisePane::ZeroSize()
{
	m_BtnPre.MoveWindow(0, 0, 0, 0);
	m_BtnPre.ShowWindow(SW_HIDE);
	m_BtnNext.MoveWindow(0, 0, 0, 0);
	m_BtnNext.ShowWindow(SW_HIDE);
}

void CTurnCruisePane::SetButtonPicture( CTranslucentButton &tButton,const std::string &strButtonName )
{
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

void CTurnCruisePane::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}


void CTurnCruisePane::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CTranslucentDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}


void CTurnCruisePane::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTranslucentDialog::OnMouseMove(nFlags, point);
}


void CTurnCruisePane::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTranslucentDialog::OnLButtonDown(nFlags, point);
}


void CTurnCruisePane::OnBnClickedBtnPre()
{
	// TODO: Add your control notification handler code here
	SetCurrentPaneToActivePane();

	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	//通知CU，由他来弹出对话框
	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER_VOID(pVideoPaneMgr);

	//没有在实况，事件不触发
	//if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
	//{
	//	return;
	//}

	ULONG ulWinID = pVideoPane->GetPaneID();
	pVideoPane->GetOCXPlayerCtrl()->PostTurnCruisePreEventMsg(ulWinID);
}


void CTurnCruisePane::OnBnClickedBtnNext()
{
	// TODO: Add your control notification handler code here
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
	//if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
	//{
	//	return;
	//}
	ULONG ulWinID = pVideoPane->GetPaneID();
	pVideoPane->GetOCXPlayerCtrl()->PostTurnCruiseNextEventMsg(ulWinID);
}

void CTurnCruisePane::SetBarPictrue(LPCTSTR lpszFile)
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


void CTurnCruisePane::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
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
