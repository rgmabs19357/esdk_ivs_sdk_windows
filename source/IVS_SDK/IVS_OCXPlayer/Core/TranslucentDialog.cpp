// TranslucentDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "TranslucentDialog.h"
#include <algorithm>
#include "ToolsHelp.h"
#include "I18N.h"

// CTranslucentDialog 对话框
//lint -e831 831错误不需要清楚

IMPLEMENT_DYNAMIC(CTranslucentDialog,CDialog)

CTranslucentDialog::CTranslucentDialog(UINT nIDTemplate, LPCTSTR lpszFile, CWnd* pParent )
    : CDialog(nIDTemplate, pParent)
	, m_pImage(nullptr)
	, m_bEnableDrag(false)
    , m_bCenterAligned(true)
    , m_bBuffered(false)
	, m_pVideoPane(nullptr)
    , m_IsCreate(false)
{
    m_translucentWndList.clear();
    TCHAR wsFile[MAX_PATH] = {0};
    if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)lpszFile,strnlen(lpszFile,MAX_PATH)))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,_T("IVS_OCX::"),_T("Translucent Dialog Failed"));
    }
	WCHAR* pcharFileName = CTranslucentUtility::ttowc(wsFile);
    m_pImage = Gdiplus::Image::FromFile(pcharFileName);

	VERIFY( CTranslucentUtility::IsFileExist(lpszFile) );
    VERIFY( m_pImage != NULL );

	memset(&m_blend,0x0,sizeof(BLENDFUNCTION));

	if(pcharFileName)
	{
		delete []pcharFileName;
		pcharFileName = NULL;
	}
	mUpdateLayeredWindow = nullptr;

	m_pMutexWndLock = VOS_CreateMutex();
}

CTranslucentDialog::CTranslucentDialog(UINT nIDTemplate, UINT nImgID, LPCTSTR lpszType, HINSTANCE hResourceModule, CWnd* pParent )
    : CDialog(nIDTemplate, pParent)
    , m_pImage(CTranslucentUtility::LoadImage(nImgID, lpszType, hResourceModule))
    , m_bEnableDrag(false)
    , m_bCenterAligned(true)
	, m_bBuffered(false)
	, m_pVideoPane(nullptr)
    , m_IsCreate(false)
{
    m_translucentWndList.clear();
    VERIFY( m_pImage != NULL );
	memset(&m_blend,0x0,sizeof(BLENDFUNCTION));
	mUpdateLayeredWindow = nullptr;
	m_pMutexWndLock = VOS_CreateMutex();
}

CTranslucentDialog::CTranslucentDialog(UINT nIDTemplate, Gdiplus::Image* pImage, CWnd* pParent)
	: CDialog(nIDTemplate, pParent)
	, m_pImage(nullptr)
	, m_bEnableDrag(false)
	, m_bCenterAligned(true)
	, m_bBuffered(false)
	, m_pVideoPane(nullptr)
	, m_IsCreate(false)
{
	m_translucentWndList.clear();
	m_pImage = pImage;
	memset(&m_blend,0x0,sizeof(BLENDFUNCTION));
	mUpdateLayeredWindow = nullptr;
	m_pMutexWndLock = VOS_CreateMutex();
}

CTranslucentDialog::~CTranslucentDialog()
{
	//Gdiplus::DllExports::GdipFree(m_pImage);
	//m_pImage = nullptr;
	try
	{
		//if(m_pImage)
		//{
		//	Gdiplus::DllExports::GdipFree(m_pImage);
		//}
		m_pVideoPane = NULL;
		m_pImage = NULL;
		VOS_DestroyMutex(m_pMutexWndLock);
		m_pMutexWndLock = NULL;
	}
	catch(...)
	{ //lint !e1775
		//BP_RUN_LOG_ERR(IVS_SDK_RET_INTRINSIC_PTR_ERROR,_T("Delete Translucent Dialog Failed."),_T("Reason:Delete m_pImage Failed."));
	}

} //lint !e1740 !e1504  m_pImage GDI 释放

BEGIN_MESSAGE_MAP(CTranslucentDialog, CDialog)
    //}}AFX_MSG_MAP
    ON_WM_ERASEBKGND()
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_LBUTTONDOWN()
    //	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void CTranslucentDialog::SetEnableDrag(bool bEnableDrag)
{
    m_bEnableDrag = bEnableDrag;
}

void CTranslucentDialog::SetCenterAligned(bool bCenterAligned)
{
    m_bCenterAligned = bCenterAligned;
}

void CTranslucentDialog::RegisterTranslucentWnd(CTranslucentWnd* translucentWnd)
{
    translucentWnd->SetRenderListener(this);
	(void)VOS_MutexLock(m_pMutexWndLock);
    m_translucentWndList.push_back(translucentWnd);
	(void)VOS_MutexUnlock(m_pMutexWndLock);
}

void CTranslucentDialog::UnregisterTranslucentWnd(CTranslucentWnd* translucentWnd)
{
    translucentWnd->SetRenderListener(NULL);
	(void)VOS_MutexLock(m_pMutexWndLock);
    TranslucentWndList::iterator it = std::find(m_translucentWndList.begin(), m_translucentWndList.end(), translucentWnd);
    if (it != m_translucentWndList.end())
    {
        (void)m_translucentWndList.erase(it);
    }
	(void)VOS_MutexUnlock(m_pMutexWndLock);
}

BOOL CTranslucentDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

	HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));  
    if (NULL == hFuncInst)
    {
        return FALSE;
    }

	mUpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
    if (NULL == mUpdateLayeredWindow)
    {
        return FALSE;
    }
    //RECT rc;
    //GetWindowRect(&rc);

    // must be WS_POPUP
    DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);   //lint !e732  MFC自动生成的的函数
    VERIFY((dwStyle & WS_POPUP) != 0);
    VERIFY((dwStyle & WS_CHILD) == 0);

    m_blend.BlendOp = 0;
    m_blend.BlendFlags = 0;
    m_blend.AlphaFormat = 1;
    m_blend.SourceConstantAlpha = 255;//AC_SRC_ALPHA

    OnInitChildrenWnds();
    m_IsCreate = true;

    UpdateView();

    return TRUE;
}

BOOL CTranslucentDialog::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
} //lint !e818  !e1762 !e715 MFC自动生成的的函数

void CTranslucentDialog::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);
}

void CTranslucentDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    m_WindowSize.Width = cx;
    m_WindowSize.Height = cy;

    if (m_IsCreate)
    {
        UpdateView();
    }
}

int CTranslucentDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    // Must be overlapped
    VERIFY((lpCreateStruct->style & WS_POPUP) == 0); //lint !e737  MFC自动生成的的函数
    VERIFY((lpCreateStruct->style & WS_CHILD) == 0); //lint !e737  MFC自动生成的的函数

    int nX = lpCreateStruct->x;
    int nY = lpCreateStruct->y;
    int nWidth = lpCreateStruct->cx;
    int nHeight = lpCreateStruct->cy;
    if (m_bCenterAligned)
    {
        RECT rcWindow = { 0, 0, static_cast<long>(m_pImage->GetWidth()), static_cast<long>(m_pImage->GetHeight())};
        (void)::AdjustWindowRect(&rcWindow, (DWORD)GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
        nWidth = rcWindow.right - rcWindow.left;
        nHeight = rcWindow.bottom - rcWindow.top;
        (void)::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWindow, 0);
        nX = rcWindow.left + ((rcWindow.right - rcWindow.left) - nWidth) / 2;
        nY = rcWindow.top + ((rcWindow.bottom - rcWindow.top) - nHeight) / 2;
    }

    nWidth = static_cast<int>(m_pImage->GetWidth());
    nHeight = static_cast<int>(m_pImage->GetHeight());

    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    (void)ModifyStyle(WS_CAPTION, WS_POPUP, 0);
    MoveWindow(nX, nY, nWidth, nHeight, TRUE);

    return 0;
}

//lint -e831 831错误不需要清楚
void CTranslucentDialog::UpdateView()
{
    SIZE sizeWindow = { m_WindowSize.Width, m_WindowSize.Height};

    if (sizeWindow.cx <= 0 || sizeWindow.cy <=0)
    {
        return;
    }

    DWORD dwExStyle = static_cast<unsigned long >(::GetWindowLong(m_hWnd, GWL_EXSTYLE));
    if ((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
        (void)::SetWindowLong(m_hWnd, GWL_EXSTYLE, static_cast<long>(dwExStyle | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE));


    HDC hDC = ::GetDC(m_hWnd);
	CHECK_POINTER_VOID(hDC);
    HDC hdcMemory = CreateCompatibleDC(hDC);
	if(NULL==hdcMemory)
	{
		::ReleaseDC(m_hWnd, hDC);
		return;
	}
    HBITMAP hBitMap = CreateCompatibleBitmap(hDC, m_WindowSize.Width, m_WindowSize.Height);		  
	if(NULL==hBitMap)
	{
		::DeleteDC(hdcMemory);
		::ReleaseDC(m_hWnd, hDC);
		return;
	}
	::SelectObject(hdcMemory, hBitMap);
    RECT rcWindow;
    memset(&rcWindow, 0, sizeof(rcWindow));
    GetWindowRect(&rcWindow);
	BITMAPINFO bitmapInfo = { 0 };
    BITMAPINFOHEADER &stBmpInfoHeader = bitmapInfo.bmiHeader;   
    int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;  //lint !e704  MFC自动生成的的函数
    stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
    stBmpInfoHeader.biWidth = sizeWindow.cx;   
    stBmpInfoHeader.biHeight = sizeWindow.cy;   
    stBmpInfoHeader.biPlanes = 1;   
    stBmpInfoHeader.biBitCount = 32;   
    stBmpInfoHeader.biCompression = BI_RGB;   
    stBmpInfoHeader.biClrUsed = 0;   
    stBmpInfoHeader.biSizeImage = static_cast<unsigned long>(nBytesPerLine * sizeWindow.cy);
    PVOID pvBits = NULL;   
    HBITMAP hbmpMem = ::CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, &pvBits, NULL, 0); //lint !e826

    ASSERT(hbmpMem != NULL);//lint !e831
	if(NULL!=hbmpMem && NULL!=pvBits)//lint !e774 !e831
	{
		memset( pvBits, 0, static_cast<unsigned int >(sizeWindow.cx * 4 * sizeWindow.cy));
		HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

		//POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
		POINT ptWinPos = { rcWindow.left, rcWindow.top };
		Gdiplus::Graphics graph(hdcMemory);
		(void)graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

		Gdiplus::Rect rcDes(0, 0, m_WindowSize.Width, m_WindowSize.Height);

		Gdiplus::ImageAttributes ImgAtt;
		(void)ImgAtt.SetWrapMode(WrapModeTileFlipXY);
		(void)graph.DrawImage(m_pImage, 
			rcDes,  //lint !e64
			0, 
			0, 
			static_cast<int>(m_pImage->GetWidth()), 
			static_cast<int>(m_pImage->GetHeight()),
			UnitPixel, 
			&ImgAtt); //lint !e1025 !e119

		(void)VOS_MutexLock(m_pMutexWndLock);
		TranslucentWndList::const_iterator it;
		for (it = m_translucentWndList.begin(); it != m_translucentWndList.end(); ++it)
		{
			CTranslucentWnd* translucentWnd = *it;
			if (translucentWnd->IsVisible())
				translucentWnd->Render(graph);
		}
		(void)VOS_MutexUnlock(m_pMutexWndLock);

		POINT ptSrc = { 0, 0};
		(void)mUpdateLayeredWindow(m_hWnd, hDC, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &m_blend, 2);
		graph.ReleaseHDC(hdcMemory);
		::SelectObject( hdcMemory, hbmpOld);
		::DeleteObject(hbmpMem); 
	}
	else
	{
		DWORD errCode = ::GetLastError();
		BP_DBG_LOG("CTranslucentDialog::UpdateView CreateDIBSection failed, cx=%d, cy=%d, errCode=%lu.", 
					sizeWindow.cx, sizeWindow.cy, errCode);
	}
	::DeleteObject(hBitMap);
    ::DeleteDC(hdcMemory);
	::ReleaseDC(m_hWnd, hDC);
}

void CTranslucentDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_bEnableDrag)
    {
        ::SendMessage( GetSafeHwnd(), WM_SYSCOMMAND, 0xF012, 0);
    }

    CDialog::OnLButtonDown(nFlags, point);
}

void CTranslucentDialog::OnRenderEvent(CTranslucentWnd* translucentWnd)
{
   // if (!m_bBuffered)
        UpdateView();
}//lint !e715

void CTranslucentDialog::StartBuffered()
{
    if (m_bBuffered)
        EndBuffered();

    m_bBuffered = true;
}

void CTranslucentDialog::EndBuffered()
{
    if (m_bBuffered)
    {
        UpdateView();
        m_bBuffered = false;
    }
}
void CTranslucentDialog::SetBackGroudPictrue(Gdiplus::Image* pImage)
{

	m_pImage = pImage;
	// VERIFY( m_pImage != NULL );
    if (NULL != m_pImage)
    {
        UpdateView();
    }
	
}

// 更新所有tip
void CTranslucentDialog::UpdateAllTipText()
{
	MAP_TIP_ITER iterBegin = m_TipMap.begin();
	MAP_TIP_ITER iterEnd = m_TipMap.end();

	for (; iterBegin != iterEnd; iterBegin++)
	{
		CTranslucentButton* button = dynamic_cast<CTranslucentButton*>(iterBegin->second);//lint !e611
		if (NULL == button)//lint !e611
		{
			continue;
		}

		// 根据名字 获取对应的资源文件中的信息
		std::string strToolTip;
		if (CI18N::GetInstance().GetResourceLanguage(iterBegin->first, strToolTip))
		{
			button->UpdateTipText(strToolTip);
		}
	}
}

