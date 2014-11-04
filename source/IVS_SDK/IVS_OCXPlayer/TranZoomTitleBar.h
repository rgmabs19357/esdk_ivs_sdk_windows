#ifndef _TRAN_ZOOM_TITLEBAR_H__
#define _TRAN_ZOOM_TITLEBAR_H__

#include "afxwin.h"
#include "afxcmn.h"

// CTranZoomTitleBar dialog

class CTranZoomTitleBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranZoomTitleBar)

public:
	CTranZoomTitleBar();
	CTranZoomTitleBar(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/);   // standard constructor
	virtual ~CTranZoomTitleBar();

// Dialog Data
	enum { IDD = IDD_DLG_ZOOMTITLEBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitChildrenWnds();
	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);
	void SetCurrentPaneToActivePane();
	void SetBarPictrue(LPCTSTR lpszFile);
public:
	CTranslucentButton m_btnZoomOver;
	CTranslucentStatic m_StaticZoomName;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedClose();
	std::string GetZoomTitleName() const;
	void SetZoomTitleName( const std::string &strDevName );
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#endif //_TRAN_ZOOM_TITLEBAR_H__
