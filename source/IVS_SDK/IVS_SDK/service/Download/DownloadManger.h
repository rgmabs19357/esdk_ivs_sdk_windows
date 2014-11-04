#ifndef __DOWMLOAD_MGR_H__
#define __DOWMLOAD_MGR_H__

#include "Download.h"
#include "UserInfo.h"
#include "hwsdk.h"
#include "SDKDef.h"

class CDownloadMgr
{
public:
	CDownloadMgr(void);
	~CDownloadMgr(void);

public:
	void SetUserMgr(CUserMgr* pUserMgr);
	
	int StartPlatformDownLoad(
		    const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
		    const char* pCameraCode,
		    const IVS_DOWNLOAD_PARAM* pDownloadParam,
		    IVS_ULONG* pHandle
		  );
	
	int StopPlatformDownLoad(IVS_ULONG ulHandle);

    // 启动容灾录像下载
    int StartDisasterRecoveryDownload(const char* pCameraCode, 
        const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pHandle);
    // 停止容灾录像下载
    int StopDisasterRecoveryDownload(IVS_ULONG ulHandle);

	int DownLoadPause(IVS_ULONG ulHandle);
	int DownLoadResume(IVS_ULONG ulHandle);

	int SetDownLoadSpeed(IVS_ULONG ulHandle, float fSpeed);
	int GetDownLoadInfo(IVS_ULONG ulHandle, IVS_DOWNLOAD_INFO* pInfo);

	int StartPUDownLoad(
			const char* pCameraCode,
			const IVS_DOWNLOAD_PARAM* pDownloadParam,
			IVS_ULONG  *pHandle
		);

	int StopPUDownLoad(IVS_ULONG ulHandle);

	CDownload* GetFreeDownLoad(IVS_ULONG& ulHandle);
    CDownload* GetDownLoad(IVS_ULONG ulHandle);

	// 根据rtsp handle获取下载句柄
    IVS_ULONG  GetDownloadHandlebyRtspHandle(int iRtspHandle);

    // 根据netSourceHandle获取下载句柄
    IVS_ULONG GetDownloadHandleByNetSourceChannel(unsigned long ulNetSourceChannel);

	// 根据播放通道号获取下载句柄
    IVS_ULONG GetDownloadHandleByPlayChannel(unsigned long ulPlayChannel);


    // 获取前端录像文件名
	int GetPUDownLoadFileName(const char* pCameraCode,
                                              const IVS_TIME_SPAN* pTimeSpan,
                                              IVS_RECORD_INFO &stRecordInfo);

private:
	CUserMgr *m_pUserMgr;
	typedef std::map<unsigned long, CDownload*> HandleDownLoadMap;
	typedef HandleDownLoadMap::iterator HandleDownLoadIter;
	
	HandleDownLoadMap m_HandleDownLoadMap;
	VOS_Mutex* m_pHandleDownLoadMutex;	
};

#endif
