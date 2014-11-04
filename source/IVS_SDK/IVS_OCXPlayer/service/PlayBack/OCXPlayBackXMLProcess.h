/********************************************************************
	filename	: 	OCXLogMgrXMLProcess.h
	author		:	songjianfeng(00193168)
	created		:	2012/12/03	
	description	:	处理媒体视频回放的XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/
#ifndef __OCX_PLAYBACK_XML_PROCESS_H__ 
#define __OCX_PLAYBACK_XML_PROCESS_H__

#include "OCXXmlProcess.h"
#include "OCXDatatype.h"
#include "SDKDef.h"

/*
 * 同步回放参数结构体
 */
typedef struct {
	time_t beg;
	time_t end;
} TIME_INFO;

typedef std::list<TIME_INFO>     TIME_SPAN_LIST;
typedef TIME_SPAN_LIST::iterator TIME_SPAN_ITER;

class CPlayInfo
{
public:
	unsigned long  ulWndID;
	char           CameraCode[IVS_DEV_CODE_LEN + 1];
	TIME_SPAN_LIST m_TimeList;
};

typedef std::list<CPlayInfo> PLAY_INFO_LIST;
typedef PLAY_INFO_LIST::iterator PLAY_INFO_ITER;

class SyncPlayInfo
{
public:
	unsigned int    m_uProto;
	PLAY_INFO_LIST  m_PlayInfoList;
};

class CRecordSnap
{
public:
	CRecordSnap ()
 	{
		RecordType = 0;
		begine     = 0;
		end        = 0;
		memset(CamCode, 0, sizeof(CamCode));
		memset(NvrCode, 0, sizeof(NvrCode));
		memset(MbuDomain, 0, sizeof(MbuDomain));
		memset(FileName, 0, sizeof(FileName));
		memset(FilePath, 0, sizeof(FilePath)); 
		memset(&RecTimeSpan,0,sizeof(IVS_TIME_SPAN));
	}//lint !e1401
	~CRecordSnap(){};

	int  RecordType;
	char CamCode[IVS_DEV_CODE_LEN + 1];
	char NvrCode[IVS_DEV_CODE_LEN + 1];
	char MbuDomain[IVS_DEV_CODE_LEN + 1];
	char FileName[256];
	char FilePath[256];
	IVS_TIME_SPAN  RecTimeSpan;
	time_t  begine;
	time_t  end;
	std::vector<time_t> SnapPoint;
};

typedef struct 
{
	char FileName[256];
	char SnapTime[IVS_TIME_LEN];
} RECORD_SNAP_NODE;

typedef std::vector<RECORD_SNAP_NODE> SNAP_VEC;
typedef SNAP_VEC::iterator            SNAP_VEC_ITER;


class COCXPlayBackXMLProcess
{
private:
	COCXPlayBackXMLProcess();
public:
	~COCXPlayBackXMLProcess();

public:
	/******************************************************************
	function : GetMediaParamXML
	description: 获取媒体参数信息;
	input : pMediaParamXml,媒体参数信息XML；mediaParam，媒体参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static LONG GetMediaParamXML(LPCTSTR pMediaParamXml, IVS_MEDIA_PARA &mediaParam);


	/******************************************************************
	function : GetPlayInfoEventMsgXML
	description: 查询媒体播放信息上报XML;
	input : xmlToCU,上报xml信息；ulWinID，窗口ID；mediaInfo，媒体信息；XY，坐标信息；resultcode，结果码
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static void GetPlayInfoEventMsgXML(CString &xmlToCU, ULONG ulWinID,IVS_MEDIA_INFO &mediaInfo, CPoint& XY,IVS_INT32 resultcode);

	/******************************************************************
	function : GetWndIDXML
	description: 查询窗口ID的XML;
	input : xmlToCU,上报xml信息；ulWinID，窗口ID
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static void GetWndIDXML(CString &xmlToCU,ULONG ulWndID);

	/******************************************************************
	function : GetPlayBackTime
	description: 查询回放时间
	input : stPlaybackTime,回放时间信息；iResultCode，结果返回码；xmlTime，时间XML；
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    static IVS_INT32 GetPlayBackTime(const IVS_PLAYBACK_TIME& stPlaybackTime, IVS_INT32 iResultCode, CXml& xmlTime);

	/******************************************************************
	 function   : GetPlayBackSpeed
	 description: 获取回放速度;
	 input      : float fSpeed			播放速度;
	 input      : IVS_INT32 iResultCode	错误码;
	 output     : CXml & xmlSpeed		返回给CU的xml;
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetPlayBackSpeed(float fSpeed, IVS_INT32 iResultCode, CXml& xmlSpeed);

	/******************************************************************
	 function   : GetPlayBackStatus
	 description: 获取回放状态;
	 input      : IVS_INT32 iStatus		播放状态;
	 input      : IVS_INT32 iResultCode	错误码;
	 output     : CXml & xmlStatus		返回给CU的xml;
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetPlayBackStatus(IVS_INT32 iStatus, IVS_INT32 iResultCode, CXml& xmlStatus);

	/******************************************************************
	function : GetStreamExceptionMsgXML
	description: 获得码流异常上报XML;
	input :  xmlToCU,上报xml信息；ulWinID，窗口ID；iExceptionCode，异常码；iReseved1，保留字段
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    static void GetStreamExceptionMsgXML(CString &xmlToCU, ULONG ulWinID, int iExceptionCode, int iReseved1);

	/******************************************************************
	function : GetPlaybackParam
	description: 获得回放参数;
	input : pPlayback,待解析的xml；stPlaybackParam，回放参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    static IVS_INT32 GetPlaybackParam(const char* pPlayback, IVS_PLAYBACK_PARAM& stPlaybackParam);

	/******************************************************************
	function : ParsePlayBackSync
	description: 解析同步回放结构体参数;
	input : pPlayback,待解析的xml；stPlaybackParam，回放参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 ParsePlayBackSync(const char *szReqXml, SyncPlayInfo &syncInfo);

	/******************************************************************
	function : ParseLocalPlayBackParam
	description: 解析本地录像回放结构体;
	input : pPlayback,待解析的xml；stPlaybackParam，回放参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 ParseLocalPlayBackParam(const char *szReqXml, IVS_LOCAL_PLAYBACK_PARAM &ParamInfo);
	
	/******************************************************************
	function : ParseLocalPlayBackParam
	description: 解析本地录像回放结构体;
	input : pPlayback,待解析的xml；stPlaybackParam，回放参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static IVS_INT32 PraseRecordSnapParam(const char *szReqXml, CRecordSnap &SnapInfo);

	static IVS_INT32 BuildRecordSnapXml(CXml &xml, IVS_INT32 result, SNAP_VEC &SnapList);

	/******************************************************************
	function : GetKeyEventXML
	description: 获取键盘按键消息
	input : xmlToCU,上报xml信息；ulWinID，选中窗口ID ulKeyType:按键消息ID
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	static void GetKeyEventXML(CString &xmlToCU,ULONG ulWndID,ULONG ulKeyType);

    /******************************************************************
	function : GetTalkbackParamXML
	description: 获取语音对讲参数信息;
	input : pTalkbackParamXml,语音对讲参数信息XML；
            talkbackParam，语音对讲参数信息
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    static LONG GetTalkbackParamXML(LPCTSTR pTalkbackParamXml, IVS_VOICE_TALKBACK_PARAM &talkbackParam);

    //上抛获取媒体信息事件加入I按钮坐标
	static void GetMediaInfoXML(CString &xmlToCU,ULONG ulWndID,CPoint& XY);
};
#endif
