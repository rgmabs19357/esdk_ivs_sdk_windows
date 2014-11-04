/********************************************************************
filename	: 	NetKeyBoardService.cpp
author		:	
created		:	2011/10/28	
description	:	
copyright	:	Copyright (C) 2011-2015
history		:	2011/10/28 初始版本
*********************************************************************/
/*!< 预编译头文件 */
#include "StdAfx.h"
/*!< 自身头文件 */
#include "NetKeyBoard.h"
/*!< 工具类头文件 */
#include "ToolsHelp.h"
/*!< 媒体的下层接口头文件 */
#include "IVSSDKNetKeyBoard.h"


IMPLEMENT_SINGLE_INSTANCE(CNetKeyBoard)

CNetKeyBoard::CNetKeyBoard(void)
{
    m_bRun = false;
}

CNetKeyBoard::~CNetKeyBoard(void)
{
    m_bRun = false;
}

/**************************************************************************
* name       : SetListenAddr
* description: 设置服务器端监听IP和端口号
* input      : iPort服务器端监听端口号
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoard::SetNetKeyBoardListenAddr(unsigned short iPort)
{

    IVS_SDK_KB_SetNetKeyBoardListenAddr(iPort);

}//lint !e1762

/**************************************************************************
* name       : StartListenThread
* description: 启动监听线程
* input      : lpCallBack 回调函数
* output     : NA
* return     : IVS_SUCCEED - 成功,其他 - 失败
* remark     : NA
**************************************************************************/
int CNetKeyBoard::StartNetKeyBoard(unsigned short ulNetKeyBoardVendor, unsigned short ulPort, LPCallBack lpCallBack)
{

    int iRet = IVS_SDK_KB_StartNetKeyBoardListenThread(ulNetKeyBoardVendor, ulPort, lpCallBack);
    if (IVS_SUCCEED != iRet)
    {
        m_bRun = false;
    }
    else
    {
        m_bRun = true;
    }

    return iRet;
}

/**************************************************************************
* name       : ReleaseThread
* description: 释放线程
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
int CNetKeyBoard::ReleaseThread()
{

    if (!m_bRun)
    {
        return IVS_FAIL;
    }
    IVS_SDK_KB_ReleaseThread();
    m_bRun = false;

    return IVS_SUCCEED;
}

