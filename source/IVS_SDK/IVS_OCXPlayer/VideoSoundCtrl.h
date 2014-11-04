/*****************************************************************
 filename    :    VideoSoundCtrl.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    声音控件
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once
#ifndef __VIDEO_SOUND_CTRL_H_
#define __VIDEO_SOUND_CTRL_H_

#include "BitmapSlider.h"

typedef enum ST_SOUNDCTRL_WND_TYPE
{
    SOUNDCTRL_WND_TYPE_SOUND    = 0,
    SOUNDCTRL_WND_TYPE_TALKBACK = 1,
    SOUNDCTRL_WND_TYPE_RECORD   = 2,
    SOUNDCTRL_WND_TYPE_MAX
}SOUNDCTRL_WND_TYPE;


// CVideoSoundCtrl dialog

class CVideoSoundCtrl : public CDialog
{
	DECLARE_DYNAMIC(CVideoSoundCtrl)

public:
	CVideoSoundCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVideoSoundCtrl();

// Dialog Data
	enum { IDD = IDD_DLG_SOUNDCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	ULONG         m_ulWndType;         // 记录类型   随路语音 语音对讲 回放声音
	CWnd *        m_pRealTimeToolBar;         
	CBitmapSlider m_sliderSoundCtrl;
	CBitmapButton m_buttonSilence;

	ULONG         m_ulSliderPosition;  // 滑动条位置	
	BOOL          m_ubCheckSoundCtrlStatus;  // 静音的状态
public:
	virtual BOOL    OnInitDialog();
	afx_msg void    OnPaint();
	afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void    OnBnClickedButtonSoundCtrl();

	LRESULT        OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);   // 进度条移动完事件
	void           ProcessSliderMoved();  // 播放进度滑动条滑动事件

	// 由于现在标题栏的父窗口已经不是窗格，需要记录下对应的窗格
	void           SetVideopBar(CWnd * pVideoBar);

	// 设置静音图标
	void           SetSoundSilencePic(BOOL bState);
	void           SetSliderPosition(ULONG ulPos);
	void           SetTalkbackSilencePic(BOOL bState);
	void           SetRecordSilencePic(BOOL bState);
	ULONG          GetSliderPosition() const {return m_ulSliderPosition;} 

	// 类型控制
	ULONG          GetSoundCtrlType();
	void           SetSoundCtrlType(ULONG ulType);
	int            GetSkinType();
};
#endif
