#pragma once

#include <GdiPlus.h>

class CTranslucentUtility
{
public:
	static CString GetModulePath(HMODULE hModule = NULL);

	static BOOL IsFileExist(LPCTSTR lpszFilePath);

	static BOOL ExtractResourceToFile( LPCTSTR lpszType
		, UINT nResID
		, LPCTSTR lpszFilename
		, HMODULE hModule
		);

	static Gdiplus::Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance =nullptr );

	static HRGN CreateRegionFromBitmap(Gdiplus::Bitmap* bitmap, BYTE alphaValve = 0);

	static HRGN CreateRegion(LPCTSTR lpszFile, BYTE alphaValve = 0);

	static HRGN CreateRegion(UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL, BYTE alphaValve = 0); 

	static WCHAR * ttowc(const TCHAR *str ); 
};
