#pragma once
#include "SDKDef.h"
#include <GdiPlus.h>
#include "TranslucentWnd.h"
// CTranslucentStatic

class CTranslucentStatic : public CTranslucentWnd
{
	DECLARE_DYNAMIC(CTranslucentStatic)

public:
	CTranslucentStatic();
	virtual ~CTranslucentStatic();

	virtual void Render(Gdiplus::Graphics& g);
	virtual void PreSubclassWindow();

private:
	Gdiplus::Image* m_pCurrImage;
public:
	Gdiplus::Image* GetCurrImage() const { return m_pCurrImage; } //lint !e1763
	void SetCurrImage(Gdiplus::Image* val) { m_pCurrImage = val; }

protected:
	DECLARE_MESSAGE_MAP()

	Gdiplus::StringFormat m_format;
	Gdiplus::Font* m_pFont;
	Gdiplus::SolidBrush m_brush;
public:
	void SetFont(const WCHAR* familyName, float emSize, int style = Gdiplus::FontStyleRegular, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	void SetFormat(Gdiplus::StringAlignment align = Gdiplus::StringAlignmentCenter, Gdiplus::StringAlignment lineAlign = Gdiplus::StringAlignmentCenter);
	void SetColor(const Gdiplus::Color& color);

	const Gdiplus::Font* GetFont() const { return m_pFont; }				//lint !e1536
	const Gdiplus::StringFormat& GetFormat() const{ return m_format; }		//lint !e1536
	const Gdiplus::SolidBrush& GetBrush() const { return m_brush; }
};


