#include "stdafx.h"
#include "IVSMenuHeader.h"
#include "MyMenuStruct.h"
#include "SDKConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CIVSHeader

CIVSHeader::CIVSHeader()
	:m_bOpen(FALSE)
	,m_hIconStatus(nullptr)
{
}

CIVSHeader::~CIVSHeader()
{
	IVS_DELETE(m_hIconStatus); //lint !e1551 
}

BEGIN_MESSAGE_MAP(CIVSHeader, CIVSButton)
	//{{AFX_MSG_MAP(CIVSHeader)
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIVSHeader message handlers

void CIVSHeader::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CRect rect =  lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CHECK_POINTER_VOID(pDC);
	TCHAR strText[MAX_PATH + 1];
	::GetWindowText(m_hWnd, strText, MAX_PATH);

	//画图标
	CPoint pt(MARGIN*2,5);
	::DrawIconEx(pDC->m_hDC, pt.x, pt.y, m_hIcon, MENUICON_WIDTH, MENUICON_WIDTH, 0, NULL, DI_IMAGE|DI_MASK);

	// 字体可配置
	(void)CSDKConfig::instance().LoadConfigFile();
	std::string strFontFamily = CSDKConfig::instance().GetFontFamilyValue();
	if (strFontFamily.empty())
	{
		strFontFamily = "宋体";
	}
	// 使用9号字体并将宽度设置为0 使得自适应
	int nHeight = -MulDiv(9, GetDeviceCaps(*pDC, LOGPIXELSY),72);

	//写文字
	CFont font;
	font.CreateFont(nHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE,0,0,0,0,0,0,strFontFamily.c_str());
	CFont *pOldFont = (CFont*)pDC->SelectObject(&font);
	CSize szExtent = pDC->GetTextExtent(strText, lstrlen(strText));
	
	pt = CPoint(MENUICON_WIDTH + 4*MARGIN, rect.top + rect.Height()/2- szExtent.cy/2);
	IVS_ULONG OldColor;
	
	if (m_bOver)
		OldColor = pDC->SetTextColor(MENU_OVER_COL);
	else
		OldColor = pDC->SetTextColor(RGB(255,255,255));
	
	int nMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawState(pt, szExtent, strText, DSS_NORMAL, TRUE, 0, (HBRUSH)NULL);
	
	pDC->SetTextColor(OldColor);
	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nMode);
}

BOOL CIVSHeader::OnEraseBkgnd(CDC* pDC) 
{
	//填充背景
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.right = rcClient.right + 10;

	CPen pen(PS_SOLID,0,RGB(102,180,255));	
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(RGB(102,180,255));
	CBrush *pOldBrush = pDC-> SelectObject(&brush);
	pDC->RoundRect(rcClient,CPoint(8,8));	

	int i;
	for (i= rcClient.bottom - 5; i<rcClient.bottom; i++)
	{
		pDC->MoveTo(rcClient.left,i);
		pDC->LineTo(rcClient.right,i);
	}

	pDC-> SelectObject(pOldBrush);
	pDC-> SelectObject(pOldPen);

	rcClient.InflateRect(-2,-3);
	GradientFillRect(pDC->m_hDC, rcClient, GRADIENT_FILL_RECT_H, RGB(112,180,222),RGB(102,160,225) );
	return CIVSButton::OnEraseBkgnd(pDC);
}

BOOL CIVSHeader::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcTop(rcClient.left, MENUTITLE_HEIGHT-BUTTON_HEIGHT, rcClient.right, MENUTITLE_HEIGHT);
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	if (rcTop.PtInRect(pt))
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CIVSHeader::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_LBUTTONDBLCLK || message == WM_LBUTTONDOWN)
	{
		//处理这个消息的目的是：用户双击时响应单击效果
		//不过不在规定区域得到消息无效
		message = WM_LBUTTONDOWN;
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcTop(rcClient.left, MENUTITLE_HEIGHT-BUTTON_HEIGHT, rcClient.right, MENUTITLE_HEIGHT);
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if (!rcTop.PtInRect(pt))
			return TRUE;
	}
	return CIVSButton::DefWindowProc(message, wParam, lParam);
}

void CIVSHeader::SetHeaderStatus(BOOL bOpen)
{
	m_bOpen = bOpen;
}

void CIVSHeader::GradientFillRect( HDC hDC, const CRect &rcFill, ULONG nMode, COLORREF crLeftTop, COLORREF crRightBottom ) const
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

void CIVSHeader::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CIVSButton::OnLButtonDown(nFlags, point);
}
