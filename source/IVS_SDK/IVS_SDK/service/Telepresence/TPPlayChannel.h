/********************************************************************
 filename    :    TPPlayChannel.h
 author      :    g00209332
 created     :    2013/3/6
 description :    智真对接播放类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/3/6 初始版本
*********************************************************************/

#ifndef _TP_PLAY_CHANNEL_H_
#define _TP_PLAY_CHANNEL_H_

#include "hwsdk.h"
#include "SDKDef.h"

class CTelepresenceMgr;
class CTPPlayChannel
{
public:
	CTPPlayChannel(void);
	~CTPPlayChannel(void);
	//开始播放视频
	IVS_INT32 StartRealPlayByIP(const std::string& strCameraCode,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc);

	//开始平台录像回放
	IVS_INT32 StartPlatformPlayBackByIP(const std::string& strCameraCode,
								const IVS_MEDIA_ADDR& stMediaAddrDst,
								IVS_MEDIA_ADDR& stMediaAddrSrc,
								const TIME_SPAN_INTERNAL& stTimeSpanInter,
								IVS_FLOAT fSpeed);

	//停止实况及录像回放
	IVS_INT32 StopPlayByIP();

	//设置智真管理智真
	IVS_VOID SetTPMgr(CTelepresenceMgr* pTPMgr)
	{
		m_pTPMgr = pTPMgr;
	}

	//设置媒体参数
	IVS_INT32 SetMediaParam(const IVS_MEDIA_PARA* pMediaPara);

	//获取RTSP句柄
	long GetRtspHandle() const { return m_iRtspHandle; }

	//暂停平台录像回放
	IVS_INT32 PlatformPlayBackPauseByIP(IVS_VOID);

	//恢复平台录像回放
	IVS_INT32 PlatformPlayBackResumeByIP(IVS_VOID);

	//设置平台录像码流回放速度
	IVS_INT32 SetPlayBackSpeedByIP(IVS_FLOAT fSpeed);

	//设置平台录像码流回放时间
	IVS_INT32 SetPlayBackTimeByIP(IVS_UINT32 uiTime);

private:
	//获取实况URL
	IVS_INT32 GetRealPlayURL(const std::string& strCamera, 
		const IVS_MEDIA_ADDR& stMediaAddrDst, 
		ST_MEDIA_RSP& stMediaRsp);

	//获取录像回放URL
	IVS_INT32 GetPlayBackURL(const std::string& strCamera, 
		const TIME_SPAN_INTERNAL& stTimeSpanInter, 
		const IVS_MEDIA_ADDR& stMediaAddrDst, 
		ST_MEDIA_RSP& stMediaRsp);

	//获取播放流的开始结束时间
	IVS_INT32 GetPlayBackTimeSpan(const std::string& strURL,
		TIME_SPAN_INTERNAL& stTimeSpan);

private:
	IVS_INT32 m_iRtspHandle;				//RTSP句柄
	std::string m_strURL;			//媒体URL
	IVS_MEDIA_PARA m_stuMediaParam;   // 媒体参数
	CTelepresenceMgr* m_pTPMgr;		//智真对接管理
	float m_fRTSPSpeed;									// RTSP播放速度;
	TIME_SPAN_INTERNAL m_stTimeSpanInter;                // 播放流的开始结束时间;
};

#endif
