//#include "StdAfx.h"
#include "AudioObj.h"
#include "LockGuard.h"
#include "IVS_Player.h"
#include "UserMgr.h"
#include <io.h>

#define MAX_BROADCAST_COUNT				256
//文件广播类型
//#define VOICE_FILE_BROADCAST 0			//文件语音广播
//#define VOICE_REAL_BROADCAST 1			//实时语音广播

unsigned int CAudioObj::m_stcAudioType = 0;

// Rtsp媒体数据回调
static void  __SDK_CALL g_fRtspDataCallBack(int /*handle*/, 
    char *pBuffer,
    unsigned int ulBufSize,
    void *pUser)//lint !e528 
{
    //入参有效性检查
    if ((NULL == pBuffer) 
        || (0 == ulBufSize))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Audio Obj g_fRtsp Data Call Back", "Input param invalid.");
        return;
    }

    if (*pBuffer != '$')
    {
        BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_DATA_ERROR, "Audio Obj g_fRtsp Data Call Back", "rtsp media data format error, do not have $.");
        return;
    }
    if (1 == *(pBuffer+1) || 3 == *(pBuffer+1))
    {
        // RTCP保活处理，传空字符串进去
        //(void)IVS_InputData(ulPort, NULL, 0);
        return;
    }

    //获取回调出来的类对象;
    CMediaBase* pMediaBase = static_cast<CMediaBase*>(pUser);
    if (NULL == pMediaBase)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Audio Obj g_fRtsp Data Call Back", "transform pUser to MediaBase pointer failed.");
        return;
    }

    unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
    if (0 == uiPlayerChannel)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Audio Obj g_fRtsp Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
        return;
    }

    pBuffer   += 4;
    ulBufSize -= 4;
    (void)IVS_PLAY_InputRtpData(uiPlayerChannel, pBuffer, ulBufSize);
    return;
} //lint !e818 !e528

static void __SDK_CALL g_fNetDataCallBack( char *pBuf, unsigned int uiSize, void *pUser)//lint !e818 !e830
{
    //获取回调出来的类对象;
    CMediaBase* pMediaBase = reinterpret_cast<CMediaBase*>(pUser);
    if (NULL == pMediaBase)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"AudioObj Net Data Call Back", "transform pUser to MediaBase pointer failed.");
        return;
    }

    unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
    if (0 == uiPlayerChannel)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"AudioObj Net Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
        return;
    }

    (void)IVS_PLAY_InputRtpData(uiPlayerChannel, pBuf, uiSize);
    return;
}//lint !e818 !e528

CAudioObj::CAudioObj(void)
{
	m_bIsBroadcastOpenedFlag = false;
	m_CodeHandle.clear();
	memset(&m_mediaPara,0,sizeof(m_mediaPara));
	memset(m_fileName,0,FILE_NAME_LEN+1);
	m_iFileBroadcastCycle = -1;
	m_iBroadcastType = -1;
	InitializeCriticalSection(&m_csCodeHandleLock);
}

CAudioObj::~CAudioObj(void)
{
	//m_CodeHandle.clear();//lint !e1551
    try
	{
		ClearUpCodeHandle();
		DeleteCriticalSection(&m_csCodeHandleLock);
	}
	catch(...)
	{

	}
}

void CAudioObj::SetHandle(unsigned long ulHandle)
{
    m_ulHandle = ulHandle;
}//lint !e1762

//清除数据
void CAudioObj::ClearUp()
{
	StopFileBroadcast();//lint !e534
	StopRealBroadcast();//lint !e534
	m_bIsBroadcastOpenedFlag = false;
    ClearUpCodeHandle();
	//m_CodeHandle.clear();
	memset(&m_mediaPara,0,sizeof(m_mediaPara));
	memset(m_fileName,0,FILE_NAME_LEN+1);
	m_iFileBroadcastCycle = -1;
	m_iBroadcastType = -1;
}

//获取协议类型
ULONG CAudioObj::GetProtocolType(char *pProtocol, ULONG ulProtocolType)
{
    ulProtocolType -= 1;
    switch (ulProtocolType)
    {
    case PROTOCOL_RTP_OVER_UDP:
        (void)strncpy(pProtocol, "UDP", strlen("UDP"));
        return PROTOCOL_RTP_OVER_UDP;
    case PROTOCOL_RTP_OVER_TCP:
        (void)strncpy(pProtocol, "TCP", strlen("TCP"));
        return PROTOCOL_RTP_OVER_TCP;
    default:
        (void)strncpy(pProtocol, "UDP", strlen("UDP"));
        return PROTOCOL_RTP_OVER_UDP;
    }
}//lint !e1762


//开始语音对讲
int CAudioObj::StartVoiceTalkback(unsigned int uiProtocolType,
                                  IVS_BOOL bDirectFirst,
                                  const char* pCameraCode)
{
    CLockGuard lock(m_pMediaMutex);

    // add by wanglei 00165153:2013.6.13 DTS2013060904823
    if (m_bIsBroadcastOpenedFlag)
    {
        BP_RUN_LOG_ERR(IVS_SDK_BROADCASE_ERR_ALREADY_OPENED,"Start Voice Talkback", "broadcast opened");
        return IVS_SDK_BROADCASE_ERR_ALREADY_OPENED;
    }
    
    IVS_MEDIA_PARA stMediaPara;
    memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
    stMediaPara.ProtocolType = static_cast<IVS_PROTOCOL_TYPE>(uiProtocolType);//此处转换肯定正确，前面已做检查;

    // 设置默认值;
    stMediaPara.DecoderCompany = VENDOR_HUAWEI;
    stMediaPara.StreamType = STREAM_TYPE_UNSPECIFIED;
    stMediaPara.TransMode = static_cast<IVS_MEDIA_TRANS_MODE>(bDirectFirst);//此处转换肯定正确，前面已做检查;
    stMediaPara.BroadCastType = BROADCAST_UNICAST;
    stMediaPara.VideoDecType = VIDEO_DEC_H264;
    stMediaPara.AudioDecType = AUDIO_DEC_G711U;

    START_PLAY_PARAM stParam;
    stParam.enServiceType = SERVICE_TYPE_AUDIO_CALL;
    stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
    stParam.cbNetDataCallBack = g_fNetDataCallBack;
	// 设置对讲类型
	SetServiceType(SERVICE_TYPE_AUDIO_CALL);
    //开始语音对讲;
    int iRet = StartPlay(&stMediaPara, pCameraCode, stParam);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Talkback", "Start play error.");
        return iRet;
    }

    iRet = GetSupportAudioType(CAudioObj::m_stcAudioType);

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Talkback",
                       "Get Support AudioType error.");
        (void)CMediaBase::StopPlay();
        return iRet;
    }

    IVS_ULONG ulVoiceType = 0;

    if (AUDIO_DEC_G711U == CAudioObj::m_stcAudioType)
    {
        ulVoiceType = PAY_LOAD_TYPE_PCMU;
    }
    else if (AUDIO_DEC_G711A == CAudioObj::m_stcAudioType)
    {
        ulVoiceType = PAY_LOAD_TYPE_PCMA;
    }

    iRet = IVS_PLAY_SetVoiceDecoderType(ulVoiceType);

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Talkback", "Set VoiceDecoder Type.");
        (void)CMediaBase::StopPlay();

        return iRet;
    }

    iRet = IVS_PLAY_StartVoiceTalkback((IVS_CHAR*)m_strLocalIp.c_str(),
                                       (IVS_ULONG)m_uLocalPort,
                                       (IVS_CHAR*)m_szRemoteIp,
                                       (IVS_ULONG)m_uRemotePort);

    if (IVS_SUCCEED != iRet)
    {
        (void)CMediaBase::StopPlay();
        BP_RUN_LOG_ERR(iRet, "Start Voice Talkback",
                       "IVS PLAY Start Voice Talkback failed.");
        return iRet;
    }

    return IVS_SUCCEED;
}//lint !e818

//停止语音对讲
int CAudioObj::StopVoiceTalkback()
{
	/*if(NULL == pOutVoiceRtspInfo)
	{
		BP_RUN_LOG_ERR(iRet, "","CAudioObj::StopVoiceTalkback NULL == pOutVoiceRtspInfo");
		return IVS_PARA_INVALID;
	}*/

	int iRet = IVS_PLAY_StopVoiceTalkback();//lint !e732
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_SDK_VOICE_ERR_CALL_MLIB_FAIL, "AudioObj StopVoiceTalkback",
            "CAudioObj::StopVoiceTalkback IVS_PLAY_StopVoiceTalkback failed iRet:%d", iRet);
        return IVS_SDK_VOICE_ERR_CALL_MLIB_FAIL;
	}

    iRet += CMediaBase::StopPlay();

	return IVS_SUCCEED;
}//lint !e1762

void CAudioObj::AddCodeHandle(const IVS_CHAR* pCameraCode,
                              BROADCAST_RTSP_INFO* pRtspInfo)
{
	CHECK_POINTER_VOID(pCameraCode);
	CHECK_POINTER_VOID(pRtspInfo);

	EnterCriticalSection(&m_csCodeHandleLock);
    
    std::string strCameraCode = pCameraCode;
    CODE_HANDLE_ITER iter = m_CodeHandle.find(strCameraCode);
    if (iter != m_CodeHandle.end())
    {
        BROADCAST_RTSP_INFO* pstInfo = iter->second;
        memcpy(pstInfo, pRtspInfo, sizeof(BROADCAST_RTSP_INFO));
    }
    else
    {
        (void)m_CodeHandle.insert(make_pair(strCameraCode, pRtspInfo));
    }

    //m_CodeHandle.insert(strVoiceCode, pRtspInfo);
	//m_CodeHandle[strCameraCode] = pRtspInfo;
	LeaveCriticalSection(&m_csCodeHandleLock);
}

BROADCAST_RTSP_INFO* CAudioObj::GetHandleByCode(const IVS_CHAR* pCameraCode)
{
	BROADCAST_RTSP_INFO* pRtspInfo= NULL;
	EnterCriticalSection(&m_csCodeHandleLock);
	std::string strTmp = pCameraCode;
	CODE_HANDLE_ITER iter = m_CodeHandle.find( strTmp );
	if ( iter != m_CodeHandle.end() )
	{
		pRtspInfo = iter->second;//lint !e64
	}
	LeaveCriticalSection(&m_csCodeHandleLock);
	return pRtspInfo;
}

void CAudioObj::DeleteCodeHandle( const IVS_CHAR* pCameraCode )
{
	EnterCriticalSection(&m_csCodeHandleLock);
	std::string strTmp = pCameraCode;
	CODE_HANDLE_ITER iter = m_CodeHandle.find( strTmp );
	if ( iter != m_CodeHandle.end() )
	{
        BROADCAST_RTSP_INFO* pRtspInfo = dynamic_cast<BROADCAST_RTSP_INFO*>(iter->second);
		m_CodeHandle.erase( iter );
        IVS_DELETE(pRtspInfo);
	}
    if (m_CodeHandle.empty())
    {
        // modify by wanglei 00165153:2013.4.26 如果目前不存在广播设备，默认目前处于未开启状态，否则下次加设备会出现音频格式冲突
        //m_bIsBroadcastOpenedFlag = false;
        CAudioObj::m_stcAudioType = 0;
    }
	LeaveCriticalSection(&m_csCodeHandleLock);
}

void CAudioObj::ClearUpCodeHandle()
{
    CODE_HANDLE_ITER iter = m_CodeHandle.begin();
    for( ; iter != m_CodeHandle.end(); iter++)
    {
        BROADCAST_RTSP_INFO* pRtspInfo = dynamic_cast<BROADCAST_RTSP_INFO*>(iter->second);
        iter = m_CodeHandle.erase(iter);
        IVS_DELETE(pRtspInfo);
    }
}

//添加广播镜头
int CAudioObj::AddBroadcastDevice(const IVS_CHAR* pCameraCode)
{
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "AudioObj AddBroadcastDevice",
            "CAudioObj::AddBroadcastDevice NULL == pCameraCode");
		return IVS_PARA_INVALID;
	}

	int strVoiceCodeLen = (int)strlen(pCameraCode);
	if(strVoiceCodeLen<= 0 || strVoiceCodeLen>IVS_DEV_CODE_LEN)
	{
		return IVS_PARA_INVALID;
	}


	//判断是否已经在列表中加入此设备，若是，那么直接返回，保证每个设备只添加一次
	EnterCriticalSection(&m_csCodeHandleLock);

	//CODE_HANDLE_ITER codeIter = m_CodeHandle.begin();
    // add by wanglei 00165153:2013.4.27
    unsigned int unCodeHandleSize = m_CodeHandle.size();
    CODE_HANDLE_ITER codeIter = m_CodeHandle.find(pCameraCode);
    if (m_CodeHandle.end() != codeIter)
    {
        LeaveCriticalSection(&m_csCodeHandleLock);
        return IVS_SDK_VOICE_ERR_DEV_ALREADY_IN;//设备已加入设备列表。
    }
    LeaveCriticalSection(&m_csCodeHandleLock);

	//如果设备组中的设备数已经为最大,不允许再添加
	if ( MAX_BROADCAST_COUNT <= unCodeHandleSize)
	{
		return IVS_SDK_VOICE_ERR_TOO_MUCH_DEVICES;
	}

    //如果没有开启广播，只是添加设备，handle值设为-1
    BROADCAST_RTSP_INFO *pRtspInfo = NULL;
    pRtspInfo = IVS_NEW(pRtspInfo);

    if (NULL == pRtspInfo)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Add Broadcast Device",
            "NEW BROADCAST RTSP INFO error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    // start add by wanglei 00165153:2013.5.3 语音编码类型冲突检测
    // 原先该检测放在start中进行，DTS2013040903851 需要在添链表的实际就需要检测，故正此进行音频编码格式一致性检测
    unsigned int unAudioType = 0;
    int iRet = GetSupportAudioType(pCameraCode, unAudioType);
    if (iRet != IVS_SUCCEED)
    {
        BP_RUN_LOG_ERR(iRet, "add broadcast device", "fail to get support audio type, camera code:%s", pCameraCode);
        return iRet;
    }
    BP_RUN_LOG_INF("add broadcast device", "success to get support audio type, camera code:%s, support audio type:%d", 
        pCameraCode, unAudioType);
    
    unsigned int unPayLoadType = 0;
    if (AUDIO_DEC_G711U == unAudioType)
    {
        unPayLoadType = PAY_LOAD_TYPE_PCMU;
    }
    else/* if (AUDIO_DEC_G711A == unAudioType)*/
    {
        unPayLoadType = PAY_LOAD_TYPE_PCMA;
    }

    // 设备表中有数据，并且语音编码类型与之前的不一样，则拒绝添加
    if (0 < unCodeHandleSize && CAudioObj::m_stcAudioType != unAudioType)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Broadcast", "audio pay load type conflict, current-type:%d, requet-type:%d", 
            CAudioObj::m_stcAudioType, unAudioType);
        return IVS_SDK_VOICE_ERR_AUDIO_TYPE_CONFLICT;
    }

    CAudioObj::m_stcAudioType = unAudioType;
  
    iRet = IVS_PLAY_SetVoiceDecoderType(unPayLoadType);

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Broadcast", "Set VoiceDecoder Type.");
        return iRet;
    }
    // end add by wanglei 00165153:2013.5.3

    memset(pRtspInfo, 0, sizeof(BROADCAST_RTSP_INFO));
    AddCodeHandle(pCameraCode, pRtspInfo);

	iRet = IVS_FAIL;
	//判断是否已经开启广播，如果是那么添加此镜头后要启动广播，否则只添加入镜头列表
	if(m_bIsBroadcastOpenedFlag)
	{
		//调用启动广播接口 如果开启成功，会在函数内部修正对应设备的handle值
		iRet = StartSingleBroadcast(m_iBroadcastType, pCameraCode,
                                    m_fileName,m_iFileBroadcastCycle);
		if(IVS_SUCCEED != iRet)//如果开启失败直接返回
		{
            // modify by wanglei 00165153:2013.4.26 未成功开启广播，需要删除之前预加的设备
            DeleteCodeHandle(pCameraCode); 
			return iRet;
		}	
	}

	return IVS_SUCCEED;
}

//删除广播镜头
int CAudioObj::DeleteBroadcastDevice(const IVS_CHAR* pCameraCode)
{
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "AudioObj DeleteBroadcastDevice",
            "CAudioObj::DeleteBroadcastDevice NULL == pCameraCode");
		return IVS_PARA_INVALID;
	}

	int strVoiceCodeLen = (int)strlen(pCameraCode);
	if(strVoiceCodeLen<= 0 || strVoiceCodeLen>IVS_DEV_CODE_LEN)
	{
		return IVS_PARA_INVALID;
	}
    
	BROADCAST_RTSP_INFO* pRtspInfo = GetHandleByCode(pCameraCode);
	if ( NULL == pRtspInfo )
	{
		return IVS_SDK_VOICE_ERR_DEV_NOT_IN;//删除的设备不在广播列表中
	}

	//判断是否已经开启广播，如果是那么删除此镜头后要停止其广播，否则只删除
	if(m_bIsBroadcastOpenedFlag)
	{
		//调用停止单路广播接口
		int iRet = IVS_PLAY_StopVoiceBroadcast((IVS_CHAR*)pRtspInfo->ServiceIP,
                                               pRtspInfo->ServicePort);
		if ( IVS_SUCCEED != iRet )
		{
			return IVS_SDK_VOICE_ERR_STOP_BROADCAST_FAIL;//失败直接返或错误，不删除设备列表中的对应项
		}

        // modify by wanglei 00165153:2013.4.26 不发送停止，再次进行广播MU就会返回失败
        iRet += StopPlay(pRtspInfo); // 发送RTSP停止广播
	}

	//从列表中删除
	DeleteCodeHandle(pCameraCode);
	return IVS_SUCCEED;
}
	
//开始语音广播
int CAudioObj::StartSingleBroadcast(int ulBroadcastType,
                                    const IVS_CHAR* pCameraCode,
                                    const IVS_CHAR* fileName,
                                    int iCycle)
{
    if (NULL == pCameraCode)
    {
        return IVS_PARA_INVALID;
    }

    EnterCriticalSection(&m_csCodeHandleLock);
    if ( MAX_BROADCAST_COUNT-1 < m_CodeHandle.size() )
    {
        LeaveCriticalSection(&m_csCodeHandleLock);
        return IVS_SDK_VOICE_ERR_TOO_MUCH_DEVICES;
    }
    LeaveCriticalSection(&m_csCodeHandleLock);

    CLockGuard lock(m_pMediaMutex);

    IVS_MEDIA_PARA stMediaPara;
    memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
    stMediaPara.ProtocolType = PROTOCOL_RTP_OVER_UDP;

    // 设置默认值;
    stMediaPara.DecoderCompany = VENDOR_HUAWEI;
    stMediaPara.StreamType = STREAM_TYPE_UNSPECIFIED;
    stMediaPara.TransMode = MEDIA_TRANS;
    stMediaPara.BroadCastType = BROADCAST_UNICAST;
    stMediaPara.VideoDecType = VIDEO_DEC_H264;
    stMediaPara.AudioDecType = AUDIO_DEC_G711U;

    START_PLAY_PARAM stParam;
    stParam.enServiceType = SERVICE_TYPE_AUDIO_BROADCAST;
    stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
    stParam.cbNetDataCallBack = g_fNetDataCallBack;
    //开始语音对讲;
    int iRet = StartPlay(&stMediaPara, pCameraCode, stParam);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"StartSingleBroadcast", "Start play error.");
        return iRet;
    }

    // delete by wanglei 00165153:2013.5.3 添加链表的时候已经进行了音频编码一致性检测DTS2013040903851 ，
    // 这里无需进行检测，所有进行屏蔽

    // add for 检查新增镜头的音频格式是否和第一路相同
    //iRet = CheckAudioType();
    //if (IVS_SUCCEED != iRet)
    //{
    //    (void)CMediaBase::StopPlay();
    //    return iRet;
    //}
    // end add

    iRet = IVS_PLAY_StartVoiceBroadcast(m_strLocalIp.c_str(),
                                        (IVS_ULONG)m_uLocalPort,
                                        (IVS_CHAR*)m_szRemoteIp,
                                        (IVS_ULONG)m_uRemotePort,
                                        (IVS_ULONG)ulBroadcastType,
                                        fileName, //lint !e1773
                                        (bool)iCycle); //lint !e1786

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Broadcast",
                       "Start Voice Broadcast fail with %d.", iRet);
        (void)CMediaBase::StopPlay();

        // modofy by wanglei 00165153:2013.6.13 DTS2013060904823
        // 如果是找不到音频设备的错误原因，具有很强的适配性，需要返回该错误码
        return IVS_PLAYER_RET_AUDIO_DEVICE_NOT_FIND == iRet ? IVS_PLAYER_RET_AUDIO_DEVICE_NOT_FIND : IVS_SDK_VOICE_ERR_CALL_MLIB_FAIL;
    }

    BROADCAST_RTSP_INFO *pRtspInfo = NULL;
    pRtspInfo = GetHandleByCode(pCameraCode);

    if (NULL == pRtspInfo)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Broadcast",
            "NEW BROADCAST RTSP INFO error.");
        (void)IVS_PLAY_StopVoiceBroadcast((IVS_CHAR*)m_szRemoteIp,
            (IVS_ULONG)m_uRemotePort);
        (void)CMediaBase::StopPlay();
        return IVS_FAIL;
    }

    (void)CToolsHelp::Strncpy(pRtspInfo->szUrl,
                              IVS_URL_LEN,
                              m_stMediaRsp.szURL,
                              strlen(m_stMediaRsp.szURL));

    (void)CToolsHelp::Strncpy(pRtspInfo->ServiceIP,
                              IVS_IP_LEN,
                              m_szRemoteIp,
                              strlen(m_szRemoteIp));

    //int iPort = CToolsHelp::StrToInt(m_stMediaRsp.szRTSPPORT);
    pRtspInfo->ServicePort = m_uRemotePort;

    pRtspInfo->RtspHandle = (ULONG)m_iRtspHandle;
    pRtspInfo->PlayHandle = m_ulPlayerChannel;
    pRtspInfo->NetSourceHandle = m_ulNetSourceChannel;

    //此时调用AddCodeHandle用于修改pVoiceCode对应的handle值
    //AddCodeHandle(pCameraCode, pRtspInfo);
    return IVS_SUCCEED;
}//lint !e818

//开始语音广播
int CAudioObj::StartRealBroadcast()
{
	//判断是否已开启广播，是则返回
	if(m_bIsBroadcastOpenedFlag)
	{
		return IVS_SDK_BROADCASE_ERR_ALREADY_OPENED;
	}

	//备份媒体参数
	//memcpy(&m_mediaPara,pMediaPara,sizeof(IVS_MEDIA_PARA));

	int iRet = IVS_FAIL;
	m_iBroadcastType = VOICE_REAL_BROADCAST;

	std::string pCameraCode;

	//循环遍历设备列表，开启广播
	EnterCriticalSection(&m_csCodeHandleLock);

	int codesSize = (int)m_CodeHandle.size();
	if(codesSize <= 0)
	{
		LeaveCriticalSection(&m_csCodeHandleLock);
		return IVS_SDK_BROADCASE_ERR_NO_DEVICE;
	}

	//全部开启失败需要返回错误
	bool bIsAnySucceed = false;
    m_ulHandle = (unsigned long)this;
	//下面开始遍历设备组并一一开启广播
	CODE_HANDLE_ITER iter = m_CodeHandle.begin();
	for( ;iter != m_CodeHandle.end();iter++)
	{
		pCameraCode = iter->first;
		iRet = StartSingleBroadcast(m_iBroadcastType, pCameraCode.c_str());
		if(IVS_SUCCEED != iRet)//如果某路开启失败，不处理，继续开启其他设备
		{
			//写日志
			BP_RUN_LOG_ERR(IVS_SDK_VOICE_ERR_ALL_BROADCAST_FAIL, "AudioObj StartRealBroadcast",
                "CAudioObj::StartRealBroadcast StartSingleBroadcast failed iRet:%d pCameraCode:%s",
                iRet, pCameraCode.c_str());
			
			//上报告警
			EventCallBack fnCallBack = m_pUserMgr->GetEventCallBack();
			if (NULL == fnCallBack)
			{
				BP_RUN_LOG_ERR(IVS_SDK_VOICE_ERR_CANNOT_REPORT_FAIL, "AudioObj StartRealBroadcast",
                    "CAudioObj::StartRealBroadcast NULL == fnCallBack");
				LeaveCriticalSection(&m_csCodeHandleLock);
				return IVS_SDK_VOICE_ERR_CANNOT_REPORT_FAIL;
			}

            STREAM_EXCEPTION_INFO exceptionInfo;
            memset(&exceptionInfo, 0, sizeof(exceptionInfo));
            exceptionInfo.iSessionID = m_pUserMgr->GetSessionID();
            exceptionInfo.ulHandle = m_ulHandle;
           
            strncpy(exceptionInfo.szDeviceCode, pCameraCode.c_str(), IVS_DEV_CODE_LEN - 1);
            exceptionInfo.iExceptionCode = IVS_EVENT_BROADCAST_FAILED;

			fnCallBack(IVS_EVENT_BROADCAST_FAILED, &exceptionInfo,
                sizeof(exceptionInfo), m_pUserMgr->GetEventUserData());
		}
		else
		{
			bIsAnySucceed = true;//只要有一路开启成功就返回成功
		}
		
	}
	LeaveCriticalSection(&m_csCodeHandleLock);
	
	if(bIsAnySucceed)
	{
		m_bIsBroadcastOpenedFlag = true;
		return IVS_SUCCEED;
	}
	else
	{
        // modofy by wanglei 00165153:2013.6.13 DTS2013060904823
        // 如果是找不到音频设备的错误原因，具有很强的适配性，需要返回该错误码
        return IVS_PLAYER_RET_AUDIO_DEVICE_NOT_FIND == iRet ? IVS_PLAYER_RET_AUDIO_DEVICE_NOT_FIND : IVS_SDK_VOICE_ERR_ALL_BROADCAST_FAIL;
		// return IVS_SDK_VOICE_ERR_ALL_BROADCAST_FAIL;
	}
    
}

//停止单路语音广播
int CAudioObj::StopSingleVoiceBroadcast(const BROADCAST_RTSP_INFO* pRtspInfo)
{
    if (NULL == pRtspInfo)
    {
        return IVS_PARA_INVALID;
    }

	int iRet =  IVS_PLAY_StopVoiceBroadcast((IVS_CHAR*)pRtspInfo->ServiceIP,
                                            pRtspInfo->ServicePort);

	if ( IVS_SUCCEED != iRet )
	{
		return IVS_SDK_VOICE_ERR_CALL_MLIB_FAIL;
	}

    iRet += StopPlay(pRtspInfo);
	
	//停止广播时不要删除设备，除非用户自行调用删除接口删除
	//DeleteCodeHandle(pCameraCode);
	
	return iRet;
}//lint !e1762

int CAudioObj::StopPlay(const BROADCAST_RTSP_INFO* pRtspInfo) const
{
    if (NULL == pRtspInfo)
    {
        return IVS_PARA_INVALID;
    }

    int iRet = RTSP_CLIENT_SendTeardownMsg((int)pRtspInfo->RtspHandle,
                                           pRtspInfo->szUrl,
                                           RTSP_CLIENT_TIMEOUT);
    if (RET_OK != iRet)
    {
        BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_SEND_TEARDOWN_FAIL,"Stop Play",
                    "call RTSP_CLIENT_SendTeardownMsg failed, ret = %d", iRet);
    }

    iRet += IVS_NETSOURCE_CloseNetStream(pRtspInfo->NetSourceHandle);

    iRet += IVS_PLAY_CloseStream(pRtspInfo->PlayHandle);

    iRet += IVS_PLAY_StopPlay(pRtspInfo->PlayHandle);

    iRet += IVS_NETSOURCE_FreeChannel(pRtspInfo->NetSourceHandle);


    iRet += IVS_PLAY_FreeChannel(pRtspInfo->PlayHandle);
    if (IVS_SUCCEED != iRet) //lint !e774 !e831
    {
        BP_RUN_LOG_ERR(iRet,"Stop Play", "call ivs_player stop play failed, ret = %d", iRet);
    }

    iRet = RTSP_CLIENT_FreeHandle((int)pRtspInfo->RtspHandle);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Stop Play", "call rtspclient free handle failed, ret = %d", iRet);
    }

    return IVS_SUCCEED;
}

//停止语音广播 这个相当于停止全部广播 
int CAudioObj::StopRealBroadcast()
{
	//std::vector<std::string> strVector;
	//判断是否已开启广播，不是则返回
	if(!m_bIsBroadcastOpenedFlag)
	{
		return IVS_SDK_BROADCASE_ERR_NOT_OPENED;
	}

	EnterCriticalSection(&m_csCodeHandleLock);

	int codesSize = (int)m_CodeHandle.size();
	if(codesSize <= 0)
	{
		LeaveCriticalSection(&m_csCodeHandleLock);
		m_bIsBroadcastOpenedFlag = false;
		return IVS_SUCCEED;//广播组中没有设备时返回成功!
	}

    //全部停止失败需要返回错误
    bool bIsAnySucceed = false;
    int iRet = IVS_FAIL;
    std::string strCameraCode;

	CODE_HANDLE_ITER iter = m_CodeHandle.begin();
	for (; iter != m_CodeHandle.end(); ++iter)
	{
        strCameraCode = (iter->first);

        iRet = StopSingleVoiceBroadcast(dynamic_cast<BROADCAST_RTSP_INFO*>(iter->second));
        if(IVS_SUCCEED != iRet)//一路停止失败不处理，继续停止其他
        {
            //写日志
            BP_RUN_LOG_ERR(IVS_SDK_VOICE_ERR_ALL_BROADCAST_FAIL, "AudioObj StopRealBroadcast",
                "CAudioObj::StopRealBroadcast StopSingleVoiceBroadcast failed. pCameraCode:%s",
                strCameraCode.c_str());

            //上报告警
            EventCallBack fnCallBack = m_pUserMgr->GetEventCallBack();
            if (NULL == fnCallBack)
            {
				LeaveCriticalSection(&m_csCodeHandleLock);
                BP_RUN_LOG_ERR(IVS_SDK_VOICE_ERR_CANNOT_REPORT_FAIL,
                    "AudioObj StopRealBroadcast",
                    "CAudioObj::StopRealBroadcast NULL == fnCallBack");
                return IVS_SDK_VOICE_ERR_CANNOT_REPORT_FAIL;
            }

            //STREAM_EXCEPTION_INFO streamExceptionInfo;
            //memset(&streamExceptionInfo, 0, sizeof(STREAM_EXCEPTION_INFO));
            //streamExceptionInfo.iSessionID = m_pUserMgr->GetSessionID();
            //streamExceptionInfo.ulHandle = m_ulHandle;
            //streamExceptionInfo.iExceptionCode = IVS_EVENT_BROADCAST_FAILED;

            //fnCallBack(IVS_EVENT_BROADCAST_FAILED, &streamExceptionInfo,
            //    sizeof(STREAM_EXCEPTION_INFO), m_pUserMgr->GetEventUserData());

            // edit by wanglei 00165153:2013.8.24 广播异常不上报
            //STREAM_EXCEPTION_INFO exceptionInfo;
            //memset(&exceptionInfo, 0, sizeof(exceptionInfo));
            //exceptionInfo.iSessionID = m_pUserMgr->GetSessionID();
            //exceptionInfo.ulHandle = m_ulHandle;

            //strncpy(exceptionInfo.szDeviceCode, strCameraCode.c_str(), IVS_DEV_CODE_LEN - 1);
            //exceptionInfo.iExceptionCode = IVS_EVENT_BROADCAST_FAILED;

            //fnCallBack(IVS_EVENT_BROADCAST_FAILED, &exceptionInfo,
            //    sizeof(exceptionInfo), m_pUserMgr->GetEventUserData());

        }
        else
        {
            bIsAnySucceed = true;//只要有一路停止成功就返回成功
        }
	}

	LeaveCriticalSection(&m_csCodeHandleLock);

	m_iBroadcastType = -1;
	if(bIsAnySucceed)
	{
		m_bIsBroadcastOpenedFlag = false;
		return IVS_SUCCEED;
	}
	else
	{
		return IVS_SDK_VOICE_ERR_ALL_BROADCAST_FAIL;
	}
}

int CAudioObj::CheckFileExt(const char*pFileName)
{
	//只支持wav格式，后缀长度固定为3
	const int fileExtLen = 3;

	int fileNameLen = (int)strlen(pFileName);

	//备份长度
	int tempFileNameLen = fileNameLen;

	while(tempFileNameLen>0 && pFileName[tempFileNameLen-1]!='.')
	{
		tempFileNameLen--;
	}
	if(tempFileNameLen<=0)//文件名异常
	{
		return IVS_PARA_INVALID;
	}

	if(fileNameLen-tempFileNameLen != fileExtLen)
	{
		return IVS_SDK_VOICE_ERR_FILE_NOT_RIGHT_TYPE;
	}

	char fileNameExt[fileExtLen+1] = {0};
	memcpy(fileNameExt,&pFileName[tempFileNameLen],fileExtLen);
	
    if(0 != _stricmp(fileNameExt, "wav")
       && 0 != _stricmp(fileNameExt, "mp3"))
    {
        return IVS_SDK_VOICE_ERR_FILE_NOT_RIGHT_TYPE;
    }

	return IVS_SUCCEED;
}//lint !e1762

//开始文件语音广播
int CAudioObj::StartFileBroadcast(const IVS_CHAR* pFileName,IVS_INT32 iCycle)
{
	//判断是否已开启广播，是则返回
	if(m_bIsBroadcastOpenedFlag)
	{
		return IVS_SDK_BROADCASE_ERR_ALREADY_OPENED;
	}

	if(NULL == pFileName)
	{
		return IVS_PARA_INVALID;
	}

	int fileNameLen = (int)strlen(pFileName);
	if(fileNameLen<=0 || fileNameLen>FILE_NAME_LEN)
	{
		return IVS_PARA_INVALID;
	}

	//广播文件存在性判断
	int iRet = _access(pFileName,0);
	if (iRet)
	{
		return IVS_SDK_VOICE_ERR_FILE_NOT_EXIST;
	}

	//广播文件后缀判断，目前只支持.wav格式
	iRet = CheckFileExt(pFileName);
	if (iRet)
	{
		return iRet;
	}
	
	//备份媒体参数
//	memcpy(&m_mediaPara,pMediaPara,sizeof(IVS_MEDIA_PARA));
	//备份文件名
	memcpy(m_fileName,pFileName,(unsigned long)fileNameLen);
	//备份文件广播方式
	m_iFileBroadcastCycle = iCycle;

	iRet = IVS_FAIL;
	std::string pCameraCode;

	//全部开启失败需要返回错误
	bool bIsAnySucceed = false;

	//循环遍历设备列表，开启广播
	EnterCriticalSection(&m_csCodeHandleLock);
	
	int codesSize = (int)m_CodeHandle.size();
	if(codesSize <= 0)
	{
		LeaveCriticalSection(&m_csCodeHandleLock);
		return IVS_SDK_BROADCASE_ERR_NO_DEVICE;
	}

	m_iBroadcastType = VOICE_FILE_BROADCAST;
    m_ulHandle = (unsigned long)this;
	CODE_HANDLE_ITER iter = m_CodeHandle.begin();
	for( ;iter != m_CodeHandle.end();iter++)
	{
		pCameraCode = iter->first;
		iRet = StartSingleBroadcast(m_iBroadcastType, pCameraCode.c_str(),
                                    pFileName, iCycle);
		if(IVS_SUCCEED != iRet)
		{
			//写日志
			BP_RUN_LOG_ERR(iRet, "AudioObj StartFileBroadcast",
                "CAudioObj::StartFileBroadcast StartSingleBroadcast failed iRet:%d pCameraCode:%s",
                iRet,pCameraCode.c_str());
			//上报告警
			EventCallBack fnCallBack = m_pUserMgr->GetEventCallBack();
			if (NULL == fnCallBack)
			{
				BP_RUN_LOG_ERR(iRet, "Start File Broadcast","CAudioObj::StartFileBroadcast NULL == fnCallBack");
				return IVS_SDK_VOICE_ERR_CANNOT_REPORT_FAIL;
			}
			
			//BROADCAST_FAILED_INFO broadcastFailedInfo;
			//memset(&broadcastFailedInfo,0,sizeof(BROADCAST_FAILED_INFO));

			//broadcastFailedInfo.iSessionID = m_pUserMgr->GetSessionID();
			//int voiceCodeLen = (int)strlen(pCameraCode.c_str());
			//voiceCodeLen = voiceCodeLen > IVS_DEV_CODE_LEN ? IVS_DEV_CODE_LEN : voiceCodeLen;
			//memcpy(broadcastFailedInfo.cVoiceCode,pCameraCode.c_str(),(unsigned long)voiceCodeLen);

			//fnCallBack(IVS_EVENT_BROADCAST_FAILED,&broadcastFailedInfo,sizeof(BROADCAST_FAILED_INFO),m_pUserMgr->GetEventUserData());

            // edit by wanglei 00165153:2013.8.24 广播异常，统一用STREAM_EXCEPTION_INFO结构体

            STREAM_EXCEPTION_INFO exceptionInfo;
            memset(&exceptionInfo, 0, sizeof(exceptionInfo));
            exceptionInfo.iSessionID = m_pUserMgr->GetSessionID();
            exceptionInfo.ulHandle = m_ulHandle;

            strncpy(exceptionInfo.szDeviceCode, pCameraCode.c_str(), IVS_DEV_CODE_LEN - 1);
            exceptionInfo.iExceptionCode = IVS_EVENT_BROADCAST_FAILED;

            fnCallBack(IVS_EVENT_BROADCAST_FAILED, &exceptionInfo,
                sizeof(exceptionInfo), m_pUserMgr->GetEventUserData());


		}
		else
		{
			bIsAnySucceed = true;//只要有一路开启成功就返回成功
		}
	}
	LeaveCriticalSection(&m_csCodeHandleLock);

	
	
    if(bIsAnySucceed)
	{
		m_bIsBroadcastOpenedFlag = true;
		return IVS_SUCCEED;
	}
	else
	{
		return IVS_SDK_VOICE_ERR_ALL_BROADCAST_FAIL;
	}
}

//停止文件语音广播
int CAudioObj::StopFileBroadcast()
{
	std::vector<std::string> strVector;

	//判断是否已开启，不是则返回
	if(!m_bIsBroadcastOpenedFlag)
	{
		return IVS_SDK_BROADCASE_ERR_NOT_OPENED;
	}

	EnterCriticalSection(&m_csCodeHandleLock);

	int codesSize = (int)m_CodeHandle.size();
	if(codesSize <= 0)
	{
		LeaveCriticalSection(&m_csCodeHandleLock);
		m_bIsBroadcastOpenedFlag = false;
		return IVS_SUCCEED;//广播组中没有设备时返回成功!
	}

    bool bIsAnySucceed = false;
    std::string strCameraCode;

	CODE_HANDLE_ITER iter = m_CodeHandle.begin();
	for (; iter != m_CodeHandle.end(); ++iter)
	{
		strCameraCode = (iter->first);
        int iRet = StopSingleVoiceBroadcast(dynamic_cast<BROADCAST_RTSP_INFO*>(iter->second));
        if(IVS_SUCCEED != iRet)
        {
            //写日志
            BP_RUN_LOG_ERR(iRet, "","CAudioObj::StopFileBroadcast StopSingleVoiceBroadcast failed. VoiceCode:%s",
                           strCameraCode.c_str());
            //上报告警
            EventCallBack fnCallBack = m_pUserMgr->GetEventCallBack();
            if (NULL == fnCallBack)
            {
                BP_RUN_LOG_ERR(iRet, "","CAudioObj::StopFileBroadcast NULL == fnCallBack");
				LeaveCriticalSection(&m_csCodeHandleLock);
                return IVS_SDK_VOICE_ERR_CANNOT_REPORT_FAIL;
            }
            
            // edit by wanglei 00165153:2013.8.24 广播关闭失败后不上报
            //BROADCAST_FAILED_INFO broadcastFailedInfo;
            //memset(&broadcastFailedInfo,0,sizeof(BROADCAST_FAILED_INFO));

            //broadcastFailedInfo.iSessionID = m_pUserMgr->GetSessionID();
            //int voiceCodeLen = (int)strlen(strCameraCode.c_str());
            //voiceCodeLen = voiceCodeLen > IVS_DEV_CODE_LEN ? IVS_DEV_CODE_LEN : voiceCodeLen;
            //memcpy(broadcastFailedInfo.cVoiceCode,strCameraCode.c_str(),(unsigned long)voiceCodeLen);

            //fnCallBack(IVS_EVENT_BROADCAST_FAILED,&broadcastFailedInfo,sizeof(BROADCAST_FAILED_INFO),m_pUserMgr->GetEventUserData());

            //STREAM_EXCEPTION_INFO exceptionInfo;
            //memset(&exceptionInfo, 0, sizeof(exceptionInfo));
            //exceptionInfo.iSessionID = m_pUserMgr->GetSessionID();
            //exceptionInfo.ulHandle = m_ulHandle;

            //strncpy(exceptionInfo.szDeviceCode, strCameraCode.c_str(), IVS_DEV_CODE_LEN - 1);
            //exceptionInfo.iExceptionCode = IVS_EVENT_BROADCAST_FAILED;

            //fnCallBack(IVS_EVENT_BROADCAST_FAILED, &exceptionInfo,
            //    sizeof(exceptionInfo), m_pUserMgr->GetEventUserData());
        }
        else
        {
            bIsAnySucceed = true;//只要有一路停止成功就返回成功
        }
	}
	LeaveCriticalSection(&m_csCodeHandleLock);
	
	m_iBroadcastType = -1;
	if(bIsAnySucceed)
	{
		m_bIsBroadcastOpenedFlag = false;
		return IVS_SUCCEED;
	}
	else
	{
		return IVS_SDK_VOICE_ERR_ALL_BROADCAST_FAIL;
	}
}

int CAudioObj::StartPlay(const IVS_MEDIA_PARA* pstMediaPara, 
                         const char* pCameraCode, 
                         const START_PLAY_PARAM& stParam)
{
    //校验参数
    if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || 0 == strlen(pCameraCode))
    {
        BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST,"Start Talk Back", "Voice code invalid");
        return IVS_PARA_INVALID;
    }

    //设置参数
    bool bRet = CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID),
                                    pCameraCode, strlen(pCameraCode));

    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Talk Back", "Strncpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    int iRet = SetMediaParam(pstMediaPara);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Talk Back", "set media param error");
        return iRet;
    }

    //从SCU获取媒体URL，解析数据保存在基类.
    iRet = DoGetUrl(stParam.enServiceType);
    if (IVS_SUCCEED != iRet) 
    {
        BP_RUN_LOG_ERR(iRet,"Start Talk Back", "get url play back error, stream default.");
        return iRet;
    }

    // 调用基类启动播放;
    iRet = CMediaBase::StartPlay(stParam);
    if (iRet != IVS_SUCCEED)
    {
        //m_fSpeed = 0.0;
        BP_RUN_LOG_ERR(iRet,"Start Play Back", "call base class failed.");
        return iRet;
    }

    return iRet;
}

int CAudioObj::DoGetUrl(IVS_SERVICE_TYPE enServiceType)
{
    //请求消息;
    memset(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));

    //TODO : szDstPort无实际作用，传一个固定值；打包协议类型szDstPP入参结构体未定义
    m_stUrlMediaInfo.ServiceType = enServiceType;

    //std::string strCameraDomainCode = m_szCameraID;
    std::string strCameraCode;
    std::string strDomainCode;
    int iRet = CToolsHelp::SplitCameraDomainCode(m_szCameraID, strCameraCode, strDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Talkback", "CToolsHelp::SplitCameraCode failed, RetCode=%d.", iRet);
        return iRet;
    }

    bool bRet = CToolsHelp::Strncpy(m_stUrlMediaInfo.szSrcCode, sizeof(m_stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.size());//lint !e534
    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Do Get Url Talkback", "Strncpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    bRet = CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534
    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Do Get Url Talkback", "Strncpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    bRet = CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstCode,sizeof(m_stUrlMediaInfo.szDstCode), "01",strlen("01"));//lint !e534
    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Do Get Url Talkback", "Strncpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    m_stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
    m_stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
    m_stUrlMediaInfo.PackProtocolType = PACK_PROTOCOL_ES;
    m_stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
    bRet = CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstIP,sizeof(m_stUrlMediaInfo.szDstIP), "",strlen(""));//lint !e534
    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Do Get Url Talkback", "Strncpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    bRet = CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstPort,sizeof(m_stUrlMediaInfo.szDstPort), "35016",strlen("35016"));//lint !e534
    if(false == bRet)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Do Get Url Talkback", "Strncpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    m_stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
    m_stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
    m_stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;

    //获取媒体URL
    iRet = GetURL(CToolsHelp::GetSCCHead().c_str(),m_pUserMgr, m_stUrlMediaInfo, m_stMediaRsp, m_bAssociatedAudio);//lint !e838
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_SDK_VOICE_ERR_GET_URL_FAIL, "Do Get Url Talkback",
                     "Start Voice Talkback SCC get URL failed with %d!", iRet);
        return iRet;
    }

    return iRet;
}

 // add by wanglei 00165153:2013.5.3 用于获取指定摄像机支持的音频编码格式
int CAudioObj::GetSupportAudioType(const char* pszCameraCode, unsigned int& unAudioType)
{
    // 通过摄像机编码获取VoiceOutCode
    IVS_CAMERA_CFG stCameraCfg;
    memset(&stCameraCfg, 0, sizeof(stCameraCfg));

    int iRet = m_pUserMgr->GetDeviceParaConfig().GetDeviceConfig(pszCameraCode,
        CONFIG_CAMERA_CFG,
        &stCameraCfg,
        sizeof(stCameraCfg));
    if (IVS_SUCCEED  != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Support Audio Type",
            "Get Device Config CONFIG_CAMERA_CFG failed with %d!", iRet);
        return iRet;
    }

    // 通过cVoiceOutCode查询支持的音频格式
    std::string strAudioOutID = stCameraCfg.cVoiceOutCode;

    IVS_AUDIO_CFG stAudioCfg;
    memset(&stAudioCfg, 0, sizeof(stAudioCfg));

    iRet = m_pUserMgr->GetDeviceParaConfig().GetAudioDeviceConfig(
        pszCameraCode,
        strAudioOutID.c_str(),
        CONFIG_CAMERA_AUDIO_CFG,
        &stAudioCfg,
        sizeof(stAudioCfg));
    if (IVS_SUCCEED  != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Support Audio Type",
            "Get Audio Device Config CONFIG_CAMERA_AUDIO_CFG failed with %d!", iRet);
        return iRet;
    }

    if (1 != stAudioCfg.uiChannelType)
    {
        return IVS_FAIL;
    }

    unAudioType = stAudioCfg.stAudioParam.uiDataFormat;
    return IVS_SUCCEED;
}

// 经过和SE确认，由于无法直接通过摄像机编码获取音频输出支持的格式，要分两步走
// 1. SDK接口IVS_SDK_GetDeviceConfig(查询摄像机参数类型,摄像机编码)就能拿到 VoiceOutCode
// 2. 根据VoiceOutCode SDK接口IVS_SDK_GetDeviceConfig(查询语音子设备参数类型,VoiceOutCode)就能拿到语音子设备的编码能力，例如G711A，G711U
int CAudioObj::GetSupportAudioType(unsigned int &uAudioType)
{
    // 通过摄像机编码获取VoiceOutCode
    IVS_CAMERA_CFG stCameraCfg;
    memset(&stCameraCfg, 0, sizeof(stCameraCfg));

    int iRet = m_pUserMgr->GetDeviceParaConfig().GetDeviceConfig(m_szCameraID,
                                                        CONFIG_CAMERA_CFG,
                                                        &stCameraCfg,
                                                        sizeof(stCameraCfg));
    if (IVS_SUCCEED  != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Support Audio Type",
            "Get Device Config CONFIG_CAMERA_CFG failed with %d!", iRet);
        return iRet;
    }

    // 通过cVoiceOutCode查询支持的音频格式
	std::string strAudioOutID = stCameraCfg.cVoiceOutCode;

    IVS_AUDIO_CFG stAudioCfg;
    memset(&stAudioCfg, 0, sizeof(stAudioCfg));

    iRet = m_pUserMgr->GetDeviceParaConfig().GetAudioDeviceConfig(
		                                              m_szCameraID,
                                                      strAudioOutID.c_str(),
                                                      CONFIG_CAMERA_AUDIO_CFG,
                                                      &stAudioCfg,
                                                      sizeof(stAudioCfg));
    if (IVS_SUCCEED  != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Support Audio Type",
                    "Get Audio Device Config CONFIG_CAMERA_AUDIO_CFG failed with %d!", iRet);
        return iRet;
    }

    if (1 != stAudioCfg.uiChannelType)
    {
        return IVS_FAIL;
    }

    uAudioType = stAudioCfg.stAudioParam.uiDataFormat;
    return IVS_SUCCEED;
}


int CAudioObj::CheckAudioType()
{
    unsigned int uAudioType = 0;
    int iRet = GetSupportAudioType(uAudioType);

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Voice Broadcast",
            "Get Support AudioType error.");
        return iRet;
    }

    // modify by wanglei 00165153:2013.4.26 如果当前没设备, 则允许刷新编码类型
    if (m_bIsBroadcastOpenedFlag && CAudioObj::m_stcAudioType != 0)
    {
        
        if (CAudioObj::m_stcAudioType != uAudioType)
        {
            BP_RUN_LOG_ERR(IVS_SDK_VOICE_ERR_AUDIO_TYPE_CONFLICT,
                "Start Voice Broadcast",
                "AudioType conflict %u : %u error.",
                CAudioObj::m_stcAudioType, uAudioType);
            return IVS_SDK_VOICE_ERR_AUDIO_TYPE_CONFLICT;
        }
    }
    else
    {
        IVS_ULONG ulVoiceType = 0;

        if (AUDIO_DEC_G711U == uAudioType)
        {
            ulVoiceType = PAY_LOAD_TYPE_PCMU;
        }
        else if (AUDIO_DEC_G711A == uAudioType)
        {
            ulVoiceType = PAY_LOAD_TYPE_PCMA;
        }

        iRet = IVS_PLAY_SetVoiceDecoderType(ulVoiceType);

        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet,"Start Voice Broadcast", "Set VoiceDecoder Type.");
            return iRet;
        }

        CAudioObj::m_stcAudioType = uAudioType;
    }

    return iRet;
}

bool CAudioObj::IsExistTalkBackRtspHandle(IVS_ULONG ulRtspHandle) const
{
	return ulRtspHandle == (IVS_ULONG)m_iRtspHandle ? true : false;
}

bool CAudioObj::IsExistBroadCastRtspHandle(IVS_ULONG ulRtspHandle)
{
     BP_RUN_LOG_ERR(11,"IsExistBroadCastRtspHandle",  "Param[%lu]", ulRtspHandle);
    // 打印map值
    CODE_HANDLE_ITER iterBegin = m_CodeHandle.begin();
    CODE_HANDLE_ITER iterEnd = m_CodeHandle.end();
    int iIndex = 0;
    for (; iterBegin != iterEnd; iterBegin++)
    {
        BROADCAST_RTSP_INFO *pRtspInfoTmp = iterBegin->second;
        BP_RUN_LOG_ERR(11,"IsExistBroadCastRtspHandle", "Map[%d]. RtspHandle[%lu] NetSourceHandle[%lu]", iIndex, pRtspInfoTmp->RtspHandle, pRtspInfoTmp->NetSourceHandle);
        iIndex++;
    }


    CODE_HANDLE_ITER iter = m_CodeHandle.begin();
    for (; iter != m_CodeHandle.end(); ++iter)
    {
        BROADCAST_RTSP_INFO* pstBroadCastRtspInfo = iter->second;
        if (NULL == pstBroadCastRtspInfo)
        {
            continue;
        }

        if (ulRtspHandle == (IVS_ULONG)pstBroadCastRtspInfo->RtspHandle)
        {
            BP_RUN_LOG_ERR(11,"IsExistBroadCastRtspHandle", "RtspHandle Equal", ulRtspHandle);
            return true;
        }
    }
    BP_RUN_LOG_ERR(11,"IsExistBroadCastRtspHandle", "RtspHandle Different", ulRtspHandle);
    return false;
}

bool CAudioObj::IsExistTalkBackNetHandle(IVS_ULONG ulNetHandle) const
{
    return ulNetHandle == m_ulNetSourceChannel ? true : false;
}

bool CAudioObj::IsExistBroadCastNetHandle(IVS_ULONG ulNetHandle)
{
    CODE_HANDLE_ITER iter = m_CodeHandle.begin();
    for (; iter != m_CodeHandle.end(); ++iter)
    {
        BROADCAST_RTSP_INFO* pstBroadCastRtspInfo = iter->second;
        if (NULL == pstBroadCastRtspInfo)
        {
            continue;
        }

        if (ulNetHandle == pstBroadCastRtspInfo->NetSourceHandle)
        {
            return true;
        }
    }

    return false;
}

//int CAudioObj::GetBroadCastVoiceCodeByRtspHandle(IVS_ULONG ulRtspHandle, std::string& strVoiceCode)
//{
//	CODE_HANDLE_ITER iter = m_CodeHandle.begin();
//	for (; iter != m_CodeHandle.end(); ++iter)
//	{
//		BROADCAST_RTSP_INFO* pstBroadCastRtspInfo = iter->second;
//		if (NULL == pstBroadCastRtspInfo)
//		{
//			continue;
//		}
//
//		if (ulRtspHandle == pstBroadCastRtspInfo->RtspHandle)
//		{
//			strVoiceCode = iter->first;
//			return IVS_SUCCEED;
//		}
//	}
//
//	return IVS_FAIL;
//}

int CAudioObj::GetBroadCastDeviceCodeByConnHandle(IVS_ULONG ulNetHandle, std::string& strDeviceCode)
{
    CODE_HANDLE_ITER iter = m_CodeHandle.begin();
    for (; iter != m_CodeHandle.end(); ++iter)
    {
        BROADCAST_RTSP_INFO* pstBroadCastRtspInfo = iter->second;
        if (NULL == pstBroadCastRtspInfo)
        {
            continue;
        }

        if (ulNetHandle == pstBroadCastRtspInfo->RtspHandle || ulNetHandle == pstBroadCastRtspInfo->NetSourceHandle)
        {
            strDeviceCode = iter->first;
            return IVS_SUCCEED;
        }
    }

    return IVS_FAIL;
}


