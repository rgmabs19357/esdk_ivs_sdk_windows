/********************************************************************
 filename	    : TVWallSyncPlayBackMgr.h 
 author		    : f00152768
 created		: 2013/03/21
 description	: 电视墙同步回放上下墙管理
 copyright	    : Copyright (C) 2011-2015
 history	    : 2013/03/21 初始版本
*********************************************************************/
#ifndef _H_TVWALL_SYNC_PLAYBACK_MGR_H_
#define _H_TVWALL_SYNC_PLAYBACK_MGR_H_
#include "windows.h"
#include "hwsdk.h"
#include <list>
#include "SyncPlayBackMgr.h" 

class CVideoPane;

/*
 * FormatTime
 * yyyyMMddHHmmss
 */
#define FORMAT_TIME(t, buf)                                   \
	do {                                                      \
       struct tm _tm;                                         \
       gmtime_s(&_tm, &(t));                                  \
       _snprintf(buf, 14, "%04d%02d%02d%02d%02d%02d",         \
	         _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, \
	         _tm.tm_hour, _tm.tm_min, _tm.tm_sec);            \
	} while (0)

//同步上墙任务
class CTVWallSyncTask
{
public:
	CTVWallSyncTask(ULONG ulTVwallID, 
		IVS_INT32 uiSessionID,
		CVideoPane* pVideoPane,
		const IVS_CHAR *pCamID, 
		const IVS_PLAYBACK_PARAM *pPlaybackParam, 
		const IVS_TVWALL_PARAM* pTVWallParam,
		const REC_TIME_SPAN& timeSpan);
	~CTVWallSyncTask();

public:
	IVS_INT32 StartTVWallPlayBack();
	IVS_INT32 StopTVWallPlayBack();
	IVS_INT32 GetSessionID(void) const {return m_SessionID;}
	ULONG GetCurWndID() const {return m_ulCurWndID;}
private:
	ULONG m_TVwallID;//电视墙ID
	ULONG m_ulCurWndID;//当前的窗格ID
	IVS_CHAR m_CameraID[IVS_DEV_CODE_LEN+1];//摄像机code
	IVS_INT32 m_SessionID;//SessionID
	IVS_PLAYBACK_PARAM m_PlayBackParam;
	unsigned long long m_llStartTime;
	unsigned long long m_llEndTime;
	unsigned long long m_llCurTime;//当前时间
	bool m_bIsHasOnTVWall;//是否已上墙
	CVideoPane* m_pVideoPane;//当前窗格
public:
    IVS_TVWALL_PARAM m_TVWallParam;
public:
	const unsigned long long& GetStartTime() const {return m_llStartTime;}
	const unsigned long long& GetEndTime() const {return m_llEndTime;}
	bool  GetOnTVwallStatus() const {return m_bIsHasOnTVWall;};
	void SetCurTime(unsigned long long ullcurTime){m_llCurTime = ullcurTime;}
	CVideoPane* GetVideoPane() {return m_pVideoPane;}
private:
	CTVWallSyncTask(){}//lint !e1744
};

typedef std::list<CTVWallSyncTask*> TVWALL_SYNC_TASK;
typedef TVWALL_SYNC_TASK::iterator TVWALL_SYNC_TASK_ITER;

//
class CTVWallSyncPlayBackMgr
{
private:
	CTVWallSyncPlayBackMgr();
	~CTVWallSyncPlayBackMgr();
public:
	IVS_INT32 StartTVwallSyncPlayBack();//开启电视墙回放同步上墙
	IVS_INT32 StopTVwallSyncPlayBack();//停止电视墙回放同步上墙
	IVS_INT32 StopTVWallPlayBackByWndID(ULONG ulWndID);//停止窗格对应的上墙
	IVS_INT32 InsertTask(ULONG ulTVwallID, 
		IVS_INT32 uiSessionID,
		CVideoPane* pVideoPane,
		const char *pCamID, 
		const IVS_TVWALL_PARAM* pTVWallParam,
		const IVS_PLAYBACK_PARAM *pPlaybackParam,
		const REC_TIME_SPAN& timeSpan);
	unsigned long long GetCurentTime(const CVideoPane *pVideoPane);

    //暂停电视墙的同步回放
    IVS_INT32 PauseTVWallSyncPlayBack();

    //继续电视墙的同步回放
    IVS_INT32 ResumeTVWallSyncPlayBack();

    //指定窗格是否是电视墙的同步回放
    bool IsTVWallSyncPlayBack(ULONG ulWndID);
private:
	TVWALL_SYNC_TASK m_Task;//当前的电视墙的任务
	typedef std::map<CVideoPane*,unsigned long long> VIDEOPANE_TIME_MAP;
	typedef VIDEOPANE_TIME_MAP::iterator VIDEOPANE_TIME_MAP_ITER;
	VIDEOPANE_TIME_MAP m_VideoTimeMap;
	IVS_INT32 m_iSessionID;
	static unsigned int _stdcall TVwallSyncScheduleProc(LPVOID lpParameter);
public:
	bool m_bStart;//是否开启线程
	CIVSMutex    m_TVWallMutex;
	HANDLE       m_hThread;
public:
	static CTVWallSyncPlayBackMgr& Instance();

};

#endif//_H_TVWALL_SYNC_PLAYBACK_MGR_H_
