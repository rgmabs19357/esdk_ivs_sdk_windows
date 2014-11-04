// TranZoomBar.cpp : implementation file
//

#include "stdafx.h"
#include "IVS_OCXPlayerCtrl.h"
#include "TranZoomTitleBar.h"
#include "afxdialogex.h"
//#include "TranslucentUtility.h"

// CTranZoomBar dialog

IMPLEMENT_DYNAMIC(CTranZoomTitleBar, CTranslucentDialog)

CTranZoomTitleBar::CTranZoomTitleBar(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CTranZoomTitleBar::IDD, lpszFile, pParent)
{
}

CTranZoomTitleBar::~CTranZoomTitleBar()
{
}

void CTranZoomTitleBar::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ZOOM_NAME, m_StaticZoomName);
	DDX_Control(pDX, IDC_BTN_ZOOMOVER, m_btnZoomOver);
}


BEGIN_MESSAGE_MAP(CTranZoomTitleBar, CTranslucentDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_ZOOMOVER, &CTranZoomTitleBar::OnBnClickedClose)
	ON_WM_KEYUP()
END_MESSAGE_MAP()//lint !e751


// CTranZoomBar message handlers

BOOL CTranZoomTitleBar::OnInitDialog()
{
	CTranslucentDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTranZoomTitleBar::OnPaint()
{
 	CPaintDC dc(this); // device context for painting
 	// TODO: Add your message handler code here
 	// Do not call CTranslucentDialog::OnPaint() for painting messages
 	//TODO 去掉上下两条的宽度和计算不是全屏时的放大
 	CRect rect;
 	GetClientRect(rect);
 
 	//int iWidth = rect.Width();
 	//int iHeight = rect.Height();
	m_StaticZoomName.MoveWindow(1,1,rect.right - 70, 22);
	m_StaticZoomName.ShowWindow(SW_SHOW);
	m_btnZoomOver.MoveWindow(rect.right - 23, 3, 22, 22);
	m_btnZoomOver.ShowWindow(SW_SHOW);

 	UpdateView();
}

void CTranZoomTitleBar::OnInitChildrenWnds()
{
	SetButtonPicture(m_btnZoomOver, "ZoomOver");
	RegisterTranslucentWnd(&m_StaticZoomName);
}

void CTranZoomTitleBar::SetButtonPicture( CTranslucentButton &tButton,const std::string &strButtonName )
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
	(void)SkinMgr.DigtalPtzGetPNGButtonByName(strButtonName,pcImageList);
	(void)tButton.LoadImageList(pcImageList);

	RegisterTranslucentWnd(&tButton);
}

void CTranZoomTitleBar::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}

void CTranZoomTitleBar::OnBnClickedClose()
{
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
	CHECK_POINTER_VOID(pSrcVideoPane);
	::PostMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (int)m_pVideoPane);
}

std::string CTranZoomTitleBar::GetZoomTitleName() const
{
	std::string strZoomName;
	TCHAR pszZoomName[MAX_PATH] = {0};
	memset(pszZoomName,0x0,MAX_PATH);
	m_StaticZoomName.GetWindowText(pszZoomName,MAX_PATH);
	strZoomName.append(pszZoomName);
	return strZoomName;//lint !e1036
}//lint !e522

void CTranZoomTitleBar::SetZoomTitleName( const std::string &strDevName )
{
	m_StaticZoomName.SetWindowText(strDevName.c_str());
	UpdateView();
}

void CTranZoomTitleBar::SetBarPictrue(LPCTSTR lpszFile)
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


void CTranZoomTitleBar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
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
