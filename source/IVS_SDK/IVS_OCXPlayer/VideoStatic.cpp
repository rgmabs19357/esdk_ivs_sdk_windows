// VideoStatic.cpp : implementation file

#include "stdafx.h"
#include "VideoStatic.h"
#include "VideoPane.h"

#define  VIDEO_RESOLUTION_CHANGED_PANE WM_USER+4563     //lint !e773 MFC的消息宏

// CVideoStatic

IMPLEMENT_DYNAMIC(CVideoStatic, CStatic);

CVideoStatic::CVideoStatic()
    : m_bIsLButtonDown(false)
    , m_StartPos(0)
{
}

CVideoStatic::~CVideoStatic()
{
}

BEGIN_MESSAGE_MAP(CVideoStatic, CStatic)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_SETCURSOR()
    ON_WM_LBUTTONDBLCLK()
    ON_MESSAGE(VIDEO_RESOLUTION_CHANGED_PANE, &CVideoStatic::OnResolutionChanged)   //lint !e665
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CVideoStatic message handlers

void CVideoStatic::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    dc.SetBkColor(TRANSPARENT);
}

void CVideoStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane*>(GetParent());
    CHECK_POINTER_VOID(pVideoPane);

   pVideoPane->SetActivePane();

    CRect mainRect;
    GetClientRect(&mainRect);

    m_StartPos = point;             // 如果是拖动记录起始点位置
    m_bIsLButtonDown = true;
}//lint !e715

void CVideoStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_bIsLButtonDown =false;
}//lint !e715

void CVideoStatic::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bIsLButtonDown)
    {
        if ((point.x == m_StartPos.x) && (point.y == m_StartPos.y))
        {
            return;
        }
        CRect mainRect;
        GetClientRect(&mainRect);
    }

    TRACKMOUSEEVENT tme;   
    tme.cbSize=sizeof(TRACKMOUSEEVENT);   
    tme.dwFlags=TME_LEAVE;   
    tme.dwHoverTime=HOVER_DEFAULT;   
    tme.hwndTrack=m_hWnd;   
    if(!_TrackMouseEvent(&tme))
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"", "_TrackMouseEvent fail");
    }
}//lint !e715

void CVideoStatic::OnMouseLeave()
{
    m_bIsLButtonDown = false;
}

BOOL CVideoStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    //目前只有鹰眼static会接收事件，视频显示static事件在VideoPane中处理
    ::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MAGNIFY));
    return FALSE;
}//lint !e818 !e1762 !e715

void CVideoStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CVideoPane* pVideoPane = (dynamic_cast<CVideoPane*>(GetParent()));
	CHECK_POINTER_VOID(pVideoPane);
    pVideoPane->OnLButtonDblClk(nFlags, point);
}//lint !e1762 MFC自动生成的行数

afx_msg LRESULT CVideoStatic::OnResolutionChanged(WPARAM wParam, LPARAM lParam)
{
    
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(GetParent());
    CHECK_POINTER(pVideoPane,IVS_FAIL);

    if (pVideoPane->GetDisplayScale()==DISPLAY_ASPECT_RATIO_ORIGINAL)
    {    
        if (pVideoPane->GetRealtimeMagnifyState()==MAGNIFY_OPEN 
            || pVideoPane->GetRecordMagnifyState()==MAGNIFY_OPEN 
            || pVideoPane->Get3DPTZState()==PTZ_3D_OPEN 
            || pVideoPane->GetWndType()==WND_TYPE_ANALYSIS)
        {
            pVideoPane->ChangePaneSize();
        }
    }
    
    return IVS_SUCCEED;
}//lint !e1762 !e715

BOOL CVideoStatic::OnEraseBkgnd(CDC* pDC)
{
    return CWnd::OnEraseBkgnd(pDC);
}
