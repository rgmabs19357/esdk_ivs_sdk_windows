#ifndef IVSMENU_H
#define IVSMENU_H

#include "MyMenuStruct.h"
#define  ICON_MDSMALL 1
#define  ICON_MDMIDD 2
#define  ICON_MDBIG 3
#define  LEFTTOP 0
#define  LEFTBOTTOM 1
#define  RIGHTTOP 2
#define  RIGHTBOTTOM 3
//////////////////////////////////
#define		MENU_NORMAL		0x10000000
#define		MENU_SUBITEM	0x00000001
#define		MENU_FONTBOLD	0x00000002
#define		MENU_BORDLINE	0x00000004
#define		MENU_CHECKITEM	0x00000008

/////////////////////////////////////////////////////////////////////////////
// CIVSMenu window

class CIVSMenu : public CWnd
{
// Construction
public:
	CIVSMenu();
	CWnd*			m_pParent;
	int				m_ButtonPos;
	BOOL			m_bCapture;
	CString			m_strTip;
	COLORREF		m_clBack;
protected:
	COLORREF		m_crGradFill;
	IVSMenu		m_MenuData;
	HCURSOR			m_hCursor;
	BOOL			m_bTransparentBk;		//背景是否透明
	int				m_nBordlineCount;
	int				m_nModeIcon;//			//图标大小模式：1：16X16 2：32X32, 3: 48X48;
	BOOL			m_bShowHead;
	BOOL			m_bMouseOn;	
	BOOL			m_bOver;
	BOOL			m_bEraseBack;
// Operations
public:
	void ShowMenuPos(int x, int y, int nMode = LEFTTOP);
	BOOL SetMenuHeader( const CString &StrHeadTitle, UINT nIDICON, BOOL bShowHead = TRUE );
	void UnLoadMenu();
	void UnLoadChildMenu();
	void ClearButtonData();
	int	 MenuHeight() const;
	int	 MenuWidth() const;
	void SetGradientFillColor(COLORREF crFill) { m_crGradFill = crFill; }
	void SetTransparentBk(BOOL bTransparentBk=TRUE);
	BOOL GetMenuStatus() const { return m_MenuData.bStatus;}
	void SetMenuStatus(BOOL bOpen=TRUE) { m_MenuData.bStatus = bOpen; }
	void SetBackColor(COLORREF clBack);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIVSMenu)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadMainMenu();
	BOOL AppendMenuItem( const CString &StrTitle, IVS_INT32 nID, IVS_INT32 nNorBitmapID,IVS_INT32 nSelBitMapID , IVS_INT32 nDisBitMapID, CWnd* pSubMenu, DWORD dwStyles, const CString & strTip );
	BOOL AppendMenuItem( const CString &StrTitle, IVS_INT32 nID, const std::string &strNorBitmap,const std::string &strSelBitMap, const std::string &strDisBitMap, CWnd* pSubMenu, DWORD dwStyles, const CString & strTip );
	void SetMenuOpen(BOOL bOpen=TRUE);
	void UpdateMenu();
	BOOL CreateGroupMenu(const CWnd *pParent, const CRect &rcClient, UINT nID, BOOL bShowHead = TRUE,int nModeIcon = ICON_MDSMALL);
	virtual ~CIVSMenu();

	IVS_INT32 GetItemNum() const;
	void AddMenuItem(MenuItemInfo &bt);
	void EnableMenuStatus(UINT ulMenuPos,BOOL bStatus);
	// Generated message map functions
protected:
	void KeyProcess(int nKeyCode);

	bool KeyCodeIsEqUpORDown( int nKeyCode ) const;

	void SetMenuInfo(const IVSMenu &gm);


	BOOL RegisterWindowClass() const;
	void GradientFillRect(HDC hDC, const CRect &rcFill, ULONG nMode, COLORREF crLeftTop, COLORREF crRightBottom) const;
	void UpdateMenuStatus();

	//{{AFX_MSG(CIVSMenu)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangeMenuStatus();
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);



	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void VKDOWNEvent();
	void VKUPEvent();
};

/////////////////////////////////////////////////////////////////////////////

#endif 
