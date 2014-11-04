#include "StdAfx.h"
#include "Business.h"
//#include "RDAProt.h"

CBusiness::CBusiness(): m_ulChannelId(0)//远程解码器的通道编号
						,m_enVideoPt (CUMW::PAY_LOAD_TYPE_H264)//视频PT
						,m_enAudioPt (CUMW::PAY_LOAD_TYPE_PCMA)//音频PT
						,m_ulStartTime(0)//开始时间
						,m_ulEndTime(0)//结束时间
						,m_ulAsyncSeq(0)//异步序号
						,m_bReusing(FALSE)//是否复用
						,m_ulSharedBusinessId(0)//共享媒体流的业务ID
						,m_bIngressStart(FALSE)//Ingress是否已启动
						,m_ulInitBusinessId(0)
						,m_enBusinessSrcType(CUMW::BUSINESS_TYPE_MASK_NONE)//业务来源类型
						,m_enBusinessDstType(CUMW::BUSINESS_TYPE_MASK_NONE)//业务目标类型
						,m_ulLinkMode(CUMW_LINK_MODE_TRANSMIT)  //连接模式
						,m_ucProtocolType(RDA_PROTOCOL_RTP_OVER_UDP)   //协议类型
						,m_ulBusinessId(0)//业务ID

{
    // 远程解码器ID
    memset(m_szDecoderId, 0, sizeof(m_szDecoderId));

    //媒体服务器IP
    memset(m_szServerIp, 0, sizeof(m_szServerIp));

    //媒体本地IP
    memset(m_szLocalIp, 0, sizeof(m_szLocalIp));

    //解码器IP
    memset(m_szDecoderIp, 0, sizeof(m_szDecoderIp));

    //媒体分发IP
    memset(m_szMediaDistributeIp, 0, sizeof(m_szMediaDistributeIp));

    //解码器端口信息
    memset(&m_stDecoderPortInfo, 0, sizeof(m_stDecoderPortInfo));

    //媒体分发端口信息
    memset(&m_stMediaDistributePortInfo, 0, sizeof(m_stMediaDistributePortInfo));

    //媒体服务器端口信息
    memset(&m_stServerPortInfo, 0, sizeof(m_stServerPortInfo));

    //本地端口信息
    memset(&m_stLocalPortInfo, 0, sizeof(m_stLocalPortInfo));

    //会话URL
    memset(m_szSessionUrl, 0, sizeof(m_szSessionUrl));

    //媒体会话ID
    memset(m_szSessionId, 0, sizeof(m_szSessionId));
	//播放/停止播放音频消息
	m_cAudioCmd = 0;
	//音量大小
	m_cVolume = 0;
	//帧率
	m_ulFrameRate = 0;
}

CBusiness::~CBusiness()
{}

CUMW::BUSINESS_TYPE CBusiness::GetBusinessType() const
{
    //业务来源类型与目标类型的组合
    return (CUMW::BUSINESS_TYPE)(m_enBusinessSrcType | m_enBusinessDstType);
}

CUMW::BUSINESS_TYPE_MASK CBusiness::GetBusinessSrcType() const
{
    return m_enBusinessSrcType;
}

CUMW::BUSINESS_TYPE_MASK CBusiness::GetBusinessDstType() const
{
    return m_enBusinessDstType;
}

BOOL CBusiness::SetLinkMode
(   IN unsigned long ulLinkMode)
{
    //参数检查
    if (CUMW_LINK_MODE_MAX <= ulLinkMode)
    {
        IVS_RUN_LOG_ERR( "Set Link Mode Failed. Link Mode Value(%lu) Invalidate.", ulLinkMode);
        return FALSE;
    }

    //保存合法值
    m_ulLinkMode = ulLinkMode;

    return TRUE;
}

unsigned long CBusiness::GetLinkMode() const
{
    return m_ulLinkMode;
}

BOOL CBusiness::SetProtocolType
(   IN unsigned char ulProtocolType)
{
    //参数检查
    if (RDA_PROTOCOL_MAX <= ulProtocolType)
    {
        IVS_RUN_LOG_ERR( "Set Protocol Type Failed. Protocol Type Value(%lu) Invalidate.", ulProtocolType);
        return FALSE;
    }

    //保存合法值
    m_ucProtocolType = ulProtocolType;

    return TRUE;
}

unsigned char CBusiness::GetProtocolType() const
{
    return m_ucProtocolType;
}

unsigned char CBusiness::GetDecodeProtocolType() const
{
    //非直连上墙，强制使用TCP
    /*if (CUMW::BUSINESS_TYPE_MASK_DST_REMOTE == m_enBusinessDstType)
    {
        return CUMW_PROTOCOL_TYPE_TCP;
    }*/

    return m_ucProtocolType;
}

BOOL CBusiness::SetPlayType
(   IN unsigned long ulPlayType)
{
    BOOL bResult = TRUE;

    //根据播放类型，设置业务目标类型
    switch (ulPlayType)
    {
    case CUMW_PLAY_TYPE_REMOTE: //上墙显示
    {
        m_enBusinessDstType = CUMW::BUSINESS_TYPE_MASK_DST_REMOTE;
        break;
    }

    case CUMW_PLAY_TYPE_PREVIEW: //上墙预览
    {
        m_enBusinessDstType = CUMW::BUSINESS_TYPE_MASK_DST_PREVIEW;
        break;
    }

    case CUMW_PLAY_TYPE_REMOTE_DIRECT_CONNECT:  //远程上墙直连视频源，可以是MDU或PU
    {
        m_enBusinessDstType = CUMW::BUSINESS_TYPE_MASK_DST_REMOTE_DIRECT_CONNECT;
        break;
    }

    default:
    {
        IVS_RUN_LOG_ERR( "Set Play Type Failed. Play Type Value(%lu) Invalidate.", ulPlayType);
        bResult = FALSE;
        break;
    }
    }

    return bResult;
}

const char* CBusiness::GetBusinessTypeString() const
{
    const char* pszResult = NULL;

    //unsigned long ulType = GetBusinessType();

    return pszResult;
}

void CBusiness::GetOpenMediaChannelPara
(   IO CUMW::OPEN_MEDIA_CHANNEL_PARA& stPara) const
{
    //设置公共值
    stPara.ulAsyncSeq   = m_ulAsyncSeq;
    stPara.ulBusinessId = m_ulBusinessId;
    stPara.ulBusinessSrcType = m_enBusinessSrcType;
    stPara.ulLinkMode = m_ulLinkMode;
    stPara.ulProtocolType  = m_ucProtocolType;
    stPara.stLocalPortInfo = m_stLocalPortInfo;
    stPara.ulStartTime = m_ulStartTime;
    stPara.ulEndTime = m_ulEndTime;

    if(!CToolsHelp::Strncpy(stPara.szLocalIp, IVS_IP_LEN, m_szLocalIp, IVS_IP_LEN - 1))
	{
		IVS_RUN_LOG_ERR("Get Open Media Channel Para Copy LocalIp failed");
	}
}

void CBusiness::SetServerMediaInfo
(   IN const CUMW::SERVER_MEDIA_INFO& stSvrMediaInfo)
{
    m_stServerPortInfo = stSvrMediaInfo.stServerPortInfo;
    m_enVideoPt = stSvrMediaInfo.enVideoPt;
    m_enAudioPt = stSvrMediaInfo.enAudioPt;

    if(!CToolsHelp::Strncpy(m_szServerIp, sizeof(m_szServerIp), stSvrMediaInfo.szServerIp, sizeof(m_szServerIp) - 1))
	{
		IVS_RUN_LOG_ERR("Set Server Media Info Copy SeverIp failed");
	}
    if(!CToolsHelp::Strncpy(m_szSessionUrl, sizeof(m_szSessionUrl), stSvrMediaInfo.szSessionUrl, sizeof(m_szSessionUrl) - 1))
	{
		IVS_RUN_LOG_ERR("Set Server Media Info Copy SessionUrl failed");
	}
    if(!CToolsHelp::Strncpy(m_szSessionId, sizeof(m_szSessionId), stSvrMediaInfo.szSessionId, sizeof(m_szSessionId) - 1))
	{
		IVS_RUN_LOG_ERR("Set Server Media Info Copy SessionId failed");
	}
}

void CBusiness::CopyServerMediaInfo
(   IN const CBusiness& objSharedBusiness)
{
    //保存复用的最原始业务ID
    m_ulSharedBusinessId = objSharedBusiness.m_ulSharedBusinessId;

    m_stServerPortInfo = objSharedBusiness.m_stServerPortInfo;
    m_enVideoPt = objSharedBusiness.m_enVideoPt;
    m_enAudioPt = objSharedBusiness.m_enAudioPt;

    if(!CToolsHelp::Strncpy(m_szServerIp, sizeof(m_szServerIp), objSharedBusiness.m_szServerIp, sizeof(m_szServerIp) - 1))
	{
		IVS_RUN_LOG_ERR("Copy Server Media Info Copy SeverIp failed");
	}
    if(!CToolsHelp::Strncpy(m_szSessionUrl, sizeof(m_szSessionUrl), objSharedBusiness.m_szSessionUrl, sizeof(m_szSessionUrl) - 1))
	{
		IVS_RUN_LOG_ERR("Copy Server Media Info Copy SessionUrl failed");
	}
    if(!CToolsHelp::Strncpy(m_szSessionId, sizeof(m_szSessionId), objSharedBusiness.m_szSessionId, sizeof(m_szSessionId) - 1))
	{
		IVS_RUN_LOG_ERR("Copy Server Media Info Copy SessionId failed");
	}
}

void CBusiness::SetBusinessId
(   IN unsigned long ulBusinessId)
{
    m_ulBusinessId = ulBusinessId;

    //保存最原始业务ID
    m_ulSharedBusinessId = ulBusinessId;
}

unsigned long CBusiness::GetBusinessId() const
{
    return m_ulBusinessId;
}

BOOL CBusiness::IsRemoteBusiness() const
{
    BOOL bResult = FALSE;

    switch ((unsigned long)m_enBusinessDstType)
    {
    case CUMW::BUSINESS_TYPE_MASK_DST_REMOTE:
    case CUMW::BUSINESS_TYPE_MASK_DST_REMOTE_DIRECT_CONNECT:
    {
        bResult = TRUE;
        break;
    }

    default:
    {
		IVS_DBG_LOG("is not Remote Business");
        bResult = FALSE;
        break;
    }
    }

    return bResult;
}

BOOL CBusiness::IsRemoteDirectConnect() const
{
    BOOL bResult = FALSE;

    if (CUMW::BUSINESS_TYPE_MASK_DST_REMOTE_DIRECT_CONNECT == m_enBusinessDstType)
    {
        bResult = TRUE;
    }

    return bResult;
}
