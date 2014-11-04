#include "stdafx.h"
#include "TranslucentUtility.h"
#include <ToolsHelp.h>

// 第三方开源lint不检查

CString CTranslucentUtility::GetModulePath(HMODULE hModule /* = NULL */)
{
	TCHAR pBuf[MAX_PATH] = {'\0'};
	CString strDir, strTemp;

	(void)::GetModuleFileName(hModule, pBuf, MAX_PATH);
	strTemp = pBuf;
	strDir = strTemp.Left(strTemp.ReverseFind('\\') + 1 );
	return strDir;
}

BOOL CTranslucentUtility::IsFileExist(LPCTSTR lpszFilePath)
{
	BOOL bExist = FALSE;
	HANDLE hHandle = NULL;

	hHandle = CreateFile(lpszFilePath
		, GENERIC_READ
		, FILE_SHARE_READ | FILE_SHARE_WRITE
		, NULL
		, OPEN_EXISTING
		, 0
		, 0
		);

	if( hHandle != INVALID_HANDLE_VALUE )
	{
		(void)CloseHandle(hHandle);
		bExist = TRUE;
	}

	return bExist;
}

BOOL CTranslucentUtility::ExtractResourceToFile( LPCTSTR lpszType
	, UINT nResID
	, LPCTSTR lpszFilename
	, HMODULE hModule)
{
    HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(nResID), lpszType);
    if (nullptr == hRes)
    {
        return FALSE;
    }

    DWORD dwSize = ::SizeofResource(hModule, hRes);
    if (0 == dwSize)
    {
        return FALSE;
    }

    HGLOBAL hGlobal = ::LoadResource(hModule, hRes);
    if (nullptr == hGlobal)
    {
        return FALSE;
    }

	LPVOID pBuffer = ::LockResource(hGlobal);
	if( pBuffer == NULL )
	{
		(void)::FreeResource(hGlobal);
		return FALSE;
	}

	HANDLE hHandle = ::CreateFile(lpszFilename
		, GENERIC_WRITE
		, FILE_SHARE_WRITE | FILE_SHARE_READ
		, NULL
		, CREATE_ALWAYS
		, 0
		, NULL
		);
	if(hHandle == NULL )
	{
		(void)::FreeResource(hGlobal);
		return FALSE;
	}

	DWORD dwWritten = 0;
	(void)::WriteFile(hHandle, pBuffer, dwSize, &dwWritten, NULL);
	if( dwWritten != dwSize )
	{
		(void)::CloseHandle(hHandle);
		(void)::FreeResource(hGlobal);
		return FALSE;
	}

	(void)::FlushFileBuffers(hHandle);
	(void)::CloseHandle(hHandle);
	(void)::FreeResource(hGlobal);

	return TRUE;
}// ExtractResourceToFile

Gdiplus::Image *CTranslucentUtility::LoadImage(UINT nID, LPCTSTR lpszType, HINSTANCE hInstance)
{
	Gdiplus::Image * pImage = nullptr;
    if (nullptr == lpszType)
    {
        return pImage;
    }

	if(lpszType == RT_BITMAP)
	{
		HBITMAP hBitmap = ::LoadBitmap( hInstance, MAKEINTRESOURCE(nID) );
		pImage = dynamic_cast<Gdiplus::Image*>(Gdiplus::Bitmap::FromHBITMAP(hBitmap, 0));
		(void)::DeleteObject(hBitmap);
		return pImage;
	}

	hInstance = (hInstance == NULL) ? ::AfxGetResourceHandle() : hInstance;
	HRSRC hRsrc = ::FindResource (hInstance, MAKEINTRESOURCE(nID), lpszType);
	ASSERT(hRsrc != NULL);

	DWORD dwSize = ::SizeofResource(hInstance, hRsrc);
	LPBYTE lpRsrc = (LPBYTE)::LoadResource(hInstance, hRsrc);
	ASSERT(lpRsrc != NULL);

	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
	LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
    if (NULL  == pMem)
    {
        return pImage;
    }

	memcpy( pMem, lpRsrc, dwSize);
	IStream * pStream = NULL;
	::CreateStreamOnHGlobal(hMem, FALSE, &pStream);

	pImage = Gdiplus::Image::FromStream(pStream);

	(void)::GlobalUnlock(hMem);
	pStream->Release();
	(void)::FreeResource(lpRsrc);

	return pImage;
}

HRGN CTranslucentUtility::CreateRegionFromBitmap(Bitmap* bitmap, BYTE alphaValve)
{
	if (bitmap == NULL)
	{
		return NULL;
	}
	UINT ulWidth = bitmap->GetWidth();
	UINT ulHeight = bitmap->GetHeight();
	Color color;
	HRGN hRegion = ::CreateRectRgn(0, 0, static_cast<int>(ulWidth), static_cast<int>(ulHeight));
	if (hRegion == NULL)
	{
		return NULL;
	}
	HRGN hRgn = ::CreateRectRgn(0, 0, static_cast<int>(ulWidth), static_cast<int>(ulHeight));
	if (hRgn == NULL)
	{
		return NULL;
	}
	for (UINT h = 0; h < ulWidth; ++h)
	{
		for (UINT w = 0; w < ulHeight; ++w)
		{
			UINT start = w;
			do
			{
				bitmap->GetPixel(static_cast<int>(w), static_cast<int>(h), &color);
				++w;
			} while ((w < ulWidth )&&(color.GetAlpha() <= alphaValve));

			if (w > start)
			{
				(void)::SetRectRgn(hRgn, static_cast<int>(start), static_cast<int>(h), static_cast<int>(w), static_cast<int>(h + 1));
				(void)::CombineRgn(hRegion, hRegion, hRgn, RGN_DIFF);
			}
		}
	}

	(void)::DeleteObject(hRgn);
	return hRegion;
}

HRGN CTranslucentUtility::CreateRegion(LPCTSTR lpszFile, BYTE alphaValve)
{
	if (!IsFileExist(lpszFile))
		return nullptr;

    TCHAR wsFile[MAX_PATH] = {0};
    if(!CToolsHelp::Strncpy(wsFile,MAX_PATH,(LPCTSTR)lpszFile,strnlen(lpszFile,MAX_PATH)))
    {
    	BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS_OCX::","Translucent Utility Create region.Failed");
        return nullptr;
    };
	WCHAR* wstr = CTranslucentUtility::ttowc(wsFile);
	Gdiplus::Bitmap bitmap(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	return CreateRegionFromBitmap(&bitmap);
}//lint !e715

HRGN CTranslucentUtility::CreateRegion(UINT nID, LPCTSTR lpszType, HINSTANCE hInstance, BYTE alphaValve)
{
	Gdiplus::Image *image = LoadImage(nID, lpszType, hInstance);
    if (nullptr == image)
    {
        return nullptr;
    }
	if (ImageTypeBitmap != image->GetType()) //lint !e613
		return nullptr;

	Gdiplus::Bitmap* bitmap =  dynamic_cast<Gdiplus::Bitmap*>(image);

    if (nullptr == bitmap)
    {
        return nullptr;
    }
	return CreateRegionFromBitmap(bitmap);
}//lint !e715

WCHAR * CTranslucentUtility::ttowc(const TCHAR *strTrans)
{
#ifdef _UNICODE
    return strTrans;
#else
    WCHAR* wStr = new WCHAR[2 * strlen(strTrans) + 2];
    if(!wStr)
        return nullptr;
    (void)MultiByteToWideChar(CP_ACP, 0, strTrans, -1, wStr, 2 * (int )strlen(strTrans) + 2);
    return wStr;
#endif
}
