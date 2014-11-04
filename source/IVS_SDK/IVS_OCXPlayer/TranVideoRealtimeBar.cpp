// DemoTranslucentDlg.cpp : implementation file
//
#include "stdafx.h"
#include "TranVideoRealtimeBar.h"
#include <typeinfo>
#include "IVS_OCXPlayerCtrl.h"
#include "VideoPane.h"
#include "VideoPaneMgr.h"
#include "IVS_OCXPlayerCtrl.h"
#include "LayoutMgr.h"
#include "FunctionObject.h"
#include "OCXRecordXMLProcess.h"
#include "OCXCapturePictureXMLProcess.h"
#include "ivs_xml.h "
#include "PostEventXML.h"
#include "IVS_Trace.h"
#include "GlobalPlayMgr.h"
#include "constant.h"
// CTranslucentVideoRealtimeBar dialog
//
IMPLEMENT_DYNAMIC(CTranVideoRealtimeBar, CTranslucentDialog) //lint !e18  MFC自动生成的的函数
CTranVideoRealtimeBar::CTranVideoRealtimeBar(Gdiplus::Image* pImage, CWnd* pParent)
	: CTranslucentDialog(CTranVideoRealtimeBar::IDD, pImage, pParent)
	,m_pMenu(nullptr)
	,m_isExtend(false)
	,m_iNumBtn(MAXNUM_SHOW_BUTTON)
	,m_pVideoSoundCtrl(nullptr)
	,m_ulSoundCtrlValue(SOUNDCTRL_SLIDER_LEN/2)
	,m_bSoundCtrlState(FALSE)
	,m_bIsInstansPlay(FALSE)
	,m_bIsStartPlay(TRUE)
	,m_bSoundState(FALSE)
	,m_bRecordStarted(FALSE)
	,m_pVideoTalkbackSound(nullptr)
	,m_ulTalkbackSoundValue(SOUNDCTRL_SLIDER_LEN/2)
	,m_bTalkbackSoundState(BOTTOMBAR_SOUND_OPEN)
	,m_bTalkbackSlienceState(FALSE)
	,m_bIsTurning(FALSE)
	,m_bIsButtonEnable(FALSE)
	,m_bTalkbackSoundCreated(false)//lint !e1729
	,m_bSoundCreated(false)
	,m_bMenuCreated(false)
	,m_lPalyQualityMode(PLAY_QUALITY_DEFINITION)
	,m_bPtz3DStarted(false)//lint !e1729
	,m_bShowTalkbackSoundCtrl(false)
	,m_ulSoundCtrlLeft(0)
	,m_ulSoundCtrlTop(0)
	,m_ullToolBarBtnVisableStatus(0)
{
	m_btnVector.clear();
	(void)m_bitsetButtonUsableStatus.set();
	(void) m_bitsetButtonCompetenceStatus.set();
}

CTranVideoRealtimeBar::~CTranVideoRealtimeBar()
{
// 	IVS_DELETE(m_pVideoSoundCtrl); //lint !e1551 IVS_DELETE 宏定义
// 	IVS_DELETE(m_pMenu);           //lint !e1551 IVS_DELETE 宏定义
	DeleteSoundCtrl();//lint !e1551
	DeletePopMenuBar();//lint !e1551
} //lint !e1740 m_pVideoPane 由外部传入 无须释放

void CTranVideoRealtimeBar::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_EXTEND,				m_buttonLocker);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_CAPTURE,				m_buttonSnapShot);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_RECORD,				m_buttonLocalRecord);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_LABEL,				m_buttonBookMark);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_MAGNIFY,				m_buttonMagnify);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_SOUND,				m_buttonPlaySound);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_TALKBACK,			m_buttonTalkback);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_TVWALL,				m_buttonTVWall);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_INSTANT_BACK_PLAY,	m_buttonInstantBackPlay);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_SUPER_INTEL,			m_buttonSuperIntel);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_E_MAP,				m_buttonEMap);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_SET_WARN_WND,		m_buttonSetWardWnd);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_MASTER_AUXILIARY,	m_buttonMasterMAuxiliary);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_PTZ,					m_buttonPtz);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_NET_QUALITY,			m_buttonNetQuality);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_PTZ3D,               m_BtnPtz3D);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_RETURN,              m_btnReturn);
	DDX_Control(pDX, IDC_BTN_TOOL_REALTIME_START,               m_btnStart);
}

BEGIN_MESSAGE_MAP(CTranVideoRealtimeBar, CTranslucentDialog)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_CAPTURE, &CTranVideoRealtimeBar::OnBnClickedButtonRealtimeCapture)
    ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_RECORD, &CTranVideoRealtimeBar::OnBnClickedButtonRealtimeRecord)
    ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_LABEL, &CTranVideoRealtimeBar::OnBnClickedButtonRealtimeLabel)
    ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_MAGNIFY, &CTranVideoRealtimeBar::OnBnClickedButtonRealtimeMagnify)
    ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_SOUND, &CTranVideoRealtimeBar::OnBnClickedButtonRealtimeSound)
    ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_TALKBACK, &CTranVideoRealtimeBar::OnBnClickedButtonRealtimeTalkback)
    ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_EXTEND, &CTranVideoRealtimeBar::OnBnClickedButtonRealtimeExtent)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_NET_QUALITY, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeNetQuality)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_MASTER_AUXILIARY, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeMasterAuxiliary)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_INSTANT_BACK_PLAY, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeInstantBackPlay)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_TVWALL, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeTvwall)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_SET_WARN_WND, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeSetWarnWnd)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_PTZ, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimePtz)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_SUPER_INTEL, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeSuperIntel)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_E_MAP, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeEMap)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_PTZ3D, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimePtz3d)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_RETURN, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeReturn)
	ON_BN_CLICKED(IDC_BTN_TOOL_REALTIME_START, &CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeStart)
	ON_MESSAGE(IDC_MENU, &CTranVideoRealtimeBar::OnIdcMenu)
    ON_MESSAGE(WM_SETSOUND_STATE, &CTranVideoRealtimeBar::OnSetSoundState)
	//ON_COMMAND_RANGE()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

// Qualifier: 加载所有PNG控件的 PNG 图片资源
void CTranVideoRealtimeBar::OnInitChildrenWnds()
{
	//扩展
	SetButtonPicture(m_buttonLocker,      _T("Extend"));
	//抓拍
	SetButtonPicture(m_buttonSnapShot,	  _T("SnapShot"));
	//本地录像
	SetButtonPicture(m_buttonLocalRecord, _T("LocalRecord"));
	//书签
	SetButtonPicture(m_buttonBookMark,	  _T("BookMark"));
	//局部放大
	SetButtonPicture(m_buttonMagnify,	  _T("Zoom"));
	//即时回放
	SetButtonPicture(m_buttonInstantBackPlay,   _T("PlayRecord"));
	//声音
	SetButtonPicture(m_buttonPlaySound,    _T("PlayNoSound"));
	//对讲
	SetButtonPicture(m_buttonTalkback,	   _T("TalkBack"));
	//视屏上墙
	SetButtonPicture(m_buttonTVWall,		_T("VideoTVW"));
	//设置告警窗口
	SetButtonPicture(m_buttonSetWardWnd,	_T("AlarmWin"));
	//云镜控制
	SetButtonPicture(m_buttonPtz,			_T("PTZ"));
	//叠加智能分析
	SetButtonPicture(m_buttonSuperIntel,	_T("IA"));
	//打开电子地图
	SetButtonPicture(m_buttonEMap,			_T("OpenMap"));
	//一主多辅
	SetButtonPicture(m_buttonMasterMAuxiliary,	_T("WindowMain"));
	//网速优先/画质优先
	SetButtonPicture(m_buttonNetQuality,    _T("PlayQuality"));
    //3D定位
    SetButtonPicture(m_BtnPtz3D,            _T("PTZ3D"));
	//返回
	SetButtonPicture(m_btnReturn,      _T("ReturnBack"));
	//开始播放
	SetButtonPicture(m_btnStart,	  _T("StartBack"));

	UpdateView();
}

void CTranVideoRealtimeBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawButton(m_iNumBtn);
	UpdateView();
}

int CTranVideoRealtimeBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CTranslucentDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

HBRUSH CTranVideoRealtimeBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CTranslucentDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    return hbr;
}

BOOL CTranVideoRealtimeBar::OnInitDialog()
{
    CTranslucentDialog::OnInitDialog();
    m_btnVector.push_back(&m_buttonSnapShot);
    m_btnVector.push_back(&m_buttonLocalRecord);
    m_btnVector.push_back(&m_buttonBookMark);
    m_btnVector.push_back(&m_buttonMagnify);
	m_btnVector.push_back(&m_buttonInstantBackPlay);
    m_btnVector.push_back(&m_buttonPlaySound);
    m_btnVector.push_back(&m_buttonTalkback);
    m_btnVector.push_back(&m_buttonTVWall);
	m_btnVector.push_back(&m_buttonSetWardWnd);
	m_btnVector.push_back(&m_buttonPtz);
	m_btnVector.push_back(&m_buttonSuperIntel);
	m_btnVector.push_back(&m_buttonEMap);
	m_btnVector.push_back(&m_buttonMasterMAuxiliary);
	m_btnVector.push_back(&m_buttonNetQuality);
    m_btnVector.push_back(&m_BtnPtz3D);

    EndBuffered();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CTranVideoRealtimeBar::CreateSoundCtrl()
{
    if (nullptr == m_pVideoSoundCtrl)
    {
        m_pVideoSoundCtrl = IVS_NEW((CVideoSoundCtrl*&)m_pVideoSoundCtrl);
        CHECK_POINTER_VOID(m_pVideoSoundCtrl);
        m_pVideoSoundCtrl->SetSoundCtrlType(SOUNDCTRL_WND_TYPE_SOUND);
        (void)m_pVideoSoundCtrl->Create(IDD_DLG_SOUNDCTRL);
		m_bSoundCreated = true;
    }
    m_bSoundCtrlState = TRUE;
}

void CTranVideoRealtimeBar::CreateTalkbackCtrl()
{
	if (nullptr == m_pVideoTalkbackSound)
	{
		m_pVideoTalkbackSound = IVS_NEW((CVideoSoundCtrl*&)m_pVideoTalkbackSound);
		CHECK_POINTER_VOID(m_pVideoTalkbackSound);
		m_pVideoTalkbackSound->SetSoundCtrlType(SOUNDCTRL_WND_TYPE_TALKBACK);
		(void)m_pVideoTalkbackSound->Create(IDD_DLG_SOUNDCTRL);
		m_bTalkbackSoundCreated = true;
	}
	m_bSoundCtrlState = TRUE;
}

void CTranVideoRealtimeBar::DeleteSoundCtrl()
{
	if (m_bSoundCreated)
	{
		m_bSoundCreated = false;
		IVS_DELETE(m_pVideoSoundCtrl);
	}
	m_bSoundCtrlState = FALSE;
}

void CTranVideoRealtimeBar::DeletePopMenuBar()
{
	if (m_bMenuCreated)
	{
		m_bMenuCreated = false;
		IVS_DELETE(m_pMenu);
	}
}

void CTranVideoRealtimeBar::OnBnClickedButtonRealtimeCapture()
{
	IVS_INFO_TRACE("");
	SetCurrentPaneToActivePane();

    CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);

    CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
    CHECK_POINTER_VOID(pCtrl);
// 	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_SNAPSHOT, pVideoPane->GetPaneID());

	(void)pCtrl->IVS_OCX_LocalSnapshot((LONG)pVideoPane->GetPaneID());
}

void CTranVideoRealtimeBar::OnBnClickedButtonRealtimeRecord()
{
    SetCurrentPaneToActivePane();
    CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);

    CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
    CHECK_POINTER_VOID(pCtrl);
    pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_LOCAL_RECORD, pVideoPane->GetPaneID());

    if (!pVideoPane->GetIsLocalRecord())
    {
		unsigned long result = pCtrl->IVS_OCX_StartLocalRecord(NULL, (LONG)pVideoPane->GetPaneID());
        if (IVS_SUCCEED == result)
        {
            pCtrl->PostLocRecEventGeneric(5, (int)pVideoPane->GetPaneID()); // 5 为录像开始
        }
		else if (IVS_PLAYER_RET_DISK_SPACE_ERROR == result)
		{
	        pCtrl->PostLocRecEventGeneric(3, (int)pVideoPane->GetPaneID()); // 3 :磁盘空间满
		}
		else
		{
			pCtrl->PostLocRecEventGeneric(0, (int)pVideoPane->GetPaneID()); // 0 为通用错误
		}
    } 
    else 
    {
        (void)pCtrl->IVS_OCX_StopLocalRecord((LONG)pVideoPane->GetPaneID());
        pCtrl->PostLocRecEventGeneric(1, (int)pVideoPane->GetPaneID());     // 1 为录像停止
    }
}

void CTranVideoRealtimeBar::OnBnClickedButtonRealtimeLabel()
{
	SetCurrentPaneToActivePane();

	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

    //通知CU，由他来调用OCX的接口
    CVideoPaneMgr* pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER_VOID(pVideoPaneMgr);

    //没有在实况，事件不触发
    if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
    {
        return;
    }

    // 删除声音控制对话框
    pVideoPaneMgr->DeleteSoundCtrl();
    pVideoPane->GetOCXPlayerCtrl()->PostToolBarClickEventMsg(2,IVS_TOOLBAR_BOOKMARK, pVideoPane->GetPaneID());
}

// 单击局部放大
void CTranVideoRealtimeBar::OnBnClickedButtonRealtimeMagnify()
{
	SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);

	// 已经是局部放大了，关闭局部放大
	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
	{
		std::string strButStatus = CToolsHelp::Int2Str(MAGNIFY_CLOSE);
		pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_ZOOM, pVideoPane->GetPaneID(), strButStatus);//lint !e1036

		// 删除源窗口的所有并停止其他窗口的放大效果
		::SendMessage(pVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE_ALL, 0, 0);
		(void)pVideoPane->SetRealtimeMagnifyState(MAGNIFY_CLOSE);
		SetMagnifyBtnState(false);
		// 关闭智能分析画图
		(void)pCtrl->IVS_OCX_DisableDraw(pVideoPane->GetPaneID());
		// 恢复所有窗口的局部放大按钮
		pCtrl->GetVideoPaneMgr()->SetALLMagnifyBtnStatus(pVideoPane, 1);//lint !e747
		return;
	}
	// TODO 其他案件也应该这样处理，防止各种上抛事件异常
	std::string strButStatus = CToolsHelp::Int2Str(MAGNIFY_OPEN);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_ZOOM, pVideoPane->GetPaneID(), strButStatus);//lint !e1036

	// 已经是PTZ,需要关闭云台
	if (BOTTOMBAR_PTZ_OPEN == pVideoPane->GetRealtimePtzState())
	{
		// 删除云镜控制
		pVideoPane->DeleteDigtalPtz();
		pVideoPane->SetRealtimePtzState(BOTTOMBAR_PTZ_CLOSE);
		SetPtzBtnState(false);
	}

	//没有在实况，事件不触发
	if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState() || VIDEO_PLAY_STATE_RESTARTING == pVideoPane->GetPlayState())
	{
		return;
	}
	// 除本窗口外。禁用其他窗口的局部放大按钮
	if (NULL != pCtrl->GetVideoPaneMgr())
	{
		pCtrl->GetVideoPaneMgr()->SetALLMagnifyBtnStatus(pVideoPane,0);//lint !e747
	}

	// 开启画图
	(void)pCtrl->IVS_OCX_EnableDraw(pVideoPane->GetPaneID());
	// 设置局部放大状态
	(void)pVideoPane->SetRealtimeMagnifyState(MAGNIFY_OPEN);
	pCtrl->GetVideoPaneMgr()->SetMagnifyLayOut(pCtrl->GetVideoPaneMgr()->GetWindLayOut());
	// 设置按钮按下状态
	SetMagnifyBtnState(true);
}

// 单击实况工具栏上声音按钮
void CTranVideoRealtimeBar::OnBnClickedButtonRealtimeSound()
{
    SetCurrentPaneToActivePane();
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER_VOID(pVideoPaneMgr);
    // 删除自适应大小对话框

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_PLAY_SOUND, pVideoPane->GetPaneID());

    if (!m_bitsetButtonUsableStatus.test(6))
    {
        return;
    }
    if (!m_bitsetButtonCompetenceStatus.test(6))
    {
        return;
    }

    // 删除声音控制对话框
    pVideoPaneMgr->DeleteSoundCtrlForOther();
    // 删除自身语音对讲对话框
    if (m_bSoundCtrlState)
    {
        // 删除控制声音对话框
        DeleteSoundCtrl();
    } else {
        // 如果有语音对讲,则不做处理
        if(IsTalkback())
        {
            // modi by wKF71044 at [2013-6-4 16:30:52] DTS2013052309584
            // 根据问题单要求需要上报给CU
            pCtrl->PostSoundEvent();
            return;
        }
        DeleteTalkbackCtrl();
        CreateSoundCtrl();

        if (NULL != m_pVideoSoundCtrl)
        {
            CRect rect;
            m_buttonPlaySound.GetWindowRect(&rect);

            rect.left   -= static_cast<long>((SOUNDCTRL_WIDTH - static_cast<ULONG>(rect.Width()))/2);
            rect.right  = rect.left + static_cast<long>(SOUNDCTRL_WIDTH);
            rect.bottom -= 20;
            rect.top   = rect.bottom - static_cast<long>(SOUNDCTRL_SLIDER_HEIGHT);
            m_ulSoundCtrlLeft = static_cast<ULONG>(rect.left);
            m_ulSoundCtrlTop  = static_cast<ULONG>(rect.top);
            m_pVideoSoundCtrl->SetVideopBar(this);
            m_pVideoSoundCtrl->MoveWindow(rect);
            m_pVideoSoundCtrl->ShowWindow(SW_SHOW);
            m_bSoundCtrlState = TRUE;
        }
        //当前是静音状态表示未开启声音，用上一次存储的值，否则获取系统当前音量
//         if(!m_bSoundState)
//         {
//             DWORD ulVolume = 0;
//             (void)waveOutGetVolume(0, &ulVolume);
//             ulVolume = ulVolume / (VOLUME_FULL / 100);
//             m_ulSoundCtrlValue = ulVolume;
//         }
        InitSoundCtrl(SOUNDCTRL_WND_TYPE_SOUND);
		if (NULL != m_pVideoSoundCtrl)
		{
			m_pVideoSoundCtrl->Invalidate(TRUE);
		}
    }
}

void CTranVideoRealtimeBar::OnBnClickedButtonRealtimeTalkback()
{
	IVS_INT32 iRet = IVS_FAIL;
	std::string strResult;
	SetCurrentPaneToActivePane();
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER_VOID(pVideoPaneMgr);

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	
	// 删除声音控制对话框
	pVideoPaneMgr->DeleteSoundCtrlForOther();

	// 已经开启了对讲，关闭对讲
	if (BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
		iRet = StopTalkbackSound();
		
		if (IVS_SUCCEED != iRet)
		{
			strResult = "1#" + CToolsHelp::Int2Str(iRet);
			// 上抛关闭的状态
			pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_TALKBACK, pVideoPane->GetPaneID(), strResult.c_str());
			return;
		}
		// 删除语音对话框
		DeleteTalkbackCtrl();
		strResult = "0#" + CToolsHelp::Int2Str(iRet);
		// 上抛关闭的状态
		pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_TALKBACK, pVideoPane->GetPaneID(), strResult.c_str());
		return;
	}

    // 如果有语音对讲
    if("" != CIVS_OCXPlayerCtrl::m_strVoiceTalkbackCameraCode)
	{
		// modi by wKF71044 at [2013-6-4 16:30:52] DTS2013052308679 
		// 根据问题单要求需要上报给CU
		pCtrl->PostTalkbackEvent();
		return;
	}
    DeleteSoundCtrl();
	CreateTalkbackCtrl();
	if (NULL != m_pVideoTalkbackSound)
	{
		m_pVideoTalkbackSound->ShowWindow(SW_HIDE);
	}
	iRet = StartTalkbackSound();
	if (IVS_SUCCEED != iRet)
	{
		// 上抛错误码提示
		strResult = "1#" + CToolsHelp::Int2Str(iRet);
		pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_TALKBACK, pVideoPane->GetPaneID(),strResult.c_str());
		return;
	}

    //  modify by wanglei 00165153:2013.6.10 DTS2013060606556 
    // 开启语音对讲，需要关闭任一随路语音
	CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
	SoundMgr.SetSoundingPanel(NULL);

	strResult = "0#" + CToolsHelp::Int2Str((int)pCtrl->m_ulVoiceTalkbackHandle);//lint !e1705
	// 上抛开启状态，直接把句柄也抛上去
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_TALKBACK, pVideoPane->GetPaneID(),strResult.c_str());

	if (NULL != m_pVideoTalkbackSound && m_bShowTalkbackSoundCtrl)
	{
		m_pVideoTalkbackSound->ShowWindow(SW_SHOW);
		CRect rect;
		m_buttonTalkback.GetWindowRect(&rect);
		CRect rect1;
		this->GetClientRect(&rect1);
		rect.left -= static_cast<long>((SOUNDCTRL_WIDTH - static_cast<ULONG>(rect.Width()))/2);
		rect.right = rect.left + static_cast<long>(SOUNDCTRL_WIDTH);
		rect.bottom -= 20;
		rect.top   = rect.bottom - static_cast<long>(SOUNDCTRL_SLIDER_HEIGHT);
		m_ulSoundCtrlLeft = static_cast<ULONG>(rect.left);
		m_ulSoundCtrlTop  = static_cast<ULONG>(rect.top);
		m_pVideoTalkbackSound->SetVideopBar(this);
		m_pVideoTalkbackSound->MoveWindow(rect);
		m_pVideoTalkbackSound->ShowWindow(SW_SHOW);
		m_bTalkbackSoundState = TRUE;
	}
	if (NULL != m_pVideoTalkbackSound)
	{
		m_pVideoTalkbackSound->SetTalkbackSilencePic(TRUE);

		m_pVideoTalkbackSound->Invalidate(TRUE);
		// 解决在关闭声音情况下，关闭对讲后重新开启的情况,sleep时间过短会导致无法播放声音
 		Sleep(500);
		m_pVideoTalkbackSound->PostMessage(WM_BITMAPSLIDER_MOVED,IDC_SLIDER_SOUNDCTRL,0);
	}
}

void CTranVideoRealtimeBar::DrawButton(IVS_INT32 iBtnNum)
{
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    CVideoPaneMgr * m_pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER_VOID(m_pVideoPaneMgr);

    CRect rect;
    m_pVideoPane->GetClientRect(&rect);
	//GetClientRect(rect);

	IVS_INT32 iButtonOffSetNUm = 1;	//减1是给工具栏的隐藏键留位置
	IVS_INT32 iButtonMaxShowNum = MAXNUM_SHOW_BUTTON;//7表示实况界面工具栏初始显示7个按钮
	int nBtnLeftIndex = rect.left + 1;
    const IVS_INT32 nAllowBtnNum = rect.Width() / REALTIMEBAR_BUTTON_WITDH - iButtonOffSetNUm;

	IVS_INT32 _iBtnNum = 0;
	if (nAllowBtnNum >= iBtnNum )
	{
		_iBtnNum = iBtnNum;
	}else if (nAllowBtnNum > iButtonMaxShowNum) {
		_iBtnNum = iButtonMaxShowNum;
	}else{
		_iBtnNum = nAllowBtnNum;
	}

	FunctionObjectDrawButtonPosition Menu =
		for_each(m_btnVector.begin(),m_btnVector.end(),
		FunctionObjectDrawButtonPosition(this,_iBtnNum,nBtnLeftIndex));

	iBtnNum = static_cast<int >(GetBitsetButtonVisableStatus().count());
	if(m_pVideoPaneMgr->GetCurTVWallID() == 0)//处理非电视墙的工具栏显示
	{
		if (iBtnNum < MAXNUM_SHOW_BUTTON &&iBtnNum*16<rect.Width())
		{
			m_buttonLocker.ShowWindow(SW_HIDE);
		}
		else
		{
			m_buttonLocker.ShowWindow(SW_SHOW);
			m_buttonLocker.MoveWindow(Menu.value(), 2, 16, 16);
		}
	}
	else//处理电视墙的工具栏显示
	{
		IVS_INT32 iMaxShowButtonNum = 4;//实况下的电视墙显示4个按钮
		if(pVideoPane->GetToolBarBtnVisableStatus() == 7)//录像下面的电视墙显示3个按钮
		{
			iMaxShowButtonNum = 3;
		}
		if (nAllowBtnNum >= iMaxShowButtonNum)
		{
			m_buttonLocker.ShowWindow(SW_HIDE);
		}
		else
		{
			m_buttonLocker.ShowWindow(SW_SHOW);
			m_buttonLocker.MoveWindow(Menu.value(), 2, 16, 16);
		}
	}

	if (m_bIsInstansPlay)
	{
		//及时回放按钮特殊处理
		m_btnReturn.ShowWindow(SW_SHOW);
		m_btnReturn.MoveWindow(0, 2, 16, 16);
		m_btnStart.ShowWindow(SW_SHOW);
		m_btnStart.MoveWindow(REALTIMEBAR_BUTTON_WITDH, 2, 16, 16);
	}
	else
	{
		m_btnReturn.ShowWindow(SW_HIDE);
		m_btnStart.ShowWindow(SW_HIDE);
	}
	// 展开的时候看一下对讲是否开启，开启展开的时候就屏蔽掉语音
	if (BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
		m_buttonPlaySound.EnableWindow(FALSE);
	}

	if (FULL_SCREEN == m_pVideoPaneMgr->GetFullScreenState())
	{
		(void)::SetWindowLong(this->GetSafeHwnd(),GWL_HWNDPARENT,(LONG)(0));
	}
	else
	{
		(void)::SetWindowLong(this->GetSafeHwnd(),GWL_HWNDPARENT,m_pVideoPaneMgr->GetParentHwnd());//lint !e713
	}
}//lint !e550 !e529

ULONG CTranVideoRealtimeBar::GetClientMode() const
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane,CLIENT_PC);
    return (ULONG)pVideoPane->GetClientType();
}

void CTranVideoRealtimeBar::OnBnClickedButtonRealtimeExtent()
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER_VOID(pVideoPaneMgr);
	// 删除声音控制对话框
	pVideoPaneMgr->DeleteSoundCtrlForOther();
	if (m_isExtend)
	{
		m_iNumBtn = MAXNUM_SHOW_BUTTON;
		DrawButton(m_iNumBtn);
		m_isExtend = false;
		//扩展
		UnregisterTranslucentWnd(&m_buttonLocker);
		SetButtonPicture(m_buttonLocker, "Extend");
		UpdateView();
	} else{
		m_iNumBtn = static_cast<int >(GetBitsetButtonVisableStatus().count());
		DrawButton(m_iNumBtn);
		CreatePopToolBar();
		UpdateView();
	}
}

void CTranVideoRealtimeBar::CreatePopToolBar()
{
	//IVS_DELETE(m_pMenu);
	// 只有创建这个的时候设置一下，防止对象改变弹不出来啊
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER_VOID(pVideoPaneMgr);
	CUtility::SetVideoPaneMgr(pVideoPaneMgr);

	DeletePopMenuBar();
	m_pMenu = new CIVSMenu();

	m_pMenu->ClearButtonData();
	(void)m_pMenu->AppendMenuItem("",ID_MENU_HEADER,0,0,0,NULL, MENU_NORMAL|MENU_BORDLINE,"");

	(void)std::for_each(m_btnVector.begin(),m_btnVector.end(),FunctionObjectToolBarAppendItem(this));

	CPoint pt;
	GetCursorPos(&pt);

	(void)m_pMenu->AppendMenuItem("",ID_MENU_TRAIL , 0,0,0, NULL, MENU_NORMAL|MENU_BORDLINE,"");
	(void)m_pMenu->CreateGroupMenu(this,CRect(0,0,173,290),IDC_MENU,0,1);

	if (m_pMenu->GetItemNum() > 2)
	{
		m_pMenu->ShowMenuPos(pt.x,pt.y,LEFTBOTTOM);
		m_isExtend = false;
		//扩展
		UnregisterTranslucentWnd(&m_buttonLocker);
		SetButtonPicture(m_buttonLocker, "Extend");
	}
	else{
		m_isExtend = true;
		//扩展
		UnregisterTranslucentWnd(&m_buttonLocker);
		SetButtonPicture(m_buttonLocker, "Retracted");
	}

	//对不用的菜单进行置灰操作
	std::vector<CTranslucentButton *>::const_iterator it;
	for (it = m_btnVector.begin(); it != m_btnVector.end(); ++it)
	{
		CTranslucentWnd* translucentWnd = *it;
		m_pMenu->EnableMenuStatus((unsigned int)translucentWnd->GetDlgCtrlID(),m_bIsButtonEnable);
	}
	//告警,一主多辅，地图
	m_pMenu->EnableMenuStatus(IDC_BTN_TOOL_REALTIME_SET_WARN_WND,TRUE);
	m_pMenu->EnableMenuStatus(IDC_BTN_TOOL_REALTIME_MASTER_AUXILIARY,TRUE);
	m_pMenu->EnableMenuStatus(IDC_BTN_TOOL_REALTIME_E_MAP,TRUE);
	m_bMenuCreated = true;
} //lint !e550

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeNetQuality()
{
	SetCurrentPaneToActivePane();
	//设置视频流畅度、清晰度按钮
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_PLAY_QUALITY, pVideoPane->GetPaneID());

	//没有在实况，事件不触发
	if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
	{
		return;
	}
	ULONG ulWinID = pVideoPane->GetPaneID();
	LONG  lPalyQualityMode = PLAY_QUALITY_FLUENCY; //默认设置网络;
	//如果当前模式为网络优先
	if (PLAY_QUALITY_FLUENCY == m_lPalyQualityMode)
	{
		SetButtonPicture(m_buttonNetQuality, "PlayQuality"); //设置按钮为画质优先
		lPalyQualityMode = m_lPalyQualityMode;
		m_lPalyQualityMode = PLAY_QUALITY_DEFINITION;    //修改当前播放为画质优先
	}
	else
	{
		SetButtonPicture(m_buttonNetQuality, "PlayNet");// 网络优先
		lPalyQualityMode = m_lPalyQualityMode;
		m_lPalyQualityMode = PLAY_QUALITY_FLUENCY;       //修改当前播放为网络优先
	}
	if (NULL != pVideoPane->GetOCXPlayerCtrl())
	{
		pVideoPane->GetOCXPlayerCtrl()->IVS_OCX_SetPlayQualityMode(ulWinID,lPalyQualityMode);
	}
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeMasterAuxiliary()
{
    SetCurrentPaneToActivePane();
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER_VOID(pVideoPaneMgr);

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_WINDOW_MAIN, pVideoPane->GetPaneID());

	ULONG _ulPaneNUm = pVideoPaneMgr->GetNumOfDisplayPane();

	if (_ulPaneNUm < LADDERLAYOUTMINWINDOWSNUM || _ulPaneNUm > LADDERLAYOUTMAXWINDOWSNUM){
		return;
	}
	CRect _Rect;
    pCtrl->GetClientRect(&_Rect);
	ULONG ulLadderLayoutWinNum = 0;

	if (LayoutMgr.isLadderLayout_6(_ulPaneNUm)){
		ulLadderLayoutWinNum = 6;
		(void)pVideoPaneMgr->SetWndLayout(_Rect,WIND_LAYOUT_6_3);
	}else if (LayoutMgr.isLadderLayout_8(_ulPaneNUm))
	{
		ulLadderLayoutWinNum = 8;
		(void)pVideoPaneMgr->SetWndLayout(_Rect,WIND_LAYOUT_8_1);
	}else if (LayoutMgr.isLadderLayout_10(_ulPaneNUm))
	{
		ulLadderLayoutWinNum = 10;
		(void)pVideoPaneMgr->SetWndLayout(_Rect,WIND_LAYOUT_10_2);
	}else if (LayoutMgr.isLadderLayout_12(_ulPaneNUm))
	{
		ulLadderLayoutWinNum = 12;
		(void)pVideoPaneMgr->SetWndLayout(_Rect,WIND_LAYOUT_12_2);
	}else if (LayoutMgr.isLadderLayout_16(_ulPaneNUm))
	{
		ulLadderLayoutWinNum = 16;
		(void)pVideoPaneMgr->SetWndLayout(_Rect,WIND_LAYOUT_16_2);
	} else if (LayoutMgr.isLadderLayout_20(_ulPaneNUm))
	{
		ulLadderLayoutWinNum = 20;
		(void)pVideoPaneMgr->SetWndLayout(_Rect,WIND_LAYOUT_20_1);
	}
	std::string strPostXML;
	PostEventXMLImpl.GetLadderLayoutEventMsg(strPostXML,ulLadderLayoutWinNum);

	pCtrl->IVS_OCX_Windows_Event(EVENT_LADDER_LAYOUT_WINDOW,strPostXML.c_str());
	// 交换这个窗格到第一个窗格（最大的窗格）
	pVideoPaneMgr->ExchangePane(1);
}

void CTranVideoRealtimeBar::SetToolbarEnableStatus( ULONGLONG dwToolbarBtn, ULONGLONG lButtonStatus )
{
	std::bitset<TOOLBARBTNNUM> bitsetButtonStatus(dwToolbarBtn);
	IVS_UINT32 _iCount = 0;

	size_t s = m_btnVector.size();

	for (_iCount = 0; _iCount < TOOLBARBTNNUM && _iCount < s; _iCount++)
	{
		if (bitsetButtonStatus.test(_iCount))
		{
			m_btnVector[_iCount]->EnableWindow(static_cast<BOOL>(lButtonStatus));
			m_btnVector[_iCount]->Invalidate(TRUE);
		}
	}
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeInstantBackPlay()
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	// 如果局部放大开启了，需要关闭
	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
	{
		// 删除源窗口的所有并停止其他窗口的放大效果
		::PostMessage(pVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE_ALL, 0, 0);
		(void)pVideoPane->SetRealtimeMagnifyState(MAGNIFY_CLOSE);
		SetMagnifyBtnState(false);
		CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
		if (NULL != pCtrl)
		{
			// 关闭智能分析画图
			(void)pCtrl->IVS_OCX_DisableDraw(pVideoPane->GetPaneID());
		}
	}

	// 如果云镜开启了，需要关闭
	if (BOTTOMBAR_PTZ_OPEN == pVideoPane->GetRealtimePtzState())
	{
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
		::SendMessage(this->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_PTZ, BN_CLICKED), (LPARAM)this->GetPTZBtnHwnd());
	}

	// 轮训按钮
	if (TYPE_WND_TURNCTUISE == pVideoPane->GetTypeWnd())
	{
		pVideoPane->SetPrveTurnCruiseStyle(pVideoPane->GetTurnCruiseStyle());
		pVideoPane->SetTurnCruiseStyle(0);
		pVideoPane->SetPrveTypeWnd(pVideoPane->GetTypeWnd());
	}

	// 已经开启了对讲，关闭对讲
	if (BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
		(void)StopTalkbackSound();
		DeleteTalkbackCtrl();
		// 上抛关闭的状态
		CIVS_OCXPlayerCtrl *pOcx = pVideoPane->GetOCXPlayerCtrl();
		if (pOcx)
		{
			std::string strResult = "0#" + CToolsHelp::Int2Str(IVS_SUCCEED);
			pOcx->PostToolBarClickEventMsg(2,IVS_TOOLBAR_TALKBACK, pVideoPane->GetPaneID(), strResult.c_str());
		}
	}
	 
	//弹出及时回放标尺
	pVideoPane->ShowInstantReplayBar(1);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_PLAY_RECORD, pVideoPane->GetPaneID());
	pVideoPane->ShowInstantBar();
	pVideoPane->Invalidate();
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeTvwall()
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_VIDEO_TVW, pVideoPane->GetPaneID());
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeSetWarnWnd()
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_ALARM_WIN, pVideoPane->GetPaneID());
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimePtz()
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);

	// 已经是PTZ
	if (BOTTOMBAR_PTZ_OPEN == pVideoPane->GetRealtimePtzState())
	{
		// 轮训按钮
		if (TYPE_WND_TURNCTUISE == pVideoPane->GetPrveTypeWnd())
		{
			pVideoPane->SetTypeWnd(pVideoPane->GetPrveTypeWnd());
			pVideoPane->SetPrveTypeWnd(TYPE_WND_NOMAL);
			pVideoPane->SetTurnCruiseStyle(pVideoPane->GetPrveTurnCruiseStyle());
			pVideoPane->SetPrveTurnCruiseStyle(0);
			pVideoPane->ResetBarPosition();
		}

		std::string strButStatus = CToolsHelp::Int2Str(BOTTOMBAR_PTZ_CLOSE);
		pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_PTZ, pVideoPane->GetPaneID(), strButStatus);//lint !e1036

		// 删除云镜控制
		pVideoPane->DeleteDigtalPtz();
		pVideoPane->SetRealtimePtzState(BOTTOMBAR_PTZ_CLOSE);
		SetPtzBtnState(false);
		return;
	}
	// 轮训按钮
	if (TYPE_WND_TURNCTUISE == pVideoPane->GetTypeWnd())
	{
		pVideoPane->SetPrveTurnCruiseStyle(pVideoPane->GetTurnCruiseStyle());
		pVideoPane->SetTurnCruiseStyle(0);
		pVideoPane->SetPrveTypeWnd(pVideoPane->GetTypeWnd());
		// 设置窗口类型
		pVideoPane->SetTypeWnd(TYPE_WND_NOMAL);
	}

	std::string strButStatus = CToolsHelp::Int2Str(BOTTOMBAR_PTZ_OPEN);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_PTZ, pVideoPane->GetPaneID(), strButStatus);//lint !e1036

	// 如果局部放大开启了，需要关闭
	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
	{
		// 删除源窗口的所有并停止其他窗口的放大效果
		::PostMessage(pVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE_ALL, 0, 0);
		(void)pVideoPane->SetRealtimeMagnifyState(MAGNIFY_CLOSE);
		SetMagnifyBtnState(false);
		// 关闭智能分析画图
		(void)pCtrl->IVS_OCX_DisableDraw(pVideoPane->GetPaneID());

		// 恢复所有窗口的局部放大按钮
		pCtrl->GetVideoPaneMgr()->SetALLMagnifyBtnStatus(pVideoPane, 1);//lint !e747
	}

	//没有在实况，事件不触发
	if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState() || VIDEO_PLAY_STATE_RESTARTING == pVideoPane->GetPlayState())
	{
		return;
	}
	// 开启云镜
	pVideoPane->CreateDigtalPtz();
	pVideoPane->ResetBarPosition();
	// 设置云镜状态
	pVideoPane->SetRealtimePtzState(BOTTOMBAR_PTZ_OPEN);
	SetPtzBtnState(true);
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeSuperIntel()
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_IA, pVideoPane->GetPaneID());
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeEMap()
{
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	//pVideoPane->SetPlayState(VIDEO_PLAY_STATE_EMAP);
	pVideoPane->Invalidate(TRUE);

	CRect rect;
	pVideoPane->GetClientRect(rect);
	pCtrl->PostToolBarEMapClickEventMsg(2,IVS_TOOLBAR_OPEN_MAP, (ULONG)pVideoPane->GetSafeHwnd(),rect);
}

afx_msg LRESULT CTranVideoRealtimeBar::OnIdcMenu(WPARAM wParam, LPARAM /*lParam*/)
{
	RoutineButtonShowMenuEvent(wParam);
	RoutineButtonHideMenuEvent(wParam);

	return IVS_SUCCEED;
}

void CTranVideoRealtimeBar::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}

/******************************************************************
//function : RoutineButtonShowMenuEvent
//description:
//input : WPARAM wParam
//output : NA
//return : void
*******************************************************************/
void CTranVideoRealtimeBar::RoutineButtonShowMenuEvent( WPARAM wParam )
{
	if( IDC_BTN_TOOL_REALTIME_CAPTURE == wParam ){
		OnBnClickedButtonRealtimeCapture();
	} else if( IDC_BTN_TOOL_REALTIME_RECORD == wParam ){
		OnBnClickedButtonRealtimeRecord();
	} else if( IDC_BTN_TOOL_REALTIME_LABEL == wParam ){
		OnBnClickedButtonRealtimeLabel();
	} else if( IDC_BTN_TOOL_REALTIME_MAGNIFY == wParam ){
		OnBnClickedButtonRealtimeMagnify();
	} else if( IDC_BTN_TOOL_REALTIME_SOUND == wParam ){
		OnBnClickedButtonRealtimeSound();
	} else if( IDC_BTN_TOOL_REALTIME_TALKBACK == wParam ){
		OnBnClickedButtonRealtimeTalkback();
	} else if( IDC_BTN_TOOL_REALTIME_INSTANT_BACK_PLAY == wParam ){
		OnBnClickedBtnToolRealtimeInstantBackPlay();
	}
}

/******************************************************************
//function : RoutineButtonHideMenuEvent
//description:
//input : WPARAM wParam
//output : NA
//return : void
*******************************************************************/
void CTranVideoRealtimeBar::RoutineButtonHideMenuEvent( WPARAM wParam )
{
	if( IDC_BTN_TOOL_REALTIME_NET_QUALITY == wParam ){
		OnBnClickedBtnToolRealtimeNetQuality();
	} else if( IDC_BTN_TOOL_REALTIME_MASTER_AUXILIARY == wParam ){
		OnBnClickedBtnToolRealtimeMasterAuxiliary();
	} else if( IDC_BTN_TOOL_REALTIME_TVWALL == wParam ){
		OnBnClickedBtnToolRealtimeTvwall();
	} else if( IDC_BTN_TOOL_REALTIME_SET_WARN_WND == wParam ){
		OnBnClickedBtnToolRealtimeSetWarnWnd();
	} else 	if( IDC_BTN_TOOL_REALTIME_PTZ == wParam ){
		OnBnClickedBtnToolRealtimePtz();
	} else 	if( IDC_BTN_TOOL_REALTIME_SUPER_INTEL == wParam ){
		OnBnClickedBtnToolRealtimeSuperIntel();
	} else 	if( IDC_BTN_TOOL_REALTIME_E_MAP == wParam ){
		OnBnClickedBtnToolRealtimeEMap();
    } else 	if( IDC_BTN_TOOL_REALTIME_PTZ3D == wParam ){
        OnBnClickedBtnToolRealtimePtz3d();
    }
}

/******************************************************************
function : CTranslucentButton
description:
output : NA
return : void
*******************************************************************/
void CTranVideoRealtimeBar::SetButtonPicture(
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

	TCHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]={0x0};

	memset(pcImageList,0x0,256*5);
	
	/*
	 * DTS2013122007506  modify by w00210470 TO BE FIXED!!
	 * 下面两个函数中有一个可能发生异常，暂测试捕获
	 * LoadImageList 内部增加代码捕获
	 * ToolBarGetPNGButtonByName 全局捕获
	 */
	try 
	{
		(void)SkinMgr.ToolBarGetPNGButtonByName(strButtonName,pcImageList);
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "ToolBar GetPNGButtonByName", "Exception");
	}

	(void)tButton.LoadImageList(pcImageList);
	tButton.SetStrFunctionName(strButtonName);
	RegisterTranslucentWnd(&tButton);
}

void CTranVideoRealtimeBar::OnMouseMove(UINT nFlags, CPoint point)
{
    CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    pVideoPane->OnMouseMove(nFlags, point);
    CTranslucentDialog::OnMouseMove(nFlags, point);
}

void CTranVideoRealtimeBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCurrentPaneToActivePane();
    CTranslucentDialog::OnLButtonDown(nFlags, point);
}

void CTranVideoRealtimeBar::SetRecordBtnState(bool state)
{
	CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
	if (NULL != pVideoPane)
	{
		pVideoPane->GetVideoTitleBar()->SetRecordStatus(state ? 1 : 0);
	}
    m_bRecordStarted = state;
    m_buttonLocalRecord.SetChecked(state);
    m_buttonLocalRecord.Invalidate(TRUE);
	if (NULL != pVideoPane)
	{
		if (pVideoPane->GetVideoTitleBar())
		{
			pVideoPane->GetVideoTitleBar()->Invalidate(TRUE);
		}
	}
}

void CTranVideoRealtimeBar::SetMagnifyBtnState(bool state)
{
	//m_bMagnifyStarted = state;
	m_buttonMagnify.SetChecked(state);
	m_buttonMagnify.Invalidate(TRUE);
}

void CTranVideoRealtimeBar::SetTalkbackCtrlState(bool state)
{
	//m_bMagnifyStarted = state;
	m_buttonTalkback.SetChecked(state);
	m_buttonTalkback.Invalidate(TRUE);
}

void CTranVideoRealtimeBar::SetPtzBtnState(bool state)
{
	//m_bMagnifyStarted = state;
	m_buttonPtz.SetChecked(state);
	m_buttonPtz.Invalidate(TRUE);
}

void CTranVideoRealtimeBar::SetPtz3DBtnState(bool state)
{
    m_bPtz3DStarted = state;
    m_BtnPtz3D.SetChecked(state);
    m_BtnPtz3D.Invalidate(TRUE);
}

ULONG CTranVideoRealtimeBar::GetTalkbackSoundState()
{
    CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane,IVS_FAIL);
    return pVideoPane->GetRealtimeTalkbackState();
}

BOOL CTranVideoRealtimeBar::IsTalkback()
{
    CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane,TRUE);
    CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER(pVideoPaneMgr,TRUE);
    return pVideoPaneMgr->IsTalkback();
}

// 设置声音按钮图片
void CTranVideoRealtimeBar::SetSoundPic(ULONG ulState)
{
    CVideoPane *pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);

    if (BOTTOMBAR_SOUND_CLOSE == GetSoundState())
    {
        // 静音
        SetButtonPicture(m_buttonPlaySound,"PlayNoSound");
        if (BUTTON_NORMAL == ulState)
        {
            if (m_bSoundCtrlState)
            {
                m_bSoundState = FALSE;
                pVideoPane->IsSoundOn(false);
            }
        } else if (BUTTON_CLICKED == ulState) {
            m_bSoundState = TRUE;

            pVideoPane->IsSoundOn(true);
        } else {
            m_bSoundState = FALSE;

            pVideoPane->IsSoundOn(false);
        }
    } else if(BOTTOMBAR_SOUND_OPEN == GetSoundState()) {
        SetButtonPicture(m_buttonPlaySound,"PlaySound");
        m_bSoundState = TRUE;

        pVideoPane->IsSoundOn(true);
    }

    //m_ulStatusSound = ulState;
    UpdateView();
}

void CTranVideoRealtimeBar::InitSoundCtrl( ULONG ulType )
{
    switch(ulType)
    {
    case SOUNDCTRL_WND_TYPE_SOUND:
        CHECK_POINTER_VOID(m_pVideoSoundCtrl);
        m_pVideoSoundCtrl->SetSliderPosition(SOUNDCTRL_SLIDER_LEN - m_ulSoundCtrlValue);
        m_pVideoSoundCtrl->SetSoundSilencePic(FALSE);
        m_pVideoSoundCtrl->SetSoundCtrlType(SOUNDCTRL_WND_TYPE_SOUND);

        break;
    case SOUNDCTRL_WND_TYPE_TALKBACK:
        CHECK_POINTER_VOID(m_pVideoTalkbackSound);

        m_pVideoTalkbackSound->SetSliderPosition(SOUNDCTRL_SLIDER_LEN - m_ulTalkbackSoundValue);
        m_pVideoTalkbackSound->SetTalkbackSilencePic(TRUE);
        m_pVideoTalkbackSound->SetSoundCtrlType(SOUNDCTRL_WND_TYPE_TALKBACK);

        break;
    default:
        break;
    }
}

void CTranVideoRealtimeBar::ChangeSoundCtrlValue( ULONG ulType, ULONG ulValue )
{
	IVS_DEBUG_TRACE("Type: %lu, Value: %lu", ulType, ulValue);

	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	ULONG ulWinID = pVideoPane->GetPaneID();

	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER_VOID(pVideoPaneMgr);
 // TODO 加类型，换句柄
	if (BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState() || BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
		DWORD ulVolumeValue = 0;
		ulVolumeValue = (ULONG)(((float)0xFFFF / 100) * (float)ulValue);
		ulVolumeValue = (ulVolumeValue << 16) + ulVolumeValue;
		CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
		CHECK_POINTER_VOID(pCtrl);

		if(IVS_SUCCEED != pCtrl->SetVolume(ulWinID, ulType, ulVolumeValue))
		{
			return;
		}
		
	}
}

ULONG CTranVideoRealtimeBar::GetSoundState()
{
    CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);

    return pVideoPane->GetRealtimeSoundState();
}

void CTranVideoRealtimeBar::StartSound()
{
	IVS_DEBUG_TRACE("");

    CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    ULONG ulWinID = pVideoPane->GetPaneID();

    CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER_VOID(pVideoPaneMgr);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
    // 如果有语音对讲，则不开启随路语音
    if(BOTTOMBAR_TALKBACK_OPEN == pVideoPaneMgr->IsTalkback())
    {
        //BP_RUN_LOG_ERR(IVS_OCX_RET_VOICE_TALKBACK_HAS_PLAYED_ERROR,"","can't open realtime sound, when talkback is open");
        // modi by wKF71044 at [2013-6-4 16:30:52] DTS2013052309584
        // 根据问题单要求需要上报给CU
        pCtrl->PostSoundEvent();
        return;
    }

    CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
    SoundMgr.SetSoundingPanel(NULL); //关闭随路语音
    // 开启随路语音之前先关闭其他窗口的随路语音
    pVideoPaneMgr->StopSoundCtrlForOther(true);

    if (BOTTOMBAR_SOUND_CLOSE == pVideoPane->GetRealtimeSoundState())
    {
		// 开启声音
        IVS_INT32 iRet = (IVS_INT32)pCtrl->StartSound(ulWinID);
        if (IVS_SUCCEED != iRet)
        {
            //  modify by wanglei 00165153:2013.6.10 DTS2013060606556 
            // 如果已经开启语音对讲，则需要通知CU随路语音失败原因
            if (iRet == IVS_PLAYER_RET_TALKBACK_OPENED_ERROR)
            {
                pCtrl->PostSoundEvent();
            }
            return;

        }
		//if(IVS_SUCCEED != pVideoPane->GetOCXPlayerCtrl()->StartSound(ulWinID))
		//{
		//	return;
		//}

		pVideoPane->SetRealtimeSoundState(BOTTOMBAR_SOUND_OPEN);
		pVideoPane->IsSoundOn(true);
		m_bSoundState = TRUE;

        // modify by wanglei 00165153:2013.6.28 DTS2013062702103
        // 需要上报随路语音当前状态，否则录像回放跳段会有问题
        CString xmlToCU;
        xmlToCU ="";
        //CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
        //CHECK_POINTER_VOID(pCtrl);
        PostEventXMLImpl.GetVolumeEventMsg(xmlToCU, pVideoPane->GetPaneID(), 1, 0);
        pCtrl->IVS_OCX_Windows_Event(EVENT_SOUND_STATUS, (LPCTSTR)xmlToCU);

		//声音
		SetButtonPicture(m_buttonPlaySound,    "PlaySound");
		UpdateView();
		//CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
		SoundMgr.SetSoundingPanel(pVideoPane);
		if (NULL != m_pVideoSoundCtrl)
		{
			m_pVideoSoundCtrl->SetSoundSilencePic(TRUE);
		}

        // 通知其他相同摄像机的窗口设置声音为开启状态
        //pVideoPaneMgr->NotifyOtherSoundOpen(ulWinID, true);
	}
}

void CTranVideoRealtimeBar::StopSound()
{
	IVS_DEBUG_TRACE("");

    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    ULONG ulWinID = pVideoPane->GetPaneID();

    CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER_VOID(pVideoPaneMgr);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
    if (BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
    {
		pVideoPane->SetRealtimeSoundState(BOTTOMBAR_SOUND_CLOSE);
		pVideoPane->IsSoundOn(false);
		m_bSoundState = FALSE;

		// modify for 此处不能返回，要之后后面那句再返回
		// CHECK_POINTER_VOID(m_pVideoSoundCtrl);
		// m_pVideoSoundCtrl->SetSoundSilencePic(FALSE);

		if (m_pVideoSoundCtrl)
		{
			m_pVideoSoundCtrl->SetSoundSilencePic(FALSE);
		}

		//声音
		SetButtonPicture(m_buttonPlaySound,"PlayNoSound");
		UpdateView();


// 		CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
// 		SoundMgr.SetSoundingPanel(NULL);

        // 关闭声音
        if(IVS_SUCCEED != pCtrl->StopSound(ulWinID))
        {
            return;
        }
        //pVideoPaneMgr->NotifyOtherSoundOpen(ulWinID, false);
    }
}

std::bitset<TOOLBARBTNNUM> CTranVideoRealtimeBar::GetBitsetButtonVisableStatus() const
{
    std::bitset<TOOLBARBTNNUM> BtnVisableStatus(m_ullToolBarBtnVisableStatus);
    return BtnVisableStatus;
}

void CTranVideoRealtimeBar::DeleteTalkbackCtrl()
{
	if (m_bTalkbackSoundCreated)
	{
		m_bTalkbackSoundCreated = false;
		IVS_DELETE(m_pVideoTalkbackSound);
	}
    //m_bTalkbackCtrlState = FALSE;
}

void CTranVideoRealtimeBar::EnableALLButton( BOOL isEnable )
{
	std::vector<CTranslucentButton *>::const_iterator it;
	for (it = m_btnVector.begin(); it != m_btnVector.end(); ++it)
	{
		CTranslucentWnd* translucentWnd = *it;
		translucentWnd->EnableWindow(isEnable);
	}
	//保存按钮状态
	m_bIsButtonEnable = isEnable;
	UpdateView();
}

IVS_INT32 CTranVideoRealtimeBar::StopTalkbackSound()
{
	CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
	CHECK_POINTER(pVideoPane, IVS_FAIL);
	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER(pVideoPaneMgr, IVS_FAIL);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER(pCtrl, IVS_FAIL);
// 	if (BOTTOMBAR_TALKBACK_CLOSE == pVideoPane->GetRealtimeTalkbackState())
// 	{
// 		(void)pVideoPane->GetOCXPlayerCtrl()->StopTalkback(pVideoPane->GetPaneID());
// 	}
	IVS_INT32 iRet = IVS_FAIL;
	if (BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
		// 开启声音按钮
		m_buttonPlaySound.EnableWindow(TRUE);
		// 关闭对讲
		iRet = (IVS_INT32)pCtrl->StopTalkback(pVideoPane->GetPaneID());
		if(IVS_SUCCEED != iRet)
		{
			return iRet;
		}

		pVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_CLOSE);
		//pVideoPane->IsSoundOn(false);
		SetTalkbackCtrlState(false);
		m_bTalkbackSoundState = TRUE;

		// modify for 此处不能返回，要之后后面那句再返回
		// CHECK_POINTER_VOID(m_pVideoSoundCtrl);
		// m_pVideoSoundCtrl->SetSoundSilencePic(FALSE);

		if (m_pVideoTalkbackSound)
		{
			m_pVideoTalkbackSound->SetTalkbackSilencePic(FALSE);
		}

		//声音
		//SetButtonPicture(m_buttonTalkback,"NoTalkBack");

        // 关闭其他窗口的语音
        pVideoPaneMgr->StopSoundCtrlForOther(true);
		UpdateView();


// 		CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
// 		SoundMgr.SetSoundingPanel(NULL);
	}



	return IVS_SUCCEED;
}

IVS_INT32 CTranVideoRealtimeBar::StartTalkbackSound()
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;
	CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
	CHECK_POINTER(pVideoPane, IVS_FAIL);
	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER(pVideoPaneMgr, IVS_FAIL);
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER(pCtrl,IVS_FAIL);
	// 开启语音对讲 先关闭其他窗口的语音对讲
	pVideoPaneMgr->StopTalkbackSoundForOther();
    // 关闭其他窗口的语音
    pVideoPaneMgr->StopSoundCtrlForOther(true);
	if (BOTTOMBAR_TALKBACK_CLOSE == pVideoPane->GetRealtimeTalkbackState())
	{
		// 开启对讲，如果失败，直接不做后面的任何动作
		iRet = (IVS_INT32)pCtrl->StartTalkback(pVideoPane->GetPaneID());
		if (IVS_SUCCEED != iRet)
		{
			return iRet;
		}
		// 屏蔽随路语音按钮
		this->StopSound();
		m_buttonPlaySound.EnableWindow(FALSE);

		pVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_OPEN);
		//pVideoPane->IsSoundOn(true);
		SetTalkbackCtrlState(true);
		m_bTalkbackSoundState = FALSE;
		CHECK_POINTER(m_pVideoTalkbackSound, IVS_FAIL);
		m_pVideoTalkbackSound->SetTalkbackSilencePic(TRUE);
		//声音
		//SetButtonPicture(m_buttonTalkback,    "TalkBack");
		UpdateView();
//      全局同步
// 		CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
// 		SoundMgr.SetSoundingPanel(pVideoPane);
	}	
	return IVS_SUCCEED;
}

void CTranVideoRealtimeBar::ResetBottomButton()
{
	m_isExtend = false;
	m_iNumBtn = MAXNUM_SHOW_BUTTON;
	DrawButton();
	//扩展
	UnregisterTranslucentWnd(&m_buttonLocker);
	SetButtonPicture(m_buttonLocker,		_T("Extend"));
	//声音
	//SetButtonPicture(m_buttonPlaySound,    _T("PlayNoSound"));
	//对讲
	//SetButtonPicture(m_buttonTalkback,	   _T("TalkBack"));
	UpdateView();
}

void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimePtz3d()
{
    // TODO: Add your control notification handler code here
    SetCurrentPaneToActivePane();
    CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
    CHECK_POINTER_VOID(pVideoPane);
    CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
    CHECK_POINTER_VOID(pCtrl);
    pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_PTZ3D, pVideoPane->GetPaneID());

    // 设置3D云镜
    //判断是否正在播放，没在播放则不能设置3D状态
    if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_ISNOT_PLAYING_ERROR,"", "Video Pane Manager set 3D PTZ error,pane is not playing video.");
        return;
    }
    ULONG ulIsStart3D = pVideoPane->Get3DPTZState();
    if (PTZ_3D_CLOSE == ulIsStart3D) //lint !e737 无问题
    {
        ulIsStart3D = PTZ_3D_OPEN;
        SetPtz3DBtnState(true);
    }
    else if (PTZ_3D_OPEN == ulIsStart3D) //lint !e737 无问题
    {
        ulIsStart3D = PTZ_3D_CLOSE;
        SetPtz3DBtnState(false);
    }
    //设置窗格的工具栏的设备名称
    pVideoPane->Set3DPTZState(ulIsStart3D);
}


void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeReturn()
{
	// TODO: Add your control notification handler code here
	SetCurrentPaneToActivePane();
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	//上抛一个返回按钮点击的事件
	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER_VOID(pCtrl);
	pCtrl->PostToolBarClickEventMsg(2,IVS_TOOLBAR_EXIT_PLAY_RECORD, pVideoPane->GetPaneID());
	//恢复实况
	pVideoPane->ResumeRealPlay();

	// 轮训按钮
	if (TYPE_WND_TURNCTUISE == pVideoPane->GetPrveTypeWnd())
	{
		pVideoPane->SetTypeWnd(pVideoPane->GetPrveTypeWnd());
		pVideoPane->SetPrveTypeWnd(TYPE_WND_NOMAL);
		pVideoPane->SetTurnCruiseStyle(pVideoPane->GetPrveTurnCruiseStyle());
		pVideoPane->SetPrveTurnCruiseStyle(0);
		pVideoPane->ResetBarPosition();
	}
}


void CTranVideoRealtimeBar::OnBnClickedBtnToolRealtimeStart()
{
	// TODO: Add your control notification handler code here
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	//已经在播放了就停止设置为待播放状态
	if (m_bIsStartPlay)
	{
		//停止播放，成功返回0
		if (0 != pVideoPane->PauseRecordPlay())
		{
			return;
		}
		SetRecordPlayStop(TRUE);
		m_bIsStartPlay = FALSE;
		//SetIsStartPlay(FALSE);
		////停止播放
		//m_bIsStartPlay = FALSE;
		////设置为待播放按钮图片
		//SetButtonPicture(m_btnStart,	  _T("StartBack"));
	}
	else
	{
		//开始播放,成功返回0
        pVideoPane->SetStartPlayTime();
		if (0 != pVideoPane->StartRecordPlay())
		{
			return;
		}
		SetRecordPlayStop(FALSE);
		m_bIsStartPlay = TRUE;
		//SetIsStartPlay(TRUE);
		////开始播放
		//m_bIsStartPlay = TRUE;
		////设置为待停止按钮图片
		//SetButtonPicture(m_btnStart,	  _T("StopBack"));
	}

	//SetIsStartPlay(m_bIsStartPlay);

	UpdateView();
}

void CTranVideoRealtimeBar::SetIsStartPlay(BOOL val)
{
	if (val)
	{
		SetButtonPicture(m_btnStart,	  _T("StartBack"));
	}
	else
	{
		SetButtonPicture(m_btnStart,	  _T("StopBack"));
	}
	m_bIsStartPlay = !val;
	UpdateView();
}

void CTranVideoRealtimeBar::SetRecordPlayStop(BOOL val)
{
	if (val)
	{
		SetButtonPicture(m_btnStart,	  _T("StartBack"));
		m_bIsStartPlay = FALSE;
	}
	else
	{
		SetButtonPicture(m_btnStart,	  _T("StopBack"));
		m_bIsStartPlay = TRUE;
	}

	UpdateView();
}

LRESULT CTranVideoRealtimeBar::OnSetSoundState( WPARAM wParam, LPARAM lParam )
{
    IVS_DEBUG_TRACE("");

    CVideoPane* pVideoPane = dynamic_cast <CVideoPane*>(m_pVideoPane);
    CHECK_POINTER(pVideoPane, 0);

    CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
    CHECK_POINTER(pVideoPaneMgr, 0);

    if ( 0 == lParam )
    {
        // 如果有语音对讲，则不开启随路语音
        if(BOTTOMBAR_TALKBACK_OPEN == pVideoPaneMgr->IsTalkback())
        {
            //BP_RUN_LOG_ERR(IVS_OCX_RET_VOICE_TALKBACK_HAS_PLAYED_ERROR,"","can't open realtime sound, when talkback is open");
            return 0;
        }

        if (BOTTOMBAR_SOUND_CLOSE == pVideoPane->GetRealtimeSoundState())
        {
            pVideoPane->SetRealtimeSoundState(BOTTOMBAR_SOUND_OPEN);
            pVideoPane->IsSoundOn(true);
            m_bSoundState = TRUE;

            if (m_pVideoSoundCtrl)
            {
                m_pVideoSoundCtrl->SetSoundSilencePic(TRUE);
            }

            //声音
            SetButtonPicture(m_buttonPlaySound,    "PlaySound");
            UpdateView();
        }
    }
    else if ( 1 == lParam )
    {
        if (BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
        {
            pVideoPane->SetRealtimeSoundState(BOTTOMBAR_SOUND_CLOSE);
            pVideoPane->IsSoundOn(false);
            m_bSoundState = FALSE;

            if (m_pVideoSoundCtrl)
            {
                m_pVideoSoundCtrl->SetSoundSilencePic(FALSE);
            }

            //声音
            SetButtonPicture(m_buttonPlaySound,"PlayNoSound");
            UpdateView();
        }
    }
    
    return 0;
}//lint !e715


void CTranVideoRealtimeBar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (VK_RETURN == nChar)
	{
		return;
	}

	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	CVideoPaneMgr * pVideoPaneMgr = pVideoPane->GetVideoPaneMgr();
	CHECK_POINTER_VOID(pVideoPaneMgr);

	if (pVideoPaneMgr->GetActivePaneID()!=pVideoPane->GetPaneID())
	{
		return;
	}

	CWnd *pWnd = pVideoPaneMgr->GetParentWnd();
	CHECK_POINTER_VOID(pWnd);

	::PostMessage(pWnd->GetSafeHwnd(),WM_KEYUP,static_cast<LONG>(nChar),0);//lint !e732
	//CTranslucentDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}//lint !e715
