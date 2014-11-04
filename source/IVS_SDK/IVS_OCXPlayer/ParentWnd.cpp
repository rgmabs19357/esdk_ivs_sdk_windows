#include "stdafx.h"
#include "ParentWnd.h"
#include <afxdialogex.h>
#include "VideoPaneMgr.h"
#include "IVS_OCXPlayerCtrl.h"
#define  MAX_DRAG_TIME_RANGE 100
// CParentWnd dialog

IMPLEMENT_DYNAMIC(CParentWnd, CDialog)

CParentWnd::CParentWnd(CWnd* pParent /*=nullptr*/)
	: CDialog(CParentWnd::IDD, pParent)
    , m_nRightEdge(0)
    , m_nBottomEdge(0)
	, m_bIsDrag(false)
	, m_bDisableDrag(false)
	, m_pVideoPaneMgr(NULL)
	, m_dwStart(0)
	, m_dwEnd(0)
{
    m_ListCrect.clear();
}

CParentWnd::~CParentWnd()
{
    try
    {
        m_ListCrect.clear();
    }
    catch(...)
    {
    }
}//lint !e1740 m_pVideoPaneMgr 由控件传入 由控件释放 在此无须释放

void CParentWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CParentWnd, CDialog)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_KEYUP()
    ON_WM_SETFOCUS()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSELEAVE()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CParentWnd message handlers

BOOL CParentWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
 //   CRect rect;
 //   GetClientRect(rect);

	//pDC->FillSolidRect(rect,COLOR_VIDEOPANE_BKG);

 //   return TRUE;
}//lint !e818 !e715 !e1762

int CParentWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    HICON m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_LOGO);
    if (nullptr != m_hIcon)
    {
        SetIcon(m_hIcon,TRUE);
        SetIcon(m_hIcon,FALSE);
    }

    return IVS_SUCCEED;
}

void CParentWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_ESCAPE:
        {
            CHECK_POINTER_VOID(m_pVideoPaneMgr);
            m_pVideoPaneMgr->ResetFullScreen();
            break;
        }
    default:
		{
			CHECK_POINTER_VOID(m_pVideoPaneMgr);
			(void)m_pVideoPaneMgr->FullScreenKeyUp(nChar);
		}
        return;
    }

    CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

// Qualifier: 虽然是没有实现，但是不能删除，下面函数截取了dialog的setFocu消息，
//             能实现alt+tab来切换窗口，否则会出现错误
void CParentWnd::OnSetFocus(CWnd* pOldWnd)
{
}//lint !e818 !e715 !e1762MFC自动生成的的函数

void CParentWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(rect);

	dc.FillSolidRect(rect,COLOR_VIDEOPANE_BKG);
}

void CParentWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    CHECK_POINTER_VOID(m_pVideoPaneMgr);
    m_pVideoPaneMgr->OnLButtonDown(nFlags,point);
}
void CParentWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CHECK_POINTER_VOID(m_pVideoPaneMgr);
    m_pVideoPaneMgr->OnLButtonDblClk(nFlags,point);
}

void CParentWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    // 组装一个鼠标离开事件
    TRACKMOUSEEVENT   tme;
    tme.cbSize=sizeof(TRACKMOUSEEVENT);
    tme.dwFlags=TME_LEAVE;
    tme.dwHoverTime=HOVER_DEFAULT;
    tme.hwndTrack=m_hWnd;
    if(!_TrackMouseEvent(&tme))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "", "_TrackMouseEvent fail");
    }

    if (!m_bIsDrag)
    {
        return;
    }

    if(nullptr != m_pVideoPaneMgr)
    {
        ULONG ulWndID = m_pVideoPaneMgr->GetWndIDByMouse();//lint !e613

        if ((MIN_PANE_NUMBER <= ulWndID) && (MAX_PANE_NUMBER >= ulWndID))
        {
			if (GetDisableDrag())
			{
				// 几种不能交换的情况
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_EXCHANGE_WARN));
			}
			else
			{
				//有效表示鼠标在有效范围
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_EXCHANGE));
			}
        } else {
            //无效表示鼠标在无效范围
            ::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_EXCHANGE_WARN));
        }

        m_pVideoPaneMgr->OnHideAllLocker();//lint !e613
    }
    CDialog::OnMouseMove(nFlags, point);
}

void CParentWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bIsDrag)
    {
		ReleaseCapture();
		m_dwEnd = GetTickCount();
		DWORD dwTime = m_dwEnd - m_dwStart;
		if (dwTime > MAX_DRAG_TIME_RANGE)
		{
			if(nullptr != m_pVideoPaneMgr)
			{
				ULONG ulWndID = m_pVideoPaneMgr->GetWndIDByMouse();//lint !e613

				if (ulWndID == (ULONG)IVS_OCX_RET_POINT_OUT_CLIENT)
				{
					//超出了界限
					m_pVideoPaneMgr->DragMoveOutofOCX();//lint !e613
				}

				BP_RUN_LOG_INF("", "ulWndID:%ld",ulWndID);

				if ((MIN_PANE_NUMBER <= ulWndID) && (MAX_PANE_NUMBER >= ulWndID))
				{
					m_pVideoPaneMgr->ExchangePane(ulWndID);//lint !e613
				}
			}
		}      
        /*ReleaseCapture();*/
        m_bIsDrag = false;
		SetDisableDrag(false);
        ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_dwEnd = 0;
		m_dwStart = 0;
    }

    CDialog::OnLButtonUp(nFlags, point);
}

void CParentWnd::OnMouseLeave()
{
    if (m_pVideoPaneMgr != nullptr)
    {
        m_pVideoPaneMgr->OnHideAllLocker();//lint !e613
    }
	CDialog::OnMouseLeave();
}

BOOL CParentWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    CHECK_POINTER(m_pVideoPaneMgr,FALSE);
    return m_pVideoPaneMgr->OnMouseWheel(nFlags, zDelta, pt);
} //lint !e1746  MFC自动生成的的函数

void CParentWnd::StartDrag()
{
	m_dwStart = GetTickCount();
}
