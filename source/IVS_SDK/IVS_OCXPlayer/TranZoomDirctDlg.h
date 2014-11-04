#ifndef _TRAN_ZOOM_DIRCT_DLG_H_
#define _TRAN_ZOOM_DIRCT_DLG_H_

#include "afxwin.h"
#include "TranslucentButton.h"
#include "TranslucentDialog.h"
#include "ENUM_DEF.h"

#define DIRCT_BTN_LEFT 1
#define DIRCT_BTN_TOP 2
#define DIRCT_BTN_RIGHT 3
#define DIRCT_BTN_BOTTOM 4
// CTranZoomDirctDlg dialog

class CTranZoomDirctDlg : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranZoomDirctDlg)

public:
	CTranZoomDirctDlg(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	virtual ~CTranZoomDirctDlg();

// Dialog Data
	enum { IDD = IDD_DLG_ZOOMDIRCT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitChildrenWnds();
	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);
	void SetCurrentPaneToActivePane();
	void SetBarPictrue(LPCTSTR lpszFile);
public:
	CTranslucentButton m_btnLeft;
	CTranslucentButton m_btnTop;
	CTranslucentButton m_btnRight;
	CTranslucentButton m_btnBottom;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnDitalBnDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDitalBnUp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedTop();
	afx_msg void OnBnClickedRight();
	afx_msg void OnBnClickedBottom();
};//lint !e1712

#endif //_TRAN_ZOOM_DIRCT_DLG_H_
