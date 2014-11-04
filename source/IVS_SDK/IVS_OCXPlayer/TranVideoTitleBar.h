/*****************************************************************
 filename    :    TranVideoTitleBar.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    透明标题栏
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#include "TranslucentButton.h"
#include "TranslucentDialog.h"
#include "TranslucentStatic.h"
#include "afxwin.h"

// CTranVideoTitleBar dialog

class CTranVideoTitleBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranVideoTitleBar)

public:
	CTranVideoTitleBar(Gdiplus::Image* pImage, CWnd* pParent = NULL);   // standard constructor
	virtual ~CTranVideoTitleBar();
// Dialog Data
	enum { IDD = IDD_DLG_TVIDEOTITLEBAR };

    virtual void OnInitChildrenWnds();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTranslucentStatic m_StaticDevName;
    // 关闭按钮
    CTranslucentButton m_btnOFF;
    // 视频信息按钮
    CTranslucentButton m_btnInf;
	// 电视墙正在上墙
	CTranslucentButton m_btnTVWallHandling;
	// 电视墙已上墙
	CTranslucentButton m_btnTVWallOK;
	// 电视墙视频断流
	CTranslucentButton m_btnTVWallVideoBroken;
	//水印状态
	CTranslucentButton m_btnWaterMakr;
	// 轮训状态
	CTranslucentButton m_btnTurnCruiseStatus;
	// 单帧状态
	CTranslucentButton m_btnSingleFrameStatus;
	// 告警状态
	CTranslucentButton m_btnAlarmStatus;
	// 手动录像状态
	CTranslucentButton m_btnRecordStatus;
	// 巡航
	CTranslucentButton m_btnCruiseTrackStatus;
    //电视墙
    CTranslucentButton m_btnTvWallReconnect;

    afx_msg void OnBnClickedBtnTitleOff();
    afx_msg void OnBnClickedBtnTitleInf();

    void SetCurrentPaneToActivePane();

    afx_msg void OnPaint();

    //把工具栏按钮大小设置为0
    void ZeroSize();

    //设置设备名称
    std::string GetDevName() const;//lint !e1036 !e1536
    void SetDevName(const std::string &strDevName);

    // 获得窗口类型
    WND_TYPE GetWndType() const;
    // 获得Client类型
    IVS_CLIENT_TYPE GetClientMode() const;
    virtual BOOL OnInitDialog();
	CPoint GetInfoButtonRightBottomPosition()const;

	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

    void EnableALLButton(BOOL isEnable);
private:
	// 电视墙状态
	ULONG m_ulTVWallStatus;
	// 水印状态
	LONG m_ulWatermarkStutus;
	// 轮训状态
	ULONG m_ulTurnCruiseStatus;
	// 单帧状态
	ULONG m_ulSingleFrameStatus;
	// 告警状态
	ULONG m_ulAlarmStatus;
	// 手动录像状态
	ULONG m_ulRecordStatus;
	// 巡航
	ULONG m_ulCruiseTrackStatus;
public:
	void SetTVWallStatus(ULONG ulStatus){m_ulTVWallStatus = ulStatus;}
    ULONG GetTVWallStatus()const{return m_ulTVWallStatus;}
	void SetWatermarkStatus(ULONG ulStatus);
	void SetTurnCruiseStatus(ULONG ulStatus){m_ulTurnCruiseStatus = ulStatus;}
    ULONG GetTurnCruiseStatus()const { return m_ulTurnCruiseStatus; }
	void SetSingleFrameStatus(ULONG ulStatus){m_ulSingleFrameStatus = ulStatus;}
	void SetAlarmStatus(ULONG ulStatus){m_ulAlarmStatus = ulStatus;}
	void SetRecordStatus(ULONG ulStatus){m_ulRecordStatus = ulStatus;}
	void SetCruiseTrackStatus(ULONG ulStatus){m_ulCruiseTrackStatus = ulStatus;}

	HWND GetOffBtnHwnd()const {return m_btnOFF.GetSafeHwnd(); }
	void UpdateAllButtonSkin();
    
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
}; //lint !e1712 MFC 窗体类
