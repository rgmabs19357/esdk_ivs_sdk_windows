/*****************************************************************
 filename    :    TranAnalysisToolBar.h
 author      :    f00152768
 created     :    2013/01/25
 description :    电视墙窗格通道状态图标
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/25 初始版本
*****************************************************************/
#pragma once


// CTranTVWallStatusBar dialog

class CTranTVWallStatusBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranTVWallStatusBar)

public:
	virtual ~CTranTVWallStatusBar();
	CTranTVWallStatusBar(Gdiplus::Image* pImage, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_TVWALL_STATUSBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	virtual void OnInitChildrenWnds();
};//lint !e1712 MFC窗体类
