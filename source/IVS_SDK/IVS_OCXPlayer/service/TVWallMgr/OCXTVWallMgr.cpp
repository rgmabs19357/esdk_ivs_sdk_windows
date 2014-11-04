#include "OCXTVWallMgr.h"
#include "IVS_SDKTVWall.h"
#include "bp_log.h"
#include "OCXTVWallXMLProcess.h"
#include "SDKDef.h"
#include "IVSCommon.h"
#include "OCXXmlProcess.h"

#define TVWALL_DECODER_MAX_COUNT 256 //解码器最大个数

COCXTVWallMgr::COCXTVWallMgr(void)
{
}


COCXTVWallMgr::~COCXTVWallMgr(void)
{
}

IVS_INT32 COCXTVWallMgr::InitTVWall(IVS_INT32 iSessionID, PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{
	IVS_INT32 iRet = IVS_SDK_InitTVWall(iSessionID, pFunNotifyCallBack, pUserData);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Init TVWall", "Failed");
	}
	return iRet;
}

IVS_INT32 COCXTVWallMgr::ReleaseTVWall(IVS_INT32 iSessionID)
{
	return IVS_SDK_CleanupTVWall(iSessionID);

}

IVS_VOID COCXTVWallMgr::GetTVWallList(CString &strResult)
{
	COCXTVWallXMLProcess::GetInst().GetTVWallList(strResult);
}

IVS_VOID COCXTVWallMgr::GetTVWallByID(IVS_UINT32 uiTVWallID, CString &strResult)
{
	COCXTVWallXMLProcess::GetInst().GetTVWallByID(uiTVWallID, strResult);
}

CString COCXTVWallMgr::AddTVWall(const CString& strTVWall)
{
	CString strResult = COCXTVWallXMLProcess::GetInst().AddTVWall(strTVWall);
	return strResult;
}

IVS_INT32 COCXTVWallMgr::DeleteTVWall(IVS_ULONG uiTVWallID)
{
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().DeleteTVWall(uiTVWallID);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::ModifyTVWallInfo(const CString& strResult)
{
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().ModifyTVWallInfo(strResult);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::ModifyTVWallLayout(const CString& strResult)
{
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().ModifyTVWallLayout(strResult);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::StartRealPlayTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, const CString& strRealplayParam, const TVWall_VPANEID& vPaneID)
{
	IVS_REALPLAY_PARAM stRealPlayParam = {0};
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start RealPlay", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
    iRet = COCXTVWallXMLProcess::GetInst().RealPlayParamParseXML(strRealplayParam, stRealPlayParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start RealPlay", "Parse RealPlayParamXML failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_StartRealPlayTVWall(iSessionID, strCameraCode, &stRealPlayParam, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::StopRealPlayTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Stop RealPlay", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_StopRealPlayTVWall(iSessionID, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::StartPlatformPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
                                                    const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID)
{
    IVS_PLAYBACK_PARAM stPlaybackParam = {0};
    IVS_TVWALL_PARAM stTVWallParam = {0};
    IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "TVWall Start RePlay", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
        return iRet;
    }
    iRet = COCXTVWallXMLProcess::GetInst().PlatformPlaybackParamParseXML(pPlaybackParam, stPlaybackParam);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "TVWall Start RePlay", "Parse RealPlayParamXML failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
        return iRet;
    }
    iRet = IVS_SDK_StartPlatformPlayBackTVWall(iSessionID, strCameraCode, &stPlaybackParam, &stTVWallParam);
    return iRet;
}

IVS_INT32 COCXTVWallMgr::StopPlatformPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID)
{
    IVS_TVWALL_PARAM stTVWallParam = {0};
    IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "TVWall Stop RePlay", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
        return iRet;
    }
    iRet = IVS_SDK_StopPlatformPlayBackTVWall(iSessionID, &stTVWallParam);
    return iRet;
}

IVS_INT32 COCXTVWallMgr::StartPUPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
										const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID)
{
	IVS_PLAYBACK_PARAM stPlaybackParam = {0};
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start PU Playback", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = COCXTVWallXMLProcess::GetInst().PlatformPlaybackParamParseXML(pPlaybackParam, stPlaybackParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start PU Playback", "Parse RealPlayParamXML failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_StartPUPlayBackTVWall(iSessionID, strCameraCode, &stPlaybackParam, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::StopPUPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Stop PU Playback", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_StopPUPlayBackTVWall(iSessionID, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::StartBackupPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strDomainCode, 
													const CString& strNVRCode, const CString& strCameraCode, 
													const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID)
{
	IVS_PLAYBACK_PARAM stPlaybackParam = {0};
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start Backup Playback", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = COCXTVWallXMLProcess::GetInst().PlatformPlaybackParamParseXML(pPlaybackParam, stPlaybackParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start Backup Playback", "Parse RealPlayParamXML failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_StartBackupPlayBackTVWall(iSessionID, strDomainCode, strNVRCode, strCameraCode, &stPlaybackParam, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::StopBackupPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Stop Backup Playback", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_StopBackupPlayBackTVWall(iSessionID, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::PlayBackPauseTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Pause Playback", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_PlayBackPauseTVWall(iSessionID, &stTVWallParam);
	return iRet;
}
IVS_INT32 COCXTVWallMgr::PlayBackResumeTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Resume Playback", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_PlayBackResumeTVWall(iSessionID, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::PlaybackFrameStepForwardTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Frame Step", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", vPaneID.ulParentWndID,vPaneID.ulChildWndID);
		return iRet;
	}
	iRet = IVS_SDK_PlaybackFrameStepForwardTVWall(iSessionID, &stTVWallParam);
	return iRet;
}

IVS_INT32 COCXTVWallMgr::GetTVWallWndIDByDecoderID(const IVS_ULONG& ulTVWallID,const CString& cstrDecoderID,std::vector<IVS_ULONG>& ulWndIDVec)
{
	IVS_UINT32 uX = 0;
	IVS_UINT32 uY = 0;
	TVWALL_PANE_LIST paneList;
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetTVWallPaneList(ulTVWallID, paneList, uX, uY);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get TVWallWndID By DecoderID Failed.", "Reason:XML Process Failed.");
		return iRet;
	}
	iRet = IVS_PARA_INVALID;//先置 没找到时，返回参数错误
	TVWALL_PANE_LIST::iterator iter = paneList.begin();
	for(;iter != paneList.end();iter++)
	{
		CString cstrCode(iter->szDecoderID);
		if(cstrDecoderID == cstrCode)
		{
			ulWndIDVec.push_back(iter->ulWndID);
			iRet = IVS_SUCCEED;//成功找到
		}
	}
	return iRet;
}

IVS_INT32 COCXTVWallMgr::GetTVWallWndIDByDecoderIDChannel(const IVS_ULONG& ulTVWallID,const CUMW_NOTIFY_REMOTE_DECODER_STATUS& decoder,TVWall_VPANEID_LIST& VPaneIDList)
{
	//ID为0表示不是电视墙
	if(ulTVWallID == 0)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get TVWallWndID By DecoderID and Channel Failed.", "Reason:TVWallID equal 0.");
		return IVS_PARA_INVALID;
	}

	IVS_UINT32 uX = 0;
	IVS_UINT32 uY = 0;
	TVWALL_PANE_LIST paneList;
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetTVWallPaneList(ulTVWallID, paneList, uX, uY);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get TVWallWndID By DecoderID and Channel Failed.", "Reason:XML Process Failed.");
		return iRet;
	}
	iRet = IVS_PARA_INVALID;//先置 没找到时，返回参数错误
	TVWALL_PANE_LIST::iterator iter = paneList.begin();
	for(;iter != paneList.end();iter++)
	{
		if(0 != strcmp(iter->szDecoderID,decoder.szDecoderID))
		{
			continue;		
		}
		for(IVS_UINT32 i=0;i<decoder.ulChannelNum && i<CUMW_DECODER_CHANNEL_MAX_NUM;i++)
		{
			ULONG j = 1;
			for(TVWALL_PANE_CHANNEL_LIST::iterator it=iter->listChanel.begin();it!=iter->listChanel.end();it++)
			{
				if(decoder.stChannelInfoArray[i].ulChannelID == it->ulChannelID)
				{
					TVWall_VPANEID vPaneID = {0};
					vPaneID.ulParentWndID = iter->ulWndID;
					vPaneID.ulChildWndID = j;
					VPaneIDList.push_back(vPaneID);
					iRet = IVS_SUCCEED;//成功找到
				}
				j++;
			}
		}	
	}
	return iRet;
}

IVS_INT32 COCXTVWallMgr::GetTVWallWndIDByDecoderIDChannel(const IVS_ULONG& ulTVWallID,const CString& strDecoderID,IVS_UINT32 uiChannel,TVWall_VPANEID& stPaneID)
{
	IVS_UINT32 uiX = 0;
	IVS_UINT32 uiY = 0;
	TVWALL_PANE_LIST paneList;
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetTVWallPaneList(ulTVWallID, paneList, uiX, uiY);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get WndID By DecoderID and Channel", "Failed.");
		return iRet;
	}
	iRet = IVS_PARA_INVALID;//先置 没找到时，返回参数错误
	TVWALL_PANE_LIST::iterator iter = paneList.begin();
	for(;iter != paneList.end();iter++)
	{
		if(0 != strcmp(iter->szDecoderID,strDecoderID))
		{
			continue;		
		}
		ULONG index=0;
		for(TVWALL_PANE_CHANNEL_LIST::iterator it=iter->listChanel.begin();it!=iter->listChanel.end();it++)
		{
			index++;
			if(uiChannel == it->ulChannelID)
			{
				stPaneID.ulParentWndID = iter->ulWndID;
				stPaneID.ulChildWndID = index;
				return IVS_SUCCEED;//成功找到
			}
		}
	}
	return iRet;
}

IVS_VOID COCXTVWallMgr::GetDecoderList(IVS_INT32 iSessionID, CString &strResult)
{
	IVS_UINT32 uiBufLen = TVWALL_DECODER_MAX_COUNT*sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)+sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST);
	IVS_CHAR* pBuf = IVS_NEW(pBuf, uiBufLen);
	if(NULL!=pBuf)
	{
		memset(pBuf, 0, uiBufLen);
		IVS_INDEX_RANGE stIndex = {0};
		stIndex.uiFromIndex = 1;
		stIndex.uiToIndex = TVWALL_DECODER_MAX_COUNT;
		CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList = reinterpret_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST*>(pBuf);//lint !e826
		IVS_INT32 iRet = IVS_SDK_GetDecoderList(iSessionID, &stIndex, pDecoderList, uiBufLen);
		if(IVS_SUCCEED!=iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get Decoder List", "Failed");
			COCXXmlProcess::GetErrString(strResult, iRet);
		}
		else
		{
			if(pDecoderList->stIndexRange.uiToIndex>=pDecoderList->stIndexRange.uiFromIndex)
			{
				IVS_UINT32 uiNum = (pDecoderList->stIndexRange.uiToIndex-pDecoderList->stIndexRange.uiFromIndex)+1;
				if(0==pDecoderList->uiTotal)
				{
					uiNum = 0;
				}
				iRet = COCXTVWallXMLProcess::GetInst().GetDecoderInfoList(pDecoderList->stDecoderList, uiNum, strResult);
				if(IVS_SUCCEED!=iRet)
				{
					BP_RUN_LOG_ERR(iRet, "Get Decoder List", "Failed");
					COCXXmlProcess::GetErrString(strResult, iRet);
				}
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Decoder List", "Failed, Index Invalid");
				COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
			}
		}
		IVS_DELETE(pBuf, MUILI);
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder List", "Alloc Memory Error");
		COCXXmlProcess::GetErrString(strResult, IVS_ALLOC_MEMORY_ERROR);
	}
}
IVS_INT32 COCXTVWallMgr::GetOnlineDecoderIDList(IVS_INT32 iSessionID,std::list<CUMW_NOTIFY_REMOTE_DECODER_STATUS>& decoderIDList)
{
	IVS_UINT32 uiBufLen = TVWALL_DECODER_MAX_COUNT*sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)+sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST);
	IVS_CHAR* pBuf = IVS_NEW(pBuf, uiBufLen);
	if(NULL == pBuf)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder List", "Alloc Memory Error");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pBuf, 0, uiBufLen);
	IVS_INDEX_RANGE stIndex = {0};
	stIndex.uiFromIndex = 1;
	stIndex.uiToIndex = TVWALL_DECODER_MAX_COUNT;
	CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList = reinterpret_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST*>(pBuf);//lint !e826
	IVS_INT32 iRet = IVS_SDK_GetDecoderList(iSessionID, &stIndex, pDecoderList, uiBufLen);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Decoder List Failed.", "Reason:SDK Get Decoder List Failed.");
		IVS_DELETE(pBuf, MUILI);
		return iRet;
	}
	if(pDecoderList->stIndexRange.uiToIndex < pDecoderList->stIndexRange.uiFromIndex)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Decoder List Failed.", "Reason:SDK Get Decoder List Index Invalid");
		IVS_DELETE(pBuf, MUILI);
		return IVS_PARA_INVALID;
	}
	IVS_UINT32 uiNum = (pDecoderList->stIndexRange.uiToIndex-pDecoderList->stIndexRange.uiFromIndex)+1;
	if(0==pDecoderList->uiTotal)
	{
		uiNum = 0;
	}
	for(IVS_UINT32 i=0;i<uiNum;i++)
	{
		CUMW_NOTIFY_REMOTE_DECODER_STATUS strudecoder;
		memset(&strudecoder,0,sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS));
		if(!CToolsHelp::Memcpy(&strudecoder,sizeof(strudecoder),&pDecoderList->stDecoderList[i],sizeof(strudecoder)))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy DECODER failed", "NA");
			IVS_DELETE(pBuf, MUILI);
			return IVS_ALLOC_MEMORY_ERROR;
		}
		decoderIDList.push_back(strudecoder);
	}
	IVS_DELETE(pBuf, MUILI);
	return IVS_SUCCEED;
}

// 开启声音
IVS_INT32 COCXTVWallMgr::PlaySoundTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, stPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Play Sound", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", stPaneID.ulParentWndID,stPaneID.ulChildWndID);
		return iRet;
	}
	return iRet;//IVS_SDK_PlaySoundTVWall(iSessionID, &stTVWallParam);
}//lint !e715
// 停止声音
IVS_INT32 COCXTVWallMgr::StopSoundTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, stPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Play Sound", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", stPaneID.ulParentWndID,stPaneID.ulChildWndID);
		return iRet;
	}
	return iRet;//IVS_SDK_StopSoundTVWall(iSessionID, &stTVWallParam);
}//lint !e715
// 设置音量
IVS_INT32 COCXTVWallMgr::SetVolumeTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID, IVS_UINT32 uiVolumeValue)
{
	IVS_TVWALL_PARAM stTVWallParam = {0};
	IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, stPaneID, stTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Play Sound", "Get DecorderID and Channel failed, WndID=[%lu,%lu]", stPaneID.ulParentWndID,stPaneID.ulChildWndID);
		return iRet;
	}
	return iRet;//IVS_SDK_SetVolumeTVWall(iSessionID, &stTVWallParam, uiVolumeValue);
}//lint !e715
