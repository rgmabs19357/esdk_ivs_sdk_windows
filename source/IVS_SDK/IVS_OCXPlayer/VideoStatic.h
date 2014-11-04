/*****************************************************************
 filename    :    VideoStatic.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    视频播放窗体
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

// CVideoStatic

class CVideoStatic : public CStatic
{
	DECLARE_DYNAMIC(CVideoStatic)

public:
	CVideoStatic();
	virtual ~CVideoStatic();

protected:
	DECLARE_MESSAGE_MAP()
private:
    // 鼠标左键是否按下
    bool m_bIsLButtonDown;
public:
    // 起始坐标
    CPoint m_StartPos;
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg LRESULT OnResolutionChanged(WPARAM wParam, LPARAM lParam);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


