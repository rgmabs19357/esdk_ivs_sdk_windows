#include "stdafx.h"
#include "BitmapSlider.h"
CBitmapSlider::CBitmapSlider()
    : m_nMax(100)
    , m_nMin(0)
    , m_nPos(0)
    , m_nPage(20)
    , m_nWidth(0)
    , m_nHeight(0)
    , m_nThumbWidth(0)
    , m_nThumbHeight(0)
    , m_nMarginLeft(0)
    , m_nMarginRight(0)
    , m_nMarginTop(0)
    , m_nMarginBottom(0)
    , m_nThumbBgX(-1)
    , m_nThumbBgY(-1)
    , m_nMouseOffset(0)
    , m_bVertical(FALSE)
    , m_bChannelActive (FALSE)
    , m_bThumbActive ( FALSE)
    , m_bTransparentChannel ( FALSE)
    , m_bTransparentThumb ( FALSE)
    , m_bThumb(FALSE)
    , m_bChannel(FALSE)
    , m_bLButtonDown(FALSE)
    , m_bFocus(FALSE)
    , m_bFocusRect(FALSE)
    , m_bDrawFocusRect(TRUE)
    , m_bEnable(TRUE)
{
}

CBitmapSlider::~CBitmapSlider()
{}

BEGIN_MESSAGE_MAP(CBitmapSlider, CSliderCtrl)
	ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_GETDLGCODE()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
    ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
    ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
    ON_WM_CREATE()
END_MESSAGE_MAP()

BOOL CBitmapSlider::SetBitmapChannel(UINT nChannelID, UINT nActiveID, BOOL bTransparent,
	COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY )
{
	(void)AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!nChannelID)
	{
		m_bChannel = FALSE;

		(void)m_bmChannel.DeleteObject();
		(void)m_bmChannelMask.DeleteObject();
		(void)m_bmChannelActive.DeleteObject();
		(void)m_bmChannelActiveMask.DeleteObject();

		return TRUE;
	}

	(void)m_bmChannel.DeleteObject();
	if (!m_bmChannel.LoadBitmap( nChannelID ))
	{
		return FALSE;
	}

	if (bTransparent)
	{
		PrepareMask( &m_bmChannel, &m_bmChannelMask,
			clrpTransColor, iTransPixelX, iTransPixelY );
	}

	if (nActiveID)
	{
		(void)m_bmChannelActive.DeleteObject();

		if (!m_bmChannelActive.LoadBitmap( nActiveID ))
		{
			(void)m_bmChannel.DeleteObject();
			if (bTransparent)
			{
				(void)m_bmChannelMask.DeleteObject();
			}

			return FALSE;
		}

		if (bTransparent)
		{
			PrepareMask( &m_bmChannelActive, &m_bmChannelActiveMask,
				clrpTransColor, iTransPixelX, iTransPixelY );
		}

		m_bChannelActive = TRUE;
	} else {
		m_bChannelActive = FALSE;
	}

	BITMAP bitmap = {0};
	(void)m_bmChannel.GetBitmap( &bitmap );

	m_nHeight = bitmap.bmHeight;
	m_nWidth  = bitmap.bmWidth;

	if (m_bChannelActive)
	{
		BITMAP bitmapActive;
		(void)m_bmChannelActive.GetBitmap( &bitmapActive );

		ASSERT( m_nWidth == bitmapActive.bmWidth && m_nHeight == bitmapActive.bmHeight );
	}

	(void)SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOZORDER | SWP_NOMOVE);
	GetClientRect(&m_rectWindow);
	m_bTransparentChannel = bTransparent;
	m_bChannel = TRUE;

	return TRUE;
}

BOOL CBitmapSlider::OnEraseBkgnd(CDC* pDC)
{
    (void)pDC;

    return TRUE;
}//lint !e1762


void CBitmapSlider::SetRangeMax(int nMax, BOOL bRedraw)
{
    m_nMax = nMax;
    if (bRedraw)
    {
        Invalidate();
    }
}

void CBitmapSlider::SetRangeMin(int nMin, BOOL bRedraw)
{
    m_nMin = nMin;
    if (bRedraw)
    {
        Invalidate();
    }
}

void CBitmapSlider::SetRange(int nMin, int nMax, BOOL bRedraw)
{
    SetRangeMin( nMin, FALSE );
    SetRangeMax( nMax, bRedraw );
}

void CBitmapSlider::SetPos(int nPos)
{
    m_nPos = nPos;
    if (m_nPos > m_nMax)
    {
        m_nPos = m_nMax;
    }

    if (m_nPos < m_nMin)
    {
        m_nPos = m_nMin;
    }

    Invalidate();
}

int CBitmapSlider::SetPageSize(int nSize)
{
    int nRet = m_nPage;
    m_nPage = nSize;
    return nRet;
}

void CBitmapSlider::SetMargin(int nLeft, int nTop, int nRight, int nBottom )
{
    SetMarginLeft( nLeft );
    SetMarginTop( nTop );
    SetMarginRight( nRight );
    SetMarginBottom( nBottom );
}

void CBitmapSlider::Enable(BOOL bEnable)
{
    m_bEnable = bEnable;
    if (!m_bEnable && m_bLButtonDown)
    {
        m_bLButtonDown = FALSE;
        (void)ReleaseCapture();
    }

    Invalidate();
}

void CBitmapSlider::OnPaint()
{
	CPaintDC dcOrigin(this);

	XCMemDC dc(&dcOrigin, &m_rectWindow, m_bTransparentChannel);

	CDC dcMem;
	(void)dcMem.CreateCompatibleDC( &dc );
	if (m_bFocusRect && (m_bTransparentChannel || !m_bChannel))
	{
		dc.DrawFocusRect(m_rectWindow);
		m_bFocusRect = FALSE;
	}

	DrawChannel(dcMem, dc);
	if (m_nThumbBgX != -1)
	{
		RestoreBackground(
			&dc, m_nThumbBgX, m_nThumbBgY,
			m_nThumbWidth, m_nThumbHeight, &m_bmThumbBg );

		m_nThumbBgX = -1;
	}

	DrawThumb( dcMem, dc);
	if (m_bDrawFocusRect && m_bFocus && m_bEnable)
	{
		dc.DrawFocusRect(m_rectWindow);
		m_bFocusRect = TRUE;
	}

	(void)dcMem.DeleteDC();
}

void CBitmapSlider::DrawFocusRect(BOOL bDraw, BOOL bRedraw)
{
    m_bDrawFocusRect = bDraw;

    if (bRedraw)
    {
        Invalidate();
    }
}

BOOL CBitmapSlider::SetBitmapChannelEx(HBITMAP hBitmapChannel, HBITMAP hBitmapChannelActive, BOOL bTransparent,
                                       COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY )
{
    (void)m_bmChannel.DeleteObject();
    (void)m_bmChannel.Attach(hBitmapChannel);
    if (bTransparent)
    {
        PrepareMask( &m_bmChannel, &m_bmChannelMask,
                     clrpTransColor, iTransPixelX, iTransPixelY );
    }

    if (hBitmapChannelActive)
    {
        (void)m_bmChannelActive.DeleteObject();
        (void)m_bmChannelActive.Attach(hBitmapChannelActive);

        if (bTransparent)
        {
            PrepareMask( &m_bmChannelActive, &m_bmChannelActiveMask,
                         clrpTransColor, iTransPixelX, iTransPixelY );
        }
        m_bChannelActive = TRUE;
    }
    else
    {
        m_bChannelActive = FALSE;
    }

	BITMAP bitmap = {0};
    (void)m_bmChannel.GetBitmap( &bitmap );

    m_nHeight = bitmap.bmHeight;
	m_nWidth  = bitmap.bmWidth;
    if (m_bChannelActive)
    {
        BITMAP bitmapActive;
        (void)m_bmChannelActive.GetBitmap( &bitmapActive );
        ASSERT( m_nWidth == bitmapActive.bmWidth && m_nHeight == bitmapActive.bmHeight );
    }

    (void)SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOZORDER | SWP_NOMOVE);
    GetClientRect(&m_rectWindow);
    m_bTransparentChannel = bTransparent;
    m_bChannel = TRUE;

    return TRUE;
}

void CBitmapSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_bEnable)
    {
        return;
    }

    (void)SetCapture();
    (void)SetFocus();

    m_bLButtonDown = TRUE;
    if (m_bVertical)
    {
        if (abs( point.y - Pos2Pixel( m_nPos )) <= m_nThumbHeight / 2)
        {
            m_nMouseOffset = point.y - Pos2Pixel( m_nPos );
        }
        else
        {
            m_nMouseOffset = 0;
        }
    }
    else
    {
        if (abs( point.x - Pos2Pixel( m_nPos )) <= m_nThumbWidth / 2)
        {
            m_nMouseOffset = point.x - Pos2Pixel( m_nPos );
        }
        else
        {
            m_nMouseOffset = 0;
        }
    }

    OnMouseMove( nFlags, point );
    Invalidate();

    CSliderCtrl::OnLButtonDown(nFlags, point);
}

void CBitmapSlider::RelayToolTipEventFromParent(CPoint &point)
{
    GetParent()->ClientToScreen(&point);
    ScreenToClient(&point);
    RelayToolTipEvent(point);
}

void CBitmapSlider::OnMouseMove(UINT nFlags, CPoint point)
{
    RelayToolTipEvent(point );
    if (!m_bLButtonDown || !m_bEnable)
    {
        return;
    }

    int nPixel;
    if (m_bVertical)
    {
        nPixel = point.y - m_nMouseOffset;

        if (nPixel > m_nHeight - (m_nMarginBottom + m_nThumbHeight / 2))
        {
            nPixel = m_nHeight - (m_nMarginBottom + m_nThumbHeight / 2);
        }

        if (nPixel < m_nMarginTop + m_nThumbHeight / 2)
        {
            nPixel = m_nMarginTop + m_nThumbHeight / 2;
        }
    }
    else
    {
        nPixel = point.x - m_nMouseOffset;

        if (nPixel < m_nMarginLeft + m_nThumbWidth / 2)
        {
            nPixel = m_nMarginLeft + m_nThumbWidth / 2;
        }

        if (nPixel > m_nWidth - (m_nMarginRight + m_nThumbWidth / 2))
        {
            nPixel = m_nWidth - (m_nMarginRight + m_nThumbWidth / 2);
        }
    }

    if (Pos2Pixel(m_nPos) != nPixel)
    {
        SetPos( Pixel2Pos( nPixel ));

        ::PostMessage(
            GetParent()->GetSafeHwnd(), WM_BITMAPSLIDER_MOVING,
            static_cast <UINT>(GetDlgCtrlID()), static_cast <long>(m_nPos));
    }

    CSliderCtrl::OnMouseMove(nFlags, point);
}

void CBitmapSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!m_bEnable)
    {
        return;
    }

    (void)ReleaseCapture();
    m_bLButtonDown = FALSE;

    Invalidate();
    ::PostMessage(
        GetParent()->GetSafeHwnd(), WM_BITMAPSLIDER_MOVED,
        static_cast <UINT>(GetDlgCtrlID()), static_cast <long>(m_nPos));

    CSliderCtrl::OnLButtonUp(nFlags, point);
}

int CBitmapSlider::Pos2Pixel(int nPos)
{
    if (m_bVertical)
    {
        return m_nMarginTop + m_nThumbHeight / 2 +
               (int)((m_nHeight - (m_nMarginTop + m_nMarginBottom + m_nThumbHeight)) *
                     ((double) (nPos - m_nMin) / (m_nMax - m_nMin))
               );
    }
    else
    {
        return (int)(
                   (m_nWidth - (m_nMarginLeft + m_nMarginRight + m_nThumbWidth)) *
                   ((double) (nPos - m_nMin) / (m_nMax - m_nMin))
        ) + m_nMarginLeft + m_nThumbWidth / 2;
    }
}//lint !e1762

int CBitmapSlider::Pixel2Pos(int nPixel)
{
    int nPos = 0;

    if (m_bVertical)
    {
        int totalPixelHeight = m_nHeight - (m_nMarginBottom + m_nMarginTop + m_nThumbHeight);
        if (totalPixelHeight != 0)
        {
            nPos = (int)(m_nMin
                         + ((double)(nPixel
                                     - (m_nMarginTop + (double)m_nThumbHeight / 2))
                            / (double)totalPixelHeight) * (m_nMax - m_nMin));
        }
    }
    else
    {
        int totalPixelWidth = m_nWidth - (m_nMarginLeft + m_nMarginRight + m_nThumbWidth);
        if (totalPixelWidth != 0)
        {
            nPos = (m_nMin + (int)(((double)(nPixel - (m_nMarginLeft + (double)m_nThumbWidth / 2))
                                    / (double)totalPixelWidth) * (m_nMax - m_nMin)));
        }
    }

    return nPos;
}//lint !e1762

void CBitmapSlider::CopyBackground(CDC *pDC, int nXSrc, int nYSrc, int nWidth, int nHeight, CBitmap *pBmDst)
{
	CHECK_POINTER_VOID(pBmDst);
    (void)pBmDst->DeleteObject();
    (void)pBmDst->CreateCompatibleBitmap( pDC, nWidth, nHeight );

    CDC memDC;

    (void)memDC.CreateCompatibleDC(pDC);
    CBitmap *pBmTmp = memDC.SelectObject(pBmDst);
    (void)memDC.BitBlt(0, 0, nWidth, nHeight, pDC, nXSrc, nYSrc, SRCCOPY);
    (void)memDC.SelectObject(pBmTmp);
	(void)pBmDst->DeleteObject();
    (void)memDC.DeleteDC();
}//lint !e1762

void CBitmapSlider::RestoreBackground(CDC *pDC, int nXDst, int nYDst, int nWidth, int nHeight, CBitmap *pBmSrc)
{
    CDC memDC;
	(void)memDC.CreateCompatibleDC(pDC);
	CBitmap *pBmTmp = memDC.SelectObject(pBmSrc);
	(void)pDC->BitBlt(nXDst, nYDst, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);
	(void)memDC.SelectObject(pBmTmp);
	(void)memDC.DeleteDC();
}//lint !e1762

void CBitmapSlider::DrawBitmap(CDC *pDC, int xStart, int yStart, int wWidth, int wHeight,
                               CDC *pTmpDC, int xSource, int ySource, CBitmap *bmMask, BOOL bTransparent )
{
    if (bTransparent)
    {
        DrawTransparentBitmap(
            pDC, xStart, yStart,
            wWidth, wHeight,
            pTmpDC, xSource, ySource, bmMask );
    }
    else
    {
        (void)pDC->BitBlt( xStart, yStart,
                           wWidth, wHeight,
                           pTmpDC, xSource, ySource, SRCCOPY );
    }
}

void CBitmapSlider::PrepareMask(CBitmap *pBmpSource, CBitmap *pBmpMask,
                                COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY)
{
    BITMAP bm;
    memset(&bm, 0, sizeof(bm));
    (void)pBmpSource->GetObject(sizeof(BITMAP), &bm);
    (void)pBmpMask->DeleteObject();
    (void)pBmpMask->CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    CDC hdcSrc, hdcDst;

    (void)hdcSrc.CreateCompatibleDC(NULL);
    (void)hdcDst.CreateCompatibleDC(NULL);

    CBitmap* hbmSrcT = (CBitmap*) hdcSrc.SelectObject(pBmpSource);
    CBitmap* hbmDstT = (CBitmap*) hdcDst.SelectObject(pBmpMask);
    COLORREF clrTrans;
    if (clrpTransColor == 0xFF000000)
    {
        clrTrans = hdcSrc.GetPixel(iTransPixelX, iTransPixelY);
    }
    else
    {
        clrTrans = clrpTransColor;
    }

    COLORREF clrSaveBk = hdcSrc.SetBkColor(clrTrans);
    (void)hdcDst.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &hdcSrc, 0, 0, SRCCOPY);
    COLORREF clrSaveDstText = hdcSrc.SetTextColor(COLOR_WHITE);
    (void)hdcSrc.SetBkColor(COLOR_BLACK);//lint !e835 !e845
    (void)hdcSrc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &hdcDst, 0, 0, SRCAND);
    (void)hdcDst.SetTextColor(clrSaveDstText);
    (void)hdcSrc.SetBkColor(clrSaveBk);
    (void)hdcSrc.SelectObject(hbmSrcT);
    (void)hdcDst.SelectObject(hbmDstT);
    (void)hdcSrc.DeleteDC();
    (void)hdcDst.DeleteDC();
}//lint !e1762
void CBitmapSlider::DrawTransparentBitmap(CDC *pDC, int xStart, int yStart, int wWidth, int wHeight,
                                          CDC *pTmpDC, int xSource, int ySource, CBitmap *bmMask )
{
    CDC hdcMem;

    (void)hdcMem.CreateCompatibleDC(NULL);
    CBitmap* hbmT = hdcMem.SelectObject(bmMask);
    (void)pDC->BitBlt( xStart, yStart, wWidth, wHeight, &hdcMem,
                       xSource, ySource, SRCAND);

    (void)pDC->BitBlt(xStart, yStart, wWidth, wHeight, pTmpDC,
                      xSource, ySource, SRCPAINT);

    (void)hdcMem.SelectObject(hbmT);
    (void)hdcMem.DeleteDC();
}//lint !e1762

UINT CBitmapSlider::OnGetDlgCode()
{
    if (GetKeyState(VK_TAB) >= 0)
    {
        return DLGC_WANTALLKEYS;
    }

    return CSliderCtrl::OnGetDlgCode();
}

void CBitmapSlider::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (!m_bEnable)
    {
        return;
    }

    switch (nChar)
    {
        // Right & down
    case VK_RIGHT:
    case VK_DOWN:
        SetPos(m_nPos + 1 );
        break;
		// Page down
	case VK_NEXT:
		SetPos(m_nPos + m_nPage );
		break;


        // Home
    case VK_HOME:
        SetPos(m_nMin );
        break;

        // End
    case VK_END:
        SetPos(m_nMax );
        break;

		// Page up
	case VK_PRIOR:
		SetPos(m_nPos - m_nPage );
		break;

		// Left & up
	case VK_LEFT:
	case VK_UP:
		SetPos(m_nPos - 1 );
		break;

    default:
        CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    ::PostMessage(
        GetParent()->GetSafeHwnd(), WM_BITMAPSLIDER_MOVED,
        static_cast <UINT>(GetDlgCtrlID()), static_cast <long>(m_nPos));

    CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CBitmapSlider::OnKillFocus(CWnd* pNewWnd)
{
    CSliderCtrl::OnKillFocus(pNewWnd);

    m_bFocus = FALSE;
    Invalidate();
}

void CBitmapSlider::OnSetFocus(CWnd* pOldWnd)
{
    CSliderCtrl::OnSetFocus(pOldWnd);

    m_bFocus = TRUE;
    Invalidate();
}

void CBitmapSlider::OnDestroy()
{
    CSliderCtrl::OnDestroy();

    (void)m_bmThumb.DeleteObject();
    (void)m_bmThumbMask.DeleteObject();

    (void)m_bmThumbActive.DeleteObject();
    (void)m_bmThumbActiveMask.DeleteObject();

    (void)m_bmThumbBg.DeleteObject();

    (void)m_bmChannel.DeleteObject();
    (void)m_bmChannelMask.DeleteObject();

    (void)m_bmChannelActive.DeleteObject();
    (void)m_bmChannelActiveMask.DeleteObject();
}

void CBitmapSlider::DrawThumb( CDC &dcMem, XCMemDC& dc )
{
    CBitmap * pbmTmp;
    if (m_bThumb && m_bEnable)
    {
        if (m_bThumbActive && m_bLButtonDown)
        {
            pbmTmp = dcMem.SelectObject(&m_bmThumbActive);
        }                                                    
        else
        {
            pbmTmp = dcMem.SelectObject(&m_bmThumb);
        }                                              
        if (m_bVertical)
        {
            if (m_bTransparentChannel || !m_bChannel)
            {
                m_nThumbBgX = m_nMarginLeft;
                m_nThumbBgY = Pos2Pixel(m_nPos) - m_nThumbHeight / 2;

                CopyBackground(
                    &dc, m_nThumbBgX, m_nThumbBgY,
                    m_nThumbWidth, m_nThumbHeight, &m_bmThumbBg );
            }

            DrawBitmap(
                &dc, m_nMarginLeft, Pos2Pixel(m_nPos) - m_nThumbHeight / 2,
                m_nThumbWidth, m_nThumbHeight,
                &dcMem, 0, 0, &m_bmThumbMask, m_bTransparentThumb );
        }
        else
        {
            // Background image is need to be restored
            if (m_bTransparentChannel || !m_bChannel)
            {
                m_nThumbBgX = Pos2Pixel(m_nPos) - m_nThumbWidth / 2;
                m_nThumbBgY = m_nMarginTop;

                CopyBackground(
                    &dc, m_nThumbBgX, m_nThumbBgY,
                    m_nThumbWidth, m_nThumbHeight, &m_bmThumbBg );
            }

            DrawBitmap(
                &dc, Pos2Pixel(m_nPos) - m_nThumbWidth / 2, m_nMarginTop,
                m_nThumbWidth, m_nThumbHeight,
                &dcMem, 0, 0, &m_bmThumbMask, m_bTransparentThumb );
        } // if horizontal

        (void)dcMem.SelectObject(pbmTmp);
    } // if draw thumb	return pbmTmp;
}

void CBitmapSlider::DrawChannel(CDC &dcMem, XCMemDC& dc)
{
    CBitmap * pbmTmp;
    if (m_bChannel)
    {
        pbmTmp = dcMem.SelectObject(&m_bmChannel);
        if (m_bChannelActive && m_bEnable)
        {
            if (m_bVertical)
            {
				 DrawBitmap( &dc, 0, 0, m_nWidth, Pos2Pixel(m_nPos),
                            &dcMem, 0, 0, &m_bmChannelMask, m_bTransparentChannel );

                (void)dcMem.SelectObject(&m_bmChannelActive);
				DrawBitmap( &dc, 0, Pos2Pixel(m_nPos),
                            m_nWidth, m_nHeight - Pos2Pixel(m_nPos),
                            &dcMem, 0, Pos2Pixel(m_nPos),
                            &m_bmChannelActiveMask, m_bTransparentChannel );
            }
            else
            {
                DrawBitmap( &dc, Pos2Pixel(m_nPos), 0,
                            m_nWidth - Pos2Pixel(m_nPos), m_nHeight,
                            &dcMem, Pos2Pixel(m_nPos), 0,
                            &m_bmChannelActiveMask, m_bTransparentChannel );

                (void)dcMem.SelectObject( &m_bmChannelActive );
                DrawBitmap( &dc, 0, 0, Pos2Pixel(m_nPos), m_nHeight,
                            &dcMem, 0, 0, &m_bmChannelMask, m_bTransparentChannel );
            }
        }
        else
        {
            DrawBitmap( &dc, 0, 0, m_nWidth, m_nHeight,
                        &dcMem, 0, 0, &m_bmChannelMask, m_bTransparentChannel );
        }

        (void)dcMem.SelectObject( pbmTmp );
    }
}

HBITMAP CBitmapSlider::StretchImage(UINT nBitmapID, int nWidth, int nHeight)
{
	(void)AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CBitmap cbitmap;
    (void)cbitmap.LoadBitmap(nBitmapID);
    HBITMAP hBitmap = (HBITMAP)cbitmap;
    BITMAP bmpInfo;

    //获取位图信息
    (void)GetObject(hBitmap, sizeof(BITMAP), (LPBYTE)&bmpInfo);

    HDC hDC = ::GetDC(m_hWnd);
    HDC hdcMem = CreateCompatibleDC(hDC);

    //选择位图
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    HBITMAP hdstBmp = CreateCompatibleBitmap(hdcMem, nWidth, nHeight);
    HDC hDcTemp = CreateCompatibleDC(hDC);
    HBITMAP hOldBmpTemp = (HBITMAP)SelectObject(hDcTemp, hdstBmp);
    StretchBlt(hDcTemp, 0, 0, nWidth, nHeight, hdcMem, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY );

    hdstBmp = (HBITMAP)SelectObject(hDcTemp, hOldBmpTemp);
    DeleteDC(hDcTemp); //删除DC，否则Bitmap不可同时使用
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
	DeleteObject(hOldBmpTemp);
	DeleteObject(hOldBitmap);
    DeleteDC(hdcMem); //
	DeleteDC(hDC);

    return hdstBmp;
}

void CBitmapSlider::SetToolTipText(LPCTSTR lpszToolTipText)
{
    if (m_ToolTip.GetSafeHwnd())
    {
        m_ToolTip.UpdateTipText(lpszToolTipText, this);
    }
}

void CBitmapSlider::AddToolTip(void)
{
    if (!m_ToolTip.GetSafeHwnd())
    {
        (void)m_ToolTip.Create(this);
    }

    (void)m_ToolTip.AddTool(this);
}

void CBitmapSlider::RelayToolTipEvent(const CPoint &point )
{
    if (m_ToolTip.m_hWnd != NULL)
    {
        MSG msg;
        msg.hwnd = m_hWnd;
        msg.message = WM_MOUSEMOVE;
        msg.time = 0;
        msg.pt.x = LOWORD(point.y);
        msg.pt.y = HIWORD(point.y);
		msg.wParam = LOWORD(point.x);
		msg.lParam = LOWORD(point.y);

        m_ToolTip.RelayEvent(&msg);
    }
}

BOOL CBitmapSlider::SetBitmapThumb(UINT nThumbID, UINT nActiveID, BOOL bTransparent,
	COLORREF clrpTransColor, int iTransPixelX, int iTransPixelY )
{
	(void)AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!nThumbID)
	{
		m_bThumb = FALSE;
		(void)m_bmThumb.DeleteObject();
		(void)m_bmThumbMask.DeleteObject();
		(void)m_bmThumbActive.DeleteObject();
		(void)m_bmThumbActiveMask.DeleteObject();
		(void)m_bmThumbBg.DeleteObject();

		return TRUE;
	}

	(void)m_bmThumb.DeleteObject();
	if (!m_bmThumb.LoadBitmap(nThumbID))
	{
		return FALSE;
	}

	if (bTransparent)
	{
		PrepareMask(&m_bmThumb, &m_bmThumbMask,
			clrpTransColor, iTransPixelX, iTransPixelY );
	}

	if (nActiveID)
	{
		(void)m_bmThumbActive.DeleteObject();
		if (!m_bmThumbActive.LoadBitmap( nActiveID ))
		{
			(void)m_bmThumb.DeleteObject();
			if (bTransparent)
			{
				(void)m_bmThumbMask.DeleteObject();
			}

			return FALSE;
		}

		if (bTransparent)
		{
			PrepareMask( &m_bmThumbActive, &m_bmThumbActiveMask,
				clrpTransColor, iTransPixelX, iTransPixelY );
		}

		m_bThumbActive = TRUE;
	}
	else
	{
		m_bThumbActive = FALSE;
	}
	BITMAP bitmap = {0};
	(void)m_bmThumb.GetBitmap( &bitmap );

	m_nThumbWidth  = bitmap.bmWidth;
	m_nThumbHeight = bitmap.bmHeight;

	if (!m_bChannel)
	{
		GetClientRect(&m_rectWindow );
		m_nHeight = m_rectWindow.Height();
		m_nWidth = m_rectWindow.Width();
	}

	ASSERT( m_nThumbWidth <= m_nWidth && m_nThumbHeight <= m_nHeight );

	if (m_bThumbActive)
	{
		BITMAP bitmapActive;
		(void)m_bmThumbActive.GetBitmap( &bitmapActive );

		ASSERT(
			m_nThumbWidth == bitmapActive.bmWidth
			&& m_nThumbHeight == bitmapActive.bmHeight );
	}

	m_bTransparentThumb = bTransparent;
	m_bThumb = TRUE;

	return TRUE;
}
