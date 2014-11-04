// Core\TranslucentProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "TranslucentProgressBar.h"
#include "ToolsHelp.h"
#include "TranslucentUtility.h"

// 第三方开源lint不检查

// CTranslucentProgressBar

IMPLEMENT_DYNAMIC(CTranslucentProgressBar, CTranslucentWnd)
    CTranslucentProgressBar::CTranslucentProgressBar()
    : CTranslucentWnd()
    , m_nLower(0)
    , m_nUpper(100)
    , m_nStep(1)
    , m_nPos(0)
	, m_format(0,LANG_NEUTRAL)
    , m_brush(Gdiplus::Color(0, 0, 0))
{
	WCHAR* wstr = CTranslucentUtility::ttowc("Arial");
	m_pFont = ::new Gdiplus::Font(wstr, static_cast<float>(12)); //lint !e1732 !e1733 !e1776  MFC自动生成的的函数
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

    m_imageList.resize(2, NULL);
    m_format.SetAlignment(Gdiplus::StringAlignmentCenter);
    m_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
}

CTranslucentProgressBar::~CTranslucentProgressBar()
{
    try
    {
        ::delete m_pFont;
    }
    catch (...)
    {
    }
}//lint !e1740 GDIplus  

BEGIN_MESSAGE_MAP(CTranslucentProgressBar, CTranslucentWnd)
END_MESSAGE_MAP()				//lint !e751  MFC自动生成的的函数

bool CTranslucentProgressBar::SetBgImage(LPCTSTR szBgImage)
{
    DestroyImage(0);

    TCHAR wsFile[MAX_PATH] = {0};
    if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)szBgImage,strnlen(szBgImage,MAX_PATH)))
    {
    	BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS_OCX::","Translucent ProgressBar Set Image.Failed");
        return false;
    }
	WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
	m_imageList[0] = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
    
    return (m_imageList[0] != NULL);
}

bool CTranslucentProgressBar::SetBgImage(UINT nBgImageID, LPCTSTR lpszResType, HINSTANCE hInstance)
{
    DestroyImage(0);

    m_imageList[0] = CTranslucentUtility::LoadImage(nBgImageID, lpszResType, hInstance);
    return (m_imageList[0] != NULL);
}

bool CTranslucentProgressBar::SetFgImage(LPCTSTR szFgImage)
{
    DestroyImage(1);

    TCHAR wsFile[MAX_PATH] = {0};
    if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)szFgImage,strnlen(szFgImage,MAX_PATH)))
    {
    	BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS_OCX::","Translucent ProgressBar Set front Image.Faile");
        return false;
    }
	WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
	 m_imageList[1] = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

    return (m_imageList[1] != NULL);
}

bool CTranslucentProgressBar::SetFgImage(UINT nFgImageID, LPCTSTR lpszResType, HINSTANCE hInstance)
{
    DestroyImage(1);

    m_imageList[1] = CTranslucentUtility::LoadImage(nFgImageID, lpszResType, hInstance);
    return (m_imageList[1] != NULL);
}

void CTranslucentProgressBar::GetRange(int& lower, int& upper) const
{
    lower = m_nLower;
    upper = m_nUpper;
}

int CTranslucentProgressBar::GetPos() const
{
    return m_nPos;
}

int CTranslucentProgressBar::GetStep() const
{
    return m_nStep;
}

void CTranslucentProgressBar::StepIt()
{
    m_nPos += m_nStep;
    if (m_nPos > m_nUpper)
        m_nPos = m_nUpper;
    else if (m_nPos < m_nLower)
        m_nPos = m_nLower;

    UpdateState();
}

BOOL CTranslucentProgressBar::SetStep(int nStep)
{
    if (nStep != 0)
    {
        m_nStep = nStep;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CTranslucentProgressBar::OffsetPos(int nPos)
{
    if (nPos != 0)
    {
        m_nPos += nPos;
        if (m_nPos > m_nUpper)
            m_nPos = m_nUpper;
        else if (m_nPos < m_nLower)
            m_nPos = m_nLower;

        UpdateState();
    }
}

BOOL CTranslucentProgressBar::SetPos(int nPos)
{
    if (nPos >= m_nLower && nPos <= m_nUpper)
    {
        m_nPos = nPos;
        UpdateState();

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CTranslucentProgressBar::SetRange(int nLower, int nUpper)
{
    VERIFY(nLower != nUpper);

    if (nUpper > nLower)
    {
        m_nLower = nLower;
        m_nUpper = nUpper;
    }
    else
    {
        m_nLower = nUpper;
        m_nUpper = nLower;
    }

    if (m_nPos < m_nLower)
    {
        m_nPos = m_nLower;
        UpdateState();
    }
    else if (m_nPos > m_nUpper)
    {
        m_nPos = m_nUpper;
        UpdateState();
    }
}

void CTranslucentProgressBar::SetFont(const WCHAR* familyName, float emSize, int style/* = Gdiplus::FontStyleRegular*/, Gdiplus::Unit unit/* = Gdiplus::UnitPixel*/)
{
    ::delete m_pFont;
    m_pFont = ::new Gdiplus::Font(familyName, emSize, style, unit);
}

void CTranslucentProgressBar::SetFormat(Gdiplus::StringAlignment align/* = Gdiplus::StringAlignmentCenter*/, Gdiplus::StringAlignment lineAlign/* = Gdiplus::StringAlignmentCenter*/)
{
    m_format.SetAlignment(align);
    m_format.SetLineAlignment(lineAlign);
}

void CTranslucentProgressBar::SetColor(const Gdiplus::Color& color)
{
    m_brush.SetColor(color);
}

void CTranslucentProgressBar::Render(Gdiplus::Graphics& g)
{
    float ratio = (float)m_nPos / (m_nUpper - m_nLower);

    CRect rc;
    GetWindowRect(&rc);
    GetParent()->ScreenToClient(&rc);

    Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
    if (m_imageList[0] != NULL)
        g.DrawImage(m_imageList[0], rect, static_cast<float>(0), static_cast<float>(0), rect.Width, rect.Height, Gdiplus::UnitPixel);

    if (m_imageList[1] != NULL)
    {
        float nWidth = rc.Width() * ratio;
        rect.Width = nWidth;
        (void)g.DrawImage(m_imageList[1], rect, static_cast<float>(0), static_cast<float>(0), nWidth, rect.Height, Gdiplus::UnitPixel);
        rect.Width = (float)rc.Width();
    }

    CString szText;
    GetWindowText(szText);
    if (szText.GetLength() > 0)
    {
        TCHAR wsFile[MAX_PATH] = {0};
        if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)szText,static_cast<IVS_UINT32>(szText.GetLength())))
        {
        	BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS_OCX::","Failed");
            return;
        }
		WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);

		if (NULL != wstr)
		{
			(void)g.DrawString(wstr, static_cast<int >(wcslen(wstr)), m_pFont, rect, &m_format, &m_brush);
			delete []wstr;
			wstr = NULL;
		}
    }
}
