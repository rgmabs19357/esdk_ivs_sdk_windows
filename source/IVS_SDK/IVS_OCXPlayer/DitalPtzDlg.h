#ifndef _DITAL_PTZ_H__
#define _DITAL_PTZ_H__

#include "afxwin.h"
#include "TranslucentButton.h"
#include "TranslucentDialog.h"
#include "ENUM_DEF.h"

#define DITAL_PTZ_LEFT 1
#define DITAL_PTZ_TOP_LEFT 2
#define DITAL_PTZ_TOP 3
#define DITAL_PTZ_TOP_RIGHT 4
#define DITAL_PTZ_RIGHT 5
#define DITAL_PTZ_BOTTOM_RIGHT 6
#define DITAL_PTZ_BOTTOM 7
#define DITAL_PTZ_BOTTOM_LEFT 8
// CDitalPtzDlg dialog

class CDitalPtzDlg : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CDitalPtzDlg)

public:
	CDitalPtzDlg(LPCTSTR lpszFile, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDitalPtzDlg();

// Dialog Data
	enum { IDD = IDD_DLG_DITTAL_PTZ };

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
	CTranslucentButton m_btnTopLeft;
	CTranslucentButton m_btnTop;
	CTranslucentButton m_btnTopRight;
	CTranslucentButton m_btnRight;
	CTranslucentButton m_btnBottomRight;
	CTranslucentButton m_btnBottom;
	CTranslucentButton m_btnBottomLeft;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg LRESULT OnDitalPTZBnDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDitalPTZBnUp(WPARAM wParam, LPARAM lParam);
private:
	IVS_INT32 GetDitalPTZSpeed(const char* pxml, int& iSpeed)const;
	IVS_UINT32 m_uiLockStatus;
};//lint !e1712

#endif //_DITAL_PTZ_H__
