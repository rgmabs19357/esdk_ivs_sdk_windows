#include "RDAChannel.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "IVS_TVWall.h"
#include "TVWallMgr.h"
#include "MediaBase.h"

CRDAChannel::CRDAChannel(IVS_ULONG ulAsyncSeq, 
						const IVS_CHAR* pDecoderID, 
						IVS_UINT32 uiChannel,
						CTVWallMgr* pTVWallMgr) 
						: m_ulBusinessID(0)
						, m_ucTransportProtocol(0)
						, CDecoderChannel(ulAsyncSeq, pDecoderID, uiChannel, pTVWallMgr)
{
}

CRDAChannel::CRDAChannel() : m_ulBusinessID(0)
							, m_ucTransportProtocol(0)
{

}
CRDAChannel::~CRDAChannel(void)
{

}

IVS_INT32 CRDAChannel::InitChannel(IVS_UINT32 uiProtocolType, MEDIA_ADDR& stDecoderAddr)
{
	// 初始化解码器通道
	IPI_INIT_BUSINESS_REQ pParaInfo = {0};
	pParaInfo.ulAsyncSeq = m_ulAsyncSeq;
	pParaInfo.usChannelNo = static_cast<unsigned short>(m_uiChannel);
	bool bSucc = CToolsHelp::Strncpy(pParaInfo.szDecoderID, sizeof(pParaInfo.szDecoderID), m_strDecoderID.c_str(), m_strDecoderID.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);
	pParaInfo.ucTransportProtocol = (1==uiProtocolType) ? RDA_PROTOCOL_RTP_OVER_UDP:RDA_PROTOCOL_RTP_OVER_TCP;
	m_ucTransportProtocol = pParaInfo.ucTransportProtocol;
	IVS_INT32 iRet = IVS_TVWall_InitBusiness(&pParaInfo);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Init Channel", "Init Business failed!");
		return iRet;
	}

    BP_DBG_LOG("CRDAChannel::InitChannel: Send Init Channel Info Suc. usChannelNo = %d.", pParaInfo.usChannelNo);

	//获取解码器初始化信息，要从中取出媒体IP等信息
	CUMW_NOTIFY_INIT_DECODER_INFO decoderInitInfo = {0};
	for(int i=0;i<90;i++)
	{
        //检查是否有停止请求
        CHECK_STOP_FLAG_FOR_START();

		iRet = m_pTVWallMgr->GetDecoderInitInfo(m_ulAsyncSeq, &decoderInitInfo);
		if(IVS_SUCCEED==iRet)
		{
			m_ulBusinessID = decoderInitInfo.ulBusinessID;
			break;
		}
		VOS_Sleep(70);
	}
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Init Channel", "Get Decoder Init Info Failed!");
		HandlePlayException();
		return iRet;
	}

    //检查解码器返回的数据是否为成功
    if (IVS_SUCCEED != decoderInitInfo.stErrorCode.ulErrorCode)
    {
        BP_RUN_LOG_ERR(IVS_TVWALL_DECODER_INIT_CHANNEL_FAIL, "Init Channel", "Init Business failed!");
		(void)StopChannel();
		HandlePlayException();
        return IVS_TVWALL_DECODER_INIT_CHANNEL_FAIL;
    }

	stDecoderAddr.unMediaIp = htonl((unsigned int)inet_addr(decoderInitInfo.stMediaNetInfo.szMediaIP));
	if(decoderInitInfo.stMediaNetInfo.ucTransportProtocol==RDA_PROTOCOL_RTP_OVER_TCP)
	{
		stDecoderAddr.unMediaVideoPort = decoderInitInfo.stMediaNetInfo.usTcpInterleavedPort;
		stDecoderAddr.unMediaAudioPort = decoderInitInfo.stMediaNetInfo.usTcpInterleavedPort;
	}
	else
	{
		stDecoderAddr.unMediaVideoPort = decoderInitInfo.stMediaNetInfo.usUdpVideoRtpPort;
		stDecoderAddr.unMediaAudioPort = decoderInitInfo.stMediaNetInfo.usUdpAudioRtpPort;
	}
	BP_DBG_LOG("CRDAChannel::InitChannel, get DecoderInitInfo success: \
			   DecoderID=%s,TCPPort=%d,UDPVideoPort=%d,UDPAudioPort=%d", 
			   decoderInitInfo.szDecoderID,
			   decoderInitInfo.stMediaNetInfo.usTcpInterleavedPort, 
			   decoderInitInfo.stMediaNetInfo.usUdpVideoRtpPort,
			   decoderInitInfo.stMediaNetInfo.usUdpAudioRtpPort);
	return IVS_SUCCEED;
}

IVS_INT32 CRDAChannel::StartChannel(const std::string strURL, const MEDIA_ADDR stSourceMediaAddr)
{
	// 命令远程解码器开始解码
	IPI_START_BUSINESS_REQ stStartParaInfo = {0};
	stStartParaInfo.ulAsyncSeq = m_ulAsyncSeq;
	stStartParaInfo.ulBusinessID = m_ulBusinessID;
	stStartParaInfo.usChannelNo = (unsigned short)m_uiChannel;
	//99为H264 ，8为G711A
	stStartParaInfo.ucVideoPayloadType = PAY_LOAD_TYPE_H264;
	stStartParaInfo.ucAudioPayloadType = PAY_LOAD_TYPE_PCMA;
	stStartParaInfo.stMediaNetInfo.ucTransportProtocol = m_ucTransportProtocol;

	// IP地址需要进行网络需转换
	std::string strServiceIP = "";
	if (IVS_SUCCEED != CMediaBase::AddrUint2Char(ntohl(stSourceMediaAddr.unMediaIp), strServiceIP))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Channel","Parse IP Failed!");  
		return IVS_PARA_INVALID;
	}

	bool bSucc = CToolsHelp::Strncpy(stStartParaInfo.szDecoderID,sizeof(stStartParaInfo.szDecoderID),m_strDecoderID.c_str(),m_strDecoderID.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	bSucc = CToolsHelp::Strncpy(stStartParaInfo.stMediaNetInfo.szMediaIP,sizeof(stStartParaInfo.stMediaNetInfo.szMediaIP),strServiceIP.c_str(),strServiceIP.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	int iPosURL = (int)strURL.find("?");

	bSucc = CToolsHelp::Strncpy(stStartParaInfo.szSessionUrl,sizeof(stStartParaInfo.szSessionUrl), strURL.substr(0,(unsigned int)iPosURL).c_str(),(unsigned int)iPosURL);
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);
	//int iPosID = (int)strURL.rfind("/",(unsigned int)iPosURL);
	//bSucc = CToolsHelp::Strncpy(stStartParaInfo.szSessionId,sizeof(stStartParaInfo.szSessionId), strURL.substr((unsigned int)iPosID+1,(unsigned int)iPosURL).c_str(),(unsigned int)((iPosURL-iPosID)-1));
	//CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);
	std::string strSessionID;
	IVS_INT32 iRet = ParseURL(strURL.c_str(), "Token=", strSessionID);
	if (IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start Channel", "Parse TOKEN type failed from url, AsyncSeq=%lu", m_ulAsyncSeq);
		return iRet;
	}
	bSucc = CToolsHelp::Strncpy(stStartParaInfo.szSessionId,sizeof(stStartParaInfo.szSessionId), strSessionID.c_str(),strSessionID.length());
	CHECK_SUCCESS_RET(bSucc, IVS_ALLOC_MEMORY_ERROR);

	stStartParaInfo.stMediaNetInfo.usTcpInterleavedPort = stSourceMediaAddr.unMediaVideoPort;
	stStartParaInfo.stMediaNetInfo.usUdpVideoRtpPort = stSourceMediaAddr.unMediaVideoPort;
	stStartParaInfo.stMediaNetInfo.usUdpVideoRtcpPort = (unsigned short)(stSourceMediaAddr.unMediaVideoPort + 1);
	stStartParaInfo.stMediaNetInfo.usUdpAudioRtpPort = stSourceMediaAddr.unMediaAudioPort;
	stStartParaInfo.stMediaNetInfo.usUdpAudioRtcpPort = (unsigned short)(stSourceMediaAddr.unMediaAudioPort + 1);

	iRet = IVS_TVWall_StartBusiness(&stStartParaInfo);
	return iRet;
}

IVS_INT32 CRDAChannel::StopChannel()
{
	IPI_STOP_BUSINESS_REQ stStopParaInfo = {0};
	stStopParaInfo.ulAsyncSeq = m_ulAsyncSeq;
	stStopParaInfo.usChannelNo = (unsigned short)m_uiChannel;
	stStopParaInfo.ulBusinessID = m_ulBusinessID;
	if(!CToolsHelp::Strncpy(stStopParaInfo.szDecoderID, sizeof(stStopParaInfo.szDecoderID), m_strDecoderID.c_str(), m_strDecoderID.length()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Stop Channel", "Copy Decoder ID Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)IVS_TVWall_StopBusiness(&stStopParaInfo);
	return IVS_SUCCEED;
}

// 开启通道声音
IVS_INT32 CRDAChannel::StartChannelSound()
{
    /*
	IPI_AUDIO_CONTROL_REQ stAudioInfo = {0};
	stAudioInfo.ulAsyncSeq = m_ulAsyncSeq;
	stAudioInfo.usChannelNo = (unsigned short)m_uiChannel;
	stAudioInfo.ulBusinessID = m_ulBusinessID;
	if(!CToolsHelp::Strncpy(stAudioInfo.szDecoderID, sizeof(stAudioInfo.szDecoderID), m_strDecoderID.c_str(), m_strDecoderID.length()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Start Channel Sound", "Copy Decoder ID Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	stAudioInfo.cAudioCmd = 0x01;
	IVS_INT32 iRet = IVS_TVWall_PlayAudio(&stAudioInfo);
	return iRet;
    */
    return IVS_SYS_NO_SUPPORT;
}
// 停止通道声音
IVS_INT32 CRDAChannel::StopChannelSound()
{
    /*
	IPI_AUDIO_CONTROL_REQ stAudioInfo = {0};
	stAudioInfo.ulAsyncSeq = m_ulAsyncSeq;
	stAudioInfo.usChannelNo = (unsigned short)m_uiChannel;
	stAudioInfo.ulBusinessID = m_ulBusinessID;
	if(!CToolsHelp::Strncpy(stAudioInfo.szDecoderID, sizeof(stAudioInfo.szDecoderID), m_strDecoderID.c_str(), m_strDecoderID.length()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Stop Channel Sound", "Copy Decoder ID Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	stAudioInfo.cAudioCmd = 0x00;
	IVS_INT32 iRet = IVS_TVWall_PlayAudio(&stAudioInfo);
	return iRet;
    */
    return IVS_SYS_NO_SUPPORT;
}
// 设置通道声音音量
IVS_INT32 CRDAChannel::SetChannelVolume(IVS_UINT32 /*uiVolumeValue*/)
{
    /*
	IPI_TUNEUP_VOLUME_REQ stVolumeInfo = {0};
	stVolumeInfo.ulAsyncSeq = m_ulAsyncSeq;
	stVolumeInfo.usChannelNo = (unsigned short)m_uiChannel;
	stVolumeInfo.ulBusinessID = m_ulBusinessID;
	if(!CToolsHelp::Strncpy(stVolumeInfo.szDecoderID, sizeof(stVolumeInfo.szDecoderID), m_strDecoderID.c_str(), m_strDecoderID.length()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Set Channel Volume", "Copy Decoder ID Failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	stVolumeInfo.cVolume = static_cast<unsigned char>(uiVolumeValue);
	IVS_INT32 iRet = IVS_TVWall_SetVolume(&stVolumeInfo);
	return iRet;
    */
    return IVS_SYS_NO_SUPPORT;
}
