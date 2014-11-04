#include "stdafx.h"
#include "IVSMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CIVSMenu
// 
BOOL  CALLBACK EnumWindowsProcB(HWND hwnd, LPARAM lparam)
{
	if(hwnd!=NULL)
	{
        TCHAR ClassName[MAX_PATH];
		(void)GetClassName(hwnd, ClassName, sizeof(ClassName));
		if(strcmp(ClassName,MENU_CLASSNAME)==0)
		{
			CIVSMenu *pMenu = NULL;
			pMenu = dynamic_cast<CIVSMenu*>(CWnd::FromHandle(hwnd));
			CHECK_POINTER(pMenu,FALSE);
			if(pMenu == (CIVSMenu *)lparam)
			{   
				//枚举到本窗口到忽略，并继续。
				return TRUE;
			}
			CPoint point;
			(void)::GetCursorPos(&point);
			CRect rtWnd;
			pMenu->GetWindowRect(rtWnd);
			if((rtWnd.PtInRect(point))&& pMenu->IsWindowVisible())
			{	
				//ReleaseCapture();
				CIVSMenu* pWnd = (CIVSMenu*)lparam ;
				pWnd ->ShowWindow(FALSE);
				pWnd ->ClearButtonData();
				pWnd ->m_bCapture = FALSE;
				pMenu->ClearButtonData();				
				pMenu->m_bCapture = FALSE;
				pMenu->Invalidate();
				return FALSE;
			}						
		}
		return TRUE;
	}
	return FALSE;
}

BOOL  CALLBACK EnumWindowsProcA(HWND hwnd, LPARAM)
{
	if(hwnd!=NULL)
	{
		TCHAR ClassName[MAX_PATH];
		(void)GetClassName(hwnd, ClassName, sizeof(ClassName));
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

//////////////////////////////////////////////////////////////////////////

CIVSMenu::CIVSMenu()
	:m_pParent(nullptr)
	,m_ButtonPos(-1)
	,m_bCapture(FALSE)
	,m_strTip("")
	,m_clBack(RGB(242,244,255))
	,m_crGradFill(RGB(0,0,0))
	,m_hCursor(nullptr)
	,m_bTransparentBk(TRUE)
	,m_nBordlineCount(0)
	,m_nModeIcon(1)
	,m_bShowHead(TRUE)
	,m_bMouseOn(FALSE)
	,m_bOver(FALSE)
	,m_bEraseBack(FALSE)
{
}

CIVSMenu::~CIVSMenu()
{
	POSITION pos = m_MenuData.BtnList.GetHeadPosition(); //lint !e1551
	MenuItemInfo *pBtnInfo = NULL;
	while(pos!=NULL)
	{
		pBtnInfo = (MenuItemInfo*)m_MenuData.BtnList.GetNext(pos); //lint !e1551
		IVS_DELETE(pBtnInfo); //lint !e1551
	}
	IVS_DELETE(m_pParent); //lint !e1551
	IVS_DELETE(m_hCursor); //lint !e1551
}

BEGIN_MESSAGE_MAP(CIVSMenu, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_MENUHEADER, OnChangeMenuStatus)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIVSMenu message handlers
BOOL CIVSMenu::RegisterWindowClass() const
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, MENU_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = NULL;
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = MENU_CLASSNAME;

		if (!AfxRegisterClass(&wndcls)) {
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CIVSMenu::CreateGroupMenu(const CWnd *pParent, const CRect &rcClient, UINT /*nID*/, BOOL bShowHead,int nModeIcon)
{
	if (!RegisterWindowClass())
		return FALSE;

	//是否创建头部
	m_bShowHead  = bShowHead;

	if (!CreateEx(0,MENU_CLASSNAME, "IVS_Memu", WS_POPUP,rcClient.left,rcClient.top,rcClient.right,rcClient.bottom, pParent->GetSafeHwnd(),0,NULL))
		return FALSE;

	m_nModeIcon = nModeIcon;
	//创建头部窗体

	if(m_bShowHead)
	{
		if (!m_MenuData.btnHeader.Create(m_MenuData.strTitle, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(0,0,0,0), this, IDC_MENUHEADER))
		{
			return FALSE;
		} else {
			m_MenuData.btnHeader.SetButtonIcon(m_MenuData.nIcon);
			m_MenuData.btnHeader.SetHeaderStatus(m_MenuData.bStatus);
		}
	}

	//创建子项目窗体
	MenuItemInfo *pBtnInfo=NULL;
	for(POSITION pos = m_MenuData.BtnList.GetHeadPosition(); pos!=NULL; )
	{
		pBtnInfo = m_MenuData.BtnList.GetNext(pos);
		if (pBtnInfo)
		{
			(void)pBtnInfo->btn.Create(pBtnInfo->strTitle, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(0,0,0,0), this, pBtnInfo->nBtnID);
			if (pBtnInfo->isCreatedResource){
				pBtnInfo->btn.SetButtonBitmap(pBtnInfo->nNorBitmap ,pBtnInfo->nSelBitmap,pBtnInfo->nDisBitmap);
			} else {
				pBtnInfo->btn.SetButtonBitmap( pBtnInfo->strNorBitmap,pBtnInfo->strSelBitmap,pBtnInfo->strDisBitmap);
			}
			
			pBtnInfo->btn.SetButtonSubMenu(pBtnInfo->bSubMenu);
			pBtnInfo->btn.SetUlButtonID(pBtnInfo->nBtnID);
			pBtnInfo->btn.SetButtonCheckMenu(pBtnInfo->bCheckBox);			
			pBtnInfo->btn.m_strTip = pBtnInfo->strTip;
			if(pBtnInfo->bSubMenu)
			{
				pBtnInfo->btn.m_pChildWnd = pBtnInfo->pChild;
			} else {
				pBtnInfo->btn.m_pChildWnd = NULL;
			}
			pBtnInfo->btn.m_bFontBold = pBtnInfo->bFontBold;
			pBtnInfo->btn.m_bIsBordLine = pBtnInfo->bIsBordLine;

			pBtnInfo->btn.SetButtonIconMode(m_nModeIcon);
			pBtnInfo->btn.EnableWindow();
		}
	}
	UpdateMenuStatus();
	SetMenuStatus();
	ShowWindow(FALSE);
	m_bEraseBack = TRUE;	
	return TRUE;
}

BOOL CIVSMenu::OnEraseBkgnd(CDC* pDC) 
{
	if(!m_bEraseBack) return FALSE;
	return CWnd::OnEraseBkgnd(pDC);
}

//打开和关闭组合框菜单时更新组合框菜单
void CIVSMenu::OnChangeMenuStatus()
{
	//	m_MenuData.bStatus = !m_MenuData.bStatus;
	//	m_MenuData.btnHeader.SetHeaderStatus(m_MenuData.bStatus);
	//	UpdateMenuStatus();
	//向父窗体送去窗体更新（打开或关闭）的消息
	//	GetParent()->SendMessage(WM_UPDATEPOS, (WPARAM)this);
} //lint !e1762  MFC自动生成的的函数

//让父窗体能够获取到子项的消息
BOOL CIVSMenu::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//当菜单发生变化时更新子项的背景
void CIVSMenu::UpdateMenu()
{
	if (m_MenuData.btnHeader.GetSafeHwnd())
		m_MenuData.btnHeader.Invalidate();
	MenuItemInfo *pBtnInfo = NULL;
	for(POSITION pos = m_MenuData.BtnList.GetHeadPosition(); pos!=NULL; )
	{
		pBtnInfo = (MenuItemInfo*)m_MenuData.BtnList.GetNext(pos);
		if (pBtnInfo&&pBtnInfo->btn.GetSafeHwnd())
		{
			pBtnInfo->btn.UpdateButton();
		}
	}
}

//设置当前组合框菜单的基本信息
void CIVSMenu::SetMenuInfo( const IVSMenu &gm)
{
	m_MenuData.bStatus = gm.bStatus;
	m_MenuData.nBitmap = gm.nBitmap;
	m_MenuData.nIcon = gm.nIcon;
	m_MenuData.strTitle = gm.strTitle;
}

//向当前组合框菜单中添加子项
void CIVSMenu::AddMenuItem(MenuItemInfo &bt)
{
	MenuItemInfo *pBtnInfo = new MenuItemInfo;
	pBtnInfo->isCreatedResource = bt.isCreatedResource;
	if (!bt.bIsBordLine)
	{	//非分界线
		pBtnInfo->nNorBitmap = bt.nNorBitmap;
		pBtnInfo->nSelBitmap= bt.nSelBitmap;
		pBtnInfo->nDisBitmap= bt.nDisBitmap;
		pBtnInfo->strNorBitmap.clear();
		pBtnInfo->strSelBitmap.clear();
		pBtnInfo->strDisBitmap.clear();
		(void)pBtnInfo->strNorBitmap.append(bt.strNorBitmap);
		(void)pBtnInfo->strSelBitmap.append(bt.strSelBitmap);
		(void)pBtnInfo->strDisBitmap.append(bt.strDisBitmap);
		pBtnInfo->nIcon = bt.nIcon;
		pBtnInfo->strTitle = bt.strTitle;
		pBtnInfo->nBtnID = bt.nBtnID;
		pBtnInfo->bSubMenu = bt.bSubMenu;
		pBtnInfo->pChild = bt.pChild;
		pBtnInfo->strTip = bt.strTip;
		pBtnInfo->bFontBold = bt.bFontBold;
		pBtnInfo->bIsBordLine = bt.bIsBordLine;
		pBtnInfo->bCheckBox = bt.bCheckBox;
		m_MenuData.BtnList.AddTail(pBtnInfo);
	} else {
		pBtnInfo->nNorBitmap = 0;
		pBtnInfo->nSelBitmap= 0;
		pBtnInfo->nDisBitmap= 0;
		pBtnInfo->strNorBitmap.clear();
		pBtnInfo->strSelBitmap.clear();
		pBtnInfo->strDisBitmap.clear();
		pBtnInfo->nIcon = 0;
		pBtnInfo->strTitle ="";
		pBtnInfo->nBtnID = bt.nBtnID;
		pBtnInfo->bSubMenu = FALSE;
		pBtnInfo->pChild = NULL;
		pBtnInfo->bFontBold = FALSE;
		pBtnInfo->bIsBordLine = bt.bIsBordLine;
		m_MenuData.BtnList.AddTail(pBtnInfo);
		pBtnInfo->bCheckBox = 0;
		m_nBordlineCount++;
	}
} //lint !e429

//**********************************************
// Method:    UpdateMenuStatus
// FullName:  CIVSMenu::UpdateMenuStatus
// Access:    protected 
// Returns:   void
// Qualifier: 更新Menu状态
// Author:    yangdazhao ykf75928
// created:	  22:11:2012   22:05
//**********************************************
void CIVSMenu::UpdateMenuStatus()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	//改变Menu header 位置
	if (m_MenuData.btnHeader.GetSafeHwnd())
	{
		m_MenuData.btnHeader.MoveWindow(CRect(0, 10, m_bShowHead ? rcClient.Width()-10:0, m_bShowHead ? MENUTITLE_HEIGHT:2), FALSE);
	}

	CRect rcPos(0, m_bShowHead ? MENUTITLE_HEIGHT:0, rcClient.right, (m_bShowHead ? MENUTITLE_HEIGHT:0) + BUTTON_HEIGHT*m_nModeIcon);

	MenuItemInfo *pBtnInfo = NULL;
	for(POSITION pos = m_MenuData.BtnList.GetHeadPosition(); pos!=NULL; )
	{
		pBtnInfo = (MenuItemInfo*)m_MenuData.BtnList.GetNext(pos);
		if (pBtnInfo&&pBtnInfo->btn.GetSafeHwnd())
		{
			if(pBtnInfo->btn.m_bIsBordLine)
			{
				rcPos.bottom = rcPos.top + 4;
			} else{
				rcPos.bottom = rcPos.top+ BUTTON_HEIGHT*m_nModeIcon;
			}
			pBtnInfo->btn.MoveWindow(rcPos ,FALSE);
			pBtnInfo->btn.ShowWindow(m_MenuData.bStatus ? SW_SHOW:SW_HIDE);

			rcPos.top = rcPos.bottom;
		}
	}

//	int nCount = m_MenuData.BtnList.GetCount();

	if (m_MenuData.bStatus)
	{
		(void)SetWindowPos(&wndTopMost, 0, 0,  rcClient.Width(), rcPos.bottom, SWP_NOMOVE|SWP_SHOWWINDOW);
	} else {
		(void)SetWindowPos(&wndTopMost, 0, 0,  rcClient.Width(), MENUTITLE_HEIGHT, SWP_NOMOVE|SWP_SHOWWINDOW);
	}
}

void CIVSMenu::EnableMenuStatus(UINT ulMenuPos,BOOL bStatus)
{
	MenuItemInfo *pBtnInfo = NULL;
	//int nPos = 0;
	for(POSITION pos = m_MenuData.BtnList.GetHeadPosition(); pos!=NULL; )
	{
		pBtnInfo = (MenuItemInfo*)m_MenuData.BtnList.GetNext(pos);
		//找到对应的项后对其状态进行设置
		if (pBtnInfo&&pBtnInfo->btn.GetSafeHwnd()&&(pBtnInfo->nBtnID == ulMenuPos))
		{
			pBtnInfo->btn.EnableButtonStatus(static_cast<bool>(bStatus));
			//pBtnInfo->btn.EnableWindow(bStatus);
			//UpdateMenu();
			break;
		}
		//nPos++;
	}
}

int CIVSMenu::MenuHeight() const
{
	int nHeight = 0;
	if (m_MenuData.bStatus)
	{
		int nCount = m_MenuData.BtnList.GetCount();
		nHeight = (m_bShowHead ? MENUTITLE_HEIGHT:0) + nCount*(BUTTON_HEIGHT*m_nModeIcon)/* + 1*/;
		nHeight = nHeight - m_nBordlineCount*(BUTTON_HEIGHT*m_nModeIcon*5/6)/* + 10*/;
	}
	else
		nHeight = MENUTITLE_HEIGHT;

	return nHeight;
}

int CIVSMenu::MenuWidth() const
{
	CRect rtWnd;
	GetClientRect(rtWnd);
	return rtWnd.Width();
}

void CIVSMenu::SetMenuOpen(BOOL bOpen)
{
	if (m_MenuData.bStatus != bOpen)
	{
		m_MenuData.bStatus = bOpen;
		m_MenuData.btnHeader.SetHeaderStatus(m_MenuData.bStatus);
		UpdateMenuStatus();
	}
}

void CIVSMenu::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	UpdateMenuStatus();
}

//当菜单移动的时候更新子项的背景
void CIVSMenu::OnMove(int x, int y) 
{
	CWnd::OnMove(x, y);
	UpdateMenu();
}

void CIVSMenu::SetTransparentBk(BOOL bTransparentBk)
{
	m_bTransparentBk = bTransparentBk; 
	if (GetSafeHwnd())
		Invalidate();
}

//渐变填充函数
void CIVSMenu::GradientFillRect(HDC hDC, const CRect &rcFill, ULONG nMode, COLORREF crLeftTop, COLORREF crRightBottom) const
{
	COLOR16 r = (COLOR16) ((crLeftTop & 0x000000FF)<<8);
	COLOR16 g = (COLOR16) (crLeftTop & 0x0000FF00);
	COLOR16 b = (COLOR16) ((crLeftTop & 0x00FF0000)>>8);

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = rcFill.left;
	vert [0] .y      = rcFill.top;
	vert [0] .Red    = r;
	vert [0] .Green  = g;
	vert [0] .Blue   = b;
	vert [0] .Alpha  = 0x0000;

	r = (COLOR16) ((crRightBottom & 0x000000FF)<<8);
	g = (COLOR16) (crRightBottom & 0x0000FF00);
	b = (COLOR16) ((crRightBottom & 0x00FF0000)>>8);

	vert [1] .x      = rcFill.right;
	vert [1] .y      = rcFill.bottom;
	vert [1] .Red    = r;
	vert [1] .Green  = g;
	vert [1] .Blue   = b;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	GradientFill(hDC, vert, 2, &gRect,1, nMode);
}

void CIVSMenu::PreSubclassWindow() 
{	
	//ModifyStyle(0,WS_POPUP);	
	CWnd::PreSubclassWindow();
}

BOOL CIVSMenu::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////
//释放所有菜单窗口
void CIVSMenu::UnLoadMenu()
{	
	EnumWindows(EnumWindowsProcA,0);
	m_bCapture = FALSE;
	m_bMouseOn = FALSE;
	m_bOver = FALSE;		
}

void CIVSMenu::OnMouseMove(UINT nFlags, CPoint point) 
{
	if((!m_bMouseOn)&& (!m_bCapture))
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 2;
		m_bMouseOn = _TrackMouseEvent(&tme);
	} else {
		if(m_bCapture)
		{
			CRect rtWnd;
			GetClientRect(rtWnd);
			//ClientToScreen(rtWnd);
			if (rtWnd.PtInRect(point))
			{	//如果回到本窗口则释放capture
				//ReleaseCapture();

				m_bCapture = FALSE;
				m_bMouseOn = FALSE;
			} else { 
				//确定mouse是在其它Menu窗口内，是内释放capture
				//并unload 本Menu;
				EnumWindows(EnumWindowsProcB,(LPARAM)this);
			}
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

LRESULT CIVSMenu::OnMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if((!m_bCapture) && m_bMouseOn )
	{
		CRect rtWnd;
		GetWindowRect(rtWnd);
		//ClientToScreen(rtWnd);
		CPoint point;
		GetCursorPos(&point);
		if (!rtWnd.PtInRect(point))
		{
			//m_bMouseOn = FALSE;
			m_bOver = FALSE;
			//SetCapture();
			m_bCapture = TRUE;
		}
	}
	m_bMouseOn = FALSE;
	return 0;
}

LRESULT CIVSMenu::OnMouseHover(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_bOver = TRUE;
	return 0;
}

void CIVSMenu::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rtWnd;
	GetClientRect(rtWnd);
	if (rtWnd.PtInRect(point))
	{
		return;
	}
	//ReleaseCapture();
	m_bCapture = FALSE;
	m_bMouseOn = FALSE;
	m_bOver = FALSE;
	UnLoadMenu();
	CWnd::OnLButtonDown(nFlags, point);
}
BOOL CIVSMenu::SetMenuHeader( const CString &StrHeadTitle, UINT nIDICON, BOOL bShowHead /*= TRUE */ )
{
	IVSMenu  menuData;

	menuData.bStatus = TRUE;			//
	menuData.nIcon = nIDICON;			//标题上显示的图标
	menuData.nBitmap = NULL;			//标题上显示的位图没有
	menuData.strTitle = StrHeadTitle;	// 标题内容
	this->m_bShowHead = bShowHead;
	SetMenuInfo(menuData);
	return TRUE;
}

BOOL CIVSMenu::AppendMenuItem( 
	const CString &StrTitle, 
	IVS_INT32 nID, 
	IVS_INT32 nNorBitmapID,
	IVS_INT32 nSelBitMapID , 
	IVS_INT32 nDisBitMapID, 
	CWnd* pSubMenu, 
	DWORD dwStyles, 
	const CString & strTip )
{
	MenuItemInfo menuItem;
	menuItem.isCreatedResource = true;
	menuItem.nBtnID = static_cast<IVS_UINT32>(nID);
	menuItem.nIcon  = 0;
	menuItem.nNorBitmap	= nNorBitmapID;
	menuItem.nSelBitmap = nSelBitMapID;
	menuItem.nDisBitmap = nDisBitMapID;
	menuItem.strTitle = StrTitle;
	menuItem.bSubMenu = dwStyles & MENU_SUBITEM;
	menuItem.bFontBold = dwStyles & MENU_FONTBOLD;
	menuItem.bIsBordLine = dwStyles & MENU_BORDLINE;
	menuItem.pChild = pSubMenu;
	menuItem.strTip  = strTip;
	menuItem.bCheckBox = dwStyles & MENU_CHECKITEM;
	this->AddMenuItem(menuItem);
	return true;
}

BOOL CIVSMenu::AppendMenuItem( 
	const CString &StrTitle, 
	IVS_INT32 nID, 
	const std::string &strNorBitmap,
	const std::string &strSelBitMap, 
	const std::string &strDisBitMap, 
	CWnd* pSubMenu, 
	DWORD dwStyles, 
	const CString & strTip )
{
	MenuItemInfo menuItem;
	menuItem.isCreatedResource = false;
	menuItem.nBtnID = static_cast<IVS_UINT32>(nID);
	menuItem.nIcon  = 0;

    menuItem.nNorBitmap	= 0;
    menuItem.nSelBitmap = 0;
    menuItem.nDisBitmap = 0;

	menuItem.strNorBitmap  = strNorBitmap;
	menuItem.strSelBitmap  = strSelBitMap;
	menuItem.strDisBitmap  = strDisBitMap;
	menuItem.strTitle = StrTitle;
	menuItem.bSubMenu = dwStyles & MENU_SUBITEM;
	menuItem.bFontBold = dwStyles & MENU_FONTBOLD;
	menuItem.bIsBordLine = dwStyles & MENU_BORDLINE;
	menuItem.pChild = pSubMenu;
	menuItem.strTip  = strTip;
	menuItem.bCheckBox = dwStyles & MENU_CHECKITEM;
	this->AddMenuItem(menuItem);
	return true;
}

BOOL CIVSMenu::LoadMainMenu()
{	
	ShowWindow(TRUE);
	//SetCapture();
	m_bOver = FALSE;
	m_bCapture = TRUE;
	m_bMouseOn = FALSE;

	return TRUE;
}
void CIVSMenu::ClearButtonData()
{
	MenuItemInfo *pBtnInfo = NULL;
	for(POSITION pos = m_MenuData.BtnList.GetHeadPosition(); pos!=NULL; )
	{
		pBtnInfo = (MenuItemInfo*)m_MenuData.BtnList.GetNext(pos);
		if (pBtnInfo&&pBtnInfo->btn.GetSafeHwnd())
		{
			pBtnInfo->btn.DestroyWindow();
		}
	}
	m_MenuData.BtnList.RemoveAll();
}

/////////////////////////////////////////////////
void CIVSMenu::UnLoadChildMenu()
{
	ShowWindow(FALSE);
	ClearButtonData();
}
/////////////////////////////////////////////////
//指定菜单显示位置,参数nMODE指出菜单四个点中一个的坐标(x,y)
void CIVSMenu::ShowMenuPos(int x, int y, int nMode)
{
	if ((nMode > RIGHTBOTTOM) || (nMode < LEFTTOP))
	{
		nMode = 0;
	}

	int iWidth = GetSystemMetrics(SM_CXSCREEN);
	//int iHeight = GetSystemMetrics(SM_CYSCREEN);
	int iMenuWidth = MenuWidth();
	int iMenuHeight = MenuHeight();

	//如果超过右边边界了就左移
	if (x+iMenuWidth > iWidth)
	{
		x = iWidth - iMenuWidth;
	}
	//如果上面越界了就从0开始
	if (y-iMenuHeight<0)
	{
		y = iMenuHeight;
	}

	switch(nMode)
	{
	case LEFTTOP:
		SetWindowPos(&wndTopMost,x, y, MenuWidth(), MenuHeight(),SWP_SHOWWINDOW);
		(void)LoadMainMenu();
		break;
	case LEFTBOTTOM:
		SetWindowPos(&wndTopMost,x, y - MenuHeight(), MenuWidth(), MenuHeight(),SWP_SHOWWINDOW);
		(void)LoadMainMenu();
		break;

	case RIGHTBOTTOM:
		SetWindowPos(&wndTopMost,x - MenuWidth(), y - MenuHeight(), MenuWidth(), MenuHeight(),SWP_SHOWWINDOW);
		(void)LoadMainMenu();
		break;
	case RIGHTTOP:
		SetWindowPos(&wndTopMost,x - MenuWidth() , y, MenuWidth(), MenuHeight(),SWP_SHOWWINDOW);
		(void)LoadMainMenu();
		break;
    default:
        break;
	}
}

BOOL CIVSMenu::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		Beep(300,10);
		IVS_INT32 KeyCode = static_cast<IVS_INT32>(pMsg->wParam);
		switch(KeyCode)
		{
		case VK_UP:			 
			VKUPEvent();
			KeyProcess(KeyCode);
			return FALSE;
		case VK_DOWN:
			VKDOWNEvent();
			KeyProcess(KeyCode);
			return FALSE;
		case VK_LEFT:			
			KeyProcess(KeyCode);
			break;
		case VK_RIGHT:
		case VK_RETURN:
			KeyProcess(KeyCode);
			break;
		case VK_ESCAPE:
			UnLoadMenu();
			//ReleaseCapture();
			break;
		case VK_SPACE:
			return TRUE;
		default:
			break;			
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CIVSMenu::VKUPEvent(){
	if ((m_ButtonPos>0)&&(m_ButtonPos <= m_MenuData.BtnList.GetCount()))
	{
		m_ButtonPos--;
	} else {
		m_ButtonPos = m_MenuData.BtnList.GetCount() - 1 ;
	}
}

void CIVSMenu::VKDOWNEvent(){
	if ((m_ButtonPos>=0)&&(m_ButtonPos < m_MenuData.BtnList.GetCount()-1))
	{
		m_ButtonPos++;
	} else {
		m_ButtonPos = 0;
	}
}

void CIVSMenu::KeyProcess(int nKeyCode)
{
	bool isCyc = true;
	if(m_ButtonPos < 0)
		return;
	MenuItemInfo *pBtnInfo=NULL;
	while(isCyc)
	{
		POSITION pos = m_MenuData.BtnList.FindIndex(m_ButtonPos);
		pBtnInfo = m_MenuData.BtnList.GetAt(pos);

		if (NULL == pBtnInfo)
		{
			continue;
		}
		//处理ENTER
		if(nKeyCode == VK_RETURN)
		{ 
			pBtnInfo->btn.PostMessage(WM_LBUTTONDOWN);
			pBtnInfo->btn.PostMessage(WM_LBUTTONUP);
			pBtnInfo->btn.m_bHold = TRUE;

			return;
		}
		//处理LEFT KEY
		if(nKeyCode == VK_LEFT)
		{ 
			CHECK_POINTER_VOID(m_pParent);
			this->ClearButtonData();
			ShowWindow(FALSE);
			m_ButtonPos = -1;
			CIVSMenu *pMenu = dynamic_cast<CIVSMenu *>(m_pParent);
			CHECK_POINTER_VOID(pMenu);
			pMenu->ClearButtonData();
			pMenu->Invalidate();
			m_pParent->SetFocus();
		}
		//处理右方向键
		if( (nKeyCode == VK_RIGHT) &&( pBtnInfo->btn.m_bSubMenu)&&(pBtnInfo->btn.m_pChildWnd->GetSafeHwnd()))
		{	
			pBtnInfo->btn.PostMessage(WM_LBUTTONDOWN);
			pBtnInfo->btn.PostMessage(WM_LBUTTONUP);
			pBtnInfo->btn.m_bHold = TRUE;
		}
		//处理UP DOWN KEY
		if(KeyCodeIsEqUpORDown(nKeyCode))
		{
			if(!pBtnInfo->btn.m_bIsBordLine)
			{
				pBtnInfo->btn.SetFocus();
				isCyc = false;
				break;
			}
			//如果是分界线
			if(nKeyCode == VK_UP)
			{
				m_ButtonPos--;
			} else {
				m_ButtonPos++;
			}
		}
	}
}     //lint !e438
////////////////////////////////////////////////
void CIVSMenu::SetBackColor(COLORREF clBack)
{
	m_clBack = clBack;
	MenuItemInfo *pBtnInfo = NULL;
	for(POSITION pos = m_MenuData.BtnList.GetHeadPosition(); pos!=NULL; )
	{
		pBtnInfo = (MenuItemInfo*)m_MenuData.BtnList.GetNext(pos);
		pBtnInfo->btn.SetBackColor(m_clBack);
	}
}

IVS_INT32 CIVSMenu::GetItemNum() const
{
	return m_MenuData.BtnList.GetCount();
}
/******************************************************************
//function : KeyCodeIsEqUpORDown
//description: 
//input : int nKeyCode
//output : NA
//return : bool
*******************************************************************/
bool CIVSMenu::KeyCodeIsEqUpORDown( int nKeyCode ) const
{
	return nKeyCode == VK_UP || nKeyCode == VK_DOWN ;
}
