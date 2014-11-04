// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

/**************************************************************************
* name       : FreeSock
* description: 释放Socket通讯库资源;
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void FreeSock()
{
	(void)WSACleanup();
}

/**************************************************************************
* name       : InitSock
* description: 初始化Socket通讯库
* input      : NA
* output     : NA
* return     : TRUE 成功，FALSE 失败
* remark     : NA
**************************************************************************/
BOOL InitSock()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2,2);

	//打开socket通讯库;
	if (0 != WSAStartup(wVersionRequested,&wsaData))
	{
		return FALSE;
	}

	//检查版本号是否为2.2;
	if ((2 != LOBYTE(wsaData.wVersion))||(2 != HIBYTE(wsaData.wVersion)))
	{
		FreeSock();
		return FALSE;
	}

	return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (FALSE == InitSock())
		{
			return FALSE;
		}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
        FreeSock();
		break;
	default:
		break;
	}
	return TRUE;
}

