#pragma once
#include <map>
// #include "IVSSDKDef.h"
#include "Lock.h"
#include "hwsdk.h"
//#include "IVS_MLIB.h"

#define BASE_HANDLE_TALK_BACK 1000       // 语音对讲的最小handle值

//自定义同步类，后期需要改造成跨平台
class CStreamLock
{
public:
	CStreamLock();
    CStreamLock(HANDLE hHandle);
    ~CStreamLock();

private:
    HANDLE m_hHandle;
};

// 定义Rtsp连接信息
typedef struct _stRTSP_INFO
{
    int     iHandle;
    int     refCont;
    char    szUrl[IVS_URL_LEN];
    bool    bIsTcpDirect;
}RTSP_INFO;
 
typedef struct tagInnerStreamInfo
{
    void   *pStream;
    HANDLE lEventHandle; //用于对流类实例指针pStream进行同步保护，用于支持多线程调用
	long   lStreamHandle;
	HWND   hWnd;
    char   szDeviceID[IVS_DEV_CODE_LEN+1];
	RTSP_INFO voiceRtspInfo;
}INNER_STREAM_INFO,*pINNER_STREAM_INFO;

class CStreamData
{
public:
	CStreamData(void);
	~CStreamData(void);
	long CreateHandle();

	//参数pStream为具体的流类实例
	long AddStream(long lHandle, void *pStream, HWND /*hWnd*/, char* szDeviceID,RTSP_INFO* pRtspInfo);
	void* RemoveStream(long lHandle);
	void* GetStream(long lHandle);
	void* GetStream(char* pszDeviceID);
	void* GetFirstStream();
	BOOL FindStream(long lHandle);
	BOOL FindStream(char* /*pszDeviceID*/);
	long GetHandleByID(char* /*pszDeviceID*/);

private:
    //CCriticalSection m_MapLock;
	CIVSMutex m_criticalSectionEx;
    //CMap <long, long, INNER_STREAM_INFO*, INNER_STREAM_INFO*> m_MapStream;
	std::map <long,  INNER_STREAM_INFO*> m_MapStream;

};
