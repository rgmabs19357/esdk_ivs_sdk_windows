// Core\TranslucentStatic.cpp : implementation file
//

#include "stdafx.h"
#include "TranslucentStatic.h"
//#include "IVS_OCXPlayer.h"
#include "TranslucentUtility.h"
#include "ToolsHelp.h"
#include "SDKConfig.h"

// CTranslucentStatic

IMPLEMENT_DYNAMIC(CTranslucentStatic, CTranslucentWnd)

CTranslucentStatic::CTranslucentStatic()
	: m_pCurrImage(NULL)
	, m_format(0,LANG_NEUTRAL)
	, m_pFont(nullptr)
	, m_brush(Gdiplus::Color(245,245 ,245))
{
	TCHAR wsFile[MAX_PATH] = {_T("Arial")};
	WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
	m_pFont = ::new Gdiplus::Font(wstr, 9);  //lint !e1732 !e1733 !e747  GDIplus ø‚
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	
	(void)m_imageList.resize(TWS_BUTTON_NUM, NULL);//lint !e526 !e628 !e746 !e792
	
	(void)m_format.SetAlignment(Gdiplus::StringAlignmentNear);
	(void)m_format.SetLineAlignment(Gdiplus::StringAlignmentNear);
}

CTranslucentStatic::~CTranslucentStatic()
{
	try
	{
		m_pCurrImage = NULL;
		::delete m_pFont;
	}
	catch (...)
	{
	}
}


BEGIN_MESSAGE_MAP(CTranslucentStatic, CWnd)
END_MESSAGE_MAP()//lint !e751

void CTranslucentStatic::SetFont(const WCHAR* familyName, float emSize, int style, Gdiplus::Unit unit/* = Gdiplus::UnitPixel*/)
{
	::delete m_pFont;
	m_pFont = ::new Gdiplus::Font(familyName, emSize, style, unit);
}

void CTranslucentStatic::SetFormat(Gdiplus::StringAlignment align, Gdiplus::StringAlignment lineAlign/* = Gdiplus::StringAlignmentCenter*/)
{
	(void)m_format.SetAlignment(align);
	(void)m_format.SetLineAlignment(lineAlign);
}

void CTranslucentStatic::SetColor(const Gdiplus::Color& color)
{
	(void)m_brush.SetColor(color);
}

void CTranslucentStatic::Render( Gdiplus::Graphics& g )
{
	CRect rc;
	GetWindowRect(&rc);//lint !e64 !e118
	GetParent()->ScreenToClient(&rc);//lint !e64 !e118
	
	Gdiplus::RectF rect;

	rect.X = static_cast<REAL>(rc.left);
	rect.Y = static_cast<REAL>(rc.top);
	rect.Width = static_cast<REAL>(rc.Width());
	rect.Height = static_cast<REAL>(rc.Height());

	if (m_pCurrImage != NULL)
	{
		rect.Width = static_cast<REAL>(m_pCurrImage->GetWidth());
		rect.Height = static_cast<REAL>(m_pCurrImage->GetHeight());
		(void)g.DrawImage(m_pCurrImage, rect);
	}


	CString szText;
	GetWindowText(szText);//lint !e64 !e118
	if (szText.GetLength() > 0)
	{
		TCHAR wsFile[MAX_PATH] = {0};
		memset(wsFile,0x0,sizeof(wsFile));
		if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)szText,static_cast<IVS_UINT32>(szText.GetLength())))//lint !e526 !e628 !e746 !e1055
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS_OCX::"," Translucent Button Render");
			return;
		};
		rect.X += 2;
		WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
		if (NULL != wstr)
		{
			(void)g.DrawString(wstr, static_cast<int >(wcslen(wstr)), m_pFont, rect, &m_format, &m_brush);
			delete []wstr;
			wstr = NULL;
		}
	}

}

void CTranslucentStatic::PreSubclassWindow()
{
	// ◊÷ÃÂø…≈‰÷√
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
	m_pFont = ::new Gdiplus::Font(wstr, 9);  //lint !e1732 !e1733 !e747  GDIplus ø‚
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	CTranslucentWnd::PreSubclassWindow();
}
// CTranslucentStatic message handlers


