/*****************************************************************
 filename    :    ParentWnd.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    VideoPane的父窗体
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#ifndef _PARENTWND_H__
#define _PARENTWND_H__

#include "VideoPaneMgr.h"

#define  BACKGROUND_CHANGE 1

// CParentWnd dialog
typedef std::list <CRect>   LISTCRECT;
typedef LISTCRECT::iterator LISTITER;

class CParentWnd : public CDialog
{
    DECLARE_DYNAMIC(CParentWnd)

public:
    CParentWnd(CWnd* pParent = NULL);   // standard constructor
    virtual ~CParentWnd();

    // Dialog Data
    LISTCRECT m_ListCrect;

    int m_nRightEdge;
    int m_nBottomEdge;
    enum { IDD = IDD_DLG_PARENT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnPaint();
    afx_msg void OnClose(){};//lint !e1762
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

private:
    bool m_bIsDrag;
	bool m_bDisableDrag;
public:
    bool GetIsDrag() const { return m_bIsDrag; }
    void SetIsDrag(bool val) { m_bIsDrag = val; }

	bool GetDisableDrag() const { return m_bDisableDrag; }
	void SetDisableDrag(bool val) { m_bDisableDrag = val; }
private:
    CVideoPaneMgr * m_pVideoPaneMgr;
	DWORD m_dwStart;
	DWORD m_dwEnd;
public:
    void SetVideoPaneMgr(CVideoPaneMgr * val) { m_pVideoPaneMgr = val; }
	void StartDrag();
};
#endif // _PARENTWND_H__
