#ifndef _PLAYBACK_PANE_H__
#define _PLAYBACK_PANE_H__
#include "afxwin.h"


// CPalyBackPane dialog
//用来设备录像回放背景上按钮的类
class CPalyBackPane : public CDialogEx
{
	DECLARE_DYNAMIC(CPalyBackPane)

public:
	CPalyBackPane(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPalyBackPane();

// Dialog Data
	enum { IDD = IDD_DLG_PLAYBACKPANE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnPlay;
};

#endif //_PLAYBACK_PANE_H__
