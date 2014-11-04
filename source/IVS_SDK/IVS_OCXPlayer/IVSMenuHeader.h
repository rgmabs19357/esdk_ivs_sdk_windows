#ifndef MENUHEADER_H
#define MENUHEADER_H

/////////////////////////////////////////////////////////////////////////////
// CIVSHeader window

class CIVSHeader : public CIVSButton
{
// Construction
public:
	CIVSHeader();

protected:
	BOOL	m_bOpen;					//当前是否打开
	HICON	m_hIconStatus;
// Operations
public:
	void SetHeaderStatus(BOOL bOpen);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIVSHeader)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIVSHeader();

	// Generated message map functions
protected:
	void GradientFillRect( HDC hDC, const CRect &rcFill, ULONG nMode, COLORREF crLeftTop, COLORREF crRightBottom ) const;
	//{{AFX_MSG(CIVSHeader)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif 
