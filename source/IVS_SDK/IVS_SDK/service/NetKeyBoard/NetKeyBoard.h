/********************************************************************
	filename	: 	NetKeyBoardService.h
	author		:	
	created		:	2011/10/28	
	description	:	网络键盘实现类
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28 初始版本
*********************************************************************/
#ifndef NET_KEY_BOARD_H
#define NET_KEY_BOARD_H

/*!< 单键实例头文件 */
#include "SingleInst.h"
/*!< 公共宏、结构体头文件 */
#include "IVSSDKNetKeyBoardDef.h"


class CNetKeyBoard
{
    DECLARE_SINGLE_INSTANCE(CNetKeyBoard)

public:
    CNetKeyBoard(void);
    virtual ~CNetKeyBoard(void);


    /**************************************************************************
    * name       : SetListenAddr
    * description: 设置服务器端监听IP和端口号
    * input      : iPort服务器端监听端口号
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void SetNetKeyBoardListenAddr(unsigned short iPort);

    /**************************************************************************
    * name       : StartListenThread
    * description: 启动监听线程
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : g_hThread
    * remark     : NA
    **************************************************************************/
    int StartNetKeyBoard(unsigned short ulNetKeyBoardVendor, unsigned short ulPort, LPCallBack lpCallBack);

    /**************************************************************************
    * name       : ReleaseThread
    * description: 释放线程
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    int ReleaseThread();

private:
    bool m_bRun;

};
#endif //NET_KEY_BOARD_H
