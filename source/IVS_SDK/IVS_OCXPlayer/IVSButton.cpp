// GroupButton.cpp : implementation file
//

#include "stdafx.h"
#include "Utility.h"
#include "VideoPaneMgr.h"
#include "VideoPane.h"
#include "SDKConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CIVSButton

CIVSButton::CIVSButton()
	:ulButtonID(0)
	,m_pChildWnd(nullptr)
	,m_pParentWnd(nullptr)
	,m_bIsBordLine(FALSE)
	,m_bFontBold(FALSE)
	,m_bSubMenu(FALSE)
	,m_bCheckBox(FALSE)
	,m_CheckValue(FALSE)
	,m_bHold(FALSE)
	,m_bOver(FALSE)
	,m_bNormalMode(FALSE)
	,m_nModeIcon(1)
	,m_ShowMenuTime(0)
	,m_bTracking(FALSE)
	,m_hIcon(nullptr)
	,m_hBitmap(nullptr)
	,m_hBitmapOver(nullptr)
	,m_hBitmapDis(nullptr)
	,m_hCursor(nullptr)
	,m_bIsEnable(true)
{
	m_clBack = RGB(242,244,255);
	m_strTip.Empty();
}

CIVSButton::~CIVSButton()
{
	try{
		::DeleteObject(m_hBitmap);		
		::DeleteObject(m_hBitmapDis);	
		::DeleteObject(m_hBitmapOver);	
		::DestroyCursor(m_hCursor);
		IVS_DELETE(m_pChildWnd);	//lint !e1551
		IVS_DELETE(m_pParentWnd);	//lint !e1551
		::DestroyIcon(m_hIcon);	
	}
	catch(...)
	{

	}
} //lint !e1740

BEGIN_MESSAGE_MAP(CIVSButton, CButton)
	//{{AFX_MSG_MAP(CIVSButton)
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIVSButton message handlers

void CIVSButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}

void CIVSButton::SetButtonIcon(UINT nId)
{
	if(nId==0) return;
	m_hIcon = AfxGetApp()->LoadIcon(nId);
	//ASSERT(m_hIcon);
}

void CIVSButton::SetButtonIcon(const CString &strName)
{
	if(strName.IsEmpty()) return;
	m_hIcon = (HICON)::LoadImage(NULL, strName, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	m_hBitmap = NULL;
}

void CIVSButton::SetButtonBitmap(IVS_INT32 nNorId,IVS_INT32 nSelId,IVS_INT32 nDisId)
{
	if((nNorId==0)|| (nSelId==0)|| (nDisId==0))
	{
		return;
	}
	m_hBitmap = LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nNorId));
	m_hBitmapOver = LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nSelId));
	m_hBitmapDis = LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nDisId));
}

void CIVSButton::SetButtonBitmap( 
	const std::string &strNormalName, 
	const std::string &strSelectName,
	const std::string &strDisableName)
{
	if(strNormalName.empty()) return;
	if(strSelectName.empty()) return;
	if(strDisableName.empty()) return;

	m_hBitmap = (HBITMAP)::LoadImage(NULL, strNormalName.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	ASSERT(m_hBitmap);
	m_hBitmapOver = (HBITMAP)::LoadImage(NULL, strSelectName.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	ASSERT(m_hBitmapOver);
	m_hBitmapDis = (HBITMAP)::LoadImage(NULL, strDisableName.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	ASSERT(m_hBitmapDis);

	m_hIcon = NULL;
}

void CIVSButton::SetButtonCursor(UINT nId)
{
	if(nId==0) return;
	m_hCursor = AfxGetApp()->LoadCursor(nId);
	ASSERT(m_hCursor);
}

// 绘制菜单项
void CIVSButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(m_bNormalMode)
	{	
		DrawItemNormalButton(lpDrawItemStruct);
		return;
	}
	CRect rect =  lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CHECK_POINTER_VOID(pDC);
	UINT state = lpDrawItemStruct->itemState;
	TCHAR strText[MAX_PATH + 1];
	::GetWindowText(m_hWnd, strText, MAX_PATH);

	CBitmap btp;
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CRect rcParent;
	GetWindowRect(&rcParent);

	// 绘制分隔条
	if(m_bIsBordLine) { 
		DrawLineHeaderOrTrail(lpDrawItemStruct, memDC, pDC);

	} else {
		std::string strMidlPath;
		(void)SkinMgr.ToolBarPopMenuMidl(strMidlPath);
		HANDLE hBitMap = ::LoadImage(0, strMidlPath.c_str(),IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		CHECK_POINTER_VOID(hBitMap);
		btp.Attach((HBITMAP)hBitMap);
	

		CBitmap* pOld = memDC.SelectObject(&btp);
		CHECK_POINTER_VOID(pOld);
		BITMAP bm = {0};
		btp.GetObject(sizeof(BITMAP),&bm);

		CRect rectItem;
		rectItem.CopyRect(&lpDrawItemStruct->rcItem);

		pDC->StretchBlt(rectItem.left, rectItem.top, rectItem.Width(), rectItem.Height(),&memDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY/*MERGECOPY*/);
		memDC.SelectObject(pOld);
		::DeleteObject(hBitMap);
	}

	if(isHover(state))
	{
		if (m_bIsEnable)
		{
			std::string strHoverPath;
			(void)SkinMgr.ToolBarPopMenuHover(strHoverPath);

			CBitmap btpOver;
			HANDLE hBitMap = ::LoadImage(0, strHoverPath.c_str(),IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			CHECK_POINTER_VOID(hBitMap);
			btpOver.Attach((HBITMAP)hBitMap);

			CBitmap* pOld = memDC.SelectObject(&btpOver);
			CHECK_POINTER_VOID(pOld);
			BITMAP bm;
			btp.GetObject(sizeof(BITMAP),&bm);

			CRect _rect;
			_rect.CopyRect(&lpDrawItemStruct->rcItem);
			pDC->BitBlt(_rect.left + 3 , _rect.top, _rect.Width(), _rect.Height(), &memDC, 0, 0, SRCCOPY);

			memDC.SelectObject(pOld);
			::DeleteObject(hBitMap);
		}
		
	}

	//画图标
	if(isRequireProtractIcon())
	{
		CPoint pt(BUTTON_MAGIN, 2);
		::DrawIconEx(pDC->m_hDC, pt.x, pt.y, m_hIcon, BTNICON_WIDTH*m_nModeIcon, BTNICON_HEIGHT*m_nModeIcon, 0, NULL, DI_IMAGE|DI_MASK);
	} else {
		CPoint pt(1,0);
		CSize sz(rect.Width(),rect.Height());
		if (state & ODS_DISABLED)
			pDC->DrawState(	pt, sz, m_hBitmap, DSS_DISABLED, (HBRUSH)NULL);
		else if (m_bOver){
			pDC->DrawState(	pt, sz, m_hBitmapOver, DSS_NORMAL, (HBRUSH)NULL);
		} else{
			pDC->DrawState(	pt, sz, m_hBitmap, DSS_NORMAL, (HBRUSH)NULL);
		}

		if (!m_bIsEnable)
		{
			pDC->DrawState(	pt, sz, m_hBitmap, DSS_NORMAL, (HBRUSH)NULL);
		}
	}

	//写文字
	if (isRequireProtractCharacter(strText))
	{
		// 字体可配置
		(void)CSDKConfig::instance().LoadConfigFile();
		std::string strFontFamily = CSDKConfig::instance().GetFontFamilyValue();
		if (strFontFamily.empty())
		{
			strFontFamily = "宋体";
		}
		// 使用9号字体并将宽度设置为0 使得自适应
		int nHeight = -MulDiv(9, GetDeviceCaps(*pDC, LOGPIXELSY),72);  

		CFont font;
		font.CreateFont(nHeight,0, 0, 0,(m_bFontBold ? FW_BOLD :0), FALSE,0,0,0,0,0,0,0,strFontFamily.c_str());
		CFont *pOldFont = (CFont*)pDC->SelectObject(&font);
		CSize szExtent = pDC->GetTextExtent(strText, lstrlen(strText));
		CPoint pt(BTNICON_WIDTH*m_nModeIcon + 2*BUTTON_MAGIN, (rect.CenterPoint().y - szExtent.cy / 2));

		COLORREF crBak;
		crBak = pDC->SetTextColor(RGB(0,0,0));

		int nMode = pDC->SetBkMode(TRANSPARENT);
		if (state & ODS_DISABLED)
			pDC->DrawState(pt, szExtent, strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		else
			pDC->DrawState(pt, szExtent, strText, DSS_NORMAL, TRUE, 0, (HBRUSH)NULL);

		pDC->SetTextColor(crBak);
		pDC->SelectObject(pOldFont);
		pDC->SetBkMode(nMode);	
	}
	(void)memDC.DeleteDC();
}

bool CIVSButton::isRequireProtractIcon()const
{
	return (m_hIcon != nullptr) && (!m_bIsBordLine) ;
}

bool CIVSButton::isRequireProtractCharacter(const TCHAR *strText)const
{
	return (strText!= nullptr ) && (!m_bIsBordLine) ;
}

bool CIVSButton::isHover(UINT state)const
{
	return (m_bOver || m_bHold || (GetFocus()==this))&&(!m_bIsBordLine)&&(!( state & ODS_DISABLED)) ;
}

void CIVSButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
		if(m_bSubMenu)
			SetTimer(0,1000,NULL);
	}

	CButton::OnMouseMove(nFlags, point);
}

LRESULT CIVSButton::OnMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{	
	m_bOver = FALSE;
	m_bTracking = FALSE;
	Invalidate();
	if(m_bSubMenu)
	{
		CHECK_POINTER(m_pChildWnd,0);
		KillTimer(0);
		CRect rtWnd;
		m_pChildWnd->GetWindowRect(rtWnd);
		CPoint point;
		::GetCursorPos(&point);
		if(!rtWnd.PtInRect(point))
		{
			//m_pChildWnd->ShowWindow(FALSE);
			CIVSMenu* pMenu;
			pMenu = dynamic_cast<CIVSMenu*>(m_pChildWnd);
			CHECK_POINTER(pMenu,0);
			pMenu ->UnLoadChildMenu();
			m_bHold = FALSE;
		} else {
			if(m_pChildWnd->IsWindowVisible())
			{
				m_bHold = TRUE;
			} else {
				m_bHold = FALSE;
			}
		}
	}

	return 0;
}

LRESULT CIVSButton::OnMouseHover(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_bOver = TRUE;
	Invalidate();
	return 0;
}

BOOL CIVSButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CIVSButton::PreTranslateMessage(MSG* pMsg) 
{
	if (m_ToolTip.GetSafeHwnd())
	{
		m_ToolTip.RelayEvent( pMsg );
	} else {
		TCHAR szText[MAX_PATH + 1];
		::GetWindowText(m_hWnd, szText, MAX_PATH);
		m_ToolTip.Create(this);
		m_ToolTip.AddTool(this, m_strTip.IsEmpty() ? szText : m_strTip);
		m_ToolTip.Activate(TRUE);
	}

	return CButton::PreTranslateMessage(pMsg);
}

BOOL CIVSButton::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
} //lint !e818 !e1762  MFC自动生成的的函数

void CIVSButton::DrawParentBack( CDC* pDC, CWnd *pParent, const CRect &rcClient, CRect rcParent )
{
	CClientDC ParentDC(pParent);
	pParent->ScreenToClient(rcParent);
	if (m_dcParent.m_hDC == NULL)
	{
		m_dcParent.CreateCompatibleDC(&ParentDC);
		m_bmParent.CreateCompatibleBitmap(&ParentDC, rcClient.Width(), rcClient.Height());
		m_dcParent.SelectObject(&m_bmParent);
		m_dcParent.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &ParentDC, rcParent.left, rcParent.top, SRCCOPY);
	}

	pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_dcParent, 0, 0, SRCCOPY);
}

void CIVSButton::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CButton::OnShowWindow(bShow, nStatus);

	//加入这段代码的目的：当窗体隐藏的时候释放保存的父窗体背景可以在出现窗背景获取不正常的
	//情况下修复父窗体背景还可以在其他一些消息中释放这些资源，更加可以保证父窗体背景的正确性
	if (m_dcParent.m_hDC)
	{
		m_dcParent.DeleteDC();
		m_bmParent.DeleteObject();
	}
}

void CIVSButton::UpdateButton()
{
	if (m_dcParent.m_hDC)
	{
		m_dcParent.DeleteDC();
		m_bmParent.DeleteObject();
	}
	Invalidate();
}

void CIVSButton::SetButtonSubMenu(BOOL bSubMenu)
{
	m_bSubMenu = bSubMenu;
}

void CIVSButton::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case 0:
		TimerIDEventEQZero();
		break;
	default:
		break;
	}
	CButton::OnTimer(nIDEvent);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM)
{
	if(hwnd!=NULL)
	{
		TCHAR ClassName[MAX_PATH];
		GetClassName(hwnd, ClassName, sizeof(ClassName));
		if(strcmp(ClassName,MENU_CLASSNAME)==0)
		{
			ShowWindow(hwnd,FALSE);
			CIVSMenu * pMenu = NULL;
			pMenu = dynamic_cast<CIVSMenu*>(CWnd::FromHandle(hwnd));
			CHECK_POINTER(pMenu,FALSE);
			pMenu->ClearButtonData();
		}
		return TRUE;
	}
	return FALSE;
}
void CIVSButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bIsBordLine)
		return;
	if (NULL != m_pChildWnd)
	{
		if((m_bSubMenu) && (m_pChildWnd->GetSafeHwnd()))
		{
			ShowSubMenuWindow(m_pChildWnd);
			return;
		}
	}
	
	if(!m_bSubMenu)
	{	
		//处理本窗口及其它窗口
		EnumWindows(EnumWindowsProc,0);
	}
	if (m_bIsEnable)
	{
		if (NULL != CUtility::GetVideoPaneMgr()->GetActiveWndPointer()
			&& NULL != CUtility::GetVideoPaneMgr()->GetActiveWndPointer()->GetVideoRealtimeBar())
		{
			::SendMessage(CUtility::GetVideoPaneMgr()->GetActiveWndPointer()->GetVideoRealtimeBar()->GetSafeHwnd(),IDC_MENU,GetUlButtonID(),NULL);
		}
	}
	CButton::OnLButtonDown(nFlags, point);
}

void CIVSButton::ShowSubMenuWindow(CWnd* pSubMenuWnd)
{
	if(pSubMenuWnd->GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rtWnd;
	GetWindowRect(rtWnd);
	CRect rtClient;
	pSubMenuWnd->GetClientRect(rtClient);

	if(((rtWnd.right + rtClient.Width()) <= GetSystemMetrics(SM_CXSCREEN))
		&& ((rtWnd.bottom + rtClient.Height()) <= GetSystemMetrics(SM_CYSCREEN)))
	{
		pSubMenuWnd->SetWindowPos(&wndTopMost, rtWnd.right,rtWnd.top,rtClient.Width(),rtClient.Height(),SWP_SHOWWINDOW);
	} else {
		if(((rtWnd.right + rtClient.Width()) > GetSystemMetrics(SM_CXSCREEN))
			&& ((rtWnd.bottom + rtClient.Height()) <= GetSystemMetrics(SM_CYSCREEN)))
		{
			//仅超出右屏
			pSubMenuWnd->SetWindowPos(&wndTopMost, rtWnd.left - rtClient.Width() ,rtWnd.top,rtClient.Width(),rtClient.Height(),SWP_SHOWWINDOW);
		} else {
			if(((rtWnd.right + rtClient.Width()) <= GetSystemMetrics(SM_CXSCREEN))
				&& ((rtWnd.bottom + rtClient.Height()) > GetSystemMetrics(SM_CYSCREEN)))
			{
				//仅超出底屏
				int nYOffset;
				nYOffset = ((rtWnd.top + rtClient.Height()) - GetSystemMetrics(SM_CYSCREEN)) + 10;
				pSubMenuWnd->SetWindowPos(&wndTopMost, rtWnd.right,rtWnd.top - nYOffset,rtClient.Width(),rtClient.Height(),SWP_SHOWWINDOW);
			} else {
				//超出右和底屏
				int nYOffset;
				nYOffset = ((rtWnd.top + rtClient.Height()) - GetSystemMetrics(SM_CYSCREEN)) + 10;
				pSubMenuWnd->SetWindowPos(&wndTopMost, 
					rtWnd.left - rtClient.Width(),
					rtWnd.top - nYOffset,
					rtClient.Width(),
					rtClient.Height(),
					SWP_SHOWWINDOW);
			}
		}
	}

	pSubMenuWnd->ShowWindow(TRUE);
	CIVSMenu *pMenu = dynamic_cast<CIVSMenu*>(pSubMenuWnd);
	CHECK_POINTER_VOID(pMenu);
	pMenu->m_pParent = GetParent();
}//lint !e1762
/////////////////////////////////////////////////////
//设置图标大小模式：1:16 X 16  2:32X32  3:48X48
void CIVSButton::SetButtonIconMode(int nModeIcon)
{
	if(nModeIcon <= 0 || nModeIcon >3)
		return;
	m_nModeIcon  = nModeIcon;
}
/////////////////////////////////////////////////////
//设置菜单项是否为Check菜单子项
void CIVSButton::SetButtonCheckMenu(BOOL bCheckBox)
{
	m_bCheckBox = bCheckBox;
}

void CIVSButton::OnKillFocus(CWnd* pNewWnd) 
{
	CHECK_POINTER_VOID(m_pChildWnd);
	CButton::OnKillFocus(pNewWnd);
	if(m_bSubMenu && m_pChildWnd->GetSafeHwnd())
	{
		if(m_pChildWnd->IsWindowVisible())
		{
			//失去焦点进入子菜单
			return;
		}
		m_bHold = FALSE;
	} else {
		m_bHold = FALSE;
	}
	m_bOver = FALSE;
}

void CIVSButton::OnSetFocus(CWnd* pOldWnd) 
{
	m_bHold = TRUE;	
	CButton::OnSetFocus(pOldWnd);
}
void CIVSButton::SetBackColor(COLORREF clBack)
{
	m_clBack = clBack;
}


BOOL CIVSButton::ODSDISABLED(UINT state)const
{
	return static_cast<BOOL>(state & ODS_DISABLED);
}

bool CIVSButton::isHover()const
{
	return (m_bOver || m_bHold || (GetFocus()==this));
}

void CIVSButton::DrawItemNormalButton(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{

	CRect rect(lpDrawItemStruct->rcItem);

	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CHECK_POINTER_VOID(pDC);
	UINT state = lpDrawItemStruct->itemState;
	TCHAR strText[MAX_PATH + 1];
	::GetWindowText(m_hWnd, strText, MAX_PATH);

	//创建内存DC
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);

	CBitmap btScreen;
	btScreen.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	MemDC.SelectObject(&btScreen);

	CRect rcParent;
	GetWindowRect(&rcParent);
	DrawParentBack(&MemDC, GetParent(), rect, rcParent);

	//画图标
	if (m_hIcon)
	{
		CPoint pt(3, MARGIN/2);
		::DrawIconEx(MemDC.m_hDC, pt.x, pt.y, m_hIcon, 32, 32, 0, NULL, DI_IMAGE|DI_MASK);
	} else {
		CPoint pt(0, 0);
		CSize sz(rect.Width(), rect.Height());
		if (ODSDISABLED(state))
			MemDC.DrawState(pt,sz, m_hBitmap,DSS_DISABLED, (HBRUSH)NULL);
		else
			MemDC.DrawState(pt,sz, (m_bOver)?m_hBitmapOver:m_hBitmap, DSS_NORMAL, (HBRUSH)NULL);
	}

	if(m_hBitmap==NULL)
	{
		if(isHover())
		{	
			// mouse On 的颜色		
			CBrush brush(RGB(120,100,200));
			MemDC.FillRect(rect,&brush);
		} else {	
			CBrush brush(m_clBack);
			MemDC.FillRect(rect, &brush);
		}
	}

	//写文字
    if (0 != strlen(strText))
	//if (strText!= nullptr)
	{
		// 字体可配置
		(void)CSDKConfig::instance().LoadConfigFile();
		std::string strFontFamily = CSDKConfig::instance().GetFontFamilyValue();
		if (strFontFamily.empty())
		{
			strFontFamily = "宋体";
		}

		// 使用9号字体并将宽度设置为0 使得自适应
		int nHeight = -MulDiv(9, GetDeviceCaps(*pDC, LOGPIXELSY),72);

		CFont font;
		font.CreateFont(nHeight,0, 0, 0,(m_bFontBold ? FW_BOLD :0), FALSE,0,0,0,0,0,0,0,strFontFamily.c_str());
		CFont *pOldFont = (CFont*)pDC->SelectObject(&font);
		CSize szExtent = MemDC.GetTextExtent(strText, lstrlen(strText));
		CPoint pt(rect.Width() + 2*BUTTON_MAGIN, (rect.CenterPoint().y - szExtent.cy /2 )+1);
		//if (state & ODS_SELECTED) 
		//	pt.Offset(1, 1);
		COLORREF crBak;

		crBak = SetTextColor( MemDC);

		int nMode = MemDC.SetBkMode(TRANSPARENT);
		if (ODSDISABLED(state))
		{
			MemDC.DrawState(pt, szExtent, strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		} else {
			pDC->DrawState(pt, szExtent, strText, DSS_NORMAL, TRUE, 0, (HBRUSH)NULL);
		}

		MemDC.SetTextColor(crBak);
		MemDC.SelectObject(pOldFont);
		MemDC.SetBkMode(nMode);
	}
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);

	btScreen.DeleteObject();
	MemDC.DeleteDC();
} //lint !e818  MFC自动生成的的函数

void CIVSButton::SetButtonNormlMode(BOOL bMode)
{
	m_bNormalMode = bMode;
}

void CIVSButton::DrawLineHeaderOrTrail( LPDRAWITEMSTRUCT lpDrawItemStruct, CDC &memDC, CDC * pDC )const
{
	std::string strLinePath;
	CBitmap btpHeaderOrTrail;
	//绘制顶端圆角
	if (lpDrawItemStruct->CtlID == ID_MENU_HEADER) 
	{
		(void)SkinMgr.ToolBarPopMenuHead(strLinePath);
	} else if (lpDrawItemStruct->CtlID == ID_MENU_TRAIL){//绘制底部圆角
		(void)SkinMgr.ToolBarPopMenuTail(strLinePath);
		
	}

	HANDLE hBitMap = ::LoadImage(0, strLinePath.c_str(),IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	CHECK_POINTER_VOID(hBitMap);
	if(!btpHeaderOrTrail.Attach((HBITMAP)hBitMap))
	{
		::DeleteObject(hBitMap);
		return;
	}

	CBitmap* pOld = memDC.SelectObject(&btpHeaderOrTrail);
	CHECK_POINTER_VOID(pOld);
	BITMAP bm = {0};
	btpHeaderOrTrail.GetObject(sizeof(BITMAP),&bm);
	CRect rectLine;
	rectLine.CopyRect(&lpDrawItemStruct->rcItem);
	pDC->StretchBlt(rectLine.left, rectLine.top, rectLine.Width(), rectLine.Height(),&memDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);

	memDC.SelectObject(pOld);
	::DeleteObject(hBitMap);
} //lint !e818  Windows API

COLORREF CIVSButton::SetTextColor( CDC &MemDC )
{
	COLORREF crBak;
	if (isHover())
	{
		crBak = MemDC.SetTextColor(BTN_OVER_COL);
	} else {
		crBak = MemDC.SetTextColor(RGB(0,0,0));
	}	
	
	return crBak;
}//lint !e1762

void CIVSButton::TimerIDEventEQZero()
{
	if(m_bOver && m_bSubMenu)
	{
		m_ShowMenuTime++;
		if(m_ShowMenuTime >= 3)
		{
			this->ShowSubMenuWindow(m_pChildWnd);
		}
	} else {
		m_ShowMenuTime = 0;
		if(m_pChildWnd)
			m_pChildWnd->ShowWindow(FALSE);
	}
}
