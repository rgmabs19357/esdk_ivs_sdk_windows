#ifndef IVSBUTTON_H
#define IVSBUTTON_H

/////////////////////////////////////////////////////////////////////////////
// CIVSButton window

class CIVSButton : public CButton
{
// Construction
public:
	CIVSButton();
private:
	UINT ulButtonID;
public:
	UINT GetUlButtonID() const { return ulButtonID; }
	void SetUlButtonID(UINT val) { ulButtonID = val; }

	CWnd* m_pChildWnd;
	CWnd* m_pParentWnd;
	BOOL	m_bIsBordLine;				//
	BOOL	m_bFontBold;
	BOOL	m_bSubMenu;
	BOOL	m_bCheckBox;
	BOOL	m_CheckValue;
	BOOL	m_bHold;
	BOOL	m_bOver;		//鼠标位于按钮之上时该值为true，反之为flase
	CString m_strTip;
protected:
	BOOL	m_bNormalMode;
	COLORREF m_clBack;
	//按钮的状态
	int	 m_nModeIcon;
	int  m_ShowMenuTime;
	BOOL m_bTracking;
	HICON	m_hIcon;
	HBITMAP	m_hBitmap;
	HBITMAP m_hBitmapOver;
	HBITMAP m_hBitmapDis;

	HCURSOR m_hCursor;
	CToolTipCtrl m_ToolTip;
	
	CBitmap	m_bmParent;
	CDC		m_dcParent;
	//判断该按钮是否可以被点击
	bool m_bIsEnable;
// Operations
public:
	void SetButtonNormlMode(BOOL bMode = TRUE);
	void SetBackColor(COLORREF clBack);
	void SetButtonCheckMenu(BOOL bCheckBox);
	void SetButtonIconMode(int nModeIcon);
	void SetButtonIcon(UINT nId);
	void SetButtonIcon(const CString &strName);

	void SetButtonBitmap(IVS_INT32 nNorId,IVS_INT32 nSelId,IVS_INT32 nDisId);
	void SetButtonBitmap( 
		const std::string &strNormalName, 
		const std::string &strSelectName,
		const std::string &strDisableName);
	void SetButtonCursor(UINT nId);
	void SetButtonSubMenu(BOOL bSubMenu = FALSE);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIVSButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawLineHeaderOrTrail( LPDRAWITEMSTRUCT lpDrawItemStruct, CDC &memDC, CDC * pDC )const;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void UpdateButton();
	virtual ~CIVSButton();
	void EnableButtonStatus(bool bIsEnable){m_bIsEnable = bIsEnable;}
	// Generated message map functions
protected:
	void DrawItemNormalButton(LPDRAWITEMSTRUCT lpDrawItemStruct) ;
	COLORREF SetTextColor( CDC &MemDC );

	void DrawParentBack( CDC* pDC, CWnd *pParent, const CRect &rcClient, CRect rcParent );
	//{{AFX_MSG(CIVSButton)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);

	void TimerIDEventEQZero();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	void ShowSubMenuWindow(CWnd* pSubMenu);
	bool isRequireProtractIcon()const;
	bool isRequireProtractCharacter(const TCHAR *strText)const;
	bool isHover(UINT state)const;
	bool isHover()const;
	BOOL ODSDISABLED(UINT state)const;
};

/////////////////////////////////////////////////////////////////////////////

#endif 
