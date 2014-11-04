#include "TPPlayChannel.h"
#include "TelepresenceMgr.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "rtsp_client_api.h"
#include "MediaBase.h"
#include "IVS_Trace.h"

// Rtsp媒体回调函数
void __stdcall  RtspDataCallBack(int /*handle*/, char * /*pBuffer*/, unsigned int /*ulBufSize*/, void * /*pUser*/)
{
	return;
}//lint !e818

CTPPlayChannel::CTPPlayChannel(void)
				: m_iRtspHandle(0)
				, m_pTPMgr(NULL)
				, m_fRTSPSpeed(1.0)
{
	memset(&m_stuMediaParam, 0, sizeof(m_stuMediaParam));
	memset(&m_stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
}


CTPPlayChannel::~CTPPlayChannel(void)
{
	m_pTPMgr = NULL;
	m_fRTSPSpeed = 0.0;
}

//开始播放视频
IVS_INT32 CTPPlayChannel::StartRealPlayByIP(const std::string& strCameraCode,
											const IVS_MEDIA_ADDR& stMediaAddrDst,
											IVS_MEDIA_ADDR& stMediaAddrSrc)
{
	IVS_DEBUG_TRACE("");
	//如果该通道正在播放，则关闭
	if(m_iRtspHandle>0)
	{
		(void)StopPlayByIP();
	}
	//获取媒体URL
	ST_MEDIA_RSP mediaRsp;
	memset(&mediaRsp, 0, sizeof(ST_MEDIA_RSP));
	IVS_INT32 iRet = GetRealPlayURL(strCameraCode, stMediaAddrDst, mediaRsp);
	if (IVS_SUCCEED != iRet || 0 == strlen(mediaRsp.szURL))
	{
		BP_RUN_LOG_ERR(iRet,"TP Start Play Req", "get URL failed!");
		return iRet;
	}
	BP_DBG_LOG("CTPPlayChannel::StartRealPlayByIP, get URL success: url = %s",mediaRsp.szURL);
	m_strURL = mediaRsp.szURL;
	// 建立RTSP连接
	iRet = RTSP_CLIENT_GetHandle(true, &m_iRtspHandle);
	if (IVS_SUCCEED != iRet || 0==m_iRtspHandle)
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL,"TP Start Play", "Rtsp client get handle fail.");
		return IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL;
	}

	// setup
	MEDIA_ADDR stTPMCUAddr = {0};	//智真媒体服务器
	MEDIA_ADDR stMUMediaAddr = {0};	//MU
	INET_ADDR stServiceAddr = {0};
	bool bTcpFlag = false;
	stTPMCUAddr.unMediaIp = htonl((unsigned int)inet_addr(stMediaAddrDst.stIP.cIP));
	stTPMCUAddr.unMediaVideoPort = static_cast<unsigned short>(stMediaAddrDst.uiVideoPort);
	stTPMCUAddr.unMediaAudioPort = static_cast<unsigned short>(stMediaAddrDst.uiAudioPort);
	stServiceAddr.unAddrIp = htonl((unsigned int)inet_addr(mediaRsp.szRTSPIP));
	stServiceAddr.unAddrPort = (unsigned short)atoi(mediaRsp.szRTSPPORT);
	iRet = RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, RtspDataCallBack, this);
	iRet += RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, mediaRsp.szURL, RTSP_CLIENT_TIMEOUT, bTcpFlag, &stTPMCUAddr, &stMUMediaAddr);

	if (IVS_SUCCEED != iRet)
	{         
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(CMediaBase::GetRtspErrorCode(iRet),"TP Start Play", "Send Setup Message failed, rtsp errcode=%d.", iRet);
		return CMediaBase::GetRtspErrorCode(iRet);
	}
	if(0==stMUMediaAddr.unMediaVideoPort && 0==stMUMediaAddr.unMediaAudioPort)
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(IVS_SDK_PLAYBACK_ERR_GET_REMOTE_DATAINFO,"TP Start Play", "Setup Message return port=0.");
		return IVS_SDK_PLAYBACK_ERR_GET_REMOTE_DATAINFO;
	}
	stMediaAddrSrc.uiAudioPort = stMUMediaAddr.unMediaAudioPort;
	stMediaAddrSrc.uiVideoPort = stMUMediaAddr.unMediaVideoPort;
	std::string strIP;
	if(IVS_SUCCEED!=CMediaBase::AddrUint2Char(ntohl(stMUMediaAddr.unMediaIp), strIP))
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"TP Start Play","Parse Media IP Failed!");  
		return IVS_PARA_INVALID;

	}
	bool bSucc = CToolsHelp::Strncpy(stMediaAddrSrc.stIP.cIP, sizeof(stMediaAddrSrc.stIP.cIP), strIP.c_str(), strIP.length());
	if(!bSucc)
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"TP Start Play","Copy Media IP Failed!");  
		return IVS_ALLOC_MEMORY_ERROR;
	}
	BP_DBG_LOG("CTPPlayChannel::StartRealPlayByIP, Media IP: %s, VideoPort: %u, AutioPort: %u", 
				stMediaAddrSrc.stIP.cIP, stMediaAddrSrc.uiVideoPort, stMediaAddrSrc.uiAudioPort);

	// play
	MEDIA_RANGE_S stMdieaRange;
	ULONG ulStartTime = 0;
	double dScale = 1.0;
	memset(&stMdieaRange, 0x0, sizeof(MEDIA_RANGE_S));
	stMdieaRange.enRangeType = RANGE_TYPE_NPT;
	stMdieaRange.MediaBeginOffset = ulStartTime;
	iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, mediaRsp.szURL, dScale, &stMdieaRange, RTSP_CLIENT_TIMEOUT);

	if (IVS_SUCCEED != iRet)
	{  
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		BP_RUN_LOG_ERR(iRet, "TP Start Play", "Send Play Message failed.");
		return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
	}
	return IVS_SUCCEED;
}

//开始平台录像回放
IVS_INT32 CTPPlayChannel::StartPlatformPlayBackByIP(const std::string& strCameraCode,
											const IVS_MEDIA_ADDR& stMediaAddrDst,
											IVS_MEDIA_ADDR& stMediaAddrSrc,
											const TIME_SPAN_INTERNAL& stTimeSpanInter,
											IVS_FLOAT fSpeed)
{
	IVS_DEBUG_TRACE("");
	//如果该通道正在播放，则关闭
	if(m_iRtspHandle>0)
	{
		(void)StopPlayByIP();
	}

	//获取媒体URL
	ST_MEDIA_RSP mediaRsp;
	memset(&mediaRsp, 0, sizeof(ST_MEDIA_RSP));
	IVS_INT32 iRet = GetPlayBackURL(strCameraCode, stTimeSpanInter, stMediaAddrDst, mediaRsp);
	if (IVS_SUCCEED != iRet || 0 == strlen(mediaRsp.szURL))
	{
		BP_RUN_LOG_ERR(iRet,"TP Start Play Req", "get URL failed!");
		return iRet;
	}
	BP_DBG_LOG("CTPPlayChannel::StartPlatformPlayBackByIP, get URL success: url = %s",mediaRsp.szURL);
	m_strURL = mediaRsp.szURL;

	// 建立RTSP连接
	iRet = RTSP_CLIENT_GetHandle(true, &m_iRtspHandle);
	if (IVS_SUCCEED != iRet || 0==m_iRtspHandle)
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL,"Start Platform PlayBack ByIP", "Rtsp client get handle fail.");
		return IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL;
	}

	// setup
	MEDIA_ADDR stTPMCUAddr = {0};	//媒体服务器
	MEDIA_ADDR stMUMediaAddr = {0};	//MU
	INET_ADDR stServiceAddr = {0};
	bool bTcpFlag = false;
	stTPMCUAddr.unMediaIp = htonl((unsigned int)inet_addr(stMediaAddrDst.stIP.cIP));
	stTPMCUAddr.unMediaVideoPort = static_cast<unsigned short>(stMediaAddrDst.uiVideoPort);
	stTPMCUAddr.unMediaAudioPort = static_cast<unsigned short>(stMediaAddrDst.uiAudioPort);
	stServiceAddr.unAddrIp = htonl((unsigned int)inet_addr(mediaRsp.szRTSPIP));
	stServiceAddr.unAddrPort = (unsigned short)atoi(mediaRsp.szRTSPPORT);
	iRet = RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, RtspDataCallBack, this);
	iRet += RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, mediaRsp.szURL, RTSP_CLIENT_TIMEOUT, bTcpFlag, &stTPMCUAddr, &stMUMediaAddr);

	if (IVS_SUCCEED != iRet)
	{         
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(CMediaBase::GetRtspErrorCode(iRet),"Start Platform PlayBack ByIP", "Send Setup Message failed, rtsp errcode=%d.", iRet);
		return CMediaBase::GetRtspErrorCode(iRet);
	}
	if(0==stMUMediaAddr.unMediaVideoPort && 0==stMUMediaAddr.unMediaAudioPort)
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(IVS_SDK_PLAYBACK_ERR_GET_REMOTE_DATAINFO,"Start Platform PlayBack ByIP", "Setup Message return port=0.");
		return IVS_SDK_PLAYBACK_ERR_GET_REMOTE_DATAINFO;
	}
	stMediaAddrSrc.uiAudioPort = stMUMediaAddr.unMediaAudioPort;
	stMediaAddrSrc.uiVideoPort = stMUMediaAddr.unMediaVideoPort;
	std::string strIP;
	if(IVS_SUCCEED!=CMediaBase::AddrUint2Char(ntohl(stMUMediaAddr.unMediaIp), strIP))
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Platform PlayBack ByIP","Parse Media IP Failed!");  
		return IVS_PARA_INVALID;

	}
	bool bSucc = CToolsHelp::Strncpy(stMediaAddrSrc.stIP.cIP, sizeof(stMediaAddrSrc.stIP.cIP), strIP.c_str(), strIP.length());
	if(!bSucc)
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Platform PlayBack ByIP","Copy Media IP Failed!");  
		return IVS_ALLOC_MEMORY_ERROR;
	}
	BP_DBG_LOG("CTPPlayChannel::StartPlatformPlayBackByIP, Media IP: %s, VideoPort: %u, AutioPort: %u", 
		stMediaAddrSrc.stIP.cIP, stMediaAddrSrc.uiVideoPort, stMediaAddrSrc.uiAudioPort);

	// play
	float fSendScale = fSpeed;
	(void)CToolsHelp::GetSendScale(SERVICE_TYPE_PLAYBACK, fSpeed, fSendScale);
	MEDIA_RANGE_S stMdieaRange;
	memset(&stMdieaRange, 0x0, sizeof(MEDIA_RANGE_S));
	stMdieaRange.enRangeType = RANGE_TYPE_NPT;
	if (0 > fSendScale || 253.0 == fSendScale || 252.0 == fSendScale) /* 252.0 1倍速全量后退, 253.0 2倍速全量后退 */
	{
		stMdieaRange.MediaBeginOffset = static_cast<IVS_UINT32>(stTimeSpanInter.iEndTime - stTimeSpanInter.iStartTime);
	}
	else
	{
		stMdieaRange.MediaBeginOffset = 0;
	}
	iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, mediaRsp.szURL, (double)fSendScale, &stMdieaRange, RTSP_CLIENT_TIMEOUT);

	if (IVS_SUCCEED != iRet)
	{  
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, mediaRsp.szURL, 1);
		(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		BP_RUN_LOG_ERR(iRet, "TP Start Play", "Send Play Message failed.");
		return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
	}

	m_fRTSPSpeed = fSendScale;
	return IVS_SUCCEED;
}

//停止实况及录像回放
IVS_INT32 CTPPlayChannel::StopPlayByIP()
{
	IVS_DEBUG_TRACE("");
	//关闭rtsp
	(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_strURL.c_str(), 1);
	(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle); 
	m_iRtspHandle = 0;
	m_strURL = "";
	return IVS_SUCCEED;
}

IVS_INT32 CTPPlayChannel::SetMediaParam(const IVS_MEDIA_PARA* pMediaPara)
{
	CHECK_POINTER(pMediaPara, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	BP_DBG_LOG("StreamType[%d], ProtocolType[%d] TransMode[%d] BroadCastType[%d]", 
				pMediaPara->StreamType, 
				pMediaPara->ProtocolType, 
				pMediaPara->TransMode, 
				pMediaPara->BroadCastType);

	if (STREAM_TYPE_UNSPECIFIED > pMediaPara->StreamType || STREAM_TYPE_SUB2 < pMediaPara->StreamType)//lint !e685
	{
		BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Set Media Param", "StreamType invalid [%d]", pMediaPara->StreamType);
		return IVS_DATA_INVALID;
	}

	if (PROTOCOL_RTP_OVER_UDP > pMediaPara->ProtocolType || PROTOCOL_RTP_OVER_TCP < pMediaPara->ProtocolType)
	{
		BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Set Media Param", "ProtocolType invalid [%d]", pMediaPara->ProtocolType);
		return IVS_DATA_INVALID;
	}

	if (MEDIA_TRANS > pMediaPara->TransMode || MEDIA_DIRECT < pMediaPara->TransMode)//lint !e685
	{
		BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Set Media Param", "TransMode invalid [%d]", pMediaPara->TransMode);
		return IVS_DATA_INVALID;
	}

	if (BROADCAST_UNICAST > pMediaPara->BroadCastType || BROADCAST_MULTICAST < pMediaPara->BroadCastType)//lint !e685
	{
		BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Set Media Param", "BroadCastType invalid [%d]", pMediaPara->BroadCastType);
		return IVS_DATA_INVALID;
	}

    bool bRet = CToolsHelp::Memcpy(&m_stuMediaParam,sizeof(IVS_MEDIA_PARA), pMediaPara, sizeof(IVS_MEDIA_PARA));
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Set Media Param", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    return IVS_SUCCEED;
}

IVS_INT32 CTPPlayChannel::GetRealPlayURL(const std::string& strCamera, const IVS_MEDIA_ADDR& stMediaAddrDst, ST_MEDIA_RSP& stMediaRsp)
{
	CHECK_POINTER(m_pTPMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//解析摄像头Code
	std::string strCameraCode;
	std::string strDomainCode;
	IVS_INT32 iRet = CToolsHelp::SplitCameraDomainCode(strCamera.c_str(), strCameraCode, strDomainCode);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TP Get RealPlay URL", "SplitCameraCode failed, CameraCode=%s", strCamera.c_str()); 
		return iRet;
	}

	//TODO : szDstPort无实际作用，传一个固定值；打包协议类型szDstPP入参结构体未定义
	ST_URL_MEDIA_INFO stUrlMediaInfo;
	memset(&stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));
	bool bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szSrcCode, sizeof(stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.cDomainCode, sizeof(stUrlMediaInfo.cDomainCode), strDomainCode.c_str(), strDomainCode.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szDstCode,sizeof(stUrlMediaInfo.szDstCode), "01",strlen("01"));
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	stUrlMediaInfo.ServiceType = SERVICE_TYPE_REALVIDEO;
	stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
	stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
	stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;
	stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
	stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	stUrlMediaInfo.PackProtocolType = PACK_PROTOCOL_ES;

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szDstIP,sizeof(stUrlMediaInfo.szDstIP), stMediaAddrDst.stIP.cIP, strnlen(stMediaAddrDst.stIP.cIP, IVS_IP_LEN));
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	std::string strPort = CToolsHelp::Int2Str(static_cast<int>(stMediaAddrDst.uiVideoPort));
	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szDstPort,sizeof(stUrlMediaInfo.szDstPort),  strPort.c_str() ,strPort.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	stUrlMediaInfo.iClientType = CLIENT_TYPE_PCG;

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szEncryAlgorithm,sizeof(stUrlMediaInfo.szEncryAlgorithm), "0000000000",strlen("0000000000"));
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	iRet = m_pTPMgr->GetURL(CToolsHelp::GetSCCHead().c_str(), stUrlMediaInfo, stMediaRsp);
	return iRet;
}

IVS_INT32 CTPPlayChannel::GetPlayBackURL(const std::string& strCamera, const TIME_SPAN_INTERNAL& stTimeSpanInter, const IVS_MEDIA_ADDR& stMediaAddrDst, ST_MEDIA_RSP& stMediaRsp)
{
	CHECK_POINTER(m_pTPMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//解析摄像头Code
	std::string strCameraCode;
	std::string strDomainCode;
	IVS_INT32 iRet = CToolsHelp::SplitCameraDomainCode(strCamera.c_str(), strCameraCode, strDomainCode);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get PlayBack URL", "SplitCameraCode failed, CameraCode=%s", strCamera.c_str()); 
		return iRet;
	}

	//TODO : szDstPort无实际作用，传一个固定值；打包协议类型szDstPP入参结构体未定义
	ST_URL_MEDIA_INFO stUrlMediaInfo;
	memset(&stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));
	bool bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szSrcCode, sizeof(stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.cDomainCode, sizeof(stUrlMediaInfo.cDomainCode), strDomainCode.c_str(), strDomainCode.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szDstCode,sizeof(stUrlMediaInfo.szDstCode), "01",strlen("01"));
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	stUrlMediaInfo.ServiceType = SERVICE_TYPE_PLAYBACK;
	stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
	stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
	stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;
	stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
	stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	stUrlMediaInfo.PackProtocolType = PACK_PROTOCOL_ES;

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szDstIP,sizeof(stUrlMediaInfo.szDstIP), stMediaAddrDst.stIP.cIP, strnlen(stMediaAddrDst.stIP.cIP, IVS_IP_LEN));
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	std::string strPort = CToolsHelp::Int2Str(static_cast<int>(stMediaAddrDst.uiVideoPort));
	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szDstPort,sizeof(stUrlMediaInfo.szDstPort),  strPort.c_str() ,strPort.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	std::string strTime;
	CMediaBase::UTCTime2Str((time_t)stTimeSpanInter.iStartTime, strTime, IVS_DATE_TIME_NET_FORMAT);
	strTime.append("-");
	std::string strEndTime;
	CMediaBase::UTCTime2Str((time_t)stTimeSpanInter.iEndTime, strEndTime, IVS_DATE_TIME_NET_FORMAT);   
	strTime.append(strEndTime);
	if (TIME_SPAN_LEN <= strTime.size())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PlayBack URL", "Parse time span from rsp url failed. time string over max length. strTime=%s.", strTime.c_str());
		return IVS_PARA_INVALID;
	}
	bSucc = CToolsHelp::Memcpy(stUrlMediaInfo.szTimeSpan,TIME_SPAN_LEN, strTime.c_str(), strTime.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	stUrlMediaInfo.iClientType = CLIENT_TYPE_PCG;

	bSucc = CToolsHelp::Strncpy(stUrlMediaInfo.szEncryAlgorithm,sizeof(stUrlMediaInfo.szEncryAlgorithm), "0000000000",strlen("0000000000"));
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	iRet = m_pTPMgr->GetURL(CToolsHelp::GetSCCHead().c_str(), stUrlMediaInfo, stMediaRsp);
	return iRet;
}


//暂停平台录像回放
IVS_INT32 CTPPlayChannel::PlatformPlayBackPauseByIP(IVS_VOID)
{
	IVS_DEBUG_TRACE("");
	//暂停rtsp
	IVS_INT32 iRet = RTSP_CLIENT_SendPauseMsg(m_iRtspHandle, m_strURL.c_str(), RTSP_CLIENT_TIMEOUT);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Play Back Pause", "RTSP client send pause play fail.");
		return IVS_PLAYER_RET_RTSP_SEND_PAUSE_FAIL;
	}

	return IVS_SUCCEED;
}


//恢复平台录像回放
IVS_INT32 CTPPlayChannel::PlatformPlayBackResumeByIP(IVS_VOID)
{
	IVS_DEBUG_TRACE("");
	//恢复rtsp
	if (RTSP_SESSION_STATUS_READY == RTSP_CLIENT_GetStatus(m_iRtspHandle))
	{
		float fSendScale = m_fRTSPSpeed;

		MEDIA_RANGE_S stRange;
		memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
		stRange.enRangeType = RANGE_TYPE_UTC;
		stRange.MediaBeginOffset = OFFSET_CUR;
		IVS_INT32 iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_strURL.c_str(), (double)fSendScale, &stRange, RTSP_CLIENT_TIMEOUT);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
			return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
		}
	}

	return IVS_SUCCEED;
}


// 设置平台录像码流回放速度
int CTPPlayChannel::SetPlayBackSpeedByIP(IVS_FLOAT fSpeed)
{
	IVS_DEBUG_TRACE("Speed: %f", fSpeed);

	RTSP_SESSION_STATUS state = RTSP_CLIENT_GetStatus(m_iRtspHandle);
	if (RTSP_SESSION_STATUS_PLAYING == state ||
		RTSP_SESSION_STATUS_READY   == state)
	{
		float fSendScale = fSpeed;
		(void)CToolsHelp::GetSendScale(SERVICE_TYPE_PLAYBACK, fSpeed, fSendScale);
		MEDIA_RANGE_S stMdieaRange;
		memset(&stMdieaRange, 0x0, sizeof(MEDIA_RANGE_S));
		stMdieaRange.enRangeType = RANGE_TYPE_NPT;
		stMdieaRange.MediaBeginOffset = OFFSET_CUR;

		IVS_INT32 iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_strURL.c_str(), (double)fSendScale, &stMdieaRange, RTSP_CLIENT_TIMEOUT);
		if (IVS_SUCCEED != iRet)
		{  
			(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_strURL.c_str(), 1);
			(void)RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			BP_RUN_LOG_ERR(iRet, "Set PlayBack Speed", "Send Play Message failed.");
			return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
		}
		m_fRTSPSpeed = fSendScale;
		
        BP_RUN_LOG_INF("Set PlayBack Speed", "m_fRTSPSpeed:[%f], fSpeed:[%f].", m_fRTSPSpeed, fSpeed);
		return IVS_SUCCEED;
	}

	BP_RUN_LOG_ERR(IVS_FAIL, "Set PlayBack Speed", "fSpeed:[%f], RTSP Session Status: %d", fSpeed, state);
	return IVS_FAIL;
}

IVS_INT32 CTPPlayChannel::GetPlayBackTimeSpan(const std::string& strURL, 
	TIME_SPAN_INTERNAL& stTimeSpan)
{
	//从URL中获取时间戳
	std::string strTimeSpan;
	IVS_UINT32 uiPos = m_strURL.find("TimeSpan=");
	if (std::string::npos == uiPos || uiPos >= m_strURL.size())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get PlayBack TimeSpan", "Can't find TimeSpan from URL");
		return IVS_FAIL;
	}
	strTimeSpan = m_strURL.substr(uiPos, m_strURL.size() - uiPos);

	uiPos = strTimeSpan.find("&");
	if (std::string::npos == uiPos)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get PlayBack TimeSpan", "Cant't find &, URL format error");
		return IVS_FAIL;
	}
	strTimeSpan = strTimeSpan.substr(strlen("TimeSpan="), uiPos - strlen("TimeSpan="));

	uiPos = strTimeSpan.find("-");
	if (std::string::npos == uiPos)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get PlayBack TimeSpan", "Can't find -, URL format error");
		return IVS_FAIL;
	}

	std::string strStartTime = strTimeSpan.substr(0, uiPos);
	std::string strEndTime = strTimeSpan.substr(uiPos + 1, strlen(IVS_DATE_TIME_NET_FORMAT));

	m_stTimeSpanInter.iStartTime = (IVS_INT32)CMediaBase::UTCStr2Time(strStartTime.c_str(), IVS_DATE_TIME_NET_FORMAT);
	m_stTimeSpanInter.iEndTime = (IVS_INT32)CMediaBase::UTCStr2Time(strEndTime.c_str(), IVS_DATE_TIME_NET_FORMAT);
	BP_RUN_LOG_INF("Get PlayBack TimeSpan", "strStartTime=%s strEndTime=%s iStartTime=%d iEndTime=%d.", 
		strStartTime.c_str(), strEndTime.c_str(), m_stTimeSpanInter.iStartTime, m_stTimeSpanInter.iEndTime);

	return IVS_SUCCEED;
}

//设置平台录像码流回放时间
IVS_INT32 CTPPlayChannel::SetPlayBackTimeByIP(IVS_UINT32 uiTime)
{
	IVS_INT32 iRet = GetPlayBackTimeSpan(m_strURL, m_stTimeSpanInter);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Set PlayBack Time ByIP", "Get PlayBack TimeSpan failed");
		return iRet;
	}

	IVS_UINT32 uiTime2Set = (unsigned int)m_stTimeSpanInter.iStartTime + uiTime;
	if (uiTime2Set >= (unsigned int)m_stTimeSpanInter.iEndTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, 
			"Set PlayBack Time ByIP", 
			"Param invalid, uiTime is over end time, startTime = %d, end time = %d, set Time = %d", 
			m_stTimeSpanInter.iStartTime, 
			m_stTimeSpanInter.iEndTime, 
			uiTime);
		return IVS_PARA_INVALID;
	}

	BP_RUN_LOG_INF("Set PlayBack Time ByIP", "RTSP_CLIENT_SendPlayMsg begin, rtsphandle=%d, speed=%f, settime=%u.", m_iRtspHandle, m_fRTSPSpeed, uiTime);

	MEDIA_RANGE_S stRange;
	memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
	stRange.enRangeType      = RANGE_TYPE_NPT;
	stRange.MediaBeginOffset = uiTime;
	
	iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_strURL.c_str(), (double)m_fRTSPSpeed, &stRange, RTSP_CLIENT_TIMEOUT);

	BP_RUN_LOG_INF("Set PlayBack Time ByIP", "RTSP_CLIENT_SendPlayMsg end, iRet=%d.", iRet);

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL,"Set PlayBack Time ByIP", "RTSP_CLIENT_SendPlayMsg failed.");
		return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
	}

	return IVS_SUCCEED;
}