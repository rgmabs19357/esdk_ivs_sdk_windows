#include "Playback.h"
#include "IVS_Player.h"
#include "LockGuard.h"
#include "IVS_Trace.h"
#include "ToolsHelp.h"
#include "UserMgr.h"

//#define STEP_BACKWARD_INTERVAL           (700)       // 单步回退请求的间隔时间，单位为毫秒

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
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Play back g_fRtsp Data Call Back", "Input param invalid.");
        return;
    }

    if (*pBuffer != '$')
    {
        BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_DATA_ERROR, "Play back g_fRtsp Data Call Back", "rtsp media data format error, do not have $.");
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
        BP_RUN_LOG_ERR(IVS_FAIL,"Play back g_fRtsp Data Call Back", "transform pUser to MediaBase pointer failed.");
        return;
    }

    unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
    if (0 == uiPlayerChannel)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Play back g_fRtsp Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
        return;
    }

    pBuffer   += 4;
    ulBufSize -= 4;
    (void) IVS_PLAY_InputRtpData(uiPlayerChannel, pBuffer, ulBufSize); //通过TCP协议控制
    return;
} //lint !e818 !e528

static void __SDK_CALL g_fNetDataCallBack( char *pBuf, unsigned int uiSize, void *pUser)//lint !e818 !e830
{
	CHECK_POINTER_VOID(pBuf);
    //获取回调出来的类对象;
    CMediaBase* pMediaBase = reinterpret_cast<CMediaBase*>(pUser);
    if (NULL == pMediaBase)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Play back g_fNet Data Call Back", "transform pUser to MediaBase pointer failed.");
        return;
    }
    
    unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
    if (0 == uiPlayerChannel)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Play back g_fNet Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
        return;
    }

    (void)IVS_PLAY_InputRtpData(uiPlayerChannel, pBuf, uiSize);


    return;
}//lint !e818 !e528

// 裸码流回调;
static IVS_VOID CALLBACK g_fReassmCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo,
                                           IVS_CHAR*           pFrame,
                                           IVS_UINT32          uFrameSize,
                                           REC_PACKET_INFO*    /*pstRecPacketInfo*/,
                                           IVS_VOID*       pUser)
{
    CPlayback* pPlayBack = static_cast<CPlayback*>(pUser);
    if (NULL == pPlayBack)
    {
        return;
    }

    pPlayBack->DoRawFrameCallBack(pstRawFrameInfo, pFrame, uFrameSize);
}//lint !e818 !e528

// 解码回调，返回解码后数据;
static IVS_VOID CALLBACK g_fDecoderCallBack(IVS_INT32   iStreamType,
                                            IVS_VOID*   pFrameInfo,
                                            IVS_VOID*   pUser)
{
    CPlayback* pPlayBack = static_cast<CPlayback*>(pUser);
    if (NULL == pPlayBack)
    {
        return;
    }

    pPlayBack->DoFrameCallBack(iStreamType, pFrameInfo);
}//lint !e818 !e528

CPlayback::CPlayback(void)
    : m_hWnd(NULL)
    , m_uiDragPlayTime(0)
	, m_FirstRtpStamp(0)
	, m_bPaused(false)
	, m_bLocalFile(false)
    , m_fPlayBackCallBackRaw(NULL)
    , m_pPlayBackUserDataRaw(NULL)
    , m_fPlayBackCallBackFrame(NULL)
    , m_pPlayBackUserDataFrame(NULL)
	, m_lBrightValue(0)
	, m_lContrastValue(0)
	, m_lSaturationValue(0)
	, m_lHueValue(0)
{
    memset(&m_stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
	m_pPlayBackRawCBMutex = VOS_CreateMutex();
	m_pPlayBackFrameDecoderCBMutex = VOS_CreateMutex();
}


CPlayback::~CPlayback(void)
{
    m_hWnd = NULL;
    m_fSpeed = 0.0;
    m_uiDragPlayTime = 0;
	m_pPlayBackUserDataRaw = NULL;
	m_pPlayBackUserDataFrame = NULL;
	try
	{
		if (NULL != m_pPlayBackRawCBMutex)
		{
			VOS_DestroyMutex(m_pPlayBackRawCBMutex);
			m_pPlayBackRawCBMutex = NULL;
		}
		
		if (NULL != m_pPlayBackFrameDecoderCBMutex)
		{
			VOS_DestroyMutex(m_pPlayBackFrameDecoderCBMutex);
			m_pPlayBackFrameDecoderCBMutex = NULL;
		}
		
		RetSet(); //lint !e1506已经实现了虚函数;
	}
	catch(...)
	{

	}
}


void CPlayback::SetHandle(unsigned long ulHandle)
{
	m_ulHandle = ulHandle;
}//lint !e1762

unsigned long CPlayback::GetHandle()
{
	return m_ulHandle;
}//lint !e1762

//开始平台录像回放
int CPlayback::StartPlatformOrBackupPlayBack(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode,
                                     unsigned int uiProtocolType,
                                     const TIME_SPAN_INTERNAL& stTimeSpanInter,
                                     HWND hWnd,
                                     float fSpeed,IVS_SERVICE_TYPE playBackServiceType )
{
#if 0  
	if(m_iStartType == PLAY_TYPE_HWND && NULL == hWnd)
    {
        BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED,"Start Platform Or Backup Play Back", "hwnd invalid");
        return IVS_PARA_INVALID;
    }
#endif 

    CLockGuard lock(m_pMediaMutex);

    IVS_MEDIA_PARA stMediaPara;
	InitMediaPara(stMediaPara);
    stMediaPara.ProtocolType = static_cast<IVS_PROTOCOL_TYPE>(uiProtocolType);//此处转换肯定正确，前面已做检查;

	START_PLAY_PARAM stParam;
	stParam.enServiceType = playBackServiceType;
	if(NULL != pDomainCode && SERVICE_TYPE_DISASTEBACKUP_PLAYBACK != stParam.enServiceType)
	{
		stParam.enServiceType = SERVICE_TYPE_BACKUPRECORD_PLAYBACK;
	}
	stParam.fSpeed = fSpeed;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
	stParam.cbNetDataCallBack = g_fNetDataCallBack;
    //开始回放;
    int iRet = StartPlay(pDomainCode,pNVRCode,&stMediaPara, pCameraCode, stTimeSpanInter, stParam, NULL);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Platform Or Backup Play Back", "Start play error.");
        return iRet;
    }

    SetServiceType(stParam.enServiceType);

	(void)IVS_PLAY_SetPlaySpeed(m_ulPlayerChannel,   m_fSpeed);//lint !e747
    (void)IVS_PLAY_SetPlayWindow(m_ulPlayerChannel, hWnd);
	m_hWnd = hWnd;
    return IVS_SUCCEED;
}

int CPlayback::StartPlatformPlayBackCB(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode,
							   		   unsigned int uiProtocolType,
									   const TIME_SPAN_INTERNAL& stTimeSpanInter,
									   PlayBackCallBackRaw fPlayBackCallBackRaw,
									   void* pUserData, 
									   float fSpeed)
{
	CLockGuard lock(m_pMediaMutex);

	IVS_MEDIA_PARA stMediaPara;
	InitMediaPara(stMediaPara);
	stMediaPara.ProtocolType = static_cast<IVS_PROTOCOL_TYPE>(uiProtocolType);//此处转换肯定正确，前面已做检查;

	START_PLAY_PARAM stParam;
	stParam.enServiceType = SERVICE_TYPE_PLAYBACK;
	if(NULL != pDomainCode && SERVICE_TYPE_DISASTEBACKUP_PLAYBACK != stParam.enServiceType)
	{
		stParam.enServiceType = SERVICE_TYPE_BACKUPRECORD_PLAYBACK;
	}
	stParam.fSpeed = fSpeed;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
	stParam.cbNetDataCallBack = g_fNetDataCallBack;
	//开始回放;
	int iRet = StartPlay(pDomainCode,pNVRCode,&stMediaPara, pCameraCode, stTimeSpanInter, stParam, NULL);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start Platform Or Backup Play Back", "Start play error.");
		return iRet;
	}

	SetServiceType(stParam.enServiceType);

	(void)IVS_PLAY_SetPlaySpeed(m_ulPlayerChannel,   m_fSpeed);//lint !e747

	m_iStartType = PLAY_TYPE_CB;
	(void)VOS_MutexLock(m_pPlayBackRawCBMutex);
	m_fPlayBackCallBackRaw = fPlayBackCallBackRaw;
	m_pPlayBackUserDataRaw = pUserData;
	(void)VOS_MutexUnlock(m_pPlayBackRawCBMutex);
	iRet = IVS_PLAY_SetRawFrameCallBack(m_ulPlayerChannel, g_fReassmCallBack, this);

	return IVS_SUCCEED;
}

int CPlayback::StartPlatformPlayBackCBFrame(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode,
											unsigned int uiProtocolType,
											const TIME_SPAN_INTERNAL& stTimeSpanInter,
											PlayBackCallBackFrame fPlayBackCallBackFrame,
											void* pUserData, 
											float fSpeed)
{
	CLockGuard lock(m_pMediaMutex);

	IVS_MEDIA_PARA stMediaPara;
	InitMediaPara(stMediaPara);
	stMediaPara.ProtocolType = static_cast<IVS_PROTOCOL_TYPE>(uiProtocolType);//此处转换肯定正确，前面已做检查;

	START_PLAY_PARAM stParam;
	stParam.enServiceType = SERVICE_TYPE_PLAYBACK;
	if(NULL != pDomainCode && SERVICE_TYPE_DISASTEBACKUP_PLAYBACK != stParam.enServiceType)
	{
		stParam.enServiceType = SERVICE_TYPE_BACKUPRECORD_PLAYBACK;
	}
	stParam.fSpeed = fSpeed;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
	stParam.cbNetDataCallBack = g_fNetDataCallBack;
	//开始回放;
	int iRet = StartPlay(pDomainCode,pNVRCode,&stMediaPara, pCameraCode, stTimeSpanInter, stParam, NULL);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start Platform Or Backup Play Back", "Start play error.");
		return iRet;
	}

	SetServiceType(stParam.enServiceType);

	(void)IVS_PLAY_SetPlaySpeed(m_ulPlayerChannel,   m_fSpeed);//lint !e747

	m_iStartType = PLAY_TYPE_CB;
	(void)VOS_MutexLock(m_pPlayBackFrameDecoderCBMutex);
	m_fPlayBackCallBackFrame = fPlayBackCallBackFrame;
	m_pPlayBackUserDataFrame = pUserData;
	(void)VOS_MutexUnlock(m_pPlayBackFrameDecoderCBMutex);
	iRet = IVS_PLAY_SetFrameCallBack(m_ulPlayerChannel, g_fDecoderCallBack, this);
	return IVS_SUCCEED;
}

//停止平台录像回放
int CPlayback::StopPlatformPlayBack()
{
	IVS_DEBUG_TRACE("");
    CLockGuard lock(m_pMediaMutex);

    int iRet = CMediaBase::StopPlay();

    //重置参数
    RetSet();
    if (iRet != IVS_SUCCEED)
    {
        BP_RUN_LOG_ERR(iRet,"Stop Platform Play Back", "call MediaBase::StopPlay failed.");
        iRet = IVS_FAIL;
    }
    return iRet;
}
//暂停录像回放
int CPlayback::PlayBackPause()
{
    CLockGuard lock(m_pMediaMutex);
    int iRet = IVS_FAIL;

	if (!m_bLocalFile)
	{
		if (RTSP_SESSION_STATUS_PLAYING == RTSP_CLIENT_GetStatus(m_iRtspHandle))
		{
			iRet = RTSP_CLIENT_SendPauseMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
			if (0 != iRet)
			{
				BP_RUN_LOG_ERR(iRet,"Play Back Pause", "RTSP client send pause play fail.");
				return IVS_PLAYER_RET_RTSP_SEND_PAUSE_FAIL;
			}
		}
	}
	
	m_bPaused = true;
	return IVS_PLAY_PausePlay(m_ulPlayerChannel);
}
//恢复录像回放
int CPlayback::PlayBackResume()
{
    CLockGuard lock(m_pMediaMutex);

	m_bStepPlay = false;
    int iRet = IVS_FAIL;
    
	if (!m_bLocalFile)
	{
		if (RTSP_SESSION_STATUS_READY == RTSP_CLIENT_GetStatus(m_iRtspHandle))
		{
			float fSendScale = m_fSpeed;
			(void)CToolsHelp::GetSendScale(m_enServiceType, m_fSpeed, fSendScale);

			MEDIA_RANGE_S stRange;
			memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
			stRange.enRangeType = RANGE_TYPE_UTC;
			stRange.MediaBeginOffset = OFFSET_CUR;
			iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)fSendScale, &stRange, RTSP_CLIENT_TIMEOUT);
			if (0 != iRet)
			{
				BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
				return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
			}
		}
	}
	
    m_bPaused = false;
	return IVS_PLAY_ResumePlay(m_ulPlayerChannel);
}

/*
 * StepForward
 * 单步前进
 */
int CPlayback::StepForward()
{
	CLockGuard lock(m_pMediaMutex);

	if (!m_bLocalFile)
	{
		if (m_fSpeed < 0 || m_fSpeed == 16.000000 || m_fSpeed == 8.000000 || m_bPaused)
		{
			float fSpeed     = 1.0;
			float fSendScale = fSpeed;
			(void)CToolsHelp::GetSendScale(m_enServiceType,fSpeed, fSendScale);
			MEDIA_RANGE_S stRange;
			memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));

			stRange.enRangeType = RANGE_TYPE_NPT;
			stRange.MediaBeginOffset = OFFSET_CUR;

            BP_DBG_LOG("Step Forward, m_fSpeed:[%f], fSpeed:[%f].", m_fSpeed, fSpeed);
            unsigned long long current = 0;

			if ((m_fSpeed < 0 && fSpeed > 0) || 
				(m_fSpeed > 0 && fSpeed < 0))
			{
				//unsigned long long current = 0;
				(void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &current);
				if (current && ((long long)(current/1000) > m_tmStart))
				{
					stRange.MediaBeginOffset = current/1000 - m_tmStart;//lint !e737 !e712
				}
			}
            else if ((m_fSpeed == 16.000000) && (fSpeed == 1.000000)
                    || (m_fSpeed == 8.000000) && (fSpeed == 1.000000))
            {
                //unsigned long long current = 0;
                (void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &current);
                if (current && ((long long)(current/1000) > m_tmStart))
                {
                    stRange.MediaBeginOffset = current/1000 - m_tmStart;//lint !e737 !e712
                    //IVS_PLAY_ResetData(m_ulPlayerChannel, true);
                    BP_DBG_LOG("16 Step Forward, m_fSpeed:[%f], fSpeed:[%f].", m_fSpeed, fSpeed);
                }         
            }

            int iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)fSendScale, &stRange, RTSP_CLIENT_TIMEOUT);
            if (0 != iRet)
            {
                BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
                return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
            }

            if ((m_fSpeed == 16.000000) && (fSpeed == 1.000000)
                || (m_fSpeed == 8.000000) && (fSpeed == 1.000000))
            {
                //IVS_PLAY_SetPlayTime(m_ulPlayerChannel, current/1000);
                IVS_PLAY_ResetData(m_ulPlayerChannel);  
				IVS_PLAY_SetResetFlag(m_ulPlayerChannel, true);
            }

		}
	}

	m_fSpeed = 1.0;
	m_bStepPlay = true;
	m_bPaused   = false;

	unsigned long framesLeft = 0;
	(void)IVS_PLAY_StepPlayForward(m_ulPlayerChannel, &framesLeft);

    return IVS_SUCCEED;
}

/*
 * StepBackward
 * 单步后退
 */
int CPlayback::StepBackward()
{
	CLockGuard lock(m_pMediaMutex);

	if (!m_bLocalFile)
	{
		if (!m_bStepPlay || m_fSpeed > 0)
		{
			float fSpeed     = -1.0;
			float fSendScale = fSpeed;
			(void)CToolsHelp::GetSendScale(m_enServiceType,fSpeed, fSendScale);
			MEDIA_RANGE_S stRange;
			memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));

			stRange.enRangeType = RANGE_TYPE_NPT;
			stRange.MediaBeginOffset = OFFSET_CUR;
			
            unsigned long long current = 0;
			if ((m_fSpeed < 0 && fSpeed > 0) || 
				(m_fSpeed > 0 && fSpeed < 0))
			{
				//unsigned long long current = 0;
				(void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &current);
				if (current && ((long long)(current/1000) > m_tmStart))
				{
					stRange.MediaBeginOffset = current/1000 - m_tmStart;//lint !e737 !e712
				}
			}
            else if ((m_fSpeed == -16.000000) && (fSpeed == -1.000000)
                || (m_fSpeed == -8.000000) && (fSpeed == -1.000000)
                || (m_fSpeed == -4.000000) && (fSpeed == -1.000000))
            {
                //unsigned long long current = 0;
                (void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &current);
                if (current && ((long long)(current/1000) > m_tmStart))
                {
                    stRange.MediaBeginOffset = current/1000 - m_tmStart;//lint !e737 !e712
                    BP_DBG_LOG("16 Step Backward, m_fSpeed:[%f], fSpeed:[%f].", m_fSpeed, fSpeed);
                }         
            }

			int iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)fSendScale, &stRange, RTSP_CLIENT_TIMEOUT);
			if (0 != iRet)
			{
				BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
				return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
			}

            if ((m_fSpeed == -16.000000) && (fSpeed == -1.000000)
                || (m_fSpeed == -8.000000) && (fSpeed == -1.000000)
                || (m_fSpeed == -4.000000) && (fSpeed == -1.000000))
            {
                //IVS_PLAY_SetPlayTime(m_ulPlayerChannel, current/1000);
                IVS_PLAY_ResetData(m_ulPlayerChannel);     
				IVS_PLAY_SetResetFlag(m_ulPlayerChannel, true);
            }

		}
	}

	m_fSpeed = -1.0;
	m_bStepPlay = true;
	m_bPaused   = false;

	unsigned long framesLeft = 0;
	(void)IVS_PLAY_StepPlayBackward(m_ulPlayerChannel, &framesLeft);

    return IVS_SUCCEED;
}


// 设置播放速度
int CPlayback::SetPlayBackSpeed(float fSpeed)
{
	CLockGuard lock(m_pMediaMutex);

	(void)IVS_PLAY_SetPlaySpeed(m_ulPlayerChannel, fSpeed);//lint !e747

	m_bStepPlay = false;
	int iRet = IVS_FAIL;
	RTSP_SESSION_STATUS state = RTSP_CLIENT_GetStatus(m_iRtspHandle);
    
	if (!m_bLocalFile)
	{
		if (RTSP_SESSION_STATUS_PLAYING == state ||
			RTSP_SESSION_STATUS_READY   == state)
		{
			float fSendScale = fSpeed;
			(void)CToolsHelp::GetSendScale(m_enServiceType,fSpeed, fSendScale);
			MEDIA_RANGE_S stRange;
			memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));

			stRange.enRangeType = RANGE_TYPE_NPT/*RANGE_TYPE_UTC*/;
			stRange.MediaBeginOffset = OFFSET_CUR;
			
            BP_DBG_LOG("SetPlayBackSpeed, m_fSpeed:[%f], fSpeed:[%f].", m_fSpeed, fSpeed);

			/* 
			 * DTS2013020300831, 
			 * 回放方向发生变化，对播放时间偏移重新定位,避免中间暂停
			 * fix by w00210470
			 */
            if ((m_fSpeed < 0 && fSpeed > 0) || 
				(m_fSpeed > 0 && fSpeed < 0))
			{
				IVS_PLAY_SetResetFlag(m_ulPlayerChannel, false);

				unsigned long long current = 0;
				(void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &current);
                if (current && ((long long)(current/1000) > m_tmStart))
				{
					stRange.MediaBeginOffset = current/1000 - m_tmStart;//lint !e737 !e712
					//IVS_PLAY_ResetData(m_ulPlayerChannel);
                    BP_DBG_LOG("Set Play Back Speed, direction is change, m_fSpeed:[%f], fSpeed:[%f].", m_fSpeed, fSpeed);
				}
			}
			else if (((m_fSpeed >= 8.000000) && (fSpeed < 8.000000))
				    || ((m_fSpeed < 8.000000) && (fSpeed >= 8.000000))
					|| ((m_fSpeed <= -2.000000) && (fSpeed > -2.000000))
					|| ((m_fSpeed > -2.000000) && (fSpeed <= -2.000000)))
            {
                unsigned long long current = 0;
                (void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &current);
                if (current && ((long long)(current/1000) > m_tmStart))
                {
                    stRange.MediaBeginOffset = current/1000 - m_tmStart;//lint !e737 !e712
                    //IVS_PLAY_ResetData(m_ulPlayerChannel);
                    BP_DBG_LOG("Set Play Back Speed, m_fSpeed:[%f], fSpeed:[%f].", m_fSpeed, fSpeed);
                }  
            }

			iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)fSendScale, &stRange, RTSP_CLIENT_TIMEOUT);
			if (0 != iRet)
			{
				BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
				return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
			}

            //必须调用接口清空流控标志，否则会导致缓冲区满但不会发PAUSE消息，造成丢包花屏
            IVS_PLAY_ReSetFlowContrlFlag(m_ulPlayerChannel);

			//正向播放8/16时只有I帧，本域反向播放-2/-4/-8/-16时只有I帧;外域反向都是I帧
			if (((m_fSpeed < 8.000000) && (fSpeed >= 8.000000))
				|| ((m_fSpeed > -2.000000) && (fSpeed <= -2.000000))
				|| ((m_fSpeed >= 8.000000) && (fSpeed < 8.000000)))
            {
                IVS_PLAY_ResetData(m_ulPlayerChannel);
				IVS_PLAY_SetResetFlag(m_ulPlayerChannel, false);
            }
			else if ((m_fSpeed <= -2.000000) && (fSpeed > -2.000000))
            {
				IVS_PLAY_ResetData(m_ulPlayerChannel);
                IVS_PLAY_SetResetFlag(m_ulPlayerChannel, true);
            }
		}
	}
	m_fSpeed = fSpeed;

	return IVS_SUCCEED;
}

/*
 * GetCurrentFrameTick
 * 获取当前播放帧的时间戳
 */
int CPlayback::GetCurrentFrameTick(unsigned long long &tick)
{
	CLockGuard lock(m_pMediaMutex);
	return IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &tick);
}

//获取录像回放时间
int CPlayback::GetPlayBackTime(IVS_PLAYBACK_TIME& stPlayBackTime)
{
    CLockGuard lock(m_pMediaMutex);

    memset(&stPlayBackTime, 0, sizeof(stPlayBackTime));
    //unsigned int uiPlayTime = 0;

	unsigned long long uiPlayTime = 0;

    int iRet = IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &uiPlayTime);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Get Play Back Time", "Get play time failed");
        return iRet;
    }

	uiPlayTime = uiPlayTime/1000; // 转换为秒;
	if (m_bLocalFile)
	{
		memset(&stPlayBackTime, 0, sizeof(stPlayBackTime));
		snprintf(stPlayBackTime.cPlayBackTime, sizeof(stPlayBackTime.cPlayBackTime) - 1, "%u", uiPlayTime);
		return IVS_SUCCEED;
	}

	if (0 == uiPlayTime)
	{
		if (m_fSpeed > 0)
		{
			uiPlayTime = m_stTimeSpanInter.iStartTime;//lint !e732
		}
		else
		{
			uiPlayTime = m_stTimeSpanInter.iEndTime;//lint !e732
		}
	}

	std::string strCurPlayTime;
    UTCTime2Str((time_t)uiPlayTime, strCurPlayTime, IVS_DATE_TIME_FORMAT);
    std::string strStartTimeTmp;
    UTCTime2Str((time_t)m_stTimeSpanInter.iStartTime, strStartTimeTmp, IVS_DATE_TIME_FORMAT);
    BP_RUN_LOG_INF("Get Play Back Time ", "Start time = %s %d Current time = %s.", strStartTimeTmp.c_str(), m_stTimeSpanInter.iStartTime, strCurPlayTime.c_str());
	bool bRet = CToolsHelp::Strncpy(stPlayBackTime.cPlayBackTime,IVS_TIME_LEN, strCurPlayTime.c_str(), strCurPlayTime.length());  
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Play Back Time", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    std::string strStartTime;
    UTCTime2Str((time_t)m_stTimeSpanInter.iStartTime, strStartTime, IVS_DATE_TIME_FORMAT);
	bRet = CToolsHelp::Strncpy(stPlayBackTime.cPlayBackStartTime,IVS_TIME_LEN+1, strStartTime.c_str(),strStartTime.length());  
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Play Back Time", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    std::string strEndTime;
    UTCTime2Str((time_t)m_stTimeSpanInter.iEndTime, strEndTime, IVS_DATE_TIME_FORMAT);
	bRet = CToolsHelp::Strncpy(stPlayBackTime.cPlayBackEndTime,IVS_TIME_LEN, strEndTime.c_str(), strEndTime.length());  
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Play Back Time", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    return iRet;
}
//设置录像回放时间
int CPlayback::SetPlayBackTime(unsigned int uiTime)
{
    CLockGuard lock(m_pMediaMutex);

    // 支持拖动到开始时间，即0 == uiTime的情况合法;
    /*if (0 == uiTime)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, 
                        "SetPlayBackTime", 
                        "Param invalid, setTime = 0");
        return IVS_PARA_INVALID;
    }*/

	IVS_PLAY_ResetData(m_ulPlayerChannel);

    if (!m_bLocalFile) 
	{
		unsigned int uiTime2Set = (unsigned int)m_stTimeSpanInter.iStartTime + uiTime;
		if (uiTime2Set >= (unsigned int)m_stTimeSpanInter.iEndTime)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, 
				"Set Play Back Time", 
				"Param invalid, uiTime is over end time, startTime = %d, end time = %d, set Time = %d", 
				m_stTimeSpanInter.iStartTime, 
				m_stTimeSpanInter.iEndTime, 
				uiTime);
			return IVS_PARA_INVALID;
		}

		BP_RUN_LOG_INF("Set Play Back Time", "RTSP_CLIENT_SendPlayMsg begin, rtsphandle=%d, speed=%f, settime=%u.", m_iRtspHandle, m_fSpeed, uiTime);

		MEDIA_RANGE_S stRange;
		memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
		stRange.enRangeType      = RANGE_TYPE_NPT;
		stRange.MediaBeginOffset = uiTime;
		float fSendScale = 0.0;
		(void)CToolsHelp::GetSendScale(m_enServiceType, m_fSpeed, fSendScale);
		int iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, 
			(double)fSendScale, &stRange, RTSP_CLIENT_TIMEOUT);

		BP_RUN_LOG_INF("Set Play Back Time", "RTSP_CLIENT_SendPlayMsg end, iRet=%d.", iRet);

		if (0 != iRet)
		{
			BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL,"Set Play Back Time", "RTSP_CLIENT_SendPlayMsg failed.");
			return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
		}
	}

	
    IVS_PLAY_SetPlayTime(m_ulPlayerChannel, m_tmStart + uiTime);

	/*
	 * DTS2013091303280
	 * 暂停或单帧状态下设置播放时间，
	 * 等待画面跳转到最新的时间点
	 */
	if (m_bPaused || m_bStepPlay)
	{
		(void)IVS_PLAY_ResumePlay(m_ulPlayerChannel);
		
		unsigned long long t1 = (unsigned long long)((m_tmStart + uiTime) * 1000);
		unsigned long long t2 = t1;
		(void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &t2);
        
		int loop = 100;
		while (t2 == t1 && --loop >= 0)
		{
			VOS_Sleep(20);
			(void)IVS_PLAY_GetCurrentFrameTick(m_ulPlayerChannel, &t2);
		}

		if (m_bPaused)
		{
			(void)RTSP_CLIENT_SendPauseMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
		}
		(void)IVS_PLAY_PausePlay(m_ulPlayerChannel);
	}

	m_uiDragPlayTime = uiTime;
    return IVS_SUCCEED;
}
//检查窗体句柄是否存在
bool CPlayback::IsExistHWnd(const HWND& hWnd) const
{
    return (hWnd == m_hWnd) ? true : false;
}
//开始回放
int CPlayback::StartPlay(const IVS_CHAR* pDomainCode,
	                     const IVS_CHAR* pNVRCode,
	                     const IVS_MEDIA_PARA* pstMediaPara,
                         const IVS_CHAR* pCameraCode,
                         const TIME_SPAN_INTERNAL& stTimeSpanInter,
                         START_PLAY_PARAM& stParam,
                         const char* pFileName)
{
    //校验参数
    if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || 0 == strlen(pCameraCode))
    {
        BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST,"Start Play Back", "camera code invalid");
        return IVS_PARA_INVALID;
    }
	IVS_DEBUG_TRACE("");
    //设置参数
	bool bRet = CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID), pCameraCode,strlen(pCameraCode));  
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start Play Back", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    int iRet = SetMediaParam(pstMediaPara);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Play Back", "set media param error");
        return iRet;
    }

    //从SCU获取媒体URL，解析数据保存在基类.
    iRet = DoGetUrlPlayBack(pDomainCode,pNVRCode,stParam.enServiceType, stTimeSpanInter, pFileName);
    if (IVS_SUCCEED != iRet) 
    {
        BP_RUN_LOG_ERR(iRet,"Start Play Back", "get url play back error, stream default.");
        return iRet;
    }

	//add by w00210470 保存启动启动时间和停止时间
	m_tmStart = stTimeSpanInter.iStartTime;
	m_tmEnd   = stTimeSpanInter.iEndTime;

    // 保存播放速率;
    m_fSpeed = stParam.fSpeed;

    // 回放需要以用户速度的2倍速度播放
    float fSendScale = m_fSpeed;
    (void)CToolsHelp::GetSendScale(m_enServiceType, m_fSpeed, fSendScale);
    stParam.fSpeed = fSendScale;

    // 调用基类启动播放;
    iRet = CMediaBase::StartPlay(stParam);
    if (iRet != IVS_SUCCEED)
    {
        m_fSpeed = 0.0;
        BP_RUN_LOG_ERR(iRet,"Start Play Back", "call base class failed.");
        return iRet;
    }
	
	if (m_fSpeed > 0)
	{
		IVS_PLAY_SetPlayTime(m_ulPlayerChannel, m_tmStart);
	}
	else
	{
		IVS_PLAY_SetPlayTime(m_ulPlayerChannel, m_tmEnd);
	}
    return iRet;
}
//获取回放Url
int CPlayback::DoGetUrlPlayBack(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
	                            IVS_SERVICE_TYPE ServiceType,
                                const TIME_SPAN_INTERNAL& stTimeSpanInter,
                                const char *pFileName)
{
	IVS_DEBUG_TRACE("");

    int iRet = IVS_FAIL;

    //请求消息;
    memset(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));

	if(NULL != pNVRCode)
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szNVRCode, sizeof(m_stUrlMediaInfo.szNVRCode),pNVRCode, strlen(pNVRCode));//lint !e534
	}
	else
	{
		memset(&m_stUrlMediaInfo.szNVRCode,0,IVS_NVR_CODE_LEN);//lint !e545
		m_pUserMgr->GetRecordList().GetRecordNVRInfo(m_stUrlMediaInfo.szSrcCode,stTimeSpanInter.iStartTime,stTimeSpanInter.iEndTime,m_stUrlMediaInfo.szNVRCode);
	}

    //校验参数
    if (NULL != pFileName)
    {
        (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szFileName, FILE_NAME_LEN,
                            pFileName, strlen(pFileName)+1);
    }

    //TODO : szDstPort无实际作用，传一个固定值；打包协议类型szDstPP入参结构体未定义
    m_stUrlMediaInfo.ServiceType = ServiceType;

    std::string strCameraDomainCode = m_szCameraID;
    std::string strCameraCode;
    std::string strDomainCode;
    iRet = CToolsHelp::SplitCameraDomainCode(m_szCameraID, strCameraCode, strDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "CToolsHelp::SplitCameraCode failed, RetCode=%d.", iRet);
        return iRet;
    }
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szSrcCode, sizeof(m_stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.size());//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstCode,sizeof(m_stUrlMediaInfo.szDstCode), "01",strlen("01"));//lint !e534 
	
	if(NULL == pDomainCode)
	{

		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534
	}
	else
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  pDomainCode, strlen(pDomainCode));//lint !e534
	}
	m_stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
    m_stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
    m_stUrlMediaInfo.PackProtocolType = PACK_PROTOCOL_ES;
    m_stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
	m_stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	m_stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	m_stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;

	

	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstIP, sizeof(m_stUrlMediaInfo.szDstIP), "",strlen(""));//lint !e534
    (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstPort, sizeof(m_stUrlMediaInfo.szDstPort), "35016",strlen("35016"));//lint !e534
    
    std::string strTime;
    UTCTime2Str((time_t)stTimeSpanInter.iStartTime, strTime, IVS_DATE_TIME_NET_FORMAT);
    strTime.append("-");
    std::string strEndTime;
    UTCTime2Str((time_t)stTimeSpanInter.iEndTime, strEndTime, IVS_DATE_TIME_NET_FORMAT);   
    strTime.append(strEndTime);
    if (TIME_SPAN_LEN <= strTime.size())
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "Parse time span from rsp url failed. time string over max length. strTime=%s.", strTime.c_str());
        return iRet;
    }
    (void)CToolsHelp::Memcpy(m_stUrlMediaInfo.szTimeSpan,TIME_SPAN_LEN, strTime.c_str(), strTime.length());
    //获取媒体URL
    iRet = GetURL(CToolsHelp::GetSCCHead().c_str(), m_pUserMgr, m_stUrlMediaInfo, m_stMediaRsp, m_bAssociatedAudio);//lint !e838
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "platform play back get URL failed!");
        return iRet;
    }

	iRet = ParseIPAddr(m_stMediaRsp.szURL,m_strMuIpAddr);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "parse IP Addr failed from url: %s",
			m_stMediaRsp.szURL);
		return iRet;
	}

    //解析URL,得到ServiceType，如果是组播实况（直连），那么协议类型转为udp
    std::string strServiceType;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_SERVICE_TYPE_FLAG, strServiceType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "parse service type from url failed, URL=%s",
                       m_stMediaRsp.szURL);
        return iRet;
    }
    int iServiceType = CToolsHelp::StrToInt(strServiceType);

    std::string strMediaTransMode;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_MEDIA_TRANS_MODE, strMediaTransMode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "parse media trans mode failed from url");
        return iRet;
    }
    int iMediaTransMode = CToolsHelp::StrToInt(strMediaTransMode);

    std::string strBroadcastType;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_BROADCAST_TYPE, strBroadcastType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "parse broadcast type failed from url");
        return iRet;
    }
    int iBroadcastType = CToolsHelp::StrToInt(strBroadcastType);

    std::string strTimeSpan;
    iRet = ParseURL(m_stMediaRsp.szURL, URL_TIME_SPAN, strTimeSpan);
    if (IVS_SUCCEED != iRet || strTimeSpan.empty())
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "parse timespan failed from url");
        return iRet;
    }

    unsigned int uiPos = strTimeSpan.find("-");
    if (std::string::npos == uiPos || uiPos >= strTimeSpan.length())
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "parse timespan failed from url,find spliter failed.");
        return iRet;
    }
    std::string strStartTime = strTimeSpan.substr(0, uiPos);
    if (std::string::npos == (uiPos+1) || (uiPos+1) >= strTimeSpan.length())
    {
        BP_RUN_LOG_ERR(iRet,"Do Get Url Play Back", "parse timespan failed from url,find spliter failed.");
        return iRet;
    }
    strEndTime = strTimeSpan.substr(uiPos+1, strlen(IVS_DATE_TIME_NET_FORMAT));
    
    m_stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(strStartTime.c_str(), IVS_DATE_TIME_NET_FORMAT);
    m_stTimeSpanInter.iEndTime = (IVS_INT32)UTCStr2Time(strEndTime.c_str(), IVS_DATE_TIME_NET_FORMAT);

    BP_RUN_LOG_INF("Do Get Url Play Back", "strStartTime=%s strEndTime=%s iStartTime=%d iEndTime=%d.", strStartTime.c_str(), strEndTime.c_str(), m_stTimeSpanInter.iStartTime, m_stTimeSpanInter.iEndTime);

    m_enServiceType = (IVS_SERVICE_TYPE)iServiceType;
    m_stuMediaParam.TransMode = (IVS_MEDIA_TRANS_MODE)iMediaTransMode;
    m_stuMediaParam.BroadCastType = (IVS_BROADCAST_TYPE)iBroadcastType;
    

    // 直连组播实况，需要将协议类型转换为UDP;
    if (SERVICE_TYPE_REALVIDEO == m_enServiceType 
        && MEDIA_DIRECT == m_stuMediaParam.TransMode
        && BROADCAST_MULTICAST == m_stuMediaParam.BroadCastType)
    {
        m_stuMediaParam.ProtocolType = PROTOCOL_RTP_OVER_UDP;
    }

    //// add for 流复用 [12/17/2012 z90003203] // TODO 疑问：这个流复用，服务器会给内容？
    //int iStreamType = 0;
    //iRet = ParseURL(m_stMediaRsp.szURL, URL_STREAM_TYPE, &iStreamType);
    //if (IVS_SUCCEED != iRet)
    //{
    //    BP_RUN_LOG_ERR(iRet,"GetUrlRealPlay", "parse stream type failed from url");
    //    return iRet;
    //}

    //m_stuMediaParam.StreamType = (IVS_STREAM_TYPE)iStreamType;

    //if (STREAM_TYPE_UNSPECIFIED == m_stuMediaParam.StreamType)
    //{
    //    m_stuMediaParam.StreamType = STREAM_TYPE_MAIN;
    //}
    //// end add

    return iRet;
}//lint !e529

//void CPlayback::DoRawFrameCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo, IVS_CHAR* pFrame, IVS_UINT32 uFrameSize)
//{
//    VOS_MutexLock(m_pRealPlayRawCBMutex);
//    if (NULL != m_fRealPlayCallBackRaw)
//    {
//        m_fRealPlayCallBackRaw(m_ulHandle, pstRawFrameInfo, pFrame, uFrameSize, m_pRealPlayUserDataRaw);
//    }
//    VOS_MutexUnlock(m_pRealPlayRawCBMutex);
//}

// 开始前端录像回放
int CPlayback::StartPUPlayBack(const char* pCameraCode,
                               unsigned int uiProtocolType,
                               const TIME_SPAN_INTERNAL& stTimeSpanInter,
                               HWND hWnd,
                               float fSpeed,
                               const char* pFileName)
{
#if 0  
	if(PLAY_TYPE_HWND == m_iStartType && NULL == hWnd)
    {
        BP_RUN_LOG_ERR(IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED,"Start PU Play Back", "hwnd invalid");
        return IVS_PARA_INVALID;
    }
#endif 
    CLockGuard lock(m_pMediaMutex);

    IVS_MEDIA_PARA stMediaPara;
	InitMediaPara(stMediaPara);
    stMediaPara.ProtocolType = static_cast<IVS_PROTOCOL_TYPE>(uiProtocolType);//此处转换肯定正确，前面已做检查;

    START_PLAY_PARAM stParam;
    //TODO:待更新结构体
    stParam.enServiceType = SERVICE_TYPE_PU_PLAYBACK;
    stParam.fSpeed = fSpeed;
    stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
    stParam.cbNetDataCallBack = g_fNetDataCallBack;
    //开始回放;
    int iRet = StartPlay(NULL,NULL,&stMediaPara, pCameraCode, stTimeSpanInter, stParam, pFileName);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start PU Play Back", "Start play error.");
        return iRet;
    }

    SetServiceType(stParam.enServiceType);

    (void)IVS_PLAY_SetPlaySpeed(m_ulPlayerChannel, m_fSpeed);//lint !e747
    iRet = IVS_PLAY_SetPlayWindow(m_ulPlayerChannel, hWnd);
    if (IVS_SUCCEED == iRet)
    {
        m_hWnd = hWnd;
    }

    return iRet;
}

// 停止前端录像回放
int CPlayback::StopPUPlayBack()
{
	IVS_DEBUG_TRACE("");
    CLockGuard lock(m_pMediaMutex);

    int iRet = CMediaBase::StopPlay();

    // 清空成员变量;
    m_hWnd = NULL;
    m_fSpeed = 0.0;
    m_uiDragPlayTime = 0;

    //重置参数
    RetSet();
    if (iRet != IVS_SUCCEED)
    {
        BP_RUN_LOG_ERR(iRet,"Stop PU Play Back", "call MediaBase::StopPlay failed.");
        iRet = IVS_FAIL;
    }
    return iRet;
}

// 通知流缓冲区状态
IVS_INT32 CPlayback::NotifyStreamBufferState(IVS_INT32 iBufferState)
{
    IVS_DEBUG_TRACE("");
    // 用户暂停时，不做处理
    if (m_bPaused)
    {
        BP_DBG_LOG("----------In pause state, m_bPaused = true");
        return IVS_SUCCEED;
    }

	return CMediaBase::NotifyStreamBufferState(iBufferState);
}

IVS_VOID CPlayback::StopNotifyBufEvent()
{
	IVS_DEBUG_TRACE("");
	//CLockGuard lock(m_pMediaMutex);
	IVS_PLAY_StopNotifyBuffEvent(m_ulPlayerChannel);
}

//停止备份录像回放
int CPlayback::StopBackupPlayBack()
{
	IVS_DEBUG_TRACE("");
	CLockGuard lock(m_pMediaMutex);
	
	int iRet = CMediaBase::StopPlay();

	//重置参数
	RetSet();
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet,"Stop Backup Play Back", "call MediaBase::StopPlay failed.");
		iRet = IVS_FAIL;
	}
	return iRet;
}

int CPlayback::AddPlayWindow(HWND hWnd)
{
	return IVS_PLAY_SetPlayWindow(m_ulPlayerChannel, hWnd);
}

int CPlayback::DelPlayWindow(HWND hWnd)
{
	return IVS_PLAY_DelPlayWindow(m_ulPlayerChannel, hWnd);
}

int CPlayback::AddPlayWindowPartial(HWND hWnd, void* pPartial)
{
	return IVS_PLAY_AddPlayWindowPartial(m_ulPlayerChannel, hWnd, (LAYOUT_DRAW_PARTIAL*)pPartial);
}

int CPlayback::UpdateWindowPartial(HWND hWnd, void* pPartial)
{
	return IVS_PLAY_UpdateWindowPartial(m_ulPlayerChannel, hWnd, (LAYOUT_DRAW_PARTIAL*)pPartial);
}

/*
 * StartLocalPlayBack
 * 启动本地回放
 */
int CPlayback::StartLocalPlayBack(const IVS_CHAR* pFileName, 
	     const IVS_LOCAL_PLAYBACK_PARAM* pPlaybackParam, HWND hWnd)
{
    int iRet = IVS_PLAY_GetChannel(&m_ulPlayerChannel);
	if (IVS_SUCCEED != iRet)
	{
	    BP_RUN_LOG_ERR(iRet,"Start LocalPlayBack", "No Play Channel");
		return iRet;
	}
	
	iRet = IVS_PLAY_OpenFile(m_ulPlayerChannel, pFileName);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start LocalPlayBack", "Open File Fail");
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		return iRet;
	}
	SetServiceType(SERVICE_TYPE_PLAYBACK);
	iRet = IVS_PLAY_SetPlayWindow(m_ulPlayerChannel, hWnd);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start LocalPlayBack", "Add Play Window Fail");
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		return iRet;
	}

	m_fSpeed = pPlaybackParam->fSpeed; 
	m_tmStart = pPlaybackParam->uiStartTime;
	IVS_PLAY_SetFileTimeRange(m_ulPlayerChannel, 
		pPlaybackParam->uiStartTime, pPlaybackParam->uiEndTime);

	if (m_fSpeed < 0)
	{
	    IVS_PLAY_SetPlayTime(m_ulPlayerChannel, pPlaybackParam->uiEndTime);//lint !e747
	}
	else
	{
		IVS_PLAY_SetPlayTime(m_ulPlayerChannel, pPlaybackParam->uiStartTime);//lint !e747
	}

	IVS_PLAY_SetPlaySpeed(m_ulPlayerChannel, m_fSpeed);//lint !e747
	iRet = IVS_PLAY_StartPlay(m_ulPlayerChannel);
	
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start LocalPlayBack", "Start Play Fail");
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = 0;
		return iRet;
	}

	m_bLocalFile = true;
	return IVS_SUCCEED;
}

/*
 * StopLocalPlayBack
 * 停止本地文件回放
 */
int CPlayback::StopLocalPlayBack()
{
	IVS_DEBUG_TRACE("");
	if (m_ulPlayerChannel)
	{
		IVS_PLAY_StopPlay(m_ulPlayerChannel);
		IVS_PLAY_FreeChannel(m_ulPlayerChannel);
		m_ulPlayerChannel = NULL;

		RetSet();
	}
	return IVS_SUCCEED;
}

void CPlayback::RetSet()
{
	CMediaBase::RetSet();
	m_fPlayBackCallBackRaw = NULL;
	m_pPlayBackUserDataRaw = NULL;
	m_fPlayBackCallBackFrame = NULL;
	m_pPlayBackUserDataFrame = NULL;

	m_hWnd = NULL;
	m_bPaused = false;
	m_bLocalFile = false;
	m_FirstRtpStamp = 0;
	m_uiDragPlayTime = 0;
	memset(&m_stTimeSpanInter, 0, sizeof(TIME_SPAN_INTERNAL));
}

void CPlayback::DoRawFrameCallBack(IVS_RAW_FRAME_INFO* pstRawFrameInfo, IVS_CHAR* pFrame, IVS_UINT32 uFrameSize)
{
	(void)VOS_MutexLock(m_pPlayBackRawCBMutex);
	if (NULL != m_fPlayBackCallBackRaw)
	{
		m_fPlayBackCallBackRaw(m_ulHandle, pstRawFrameInfo, pFrame, uFrameSize, m_pPlayBackUserDataRaw);
	}
	(void)VOS_MutexUnlock(m_pPlayBackRawCBMutex);
}

void CPlayback::DoFrameCallBack(IVS_INT32 iStreamType, IVS_VOID* pFrameInfo)
{
	(void)VOS_MutexLock(m_pPlayBackFrameDecoderCBMutex);
	if (NULL != m_fPlayBackCallBackFrame)
	{
		m_fPlayBackCallBackFrame(m_ulHandle, (unsigned int)iStreamType, pFrameInfo, m_pPlayBackUserDataFrame);
	}
	(void)VOS_MutexUnlock(m_pPlayBackFrameDecoderCBMutex);
}


// 设置视频质量
IVS_INT32 CPlayback::SetVideoEffect(IVS_LONG lBrightValue, 
	IVS_LONG lContrastValue, IVS_LONG lSaturationValue, 
	IVS_LONG lHueValue)
{
	CLockGuard lock(m_pMediaMutex);

	IVS_INT32 iRet = IVS_PLAY_SetVideoEffect(m_ulPlayerChannel, lBrightValue, lContrastValue, lSaturationValue, lHueValue);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Set Video Effect", "Play set video effect failed");
		return iRet;
	}

	m_lBrightValue = lBrightValue;
	m_lContrastValue = lContrastValue;
	m_lSaturationValue = lSaturationValue;
	m_lHueValue = lHueValue;

	return IVS_SUCCEED;
}