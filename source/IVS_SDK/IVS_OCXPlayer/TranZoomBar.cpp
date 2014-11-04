// TranZoomBar.cpp : implementation file
//

#include "stdafx.h"
#include "IVS_OCXPlayerCtrl.h"
#include "TranZoomBar.h"
#include "afxdialogex.h"
#include "TranslucentUtility.h"

// CTranZoomBar dialog

IMPLEMENT_DYNAMIC(CTranZoomBar, CTranslucentDialog)

CTranZoomBar::CTranZoomBar(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CTranZoomBar::IDD, lpszFile, pParent)
{
	m_iMagnifyRate = 100;
}

CTranZoomBar::~CTranZoomBar()
{
}

void CTranZoomBar::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAGNIFY_TEXT, m_MagnifyRateText);
	DDX_Control(pDX, IDC_BTN_SHRINK, m_btnShrink);
	DDX_Control(pDX, IDC_BTN_MAGNIFY, m_btnMagnify);
	DDX_Control(pDX, IDC_BTN_SNAPSHOT, m_btnSnapShot);
}


BEGIN_MESSAGE_MAP(CTranZoomBar, CTranslucentDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_SNAPSHOT, &CTranZoomBar::OnBnClickedButtonSnapshot)
	ON_BN_CLICKED(IDC_BTN_SHRINK, &CTranZoomBar::OnBnClickedButtonShrink)
	ON_BN_CLICKED(IDC_BTN_MAGNIFY, &CTranZoomBar::OnBnClickedButtonMagnify)
END_MESSAGE_MAP()//lint !e751


// CTranZoomBar message handlers

BOOL CTranZoomBar::OnInitDialog()
{
	CTranslucentDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	// 初始化滑动条
	m_ZoomSliderCtrl.Create(IDD_DLG_MAGNIFYCTRL,this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTranZoomBar::OnPaint()
{
 	CPaintDC dc(this); // device context for painting
 	// TODO: Add your message handler code here
 	// Do not call CTranslucentDialog::OnPaint() for painting messages
 	//TODO 去掉上下两条的宽度和计算不是全屏时的放大
 	CRect rect;
 	GetClientRect(rect);
 
 	int iWidth = rect.Width();
 	//int iHeight = rect.Height();

	m_MagnifyRateText.MoveWindow(rect.left + 10,rect.top + 1,69, 22);
	
	m_MagnifyRateText.SetWindowText(CToolsHelp::Int2Str(m_iMagnifyRate).c_str());
	m_btnShrink.MoveWindow(rect.left + 89, rect.top + 1, 22, 22);
	m_btnMagnify.MoveWindow(rect.right - 111, rect.top + 1, 22, 22);
	m_btnSnapShot.MoveWindow(rect.right - 32, rect.top + 1, 22, 22);
 	UpdateView();

	this->ClientToScreen(&rect);
	m_ZoomSliderCtrl.MoveWindow(rect.left + 111,rect.top+7, iWidth - 222, 9);
	m_ZoomSliderCtrl.ShowWindow(SW_SHOW);

	if ((IVS_UINT32)m_iMagnifyRate >= MAX_REAT_PERCETN)
	{
		m_btnMagnify.EnableWindow(FALSE);
	}
	else
	{
		m_btnMagnify.EnableWindow(TRUE);
	}
	
	if ((IVS_UINT32)m_iMagnifyRate <= MIN_REAT_PERCETN)
	{
		m_btnShrink.EnableWindow(FALSE);
	}
	else
	{
		m_btnShrink.EnableWindow(TRUE);
	}
}
void CTranZoomBar::OnInitChildrenWnds()
{
	SetButtonPicture(m_btnShrink, "ZoomOut");
	SetButtonPicture(m_btnMagnify, "ZoomIn");
	SetButtonPicture(m_btnSnapShot, "SnapShot");
	SetStaticPicture(m_MagnifyRateText, "ReatText");

	if (1 == SkinMgr.GetSkinType())
	{
		SetRateTextColor(0xffffffff);
	}
	else
	{
		SetRateTextColor(0xff000000);
	}
}

// TODO 这里的图片到时改成一张
void CTranZoomBar::SetStaticPicture( CTranslucentStatic &tStatic,const std::string &strButtonName )
{
	CHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]={0x0};

	memset(pcImageList,0x0,256*5);
	if (strButtonName.empty())
	{
		return;
	}
	(void)SkinMgr.DigtalPtzGetPNGButtonByName(strButtonName,pcImageList);
	// 根据路径获取一个GID IMAGE
	TCHAR wsFile[MAX_PATH] = {0};

	memset(wsFile,0x0,MAX_PATH);
	(void)CToolsHelp::Strncpy(wsFile,MAX_PATH,pcImageList[0],strnlen(pcImageList[0],MAX_PATH));
	
	WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
	tStatic.SetCurrImage(Gdiplus::Image::FromFile(wstr));
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	

	RegisterTranslucentWnd(&tStatic);
}

void CTranZoomBar::SetButtonPicture( CTranslucentButton &tButton,const std::string &strButtonName )
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

void CTranZoomBar::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}

// 抓拍
void CTranZoomBar::OnBnClickedButtonSnapshot()
{
	SetCurrentPaneToActivePane();

	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
// 	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_SNAPSHOT, pVideoPane->GetPaneID());

	(void)pCtrl->IVS_OCX_LocalSnapshot((LONG)(pVideoPane->GetPaneID()));
}

// 缩小按钮
void CTranZoomBar::OnBnClickedButtonShrink()
{
 	SetCurrentPaneToActivePane();
 	// 减少滑动条位置
 	m_ZoomSliderCtrl.DecSliderPosition();
 	m_iMagnifyRate = (IVS_INT32)m_ZoomSliderCtrl.GetSliderPosition();
 	Invalidate(TRUE);
 	m_ZoomSliderCtrl.Invalidate(TRUE);
 
 	// 将百分比跟对象传递出去
 	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
 	CHECK_POINTER_VOID(pVideoPane);
 	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
 	CHECK_POINTER_VOID(pSrcVideoPane);
 	::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RATE_CHANGE, (IVS_UINT32)m_iMagnifyRate, (int)pVideoPane);
}

// 放大按钮
void CTranZoomBar::OnBnClickedButtonMagnify()
{
 	SetCurrentPaneToActivePane();
 	// 增加滑动条位置
 	m_ZoomSliderCtrl.IncSliderPosition();
 	m_iMagnifyRate = (IVS_INT32)m_ZoomSliderCtrl.GetSliderPosition();
 	Invalidate(TRUE);
 	m_ZoomSliderCtrl.Invalidate(TRUE);
 
 	// 将百分比跟对象传递出去
 	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
 	CHECK_POINTER_VOID(pVideoPane);
 	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
 	CHECK_POINTER_VOID(pSrcVideoPane);
 	::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RATE_CHANGE, (IVS_UINT32)m_iMagnifyRate, (int)pVideoPane);
}

void CTranZoomBar::SetMagnifyRate(IVS_INT32 iRate)
{ 
	if ((IVS_UINT32)iRate >= MAX_REAT_PERCETN)
	{
		iRate = MAX_REAT_PERCETN;
	}

	if ((IVS_UINT32)iRate <= MIN_REAT_PERCETN)
	{
		iRate = MIN_REAT_PERCETN;
	}

	// 更新文字
	m_iMagnifyRate = iRate; 
	// 更新滑动条
	m_ZoomSliderCtrl.SetSliderPosition((ULONG)iRate);
	Invalidate(TRUE);
	m_ZoomSliderCtrl.Invalidate(TRUE);
}


BOOL CTranZoomBar::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_SHOWWINDOW)
	{
		m_ZoomSliderCtrl.ShowWindow(SW_HIDE);
	}

	return CTranslucentDialog::OnWndMsg(message, wParam, lParam, pResult);
}


void CTranZoomBar::SetSnapShotEnableStatus(BOOL bEnable)
{
	m_btnSnapShot.EnableWindow(bEnable);
	m_btnSnapShot.Invalidate(TRUE);
	// 下面不调用这个函数，这个操作时间过长，如果马上删除，可能导致概率性崩溃
	//UpdateView();
}

void CTranZoomBar::SetBarPictrue(LPCTSTR lpszFile)
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

void CTranZoomBar::SetRateTextColor(ULONG ulColor)
{
	m_MagnifyRateText.SetColor(ulColor);
}
