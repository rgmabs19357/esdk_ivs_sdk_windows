#pragma once

#include "SDKDef.h"
#include <GdiPlus.h>
#include <vector>
#include "ENUM_DEF.h"

class CTranslucentWnd;

struct IRenderListener
{
	virtual ~IRenderListener() { }
	virtual void OnRenderEvent(CTranslucentWnd* translucentWnd) = 0;
};

// CTranslucentWnd

class CTranslucentWnd : public CWnd
{
	DECLARE_DYNAMIC(CTranslucentWnd)

public:
	CTranslucentWnd();
	virtual ~CTranslucentWnd();

	virtual void Render(Gdiplus::Graphics& g) = 0;

	BOOL ShowWindow(int nCmdShow);

	void SetRenderListener(IRenderListener* pRenderListener) { m_pRenderListener = pRenderListener; }
	void NotifyRender();

	void DestroyImageList();

	void DestroyImage(UINT index);

	bool IsDisabled() const { return m_bDisabled; }
	bool IsVisible() const { return m_bVisible; }

protected:
	virtual void UpdateState();
	DECLARE_MESSAGE_MAP()
public:
	virtual void PreSubclassWindow();

public:
	afx_msg void OnEnable(BOOL bEnable);

protected:
    std::vector<Gdiplus::Image*> m_imageList;

    IRenderListener* m_pRenderListener;
    bool m_bDisabled;
    bool m_bVisible;
};

