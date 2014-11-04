#include "MediaBase.h"
#include "MediaXMLProcess.h"
#include "UserInfo.h"
#include "UserMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "IVS_Player.h"
#include "IVS_Trace.h"
#include "SDKSecurityClient.h"
#include "ivs_security_derive_key.h"
#include "RtspDef.h"

//等待ANOUNCE消息超时时长 500ms
//#define WAIT_ANOUNCE_TIMEOUT 500
#define XOR_INIT_KEY_LEN 16
#define XOR_INIT_SALT_LEN 14

struct RTSP_ERROR_CODE
{
	const int iRtspErrorCode; // rtsp client错误码
	const int iIvsErrorCode;  // 对应的ivs错误码;
};

// rtsp client的错误码转换为ivs_error错误码;
static RTSP_ERROR_CODE s_RtspErrorCodes[] =
{
	/*01*/	{ RTSP_ERR_NOT_EXIST,		IVS_PLAYER_RET_RTSP_ERR_NOT_EXIST},		// 连接句柄不存在;
	/*02*/	{ RTSP_ERR_DISCONNECT,      IVS_PLAYER_RET_RTSP_ERR_DISCONNECT},	// 连接失败;
	/*03*/	{ RTSP_ERR_RESPONSE,        IVS_PLAYER_RET_RTSP_ERR_RESPONSE},		// 响应消息错误;
	/*04*/	{ RTSP_ERR_TIMEOUT,         IVS_PLAYER_RET_RTSP_ERR_TIMEOUT},		// 响应超时;
	/*05*/	{ RTSP_ERR_STAUTS_ABNORMAL, IVS_PLAYER_RET_RTSP_ERR_STAUTS_ABNORMAL},// 连接状态异常;
	/*06*/	{ RTSP_ERR_SEND_FAIL,       IVS_PLAYER_RET_RTSP_ERR_SEND_FAIL},		// 发送出错;
	/*07*/	{ RTSP_ERR_RECV_FAIL,       IVS_PLAYER_RET_RTSP_ERR_RECV_FAIL},		// 接收出错;
	/*08*/	{ RTSP_ERR_PARAMETER,       IVS_PLAYER_RET_RTSP_ERR_PARAMETER},		// 参数检查出错;
	/*09*/	{ RTSP_ERR_NORMAL_STOP,     IVS_PLAYER_RET_RTSP_ERR_NORMAL_STOP},	// 正常结束，ANNOUNCE正常结束请求;
	/*10*/	{ RTSP_ERR_ABNORMAL_STOP,   IVS_PLAYER_RET_RTSP_ERR_ABNORMAL_STOP},	// 异常结束，ANNOUNCE异常结束请求;
	/*11*/	{ RTSP_ERR_RET_SYS,         IVS_PLAYER_RET_RTSP_ERR_RET_SYS},		// 通用系统错误;
	/*12*/	{ RTSP_SERVER_INTERNAL,     IVS_PLAYER_RET_RTSP_ERR_SERVICE_500_RSP},		// 500错误;
	/*13*/	{ RTSP_SERVICE_UNAVAILABLE, IVS_PLAYER_RET_RTSP_ERR_SERVICE_UNAVAILABLE_RSP},// 503错误;
	/*14*/  { RTSP_GONE,     IVS_PLAYER_RET_ERR_FILE_NOT_EXIST},		// 410错误;
	/*15*/  { RTSP_SERVER_NOTIMPLEMENTED, IVS_DCG_DEV_UN_SUPPORT},		// 501Not Implemented
	/*31*/  { RTSP_CLIENT_NOTENOUGH_BANDWIDTH,  IVS_PLAYER_RET_NOTENOUGH_BANDWIDTH}, 
	
	
};

CMediaBase::CMediaBase(void)
{
	m_pUserMgr = NULL;
	m_iStatus = MEDIA_STATUS_BUSY;

	m_strLocalIp = "";
	m_strLocalPort = "";

    m_hRtspSetupEvent = CreateSemaphore(NULL, 0, 1, NULL);
    memset(m_szCameraID,0,CODE_LEN);
    memset(&m_stuMediaParam, 0, sizeof(IVS_MEDIA_PARA));
    m_iStartType = PLAY_TYPE_HWND;
    
    m_ulHandle = 0;
    m_ulNetSourceChannel = 0;
    m_ulPlayerChannel = 0;

    m_enServiceType = SERVICE_TYPE_INVALID;
    m_iRtspHandle = 0;
    m_bReuseRtspConn = true;

    memset(&m_stMediaRsp, 0, sizeof(ST_MEDIA_RSP));
    m_pMediaMutex = VOS_CreateMutex();

	m_tmStart = 0;
	m_tmEnd   = 0;
	m_bStepPlay = false;
	m_bStepPlayIsLastFrame = false;
	m_bAssociatedAudio = true;
	memset(&m_stUrlMediaInfo, 0 ,sizeof(ST_URL_MEDIA_INFO));

//     memset(m_szLocalIp, 0, IVS_IP_LEN);
    m_uLocalPort = 0;

    memset(m_szRemoteIp, 0, IVS_IP_LEN);
    m_uRemotePort = 0;

    m_fSpeed = 0.0;

    m_bGetKeyIsSuccess = true;
    m_bControlPause = false;
}


CMediaBase::~CMediaBase(void)
{
	m_pUserMgr = NULL;
    if (NULL != m_hRtspSetupEvent)
    {
        CloseHandle(m_hRtspSetupEvent);
        m_hRtspSetupEvent = NULL;// TODO:此处需要释放;
    }

    try
    {
        VOS_DestroyMutex(m_pMediaMutex);
        m_pMediaMutex = NULL;
    }
    catch(...)
    {}//lint !e1775
}
//设置开启/停止校验水印
IVS_INT32 CMediaBase::SetWaterMarkVerify(IVS_UINT32 uiChannel, IVS_BOOL bIsStart) const
{
	IVS_DEBUG_TRACE("");
	return IVS_PLAY_SetWaterMarkVerify(uiChannel,bIsStart);
}
//设置实况播放模式：清晰、流畅
IVS_INT32 CMediaBase::SetPlayQualityMode(IVS_UINT32 uiChannel,IVS_INT32 iPlayQualityMode) const
{
	IVS_DEBUG_TRACE("");
	return IVS_PLAY_SetPlayQualityMode(uiChannel,iPlayQualityMode);
}
//获取视频质量参数
IVS_INT32 CMediaBase::GetMediaInfo(IVS_UINT32 uiChannel,IVS_MEDIA_INFO* pMediaInfo) const
{
	IVS_DEBUG_TRACE("");
	return IVS_PLAY_GetMediaInfo(uiChannel,pMediaInfo);
}

//获取当前播放时间
IVS_INT32 CMediaBase::GetPlayedTime(IVS_UINT32 uiChannel,IVS_UINT32 *puPlayTime) const
{
	CHECK_POINTER(puPlayTime, IVS_PARA_INVALID);
	
	IVS_DEBUG_TRACE("");
	unsigned long long tick = 0;
	if (IVS_SUCCEED == IVS_PLAY_GetPlayedTime(uiChannel, &tick))
	{
		*puPlayTime = (unsigned int)tick;
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

//设置播放比例
IVS_INT32 CMediaBase::SetDisplayScale(IVS_UINT32 uiChannel, IVS_UINT32 iDisplayScale) const
{
	IVS_DEBUG_TRACE("");
	return IVS_PLAY_SetDisplayAspectRatio(uiChannel, iDisplayScale);
}


void CMediaBase::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr = pUserMgr;
	(void)GetLocalIpPort();
}

IVS_INT32 CMediaBase::GetStatus()
{
	return m_iStatus;
}//lint !e1762

void CMediaBase::SetStatus(IVS_INT32 iStatus)
{
	m_iStatus = iStatus;
}
//从SMU/SCU获取媒体URL
IVS_INT32 CMediaBase::GetURL(const char * /*pszAuthHeader*/, CUserMgr *pUserMgr, ST_URL_MEDIA_INFO& stUrlMediaInfo, ST_MEDIA_RSP& stMediaRsp, bool& bAssociatedAudio)
{
	IVS_DEBUG_TRACE("");

	//if (NULL == pszAuthHeader)
	//{
	//	return IVS_PARA_INVALID;
	//}
	if (NULL == pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get URL", "user obj is null, when get url xml");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	CUserInfo* pUserInfo = NULL;
	pUserInfo = pUserMgr->GetUserInfoEx();//lint !e838
	if (NULL == pUserInfo)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get URL", "get user info false, when get url");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CXml xmlReq;
	IVS_INT32 iRet = IVS_FAIL;//lint !e838

	ST_SRU_LOGIN_RSP stSRULoginRsp;
	//if(0==stUrlMediaInfo.iClientType)
	//{
	//	stUrlMediaInfo.iClientType = pUserInfo->GetClientType




	//}
	//modify by limingjie lWX175483:2013-6-4  通过目标域编码找到相应的IP地址
	IVS_DOMAIN_ROUTE stDomainRoute;
	iRet = pUserMgr->GetDomainRouteMgr().GetDomainRoutebyCode(stUrlMediaInfo.cDomainCode, stDomainRoute);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get URL", "get domain route from stUrlMediaInfo false");
		return IVS_SDK_ERR_DOMAINCODE_NOT_EXIST;
	}
	if(strlen(stUrlMediaInfo.szDstIP)==0)
	{
		if(!CToolsHelp::Memcpy(stUrlMediaInfo.szDstIP, IVS_IP_LEN, stDomainRoute.stIP.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL", "Copy domain route IP error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}

        std::string strPort = CToolsHelp::Int2Str(pUserInfo->GetServerPort());
        if(!CToolsHelp::Strncpy(stUrlMediaInfo.szDstPort, PORT_LEN, strPort.c_str(),strPort.length()))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL", "Copy LocalIP error.");
            return IVS_ALLOC_MEMORY_ERROR;
        }

        BP_DBG_LOG("Media Base Get URL: IP = %s, Port = %s", stDomainRoute.stIP.cIP, stUrlMediaInfo.szDstPort);
	}
	(void)pUserInfo->GetLoginRspInfo(stSRULoginRsp);
	iRet = CMediaXMLProcess::GetURLXML(stUrlMediaInfo, stSRULoginRsp, xmlReq);//lint !e838

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get URL", "get url xml from stUrlMediaInfo false");
		return IVS_FAIL;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    BP_DBG_LOG("GetUrl request xml = %s.", pReq);
// 	string smuLinkId = "";
//     m_pUserMgr->GetSMULinkID(smuLinkId);
	//CCmd *pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_URL_REQ, pReq, smuLinkId);

	//if (NULL == pCmd)
	//{
	//	BP_RUN_LOG_ERR(IVS_SDK_RET_BP_CMD_REQ_INVALID,"CMediaBase::GetURL", "failed", "BuildSMUCmd failed");
	//	return IVS_SDK_RET_BP_CMD_REQ_INVALID;
	//}
	char cLocalDomainCode[IVS_DOMAIN_CODE_LEN + 1];
	memset(cLocalDomainCode,0,IVS_DOMAIN_CODE_LEN + 1);
	bool bRet = CToolsHelp::Memcpy(cLocalDomainCode,IVS_DOMAIN_CODE_LEN + 1,stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN);
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	CSendNssMsgInfo sendNssMsgInfo;	
	//string strLocalDomainCode;
	//m_pUserMgr->GetDomainCode(strLocalDomainCode);

	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_URL_REQ);
	sendNssMsgInfo.SetReqData(pReq);
	//stSend.iReqLength = xmlLen;

	sendNssMsgInfo.SetCameraCode(stUrlMediaInfo.szSrcCode);

	sendNssMsgInfo.SetDomainCode(cLocalDomainCode);

	iRet = IVS_FAIL;
	//IVS_CHAR* pRsp = NULL;
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
    // 发送操作失败，直接返回
	iRet = pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED == iNeedRedirect)
	{
		iRet = pUserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
	}

	////解析数据

    // NSS消息返回码错误，不解析数据，直接返回错误码
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get URL", "SendCmd operation succeed,rspCode = %d", iRet);
        return iRet;
    }

    //解析数据
	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());
	iRet =CMediaXMLProcess::GetURLResponseData(xmlRsp, stMediaRsp, bAssociatedAudio);
	//HW_DELETE(pCmdRsp);
	//IVS_DELETE(pRsp,MUILI);
	return iRet;
}
/*lint +e438*/

IVS_INT32 CMediaBase::GetLocalIpPort()
{
	if (NULL != m_pUserMgr)
	{
		CUserInfo *pUserInfo = NULL;
		pUserInfo = m_pUserMgr->GetUserInfoEx();//lint !e838
		if (NULL != pUserInfo)
		{
			m_strLocalPort = CToolsHelp::Int2Str(pUserInfo->GetServerPort());
            m_strLocalIp = pUserInfo->GetUserLocalIp();
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get Local Ip Port", "get user info false");
			return IVS_OPERATE_MEMORY_ERROR;
		}
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get Local Ip Port", "get user obj false");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return IVS_SUCCEED;
}

//根据服务类型和协议类型判断是否复用rtsp连接;
bool CMediaBase::IsReuseRtspConnection(IVS_SERVICE_TYPE enServiceType, IVS_PROTOCOL_TYPE enProtocolType, 
                                       IVS_MEDIA_TRANS_MODE enTransMode, IVS_BROADCAST_TYPE enBroadcastType)
{
    bool bReuseRtspConn = false;
    // 只有在TCP协议、非直连及中转情况下在存在rtsp连接复用的情况;
    if (PROTOCOL_RTP_OVER_TCP == enProtocolType 
        && MEDIA_TRANS == enTransMode
        && BROADCAST_UNICAST == enBroadcastType)
    {
        switch (enServiceType)
        {
        case SERVICE_TYPE_REALVIDEO:
        case SERVICE_TYPE_RECORD:
        case SERVICE_TYPE_DOWNLOAD:
        case SERVICE_TYPE_PLAYBACK:
		case SERVICE_TYPE_BACKUPRECORD_PLAYBACK:
		case SERVICE_TYPE_BACKUPRECORD_DOWNLOAD:
		case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK:
		case SERVICE_TYPE_DISASTEBACKUP_DOWNLOAD:
        case SERVICE_TYPE_AUDIO_CALL:
		case SERVICE_TYPE_PU_PLAYBACK:
            bReuseRtspConn = true;
            break;
        case SERVICE_TYPE_AUDIO_BROADCAST:
            break;
        default://默认情形，bReuseRtspConn 为false，不复用
            break;
        }
    }

    return bReuseRtspConn;
}//lint !e1762
//设置媒体参数;
int CMediaBase::SetMediaParam(const IVS_MEDIA_PARA* pMediaPara)
{
	IVS_DEBUG_TRACE("");
    if (NULL == pMediaPara)
    {
        m_stuMediaParam.DecoderCompany = VENDOR_HUAWEI;
        m_stuMediaParam.StreamType = STREAM_TYPE_MAIN;
        m_stuMediaParam.ProtocolType = PROTOCOL_RTP_OVER_UDP;
        m_stuMediaParam.TransMode = MEDIA_TRANS;
        m_stuMediaParam.BroadCastType = BROADCAST_UNICAST;
        m_stuMediaParam.VideoDecType = VIDEO_DEC_H264;
        m_stuMediaParam.AudioDecType = AUDIO_DEC_G711A;
        return IVS_SUCCEED;
    }

    // delete by s00191067 后续pMediaPara拿掉，CU界面不传该结构体，不需要判断;
    //参数校验;for 圈复杂度，枚举类型不检查
    //if ((VENDOR_HUAWEI != pMediaPara->DecoderCompany)
    //    /*|| 
    //    || (MEDIA_DIRECT != pMediaPara->TransMode && MEDIA_TRANS != pMediaPara->TransMode)
    //    || (BROADCAST_UNICAST != pMediaPara->BroadCastType || BROADCAST_MULTICAST != pMediaPara->BroadCastType)
    //    || (VIDEO_DEC_H264 > pMediaPara->VideoDecType || VIDEO_DEC_AVS < pMediaPara->VideoDecType)
    //    || (AUDIO_DEC_G711A > pMediaPara->AudioDecType || AUDIO_DEC_G726 < pMediaPara->AudioDecType)
    //    || (0 != pMediaPara->HardwareDec && 1 != pMediaPara->HardwareDec)*/)
    //{
    //    BP_RUN_LOG_ERR(IVS_PARA_INVALID,"SetMediaParam", 
    //        "param invalid, DecoderCompany:%d, StreamType:%d, ProtocolType:%d, DirectFirst:%d, Multicast:%d, VideoEncodeType:%d, AudioEncodeType:%d.",
    //        pMediaPara->DecoderCompany, pMediaPara->StreamType, pMediaPara->ProtocolType, pMediaPara->TransMode, pMediaPara->BroadCastType, pMediaPara->VideoDecType,
    //        pMediaPara->AudioDecType);
    //    return IVS_PARA_INVALID;
    //}

	BP_DBG_LOG("StreamType[%d], ProtocolType[%d] TransMode[%d] BroadCastType[%d]", 
				pMediaPara->StreamType, 
				pMediaPara->ProtocolType, 
				pMediaPara->TransMode, 
				pMediaPara->BroadCastType);

	if (PROTOCOL_RTP_OVER_UDP > pMediaPara->ProtocolType || PROTOCOL_RTP_OVER_TCP < pMediaPara->ProtocolType)
	{
		BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Set Media Param", "ProtocolType invalid [%d]", pMediaPara->ProtocolType);
		return IVS_DATA_INVALID;
	}

    bool bRet = CToolsHelp::Memcpy(&m_stuMediaParam,sizeof(IVS_MEDIA_PARA), pMediaPara, sizeof(IVS_MEDIA_PARA));
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Set Media Param", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    return IVS_SUCCEED;
}//lint !e818

void CMediaBase::RetSet()
{
    m_iStartType = PLAY_TYPE_HWND;
    memset(m_szCameraID, 0, sizeof(CODE_LEN));
    memset(&m_stuMediaParam, 0, sizeof(IVS_MEDIA_PARA));

    //重置状态;
    SetStatus(MEDIA_STATUS_FREE);

    m_ulHandle = SDK_INVALID_HANDLE;

    m_iRtspHandle = 0;
    m_ulNetSourceChannel = 0;
    m_ulPlayerChannel = 0;
	m_fSpeed = 0.0;
    m_bReuseRtspConn = false;
	m_uRemotePort = 0;
    memset(&m_stMediaRsp, 0, sizeof(ST_MEDIA_RSP));
	memset(m_szRemoteIp, 0, IVS_IP_LEN);
	m_uLocalPort = 0;
	memset(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));
	m_bAssociatedAudio = false;
	m_bStepPlay = false;
	m_enServiceType = SERVICE_TYPE_INVALID;
	m_bReuseRtspConn = false;
	//m_hRtspSetupEvent = NULL;
	//m_iStartType = PLAY_TYPE_HWND;
	m_strMuIpAddr = "";
	m_strLocalPort = "";
	m_strLocalIp = "";
	m_UserInfo.Clear();
    m_bControlPause = false;
}

//实况开启流复用
int CMediaBase::ReStartStream(const START_PLAY_PARAM& stParam)
{
	IVS_DEBUG_TRACE("");

	// 检测如果在启动之前未释放netsource流，先清空再启动;
	if (0 != m_ulNetSourceChannel)
	{
		(void)IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
		(void)IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);

		BP_RUN_LOG_ERR(IVS_FAIL,"ReStartStream", "Do not stop play begin start play, netsourcehandle[%lu]", m_ulNetSourceChannel);
		m_ulNetSourceChannel = 0;
	}

	// 建立RTSP连接
	int iRet = RTSP_CLIENT_GetHandle(true, &m_iRtspHandle);
	if (IVS_SUCCEED != iRet)//lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL,"ReStart RealPlay Stream", "Rtsp client get handle fail.");
		return IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL;
	}

	INET_ADDR stServiceAddr;
	memset(&stServiceAddr, 0x0, sizeof(INET_ADDR));
	stServiceAddr.unAddrIp = htonl((unsigned int)inet_addr(m_stMediaRsp.szRTSPIP));        //lint !e732
	int iPort = CToolsHelp::StrToInt(m_stMediaRsp.szRTSPPORT);
	stServiceAddr.unAddrPort = (unsigned short)iPort;    //lint !e507

	MEDIA_ADDR stSourceMediaAddr;
	memset(&stSourceMediaAddr, 0x0, sizeof(MEDIA_ADDR));

	// 根据ServiceType类型判断是否复用rtsp连接;
	m_bReuseRtspConn = IsReuseRtspConnection(m_enServiceType, m_stuMediaParam.ProtocolType, 
		m_stuMediaParam.TransMode, m_stuMediaParam.BroadCastType);
	// 复用rtspclient连接;
	if (m_bReuseRtspConn)
	{
		iRet = RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, stParam.cbRtspDataCallBack, this);
		if (IVS_SUCCEED != iRet)
		{
			//RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			BP_RUN_LOG_ERR(IVS_FAIL,"ReStart RealPlay Stream", "Rtsp client set data callback fail, retcode=%d.", iRet);
		}
		iRet = RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT, true, NULL, NULL);
		BP_RUN_LOG_INF("ReStart RealPlay Stream", "IVS rtsp send setup message, ret=%d.", iRet);
		if (IVS_SUCCEED != iRet)//lint !e774
		{
			RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_iRtspHandle = 0;
			BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"ReStart RealPlay Stream", "Rtsp client send setupmsg fail, retcode=%d.", iRet);
			return GetRtspErrorCode(iRet);
		}
	}
	// 不复用，根据协议类型调用NetSource创建对应的连接对象;
	else
	{
		iRet = IVS_NETSOURCE_GetChannel(&m_ulNetSourceChannel);
		IVS_NETSOURCE_SetDataCallBack(m_ulNetSourceChannel, stParam.cbNetDataCallBack, this);
		iRet += IVS_NETSOURCE_SetProtocolType(m_ulNetSourceChannel, m_stuMediaParam.ProtocolType); //lint !e732
		if (IVS_SUCCEED != iRet)
		{
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
            m_ulNetSourceChannel = 0;
			m_iRtspHandle = 0;
			BP_RUN_LOG_ERR(IVS_PLAYER_RET_GET_CHANNEL_ERROR,"ReStart RealPlay Stream", "netsource get channel or set protocol type failed, retcode=%d.", iRet);
			return IVS_PLAYER_RET_GET_CHANNEL_ERROR;
		}

		IVS_NETSOURCE_SetLocalRecvStreamIP(m_ulNetSourceChannel, (char*)m_strLocalIp.c_str());
		unsigned int uiLocalRecvStreamVideoRtpPort = 0;
		unsigned int uiLocalRecvStreamAudioRtpPort = 0;
		iRet = IVS_NETSOURCE_GetLocalRecvStreamPort(m_ulNetSourceChannel, &uiLocalRecvStreamVideoRtpPort, &uiLocalRecvStreamAudioRtpPort);
		if(IVS_SUCCEED != iRet)
		{
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_ulNetSourceChannel = 0;
			m_iRtspHandle = 0; 
			BP_RUN_LOG_ERR(iRet,"ReStart RealPlay Stream", "IVS_NETSOURCE_GetLocalRecvStreamPort failed, retcode=%d.", iRet);
			return IVS_PLAYER_RET_GET_CHANNEL_ERROR;
		}

		MEDIA_ADDR stLocalMediaAddr;
		memset(&stLocalMediaAddr, 0x0, sizeof(MEDIA_ADDR));
		stLocalMediaAddr.unMediaIp = htonl((unsigned int)inet_addr(m_strLocalIp.c_str()));        //lint !e732
		stLocalMediaAddr.unMediaVideoPort = (unsigned short)uiLocalRecvStreamVideoRtpPort;
		stLocalMediaAddr.unMediaAudioPort = (unsigned short)uiLocalRecvStreamAudioRtpPort;

		if(PROTOCOL_RTP_OVER_TCP == m_stuMediaParam.ProtocolType)
		{
			iRet = RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT, 
				false, &stLocalMediaAddr, &stSourceMediaAddr);
		}
		else
		{
			iRet = RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT, 
				false, &stLocalMediaAddr, &stSourceMediaAddr);
		}

		BP_RUN_LOG_INF("ReStart RealPlay Stream", "IVS rtsp send setup message, ret=%d.", iRet);
		if (0 != iRet)
		{   
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
    		m_ulNetSourceChannel = 0;
			m_iRtspHandle = 0;
			BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"ReStart RealPlay Stream", "Rtsp client send setupmsg fail, retcode=%d.", iRet);
			return GetRtspErrorCode(iRet);
		}

		stSourceMediaAddr.unMediaIp = ntohl(stSourceMediaAddr.unMediaIp);        //lint !e732
		string strServiceIP = "";
		if (IVS_SUCCEED != AddrUint2Char(stSourceMediaAddr.unMediaIp, strServiceIP))
		{
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_ulNetSourceChannel = 0;
			m_iRtspHandle = 0;
			BP_RUN_LOG_ERR(IVS_FAIL,"ReStart RealPlay Stream", "AddrUint2Char failed. strServiceIP = %s.", strServiceIP.c_str());
			return IVS_FAIL;
		}

		iRet = IVS_NETSOURCE_OpenNetStream(m_ulNetSourceChannel, (char*)strServiceIP.c_str(), stSourceMediaAddr.unMediaVideoPort, stSourceMediaAddr.unMediaAudioPort);
		if (IVS_SUCCEED != iRet)
		{
			IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
			RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
    		m_ulNetSourceChannel = 0;
			m_iRtspHandle = 0;
    		BP_RUN_LOG_ERR(IVS_PLAYER_RET_NET_OPEN_STREAM_FAIL,"ReStart RealPlay Stream", "netsource open net stream failed.");
			return IVS_PLAYER_RET_NET_OPEN_STREAM_FAIL;
		}

        (void)CToolsHelp::Strncpy(m_szRemoteIp, IVS_IP_LEN,
                                  strServiceIP.c_str(), strServiceIP.length());

        m_uLocalPort = stLocalMediaAddr.unMediaVideoPort;
        m_uRemotePort = stSourceMediaAddr.unMediaVideoPort;
	}

	// 等待信号量，获取密钥不成功不发送Play消息
	DWORD ulRet = WaitForSingleObject(m_hRtspSetupEvent, 500);
    BP_DBG_LOG("ReStartStream, WaitForSingleObject, ulRet:%08x", ulRet);
    if (!GetSecretFlag())
    {
        RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
        RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;

        BP_RUN_LOG_ERR(IVS_SDK_EXCHANGE_KEY_ERR_GET_KEY_FAIL, "", "ReStartStream, Get Key Fail.");
        return IVS_SDK_EXCHANGE_KEY_ERR_GET_KEY_FAIL;
    }

	if (!m_bReuseRtspConn)
	{
		iRet = IVS_NETSOURCE_StartRecvStream(m_ulNetSourceChannel);
		if (IVS_SUCCEED != iRet)
		{
			IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_ulNetSourceChannel = 0;
			m_iRtspHandle = 0;
			BP_RUN_LOG_ERR(IVS_PLAYER_RET_NET_START_RECEIVE_FAIL,"ReStart RealPlay Stream", "mp start receive stream failed, retcode=%d.", iRet);
			return IVS_PLAYER_RET_NET_START_RECEIVE_FAIL;
		}
	}

	MEDIA_RANGE_S stMdieaRange;
	memset(&stMdieaRange, 0x0, sizeof(MEDIA_RANGE_S));
	stMdieaRange.enRangeType = RANGE_TYPE_NPT;
	stMdieaRange.MediaBeginOffset = 0;

	iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)stParam.fSpeed, &stMdieaRange, RTSP_CLIENT_TIMEOUT);
	if (IVS_SUCCEED != iRet)
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);

		if (!m_bReuseRtspConn)
		{
			(void)IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
			(void)IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			m_ulNetSourceChannel = 0;
		}

		RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL, "ReStart RealPlay Stream", "RTSP client send play message fail, retcode=%d.", iRet);
		return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
	}

	return IVS_SUCCEED;
}
//实况流停止
int CMediaBase::StopStream()
{
	IVS_DEBUG_TRACE("");
	int iRet = RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle,
		m_stMediaRsp.szURL,
		RTSP_CLIENT_TIMEOUT);
	if (RET_OK != iRet)
	{
		BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Stop RealPlay Stream", "call RTSP_CLIENT_SendTeardownMsg failed, ret = %d", iRet);
	}

	iRet = RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, NULL, NULL);
	if (RET_OK != iRet)
	{
		BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Stop Stream", "call RTSP_CLIENT_SetDataCallBack failed, ret = %d", iRet);
	}
	else
	{
		BP_DBG_LOG("RTSP_CLIENT_SetDataCallBack succeed.");
	}

	iRet = RTSP_CLIENT_FreeHandle(m_iRtspHandle);
	m_iRtspHandle = 0;
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Stop RealPlay Stream", "call rtspclient free handle failed, ret = %d", iRet);
	}

	// 无论是否复用，均尝试释放netsource库;
	//if (!m_bReuseRtspConn)
	{
		(void)IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
		(void)IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
		m_ulNetSourceChannel = 0;
	}

	IVS_PLAY_StopDecryptData(m_ulPlayerChannel);

	return iRet;
}
//启动播放
int CMediaBase::StartPlay(const START_PLAY_PARAM& stParam)
{
	IVS_DEBUG_TRACE("");

	// 检测如果在启动之前未释放netsource流，先清空再启动;
	if (0 != m_ulNetSourceChannel)
	{
		(void)IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
		(void)IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
		
		BP_RUN_LOG_ERR(IVS_FAIL,"Start Play", "Do not stop play begin start play, netsourcehandle[%lu]", m_ulNetSourceChannel);
		m_ulNetSourceChannel = 0;
	}

    //m_hRtspSetupEvent = CreateSemaphore(NULL, 0, 1, NULL);
    if (NULL == m_hRtspSetupEvent)
    {
        BP_DBG_LOG("StartPlay, m_hRtspSetupEvent is Null.");
    }

	(void)IVS_PLAY_GetChannel(&m_ulPlayerChannel);
	BP_DBG_LOG("Startplay", "PlayerChannel = %08x", m_ulPlayerChannel);

	 
    // 建立RTSP连接
    int iRet = RTSP_CLIENT_GetHandle(true, &m_iRtspHandle);
    if (0 != iRet)//lint !e774
    {
		int iRtspErrorCode = GetRtspErrorCode(iRet);
        BP_RUN_LOG_ERR(iRtspErrorCode,"Start RealPlay", "Rtsp client get handle fail, ret[%d].", iRet);
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
        return iRtspErrorCode;
    }

    INET_ADDR stServiceAddr;
    memset(&stServiceAddr, 0x0, sizeof(INET_ADDR));
    stServiceAddr.unAddrIp = htonl((unsigned int)inet_addr(m_stMediaRsp.szRTSPIP));        //lint !e732
    int iPort = CToolsHelp::StrToInt(m_stMediaRsp.szRTSPPORT);
    stServiceAddr.unAddrPort = (unsigned short)iPort;    //lint !e507

    MEDIA_ADDR stSourceMediaAddr;
    memset(&stSourceMediaAddr, 0x0, sizeof(MEDIA_ADDR));
	BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp send setup message begin.");

    // 根据ServiceType类型判断是否复用rtsp连接;
    m_bReuseRtspConn = IsReuseRtspConnection(m_enServiceType, m_stuMediaParam.ProtocolType, 
                                             m_stuMediaParam.TransMode, m_stuMediaParam.BroadCastType);
    // 复用rtspclient连接;
    if (m_bReuseRtspConn)
    {
        BP_RUN_LOG_INF("Start RealPlay", "reused rtsp connect.");
        iRet = RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, stParam.cbRtspDataCallBack, this);
        if (0 != iRet)
        {
            //RTSP_CLIENT_FreeHandle(m_iRtspHandle);
            BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Start RealPlay", "Rtsp client set data callback fail, retcode=%d.", iRet);
        } 

        iRet = RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT, true, NULL, NULL);
        BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp send setup message, ret=%d.", iRet);
        if (0 != iRet)//lint !e774
        {
            RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
            RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_iRtspHandle = 0;

			IVS_PLAY_FreeChannel(m_ulPlayerChannel);
			m_ulPlayerChannel = 0;
			
			BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Start RealPlay", "Rtsp client send setupmsg fail, retcode=%d.", iRet);
            return GetRtspErrorCode(iRet);
        }
    }
    // 不复用，根据协议类型调用NetSource创建对应的连接对象;
    else
    {
        iRet = IVS_NETSOURCE_GetChannel(&m_ulNetSourceChannel);
        IVS_NETSOURCE_SetDataCallBack(m_ulNetSourceChannel, stParam.cbNetDataCallBack, this);
        iRet += IVS_NETSOURCE_SetProtocolType(m_ulNetSourceChannel, m_stuMediaParam.ProtocolType); //lint !e732
        if (IVS_SUCCEED != iRet)
        {
            IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
            RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_iRtspHandle = 0;
            
			IVS_PLAY_FreeChannel(m_ulPlayerChannel);
			m_ulPlayerChannel = 0;
			
			BP_RUN_LOG_ERR(IVS_PLAYER_RET_GET_CHANNEL_ERROR,"Start RealPlay", "netsource get channel or set protocol type failed, retcode=%d.", iRet);
            return IVS_PLAYER_RET_GET_CHANNEL_ERROR;
        }

        IVS_NETSOURCE_SetLocalRecvStreamIP(m_ulNetSourceChannel, (char*)m_strLocalIp.c_str());
        unsigned int uiLocalRecvStreamVideoRtpPort = 0;
        unsigned int uiLocalRecvStreamAudioRtpPort = 0;
        iRet = IVS_NETSOURCE_GetLocalRecvStreamPort(m_ulNetSourceChannel, &uiLocalRecvStreamVideoRtpPort, &uiLocalRecvStreamAudioRtpPort);
        if(IVS_SUCCEED != iRet)
        {
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_ulNetSourceChannel = 0;
			m_iRtspHandle = 0;

			IVS_PLAY_FreeChannel(m_ulPlayerChannel);
			m_ulPlayerChannel = 0;


			BP_RUN_LOG_ERR(iRet,"Start RealPlay", "IVS_NETSOURCE_GetLocalRecvStreamPort failed, retcode=%d.", iRet);
			return IVS_PLAYER_RET_GET_CHANNEL_ERROR;
		}

        MEDIA_ADDR stLocalMediaAddr;
        memset(&stLocalMediaAddr, 0x0, sizeof(MEDIA_ADDR));
        stLocalMediaAddr.unMediaIp = htonl((unsigned int)inet_addr(m_strLocalIp.c_str()));        //lint !e732
        stLocalMediaAddr.unMediaVideoPort = (unsigned short)uiLocalRecvStreamVideoRtpPort;
        stLocalMediaAddr.unMediaAudioPort = (unsigned short)uiLocalRecvStreamAudioRtpPort;

        if(PROTOCOL_RTP_OVER_TCP != m_stuMediaParam.ProtocolType)
        {
			iRet = RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT, 
											false, &stLocalMediaAddr, &stSourceMediaAddr);
        }
        else
        {
			iRet = RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT, 
											false, &stLocalMediaAddr, &stSourceMediaAddr);
        }

        BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp send setup message, ret=%d.", iRet);
        if (0 != iRet)
        {   
            IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			m_ulNetSourceChannel = 0;
            RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
            RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_iRtspHandle = 0;

			IVS_PLAY_FreeChannel(m_ulPlayerChannel);
			m_ulPlayerChannel = 0;

            BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Start RealPlay", "Rtsp client send setupmsg fail, retcode=%d.", iRet);
            return GetRtspErrorCode(iRet);
        }

        stSourceMediaAddr.unMediaIp = ntohl(stSourceMediaAddr.unMediaIp);        //lint !e732
        string strServiceIP = "";
        if (IVS_SUCCEED != AddrUint2Char(stSourceMediaAddr.unMediaIp, strServiceIP))
        {
            IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			m_ulNetSourceChannel = 0;
            RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
            RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_iRtspHandle = 0;

			IVS_PLAY_FreeChannel(m_ulPlayerChannel);
			m_ulPlayerChannel = 0;
            BP_RUN_LOG_ERR(IVS_FAIL,"Start RealPlay", "AddrUint2Char failed. strServiceIP = %s.", strServiceIP.c_str());
            return IVS_FAIL;
        }

        iRet = IVS_NETSOURCE_OpenNetStream(m_ulNetSourceChannel, (char*)strServiceIP.c_str(), stSourceMediaAddr.unMediaVideoPort, stSourceMediaAddr.unMediaAudioPort);
        if (IVS_SUCCEED != iRet)
        {
            IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
            RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
            IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			m_ulNetSourceChannel = 0;
			
            RTSP_CLIENT_FreeHandle(m_iRtspHandle);
			m_iRtspHandle = 0;

			IVS_PLAY_FreeChannel(m_ulPlayerChannel);
			m_ulPlayerChannel = 0;


            BP_RUN_LOG_ERR(IVS_PLAYER_RET_NET_OPEN_STREAM_FAIL,"Start RealPlay", "netsource open net stream failed.");
            return IVS_PLAYER_RET_NET_OPEN_STREAM_FAIL;
        }

        (void)CToolsHelp::Strncpy(m_szRemoteIp, IVS_IP_LEN,
                                  strServiceIP.c_str(), strServiceIP.length());

        m_uLocalPort = stLocalMediaAddr.unMediaAudioPort;
        m_uRemotePort = stSourceMediaAddr.unMediaAudioPort;
    }
	BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp send setup message end.");
    // 等待信号量，没有密钥不发送Play消息
    DWORD ulRet = WaitForSingleObject(m_hRtspSetupEvent, 500);
    BP_DBG_LOG("WaitForSingleObject, ulRet:%08x", ulRet);

    BP_RUN_LOG_INF("Start RealPlay", "WaitForSingleObject.");

    iRet = IVS_SDK_EXCHANGE_KEY_ERR_GET_KEY_FAIL;
    if (GetSecretFlag())
    {
        BP_DBG_LOG("StartPlay, Get Key Is Success.");
        iRet = ProcessRtspPlayMsg(m_stMediaRsp.szURL, stSourceMediaAddr, stParam);
        return iRet;
    }
    //BP_RUN_LOG_INF("Start RealPlay", "m_bGetKeyIsSuccess is false.");

    IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
    RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
    IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
	m_ulNetSourceChannel = 0;
    RTSP_CLIENT_FreeHandle(m_iRtspHandle);
	m_iRtspHandle = 0;

    IVS_PLAY_FreeChannel(m_ulPlayerChannel);
    m_ulPlayerChannel = 0;

    BP_RUN_LOG_ERR(iRet,"Start RealPlay", "Get Key Is Fail.");
    return iRet;
}
 
/*
 * StartDownload
 * 开始下载, 默认使用 RTP RTSP 交织方式
 */
int CMediaBase::StartDownload(const START_PLAY_PARAM& stParam, const char *pCam, const IVS_DOWNLOAD_PARAM *pParam)
{
	IVS_DEBUG_TRACE("");
	
	(void)IVS_PLAY_GetChannel(&m_ulPlayerChannel);
	int iRet = RTSP_CLIENT_GetHandle(true, &m_iRtspHandle);
	if (0 != iRet)//lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL, "Start Download", "Rtsp client get handle fail.");
		return IVS_PLAYER_RET_RTSP_GET_HANDLE_FAIL;
	}

	iRet = RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, stParam.cbRtspDataCallBack, this);
	if (0 != iRet)
	{
		//RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		BP_RUN_LOG_ERR(IVS_FAIL, "Start Download", "Rtsp client set data callback fail, retcode=%d.", iRet);
	}

	INET_ADDR stServiceAddr;
	memset(&stServiceAddr, 0x0, sizeof(INET_ADDR));
	stServiceAddr.unAddrIp = htonl((unsigned int)inet_addr(m_stMediaRsp.szRTSPIP));        //lint !e732
	int iPort = CToolsHelp::StrToInt(m_stMediaRsp.szRTSPPORT);
	stServiceAddr.unAddrPort = (unsigned short)iPort;    //lint !e507

	BP_RUN_LOG_INF("Start Download", "IVS rtsp send setup message begin.");

	// 根据ServiceType类型判断是否复用rtsp连接;
	m_bReuseRtspConn = true;
	iRet = RTSP_CLIENT_SendSetupMsg(m_iRtspHandle, &stServiceAddr, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT, true, NULL, NULL);
	BP_RUN_LOG_INF("Start Download", "IVS rtsp send setup message, ret=%d.", iRet);
	if (0 != iRet)//lint !e774
	{
		RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
		RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle  = 0;
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;

		BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Start RealPlay", "Rtsp client send setupmsg fail, retcode=%d.", iRet);
		return GetRtspErrorCode(iRet);
	}

	BP_RUN_LOG_INF("Start Download", "IVS rtsp send setup message end.");

//  等待信号量，没有密钥不发送Play消息
//	WaitForSingleObject(m_hRtspSetupEvent, WAIT_ANOUNCE_TIMEOUT);

	MEDIA_ADDR stSourceMediaAddr;
	memset(&stSourceMediaAddr, 0x0, sizeof(MEDIA_ADDR));


	return ProcessRtspPlayMsgDownLoad(m_stMediaRsp.szURL, stSourceMediaAddr, stParam, pCam, pParam);
}

int CMediaBase::StopDownLoad()
{
	IVS_DEBUG_TRACE("");
	int iRet = RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
	if (RET_OK != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_SEND_TEARDOWN_FAIL,"Stop RealPlay", "call RTSP_CLIENT_SendTeardownMsg failed, ret = %d", iRet);
	}

	(void)RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, NULL, NULL);

	iRet += IVS_PLAY_StopRecord(m_ulPlayerChannel);
	iRet += IVS_PLAY_CloseStream(m_ulPlayerChannel);
	iRet += IVS_PLAY_FreeChannel(m_ulPlayerChannel);
	if (IVS_SUCCEED != iRet) //lint !e774 !e831
	{
		BP_RUN_LOG_ERR(iRet,"Stop RealPlay", "call ivs_player stop play failed, ret = %d", iRet);
	}

	iRet = RTSP_CLIENT_FreeHandle(m_iRtspHandle);
	m_ulPlayerChannel = 0;
	m_iRtspHandle     = 0;

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Stop RealPlay", "call rtspclient free handle failed, ret = %d", iRet);
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

int CMediaBase::ProcessRtspPlayMsgDownLoad(const char* pszURL, const MEDIA_ADDR& /*stServerMediaAddr*/, const START_PLAY_PARAM& stParam, const char *pCam, const IVS_DOWNLOAD_PARAM *pParam)
{
	CHECK_POINTER(pszURL, IVS_PARA_INVALID);
	CHECK_POINTER(pCam, IVS_PARA_INVALID);
	CHECK_POINTER(pParam, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");
	//int iRet = IVS_PLAY_GetChannel(&m_ulPlayerChannel);
	int iRet = IVS_PLAY_OpenStream(m_ulPlayerChannel, SERVICE_TYPE_DOWNLOAD, (IVS_UINT32)stParam.enSourceType);

	if (SERVICE_TYPE_PU_PLAYBACK == stParam.enServiceType || 
		SERVICE_TYPE_PU_DOWNLOAD == stParam.enServiceType)
	{
		iRet += IVS_PLAY_SetDownLoadTimeSpan(m_ulPlayerChannel, m_tmStart, m_tmEnd, TRUE);
	}
	else
	{
		iRet += IVS_PLAY_SetDownLoadTimeSpan(m_ulPlayerChannel, m_tmStart, m_tmEnd, FALSE);
	}

	if (IVS_SUCCEED != iRet)
	{
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_MP_OPEN_STREAM_FAIL,"Process Rtsp Play Msg", "mp open stream failed, retcode=%d.", iRet);
		return IVS_PLAYER_RET_MP_OPEN_STREAM_FAIL;
	}

	IVS_LOCAL_RECORD_PARAM RecordParam;
	memset(&RecordParam, 0, sizeof(RecordParam));
	//TODO fill
    RecordParam.uiDiskWarningValue = pParam->uiDiskWarningValue;
	RecordParam.uiNameRule         = pParam->uiNameRule;
	RecordParam.uiRecordFormat     = pParam->uiRecordFormat;
	RecordParam.uiRecordTime       = (unsigned int)-1;  // 禁用超时停止功能
	RecordParam.uiSplitterType     = pParam->uiSplitterType;
	RecordParam.uiSplitterValue    = pParam->uiSplitterValue;
	RecordParam.uiStopRecordValue  = pParam->uiStopRecordValue;
    
    RecordParam.bEncryptRecord     = pParam->bEncryptRecord;
	(void)CToolsHelp::Strncpy(RecordParam.cRecordPWD, sizeof(RecordParam.cRecordPWD) - 1,
		    pParam->cRecordPWD, strlen(pParam->cRecordPWD)); 
	(void)CToolsHelp::Strncpy(RecordParam.cSavePath, sizeof(RecordParam.cSavePath) - 1, 
		    pParam->cSavePath, strlen(pParam->cSavePath));


	const char *pCamName = NULL;
	IVS_CAMERA_BRIEF_INFO birefinfo;
	memset(&birefinfo, 0, sizeof(birefinfo));
	if (m_pUserMgr)
	{
		std::string strCamcode = pCam;
		m_pUserMgr->GetDeviceMgr().GetCameraBriefInfobyCode(strCamcode, birefinfo, true);
		pCamName = birefinfo.cName;
    }

	iRet = IVS_PLAY_StartRecord(m_ulPlayerChannel, pCam, pCamName, pParam->cFileName, &RecordParam, true);
	//iRet += IVS_PLAY_StartPlay(m_ulPlayerChannel);
	if (IVS_SUCCEED != iRet)
	{
		IVS_PLAY_CloseStream(m_ulPlayerChannel);
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0; 
		RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;

		BP_RUN_LOG_ERR(iRet,"Process Rtsp Play Msg", "mp start play failed.");
		return iRet;
	}

	MEDIA_RANGE_S stMdieaRange;
	memset(&stMdieaRange, 0x0, sizeof(MEDIA_RANGE_S));
	stMdieaRange.enRangeType      = RANGE_TYPE_NPT;
	stMdieaRange.MediaBeginOffset = 0;

	BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp play setup message start.");
	iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, pszURL, (double)stParam.fSpeed, &stMdieaRange, RTSP_CLIENT_TIMEOUT);
	BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp play setup message end.");

	if (0 != iRet)
	{
		(void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);

		if (!m_bReuseRtspConn)
		{
			(void)IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
		}
		(void)IVS_PLAY_CloseStream(m_ulPlayerChannel);
		(void)IVS_PLAY_StopRecord(m_ulPlayerChannel);
		

		(void)IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;

		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL, "Process Rtsp Play Msg", "RTSP client send play message fail, retcode=%d.", iRet);
		return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
	}
    return IVS_SUCCEED;
}

//处理rtsp的play流程;
int CMediaBase::ProcessRtspPlayMsg(const char* pszURL, const MEDIA_ADDR& /*stServerMediaAddr*/, const START_PLAY_PARAM& stParam)
{
	IVS_DEBUG_TRACE("");
    // 打开流、启动收流线程;
    //(void)IVS_PLAY_GetChannel(&m_ulPlayerChannel);
    //(void)IVS_PLAY_SetVideoDecodeType(m_ulPlayerChannel, (unsigned int)m_stuMediaParam.VideoDecType);
    //(void)IVS_PLAY_SetAudioDecodeType(m_ulPlayerChannel, (unsigned int)m_stuMediaParam.AudioDecType);
    int iRet = IVS_PLAY_OpenStream(m_ulPlayerChannel, (IVS_UINT32)stParam.enServiceType, (IVS_UINT32)stParam.enSourceType);
    if (IVS_SUCCEED != iRet)
    {
		IVS_PLAY_CloseStream(m_ulPlayerChannel);
        IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		
        RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;

		if (!m_bReuseRtspConn && m_ulNetSourceChannel)
		{
			IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			m_ulNetSourceChannel = 0;
		}

		BP_RUN_LOG_ERR(iRet,"Process Rtsp Play Msg", "mp open stream failed, retcode=%d.", iRet);
		return iRet;
    }

    iRet = IVS_PLAY_StartPlay(m_ulPlayerChannel);
    if (IVS_SUCCEED != iRet)
    {
        IVS_PLAY_CloseStream(m_ulPlayerChannel);
		IVS_PLAY_StopPlay(m_ulPlayerChannel);
        IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		
        RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
	
		if (!m_bReuseRtspConn && m_ulNetSourceChannel)
		{
			IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
			IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			m_ulNetSourceChannel = 0;
		}

        BP_RUN_LOG_ERR(iRet,"Process Rtsp Play Msg", "mp start play failed.");
        return iRet;
    }

    if (!m_bReuseRtspConn)
    {
        iRet = IVS_NETSOURCE_StartRecvStream(m_ulNetSourceChannel);
        if (IVS_SUCCEED != iRet)
        {
            IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
            IVS_PLAY_CloseStream(m_ulPlayerChannel);
            IVS_PLAY_StopPlay(m_ulPlayerChannel);
            IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
            IVS_PLAY_FreeChannel(m_ulPlayerChannel);
            RTSP_CLIENT_FreeHandle(m_iRtspHandle);

			m_iRtspHandle = 0;
			m_ulPlayerChannel = 0;
			m_ulNetSourceChannel = 0;

            BP_RUN_LOG_ERR(IVS_PLAYER_RET_NET_START_RECEIVE_FAIL,"Process Rtsp Play Msg", "mp start receive stream failed, retcode=%d.", iRet);
            return IVS_PLAYER_RET_NET_START_RECEIVE_FAIL;
        }
    }

    MEDIA_RANGE_S stMdieaRange;
    memset(&stMdieaRange, 0x0, sizeof(MEDIA_RANGE_S));
    stMdieaRange.enRangeType = RANGE_TYPE_NPT;

	if (stParam.fSpeed < 0 || 253.0 == stParam.fSpeed || 252.0 == stParam.fSpeed) /* 252.0 1倍速全量后退, 253.0 2倍速全量后退 */
	{
		stMdieaRange.MediaBeginOffset = static_cast<IVS_UINT32>(m_tmEnd - m_tmStart);
	}
	else
	{
		stMdieaRange.MediaBeginOffset = 0;
	}
   
	BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp play setup message start.");
	iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, pszURL, (double)stParam.fSpeed, &stMdieaRange, RTSP_CLIENT_TIMEOUT);
	BP_RUN_LOG_INF("Start RealPlay", "IVS rtsp play setup message end.");
    if (0 != iRet)
    {
        (void)RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
        
        if (!m_bReuseRtspConn)
        {
            (void)IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
			(void)IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
			m_ulNetSourceChannel = 0;
        }
		
		(void)IVS_PLAY_CloseStream(m_ulPlayerChannel);
        (void)IVS_PLAY_StopPlay(m_ulPlayerChannel);
        (void)IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		
        RTSP_CLIENT_FreeHandle(m_iRtspHandle);
		m_iRtspHandle = 0;
	
        BP_RUN_LOG_ERR(GetRtspErrorCode(iRet), "Process Rtsp Play Msg", "RTSP client send play message fail, retcode=%d.", iRet);
        return GetRtspErrorCode(iRet);
    }

    // 发送Options消息
    //if ((SERVICE_TYPE_DIRECT_SCC_PLAYBACK == ulServiceType)
    //    ||(SERVICE_TYPE_TRANS_SCC_PLAYBACK == ulServiceType))
    //{
    //    char szIATime[512] = {0,};
    //    int iLength = 0;
    //    iRet = RTSP_CLIENT_SendOptionMsg(iHandle, pUrl, RTSP_CLIENT_TIMEOUT, szIATime, iLength);
    //    if ((0 != iRet) && (NULL == strstr(pUrl, "#")))  // options消息发送失败，且不是外域时，直接停流
    //    {
    //        IVS_LOG(LOG_ERR, "RTSP client send options message fail.");
    //        handleException(ulPort, iHandle, pUrl, stRtspInfo.bIsTcpDirect);
    //        return SDK_RET_SEND_PLAY_ERROR;
    //    }

    //    //         if (0 == iRet)     // options消息发送成功，设置智能分析时间
    //    //         {
    //    //             ULONG64 ulIATime = 0;
    //    //             ULONG   ulIATimePos = 0;
    //    //             (void)ParseIATime(szIATime, iLength, ulIATime, ulIATimePos);//lint !e737
    //    //             (void)IVS_SetVodIARefTime(ulPort, ulIATime, ulIATimePos);//lint !e737
    //    //         }
    //}
    return IVS_SUCCEED;
}
//实况停止
int CMediaBase::StopPlay()
{
	IVS_DEBUG_TRACE("");

	/*
	 * DTS2013080908996 
	 * 单帧状态下，先恢复播放线程
	 */
	if (m_bStepPlay)
	{
		(void)IVS_PLAY_ResumePlay(m_ulPlayerChannel);
	}

    int iRet = RTSP_CLIENT_SendTeardownMsg(m_iRtspHandle,
                                           m_stMediaRsp.szURL,
                                           RTSP_CLIENT_TIMEOUT);
    if (RET_OK != iRet)
    {
        BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Stop RealPlay", "call RTSP_CLIENT_SendTeardownMsg failed, ret = %d", iRet);
    }
	else
	{
		BP_DBG_LOG("RTSP_CLIENT_SendTeardownMsg succeed.");
	}

	iRet = RTSP_CLIENT_SetDataCallBack(m_iRtspHandle, NULL, NULL);
	if (RET_OK != iRet)
	{
		BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Stop RealPlay", "call RTSP_CLIENT_SetDataCallBack failed, ret = %d", iRet);
	}
	else
	{
		BP_DBG_LOG("RTSP_CLIENT_SetDataCallBack succeed.");
	}

	iRet = RTSP_CLIENT_FreeHandle(m_iRtspHandle);
	if (RET_OK != iRet)
	{
		BP_RUN_LOG_ERR(GetRtspErrorCode(iRet),"Stop RealPlay", "call RTSP_CLIENT_FreeHandle failed, ret = %d", iRet);
	}
	else
	{
		BP_DBG_LOG("RTSP_CLIENT_FreeHandle succeed.");
	}
	m_iRtspHandle = 0;

	// 无论是否复用，均尝试释放netsource库;
    //if (!m_bReuseRtspConn)
    {
        (void)IVS_NETSOURCE_CloseNetStream(m_ulNetSourceChannel);
		(void)IVS_NETSOURCE_FreeChannel(m_ulNetSourceChannel);
		m_ulNetSourceChannel = 0;

    }
    iRet += IVS_PLAY_CloseStream(m_ulPlayerChannel);
    iRet += IVS_PLAY_StopPlay(m_ulPlayerChannel);
    iRet += IVS_PLAY_FreeChannel(m_ulPlayerChannel);
    if (IVS_SUCCEED != iRet) //lint !e774 !e831
    {
        BP_RUN_LOG_ERR(iRet,"Stop RealPlay", "call ivs_player stop play failed, ret = %d", iRet);
    }

	m_ulPlayerChannel = 0;
    m_bGetKeyIsSuccess = true;
    return iRet;
}
//开启音频播放
int CMediaBase::PlaySound()
{
    return IVS_PLAY_PlaySound(m_ulPlayerChannel);
}
//关闭音频播放
int CMediaBase::StopSound()
{
    return IVS_PLAY_StopSound(m_ulPlayerChannel);
}

//设置音量
int CMediaBase::SetVolume(unsigned int uiVolumeValue)
{
    return IVS_PLAY_SetVolume(m_ulPlayerChannel, uiVolumeValue);
}

//获取音量
int CMediaBase::GetVolume(unsigned int* puiVolumeValue)
{
    return IVS_PLAY_GetVolume(m_ulPlayerChannel, puiVolumeValue);
}

//开始本地录像
int CMediaBase::StartLocalRecord(const IVS_LOCAL_RECORD_PARAM* pRecordParam, const IVS_CHAR* pSaveFileName)
{
	IVS_DEBUG_TRACE("");

	const char *pCamName = NULL;
	IVS_CAMERA_BRIEF_INFO birefinfo;
	memset(&birefinfo, 0, sizeof(birefinfo));
	if (m_pUserMgr)
	{
		std::string strCamcode = m_szCameraID;
		m_pUserMgr->GetDeviceMgr().GetCameraBriefInfobyCode(strCamcode, birefinfo, true);
		pCamName = birefinfo.cName;
	}
    else
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Start Local Record", "User Mgr is NULL");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	IVS_UINT32 pVideoDecoderType = 1;
	IVS_INT32 iRet = IVS_PLAY_GetVideoDecodeType(m_ulPlayerChannel,&pVideoDecoderType);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start Local Record", "Get VideoDecodeType Failed.");
		return iRet;
	}

	if(VIDEO_DEC_H264 != pVideoDecoderType)
	{
		BP_RUN_LOG_ERR(IVS_SDK_LOCALRECORD_UNSUPPORTED_VIDEODECTYPE, "Start Local Record", "VideoDecType is unsupported.");
		if(VIDEO_DEC_MJPEG == pVideoDecoderType)
		{
			return IVS_SDK_LOCALRECORD_UNSUPPORTED_MJPEG;
		}
		return IVS_SDK_LOCALRECORD_UNSUPPORTED_VIDEODECTYPE;
	}

	//ST_MEDIA_RSP stMediaRsp;
	//memset(&stMediaRsp, 0, sizeof(ST_MEDIA_RSP));
	//(void)GetURL(NULL, m_pUserMgr, m_stUrlMediaInfo, stMediaRsp, m_bAssociatedAudio);
	return IVS_PLAY_StartRecord(m_ulPlayerChannel, m_szCameraID, pCamName, pSaveFileName, pRecordParam, true);
}

//停止本地录像
int CMediaBase::StopLocalRecord()
{
	IVS_DEBUG_TRACE("");
	return IVS_PLAY_StopRecord(m_ulPlayerChannel);
}

//当加密码流切换到非加密时需要调用来设置加密标志位为false
IVS_INT32 CMediaBase::SetNoNeedSecretKey(IVS_UINT32 uiChannel, MEDIA_KEY_INFO& stMediaKeyInfo)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    BP_DBG_LOG("CMediaBase::SetNoNeedSecretKey, iVideoKeyType:%d", stMediaKeyInfo.iVideoKeyType);

    //将空密钥设置到MP中
    iRet = IVS_PLAY_SetSecretKey(uiChannel, stMediaKeyInfo.iVideoKeyType, NULL);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Set SecretKey To Mp", "Set No Need Secret Key Error");
        return iRet;
    }

    if (NULL == m_hRtspSetupEvent)
    {
        BP_DBG_LOG("SetNoNeedSecretKey m_hRtspSetupEvent Is Null.");
    }
    // 产生信号量
    bool bIsSuccess = ReleaseSemaphore(m_hRtspSetupEvent, 1, NULL);
    if (bIsSuccess)
    {
        BP_DBG_LOG("SetNoNeedSecretKey ReleaseSemaphore Is Success.");
    }
    else
    {
        BP_DBG_LOG("SetNoNeedSecretKey ReleaseSemaphore Is Fail.");
    }

    return IVS_SUCCEED;
}//lint !e1762

IVS_INT32 CMediaBase::SetSecretKey(IVS_UINT32 uiChannel, MEDIA_KEY_INFO& stMediaKeyInfo)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
   // std::string strSecretKey;
	//定义密钥存储密钥数组
	IVS_CHAR cSercetKey[ENCRY_KEY_LEN + 1] = {0};
    iRet = GetStreamencryKey(cSercetKey, stMediaKeyInfo);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get SecretKey", "Get Secret Key Error");
        return iRet;
    }

    BP_DBG_LOG("CMediaBase::SetSecretKeyLen:%d, VideoKeyType:%d.", strlen(cSercetKey), stMediaKeyInfo.iVideoKeyType);

	//TO DO将密钥设置如MP当中
	iRet = IVS_PLAY_SetSecretKey(uiChannel, stMediaKeyInfo.iVideoKeyType, cSercetKey);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set SecretKey To Mp", "Set Secret Key Error");
        //m_bGetKeyIsSuccess = false;
		return iRet;
	}

    if (NULL == m_hRtspSetupEvent)
    {
        BP_DBG_LOG("m_hRtspSetupEvent Is Null.");
    }
	// 产生信号量
	bool bIsSuccess = ReleaseSemaphore(m_hRtspSetupEvent, 1, NULL);
    if (bIsSuccess)
    {
        BP_DBG_LOG("ReleaseSemaphore Is Success.");
    }
    else
    {
        BP_DBG_LOG("ReleaseSemaphore Is Fail.");
    }
    return IVS_SUCCEED;
}

//进行密钥协商请求录像加密的工作密钥或者异或因子
IVS_INT32 CMediaBase::GetStreamencryKey(IVS_CHAR* cSecretKey,const MEDIA_KEY_INFO& stMediaKeyInfo) const
{
	if (NULL == m_pUserMgr || NULL == cSecretKey)
	{
		return IVS_FAIL;
	}
    //先进行密钥交换
    IVS_INT32 iRet = IVS_FAIL;
    std::string strCameraCode;
    std::string strDomainCode;
    std::string strDestModule = "MU";
    iRet = CToolsHelp::SplitCameraDomainCode(m_szCameraID, strCameraCode, strDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        return iRet;
    }
    // 获取本域LinkID
    CLinkInfo oLinkIDInfo;
    bool bRet = CLinkRouteMgr::instance().GetSmuLinkIDInfoByDomainCode(m_pUserMgr->GetSessionID(), strDomainCode,
																		 oLinkIDInfo);
    if (!bRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Streamencry Key", "Get SmuLinkIDInfo By DomainCode failed");
        return IVS_FAIL;
    }
    // 初始化密钥交换类对象,完成密钥交换
    CSDKSecurityClient oSecurityClient;
    int iInitSecurityClient = oSecurityClient.Init(NET_ELE_SMU_NSS, oLinkIDInfo.GetStrLinkId(), strCameraCode.c_str(),
                                                   strDomainCode.c_str(), strDestModule.c_str());
    if (IVS_SUCCEED != iInitSecurityClient)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Url", "oSecurityClient Init failed");
        return IVS_FAIL;
    }
	std::string strSecurityTransID = oSecurityClient.GetSecurityDataTransID();

    //密钥交换成功，发起NSS_GET_APPLY_STREAMENCRY_PWD_REQ消息获取密钥
    CXml xmlReq;
	//此处需要获取码流类型（主码流/辅码流）
	int iStreanmType = m_stuMediaParam.StreamType;//IVS_STREAM_TYPE
    iRet = CMediaXMLProcess::ApplyStreamencryPwdGetXML(strCameraCode, strDomainCode, strDestModule,iStreanmType,
														stMediaKeyInfo.szSessionID,strSecurityTransID, xmlReq);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Apply Streamencry XML", "get xml false");
        return IVS_FAIL;
    }
    unsigned int xmlLen  = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    BP_DBG_LOG("Get Streamencry Key request xml = %s.", pReq);
    
    //创建要发送的CMD，拼装了NSS消息头
    CCmd *pCmd = CNSSOperator::instance().BuildCmd(oLinkIDInfo.GetStrLinkId(), NET_ELE_SMU_NSS, NSS_GET_APPLY_STREAMENCRY_PWD_REQ, pReq,
                                                   strSecurityTransID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
    HW_DELETE(pCmdRsp);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "GetStreamencryKey Fail.", "iRet:%d", iRet);
        return iRet;
    }

	char szEncryPWD[ENCRY_KEY_LEN+1];
	memset(szEncryPWD, 0, ENCRY_KEY_LEN+1);
	CHECK_POINTER(pRsp, IVS_XML_INVALID);
	CXml xmlRsp;
	if (!xmlRsp.Parse(pRsp))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Apply Streamencry", "xml parsing error");
		IVS_DELETE(pRsp, MUILI);
		return IVS_XML_INVALID;
	}
	iRet = CMediaXMLProcess::ParseStreamencryKey(xmlRsp, szEncryPWD);
	BP_DBG_LOG("GetStreamencryKey EncryPWD:%s,len:%d", szEncryPWD, strlen(szEncryPWD));

    IVS_DELETE(pRsp,MUILI);
	std::string strSecretKey = "";
	iRet = oSecurityClient.DecryptString(szEncryPWD, strSecretKey);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}    

	unsigned int uiStrLen = strSecretKey.size();
    BP_DBG_LOG("GetStreamencryKey|KeyType:%d|KeyLen:%d", stMediaKeyInfo.iVideoKeyType, uiStrLen);

	if ( (XOR_INIT_KEY_LEN + XOR_INIT_SALT_LEN)<= uiStrLen 
		&& (HW_IPC_ENCRYPT_SIMPLE == stMediaKeyInfo.iVideoKeyType ||
			HW_IPC_ENCRYPT_SIMPLE_HEADER == stMediaKeyInfo.iVideoKeyType))
	{
		BP_DBG_LOG("Init SecretKey Key ;%s", strSecretKey.c_str());
		//加密用的密钥，随机数的前16(+1)个字节
		char pszInitKey[XOR_INIT_KEY_LEN + 1] = {0}; 
		memcpy(pszInitKey,&strSecretKey[0],XOR_INIT_KEY_LEN);
		//加密用的初始化向量，随机数的后14个字节
		char pszSalt[XOR_INIT_SALT_LEN + 1] = {0};       
		memcpy(pszSalt,&strSecretKey[XOR_INIT_KEY_LEN],XOR_INIT_SALT_LEN);
		unsigned int uiKeyBufferLen = XOR_INIT_KEY_LEN;
		char pszKeyBuffer[XOR_INIT_KEY_LEN + 1] = {0};  
		//生成异或码流加密的key
		iRet = IVS_Security_Derive_Key((unsigned char*)pszKeyBuffer, uiKeyBufferLen, pszInitKey, 0, pszSalt, 0, 0);
		strSecretKey.clear();
		//strSecretKey = pszKeyBuffer;
		memcpy(cSecretKey,pszKeyBuffer,XOR_INIT_KEY_LEN);
		BP_DBG_LOG("Init SecretKey Len ;%d  , SecretKey Len :%d", uiStrLen,strSecretKey.length());
        return iRet;
	}

    //AES256密钥拷贝
    strncpy(cSecretKey, strSecretKey.c_str(), strSecretKey.length());
    BP_DBG_LOG("AES256 SecretKey:%s , SecretKey Len :%d", cSecretKey, strlen(cSecretKey));
    return iRet;
}

// 本地抓拍单张图片
int CMediaBase::LocalSnapshot(const char *pSavePath, IVS_ULONG ulSnapshotFormat)
{
	IVS_DEBUG_TRACE("");
	return IVS_PLAY_LocalSnapshot(m_ulPlayerChannel, pSavePath, ulSnapshotFormat);
}
// 局部放大本地抓拍单张图片
int CMediaBase::LocalSnapshotRect(const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT* pRect)
{
	IVS_DEBUG_TRACE("");
	return IVS_PLAY_LocalSnapshotRect(m_ulPlayerChannel, pSavePath, ulSnapshotFormat, pRect);
}

int CMediaBase::GetCameraName(IVS_ULONG ulNameLen, char *pCamName)
{
	CHECK_POINTER(pCamName, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");
	IVS_CAMERA_BRIEF_INFO birefinfo;
	memset(&birefinfo, 0, sizeof(birefinfo));
	if (NULL == m_pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get Camera Name", "User Mgr is NULL");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	std::string strCamcode = m_szCameraID;
	m_pUserMgr->GetDeviceMgr().GetCameraBriefInfobyCode(strCamcode, birefinfo, true);
	if (!CToolsHelp::Strncpy(pCamName, ulNameLen, birefinfo.cName, IVS_CAMERA_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Camera Name", "Copy Name Failed");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

//地址转换函数，整型-》字符串
int CMediaBase::AddrUint2Char(const unsigned int name, std::string &addr)
{
    if (0 == name)
    {
        return IVS_FAIL;
    }

    unsigned int segment = 0;
    for (unsigned int i = 0; i < 4; i++)
    {
        //取出最左边的8位
        segment = name;
        segment >>= (8 * (3 - i));
        segment &= 0xff;
        std::stringstream strValue;
        //清除内容
        strValue.str("");
        strValue << (unsigned int)segment;
        addr += strValue.str();
        if (i < 3)
        {
            addr += ".";
        }
    }
    return IVS_SUCCEED;
}

//解析URL,pKey是想要得到的值
int CMediaBase::ParseURL(const char* pUrl, const char* pKey, std::string& strValue)
{
	IVS_DEBUG_TRACE("");
    if (NULL == pUrl)
    {
        return IVS_OPERATE_MEMORY_ERROR;
    }

    if ((URL_LEN < strlen(pUrl)) || (NULL == pKey))
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Parse URL", "ParseURL error, param invalid");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    std::string strSrc = "";

    strSrc.append(pUrl);
    int pos1 = (int)strSrc.find(pKey);
    if ((unsigned int)pos1 == std::string::npos) 
    {
        return IVS_FAIL;
    }
    strSrc = strSrc.substr((unsigned int)pos1,strSrc.size()-(unsigned int)pos1);
    int pos2 = (int)strSrc.find("&");
    if ((unsigned int)pos2 == std::string::npos)
    {
        return IVS_FAIL;
    }
    strValue = strSrc.substr(strlen(pKey), (unsigned int)pos2 - strlen(pKey));
    return IVS_SUCCEED;
}//lint !e1762

int CMediaBase::ParseIPAddr(const char* pUrl, std::string& strValue) const
{
	IVS_DEBUG_TRACE("");
	if (URL_LEN < strlen(pUrl))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Parse URL", "ParseURL error, param invalid");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	std::string strSrc = "";
	strSrc.append(pUrl);
	unsigned int pos1 = strSrc.find("rtsp://");
	if (pos1 == std::string::npos) 
	{
		return IVS_FAIL;
	}
	unsigned int posBegin = pos1 + strlen("rtsp://");
	//往后查找端口号的位置
	unsigned int pos2 = strSrc.find(":",posBegin);
	if (pos2 == std::string::npos)
	{
		return IVS_FAIL;
	}
	strValue = strSrc.substr(posBegin, pos2 - posBegin);
	return IVS_SUCCEED;
}


// 设置画图回调
int CMediaBase::SetDrawCallBack(DRAW_PROC callback, void *pUserData)
{
	// callback 和 pUserData 可以为空
	return IVS_PLAY_SetDrawCallBack(m_ulPlayerChannel, (DRAW_CALLBACK)callback, pUserData);
}

int CMediaBase::SetDrawCallBackEx(HWND hWnd, DRAW_PROC callback, void *pUserData)
{
	return IVS_PLAY_SetDrawCallBackEx(m_ulPlayerChannel, hWnd, (DRAW_CALLBACK)callback, pUserData);
}

int CMediaBase::SetDelayFrameNum(IVS_UINT32 uDelayFrameNum)
{
	return IVS_PLAY_SetDelayFrameNum(m_ulPlayerChannel, uDelayFrameNum);
}

//将UTC时间串格式以YYYY,MM,DD,hh,mm,ss分段转换，输出以秒为单位的日历时间,
//即自国际标准时间公元1970年1月1日00:00:00以来经过的秒数。
time_t CMediaBase::UTCStr2Time(const char *pStr, const char *pFormat)
{
    if (NULL == pStr || NULL == pFormat)
    {
        return 0;
    }

    //先以本地时间换算
    time_t localTime = Str2Time(pStr, pFormat);
    //如果转换失败则直接返回，否则换算成UTC时间
    if(0 == localTime)
    {
        return localTime;
    }
    return localTime - ACE_OS::timezone();
}

//将时间串格式以YYYY,MM,DD,hh,mm,ss分段转换，输出以秒为单位的日历时间,
//即自国际标准时间公元1970年1月1日00:00:00以来经过的秒数,输入时间按照本地时间参考。
time_t CMediaBase::Str2Time(const char *pStr, const char *pFormat)
{
    if ((NULL == pStr) || (NULL == pFormat)
        || (strlen(pStr) != strlen(pFormat)))
    {
        return 0;
    }

    struct tm tmvalue;

    memset(&tmvalue, 0, sizeof(tmvalue));

    const char *pch = pStr;
	char tmpstr[8] = {0};
    int iOffset = 0;

    pch = strchr(pFormat, 'Y');
	CHECK_EQUAL(NULL, pch, 0);
    iOffset = pch - pFormat;
    bool bRet = CToolsHelp::Memcpy(tmpstr,8, pStr + iOffset, 4);
	if(false == bRet)
	{
		return 0;
	}
    tmpstr[4] = '\0';
    tmvalue.tm_year = atoi(tmpstr) - 1900;
    if (0 > tmvalue.tm_year)
    {
        return 0;
    }

    pch = strchr(pFormat, 'M');
    CHECK_EQUAL(NULL, pch, 0);
    iOffset = pch - pFormat;
    bRet = CToolsHelp::Memcpy(tmpstr,8, pStr + iOffset, 2);
	if(false == bRet)
	{
		return 0;
	}
    tmpstr[2] = '\0';
    tmvalue.tm_mon = atoi(tmpstr) - 1;
    if (0 > tmvalue.tm_mon)
    {
        return 0;
    }

    pch = strchr(pFormat, 'D');
    CHECK_EQUAL(NULL, pch, 0);
    iOffset = pch - pFormat;
    bRet = CToolsHelp::Memcpy(tmpstr,8, pStr + iOffset, 2);
	if(false == bRet)
	{
		return 0;
	}
    tmpstr[2] = '\0';
    tmvalue.tm_mday = atoi(tmpstr);
    if (0 > tmvalue.tm_mday)
    {
        return 0;
    }

    pch = strchr(pFormat, 'h');
    CHECK_EQUAL(NULL, pch, 0);
    iOffset = pch - pFormat;
    bRet = CToolsHelp::Memcpy(tmpstr,8, pStr + iOffset, 2);
	if(false == bRet)
	{
		return 0;
	}
    tmpstr[2] = '\0';
    tmvalue.tm_hour = atoi(tmpstr);
    if (0 > tmvalue.tm_hour)
    {
        return 0;
    }

    pch = strchr(pFormat, 'm');
    CHECK_EQUAL(NULL, pch, 0);
    iOffset = pch - pFormat;
    bRet = CToolsHelp::Memcpy(tmpstr,8, pStr + iOffset, 2);
	if(false == bRet)
	{
		return 0;
	}
    tmpstr[2] = '\0';
    tmvalue.tm_min = atoi(tmpstr);
    if (0 > tmvalue.tm_min)
    {
        return 0;
    }

    pch = strchr(pFormat, 's');
    CHECK_EQUAL(NULL, pch, 0);
    iOffset = pch - pFormat;
    bRet = CToolsHelp::Memcpy(tmpstr,8, pStr + iOffset, 2);
	if(false == bRet)
	{
		return 0;
	}
    tmpstr[2] = '\0';
    tmvalue.tm_sec = atoi(tmpstr);
    if (0 > tmvalue.tm_sec)
    {
        return 0;
    }

    return ACE_OS::mktime(&tmvalue);
}
//将日历时间（自国际标准时间公元1970年1月1日00:00:00以来经过的秒数）转换为形如"20090630091030"的字符串
void CMediaBase::Time2Str(const time_t t, std::string& strTime, const std::string& strFormat)
{
    struct tm tmv;
    memset(&tmv, 0, sizeof(tmv));
#ifdef WIN32
    //ACE_OS::memcpy(&tmv, ACE_OS::gmtime(&t), sizeof(tmv));
	gmtime_s(&tmv,&t);
#else
    if(NULL == localtime_r(&t, &tmv))
    {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) %N, %l\n\tAn error occurred when the international"
            "standard time [%d] was converted to the local time.\n"), t));
    }
#endif

    char pDestBuf[IVS_TIME_LEN] = {0};
    (void)CToolsHelp::Snprintf(pDestBuf, IVS_TIME_LEN, "%04d%02d%02d%02d%02d%02d",
                         tmv.tm_year + CBB_YEAR_BASE, tmv.tm_mon + 1, tmv.tm_mday, tmv.tm_hour,
                         tmv.tm_min, tmv.tm_sec);

    strTime = pDestBuf;

    if (0 == strFormat.compare(IVS_DATE_TIME_NET_FORMAT))
    {
        strTime.insert(8, "T");
        strTime.insert(15, "Z");
    }

    return;
}

//将UTC时间串格式以YYYY,MM,DD,hh,mm,ss分段转换，输出以秒为单位的日历时间,即自国际标准时间公元1970年1月1日00:00:00以来经过的秒数。
void CMediaBase::UTCTime2Str(const time_t t, std::string& strTime, const std::string& strFormat)
{   
    // 将UTC的time_t转换为本地的time_t
    //time_t tLocalTime = + ACE_OS::timezone();
    Time2Str(t, strTime, strFormat);
}

int CMediaBase::GetRtspErrorCode(int iErrorCode)
{
	int iSize = sizeof(s_RtspErrorCodes) / sizeof(RTSP_ERROR_CODE);
	for ( int i = 0; i < iSize; i++ )
	{
		if ( s_RtspErrorCodes[i].iRtspErrorCode == iErrorCode )
		{
			return s_RtspErrorCodes[i].iIvsErrorCode;
		}
	}

	BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_ERR_RET_SYS, "Get Rtsp Error Code", "errorcode[%d] do not exist,use nomarl error instead.", iErrorCode);
	return IVS_PLAYER_RET_RTSP_ERR_RET_SYS;
}

// 通知流缓冲区状态
IVS_INT32 CMediaBase::NotifyStreamBufferState(IVS_INT32 iBufferState)
{
    IVS_INT32 iRet = IVS_FAIL;

    if (MEDIA_STATUS_FREE == GetStatus())
    {
        BP_RUN_LOG_ERR(iRet, "NotifyStreamBufferState", "Get Status [%d], media base is free.", m_iStatus);
        return iRet;
    }
    
    // 1 表示缓冲区达到最大阈值
    if (1 == iBufferState)
    {
		RTSP_SESSION_STATUS iState = RTSP_CLIENT_GetStatus(m_iRtspHandle);
        BP_DBG_LOG("Client VCR control, Buffer over max, rtsp client status[%d].", iState);
        if (RTSP_SESSION_STATUS_READY == iState 
			|| RTSP_SESSION_STATUS_PLAYING == iState)
        {
            iRet = RTSP_CLIENT_SendPauseMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
            if (0 != iRet)
            {
                BP_RUN_LOG_ERR(iRet, "NotifyStreamBufferState", "RTSP client send pause fail.");
                return IVS_PLAYER_RET_RTSP_SEND_PAUSE_FAIL;
            }
            m_bControlPause = true;
        }
        else
        {
            BP_DBG_LOG("over max, rtsp client no playing.");
        }
    }
    // 0 表示缓冲区达到最小阈值
    else if (0 == iBufferState)
    {
        RTSP_SESSION_STATUS iState = RTSP_CLIENT_GetStatus(m_iRtspHandle);
        BP_DBG_LOG("Client VCR control, Buffer low min. rtsp client status[%d].",iState);

		if (RTSP_SESSION_STATUS_PLAYING != iState)
		{
			float fSendScale = 0.0;
			bool bRet = CToolsHelp::GetSendScale(m_enServiceType, m_fSpeed, fSendScale);
			if (!bRet)
			{
				BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL,"NotifyStreamBufferState", "Get Send Scale m_fSpeed[%f] fSendScale[%f].", m_fSpeed, fSendScale);
				return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
			}

			MEDIA_RANGE_S stRange;
			memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
			stRange.enRangeType = RANGE_TYPE_UTC;
			stRange.MediaBeginOffset = OFFSET_CUR;
			iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)fSendScale, &stRange, RTSP_CLIENT_TIMEOUT);
			if (0 != iRet)
			{
				BP_RUN_LOG_ERR(iRet,"NotifyStreamBufferState", "RTSP_CLIENT_SendPlayMsg fail. SendScale[%f].", fSendScale);
				return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
			}
            m_bControlPause = false;
		}
		else
		{
			BP_DBG_LOG("over min, rtsp client is playing, do not need start play again.");
		}
		
    }
    else
    {
        BP_RUN_LOG_ERR(iRet,"Notify stream buffer state", "MP notify state, iBufferState [%d] value error.", iBufferState);
    }
    return iRet;
}

void CMediaBase::InitMediaPara(IVS_MEDIA_PARA& stMediaPara) const
{
	memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
	stMediaPara.DecoderCompany = VENDOR_HUAWEI;
	stMediaPara.StreamType = STREAM_TYPE_MAIN;
	stMediaPara.TransMode = MEDIA_TRANS;
	stMediaPara.BroadCastType = BROADCAST_UNICAST;
	stMediaPara.VideoDecType = VIDEO_DEC_H264;
	stMediaPara.AudioDecType = AUDIO_DEC_G711A;
}

IVS_INT32 CMediaBase::GetSourceBufferRemain(IVS_UINT32 &uBufferCount)
{
	return IVS_PLAY_GetSourceBufferRemain(m_ulPlayerChannel, uBufferCount);
}

/*
 * SetMeidaAttribute
 * 设置Payload type add by w00210470
 */
IVS_INT32 CMediaBase::SetMeidaAttribute(const MEDIA_ATTR &VideoAttr, const MEDIA_ATTR &AudioAttr)//lint !e10
{//lint !e10
	return IVS_PLAY_SetMediaAttribute(m_ulPlayerChannel, &VideoAttr, &AudioAttr);
}

void CMediaBase::SetSecretFlag(bool bGetKeyIsSuccess) //lint !e18
{//lint !e18 !e745
    m_bGetKeyIsSuccess = bGetKeyIsSuccess;
}//lint !e533

bool CMediaBase::GetSecretFlag()const
{
    return m_bGetKeyIsSuccess;
}

void CMediaBase::SetSessionInfo(char* pSessionUrl, char* pSessionId)
{
    IVS_DEBUG_TRACE("");
    BP_DBG_LOG("SetSessionInfo, SessionUrl:%s, SessionId:%s", pSessionUrl, pSessionId);
    IVS_NETSOURCE_SetSessionInfo(m_ulNetSourceChannel, pSessionUrl, pSessionId);
}
