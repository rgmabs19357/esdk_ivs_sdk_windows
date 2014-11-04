#include "TelepresenceMgr.h"
#include "TPPlayChannel.h"
#include "bp_log.h"
#include "LockGuard.h"
#include "IVS_Trace.h"
#include "IVSCommon.h"
#include "ivs_xml.h"
#include "ToolsHelp.h"
#include "MediaBase.h"

CTelepresenceMgr::CTelepresenceMgr(void)
				: m_pUserMgr(NULL)
{
	m_pChannelMapMutex = VOS_CreateMutex();
}


CTelepresenceMgr::~CTelepresenceMgr(void)
{
	VOS_MutexLock(m_pChannelMapMutex);
	try
	{
		TP_PLAY_CHANNEL_MAP::iterator iter = m_channelMap.begin();
		TP_PLAY_CHANNEL_MAP::iterator iter_end = m_channelMap.end();
		for(; iter!=iter_end; iter++)
		{
			CTPPlayChannel* pChannel = dynamic_cast<CTPPlayChannel*>(iter->second);//lint !e611
			IVS_DELETE(pChannel);
		}
	}
	catch (...)
	{
	}
	(void)VOS_MutexUnlock(m_pChannelMapMutex);
	(void)VOS_DestroyMutex(m_pChannelMapMutex);
	m_pChannelMapMutex = NULL;
	m_pUserMgr = NULL;
}

//开始播放视频
IVS_INT32 CTelepresenceMgr::StartRealPlayByIP(const std::string& strCameraCode,
												const IVS_MEDIA_ADDR& stMediaAddrDst,
												IVS_MEDIA_ADDR& stMediaAddrSrc,
												IVS_ULONG& ulHandle)
{
	IVS_DEBUG_TRACE("");

	if(0==strCameraCode.length())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TP Start Play", "Camera Code Invalid.");
		return IVS_PARA_INVALID;
	}
	if(IP_V4!=stMediaAddrDst.stIP.uiIPType || 0==stMediaAddrDst.uiAudioPort
		|| 0==stMediaAddrDst.uiVideoPort)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TP Start Play", "Media Addr Invalid.");
		return IVS_PARA_INVALID;
	}

	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle, TRUE);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "TP Start Play", "Create Play Channel Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	pPlayChannel->SetTPMgr(this);
	ulHandle = (IVS_ULONG)pPlayChannel;
	IVS_MEDIA_PARA stMediaPara;
	memset(&stMediaPara, 0, sizeof(stMediaPara));
	stMediaPara.ProtocolType = PROTOCOL_RTP_OVER_UDP;
	stMediaPara.StreamType = STREAM_TYPE_MAIN;
	stMediaPara.TransMode = MEDIA_TRANS;
	stMediaPara.BroadCastType = BROADCAST_UNICAST;
	stMediaPara.VideoDecType = VIDEO_DEC_H264;
	stMediaPara.AudioDecType = AUDIO_DEC_G711A;
	IVS_INT32 iRet = pPlayChannel->SetMediaParam(&stMediaPara);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TP Start Play", "Set Media Param Failed");
		RemovePlayChannel(ulHandle);
		ulHandle = 0;
		return iRet;
	}
	iRet = pPlayChannel->StartRealPlayByIP(strCameraCode, stMediaAddrDst, stMediaAddrSrc);
	if(IVS_SUCCEED!=iRet)
	{
		RemovePlayChannel(ulHandle);
		ulHandle = 0;
	}
	BP_DBG_LOG("CTelepresenceMgr::StartRealPlayByIP, Handle: %lu", ulHandle);
	return iRet;
}

//开始播放视频
IVS_INT32 CTelepresenceMgr::StartRealPlayByIPEx(const std::string& strCameraCode,
												const IVS_REALPLAY_PARAM&  RealplayParam,
												const IVS_MEDIA_ADDR& stMediaAddrDst,
												IVS_MEDIA_ADDR& stMediaAddrSrc,
												IVS_ULONG& ulHandle)
{
	IVS_DEBUG_TRACE("");

	if(0==strCameraCode.length())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start RealPlay ByIP", "Camera Code Invalid.");
		return IVS_PARA_INVALID;
	}

	//modify by zwx211831, Date:20140813, UDP协议下音频端口不能为0，TCP可以为0
	if(IP_V4!=stMediaAddrDst.stIP.uiIPType 
		|| (0==stMediaAddrDst.uiAudioPort && (PROTOCOL_RTP_OVER_UDP == (IVS_PROTOCOL_TYPE)RealplayParam.uiProtocolType)
		|| PROTOCOL_UDP == (IVS_PROTOCOL_TYPE)RealplayParam.uiProtocolType)
		|| 0==stMediaAddrDst.uiVideoPort
		)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start RealPlay ByIP", "Media Addr Invalid.");
		return IVS_PARA_INVALID;
	}

	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle, TRUE);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Start RealPlay ByIP", "Create Play Channel Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	pPlayChannel->SetTPMgr(this);
	ulHandle = (IVS_ULONG)pPlayChannel;

	IVS_MEDIA_PARA stMediaPara;
	memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
	stMediaPara.ProtocolType = (IVS_PROTOCOL_TYPE)RealplayParam.uiProtocolType;
	stMediaPara.StreamType = (IVS_STREAM_TYPE)RealplayParam.uiStreamType;
	stMediaPara.TransMode = (IVS_MEDIA_TRANS_MODE)RealplayParam.bDirectFirst;
	stMediaPara.BroadCastType = (IVS_BROADCAST_TYPE)RealplayParam.bMultiCast;
	stMediaPara.VideoDecType = VIDEO_DEC_H264;
	stMediaPara.AudioDecType = AUDIO_DEC_G711A;
	IVS_INT32 iRet = pPlayChannel->SetMediaParam(&stMediaPara);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start RealPlay ByIP", "Set Media Param Failed");
		RemovePlayChannel(ulHandle);
		ulHandle = 0;
		return iRet;
	}
	iRet = pPlayChannel->StartRealPlayByIP(strCameraCode, stMediaAddrDst, stMediaAddrSrc);
	if(IVS_SUCCEED!=iRet)
	{
		RemovePlayChannel(ulHandle);
		ulHandle = 0;
	}
	BP_DBG_LOG("CTelepresenceMgr::StartRealPlayByIPEx, Handle: %lu", ulHandle);
	return iRet;
}

IVS_INT32 CTelepresenceMgr::StartPlatformPlayBackByIP(const std::string& strCameraCode,
												const IVS_PLAYBACK_PARAM& stPlaybackParam,
												const IVS_MEDIA_ADDR& stMediaAddrDst,
												IVS_MEDIA_ADDR& stMediaAddrSrc,
												IVS_ULONG& ulHandle)
{
	IVS_DEBUG_TRACE("");

	if(0==strCameraCode.length())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform PlayBack ByIP", "Camera Code Invalid.");
		return IVS_PARA_INVALID;
	}

	//modify by zwx211831, Date:20140813, UDP协议下音频端口不能为0，TCP可以为0
	if(IP_V4!=stMediaAddrDst.stIP.uiIPType 
		|| (0==stMediaAddrDst.uiAudioPort && (PROTOCOL_RTP_OVER_UDP == (IVS_PROTOCOL_TYPE)stPlaybackParam.uiProtocolType)
		|| PROTOCOL_UDP == (IVS_PROTOCOL_TYPE)stPlaybackParam.uiProtocolType)
		|| 0==stMediaAddrDst.uiVideoPort
		)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform PlayBack ByIP", "Media Addr Invalid.");
		return IVS_PARA_INVALID;
	}

	TIME_SPAN_INTERNAL stTimeSpanInter;
	memset(&stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
	IVS_INT32 iRet = CheckParam(&stPlaybackParam.stTimeSpan, stPlaybackParam.uiProtocolType, stPlaybackParam.fSpeed, stTimeSpanInter);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform PlayBack ByIP", "CheckParam failed.");
		return IVS_PARA_INVALID;
	}

	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle, TRUE);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Start Platform PlayBack ByIP", "Create Play Channel Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	pPlayChannel->SetTPMgr(this);
	ulHandle = (IVS_ULONG)pPlayChannel;

	IVS_MEDIA_PARA stMediaPara;
	memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
	stMediaPara.DecoderCompany = VENDOR_HUAWEI;
	stMediaPara.StreamType = STREAM_TYPE_MAIN;
	stMediaPara.TransMode = MEDIA_TRANS;
	stMediaPara.BroadCastType = BROADCAST_UNICAST;
	stMediaPara.VideoDecType = VIDEO_DEC_H264;
	stMediaPara.AudioDecType = AUDIO_DEC_G711A;
	stMediaPara.ProtocolType = static_cast<IVS_PROTOCOL_TYPE>(stPlaybackParam.uiProtocolType);
	iRet = pPlayChannel->SetMediaParam(&stMediaPara);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start Platform PlayBack ByIP", "Set Media Param Failed");
		RemovePlayChannel(ulHandle);
		ulHandle = 0;
		return iRet;
	}
	iRet = pPlayChannel->StartPlatformPlayBackByIP(strCameraCode, stMediaAddrDst, stMediaAddrSrc, stTimeSpanInter, stPlaybackParam.fSpeed);
	if(IVS_SUCCEED!=iRet)
	{
		RemovePlayChannel(ulHandle);
		ulHandle = 0;
	}
	BP_DBG_LOG("CTelepresenceMgr::StartPlatformPlayBackByIP, Handle: %lu", ulHandle);
	return iRet;
}

//停止播放
IVS_INT32 CTelepresenceMgr::StopPlayByIP(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Stop Play", "Handle Invalid.");
		return IVS_PARA_INVALID;
	}
	IVS_INT32 iRet = pPlayChannel->StopPlayByIP();
	RemovePlayChannel(ulHandle);
	return iRet;
}

//停止所有播放
void CTelepresenceMgr::StopAllPlayByIP()
{
	IVS_DEBUG_TRACE("");
	VOS_MutexLock(m_pChannelMapMutex);

	TP_PLAY_CHANNEL_MAP::iterator iter = m_channelMap.begin();
	TP_PLAY_CHANNEL_MAP::iterator iter_end = m_channelMap.end();
	try
	{
		for(; iter!=iter_end; iter++)
		{
			CTPPlayChannel* pPlayChannel = dynamic_cast<CTPPlayChannel*>(iter->second);//lint !e611
			pPlayChannel->StopPlayByIP();
			IVS_DELETE(pPlayChannel);
		}
	}
	catch (...)
	{
	}

	m_channelMap.clear();
	VOS_MutexUnlock(m_pChannelMapMutex);
}

CTPPlayChannel* CTelepresenceMgr::GetPlayChannel(IVS_ULONG ulHandle, IVS_BOOL bCreate)
{
	IVS_DEBUG_TRACE("");
	CLockGuard autoLock(m_pChannelMapMutex);
	CTPPlayChannel* pPlayChannel = NULL;
	TP_PLAY_CHANNEL_MAP::iterator ite = m_channelMap.find(ulHandle);
	if(ite!=m_channelMap.end())
	{
		pPlayChannel = dynamic_cast<CTPPlayChannel*>(ite->second);//lint !e611
	}
	if(NULL==pPlayChannel && bCreate)
	{
		pPlayChannel = IVS_NEW(pPlayChannel);
		m_channelMap.insert(TP_PLAY_CHANNEL_MAP::value_type((IVS_ULONG)pPlayChannel, pPlayChannel));
	}
	return pPlayChannel;
}

IVS_VOID CTelepresenceMgr::RemovePlayChannel(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
	CLockGuard autoLock(m_pChannelMapMutex);
	CTPPlayChannel* pPlayChannel = NULL;
	TP_PLAY_CHANNEL_MAP::iterator ite = m_channelMap.find(ulHandle);
	if(ite!=m_channelMap.end())
	{
		pPlayChannel = dynamic_cast<CTPPlayChannel*>(ite->second);//lint !e611
		IVS_DELETE(pPlayChannel);
		m_channelMap.erase(ite);
	}
}

IVS_INT32 CTelepresenceMgr::GetURL(const char *pszAuthHeader, ST_URL_MEDIA_INFO& stUrlMediaInfo, ST_MEDIA_RSP& stMediaRsp)
{
	bool bAudio = false;
	return CMediaBase::GetURL(pszAuthHeader, m_pUserMgr, stUrlMediaInfo, stMediaRsp, bAudio);
}

//检验回放参数是否合法
IVS_INT32 CTelepresenceMgr::CheckParam(const IVS_TIME_SPAN* pTimeSpan,IVS_UINT32 uiProtocolType, IVS_FLOAT fSpeed, TIME_SPAN_INTERNAL& stTimeSpanInter)
{
	CHECK_POINTER(pTimeSpan,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;

	stTimeSpanInter.iStartTime = (IVS_INT32)CMediaBase::UTCStr2Time(pTimeSpan->cStart, IVS_DATE_TIME_FORMAT);
	stTimeSpanInter.iEndTime = (IVS_INT32)CMediaBase::UTCStr2Time(pTimeSpan->cEnd, IVS_DATE_TIME_FORMAT);
	if( stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime )
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform Video Play: param time span  is invalidate","failed");
		return IVS_PARA_INVALID;
	}

	if (PROTOCOL_RTP_OVER_UDP == uiProtocolType)
	{
		//UDP
	}
	else if(PROTOCOL_RTP_OVER_TCP == uiProtocolType)
	{
		//TCP
	}
	else
	{
		iRet = IVS_PARA_INVALID;
		return iRet;
	}

	// 检查播放倍速是否合法;
	bool bRet = CToolsHelp::IsSpeedVaild(fSpeed);
	if (!bRet)
	{
		iRet = IVS_PARA_INVALID;
		return iRet;
	}

	return IVS_SUCCEED;
}//lint !e1762

//获取实况句柄
IVS_ULONG CTelepresenceMgr::GetPlayHandlebyRtspHandle(long iRtspHandle)
{
	IVS_ULONG ulPlayHandle = 0;
	TP_PLAY_CHANNEL_MAP::iterator iter = m_channelMap.begin();
	TP_PLAY_CHANNEL_MAP::iterator iterEnd = m_channelMap.end();
	for (; iter != iterEnd; iter++)
	{
		CTPPlayChannel* pPlayChannel = dynamic_cast<CTPPlayChannel*>(iter->second);
		if (NULL == pPlayChannel)
		{
			continue;
		}
		long iRtspHandleTemp = pPlayChannel->GetRtspHandle();
		if (iRtspHandleTemp == iRtspHandle)
		{
			ulPlayHandle = iter->first;
			break;
		}
	}

	return ulPlayHandle;
}


//暂停平台录像回放
IVS_INT32 CTelepresenceMgr::PlatformPlayBackPauseByIP(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("Handle: %lu", ulHandle);
	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED, "Pause Platform PlayBack", "Handle Invalid.");
		return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	return pPlayChannel->PlatformPlayBackPauseByIP();
}


//恢复平台录像回放
IVS_INT32 CTelepresenceMgr::PlatformPlayBackResumeByIP(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("Handle: %lu", ulHandle);
	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED, "Resume Platform PlayBack", "Handle Invalid.");
		return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	return pPlayChannel->PlatformPlayBackResumeByIP();
}


//设置平台录像码流回放速度
IVS_INT32 CTelepresenceMgr::SetPlayBackSpeedByIP(IVS_ULONG ulHandle, IVS_FLOAT fSpeed)
{
	IVS_DEBUG_TRACE("Handle: %lu, Speed: %f", ulHandle, fSpeed);
	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED, "Set Platform PlayBack Speed", "Handle Invalid.");
		return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	return pPlayChannel->SetPlayBackSpeedByIP(fSpeed);
}


//设置平台录像码流回放时间
IVS_INT32 CTelepresenceMgr::SetPlayBackTimeByIP(IVS_ULONG ulHandle, IVS_UINT32 uiTime)
{
	IVS_DEBUG_TRACE("Handle: %lu, Time: %d", ulHandle, uiTime);
	CTPPlayChannel* pPlayChannel = GetPlayChannel(ulHandle);
	if(NULL==pPlayChannel)
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED, "Set PlayBack Time ByIP", "Handle Invalid.");
		return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}
	return pPlayChannel->SetPlayBackTimeByIP(uiTime);
}