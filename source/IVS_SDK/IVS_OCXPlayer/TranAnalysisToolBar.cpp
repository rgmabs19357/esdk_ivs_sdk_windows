// TranAnalysisToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "TranAnalysisToolBar.h"
#include "IVS_OCXPlayerCtrl.h"
#include "VideoPane.h"

// CTranAnalysisToolBar dialog

IMPLEMENT_DYNAMIC(CTranAnalysisToolBar, CTranslucentDialog)

CTranAnalysisToolBar::CTranAnalysisToolBar(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CTranAnalysisToolBar::IDD, lpszFile,  pParent)
    ,m_IsStartPay(FALSE)
    ,m_ullToolBarBtnVisableStatus(0)
{
    m_btnVector.clear();
}


CTranAnalysisToolBar::CTranAnalysisToolBar(Gdiplus::Image* pImage, CWnd* pParent)
	: CTranslucentDialog(CTranAnalysisToolBar::IDD, pImage, pParent)
	,m_IsStartPay(FALSE)
	,m_ullToolBarBtnVisableStatus(0)
{
	m_btnVector.clear();
}

CTranAnalysisToolBar::~CTranAnalysisToolBar()
{
}//lint !e1540

void CTranAnalysisToolBar::DoDataExchange(CDataExchange* pDX)
{
    CTranslucentDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_RECT,          m_BtnRect);
    DDX_Control(pDX, IDC_BTN_POLYGON,       m_BtnPolygon);
    DDX_Control(pDX, IDC_BTN_DELETE,		m_BtnDelete);
    DDX_Control(pDX, IDC_BTN_DISPLAY_MODE,	m_BtnDisplayMode);
    DDX_Control(pDX, IDC_BTN_PLAY,			m_BtnPlay);
}
  
BEGIN_MESSAGE_MAP(CTranAnalysisToolBar, CTranslucentDialog)
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BTN_RECT,         &CTranAnalysisToolBar::OnBnClickedBtnRect)
    ON_BN_CLICKED(IDC_BTN_POLYGON,      &CTranAnalysisToolBar::OnBnClickedBtnPolygon)
	ON_BN_CLICKED(IDC_BTN_DELETE,       &CTranAnalysisToolBar::OnBnClickedBtnDelete)
    ON_BN_CLICKED(IDC_BTN_DISPLAY_MODE, &CTranAnalysisToolBar::OnBnClickedBtnDisplayMode)
    ON_BN_CLICKED(IDC_BTN_PLAY,         &CTranAnalysisToolBar::OnBnClickedBtnPlay)
    ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_DELETE, &CTranAnalysisToolBar::OnClickedBtnDelete)
END_MESSAGE_MAP()

void CTranAnalysisToolBar::OnBnClickedBtnDelete()
{
	SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pPlayerCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pPlayerCtrl);
	if (GRAPHICS_STATE_EDITING == pVideoPane->GetCurrentGraphicState())
	{
		pPlayerCtrl->PostToolBarClickEventMsg(1,IVS_IA_TOOLBAR_DELETE,pVideoPane->GetPaneID(),_T("Delete"));
// 	    pPlayerCtrl->IVS_OCX_Windows_Event(EVENT_INTELLIGENT_TOOL_BAR_DELETE, NULL);
	}
}

void CTranAnalysisToolBar::OnBnClickedBtnDisplayMode()
{
    SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pPlayerCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pPlayerCtrl);
	pPlayerCtrl->PostToolBarClickEventMsg(1,IVS_IA_TOOLBAR_DISPLAY_MODE,pVideoPane->GetPaneID());
}

void CTranAnalysisToolBar::OnBnClickedBtnPlay()
{
    SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pPlayerCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pPlayerCtrl);
	if (m_IsStartPay)
	{
		pPlayerCtrl->PostToolBarClickEventMsg(1,IVS_IA_TOOLBAR_PLAY,pVideoPane->GetPaneID(),_T("Stop"));
// 		pPlayerCtrl->IVS_OCX_Windows_Event(EVENT_INTELLIGENT_TOOL_BAR_PAUSE, NULL);
	} else{ 
		pPlayerCtrl->PostToolBarClickEventMsg(1,IVS_IA_TOOLBAR_PLAY,pVideoPane->GetPaneID(),_T("Play"));
// 		pPlayerCtrl->IVS_OCX_Windows_Event(EVENT_INTELLIGENT_TOOL_BAR_PLAY, NULL);
	}
}

void CTranAnalysisToolBar::OnBnClickedBtnRect()
{
    SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pPlayerCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pPlayerCtrl);
	pPlayerCtrl->PostToolBarClickEventMsg(1,IVS_IA_TOOLBAR_DRAW_RECT,pVideoPane->GetPaneID());
// 	if (GRAPHICS_STATE_EDITING == pVideoPane->GetCurrentGraphicState())
// 	{	
		pVideoPane->SetCurrentGraphicType(IA_GRAPHICS_TYPE_RECTANGLE);
		pVideoPane->SetCurrentGraphicState(GRAPHICS_STATE_DRAWING);
		(void)pVideoPane->SelectCancelWhileDraw();
// 	}
}

void CTranAnalysisToolBar::OnBnClickedBtnPolygon()
{
    SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pPlayerCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pPlayerCtrl);
	pPlayerCtrl->PostToolBarClickEventMsg(1,IVS_IA_TOOLBAR_DRAW_POLYGON,pVideoPane->GetPaneID());
// 	if (GRAPHICS_STATE_EDITING == pVideoPane->GetCurrentGraphicState())
// 	{	
		pVideoPane->SetCurrentGraphicType(IA_GRAPHICS_TYPE_POLYGON);
		pVideoPane->SetCurrentGraphicState(GRAPHICS_STATE_DRAWING);
		(void)pVideoPane->SelectCancelWhileDraw();
// 	}
}

void CTranAnalysisToolBar::SetAnalysisButtonPicture(
    CTranslucentButton &tButton,
    const std::string &strButtonName)
{
    CHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]={0x0};
    
    memset(pcImageList,0x0,MAX_PATH*TWS_BUTTON_NUM);
    (void)tButton.LoadImageList(pcImageList);
    tButton.SetStrFunctionName(strButtonName);
    RegisterTranslucentWnd(&tButton);
}

void CTranAnalysisToolBar::SetButtonPicture(
    CTranslucentButton &tButton,
    const std::string &strButtonName)
{
	// 根据名字 获取对应的资源文件中的信息
	std::string strToolTip;
	if (CI18N::GetInstance().GetResourceLanguage(strButtonName, strToolTip))
	{
		tButton.UpdateTipText(strToolTip);
	}
	// 添加到tip中
	(void)m_TipMap.insert(std::make_pair(strButtonName, &tButton));

    CHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]={0x0};

    memset(pcImageList,0x0,MAX_PATH * TWS_BUTTON_NUM);
    (void)SkinMgr.ToolBarGetPNGButtonByName(strButtonName,pcImageList);
    (void)tButton.LoadImageList(pcImageList);
    tButton.SetStrFunctionName(strButtonName);
    RegisterTranslucentWnd(&tButton);
}

void CTranAnalysisToolBar::OnInitChildrenWnds()
{    
//     SetButtonPicture(m_BtnRect,	_T("Rect"));    
//     SetButtonPicture(m_BtnPolygon, _T("Polygon"));    
//     SetButtonPicture(m_BtnDelete,	_T("Delete"));
//     SetButtonPicture(m_BtnDisplayMode, _T("DisplayMode"));
// 	if (m_IsStartPay)
// 	{
// 		SetButtonPicture(m_BtnPlay, _T("Stop"));
// 	}
// 	else
// 	{
// 		SetButtonPicture(m_BtnPlay, _T("Play"));
// 	}
}

void CTranAnalysisToolBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    DrawButton();
    UpdateView();
}

BOOL CTranAnalysisToolBar::OnInitDialog()
{ 
    CTranslucentDialog::OnInitDialog();
    m_btnVector.push_back(&m_BtnRect);
    m_btnVector.push_back(&m_BtnPolygon);    
    m_btnVector.push_back(&m_BtnDelete);
    m_btnVector.push_back(&m_BtnDisplayMode);
    m_btnVector.push_back(&m_BtnPlay);

	SetButtonPicture(m_BtnRect,	_T("Rect"));    
	SetButtonPicture(m_BtnPolygon, _T("Polygon"));    
	SetButtonPicture(m_BtnDelete,	_T("Delete"));
	SetButtonPicture(m_BtnDisplayMode, _T("DisplayMode"));
	if (m_IsStartPay)
	{
		SetButtonPicture(m_BtnPlay, _T("Stop"));
	}
	else
	{
		SetButtonPicture(m_BtnPlay, _T("Play"));
	}

    EndBuffered();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CTranAnalysisToolBar::OnMouseMove(UINT nFlags, CPoint point)
{
    CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    pVideoPane->OnMouseMove(nFlags, point);
    CTranslucentDialog::OnMouseMove(nFlags, point);
}

void CTranAnalysisToolBar::SetCurrentPaneToActivePane()
{
    CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    pVideoPane->SetActivePane();
}

void CTranAnalysisToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCurrentPaneToActivePane();
    CTranslucentDialog::OnLButtonDown(nFlags, point);
}

void CTranAnalysisToolBar::DrawButton()
{
    std::bitset<IATOOLBARBTNNUM> bitsetButtonVisableStatus(m_ullToolBarBtnVisableStatus);

    CRect rect;
    GetClientRect(&rect);
    const int iWitdh = ANALYSISTOOLBAR_BUTTON_SPACE + ANALYSISTOOLBAR_BUTTON_HEIGHT;

    const IVS_INT32 nAllowBtnNum = rect.Width() / iWitdh - 1;
    //如果显示的长度不够
    if (nAllowBtnNum <= 0)
    {
        return;
    }

    std::vector< CTranslucentButton *>::iterator BtnIter = m_btnVector.begin();
    std::vector< CTranslucentButton *>::iterator BtnIterEnd = m_btnVector.end();
    int nBtnLeftIndex = rect.left + ANALYSISTOOLBAR_BUTTON_SPACE;

    int iIndex = 0;
    do 
    {
        if (static_cast<size_t>(iIndex +2) >= bitsetButtonVisableStatus.size())
        {
            break;
        } else {
            if (!bitsetButtonVisableStatus.test(static_cast<size_t>((iIndex == 2)? iIndex + 2 :  iIndex)))
            {
                (*BtnIter)->ShowWindow(SW_HIDE);
                iIndex = iIndex + 1;
                BtnIter ++;
                continue;
            }

            (*BtnIter)->MoveWindow(nBtnLeftIndex, 1, ANALYSISTOOLBAR_BUTTON_WITDH, ANALYSISTOOLBAR_BUTTON_HEIGHT);
            (*BtnIter)->ShowWindow(SW_SHOW);
            iIndex++;
            nBtnLeftIndex += iWitdh;
            BtnIter ++;
        }
    } while (BtnIter != BtnIterEnd && iIndex < nAllowBtnNum);

    if (bitsetButtonVisableStatus.test(3))
    {
        m_BtnDisplayMode.MoveWindow(rect.right - iWitdh,1,ANALYSISTOOLBAR_BUTTON_WITDH,ANALYSISTOOLBAR_BUTTON_HEIGHT);

        if (bitsetButtonVisableStatus.test(2))
        {
            m_BtnPlay.MoveWindow(rect.right - 2* iWitdh,1,ANALYSISTOOLBAR_BUTTON_WITDH,ANALYSISTOOLBAR_BUTTON_HEIGHT);
        } else {
            m_BtnPlay.ShowWindow(SW_HIDE);
        }
    } else {
        m_BtnDisplayMode.ShowWindow(SW_HIDE);

        if (bitsetButtonVisableStatus.test(2))
        {
             m_BtnPlay.MoveWindow(rect.right - iWitdh,1,ANALYSISTOOLBAR_BUTTON_WITDH,ANALYSISTOOLBAR_BUTTON_HEIGHT);
        } else {
            m_BtnPlay.ShowWindow(SW_HIDE);
        }
    }
}

void CTranAnalysisToolBar::SetToolbarEnableStatus( ULONGLONG dwToolbarBtn, ULONGLONG lButtonStatus )
{
	std::bitset<IATOOLBARBTNNUM> bitsetButtonStatus(dwToolbarBtn);
	IVS_UINT32 _iCount = 0;
	for (_iCount = 0; _iCount < IATOOLBARBTNNUM ;_iCount++)
	{
		if (bitsetButtonStatus.test(_iCount))
		{
			if (lButtonStatus == 1)
			{
				m_btnVector[_iCount]->EnableWindow(TRUE);
			}else{
				m_btnVector[_iCount]->EnableWindow(FALSE);
			}
		}
	}
	UpdateView();
}

void CTranAnalysisToolBar::SetIsStartPay( BOOL val )
{
    m_IsStartPay = val;
    if (val)
    {
        SetButtonPicture(m_BtnPlay, "Stop"); //设置 播放 按钮图片
    } else{ // 
        SetButtonPicture(m_BtnPlay, "Play"); // 设置 停止 按钮图片
    }

    UpdateView();
}

void CTranAnalysisToolBar::OnClickedBtnDelete()
{
	SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pPlayerCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pPlayerCtrl);
	pPlayerCtrl->PostToolBarClickEventMsg(1,IVS_IA_TOOLBAR_DELETE,pVideoPane->GetPaneID());
}

void CTranAnalysisToolBar::ResetBottomButton()
{
	SetIsStartPay(FALSE);
}

void CTranAnalysisToolBar::EnableALLButton( BOOL isEnable )
{
	std::vector<CTranslucentButton *>::const_iterator it;
	IVS_INT32 iCount = -1;
	for (it = m_btnVector.begin(); it != m_btnVector.end(); ++it)
	{
		iCount++;
		CTranslucentWnd* translucentWnd = *it;
		// Rect、Polygon、Delete不立刻Enable
		if (isEnable && 
			((0 == iCount) || (1 == iCount) || (2 == iCount)))
		{
			continue;
		}
		translucentWnd->EnableWindow(isEnable);
	}
	m_BtnPlay.EnableWindow(TRUE);
	UpdateView();
}
