// DitalPtzDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "IVS_OCXPlayer.h"
#include "DitalPtzDlg.h"
#include "afxdialogex.h"
#include "IVS_OCXPlayerCtrl.h"
#include "OCXPtzControlXMLProcess.h"
// CDitalPtzDlg dialog

IMPLEMENT_DYNAMIC(CDitalPtzDlg, CTranslucentDialog)

CDitalPtzDlg::CDitalPtzDlg(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/)
	: CTranslucentDialog(CDitalPtzDlg::IDD, lpszFile,  pParent)
	, m_uiLockStatus(0)
{

}

CDitalPtzDlg::~CDitalPtzDlg()
{
}

void CDitalPtzDlg::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_BTN_TOP_LEFT, m_btnTopLeft);
	DDX_Control(pDX, IDC_BTN_TOP, m_btnTop);
	DDX_Control(pDX, IDC_BTN_TOP_RIGHT, m_btnTopRight);
	DDX_Control(pDX, IDC_BTN_RIGHT, m_btnRight);
	DDX_Control(pDX, IDC_BTN_BOTTOM_RIGTH, m_btnBottomRight);
	DDX_Control(pDX, IDC_BTN_BOTTOM, m_btnBottom);
	DDX_Control(pDX, IDC_BTN_BOTTOM_LEFT, m_btnBottomLeft);
}

// TODO 这里的事件之后需要改成DOWN和UP，并启用定时器发送消息
BEGIN_MESSAGE_MAP(CDitalPtzDlg, CTranslucentDialog)
	ON_WM_PAINT()
	ON_MESSAGE(WM_TRANS_BTN_DOWN, &CDitalPtzDlg::OnDitalPTZBnDown)
	ON_MESSAGE(WM_TRANS_BTN_UP, &CDitalPtzDlg::OnDitalPTZBnUp)
END_MESSAGE_MAP()


// CDitalPtzDlg message handlers

BOOL CDitalPtzDlg::OnInitDialog()
{
	CTranslucentDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDitalPtzDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CTranslucentDialog::OnPaint() for painting messages
	//TODO 去掉上下两条的宽度和计算不是全屏时的放大
	CRect rect;
	GetClientRect(rect);

	int iWidth = rect.Width();
	int iHeight = rect.Height();

	m_btnLeft.MoveWindow(rect.left, rect.top+iHeight/2-8, 16, 16);
	m_btnLeft.ShowWindow(SW_SHOW);

	m_btnTopLeft.MoveWindow(rect.left, rect.top, 16, 16);
	m_btnTopLeft.ShowWindow(SW_SHOW);

	m_btnTop.MoveWindow(rect.left+iWidth/2-8, rect.top, 16, 16);
	m_btnTop.ShowWindow(SW_SHOW);

	m_btnTopRight.MoveWindow(rect.right-16, rect.top, 16, 16);
	m_btnTopRight.ShowWindow(SW_SHOW);

	m_btnRight.MoveWindow(rect.right-16, rect.top+iHeight/2-8, 16, 16);
	m_btnRight.ShowWindow(SW_SHOW);

	m_btnBottomRight.MoveWindow(rect.right-16, rect.bottom-16, 16, 16);
	m_btnBottomRight.ShowWindow(SW_SHOW);

	m_btnBottom.MoveWindow(rect.left+iWidth/2-8, rect.bottom-16, 16, 16);
	m_btnBottom.ShowWindow(SW_SHOW);

	m_btnBottomLeft.MoveWindow(rect.left, rect.bottom-16, 16, 16);
	m_btnBottomLeft.ShowWindow(SW_SHOW);

	UpdateView();
}

void CDitalPtzDlg::OnInitChildrenWnds()
{
	SetButtonPicture(m_btnLeft, "Left");
	SetButtonPicture(m_btnTopLeft, "TopLeft");
	SetButtonPicture(m_btnTop, "Top");
	SetButtonPicture(m_btnTopRight, "TopRight");
	SetButtonPicture(m_btnRight, "Right");
	SetButtonPicture(m_btnBottomRight, "BottomRight");
	SetButtonPicture(m_btnBottom, "Bottom");
	SetButtonPicture(m_btnBottomLeft, "BottomLeft");
}

void CDitalPtzDlg::SetButtonPicture( CTranslucentButton &tButton,const std::string &strButtonName )
{
	CHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]={0x0};

	memset(pcImageList,0x0,256*5);
	if (strButtonName.empty())
	{
		return;
	}
	(void)SkinMgr.DigtalPtzGetPNGButtonByName(strButtonName,pcImageList);
	(void)tButton.LoadImageList(pcImageList);
	RegisterTranslucentWnd(&tButton);
}

void CDitalPtzDlg::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}

LRESULT CDitalPtzDlg::OnDitalPTZBnDown(WPARAM wParam, LPARAM lParam)
{
	SetCurrentPaneToActivePane();
	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER(pVideoPane, 0);

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER(pCtrl, 0);
	//没有在实况，事件不触发
	if (VIDEO_PLAY_STATE_REALTIME != pVideoPane->GetPlayState())
	{
		return 0;
	}

	// 默认云台速度
	int iSpeed = 0; 
	// 获取步长，从本地文件中读取步长
	BSTR b = pCtrl->IVS_OCX_GetLocalPTZConfig();
	char* pSpeedXML = _com_util::ConvertBSTRToString(b);
	(void)GetDitalPTZSpeed(pSpeedXML, iSpeed);
	::SysFreeString(b);
	if (NULL != pSpeedXML)
	{
		delete[] pSpeedXML;
	}

	LONG lOperateCode = 0;
	// 发送开始云镜命令
	if ((HWND)wParam == m_btnLeft.GetSafeHwnd())
	{
		lOperateCode = PTZ_LEFT;
	}
	else if ((HWND)wParam == m_btnTopLeft.GetSafeHwnd())
	{
		lOperateCode = PTZ_UP_LEFT;
	}
	else if ((HWND)wParam == m_btnTop.GetSafeHwnd())
	{
		lOperateCode = PTZ_UP;
	}
	else if ((HWND)wParam == m_btnTopRight.GetSafeHwnd())
	{
		lOperateCode = PTZ_UP_RIGHT;
	}
	else if ((HWND)wParam == m_btnRight.GetSafeHwnd())
	{
		lOperateCode = PTZ_RIGHT;
	}
	else if ((HWND)wParam == m_btnBottomRight.GetSafeHwnd())
	{
		lOperateCode = PTZ_DOWN_RIGHT;
	}
	else if ((HWND)wParam == m_btnBottom.GetSafeHwnd())
	{
		lOperateCode = PTZ_DOWN;
	}
	else if ((HWND)wParam == m_btnBottomLeft.GetSafeHwnd())
	{
		lOperateCode = PTZ_DOWN_LEFT;
	}
	else
	{
		return 0;
	}

	IVS_UINT32 uiLockStatus = 0;
	IVS_INT32 iRet = IVS_SDK_PtzControl(pCtrl->GetSDKSessionID(),pVideoPane->GetCameraID(), lOperateCode,  "2", CToolsHelp::Int2Str(iSpeed).c_str(), &uiLockStatus);

	// 如果不成功，需要抛消息给CU
	if (IVS_SUCCEED != iRet)
	{
		CXml xml;
		(void)xml.AddDeclaration("1.0","UTF-8","");
		(void)xml.AddElem("Content");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("ResultCode");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
		(void)xml.AddElem("LockStatus");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiLockStatus).c_str());

		unsigned int len = 0;
		const char* pXml = xml.GetXMLStream(len);
		CHECK_POINTER(pXml, 0);
		pCtrl->IVS_OCX_Windows_Event(IVS_EVENT_PTZ_LOCK, pXml);

		// 保存操作
		m_uiLockStatus = uiLockStatus;
	}

	return 0;
}//lint !e715

LRESULT CDitalPtzDlg::OnDitalPTZBnUp(WPARAM wParam, LPARAM lParam)
{
	SetCurrentPaneToActivePane();
	// 如果是被锁定状态，就不再进行下面操作了
	if (m_uiLockStatus != 0)
	{
		BP_RUN_LOG_INF("ptz locked","NA");
		return 0;
	}

	CVideoPane *pVideoPane = dynamic_cast<CVideoPane *>(m_pVideoPane);
	CHECK_POINTER(pVideoPane, 0);

	CIVS_OCXPlayerCtrl *pCtrl = pVideoPane->GetOCXPlayerCtrl();
	CHECK_POINTER(pCtrl, 0);

	//没有在实况，事件不触发
	if (VIDEO_PLAY_STATE_REALTIME != pVideoPane->GetPlayState())
	{
		return 0;
	}
	// 发送开始云镜停止命令
	if ((HWND)wParam == m_btnLeft.GetSafeHwnd()
		|| (HWND)wParam == m_btnTopLeft.GetSafeHwnd()
		|| (HWND)wParam == m_btnTop.GetSafeHwnd()
		|| (HWND)wParam == m_btnTopRight.GetSafeHwnd()
		|| (HWND)wParam == m_btnRight.GetSafeHwnd()
		|| (HWND)wParam == m_btnBottomRight.GetSafeHwnd()
		|| (HWND)wParam == m_btnBottom.GetSafeHwnd()
		|| (HWND)wParam == m_btnBottomLeft.GetSafeHwnd())
	{
		IVS_UINT32 uiLockStatus = 0;
		IVS_INT32 iRet = IVS_SDK_PtzControl(pCtrl->GetSDKSessionID(),pVideoPane->GetCameraID(), PTZ_STOP,  "", "", &uiLockStatus);

		// 如果不成功，需要抛消息给CU
		if (IVS_SUCCEED != iRet)
		{
			CXml xml;
			(void)xml.AddDeclaration("1.0","UTF-8","");
			(void)xml.AddElem("Content");
			(void)xml.IntoElem();
			(void)xml.AddChildElem("ResultCode");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
			(void)xml.AddElem("LockStatus");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiLockStatus).c_str());

			unsigned int len = 0;
			const char* pXml = xml.GetXMLStream(len);
			CHECK_POINTER(pXml, 0);
			pCtrl->IVS_OCX_Windows_Event(IVS_EVENT_PTZ_LOCK, pXml);
		}
	}

	return 0;
} //lint !e715

IVS_INT32 CDitalPtzDlg::GetDitalPTZSpeed(const char* pxml, int& iSpeed)const
{
	CHECK_POINTER(pxml, IVS_FAIL);
	int iRet  = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(pxml))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/PTZConfig/Speed"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CameraCode", "NA");
		return iRet;
	}
	const char* pSpeed = xml.GetElemValue();
	CHECK_POINTER(pSpeed, IVS_XML_INVALID);
	iSpeed = atoi(pSpeed);

	// 如果这个超过正常范围
	if (iSpeed < 1 || iSpeed > 10)
	{
		iSpeed = 5;
	}
	return IVS_SUCCEED;
}


void CDitalPtzDlg::SetBarPictrue(LPCTSTR lpszFile)
{
	TCHAR wsFile[MAX_PATH] = {0};
	if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)lpszFile,strnlen(lpszFile,MAX_PATH)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("Translucent Dialog Failed"));
	}
	WCHAR* pcharFileName = CTranslucentUtility::ttowc(wsFile);
	Gdiplus::Image* pImage = Gdiplus::Image::FromFile(pcharFileName);
	if (pImage==NULL)
	{
		if (NULL != pcharFileName)
		{
			delete []pcharFileName;
			pcharFileName = NULL;
		}
		return;
	}

	SetBackGroudPictrue(pImage);
	OnInitChildrenWnds();

	if (NULL != pcharFileName)
	{
		delete []pcharFileName;
		pcharFileName = NULL;
	}
}
