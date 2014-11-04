/********************************************************************
 filename    :    PlayThread.h
 author      :    g00209332
 created     :    2013/1/8
 description :    播放线程类;请求播放时单独开启线程进行处理，防止界面阻塞
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/1/8 初始版本
*********************************************************************/
#pragma once
#include "vos.h"
#include "hwsdk.h"
#include "DecoderChannel.h"
#include "TVWallMgr.h"

class CPlayThread
{
public:
    CPlayThread(void);
    virtual ~CPlayThread(void);
    IVS_INT32 run();
	// 设置摄像机Code
    void SetCameraCode(const IVS_CHAR* pCameraCode);
	// 设置解码器解码通道
    void SetDecoderChannel(CDecoderChannel* pCh);
	// 设置电视墙管理对象
	void SetTVWallMgr(CTVWallMgr* pTVWallMgr);
	// 设置电视墙参数
    void SetTVWallParam(const IVS_TVWALL_PARAM* pTVWallParam);
protected:
	//开始播放视频（实况、回放）
    virtual IVS_INT32 StartPlay() = 0;
    IVS_CHAR m_szCameraCode[IVS_DEV_CODE_LEN+1];	//摄像机Code
    IVS_TVWALL_PARAM m_stTVWallParam;				//电视墙参数
    CDecoderChannel* m_pChannel;					//解码器通道对象
	CTVWallMgr* m_pTVWallMgr;						//电视墙管理对象
private:
	//线程调用函数
    static void invoke(void *argc);					
    VOS_Thread* m_pVosThread;						//线程对象
};

class CRealPlayThread : public CPlayThread
{
public:
    CRealPlayThread(void);
    virtual ~CRealPlayThread(void);
	// 设置实况参数
    void SetRealPlayParam(const IVS_REALPLAY_PARAM* pRealPlayParam);
protected:
	//开始播放实况
    virtual IVS_INT32 StartPlay();
private:
    IVS_REALPLAY_PARAM m_stRealPlayParam;			//实况参数
};

class CPlayBackThread : public CPlayThread
{
public:
    CPlayBackThread(void);
    virtual ~CPlayBackThread(void);
protected:
	//开始播放回放
    virtual IVS_INT32 StartPlay();
private:
    IVS_PLAYBACK_PARAM m_stRealPlayParam;			//回放参数
};
