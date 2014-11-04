// TranTVWallStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "TranTVWallStatusBar.h"


// CTranTVWallStatusBar dialog

IMPLEMENT_DYNAMIC(CTranTVWallStatusBar, CTranslucentDialog)

CTranTVWallStatusBar::CTranTVWallStatusBar(Gdiplus::Image* pImage, CWnd* pParent)
	: CTranslucentDialog(CTranVideoRealtimeBar::IDD, pImage, pParent)
{

}

CTranTVWallStatusBar::~CTranTVWallStatusBar()
{
}

void CTranTVWallStatusBar::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTranTVWallStatusBar, CTranslucentDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTranTVWallStatusBar message handlers


void CTranTVWallStatusBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CTranslucentDialog::OnPaint() for painting messages

	UpdateView();
}
void CTranTVWallStatusBar::OnInitChildrenWnds()
{

}
