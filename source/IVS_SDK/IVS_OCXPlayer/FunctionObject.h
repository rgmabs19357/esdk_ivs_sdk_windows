/*****************************************************************
 filename    :    FunctionObject.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    函数对象定义文件
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#ifndef _FUNCTIONOBJECT_H__
#define _FUNCTIONOBJECT_H__

#include "common_struct.h"
#include "VideoPane.h"
#include "TranVideoRealtimeBar.h"
#include "TranslucentButton.h"

class FunctionObjectDeleteSoundTalkBackCtrlForOther{
private:
	// 当前活动窗格ID，默认为1,范围是1-64
	ULONG m_ulPaneID;

	FunctionObjectDeleteSoundTalkBackCtrlForOther():m_ulPaneID(1)
	{
	}
public:
	FunctionObjectDeleteSoundTalkBackCtrlForOther(ULONG ulPaneID):m_ulPaneID(ulPaneID)
	{
	}
	~FunctionObjectDeleteSoundTalkBackCtrlForOther()
	{
	}

	void operator()(const PairVideoPane &iter)const
	{
		if (m_ulPaneID == iter.first)
		{
			return;
		}
		CVideoPane* pVideoPane = iter.second;  //lint !e64
		CHECK_POINTER_VOID(pVideoPane);

		CTranVideoRealtimeBar* TRBar  = pVideoPane->GetVideoRealtimeBar();
		//删除对话框
		if (TRBar == NULL)
		{
			return;
		}
		TRBar->DeleteSoundCtrl();
		TRBar->DeleteTalkbackCtrl();
		TRBar->DeletePopMenuBar();
	}
};

class FunctionObjectDrawButtonPosition{
private:
	CTranVideoRealtimeBar * m_pTranVideoRealtimeBar;
	IVS_INT32 m_iBtnVisableCount;
	size_t m_iCount;
	IVS_INT32 m_iBtnVisable;
	IVS_INT32 m_iBtnLeftIndex;

	FunctionObjectDrawButtonPosition()
		:m_pTranVideoRealtimeBar(nullptr)
		,m_iBtnVisableCount(1)
		,m_iCount(0)
		,m_iBtnVisable(0)
		,m_iBtnLeftIndex(0)
	{

	}
public:

	FunctionObjectDrawButtonPosition(CTranVideoRealtimeBar *pTranVideoRealtimeBar,
		IVS_INT32 __iBtnNum,
		IVS_INT32 iBtnLeftIndex
		)
		:m_pTranVideoRealtimeBar(pTranVideoRealtimeBar)
		,m_iBtnVisableCount(1)
		,m_iCount(0)
		,m_iBtnVisable(__iBtnNum)
		,m_iBtnLeftIndex(iBtnLeftIndex)
	{

	}

	void operator()(CTranslucentButton * btn){
        CHECK_POINTER_VOID(m_pTranVideoRealtimeBar);
		if ((m_iBtnVisableCount <=  m_iBtnVisable) 
			&& m_pTranVideoRealtimeBar->GetBitsetButtonVisableStatus().test(m_iCount++))
		{
			++ m_iBtnVisableCount;
			btn->MoveWindow(m_iBtnLeftIndex,1, 22, 22);
			btn->ShowWindow(SW_SHOW);
			m_iBtnLeftIndex += REALTIMEBAR_BUTTON_WITDH;
		} else{
			btn->ShowWindow(SW_HIDE);
		}
	}

	IVS_INT32 value()const
	{
		return m_iBtnLeftIndex;
	}
};

class FunctionObjecVideoPaneResetBarPosition
{
public:
    FunctionObjecVideoPaneResetBarPosition(){}
    ~FunctionObjecVideoPaneResetBarPosition(){}
    void operator()(const PairVideoPane &iter)const
    {
        CVideoPane* pVideoPane = (CVideoPane*)iter.second;  //lint !e611
        CHECK_POINTER_VOID(pVideoPane);
        pVideoPane->ResetBarPosition();

		CTranVideoRealtimeBar* TRBar  = pVideoPane->GetVideoRealtimeBar();
		//删除对话框
		if (NULL == TRBar)
		{
			return;
		}

		TRBar->DeleteSoundCtrl();
		TRBar->DeleteTalkbackCtrl();
		TRBar->DeletePopMenuBar();
    }
};

class  FunctionObjectVideoPaneHandleIsEqual
    : public std::binary_function<PairVideoPane, ULONG, bool>
{
public:
    FunctionObjectVideoPaneHandleIsEqual()
    {
    }

    bool operator()(const PairVideoPane & _Left, CONST ULONG & _Right) const
    {
        const CVideoPane *pVideoPane = _Left.second; //lint !e64
        CHECK_POINTER(pVideoPane,FALSE);
        return( _Right == pVideoPane->GetPlayHandle() );
    }
};

class  FunctionObjectVideoPaneVideoWndIsEqual
    : public std::binary_function<PairVideoPane, HWND, bool>
{
public:
    FunctionObjectVideoPaneVideoWndIsEqual()
    {
    }

    bool operator()(const PairVideoPane & _Left, CONST HWND & _Right) const
    {
        const CVideoPane *pVideoPane = _Left.second; //lint !e64
        CHECK_POINTER(pVideoPane,FALSE);
        return( _Right == pVideoPane->GetVideoWnd() );
    }
};

//class  FunctionObjectVideoPaneRefreshWindow
//{
//public:
//    FunctionObjectVideoPaneRefreshWindow()
//    {
//    }
//
//    void operator()(const PairVideoPane &iter) const
//    {
//   //     CVideoPane *pVideoPane = iter.second; //lint !e64
//   //     CHECK_POINTER_VOID(pVideoPane);
//   //     if (pVideoPane->IsWindowVisible())
//   //     {
//   //         //pVideoPane->OnPaint();
//			//pVideoPane->Invalidate(TRUE);
//   //     }
//    }
//};

class  FunctionObjectVideoPaneHideAllBar
{
public:
    FunctionObjectVideoPaneHideAllBar()
    {
    }

    void operator()(const PairVideoPane &iter) const
    {
        CVideoPane *pVideoPane = iter.second; //lint !e64
        CHECK_POINTER_VOID(pVideoPane);
		pVideoPane->SetTranslucentStatus(TRUE);
		pVideoPane->HideAllBar();
		pVideoPane->SetTranslucentStatus(FALSE);
		CTranVideoRealtimeBar* TRBar  = pVideoPane->GetVideoRealtimeBar();
		//删除对话框
		if (TRBar==NULL)
		{
			return;
		}
		TRBar->DeleteSoundCtrl();
		TRBar->DeleteTalkbackCtrl();
		TRBar->DeletePopMenuBar();
    }
};

class  FunctionObjectVideoPaneSetAllBarWindowsLong
{
public:
	FunctionObjectVideoPaneSetAllBarWindowsLong()
	{
	}

	void operator()(const PairVideoPane &iter) const
	{
		CVideoPane *pVideoPane = iter.second; //lint !e64
		CHECK_POINTER_VOID(pVideoPane);
		pVideoPane->SetBarWindowsLong();
	}
};

class  FunctionObjectCustomLayout
{
private:
    CRect m_Rect;
    LONG m_lUnitWith;
    LONG m_lUnitHeight;
    CVideoPaneMgr *m_pVideoPaneMgr;
    FunctionObjectCustomLayout()
        : m_Rect()
        , m_lUnitWith(0)
        , m_lUnitHeight(0)
        , m_pVideoPaneMgr(nullptr)
    {}
public:
    FunctionObjectCustomLayout(const CRect &rect, LONG lUnitWith,LONG lUnitHeight,  CVideoPaneMgr *pVideoPaneMgr)
        : m_Rect(rect)
        , m_lUnitWith(lUnitWith)
        , m_lUnitHeight(lUnitHeight)
        , m_pVideoPaneMgr(pVideoPaneMgr)
    {
    }

    void operator()(const CRect &itRect) const
    {
        CPoint _TopLefPoint  =  m_Rect.TopLeft();
        CPoint _BottomRightPoint;
        CRect _Rect;

        _TopLefPoint.x += itRect.left * m_lUnitWith;
        _TopLefPoint.y += itRect.top * m_lUnitHeight;

        _BottomRightPoint.x = _TopLefPoint.x + itRect.Width() * m_lUnitWith;
        _BottomRightPoint.y = _TopLefPoint.y + itRect.Height() * m_lUnitHeight;

        _Rect.SetRect(_TopLefPoint,_BottomRightPoint);
        CHECK_POINTER_VOID(m_pVideoPaneMgr);
		m_pVideoPaneMgr->SetVideoPanePosition(_Rect);

		//不是电视墙则不需要拆分
		if(m_pVideoPaneMgr->GetCurTVWallID() == 0)
		{
			return;
		}
		//需要拆分的窗口列表
		for(RECTLIST::iterator it = m_pVideoPaneMgr->m_SplitTo1RectList.begin();
			it!=m_pVideoPaneMgr->m_SplitTo1RectList.end();it++)
		{
			if(it->EqualRect(&itRect))
			{
				m_pVideoPaneMgr->AddSplitRectMap(CVideoPaneMgr::SPLIT_1,_Rect);
				break;
			}
		}
		for(RECTLIST::iterator it = m_pVideoPaneMgr->m_SplitTo4RectList.begin();
			it!=m_pVideoPaneMgr->m_SplitTo4RectList.end();it++)
		{
			if(it->EqualRect(&itRect))
			{
				m_pVideoPaneMgr->AddSplitRectMap(CVideoPaneMgr::SPLIT_4,_Rect);
				break;
			}
		}
		for(RECTLIST::iterator it = m_pVideoPaneMgr->m_SplitTo9RectList.begin();
			it!=m_pVideoPaneMgr->m_SplitTo9RectList.end();it++)
		{
			if(it->EqualRect(&itRect))
			{
				m_pVideoPaneMgr->AddSplitRectMap(CVideoPaneMgr::SPLIT_9,_Rect);
				break;
			}
		}
		for(RECTLIST::iterator it = m_pVideoPaneMgr->m_SplitTo16RectList.begin();
			it!=m_pVideoPaneMgr->m_SplitTo16RectList.end();it++)
		{
			if(it->EqualRect(&itRect))
			{
				m_pVideoPaneMgr->AddSplitRectMap(CVideoPaneMgr::SPLIT_16,_Rect);
				break;
			}
		}

    }
};

class  FunctionObjectSumRect
{
private:
    IVS_INT32 m_uiSumRect;
public:
    FunctionObjectSumRect()
        :m_uiSumRect(0)
    {
    }

    void operator()( const CRect &itRect)
    {
        m_uiSumRect += itRect.Width() * itRect.Height();
    }

    IVS_INT32 value()const
    {
        return m_uiSumRect;
    }
};

class  FunctionObjectToolBarAppendItem
{
private:
    CTranVideoRealtimeBar * m_pToolBar;
    size_t   m_iCount;
    FunctionObjectToolBarAppendItem()
        : m_pToolBar(nullptr)
        , m_iCount(0)
    {
    }
public:
    FunctionObjectToolBarAppendItem(CTranVideoRealtimeBar * pToolBar)
        : m_pToolBar(pToolBar)
        , m_iCount(0)
    {
    }

    void operator()(CTranslucentButton *btn)
    {
        CHECK_POINTER_VOID(btn);
        CHECK_POINTER_VOID(m_pToolBar);
        if (m_pToolBar->GetBitsetButtonVisableStatus().test( m_iCount++ ) && (!btn->IsVisible()))
        {
            std::string strButtonFunctinName = btn->GetStrFunctionName();
            std::string strNorBitmap;
            std::string strSelBitMap;
            std::string strDisBitMap;

            (void)SkinMgr.ToolBarGetBmpButtonByName(
                strButtonFunctinName,
                strSelBitMap,
                strDisBitMap,
                strNorBitmap
                );

			CString strText;
			btn->GetTipText(strText);
			if(strText.IsEmpty()){
				strText.LoadString(static_cast<IVS_UINT32>(btn->GetDlgCtrlID()));
			}

            (void)m_pToolBar->m_pMenu->AppendMenuItem(strText ,
                btn->GetDlgCtrlID(), 
                strNorBitmap, 
                strSelBitMap,
                strSelBitMap,
                NULL, 
                MENU_NORMAL,
                "");
        }
    }
};

class  FunctionObjectVideoPaneHideAllTitleBar
{
public:
	FunctionObjectVideoPaneHideAllTitleBar()
	{
	}

	void operator()(const PairVideoPane &iter) const
	{
		CVideoPane *pVideoPane = iter.second; //lint !e64
		CHECK_POINTER_VOID(pVideoPane);
		CTranVideoTitleBar* TRBar  = pVideoPane->GetVideoTitleBar();
		//删除对话框
		if (TRBar==NULL)
		{
			return;
		}
		TRBar->ShowWindow(SW_HIDE);
		TRBar->MoveWindow(0,0,0,0);
		pVideoPane->ResetBarPosition();
	}
};

class  FunctionObjectVideoPaneHideAllToolBar
{
public:
	FunctionObjectVideoPaneHideAllToolBar()
	{
	}

	void operator()(const PairVideoPane &iter) const
	{
		CVideoPane *pVideoPane = iter.second; //lint !e64
		CHECK_POINTER_VOID(pVideoPane);
		CTranVideoRealtimeBar* TRBar  = pVideoPane->GetVideoRealtimeBar();
		//删除对话框
		if (TRBar==NULL)
		{
			return;
		}
		TRBar->ShowWindow(SW_HIDE);
		TRBar->MoveWindow(0,0,0,0);
		pVideoPane->ResetBarPosition();
	}
};


class FunctionObjecVideoPaneSetSkin
{
public:
	FunctionObjecVideoPaneSetSkin(){}
	~FunctionObjecVideoPaneSetSkin(){}
	void operator()(const PairVideoPane &iter)const
	{
		CVideoPane* pVideoPane = (CVideoPane*)iter.second;  //lint !e611
		CHECK_POINTER_VOID(pVideoPane);
		pVideoPane->SetSkin();
	}
};

#endif // _FUNCTIONOBJECT_H__
