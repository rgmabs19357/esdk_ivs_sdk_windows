/********************************************************************
	filename	: 	TVWallService.h
	author		:	z00201790
	created		:	2012/12/12	
	description	:	定义电视墙业务管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/12 初始版本;
*********************************************************************/

#ifndef __TVWALL_SERVICE_H__
#define __TVWALL_SERVICE_H__

#include "hwsdk.h"
#include "ivs_error.h"
#include "ControlBus.h"

#include <string>
using namespace std;
/*!< 单键实例头文件 */
#include "SingleInst.h"
/*!< 电视上墙功能实现类 */
#include "ControlBus.h"


class CTVWallService
{
    DECLARE_SINGLE_INSTANCE(CTVWallService)

public:
    virtual ~CTVWallService(void);

    // 初始化TV Wall模块
    IVS_INT32 InitTVWallService(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

    // 释放TV Wall模块
    IVS_INT32 ReleaseTVWallService();

    // 初始化解码器
    IVS_INT32 IniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

    // 命令解码器开始解码
    IVS_INT32 StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

    // 命令解码器停止解码
    IVS_INT32 StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo);

    // 播放/停止播放声音
    IVS_INT32 PlayAudio(IPI_AUDIO_CONTROL_REQ* pParaInfo);

    // 设置音量
    IVS_INT32 SetVolume(IPI_TUNEUP_VOLUME_REQ* pParaInfo);

private:
    CTVWallService(void);

private:
    CControlBus m_ControlBus;//电视上墙控制对象

};
#endif //__TVWALL_SERVICE_H__

