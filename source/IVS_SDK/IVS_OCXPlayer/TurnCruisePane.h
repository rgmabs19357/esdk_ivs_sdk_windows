#ifndef _TURNCRUISE_PANE_H__
#define _TURNCRUISE_PANE_H__
#include "afxwin.h"


// CTurnCruisePane dialog
#include "TranslucentButton.h"
#include "TranslucentDialog.h"
#include "ENUM_DEF.h"

class CTurnCruisePane : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTurnCruisePane)
public:
	CTurnCruisePane(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	CTurnCruisePane(LPCTSTR lpszFile, CWnd* pParent = NULL,UINT uiStyle = 0);// standard constructor
	virtual ~CTurnCruisePane();

// Dialog Data
	enum { IDD = IDD_DLG_TURNCRUISEPANE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

	//把工具栏按钮大小设置为0
	void ZeroSize();

public:
	virtual void OnInitChildrenWnds();
	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);
	void SetCurrentPaneToActivePane();

	CTranslucentButton m_BtnPre;
	CTranslucentButton m_BtnNext;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnPre();
	afx_msg void OnBnClickedBtnNext();
	void SetCruisePaneStyle(ULONG val){m_uiStyle = val;}
	void SetBarPictrue(LPCTSTR lpszFile);
private:
	UINT m_uiStyle;
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};//lint !e1712

#endif //_TURNCRUISE_PANE_H__
