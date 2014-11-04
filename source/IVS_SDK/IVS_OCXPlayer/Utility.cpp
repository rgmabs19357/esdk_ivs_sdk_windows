#include "StdAfx.h"
#include "ToolsHelp.h"

CVideoPaneMgr* CUtility::pVideoPaneMgr = nullptr;

bool CUtility::CheckIsWin7( void )
{
    OSVERSIONINFOEX stOsVersionInfo = {0};
    stOsVersionInfo.dwOSVersionInfoSize = sizeof(stOsVersionInfo);

    BOOL bResult = GetVersionEx((LPOSVERSIONINFO)&stOsVersionInfo);
    if (!bResult)
    {
        //获得失败，则使用win7配置，刷新页面
        BP_RUN_LOG_ERR(IVS_FAIL,"Get OS Version Failed.","");
        return true;
    }

    if (stOsVersionInfo.dwMajorVersion >= 6 && stOsVersionInfo.dwMinorVersion >= 1)
    {
        return true;
    }
    return false;
}

// bool CUtility::ImageFromIDResource( UINT nID, LPCTSTR sTR,Image * &pImg )
// {
//     HINSTANCE hInst = AfxGetResourceHandle();
//     HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
//     if (!hRsrc)
//         return false;
// 
//     // load resource into memory
//     DWORD len = SizeofResource(hInst, hRsrc);
//     BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
//     if (!lpRsrc)
//         return false;
// 
//     // Allocate global memory on which to create stream
//     HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
//     BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
//     // _memcpy(pmem,lpRsrc,len);
// 	if(!CToolsHelp::Memcpy(pmem,len,lpRsrc,len))
// 	{
//         BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS_OCX::"," Utility Image Resource.Failed");
// 		return false;
// 	};
//     IStream* pstm;
//     CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
// 
//     // load from stream
//     pImg=Gdiplus::Image::FromStream(pstm);
// 
//     // free/release stuff
//     GlobalUnlock(m_hMem);
//     pstm->Release();
//     FreeResource(lpRsrc);
// 
// 	return true;
// }

void CUtility::SetVideoPaneMgr( CVideoPaneMgr* val )
{
    if (nullptr != val)
    {
         pVideoPaneMgr = val;
    }
}

CVideoPaneMgr* CUtility::GetVideoPaneMgr()
{
    return pVideoPaneMgr;
}
