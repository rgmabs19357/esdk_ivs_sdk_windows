#ifndef _TRAN_ZOOM_BAR_H__
#define _TRAN_ZOOM_BAR_H__

#include "afxwin.h"
#include "afxcmn.h"
#include "ZoomSliderCtrl.h"

const ULONG MAX_REAT_PERCETN = 800;     // 最大放大比例
const ULONG MIN_REAT_PERCETN = 100;     // 最小放大比例
const ULONG BTN_REAT_INTERVAL = 100;     // 按钮每次按下间隔

// CTranZoomBar dialog

class CTranZoomBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranZoomBar)

public:
	CTranZoomBar();
	CTranZoomBar(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/);   // standard constructor
	virtual ~CTranZoomBar();

// Dialog Data
	enum { IDD = IDD_DLG_ZOOMBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitChildrenWnds();
	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);
	void SetCurrentPaneToActivePane();
	void SetStaticPicture( CTranslucentStatic &tStatic,const std::string &strButtonName );
	void SetSnapShotEnableStatus(BOOL bEnable);
	void SetBarPictrue(LPCTSTR lpszFile);
private:
	CTranslucentStatic m_MagnifyRateText;
	CTranslucentButton m_btnShrink;
	CTranslucentButton m_btnMagnify;
	CTranslucentButton m_btnSnapShot;

	CZoomSliderCtrl m_ZoomSliderCtrl;

	IVS_INT32 m_iMagnifyRate; // 百分比
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg void OnBnClickedButtonSnapshot();
	afx_msg void OnBnClickedButtonShrink();
	afx_msg void OnBnClickedButtonMagnify();

	void SetMagnifyRate(IVS_INT32 iRate);
	HWND GetZoomInHwnd()const { return m_btnShrink.GetSafeHwnd(); }
	HWND GetZoomOutHwnd()const { return m_btnMagnify.GetSafeHwnd(); }
	void SetRateTextColor(ULONG ulColor);
};

#endif //_TRAN_ZOOM_BAR_H__
