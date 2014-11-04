/********************************************************************
	filename	: 	IVSSDK.cpp
	author		:	s00191067
	created		:	2011/10/29	
	description	:	定义 DLL 应用程序的导出函数。
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/29 初始版本
*********************************************************************/
/*!< 预编译头文件 */
#include "stdafx.h"
#include <WinSock2.h>   //Sockt通信使用

/*!< 自身头文件 */
#include "IVSSDKNetKeyBoard.h"
#include "NetKeyBoardService.h"
#include "IVS_Trace.h"

//登录权限标识
enum enNetKeyBoardVendor
{
    NET_KEYBOARD_DAHUA = 1,
    NET_KEYBOARD_TDWY,
};

static HANDLE g_hThread = NULL;                  //线程句柄

/**************************************************************************
* name       : SetListenAddr
* description: 设置服务器端监听IP和端口号
* input      : iPort服务器端监听端口号
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void __stdcall IVS_SDK_KB_SetNetKeyBoardListenAddr(u_short iPort)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CNetKeyBoardService *pNetKeyBoardService = CNetKeyBoardService::GetInstance();
    CHECK_POINTER_VOID(pNetKeyBoardService);
    if (true)
    {
        pNetKeyBoardService->m_UDPSocketService.SetServerAddr(iPort);
    }
    else
    {
        pNetKeyBoardService->m_SocketService.SetServerAddr(iPort);
    }
}

/**************************************************************************
* name       : StartListenThread
* description: 启动监听线程
* input      : lpCallBack 回调函数
* output     : NA
* return     : g_hThread
* remark     : NA
**************************************************************************/
int __stdcall IVS_SDK_KB_StartNetKeyBoardListenThread(u_short ulNetKeyBoardVendor, u_short ulPort, LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard Start NetKeyBoard Listen Thread: ulNetKeyBoardVendor = %d, ulPort = %d.", ulNetKeyBoardVendor, ulPort);
    
    CNetKeyBoardService *pNetKeyBoardService = CNetKeyBoardService::GetInstance();
    CHECK_POINTER(pNetKeyBoardService,IVS_FAIL);

    // 接入大华
    if (NET_KEYBOARD_DAHUA == ulNetKeyBoardVendor)
    {
        pNetKeyBoardService->m_SocketService.SetServerAddr(ulPort);

        int iRet = pNetKeyBoardService->IniTcpServer();
        if (IVS_SUCCEED != iRet)
        {
            BP_DBG_LOG("IVS_SDK_KB_StartNetKeyBoardListenThread: Init TCP Server Failed.");
            return iRet;
        }

        g_hThread = CreateThread(NULL,0,pNetKeyBoardService->ListenSocket,(LPVOID)lpCallBack,0,NULL);
    }
    // 接入天地伟业
    else
    {
        pNetKeyBoardService->m_UDPSocketService.SetServerAddr(ulPort);

        int iRet = pNetKeyBoardService->IniUdpServer();
        if (IVS_SUCCEED != iRet)
        {
            BP_DBG_LOG("IVS_SDK_KB_StartNetKeyBoardListenThread: Init UDP Server Failed.");
            return iRet;
        }

        g_hThread = CreateThread(NULL,0,pNetKeyBoardService->ListenUDPSocket,(LPVOID)lpCallBack,0,NULL);
    }

    if (NULL == g_hThread)
    {
        int iRet;
        iRet = GetLastError();
        BP_RUN_LOG_ERR(IVS_FAIL, "Start Listen Thread Failed! ", "Error Code: %d",iRet);

        pNetKeyBoardService->ReleaseListenServer();
        return iRet;
    }

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : ReleaseThread
* description: 释放线程
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void __stdcall IVS_SDK_KB_ReleaseThread()
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CNetKeyBoardService *pNetKeyBoardService = CNetKeyBoardService::GetInstance();
    CHECK_POINTER_VOID(pNetKeyBoardService);
    pNetKeyBoardService->ReleaseListenThread(g_hThread);
}

