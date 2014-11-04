#include "StdAfx.h"
#include "CumwCommon.h"
#include "RDAProt.h"
#include "RDAProtStack.h"

CRDAMsg::CRDAMsg() : m_pRDAMsgAttrMgr(NULL)
{
    memset(&m_stMsgHead, 0x00, sizeof(m_stMsgHead));
    memset(m_szRDAMsgTypeName, 0x00, LEN_RDA_MSGTYPE_NAME);
}

CRDAMsg::~CRDAMsg()
{
    VOS_DELETE(m_pRDAMsgAttrMgr); /*lint !e1551*/ //经过确认VOS_DELETE中有保护    
}

LONG CRDAMsg::Init()
{
    (void)VOS_NEW(m_pRDAMsgAttrMgr);
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAMsg Init, RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    LONG lRet = this->RegisterAttrs();
    if (VOS_SUCCESS != lRet)
    {
        IVS_RUN_LOG_ERR("RDAMsg Init, RegisterAttrs fail ret(0x%04x).", lRet);
        return VOS_FAIL;
    }

    return VOS_SUCCESS;
}

long CRDAMsg::DecodeBody(const char * pBuffer, const ULONG ulLength)
{
    if ((NULL == pBuffer) || (NULL == m_pRDAMsgAttrMgr))
    {
        IVS_RUN_LOG_ERR("RDAMsg DecodeBody fail. Input Param is Invalid");
        return VOS_FAIL;
    }

    LONG lRet = m_pRDAMsgAttrMgr->DecodeBody(pBuffer, ulLength);
    if (VOS_SUCCESS != lRet)
    {
        IVS_RUN_LOG_ERR("RDAMsg Decode fail, errorcode(0x%04x)", lRet);
        return VOS_FAIL;
    }

    return VOS_SUCCESS;
}

long CRDAMsg::EncodeBody(char *pBuffer, const ULONG ulMaxLength,
                         ULONG &ulUsedLength)
{
    if ((NULL == pBuffer) || (NULL == m_pRDAMsgAttrMgr))
    {
        IVS_RUN_LOG_ERR("EncodeBody Input is invalid.");
        return VOS_FAIL;
    }

    LONG lRet = m_pRDAMsgAttrMgr->EncodeBody(pBuffer,
                                             ulMaxLength, ulUsedLength);
    if (VOS_SUCCESS != lRet)
    {
        IVS_RUN_LOG_ERR("RDAMsg Encode fail.errorcode(0x%04x)", lRet);
        return VOS_FAIL;
    }

    return VOS_SUCCESS;
}

VOID CRDAMsg::Print()
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAMsg Print RDAMsgAttrMgr is NULL");
        return;
    }

    ;

    IVS_RUN_LOG_INF("%s", m_szRDAMsgTypeName);

    m_pRDAMsgAttrMgr->PrintBody();
}

long CRDAMsg::RegisterAttrs( )
{
    return VOS_FAIL;
}

CRDARegisteReq::CRDARegisteReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_REGISTER_REQ;

	if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_REGISTE_REQ, LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    memset(m_szDecoderID, 0, sizeof(m_szDecoderID));
    memset(m_szDecoderName, 0, sizeof(m_szDecoderName));
    memset(&m_stChannelsInfo, 0, sizeof(m_stChannelsInfo));
    memset(&m_stVideoPayloadTypes, 0, sizeof(m_stVideoPayloadTypes));
    memset(&m_stAudioPayloadTypes, 0, sizeof(m_stAudioPayloadTypes));
	memset(m_szDecoderEncoding, 0, sizeof(m_szDecoderEncoding));
}

LONG CRDARegisteReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDARegisteReq RegisterAttrs RDAMsgAttrMgr is NULL.");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderName, LEN_RDA_NAME,
                                   RDA_TAG_NAME_DECODERNAME, enRDA_STRING_ID, (char*)m_szDecoderName);

	m_pRDAMsgAttrMgr->RegisterAttr(TagEncoding, LEN_RDA_ENCODING,
		RDA_TAG_NAME_ENCODING, enRDA_STRING_ID, (char*)m_szDecoderEncoding);

    if (MAX_PAYLOAD_TYPE_NUM < m_stVideoPayloadTypes.ucPayloadTypeNum)
    {
        IVS_RUN_LOG_ERR("RDARegisteReq VideoPayloadType Num is larger than %d.", MAX_PAYLOAD_TYPE_NUM);
        return VOS_FAIL;
    }

    const unsigned short usVideoPtLen = sizeof(m_stVideoPayloadTypes)
                                        - (MAX_PAYLOAD_TYPE_NUM - m_stVideoPayloadTypes.ucPayloadTypeNum)
                                        * sizeof(m_stVideoPayloadTypes.stPayloadTypeArry[0]);

    m_pRDAMsgAttrMgr->RegisterAttr(TAG_VIDEO_PAY_LOAD_TYPES, usVideoPtLen,
                                   RDA_TAG_NAME_VIDEO_PAY_LOAD_TYPES, RDA_TYPE_PAY_LOAD_TYPES_INFO,
                                   (char*)&m_stVideoPayloadTypes);

    if (MAX_PAYLOAD_TYPE_NUM < m_stAudioPayloadTypes.ucPayloadTypeNum)
    {
        IVS_RUN_LOG_ERR("RDARegisteReq AudioPayloadType Num is larger than %d.", MAX_PAYLOAD_TYPE_NUM);
        return VOS_FAIL;
    }

    const unsigned short usAudioPtLen = sizeof(m_stAudioPayloadTypes)
                                        - (MAX_PAYLOAD_TYPE_NUM - m_stAudioPayloadTypes.ucPayloadTypeNum)
                                        * sizeof(m_stAudioPayloadTypes.stPayloadTypeArry[0]);

    m_pRDAMsgAttrMgr->RegisterAttr(TAG_AUDIO_PAY_LOAD_TYPES, usAudioPtLen,
                                   RDA_TAG_NAME_AUDIO_PAY_LOAD_TYPES, RDA_TYPE_PAY_LOAD_TYPES_INFO,
                                   (char*)&m_stAudioPayloadTypes);

    USHORT usAttrLength = (USHORT)sizeof(RDA_UINT2) +
                          (USHORT)((m_stChannelsInfo.usChannelNum) * (sizeof(RDA_UINT2) + LEN_RDA_NAME));

    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelsInfo, usAttrLength,
                                   RDA_TAG_NAME_CHANNELSINFO, enRDA_CHANNELS_INFO, (char*)&m_stChannelsInfo);

    return VOS_SUCCESS;
}

CRDARegisteRsp::CRDARegisteRsp()
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_REGISTER_RSP;

	if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_REGISTE_RSP,LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}


    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

LONG CRDARegisteRsp::RegisterAttrs(void)
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDARegisteRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDAInitBusinessReq::CRDAInitBusinessReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_INIT_BUSINESS_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_INIT_BUSINESS_REQ,
		LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}


    m_ulBusinessID = 0;
    m_usChannelNo = 0;
    m_ucTransportProtocol = RDA_PROTOCOL_RTP_OVER_TCP;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

long CRDAInitBusinessReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAInitBusinessReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagBusinessID, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_BUSINESSID, enRDA_UINT4, (char*)&m_ulBusinessID );
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelNo, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_CHANNELNO, enRDA_UINT2, (char*)&m_usChannelNo);
    m_pRDAMsgAttrMgr->RegisterAttr(TAG_TRANSPORT_PROTOCOL, sizeof(RDA_UCHAR),
                                   RDA_TAG_NAME_TRANSPROT_PROTOCOL, enRDA_UCHAR, (char*)&m_ucTransportProtocol);

    return VOS_SUCCESS;
}

CRDAInitBusinessRsp::CRDAInitBusinessRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_INIT_BUSINESS_RSP;

	if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_INIT_BUSINESS_RSP, LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));

    m_ucTransportProtocol  = RDA_PROTOCOL_RTP_OVER_TCP;
    m_usTcpInterleavedPort = 0;
    m_usUdpVideoRtpPort  = 0;
    m_usUdpVideoRtcpPort = 0;
    m_usUdpAudioRtpPort  = 0;
    m_usUdpAudioRtcpPort = 0;
    memset(m_szMediaIP, 0, sizeof(m_szMediaIP));
}

long CRDAInitBusinessRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAInitBusinessRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    //m_pRDAMsgAttrMgr->RegisterAttr(TAG_TRANSPORT_PROTOCOL, sizeof(RDA_UCHAR),
    //    RDA_TAG_NAME_TRANSPROT_PROTOCOL, enRDA_UCHAR, (char*)&m_ucTransportProtocol);
    m_pRDAMsgAttrMgr->RegisterAttr(TAG_MEDIA_IP, (USHORT)strlen(m_szMediaIP),
                                   RDA_TAG_NAME_MEDIA_IP, enRDA_STRING, (char*)m_szMediaIP);

    if (RDA_PROTOCOL_RTP_OVER_TCP == m_ucTransportProtocol)
    {
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_TCP_INTERLEAVED_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_TCP_INTERLEAVED_PORT, enRDA_UINT2, (char*)&m_usTcpInterleavedPort);
    }
    else
    {
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_VIDEO_RTP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_VIDEO_RTP_PORT, enRDA_UINT2, (char*)&m_usUdpVideoRtpPort);
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_VIDEO_RTCP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_VIDEO_RTCP_PORT, enRDA_UINT2, (char*)&m_usUdpVideoRtcpPort);
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_AUDIO_RTP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_AUDIO_RTP_PORT, enRDA_UINT2, (char*)&m_usUdpAudioRtpPort);
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_AUDIO_RTCP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_AUDIO_RTCP_PORT, enRDA_UINT2, (char*)&m_usUdpAudioRtcpPort);
    }

    return VOS_SUCCESS;
}

CRDAStartBusinessReq::CRDAStartBusinessReq()
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_START_BUSINESS_REQ;

	if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_START_BUSINESS_REQ, LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}


    m_ulBusinessID = 0;
    m_usChannelNo = 0;
    m_ucTransportProtocol  = RDA_PROTOCOL_RTP_OVER_TCP;
    m_usTcpInterleavedPort = 0;
    m_usUdpVideoRtpPort  = 0;
    m_usUdpVideoRtcpPort = 0;
    m_usUdpAudioRtpPort  = 0;
    m_usUdpAudioRtcpPort = 0;
    m_ucVideoPayloadType = 0;
    m_ucAudioPayloadType = 0;

    memset(m_szDecoderID, 0, sizeof(m_szDecoderID));
    memset(m_szMediaIP, 0, sizeof(m_szMediaIP));
    memset(m_szSessionId, 0, sizeof(m_szSessionId));
    memset(m_szSessionUrl, 0, sizeof(m_szSessionUrl));
}

long CRDAStartBusinessReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAStartBusinessReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagBusinessID, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_BUSINESSID, enRDA_UINT4, (char*)&m_ulBusinessID );
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelNo, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_CHANNELNO, enRDA_UINT2, (char*)&m_usChannelNo);

    m_pRDAMsgAttrMgr->RegisterAttr(TAG_TRANSPORT_PROTOCOL, sizeof(RDA_UCHAR),
                                   RDA_TAG_NAME_TRANSPROT_PROTOCOL, enRDA_UCHAR, (char*)&m_ucTransportProtocol);
    m_pRDAMsgAttrMgr->RegisterAttr(TAG_SESSION_ID, (USHORT)strlen(m_szSessionId),
                                   RDA_TAG_NAME_SESSION_ID, enRDA_STRING, m_szSessionId);
    m_pRDAMsgAttrMgr->RegisterAttr(TAG_MEDIA_IP, (USHORT)strlen(m_szMediaIP),
                                   RDA_TAG_NAME_MEDIA_IP, enRDA_STRING, m_szMediaIP);

    if (RDA_PROTOCOL_RTP_OVER_TCP == m_ucTransportProtocol)
    {
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_TCP_INTERLEAVED_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_TCP_INTERLEAVED_PORT, enRDA_UINT2, (char*)&m_usTcpInterleavedPort);
    }
    else
    {
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_VIDEO_RTP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_VIDEO_RTP_PORT, enRDA_UINT2, (char*)&m_usUdpVideoRtpPort);
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_VIDEO_RTCP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_VIDEO_RTCP_PORT, enRDA_UINT2, (char*)&m_usUdpVideoRtcpPort);
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_AUDIO_RTP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_AUDIO_RTP_PORT, enRDA_UINT2, (char*)&m_usUdpAudioRtpPort);
        m_pRDAMsgAttrMgr->RegisterAttr(TAG_UDP_AUDIO_RTCP_PORT, sizeof(RDA_UINT2),
                                       RDA_TAG_NAME_UDP_AUDIO_RTCP_PORT, enRDA_UINT2, (char*)&m_usUdpAudioRtcpPort);
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TAG_VIDEO_PAY_LOAD_TYPE, sizeof(RDA_UCHAR),
                                   RDA_TAG_NAME_VIDEO_PAY_LOAD_TYPE, enRDA_UCHAR, (char*)&m_ucVideoPayloadType);
    m_pRDAMsgAttrMgr->RegisterAttr(TAG_AUDIO_PAY_LOAD_TYPE, sizeof(RDA_UCHAR),
                                   RDA_TAG_NAME_AUDIO_PAY_LOAD_TYPE, enRDA_UCHAR, (char*)&m_ucAudioPayloadType);

    m_pRDAMsgAttrMgr->RegisterAttr(TAG_SESSION_URL, (USHORT)strlen(m_szSessionUrl),
                                   RDA_TAG_NAME_SESSION_URL, enRDA_STRING, m_szSessionUrl);

    return VOS_SUCCESS;
}

CRDAStartBusinessRsp::CRDAStartBusinessRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_START_BUSINESS_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_START_BUSINESS_RSP,
		LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDAStartBusinessRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAStartBusinessRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDAStopBusinessReq::CRDAStopBusinessReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_STOP_BUSINESS_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_STOP_BUSINESS_REQ,
		LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}


    m_ulBusinessID = 0;
    m_usChannelNo = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

long CRDAStopBusinessReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAStopBusinessReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagBusinessID, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_BUSINESSID, enRDA_UINT4, (char*)&m_ulBusinessID );
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelNo, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_CHANNELNO, enRDA_UINT2, (char*)&m_usChannelNo);
    return VOS_SUCCESS;
}

CRDAStopBusinessRsp::CRDAStopBusinessRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_STOP_BUSINESS_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_STOP_BUSINESS_RSP,
		LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDAStopBusinessRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAStopBusinessRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDAAudioCtrolReq::CRDAAudioCtrolReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_AUDIO_CONTROL_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_AUDIO_CONTROL_REQ,
		LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}


    m_ulBusinessID = 0;
    m_usChannelNo = 0;
    m_ucAudioCmd = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

long CRDAAudioCtrolReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAAudioCtrolReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagBusinessID, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_BUSINESSID, enRDA_UINT4, (char*)&m_ulBusinessID );
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelNo, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_CHANNELNO, enRDA_UINT2, (char*)&m_usChannelNo);
    m_pRDAMsgAttrMgr->RegisterAttr(TagAudioCmd, sizeof(RDA_UCHAR),
                                   RDA_TAG_NAME_AUDIOCONTROL, enRDA_UCHAR, (char*)&m_ucAudioCmd);
    return VOS_SUCCESS;
}

CRDAAudioCtrolRsp::CRDAAudioCtrolRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_AUDIO_CONTROL_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_AUDIO_CONTROL_RSP,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDAAudioCtrolRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAAudioCtrolRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDATuneupVolumeReq::CRDATuneupVolumeReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_TUNEUP_VOLUME_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_TUNEUP_VOLUME_REQ,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    m_ulBusinessID = 0;
    m_usChannelNo = 0;
    m_ucVolume = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

long CRDATuneupVolumeReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDATuneupVolumeReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagBusinessID, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_BUSINESSID, enRDA_UINT4, (char*)&m_ulBusinessID );
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelNo, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_CHANNELNO, enRDA_UINT2, (char*)&m_usChannelNo);
    m_pRDAMsgAttrMgr->RegisterAttr(TagVolume, sizeof(RDA_UCHAR),
                                   RDA_TAG_NAME_TUNEUPVOLUME, enRDA_UCHAR, (char*)&m_ucVolume);
    return VOS_SUCCESS;
}

CRDATuneupVolumeRsp::CRDATuneupVolumeRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_TUNEUP_VOLUME_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_TUNEUP_VOLUME_RSP,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDATuneupVolumeRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDATuneupVolumeRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDASetRDDisplayModeReq::CRDASetRDDisplayModeReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_SET_RD_DISPLAY_MODE_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_SET_RD_DISPLAY_MODE_REQ,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
   m_ulBusinessID = 0;
    m_usChannelNo = 0;
    m_usMode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

long CRDASetRDDisplayModeReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDASetRDDisplayModeReq RegisterAttrs  RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagBusinessID, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_BUSINESSID, enRDA_UINT4, (char*)&m_ulBusinessID );
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelNo, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_CHANNELNO, enRDA_UINT2, (char*)&m_usChannelNo);
    m_pRDAMsgAttrMgr->RegisterAttr(TagRDDispayMode, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_RD_DISPLAY_MODE, enRDA_UINT2, (char*)&m_usMode);
    return VOS_SUCCESS;
}

CRDASetRDDisplayModeRsp::CRDASetRDDisplayModeRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_SET_RD_DISPLAY_MODE_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_SET_RD_DISPLAY_MODE_RSP,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDASetRDDisplayModeRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDATuneupVolumeRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDAUnregisterReq::CRDAUnregisterReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_UNREGISTER_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_UNREGISTER_REQ,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

long CRDAUnregisterReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAUnregisterReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);

    return VOS_SUCCESS;
}

CRDAUnregisterRsp::CRDAUnregisterRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_UNREGISTER_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_UNREGISTER_RSP,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDAUnregisterRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAUnregisterRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDAConsoleShutdownReq::CRDAConsoleShutdownReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_CONSOLE_SHUTDOWN_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_CONSOLE_SHUTDOWN_REQ,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

long CRDAConsoleShutdownReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAConsoleShutdownReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);

    return VOS_SUCCESS;
}

CRDAConsoleShutdownRsp::CRDAConsoleShutdownRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_CONSOLE_SHUTDOWN_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_CONSOLE_SHUTDOWN_RSP,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDAConsoleShutdownRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAConsoleShutdownRsp RegisterAttrsRDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDAErrorNotifyReq::CRDAErrorNotifyReq( )
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_ERROR_NOTIFY_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_ERROR_NOTIFY_REQ,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}

    m_ulBusinessID = 0;
    m_usChannelNo = 0;
    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDAErrorNotifyReq::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAErrorNotifyReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagBusinessID, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_BUSINESSID, enRDA_UINT4, (char*)&m_ulBusinessID );
    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagChannelNo, sizeof(RDA_UINT2),
                                   RDA_TAG_NAME_CHANNELNO, enRDA_UINT2, (char*)&m_usChannelNo);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);
    return VOS_SUCCESS;
}

CRDAErrorNotifyRsp::CRDAErrorNotifyRsp( )
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_ERROR_NOTIFY_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_ERROR_NOTIFY_RSP,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
    m_ulErrorCode = 0;
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
    memset(m_szErrorDescription, 0x00, sizeof(m_szErrorDescription));
}

long CRDAErrorNotifyRsp::RegisterAttrs( )
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAErrorNotifyRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorCode, sizeof(RDA_UINT4),
                                   RDA_TAG_NAME_ERROR_CODE, enRDA_UINT4, (char*)&m_ulErrorCode);
    m_pRDAMsgAttrMgr->RegisterAttr(TagErrorDescription,
                                   (USHORT)strlen(m_szErrorDescription), RDA_TAG_NAME_ERROR_DESCRIPTION,
                                   enRDA_STRING, (char*)m_szErrorDescription);

    return VOS_SUCCESS;
}

CRDAHeartBeatReq::CRDAHeartBeatReq()
{
    m_stMsgHead.usPackType = RDA_MSG_REQ;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_HEARTBEAT_REQ;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_HEARTBEAT_REQ,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
	memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

LONG CRDAHeartBeatReq::RegisterAttrs(void)
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAHeartBeatReq RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    return VOS_SUCCESS;
}

CRDAHeartBeatRsp::CRDAHeartBeatRsp()
{
    m_stMsgHead.usPackType = RDA_MSG_RSP;
    m_stMsgHead.ucProtType = RDA_PROT_TYPE;
    m_stMsgHead.ucVersion = RDA_PROT_VERSION;
    m_stMsgHead.usMsgType = RDA_HEARTBEAT_RSP;

    if(!CToolsHelp::Strncpy(m_szRDAMsgTypeName, sizeof(m_szRDAMsgTypeName), RDA_MSG_NAME_HEARTBEAT_RSP,
            LEN_RDA_MSGTYPE_NAME))
	{
		IVS_RUN_LOG_ERR("Copy RDAMsgTypeName failed");
	}
    memset(m_szDecoderID, 0x00, sizeof(m_szDecoderID));
}

LONG CRDAHeartBeatRsp::RegisterAttrs(void)
{
    if (NULL == m_pRDAMsgAttrMgr)
    {
        IVS_RUN_LOG_ERR("RDAHeartBeatRsp RegisterAttrs RDAMsgAttrMgr is NULL");
        return VOS_FAIL;
    }

    m_pRDAMsgAttrMgr->RegisterAttr(TagDecoderID, LEN_RDA_ID,
                                   RDA_TAG_NAME_DECODERID, enRDA_STRING_ID, (char*)m_szDecoderID);
    return VOS_SUCCESS;
}
