// TranlucentButton.cpp : implementation file
#include "stdafx.h"
#include "TranslucentButton.h"
#include "TranslucentUtility.h"
#include "ToolsHelp.h"
#include "SDKConfig.h"
#include "SkinMgr.h"
using std::tr1::array;

// CTranslucentButton

IMPLEMENT_DYNAMIC(CTranslucentButton, CTranslucentWnd)

CTranslucentButton::CTranslucentButton()
	: CTranslucentWnd()
	, m_pCurrImage(NULL)
	, m_bMouseOver(false)
	, m_bPressed(false)
	, m_bSelected(false)
	, m_format(0,LANG_NEUTRAL)
	, m_pFont(nullptr)
	, m_brush(Gdiplus::Color(255,255 ,255))
{
    TCHAR wsFile[MAX_PATH] = {_T("Arial")};
	WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
	m_pFont = ::new Gdiplus::Font(wstr, 8);  //lint !e1732 !e1733 !e747  GDIplus ¿â
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
   
	m_imageList.resize(TWS_BUTTON_NUM, NULL);
     
	(void)m_format.SetAlignment(Gdiplus::StringAlignmentNear);
	(void)m_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	// ÉèÖÃ²»ÊÇ¼¤»î´°¿ÚÒ²¿ÉÒÔÏÔÊ¾
	long dwStyle = ::GetWindowLong(m_ToolTip.GetSafeHwnd(), GWL_STYLE);
	dwStyle |= TTS_ALWAYSTIP;

	(void)::SetWindowLong(m_ToolTip.GetSafeHwnd(), GWL_STYLE, dwStyle);
	//m_ToolTip.SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE |SWP_NOSIZE | SWP_SHOWWINDOW  | SWP_NOACTIVATE);
}

CTranslucentButton::~CTranslucentButton()
{
    try
    {
        ::delete m_pFont;
    }
    catch (...)
    {
    }
}//lint !e1740 ÓÃGDIÊÍ·Å

BEGIN_MESSAGE_MAP(CTranslucentButton, CTranslucentWnd)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

// CTranslucentButton message handlers

void CTranslucentButton::PreSubclassWindow()
{
	// ×ÖÌå¿ÉÅäÖÃ
	(void)CSDKConfig::instance().LoadConfigFile();
	std::string strFontFamily = CSDKConfig::instance().GetFontFamilyValue();
	if (strFontFamily.empty())
	{
		strFontFamily = "Arial";
	}

	if (m_pFont != NULL)
	{
		::delete m_pFont;
	}

	WCHAR* wstr = CTranslucentUtility::ttowc(strFontFamily.c_str());
	m_pFont = ::new Gdiplus::Font((wstr), 8);  //lint !e1732 !e1733 !e747  GDIplus ¿â
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	
	// ³õÊ¼»¯tooltip
	if (NULL == m_ToolTip.GetSafeHwnd())
	{
		TCHAR szText[MAX_PATH + 1];
		::GetWindowText(m_hWnd, szText, MAX_PATH);
		m_ToolTip.Create(this);
		m_ToolTip.SetTipBkColor(RGB(70, 70, 70)); // ÉèÖÃÌáÊ¾ÎÄ±¾±³¾°ÑÕÉ«
		m_ToolTip.SetTipTextColor(RGB(255, 255, 255)); // ÉèÖÃÌáÊ¾ÎÄ±¾×ÖÌåÑÕÉ«
		
		CFont* pFont = m_ToolTip.GetFont();
		LOGFONT logFont; 
		pFont->GetLogFont(&logFont);
		strncpy(logFont.lfFaceName,strFontFamily.c_str(), sizeof(logFont.lfFaceName) - 1);
		m_ToolTipFont.CreateFontIndirectA(&logFont);
		m_ToolTip.SetFont(&m_ToolTipFont);
		m_ToolTip.AddTool(this, szText);
		m_ToolTip.Activate(TRUE);
	}

	CTranslucentWnd::PreSubclassWindow();
}

bool CTranslucentButton::LoadImageList(const LPCTSTR szImageList[TWS_BUTTON_NUM])
{
	DestroyImageList();

	if (szImageList[TWS_NORMAL] == NULL)
		return false;
    TCHAR wsFile[MAX_PATH] = {0};
	WCHAR* wstr = NULL;
    memset(wsFile,0x0,MAX_PATH);
    if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_NORMAL],strnlen(szImageList[TWS_NORMAL],MAX_PATH)))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List"));
        return false;
    }
	wstr = CTranslucentUtility::ttowc(wsFile);
	m_imageList[TWS_NORMAL] = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	if (m_imageList[TWS_NORMAL] == NULL)
		return false;

	if (szImageList[TWS_MOUSEOVER] != NULL)
    {
        memset(wsFile,0x0,sizeof(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_MOUSEOVER],strnlen(szImageList[TWS_MOUSEOVER],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		m_imageList[TWS_MOUSEOVER] = Gdiplus::Image::FromFile(wstr);
		if (NULL != wstr)
		{
			delete []wstr;
			wstr = NULL;
		}
    }

	if (szImageList[TWS_PRESSED] != NULL)
    {
        memset(wsFile,0x0,sizeof(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_PRESSED],strnlen(szImageList[TWS_PRESSED],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		m_imageList[TWS_PRESSED] = Gdiplus::Image::FromFile(wstr);
		if (NULL != wstr)
		{
			delete []wstr;
			wstr = NULL;
		}
    }

	if (szImageList[TWS_DISABLED] != NULL)
    {
        memset(wsFile,0x0,sizeof(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_DISABLED],strnlen(szImageList[TWS_DISABLED],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		m_imageList[TWS_DISABLED] = Gdiplus::Image::FromFile(wstr);
		if (NULL != wstr)
		{
			delete []wstr;
			wstr = NULL;
		}
    }

	if (szImageList[TWS_SELECTED] != NULL)
    {
        memset(wsFile,0x0,strlen(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_SELECTED],strnlen(szImageList[TWS_SELECTED],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		m_imageList[TWS_SELECTED] = Gdiplus::Image::FromFile(wstr);
		if (NULL != wstr)
		{
			delete []wstr;
			wstr = NULL;
		}
    }

	m_pCurrImage = m_imageList[TWS_NORMAL];

	VERIFY(SetWindowPos(NULL, -1, -1, static_cast<int>(m_pCurrImage->GetWidth()),  static_cast<int>(m_pCurrImage->GetHeight()), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return true;
}

bool CTranslucentButton::LoadImageList( const UINT imageIDList[TWS_BUTTON_NUM], LPCTSTR lpszResType, HINSTANCE hInstance)
{
	DestroyImageList();

	if (imageIDList[TWS_NORMAL] == 0)
		return false;

	m_imageList[TWS_NORMAL] = CTranslucentUtility::LoadImage(imageIDList[TWS_NORMAL], lpszResType, hInstance);
	if (m_imageList[TWS_NORMAL] == NULL)
		return false;

	if (imageIDList[TWS_MOUSEOVER] != 0)
		m_imageList[TWS_MOUSEOVER] = CTranslucentUtility::LoadImage(imageIDList[TWS_MOUSEOVER], lpszResType, hInstance);

	if (imageIDList[TWS_PRESSED] != 0)
		m_imageList[TWS_PRESSED] = CTranslucentUtility::LoadImage(imageIDList[TWS_PRESSED], lpszResType, hInstance);

	if (imageIDList[TWS_DISABLED] != 0)
		m_imageList[TWS_DISABLED] = CTranslucentUtility::LoadImage(imageIDList[TWS_DISABLED], lpszResType, hInstance);


	if (imageIDList[TWS_SELECTED] != 0)
		m_imageList[TWS_SELECTED] = CTranslucentUtility::LoadImage(imageIDList[TWS_SELECTED], lpszResType, hInstance);

	m_pCurrImage = m_imageList[TWS_NORMAL];

	VERIFY(SetWindowPos(NULL, -1, -1, static_cast<int >(m_pCurrImage->GetWidth()), static_cast<int>(m_pCurrImage->GetHeight()), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return true;
}

bool CTranslucentButton::LoadImageList( const TCHAR szImageList[TWS_BUTTON_NUM][MAX_PATH] )
{
	DestroyImageList();

	if (szImageList[TWS_NORMAL] == NULL)
		return false;

	TCHAR wsFile[MAX_PATH] = {0};
	WCHAR* wstr = NULL;
	memset(wsFile,0x0,sizeof(wsFile));
    if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_NORMAL],strnlen(szImageList[TWS_NORMAL],MAX_PATH)))
    {
        return false;
    }
	wstr = CTranslucentUtility::ttowc(wsFile);
	if (NULL != wstr)
	{
		try 
		{
			m_imageList[TWS_NORMAL] = Gdiplus::Image::FromFile(wstr);
		}
		catch(...)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Load Image List", "Load %s Fail", wsFile);
		}
		
		delete []wstr;
		wstr = NULL;
	}

	if (szImageList[TWS_NORMAL] == NULL)
		return false;

	if (szImageList[TWS_MOUSEOVER] != NULL)
	{
		memset(wsFile,0x0,sizeof(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_MOUSEOVER],strnlen(szImageList[TWS_MOUSEOVER],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List has over status failed"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		if (NULL != wstr)
		{
			try 
			{
				m_imageList[TWS_MOUSEOVER] = Gdiplus::Image::FromFile(wstr);
			}
			catch (...)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Load Image List", "Load %s Fail", wsFile);
			}
			
			delete []wstr;
			wstr = NULL;
		}
	}

	if (szImageList[TWS_PRESSED] != NULL)
	{
        memset(wsFile,0x0,sizeof(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_PRESSED],strnlen(szImageList[TWS_PRESSED],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List has press status failed"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		
		if (NULL != wstr)
		{
			try 
			{
				m_imageList[TWS_PRESSED] = Gdiplus::Image::FromFile(wstr);
			}
			catch (...)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Load Image List", "Load %s Fail", wsFile);
			}
			
			delete []wstr;
			wstr = NULL;
		}
	}

	if (szImageList[TWS_DISABLED] != NULL)
	{
		memset(wsFile,0x0,sizeof(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_DISABLED],strnlen(szImageList[TWS_DISABLED],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List has disable status failed"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		
		if (NULL != wstr)
		{
			try 
			{
				m_imageList[TWS_DISABLED] = Gdiplus::Image::FromFile(wstr);
			}
			catch(...)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Load Image List", "Load %s Fail", wsFile);
			}
		
			delete []wstr;
			wstr = NULL;
		}
	}

	if (szImageList[TWS_SELECTED] != NULL)
	{
		memset(wsFile,0x0,strlen(wsFile));
        if (!CToolsHelp::Strncpy(wsFile,MAX_PATH,szImageList[TWS_SELECTED],strnlen(szImageList[TWS_SELECTED],MAX_PATH)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("TranslucentButton Load Image List has select status failed"));
            return false;
        }
		wstr = CTranslucentUtility::ttowc(wsFile);
		
		if (NULL != wstr)
		{
			try 
			{
				m_imageList[TWS_SELECTED] = Gdiplus::Image::FromFile(wstr);
			}
			catch (...)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Load Image List", "Load %s Fail", wsFile);
			}
		
			delete []wstr;
			wstr = NULL;
		}
	}

	m_pCurrImage = m_imageList[TWS_NORMAL];

	VERIFY(SetWindowPos(NULL, -1, -1, static_cast<int>(m_pCurrImage->GetWidth()),  static_cast<int>(m_pCurrImage->GetHeight()), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return true;
}

void CTranslucentButton::SetMouseOverBtnImage()
{
	if (m_bPressed)
	{
		if (m_imageList[TWS_PRESSED] != NULL)
			m_pCurrImage = m_imageList[TWS_PRESSED];
		else
			m_pCurrImage = m_imageList[TWS_NORMAL];
	} else {
		if (m_imageList[TWS_MOUSEOVER] != NULL)
			m_pCurrImage = m_imageList[TWS_MOUSEOVER];
		else
			m_pCurrImage = m_imageList[TWS_NORMAL];
	}
}

void CTranslucentButton::UpdateState()
{
	Gdiplus::Image* prevImage = m_pCurrImage;

	if (m_bMouseOver)
	{
		SetMouseOverBtnImage();
	} else if (m_bDisabled) {
		if (m_imageList[TWS_DISABLED] != NULL)
			m_pCurrImage = m_imageList[TWS_DISABLED];
		else
			m_pCurrImage = m_imageList[TWS_NORMAL];
	}
	else if (m_bSelected)
	{
		if (m_imageList[TWS_SELECTED] != NULL)
			m_pCurrImage = m_imageList[TWS_SELECTED];
		else
			m_pCurrImage = m_imageList[TWS_NORMAL];
	} else {
		m_pCurrImage = m_imageList[TWS_NORMAL];
	}

	if (prevImage != m_pCurrImage)
	{
		NotifyRender();
	}
}

void CTranslucentButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseOver)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bMouseOver = (_TrackMouseEvent(&tme) == FALSE) ? false : true;
	}

    UpdateState();
	// tooltip
	if (m_ToolTip.GetSafeHwnd())
	{
		//¹¹ÔìÒ»¸öMSG
		MSG msg;

		msg.hwnd = m_hWnd;
		msg.message = WM_MOUSEMOVE;
		msg.wParam = LOWORD(point.x);
		msg.lParam = LOWORD(point.y);
		msg.time = 0;
		msg.pt.x = LOWORD(point.x);
		msg.pt.y = HIWORD(point.y);
		if (1 == SkinMgr.GetSkinType())
		{
			m_ToolTip.SetTipBkColor(RGB(43,43,43));
			m_ToolTip.SetTipTextColor(RGB(255,255,255));
		}
		else
		{
			m_ToolTip.SetTipBkColor(RGB(206,222,234));
			m_ToolTip.SetTipTextColor(RGB(82,82,82));
		}
		
		m_ToolTip.RelayEvent(&msg);
		// °ÑÒÆ¶¯µÄ¹¤¾ßÌõ·ÅÔÚ×îÇ°¶Ë£¬Ê¹µÃtipÄÜÏÔÊ¾³öÀ´
		this->GetParent()->BringWindowToTop();
	}

	CTranslucentWnd::OnMouseMove(nFlags, point);
}

LRESULT CTranslucentButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = false;
	UpdateState();
	return IVS_SUCCEED;
}//lint !e715

LRESULT CTranslucentButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = true;
	UpdateState();
	return IVS_SUCCEED;
}//lint !e715

void CTranslucentButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	::PostMessage(GetParent()->GetSafeHwnd(),WM_TRANS_BTN_DOWN,(ULONG)m_hWnd,0);
	m_ToolTip.Activate(FALSE);
	m_bPressed = true;
	UpdateState();
	CTranslucentWnd::OnLButtonDown(nFlags, point);
}

void CTranslucentButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	::PostMessage(GetParent()->GetSafeHwnd(),WM_TRANS_BTN_UP,(ULONG)m_hWnd,0);
	m_bPressed = false;
	RECT rc;
	GetClientRect(&rc);
	BOOL bIn = ::PtInRect(&rc, point);
	if (bIn)
	{
		UINT uStyle = (UINT)::GetWindowLong(m_hWnd, GWL_STYLE) & 0xff;
		if ((uStyle == BS_RADIOBUTTON) || (uStyle == BS_CHECKBOX) || (uStyle == BS_AUTOCHECKBOX) || (uStyle == BS_AUTORADIOBUTTON))
			m_bSelected = !m_bSelected;
		else
			m_bSelected = false;
	}
	else
	{
		m_bMouseOver = false;
	}

	UpdateState();
	m_ToolTip.Activate(TRUE);
	CTranslucentWnd::OnLButtonUp(nFlags, point);
}

void CTranslucentButton::SetChecked(bool b)
{
	m_bSelected = b;
	UpdateState();
}

bool CTranslucentButton::IsChecked() const
{
	return m_bSelected;
}

void CTranslucentButton::SetFont(const WCHAR* familyName, float emSize, int style, Gdiplus::Unit unit/* = Gdiplus::UnitPixel*/)
{
	::delete m_pFont;
	m_pFont = ::new Gdiplus::Font(familyName, emSize, style, unit);
}

void CTranslucentButton::SetFormat(Gdiplus::StringAlignment align, Gdiplus::StringAlignment lineAlign/* = Gdiplus::StringAlignmentCenter*/)
{
	(void)m_format.SetAlignment(align);
	(void)m_format.SetLineAlignment(lineAlign);
}

void CTranslucentButton::SetColor(const Gdiplus::Color& color)
{
	(void)m_brush.SetColor(color);
}

void CTranslucentButton::Render(Gdiplus::Graphics& g)
{
    CHECK_POINTER_VOID(m_pCurrImage);
    CRect rc;
    GetWindowRect(&rc);
	if (GetParent() == NULL)
	{
		return;
	}
    GetParent()->ScreenToClient(&rc);

    Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)m_pCurrImage->GetWidth(), (float)m_pCurrImage->GetHeight());
    (void)g.DrawImage(m_pCurrImage, rect);

    CString szText;
    GetWindowText(szText);
    if (szText.GetLength() > 0)
    {
        TCHAR wsFile[MAX_PATH] = {0};
        memset(wsFile,0x0,sizeof(wsFile));
        if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)szText,static_cast<IVS_UINT32>(szText.GetLength())))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS_OCX::"," Translucent Button Render");
            return;
        };
        rect.X += m_pCurrImage->GetWidth();
		WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
		if (NULL != wstr)
		{
			(void)g.DrawString(wstr, static_cast<int >(wcslen(wstr)), m_pFont, rect, &m_format, &m_brush);
			delete []wstr;
			wstr = NULL;
		}
    }
}

void CTranslucentButton::UpdateTipText(const std::string& strTip)
{
	m_ToolTip.UpdateTipText(strTip.c_str(),this);
}

void CTranslucentButton::GetTipText(CString& strTipText)
{
	m_ToolTip.GetText(strTipText, this);
}



void CTranslucentButton::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (VK_RETURN == nChar)
	{
		return;
	}

	::PostMessage(GetParent()->GetSafeHwnd(),WM_KEYUP,static_cast<LONG>(nChar),0); //lint !e732

	CTranslucentWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
