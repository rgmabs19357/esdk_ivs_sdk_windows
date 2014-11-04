#include "stdafx.h"
#include "OCXPlayBackXMLProcess.h"
#include "ToolsHelp.h"
#include "SDKDef.h"
#include "IVS_Trace.h"

COCXPlayBackXMLProcess::COCXPlayBackXMLProcess()
{
}

COCXPlayBackXMLProcess::~COCXPlayBackXMLProcess()
{
}

//获取媒体参数XML
LONG COCXPlayBackXMLProcess::GetMediaParamXML(LPCTSTR pMediaParamXml, IVS_MEDIA_PARA &mediaParam)
{
	if (NULL == pMediaParamXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Media Param", "get media param error, xml is null");//lint !e40
		return IVS_PARA_INVALID;
	}

	IVS_DEBUG_TRACE("");
	//解析xml
	CXml xml;
	if (!xml.Parse(pMediaParamXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Media Param","parse xml error.");	//lint !e40
		return IVS_XML_INVALID;
	}

	if(xml.FindElem("CV_CONTENT"))
	{
		(void)xml.IntoElem();
		if(xml.FindElem("MEDIA_PARA"))
		{				
			(void)xml.IntoElem();
			IVS_INT32 iTemp = 0;
			const IVS_CHAR* szElemValue = NULL;

			GET_ELEM_VALUE_NUM("DECODER_COMPANY", szElemValue, iTemp, xml);
			mediaParam.DecoderCompany = (IVS_VENDOR_TYPE)iTemp;
			GET_ELEM_VALUE_NUM("STREAM_TYPE", szElemValue, iTemp, xml);
			mediaParam.StreamType = (IVS_STREAM_TYPE)iTemp;
			GET_ELEM_VALUE_NUM("PROTOCOL_TYPE", szElemValue, iTemp, xml);
			mediaParam.ProtocolType = (IVS_PROTOCOL_TYPE)iTemp;
			GET_ELEM_VALUE_NUM("DIRECT_FIRST", szElemValue, iTemp, xml);
			mediaParam.TransMode = (IVS_MEDIA_TRANS_MODE)iTemp;
			GET_ELEM_VALUE_NUM("MULTICAST", szElemValue, iTemp, xml);
			mediaParam.BroadCastType = (IVS_BROADCAST_TYPE)iTemp;
			GET_ELEM_VALUE_NUM_FOR_UINT("VIDEO_ENCODE_TYPE", szElemValue, mediaParam.VideoDecType, xml);
			GET_ELEM_VALUE_NUM("AUDIO_ENCODE_TYPE", szElemValue, iTemp, xml);
			mediaParam.AudioDecType = (IVS_AUDIO_DEC_TYPE)iTemp;
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Media Param","Parse xml media para fail.");//lint !e40
			return IVS_XML_INVALID;
		}
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Media Param","Parse xml Content fail.");//lint !e40
		return IVS_XML_INVALID;
	}

	return IVS_SUCCEED;
}//lint !e1762

	

//查询媒体播放信息上报XML;
void COCXPlayBackXMLProcess::GetPlayInfoEventMsgXML(CString &xmlToCU, ULONG ulWinID,IVS_MEDIA_INFO &mediaInfo, CPoint& XY,IVS_INT32 resultcode)
{
	xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

	char str[32];
	xmlToCU += "<Content>";
	xmlToCU += "<ResultCode>";
	(void)itoa(resultcode, str, 10);
	xmlToCU += str;
	xmlToCU += "</ResultCode>";
	if (IVS_SUCCEED == resultcode)
	{
		xmlToCU += "<MeidaInfo>";
		xmlToCU += "<WndID>";
		(void)itoa((int)ulWinID, str, 10);
		xmlToCU += str;
		xmlToCU += "</WndID>";
		xmlToCU += "<AudioStatus>";
		(void)itoa((int)mediaInfo.uiAudioStatus, str, 10);
		xmlToCU += str;
		xmlToCU += "</AudioStatus>";

		xmlToCU += "<FPS>";
		(void)itoa((int)mediaInfo.uiVideoFrameRate, str, 10);
		xmlToCU += str;
		xmlToCU += "</FPS>";

		xmlToCU += "<VideoCodeRate>";
		(void)itoa((int)mediaInfo.uiVideoBitrate, str, 10);
		xmlToCU += str;
		xmlToCU += "</VideoCodeRate>";

		xmlToCU += "<VideoCodeRateAverage>";
		(void)itoa((int)mediaInfo.uiVideoAverageBitrate, str, 10);
		xmlToCU += str;
		xmlToCU += "</VideoCodeRateAverage>";

		xmlToCU += "<AudioCodeRate>";
		(void)itoa((int)mediaInfo.uiAudioBitRate, str, 10);
		xmlToCU += str;
		xmlToCU += "</AudioCodeRate>";

		xmlToCU += "<AudioCodeRateAverage>";
		(void)itoa((int)mediaInfo.uiAudioAverageBitRate, str, 10);
		xmlToCU += str;
		xmlToCU += "</AudioCodeRateAverage>";

		xmlToCU += "<VideoFormat>";
		switch(mediaInfo.uiVideoCodec)
		{
		case 1:
			xmlToCU +="H264"; 
			break;
		case 2:
			xmlToCU +="MJPEG";
			break;
		case 3:
			xmlToCU +="MPEG4";
			break;
		case 4:
			xmlToCU +="MPEG2";
			break;
		case 5:
			xmlToCU +="AVS"; 
			break;
		default:
			//默认H264
			xmlToCU +="H264";
			break;
		}
		xmlToCU += "</VideoFormat>";

		xmlToCU += "<ResolutionWidth>";
		(void)itoa((int)mediaInfo.uiVideoWidth, str, 10);
		xmlToCU += str;
		xmlToCU += "</ResolutionWidth>";

		xmlToCU += "<ResolutionHeight>";
		(void)itoa((int)mediaInfo.uiVideoHeight, str, 10);
		xmlToCU += str;
		xmlToCU += "</ResolutionHeight>";

		xmlToCU += "<LossTolerance>";
		char lossToBuf[64];
		memset(lossToBuf,0,64);
		(void)_snprintf_s(lossToBuf, 64, "%f",mediaInfo.fLossTolerance);
		xmlToCU += lossToBuf;
		xmlToCU += "</LossTolerance>";

		xmlToCU += "<X>";
		(void)itoa((int)XY.x, str, 10);
		xmlToCU += str;
		xmlToCU += "</X>";

		xmlToCU += "<Y>";
		(void)itoa((int)XY.y, str, 10);
		xmlToCU += str;
		xmlToCU += "</Y>";

		xmlToCU += "<MediaSourceIP>";
		xmlToCU += mediaInfo.stMediaSourceIP.cIP;
		xmlToCU += "</MediaSourceIP>";
		xmlToCU += "</MeidaInfo>";
	}
	else
	{
		xmlToCU += "<X>";
		(void)itoa((int)XY.x, str, 10);
		xmlToCU += str;
		xmlToCU += "</X>";

		xmlToCU += "<Y>";
		(void)itoa((int)XY.y, str, 10);
		xmlToCU += str;
		xmlToCU += "</Y>";
	}
	xmlToCU +="</Content>";
}
//查询窗口ID的XML;
void COCXPlayBackXMLProcess::GetWndIDXML(CString &xmlToCU,ULONG ulWndID)
{
	xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

	char str[32];
	xmlToCU += "<Content>";
	xmlToCU += "<WndID>";
	(void)_snprintf_s(str, 32, "%d",ulWndID);
	xmlToCU += str;
	xmlToCU += "</WndID>";
	xmlToCU +="</Content>";
}
//查询回放时间
IVS_INT32 COCXPlayBackXMLProcess::GetPlayBackTime(const IVS_PLAYBACK_TIME& stPlaybackTime, IVS_INT32 iResultCode, CXml& xmlTime)
{
	//IVS_DEBUG_TRACE("");
    IVS_CHAR cPlayBackTime[IVS_TIME_LEN+1];
    memset(cPlayBackTime, 0, IVS_TIME_LEN+1);
    bool bRet = CToolsHelp::Memcpy(cPlayBackTime,IVS_TIME_LEN+1, stPlaybackTime.cPlayBackTime, IVS_TIME_LEN);
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Play Back Time", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    IVS_CHAR cPlayBackStartTime[IVS_TIME_LEN+1];
    memset(cPlayBackStartTime, 0, IVS_TIME_LEN+1);
    bRet = CToolsHelp::Memcpy(cPlayBackStartTime,IVS_TIME_LEN+1, stPlaybackTime.cPlayBackStartTime, IVS_TIME_LEN);
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Play Back Time", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    IVS_CHAR cPlayBackEndTime[IVS_TIME_LEN+1];
    memset(cPlayBackEndTime, 0, IVS_TIME_LEN+1);
    bRet = CToolsHelp::Memcpy(cPlayBackEndTime,IVS_TIME_LEN+1, stPlaybackTime.cPlayBackEndTime, IVS_TIME_LEN);
	if(false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Play Back Time", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    (void)xmlTime.AddDeclaration("1.0","UTF-8","");
    (void)xmlTime.AddElem("Content");
    (void)xmlTime.AddChildElem("ResultCode");
    (void)xmlTime.IntoElem();
    (void)xmlTime.SetElemValue(CToolsHelp::Int2Str(iResultCode).c_str());
    (void)xmlTime.AddElem("PlayBackTime");
    (void)xmlTime.SetElemValue(cPlayBackTime);
    (void)xmlTime.AddElem("StartTime");
    (void)xmlTime.SetElemValue(cPlayBackStartTime);
    (void)xmlTime.AddElem("EndTime");
    (void)xmlTime.SetElemValue(cPlayBackEndTime);
	
	return IVS_SUCCEED;
}

IVS_INT32 COCXPlayBackXMLProcess::GetPlayBackSpeed(float fSpeed, IVS_INT32 iResultCode, CXml& xmlSpeed)
{
	(void)xmlSpeed.AddDeclaration("1.0","UTF-8","");
	(void)xmlSpeed.AddElem("Content");
	(void)xmlSpeed.AddChildElem("ResultCode");
	(void)xmlSpeed.IntoElem();
	(void)xmlSpeed.SetElemValue(CToolsHelp::Int2Str(iResultCode).c_str());
	(void)xmlSpeed.AddElem("Speed");

	IVS_CHAR szSpeed[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(szSpeed, STR_LEN, "%f", fSpeed))
	{
		BP_RUN_LOG_ERR(IVS_XML_PACK_ERROR,"Get Play Back Speed", "Speed value[%f] error.", fSpeed);
		return IVS_XML_PACK_ERROR;
	}

	(void)xmlSpeed.SetElemValue(szSpeed);

	return IVS_SUCCEED;
}

IVS_INT32 COCXPlayBackXMLProcess::GetPlayBackStatus(IVS_INT32 iStatus, IVS_INT32 iResultCode, CXml& xmlStatus)
{
	(void)xmlStatus.AddDeclaration("1.0","UTF-8","");
	(void)xmlStatus.AddElem("Content");
	(void)xmlStatus.AddChildElem("ResultCode");
	(void)xmlStatus.IntoElem();
	(void)xmlStatus.SetElemValue(CToolsHelp::Int2Str(iResultCode).c_str());
	(void)xmlStatus.AddElem("Status");

	IVS_CHAR szStatus[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(szStatus, STR_LEN, "%d", iStatus))
	{
		BP_RUN_LOG_ERR(IVS_XML_PACK_ERROR,"Get Play Back Status", "Status value[%d] error.", iStatus);
		return IVS_XML_PACK_ERROR;
	}

	(void)xmlStatus.SetElemValue(szStatus);
	return IVS_SUCCEED;
}

//获得码流异常上报XML
void COCXPlayBackXMLProcess::GetStreamExceptionMsgXML(CString &xmlToCU, ULONG ulWinID, int iExceptionCode, int iReseved1)
{
    xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

    char str[TEMP_BUFFER_SIZE];
    xmlToCU += "<Content>";
    xmlToCU += "<WndID>";
    (void)itoa((int)ulWinID, str, 10);
    xmlToCU += str;
    xmlToCU += "</WndID>";
    xmlToCU += "<ExceptionCode>";
    (void)itoa((int)iExceptionCode, str, 10);
    xmlToCU += str;
    xmlToCU += "</ExceptionCode>";
    xmlToCU += "<Reserved>";
    (void)itoa((int)iReseved1, str, 10);
    xmlToCU += str;
    xmlToCU += "</Reserved>";
    xmlToCU +="</Content>";
}
//获得回放参数
IVS_INT32 COCXPlayBackXMLProcess::GetPlaybackParam(const char* pPlayback, IVS_PLAYBACK_PARAM& stPlaybackParam)
{
    if (NULL == pPlayback)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Play Back Param", "pPlayback is null");//lint !e40
        return IVS_PARA_INVALID;
    }

	IVS_DEBUG_TRACE("");
    //解析xml
    CXml xml;
    if (!xml.Parse(pPlayback))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Play Back Param","parse xml error.");	//lint !e40
        return IVS_XML_INVALID;
    }

    if(xml.FindElem("Content"))
    {
        (void)xml.IntoElem();
        if(xml.FindElem("PlaybackParam"))
        {				
            (void)xml.IntoElem();
            IVS_INT32 iTemp = 0;
			IVS_FLOAT SpeedTmp = 1.0;

            const IVS_CHAR* szElemValue = NULL;

            GET_ELEM_VALUE_NUM("ProtocolType", szElemValue, iTemp, xml);
            stPlaybackParam.uiProtocolType = (IVS_UINT32)iTemp;
            GET_ELEM_VALUE_CHAR("StartTime", szElemValue, stPlaybackParam.stTimeSpan.cStart, IVS_TIME_LEN, xml);
            GET_ELEM_VALUE_CHAR("EndTime", szElemValue, stPlaybackParam.stTimeSpan.cEnd, IVS_TIME_LEN, xml);
            GET_ELEM_VALUE_FLOAT("Speed", szElemValue, SpeedTmp, xml);
            stPlaybackParam.fSpeed = SpeedTmp;
        }
        else
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Play Back Param","Parse xml playback para fail.");//lint !e40
            return IVS_XML_INVALID;
        }
    }
    else
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Play Back Param","Parse xml Content fail.");//lint !e40
        return IVS_XML_INVALID;
    }

    return IVS_SUCCEED;
}

/*
 * MakeTimeFromStr
 * UTC: yyyymmddhhmmss 转换为时间戳
 */
static time_t MakeTimeFromStr(const char *date)
{
	if (NULL == date)
	{
		return (time_t)-1;
	}

	/* UTC = local time + bias */

	TIME_ZONE_INFORMATION tzi = {0};
	GetTimeZoneInformation(&tzi);
	long tz = -tzi.Bias / 60;     

    struct tm _tm;
	memset(&_tm, 0, sizeof(_tm));

	char tmp[8] = {0};
	memcpy(tmp, date + 4, 2);
	_tm.tm_mon = atoi(tmp) - 1;

	memcpy(tmp, date + 6, 2);
	_tm.tm_mday =  atoi(tmp);

	memcpy(tmp, date + 8, 2);
	_tm.tm_hour =  atoi(tmp) + tz;

	memcpy(tmp, date + 10, 2);
	_tm.tm_min  =  atoi(tmp);

	memcpy(tmp, date + 12, 2);
	_tm.tm_sec = atoi(tmp);

	memcpy(tmp, date, 4);
	_tm.tm_year = atoi(tmp) - 1900;
	
	_tm.tm_isdst = 0;
	return mktime(&_tm);
}

/*
 * ParsePlayBackSync
 */
IVS_INT32 COCXPlayBackXMLProcess::ParsePlayBackSync(const char *szReqXml, SyncPlayInfo &syncInfo)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(szReqXml, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	if (!xml.Parse(szReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, 
			  "Parse Sync Playback Xml Fail", "xml=%s", szReqXml);
		return IVS_XML_INVALID;
	}

	if (!xml.FindElemEx("Content/SyncPlayInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,
              "Parse Sync Playback Xml Fail", "No 'SyncPlayInfo' Node");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	//bool  bSuccess = true;
	const char *value = NULL;	
	
	if (xml.FindElem("ProtocolType"))
	{
		value = xml.GetElemValue();
		CHECK_POINTER(value, IVS_XML_INVALID);
	    syncInfo.m_uProto = (unsigned int)atoi(value);
	}

	if (!xml.FindElem("PlayList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,
			"Parse Sync Playback Xml Fail", "No 'PlayList' Node");
		return IVS_XML_INVALID;	
    }
	xml.IntoElem();
    
	if (!xml.FindElem("PlayInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,
			"Parse Sync Playback Xml Fail", "No 'PlayInfo' Node");
		return IVS_XML_INVALID;	
	}

	do 
	{
		CPlayInfo playInfo;
		playInfo.ulWndID = (unsigned long)(-1);
		memset(playInfo.CameraCode, 0, sizeof(playInfo.CameraCode));
		
		xml.IntoElem();
		if (xml.FindElem("WndID") 
			&& NULL != (value = xml.GetElemValue()))
		{
			playInfo.ulWndID = (unsigned long)atoi(value);
		}

		if (xml.FindElem("CameraCode")&& 
			NULL != (value = xml.GetElemValue()))
		{
			strncpy(playInfo.CameraCode, value, sizeof(playInfo.CameraCode) - 1);
		}
		
		if (!xml.FindElem("TimeSpanList"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID,
				"Parse Sync Playback Xml Fail", "No 'TimeSpanList' Node");
			return IVS_XML_INVALID;
		}

		xml.IntoElem();
		TIME_INFO tmi = {0};

		if (!xml.FindElem("TimeInfo"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID,
				"Parse Sync Playback Xml Fail", "No 'TimeInfo' Node");
			return IVS_XML_INVALID;
		}

		do {
			xml.IntoElem();
			if (xml.FindElem("StartTime") &&
				NULL != (value = xml.GetElemValue()))
			{
				tmi.beg = MakeTimeFromStr(value);
			}

			if (xml.FindElem("EndTime") && 
				NULL != (value = xml.GetElemValue()))
			{
				tmi.end = MakeTimeFromStr(value);
			}

			playInfo.m_TimeList.push_back(tmi);
			xml.OutOfElem();
		} while (xml.NextElem());
	
		syncInfo.m_PlayInfoList.push_back(playInfo);
		
		xml.OutOfElem();
		xml.OutOfElem();
	
	} while (xml.NextElem());

	return IVS_SUCCEED;
}

/*
 * ParseLocalPlayBackParam
 * 解析本地录像参数
 */
IVS_INT32 COCXPlayBackXMLProcess::ParseLocalPlayBackParam(const char *szReqXml, IVS_LOCAL_PLAYBACK_PARAM &ParamInfo)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(szReqXml, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	if (!xml.Parse(szReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, 
			  "Parse LocalPlayBack Param", "xml=%s", szReqXml);
		return IVS_XML_INVALID;
	}

	if (!xml.FindElemEx("Content/PlaybackParam"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,
              "Parse Sync Playback Xml Fail", "No 'SyncPlayInfo' Node");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	const char *value = NULL;	
	if (xml.FindElem("StartTime") &&  NULL != (value = xml.GetElemValue()))
	{
	    ParamInfo.uiStartTime = (unsigned int)atoi(value);
	}

	if (xml.FindElem("EndTime") && NULL != (value = xml.GetElemValue()))
	{
		ParamInfo.uiEndTime = (unsigned int)atoi(value);
    }
    
	if (xml.FindElem("Speed") && NULL != (value = xml.GetElemValue()) )
	{
	    ParamInfo.fSpeed = (float)atof(value);
    }

#if 0
	if (xml.FindElem("pPWD") && NULL != (value = xml.GetElemValue()))
	{
		 
    }
#endif 
	return IVS_SUCCEED;
}

/*
 * ParseRecordSnapParam
 * 解析录像抓图参数
 */
IVS_INT32 COCXPlayBackXMLProcess::PraseRecordSnapParam(const char *szReqXml, CRecordSnap &SnapInfo)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(szReqXml, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	if (!xml.Parse(szReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, 
			"Parse RecordSnap Param", "xml=%s", szReqXml);
		return IVS_XML_INVALID;
	}

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,
			"Parse Sync Playback Xml Fail", "No 'Content' Node");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	const char *value = NULL;	
	if (xml.FindElem("RecordMethod") &&  NULL != (value = xml.GetElemValue()))
	{
		SnapInfo.RecordType = atoi(value);
	}

	if (xml.FindElem("CameraCode") && NULL != (value = xml.GetElemValue()))
	{
		(void)strncpy_s(SnapInfo.CamCode, value, sizeof(SnapInfo.CamCode) - 1);
    }

	if (xml.FindElem("NVRCode") && NULL != (value = xml.GetElemValue()))
	{
		(void)strncpy_s(SnapInfo.NvrCode, value, sizeof(SnapInfo.NvrCode) - 1);
	}

	if (xml.FindElem("MBUDomain") && NULL != (value = xml.GetElemValue()))
	{
		(void)strncpy_s(SnapInfo.MbuDomain, value, sizeof(SnapInfo.MbuDomain) - 1);
	}

	if (xml.FindElem("FileName") && NULL != (value = xml.GetElemValue()))
	{
		(void)strncpy_s(SnapInfo.FileName, value, sizeof(SnapInfo.FileName) - 1);
	}

	if (xml.FindElem("FilePath") && NULL != (value = xml.GetElemValue()))
	{
		(void)strncpy_s(SnapInfo.FilePath, value, sizeof(SnapInfo.FilePath) - 1);
	}

	if (xml.FindElem("PlaybackParam"))
	{
		xml.IntoElem();
		if (xml.FindElem("StartTime") && NULL != (value = xml.GetElemValue()))
		{
			(void)strncpy_s(SnapInfo.RecTimeSpan.cStart, value, sizeof(SnapInfo.RecTimeSpan.cStart) - 1);
			if (SnapInfo.RecordType != 100) // 100 对应本地录像
			{
				SnapInfo.begine = MakeTimeFromStr(value);
			}
		}

		if (xml.FindElem("EndTime") && NULL != (value = xml.GetElemValue()))
		{
			(void)strncpy_s(SnapInfo.RecTimeSpan.cEnd, value, sizeof(SnapInfo.RecTimeSpan.cEnd) - 1);
			if (SnapInfo.RecordType != 100) // 100 对应本地录像
			{
				SnapInfo.end = MakeTimeFromStr(value);
			}
		}
		xml.OutOfElem();
	}

	if (xml.FindElem("TimeList"))
	{
		xml.IntoElem();

		if (!xml.FindElem("TimeInfo"))
		{
			return IVS_FAIL;
		}

		do {
			time_t t = 0;
			if (NULL != (value = xml.GetElemValue()))
			{
				if (SnapInfo.RecordType != 100) // 100 对应本地录像
				{
					t = MakeTimeFromStr(value);
				}
				else 
				{
					t = atoi(value);
				}
			}
    		SnapInfo.SnapPoint.push_back(t);
		} while (xml.NextElem());
	}
	return IVS_SUCCEED;
}

/*
 * BuildRecordSnapXml
 * 构造缩略图抓拍xml
 */
#define CHECK_SUCCESS(s)\
	do { if(!(s)) return IVS_FAIL;} while(0)

IVS_INT32 COCXPlayBackXMLProcess::BuildRecordSnapXml(CXml &xml, IVS_INT32 result, SNAP_VEC &SnapVec)
{
	IVS_DEBUG_TRACE("");

	char value[128] = {0};
	CHECK_SUCCESS(xml.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xml.AddElem("Content"));

	CHECK_SUCCESS(xml.AddChildElem("ResultCode"));
	xml.IntoElem();

	itoa(result, value, 10);
	CHECK_SUCCESS(xml.SetElemValue(value));
    CHECK_SUCCESS(xml.AddElem("ImageList"));

	bool bFirst = true;
	for (unsigned int i = 0; i < SnapVec.size(); i++)
	{
		if (bFirst)
		{
			CHECK_SUCCESS(xml.AddChildElem("ImageInfo"));
			xml.IntoElem();

			CHECK_SUCCESS(xml.AddChildElem("Image"));
			xml.IntoElem();
			CHECK_SUCCESS(xml.SetElemValue(SnapVec[i].FileName));
			CHECK_SUCCESS(xml.AddElem("Time"));
			CHECK_SUCCESS(xml.SetElemValue(SnapVec[i].SnapTime));
			xml.OutOfElem();
			bFirst = false;
			continue;
		}

		CHECK_SUCCESS(xml.AddElem("ImageInfo"));
		CHECK_SUCCESS(xml.AddChildElem("Image"));
		xml.IntoElem();
		CHECK_SUCCESS(xml.SetElemValue(SnapVec[i].FileName));
		CHECK_SUCCESS(xml.AddElem("Time"));
		CHECK_SUCCESS(xml.SetElemValue(SnapVec[i].SnapTime));
		xml.OutOfElem();
	}
	
	xml.OutOfElem();
	return IVS_SUCCEED;
}



//查询窗口ID的XML;
void COCXPlayBackXMLProcess::GetKeyEventXML(CString &xmlToCU,ULONG ulWndID,ULONG ulKeyType)
{
	xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

	char str[32] = {0};
	xmlToCU += "<Content>";
	xmlToCU += "<WndID>";
	(void)_snprintf_s(str, 32, "%d",ulWndID);
	xmlToCU += str;
	xmlToCU += "</WndID>";
	xmlToCU += "<MsgType>";
	memset(str, 0, 32);
	(void)_snprintf_s(str, 32, "%d",ulKeyType);
	xmlToCU += str;
	xmlToCU += "</MsgType>";
	xmlToCU +="</Content>";
}


//获取实况参数信息
LONG COCXPlayBackXMLProcess::GetTalkbackParamXML(LPCTSTR pTalkbackParamXml, IVS_VOICE_TALKBACK_PARAM &talkbackParam)
{
    if (NULL == pTalkbackParamXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Talkback Param", "get talkback param error, xml is null");//lint !e40
        return IVS_PARA_INVALID;
    }

    IVS_DEBUG_TRACE("");
    //解析xml
    CXml xml;
    if (!xml.Parse(pTalkbackParamXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Talkback Param","parse xml error.");	//lint !e40
        return IVS_XML_INVALID;
    }

    if(xml.FindElem("Content"))
    {
        (void)xml.IntoElem();
        if(xml.FindElem("TalkbackParam"))
        {				
            (void)xml.IntoElem();
            IVS_INT32 iTemp = 0;
            const IVS_CHAR* szElemValue = NULL;

            GET_ELEM_VALUE_NUM("ProtocolType", szElemValue, iTemp, xml);
            talkbackParam.uiProtocolType = (IVS_UINT32)iTemp;
            GET_ELEM_VALUE_NUM("DirectFirst", szElemValue, iTemp, xml);
            talkbackParam.bDirectFirst = (IVS_BOOL)iTemp;
        }
        else
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Talkback Param",
                "Parse xml talkback para fail.");//lint !e40
            return IVS_XML_INVALID;
        }
    }
    else
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Talkback Param","Parse xml Content fail.");//lint !e40
        return IVS_XML_INVALID;
    }

    return IVS_SUCCEED;
}//lint !e1762

//上抛获取媒体信息事件加入I按钮坐标
void COCXPlayBackXMLProcess::GetMediaInfoXML(CString &xmlToCU,ULONG ulWndID,CPoint& XY)
{
	xmlToCU = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";

	char str[32] = {0};
	xmlToCU += "<Content>";
	xmlToCU += "<WndID>";
	(void)_snprintf_s(str, 32, "%d",ulWndID);
	xmlToCU += str;
	xmlToCU += "</WndID>";
	xmlToCU += "<X>";
	(void)itoa((int)XY.x, str, 10);
	xmlToCU += str;
	xmlToCU += "</X>";

	xmlToCU += "<Y>";
	(void)itoa((int)XY.y, str, 10);
	xmlToCU += str;
	xmlToCU += "</Y>";
	xmlToCU +="</Content>";
}//lint !e1762
