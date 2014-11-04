/********************************************************************
	filename	: 	Download.cpp
	author		:	z00201790
	created		:	2012/12/06	
	description	:	定义录像下载业务类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本;
*********************************************************************/
#include <time.h>
#include "Download.h"
#include "IVS_Player.h"
#include "LockGuard.h"
#include "IVS_Trace.h"
#include "bp_os.h"
#include "UserMgr.h"


// #define SCALE_BACK_32           ((double)-32.0) //默认最小速率
// #define SCALE_DEFAULT           ((double)1.0)   //默认速率
// #define SCALE_FORWARD_4         ((double)4.0)   //四倍速前进
// #define SCALE_FORWARD_RESERVED  ((double)256.0) //预留播放速度
// #define SCALE_FORWARD_UNLIMIT   ((double)255.0) //不限速
// #define SCALE_FORWARD_8_ALL     ((double)254.0) //8倍速全量前进
// #define SCALE_BACK_2_ALL        ((double)253.0) //2倍速全量后退
// #define SCALE_BACK_1_ALL        ((double)252.0) //1倍速全量后退
// #define SCALE_FORWARD_4_KEY	    ((double)251.0) //4倍速前进只播放I帧


// Rtsp媒体数据回调
static void  __SDK_CALL g_fRtspDataCallBack(int /*handle*/, char *pBuffer, unsigned int ulBufSize, void *pUser)//lint !e528 
{
	if ((NULL == pBuffer)|| (0 == ulBufSize))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, 
			"Download g_fRtsp Data Call Back", "Input param invalid");
		return;
	}

	if (*pBuffer != '$')
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_DATA_ERROR, 
			"Download g_fRtsp Data Call Back", "rtsp media data format error, do not have $.");
		return;
	}

	if (1 == *(pBuffer+1) || 3 == *(pBuffer+1))
	{
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_RTSP_DATA_ERROR, 
			"Download g_fRtsp Data Call Back", "rtsp media data format error, do not 1 or 3.");
		return;
	}

	//获取回调出来的类对象;
	CMediaBase* pMediaBase = static_cast<CMediaBase*>(pUser);
	if (NULL == pMediaBase)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Download g_fRtsp Data Call Back",
			"transform pUser to MediaBase pointer failed.");
		return;
	}

	unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
	if (0 == uiPlayerChannel)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Download g_fRtsp Data Call Back", 
			 "pMediaBase's uiPlayerChannel = 0.");
		return;
	}

	pBuffer   += 4;
	ulBufSize -= 4;
	(void) IVS_PLAY_InputRtpData(uiPlayerChannel, pBuffer, ulBufSize); //通过TCP协议控制
} //lint !e818 !e528

static void __SDK_CALL g_fNetDataCallBack(char *pBuf, unsigned int uiSize, void *pUser)//lint !e818 !e830
{
	//获取回调出来的类对象;
	CMediaBase* pMediaBase = reinterpret_cast<CMediaBase*>(pUser);
	if (NULL == pMediaBase)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Download Net Data Call Back", "transform pUser to MediaBase pointer failed.");
		return;
	}

	unsigned int uiPlayerChannel = pMediaBase->GetPlayerChannel();
	if (0 == uiPlayerChannel)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"Download Net Data Call Back", "pMediaBase's uiPlayerChannel = 0.");
		return;
	}

	(void)IVS_PLAY_InputRtpData(uiPlayerChannel, pBuf, uiSize);
}//lint !e818 !e528



CDownload::CDownload(void)
{
	m_uLastPercent  = 0; 
	m_uLastLeftTime = 0;
	m_ullLastInfoTick = 0;
	m_ullPastTime = 0;
    m_fSpeed = 1.0;                       
	m_enServiceType = SERVICE_TYPE_DOWNLOAD;
	m_bPaused   = false;
	memset(&m_stTimeSpanInter, 0, sizeof(m_stTimeSpanInter));
}


CDownload::~CDownload(void)
{
}


void CDownload::SetHandle(unsigned long /*ulHandle*/)
{

}//lint !e1762

unsigned long  CDownload::GetHandle()
{
	return 0;
}//lint !e1762


int CDownload::StartPlatformDownLoad(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode, const IVS_DOWNLOAD_PARAM *pDownloadParam)
{
	CHECK_POINTER(pDownloadParam,IVS_PARA_INVALID);

	//CLockGuard lock(m_pMediaMutex);
	IVS_MEDIA_PARA stMediaPara;
	memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));

	// 设置默认值;
	stMediaPara.ProtocolType   = PROTOCOL_RTP_OVER_TCP;
	stMediaPara.DecoderCompany = VENDOR_HUAWEI;
	stMediaPara.StreamType     = STREAM_TYPE_MAIN;
	stMediaPara.TransMode      = MEDIA_TRANS;
	stMediaPara.BroadCastType  = BROADCAST_UNICAST;
	stMediaPara.VideoDecType   = VIDEO_DEC_H264;
	stMediaPara.AudioDecType   = AUDIO_DEC_G711A;

	START_PLAY_PARAM stParam;
	stParam.enSourceType       = SOURCE_RTP;
	stParam.enServiceType      = SERVICE_TYPE_DOWNLOAD;/*SERVICE_TYPE_DOWNLOAD*/;/*SERVICE_TYPE_PLAYBACK*/;/*SERVICE_TYPE_DOWNLOAD FIX ME 目前使用 SERVICE_TYPE_DOWNLOAD 无法获取到URL*/; 
	if(NULL != pDomainCode)
	{
		stParam.enServiceType      = SERVICE_TYPE_BACKUPRECORD_DOWNLOAD;
	}
	stParam.fSpeed             = (float)pDownloadParam->uiDownloadSpeed; //fSpeed;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
	stParam.cbNetDataCallBack  = g_fNetDataCallBack;

	TIME_SPAN_INTERNAL stTimeSpanInter;
	stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cStart, IVS_DATE_TIME_FORMAT);
	stTimeSpanInter.iEndTime   = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cEnd,   IVS_DATE_TIME_FORMAT);
	if (stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "time span invalid");
		return IVS_PARA_INVALID;
	}
	
	int iRet = StartDownLoad(pDomainCode,pNVRCode,pDownloadParam, &stMediaPara, pCameraCode, stTimeSpanInter, stParam, NULL);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Start Platform DownLoad", "Start DownLoad error");
		return iRet;
	}

	// FIX ME 目前使用 SERVICE_TYPE_DOWNLOAD 无法获取到URL
	stParam.enServiceType = SERVICE_TYPE_DOWNLOAD;
	SetServiceType(stParam.enServiceType);
	m_uLastLeftTime  = m_tmEnd - m_tmStart; //lint !e712

	return IVS_SUCCEED;
}


int CDownload::StopPlatformDownLoad()
{
	int iRet = CMediaBase::StopDownLoad();
	m_fSpeed = 0.0;
	RetSet();
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet, "Stop Platform DownLoad", "Stop DownLoad failed.");
		iRet = IVS_FAIL;
	}
	return iRet;

}

// 容灾
int CDownload::StartDisasterRecoveryDownload(const char* pCameraCode, const IVS_DOWNLOAD_PARAM *pDownloadParam)
{
	CHECK_POINTER(pDownloadParam,IVS_PARA_INVALID);

    //CLockGuard lock(m_pMediaMutex);
    IVS_MEDIA_PARA stMediaPara;
    memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));

    // 设置默认值;
    stMediaPara.ProtocolType   = PROTOCOL_RTP_OVER_TCP;
    stMediaPara.DecoderCompany = VENDOR_HUAWEI;
    stMediaPara.StreamType     = STREAM_TYPE_MAIN;
    stMediaPara.TransMode      = MEDIA_TRANS;
    stMediaPara.BroadCastType  = BROADCAST_UNICAST;
    stMediaPara.VideoDecType   = VIDEO_DEC_H264;
    stMediaPara.AudioDecType   = AUDIO_DEC_G711A;

    START_PLAY_PARAM stParam;
    stParam.enSourceType       = SOURCE_RTP;
    // TODO 添加容灾服务类型
    stParam.enServiceType      = SERVICE_TYPE_DISASTEBACKUP_DOWNLOAD;/*SERVICE_TYPE_DOWNLOAD*/;/*SERVICE_TYPE_PLAYBACK*/;/*SERVICE_TYPE_DOWNLOAD FIX ME 目前使用 SERVICE_TYPE_DOWNLOAD 无法获取到URL*/; 
    stParam.fSpeed             = (float)pDownloadParam->uiDownloadSpeed; //fSpeed;
    stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
    stParam.cbNetDataCallBack  = g_fNetDataCallBack;

    TIME_SPAN_INTERNAL stTimeSpanInter;
    stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cStart, IVS_DATE_TIME_FORMAT);
    stTimeSpanInter.iEndTime   = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cEnd,   IVS_DATE_TIME_FORMAT);
    if (stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "time span invalid");
        return IVS_PARA_INVALID;
    }

    // TODO
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    if (NULL != this->m_pUserMgr)
    {
        // 获取域路由，记录日志
        CDomainRouteMgr& domainRouteMgr = m_pUserMgr->GetDomainRouteMgr();

        string strLocalDomainCode;
        domainRouteMgr.GetLocalDomainCode(strLocalDomainCode);

        //this->m_pUserMgr->GetDomainCode(strLocalDomainCode);
        if (!CToolsHelp::Memcpy(chDomaCode,IVS_DOMAIN_CODE_LEN,strLocalDomainCode.c_str(),strLocalDomainCode.length()))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Query Record List", "memcpy failed");
            return IVS_ALLOC_MEMORY_ERROR;
        }
       // memset(chDomaCode,'\0',IVS_DOMAIN_CODE_LEN + 1);
        IVS_DEBUG_TRACE(" record method disaster recovery set domain is local[%s]. ",strLocalDomainCode.c_str());
    }

    int iRet = StartDownLoad(chDomaCode,NULL,pDownloadParam, &stMediaPara, pCameraCode, stTimeSpanInter, stParam, NULL);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Start Platform DownLoad", "Start DownLoad error");
        return iRet;
    }

    // FIX ME 目前使用 SERVICE_TYPE_DOWNLOAD 无法获取到URL

    SetServiceType(stParam.enServiceType);
    m_uLastLeftTime  = m_tmEnd - m_tmStart; //lint !e712

    return IVS_SUCCEED;
}

// 容灾
int CDownload::StopDisasterRecoveryDownload()
{
    int iRet = CMediaBase::StopDownLoad();
    m_fSpeed = 0.0;
    RetSet();
    if (iRet != IVS_SUCCEED)
    {
        BP_RUN_LOG_ERR(iRet, "Stop Platform DownLoad", "Stop DownLoad failed.");
        iRet = IVS_FAIL;
    }
    return iRet;

}

int  CDownload::DownLoadPause()
{
	CLockGuard lock(m_pMediaMutex);
	int iRet = IVS_FAIL;
	if (RTSP_SESSION_STATUS_PLAYING == RTSP_CLIENT_GetStatus(m_iRtspHandle))
	{
		iRet = RTSP_CLIENT_SendPauseMsg(m_iRtspHandle, m_stMediaRsp.szURL, RTSP_CLIENT_TIMEOUT);
		if (0 != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Play Back Pause", "RTSP client send pause play fail.");
			return IVS_PLAYER_RET_RTSP_SEND_PAUSE_FAIL;
		}
	}

	(void)IVS_PLAY_PausePlay(m_ulPlayerChannel);
	m_bPaused = true;
	return IVS_SUCCEED;
}//lint !e1762

int  CDownload::DownLoadResume()
{
	CLockGuard lock(m_pMediaMutex);

	m_bStepPlay = false;
	int iRet = IVS_FAIL;
	if (RTSP_SESSION_STATUS_READY == RTSP_CLIENT_GetStatus(m_iRtspHandle))
	{
		MEDIA_RANGE_S stRange;
		memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
		stRange.enRangeType = RANGE_TYPE_UTC;
		stRange.MediaBeginOffset = OFFSET_CUR;
		iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)m_fSpeed, &stRange, RTSP_CLIENT_TIMEOUT);
		if (0 != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
			return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
		}
	}
	m_bPaused = false;
	//m_ullLastInfoTick = 0; // reset
	return IVS_SUCCEED;
}//lint !e1762

int CDownload::SetDownLoadSpeed(float fSpeed)
{
	CLockGuard lock(m_pMediaMutex);
	int iRet = IVS_FAIL;
	RTSP_SESSION_STATUS state = RTSP_CLIENT_GetStatus(m_iRtspHandle);

	if (RTSP_SESSION_STATUS_PLAYING == state ||
		RTSP_SESSION_STATUS_READY   == state)
	{
		MEDIA_RANGE_S stRange;
		memset(&stRange, 0x0, sizeof(MEDIA_RANGE_S));
		stRange.enRangeType = RANGE_TYPE_UTC;
		stRange.MediaBeginOffset = OFFSET_CUR;
		m_fSpeed = fSpeed;
		iRet = RTSP_CLIENT_SendPlayMsg(m_iRtspHandle, m_stMediaRsp.szURL, (double)m_fSpeed, &stRange, RTSP_CLIENT_TIMEOUT);
		if (0 != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Play Back Resume", "IVS pause play fail.");
			return IVS_PLAYER_RET_RTSP_SEND_PLAY_FAIL;
		}
	}
	return IVS_SUCCEED;
}//lint !e1762

int CDownload::GetDownLoadInfo(IVS_DOWNLOAD_INFO* pInfo)
{
	CHECK_POINTER(pInfo,IVS_PARA_INVALID);

	unsigned long long DownloadSize  = 0;
	unsigned int DownloadSpeed = 0;
    unsigned int RecordTime    = 0;

	int result = IVS_PLAY_GetRecordStatis(m_ulPlayerChannel,
		  &RecordTime, &DownloadSpeed, &DownloadSize);

	if (IVS_SUCCEED != result)
	{
		BP_RUN_LOG_ERR(result, "Get DownLoad Info", "Fail Error=%d", result);
		return result;
	}
	
	pInfo->uiDownloadStatus = m_bPaused ? 0 : 1;
	pInfo->uiDownloadSize   = DownloadSize / 1024;
	pInfo->uiDownloadSpeed  = DownloadSpeed / 1024;
	pInfo->uiProgress       = (RecordTime / 1000) * 100 /(m_tmEnd - m_tmStart); //lint !e647 !e732
	pInfo->uiProgress       = pInfo->uiProgress > 100 ? 100 : pInfo->uiProgress;

	if (m_fSpeed < 255.0 && m_fSpeed >= 1.0)
	{
		unsigned int extra = (100 - pInfo->uiProgress) * (m_tmEnd - m_tmStart) / 100; //lint !e712
	 	pInfo->uiTimeLeft = extra / m_fSpeed;   //lint !e524 !e732
	}
	else
	{
#if 0		
		time_t tmNow = time(NULL);
		if (0 != m_TmLastGetInfo)
		{
			unsigned int diff = tmNow - m_TmLastGetInfo; //lint !e712
			if (diff > 0)
			{
				unsigned int PercentInSec = (pInfo->uiProgress - m_uLastPercent) / diff;
				if (0 != PercentInSec)
				{
					pInfo->uiTimeLeft = (100 - pInfo->uiProgress) / PercentInSec;
				}
				else
				{
					pInfo->uiTimeLeft = m_uLastLeftTime;
				}
			}
			else
			{
				pInfo->uiTimeLeft = m_uLastLeftTime;
			}
		}
		else
		{
			pInfo->uiTimeLeft = m_tmEnd - m_tmStart; //lint !e712
		}
		
		m_TmLastGetInfo = tmNow;
    	m_uLastPercent  = pInfo->uiProgress;
#endif 

		ACE_Time_Value tv = BP_OS::gettimeofday();
		unsigned long long now = tv.sec() * 1000 + tv.usec() / 1000;//lint !e732 

		if (m_ullLastInfoTick)
		{
			unsigned long long diffMS = now - m_ullLastInfoTick; //lint !e712
			
			if (!m_bPaused)
			{
				m_ullPastTime += diffMS;
			}
			
			if (0 == pInfo->uiProgress)
			{
				pInfo->uiTimeLeft = 0;
				BP_DBG_LOG("IVS Play RecordStatis: CurrentProgress is zero, cannot calculate left time");
			}
			else
			{
				if (pInfo->uiProgress == m_uLastPercent )
				{
					pInfo->uiTimeLeft = m_uLastLeftTime;
				}
				else
				{
					pInfo->uiTimeLeft = (unsigned int)((100 - pInfo->uiProgress) * m_ullPastTime)/(pInfo->uiProgress*1000);//lint !e647
				}

				BP_DBG_LOG("IVS Play RecordStatis: CurrentProgress[%u] LastPercent[%u] PastTime[%llu] TimeLeft[%u]", pInfo->uiProgress, m_uLastPercent, m_ullPastTime, pInfo->uiTimeLeft);
			}
		}
		else
		{
			m_ullPastTime = 0;
			pInfo->uiTimeLeft = 0;
			BP_DBG_LOG("IVS Play RecordStatis: FirstTime, cannot calculate left time");
		}

		m_ullLastInfoTick =  now;
		m_uLastPercent = pInfo->uiProgress;
	}

	m_uLastLeftTime = pInfo->uiTimeLeft;

	BP_DBG_LOG("IVS Play RecordStatis:RecordTime[%u] DownloadSpeed[%u] DownloadSize[%u] DownloadInfo: Status[%u] Size[%u] Speed[%u] Progress[%u] TimeLeft[%u]",
		RecordTime, DownloadSpeed, DownloadSize, pInfo->uiDownloadStatus, pInfo->uiDownloadSize, pInfo->uiDownloadSpeed, pInfo->uiProgress, pInfo->uiTimeLeft);
	return result;
}


int CDownload::StartPUDownLoad(const char* pCameraCode, 
	      const IVS_DOWNLOAD_PARAM* pDownloadParam, const char *pFileName)
{
	CHECK_POINTER(pDownloadParam,IVS_PARA_INVALID);

	CLockGuard lock(m_pMediaMutex);

	IVS_MEDIA_PARA stMediaPara;
	memset(&stMediaPara, 0, sizeof(IVS_MEDIA_PARA));
	stMediaPara.ProtocolType   = PROTOCOL_RTP_OVER_TCP;
	stMediaPara.DecoderCompany = VENDOR_HUAWEI;
	stMediaPara.StreamType     = STREAM_TYPE_MAIN;
	stMediaPara.TransMode      = MEDIA_TRANS;
	stMediaPara.BroadCastType  = BROADCAST_UNICAST;
	stMediaPara.VideoDecType   = VIDEO_DEC_H264;
	stMediaPara.AudioDecType   = AUDIO_DEC_G711A;

    START_PLAY_PARAM stParam;
	stParam.enSourceType       = SOURCE_RTP;
	stParam.enServiceType      = SERVICE_TYPE_PU_DOWNLOAD;/*SERVICE_TYPE_DOWNLOAD 无法获取到URL */
	stParam.fSpeed             = pDownloadParam->uiDownloadSpeed;
	stParam.cbRtspDataCallBack = g_fRtspDataCallBack;
	stParam.cbNetDataCallBack  = g_fNetDataCallBack;

	TIME_SPAN_INTERNAL stTimeSpanInter;
	stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cStart, IVS_DATE_TIME_FORMAT);
	stTimeSpanInter.iEndTime   = (IVS_INT32)UTCStr2Time(pDownloadParam->stTimeSpan.cEnd,   IVS_DATE_TIME_FORMAT);
	if (stTimeSpanInter.iStartTime >= stTimeSpanInter.iEndTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "time span invalid");
		return IVS_PARA_INVALID;
	}

	//开始回放;
	int iRet = StartDownLoad(NULL,NULL,pDownloadParam, &stMediaPara, pCameraCode, stTimeSpanInter, stParam, pFileName);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start PU Play Back", "Start play error.");
		return iRet;
	}

	// FIX ME 目前使用 SERVICE_TYPE_DOWNLOAD 无法获取到URL
	stParam.enServiceType = SERVICE_TYPE_PU_DOWNLOAD;
	SetServiceType(stParam.enServiceType);
	m_uLastLeftTime     = m_tmEnd - m_tmStart; //lint !e712

	return 0;
}


int CDownload::StopPUDownLoad()
{
	int iRet = CMediaBase::StopDownLoad();
    m_fSpeed = 0.0;
    RetSet();
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet, "Stop PU DownLoad", "StopDownLoad failed.");
		iRet = IVS_FAIL;
	}
	return iRet;
}


/*
 * StartDownLoad
 * 启动下载
 */
int CDownload::StartDownLoad(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_DOWNLOAD_PARAM *pParam,const IVS_MEDIA_PARA* pstMediaPara, const IVS_CHAR* pCameraCode, 
	    const TIME_SPAN_INTERNAL& stTimeSpanInter, const START_PLAY_PARAM& stParam,	const char* pFileName)
{
    if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || 0 == strlen(pCameraCode))
	{
		BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST, "Start DownLoad", "camera code invalid");
		return IVS_PARA_INVALID;
	}
	
	IVS_DEBUG_TRACE("");

    bool bRet = CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID), pCameraCode, strlen(pCameraCode));  
	if (!bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Start DownLoad", "Strncpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	int iRet = SetMediaParam(pstMediaPara);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start DownLoad", "set media param error");
		return iRet;
	}

	//从SCU获取媒体URL，解析数据保存在基类.
	iRet = DoGetUrlDownLoad(pDomainCode,pNVRCode,stParam.enServiceType, stTimeSpanInter, pFileName);
	if (IVS_SUCCEED != iRet) 
	{
		BP_RUN_LOG_ERR(iRet,"Start download", "get url play back error, stream default.");
		return iRet;
	}

	//add by w00210470 保存启动启动时间和停止时间
	m_tmStart = stTimeSpanInter.iStartTime;
	m_tmEnd   = stTimeSpanInter.iEndTime;

	// 调用基类启动播放;
    iRet = CMediaBase::StartDownload(stParam, pCameraCode, pParam);
	if (iRet != IVS_SUCCEED)
	{
		m_fSpeed = 0.0;
		BP_RUN_LOG_ERR(iRet,"Start download", "call base class failed.");
		return iRet;
	}

	m_fSpeed = stParam.fSpeed;
	return IVS_SUCCEED;
}

// 获取download url
int CDownload::DoGetUrlDownLoad(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,IVS_SERVICE_TYPE ServiceType, 
	    const TIME_SPAN_INTERNAL& stTimeSpanInter, const char *pFileName)
{
	IVS_DEBUG_TRACE("");
	int iRet = IVS_FAIL;

	//请求消息;
	memset(&m_stUrlMediaInfo, 0, sizeof(ST_URL_MEDIA_INFO));

	//校验参数
	if (NULL != pFileName)
	{
	     (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szFileName, FILE_NAME_LEN,
			  pFileName, strlen(pFileName) + 1);
	}

	//TODO : szDstPort无实际作用，传一个固定值；打包协议类型szDstPP入参结构体未定义
	m_stUrlMediaInfo.ServiceType = ServiceType;
	//std::string strCameraDomainCode = m_szCameraID;
	std::string strCameraCode;
	std::string strDomainCode;
	iRet = CToolsHelp::SplitCameraDomainCode(m_szCameraID, strCameraCode, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Do Get Url DownLoad", "SplitCameraCode failed, RetCode=%d.", iRet);
		return iRet;
	}


	if(NULL != pNVRCode)
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szNVRCode, sizeof(m_stUrlMediaInfo.szNVRCode),pNVRCode, strlen(pNVRCode));//lint !e534
	}	
	if(NULL != pDomainCode)
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  pDomainCode, strlen(pDomainCode));//lint !e534
	}
	else
	{
		(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.cDomainCode,IVS_DOMAIN_CODE_LEN+1,  strDomainCode.c_str(), strDomainCode.size());//lint !e534
	}
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szSrcCode, sizeof(m_stUrlMediaInfo.szSrcCode), strCameraCode.c_str(), strCameraCode.size());//lint !e534
    (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstCode,sizeof(m_stUrlMediaInfo.szDstCode), "01",strlen("01"));//lint !e534 
	m_stUrlMediaInfo.MediaPara.StreamType = m_stuMediaParam.StreamType;
	m_stUrlMediaInfo.MediaPara.ProtocolType = m_stuMediaParam.ProtocolType;
	m_stUrlMediaInfo.PackProtocolType = PACK_PROTOCOL_ES;
	m_stUrlMediaInfo.MediaPara.BroadCastType = m_stuMediaParam.BroadCastType;
    (void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstIP,sizeof(m_stUrlMediaInfo.szDstIP), "",strlen(""));//lint !e534
	(void)CToolsHelp::Strncpy(m_stUrlMediaInfo.szDstPort,sizeof(m_stUrlMediaInfo.szDstPort), "35016",strlen("35016"));//lint !e534
	m_stUrlMediaInfo.MediaPara.VideoDecType = m_stuMediaParam.VideoDecType;
	m_stUrlMediaInfo.MediaPara.AudioDecType = m_stuMediaParam.AudioDecType;
	m_stUrlMediaInfo.MediaPara.TransMode = m_stuMediaParam.TransMode;



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
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "platform play back get URL failed!");
		return iRet;
	}

	//解析URL,得到ServiceType，如果是组播实况（直连），那么协议类型转为udp
	std::string strServiceType;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_SERVICE_TYPE_FLAG, strServiceType);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse service type from url failed, URL=%s",
			m_stMediaRsp.szURL);
		return iRet;
	}
	int iServiceType = CToolsHelp::StrToInt(strServiceType);

	std::string strMediaTransMode;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_MEDIA_TRANS_MODE, strMediaTransMode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse media trans mode failed from url");
		return iRet;
	}
	int iMediaTransMode = CToolsHelp::StrToInt(strMediaTransMode);

	std::string strBroadcastType;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_BROADCAST_TYPE, strBroadcastType);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse broadcast type failed from url");
		return iRet;
	}
	int iBroadcastType = CToolsHelp::StrToInt(strBroadcastType);

	std::string strTimeSpan;
	iRet = ParseURL(m_stMediaRsp.szURL, URL_TIME_SPAN, strTimeSpan);
	if (IVS_SUCCEED != iRet || strTimeSpan.empty())
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse timespan failed from url");
		return iRet;
	}

	unsigned int uiPos = strTimeSpan.find("-");
	if (std::string::npos == uiPos || uiPos >= strTimeSpan.length())
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse timespan failed from url,find spliter failed.");
		return iRet;
	}
	std::string strStartTime = strTimeSpan.substr(0, uiPos);
	if (std::string::npos == (uiPos+1) || (uiPos+1) >= strTimeSpan.length())
	{
		BP_RUN_LOG_ERR(iRet,"Do Get Url DownLoad", "parse timespan failed from url,find spliter failed.");
		return iRet;
	}
	strEndTime = strTimeSpan.substr(uiPos+1, strlen(IVS_DATE_TIME_NET_FORMAT));

	m_stTimeSpanInter.iStartTime = (IVS_INT32)UTCStr2Time(strStartTime.c_str(), IVS_DATE_TIME_NET_FORMAT);
	m_stTimeSpanInter.iEndTime = (IVS_INT32)UTCStr2Time(strEndTime.c_str(), IVS_DATE_TIME_NET_FORMAT);

	BP_RUN_LOG_INF("Do Get Url DownLoad", "strStartTime=%s strEndTime=%s iStartTime=%d iEndTime=%d.", strStartTime.c_str(), strEndTime.c_str(), m_stTimeSpanInter.iStartTime, m_stTimeSpanInter.iEndTime);

	m_enServiceType = (IVS_SERVICE_TYPE)iServiceType;
	m_stuMediaParam.TransMode = (IVS_MEDIA_TRANS_MODE)iMediaTransMode;
	m_stuMediaParam.BroadCastType = (IVS_BROADCAST_TYPE)iBroadcastType;

	return IVS_SUCCEED;
}
