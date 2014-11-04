// VideoSoundCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ZoomSliderCtrl.h"
#include "IVS_OCXPlayerCtrl.h"
#include "VideoPaneMgr.h"
#include "afxdialogex.h"

// CVideoSoundCtrl dialog

IMPLEMENT_DYNAMIC(CZoomSliderCtrl, CDialog)

CZoomSliderCtrl::CZoomSliderCtrl(CWnd* pParent /*=NULL*/)
    : CDialog(CZoomSliderCtrl::IDD, pParent)
    , m_ulSliderPosition(MAGBIFYCTRL_SLIDER_MIN)
{
}

CZoomSliderCtrl::~CZoomSliderCtrl()
{
	m_ulSliderPosition = MAGBIFYCTRL_SLIDER_MIN;
} //lint !e1540  MFC函数

void CZoomSliderCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_MAGBIFYCTRL, m_sliderMagbifyCtrl);
}

BEGIN_MESSAGE_MAP(CZoomSliderCtrl, CDialog)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE((WM_BITMAPSLIDER_MOVED), &CZoomSliderCtrl::OnBitmapSliderMoved)
END_MESSAGE_MAP()

// CVideoSoundCtrl message handlers
BOOL CZoomSliderCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_sliderMagbifyCtrl.SetVertical(FALSE);
	m_sliderMagbifyCtrl.SetPos((IVS_INT32)m_ulSliderPosition);
	m_sliderMagbifyCtrl.SetRange(MAGBIFYCTRL_SLIDER_MIN, MAGBIFYCTRL_SLIDER_MAX);
	m_sliderMagbifyCtrl.SetTicFreq(MAGBIFYCTRL_SLIDER_INTERVAL);
	
//     DWORD dwExStyle = static_cast<DWORD>(::GetWindowLong(m_hWnd, GWL_EXSTYLE));
//     VERIFY((dwExStyle & WS_EX_APPWINDOW) == 0);
//     ::SetWindowLong(m_hWnd, GWL_EXSTYLE, static_cast<long >(dwExStyle | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE ));

	COLORREF maskColor = RGB(255,255,255);
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	HINSTANCE hInst=LoadLibrary(_T("User32.DLL"));
	if(hInst)
	{
		typedef BOOL (WINAPI * MYFUNC)(HWND , COLORREF,BYTE,DWORD);
		MYFUNC fun = (MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes");
		if(fun)
		{
			(void)fun(this->GetSafeHwnd(),maskColor,255,1);
		}
			
		FreeLibrary(hInst);
	}

	return TRUE;
}

void CZoomSliderCtrl::OnPaint()
{   
	CPaintDC dc(this); // device context for painting

	CRect rect;
	this->GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(0,0,0));

 	HBITMAP hBitmapChannel = m_sliderMagbifyCtrl.StretchImage(IDB_BITMAP_MAGBIGY_SLIDER_CHANNEL, rect.Width(), 9);
 	(void)m_sliderMagbifyCtrl.SetBitmapChannelEx(hBitmapChannel, NULL);
 	(void)m_sliderMagbifyCtrl.SetBitmapThumb(IDB_BITMAP_MAGBIFY_SIGN_NORMAL, IDB_BITMAP_MAGBIFY_SIGN_CLICK);
	
	m_sliderMagbifyCtrl.DrawFocusRect(FALSE);
	m_sliderMagbifyCtrl.MoveWindow(rect.left, rect.top, rect.Width(), 9);
	m_sliderMagbifyCtrl.SetPos(static_cast<int>(m_ulSliderPosition));

	return;
}

int CZoomSliderCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	return IVS_SUCCEED;
}

// 进度条移动事件
LRESULT CZoomSliderCtrl::OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam)
{
	(void)AFX_MANAGE_STATE(AfxGetStaticModuleState());
	(void)wParam;
	(void)lParam;

	if (wParam == IDC_SLIDER_MAGBIFYCTRL)
	{
		ProcessSliderMoved();
	}

	return 0;
}

// 播放进度滑动条滑动事件
void CZoomSliderCtrl::ProcessSliderMoved()
{
	(void)AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// 滑动条手动滑动
	m_ulSliderPosition = (ULONG) m_sliderMagbifyCtrl.GetPos();
	// 更新数字
	CTranZoomBar* pParent = dynamic_cast<CTranZoomBar*>(GetParent());
	CHECK_POINTER_VOID(pParent);
	pParent->SetMagnifyRate((IVS_INT32)m_ulSliderPosition);
	pParent->Invalidate(TRUE);

	// 将百分比跟对象传递出去
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(pParent->GetVideoPane());
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPane *pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
	CHECK_POINTER_VOID(pSrcVideoPane);
	::PostMessage(pSrcVideoPane->GetSafeHwnd(),WM_MAGNIFY_RATE_CHANGE, m_ulSliderPosition, (int)pVideoPane);
	return;
}

void CZoomSliderCtrl::IncSliderPosition()
{ 
	if (m_ulSliderPosition + MAGBIFYCTRL_CLICK_INTERVAL >= MAGBIFYCTRL_SLIDER_MAX)
	{
		m_ulSliderPosition = MAGBIFYCTRL_SLIDER_MAX;
		return;
	}
	m_ulSliderPosition += MAGBIFYCTRL_CLICK_INTERVAL; 
}

void CZoomSliderCtrl::DecSliderPosition()
{ 
	if (m_ulSliderPosition - MAGBIFYCTRL_CLICK_INTERVAL <= MAGBIFYCTRL_SLIDER_MIN)
	{
		m_ulSliderPosition = MAGBIFYCTRL_SLIDER_MIN;
		return;
	}
	m_ulSliderPosition -= MAGBIFYCTRL_CLICK_INTERVAL; 
}

void CZoomSliderCtrl::SetSliderPosition(ULONG ulPos)
{ 
	if (ulPos >= MAGBIFYCTRL_SLIDER_MAX)
	{
		ulPos = MAGBIFYCTRL_SLIDER_MAX;
	}

	if (ulPos <= MAGBIFYCTRL_SLIDER_MIN)
	{
		ulPos = MAGBIFYCTRL_SLIDER_MIN;
	}

	m_ulSliderPosition = ulPos; 
}

ULONG CZoomSliderCtrl::GetSliderPosition()const
{ 
	return m_ulSliderPosition; 
}

BOOL CZoomSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);

	pDC->FillSolidRect(rect,COLOR_VIDEOPANE_BKG);

	return TRUE;
}//lint !e1762

