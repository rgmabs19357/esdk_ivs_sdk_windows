#pragma once

#include "MediaBase.h"
#include "SDKDef.h"
#include <map>
#include <string>

// 语音广播信息
typedef struct _stBROADCAST_RTSP_INFO
{
    char    szUrl[IVS_URL_LEN];
    char    ServiceIP[IVS_IP_LEN];
    ULONG   ServicePort;
    ULONG   RtspHandle;
    ULONG   PlayHandle;
    ULONG   NetSourceHandle;
	// ULONG   MediaBaseHandle;
}BROADCAST_RTSP_INFO;

typedef std::map<std::string, BROADCAST_RTSP_INFO*>  CODE_HANDLE_MAP;
typedef CODE_HANDLE_MAP::iterator   CODE_HANDLE_ITER;

class CMediaBase;

class CAudioObj : public CMediaBase
{//lint !e1790
public:
    CAudioObj(void);
    ~CAudioObj(void); //lint !e1509

	//开始语音对讲
    int StartVoiceTalkback(unsigned int uiProtocolType,
                           IVS_BOOL bDirectFirst,
                           const char* pCameraCode);
	
	//停止语音对讲
	int StopVoiceTalkback();
 
	//获取协议类型
	ULONG GetProtocolType(char *pProtocol, ULONG ulProtocolType);

	//添加广播镜头
	int AddBroadcastDevice(const IVS_CHAR* pCameraCode);

	//删除广播镜头
	int DeleteBroadcastDevice(const IVS_CHAR* pCameraCode);

	//开始单路广播，内部调用
	int StartSingleBroadcast(int ulBroadcastType, const IVS_CHAR* pCameraCode,
                             const IVS_CHAR* fileNmae=NULL,int iCycle=-1);

	//停止单路语音广播，内部调用
	int StopSingleVoiceBroadcast(const BROADCAST_RTSP_INFO* pMediaRsp);

	//开始语音广播
	int StartRealBroadcast();

	//停止语音广播
	int StopRealBroadcast();

	//添加CameraCode与HANDLE关联数据
	void AddCodeHandle(const IVS_CHAR* pCameraCode, BROADCAST_RTSP_INFO* pRtspInfo);

	//获取CameraCode与HANDLE关联数据
	BROADCAST_RTSP_INFO* GetHandleByCode(const IVS_CHAR* pCameraCode);

	//删除CameraCode与HANDLE关联数据
	void DeleteCodeHandle(const IVS_CHAR* pCameraCode);

    // 清空map
    void ClearUpCodeHandle();

	//清除数据
	void ClearUp();

	//开始文件语音广播
	int StartFileBroadcast(const IVS_CHAR* pFileName,IVS_INT32 iCycle);

    //停止文件语音广播
	int StopFileBroadcast();

	//判断广播文件是否是WAV格式的，是返回0，否则返回负数错误码，内部调用
	int CheckFileExt(const char*pFileName);

    void SetHandle(unsigned long ulHandle);

	inline IVS_SERVICE_TYPE GetServiceType()
	{
		return m_enServiceType;
	}//lint !e1762

	inline void SetServiceType(IVS_SERVICE_TYPE enServiceType)
	{
		m_enServiceType = enServiceType;
	}

	inline unsigned long GetPlayHandle() const { return m_ulHandle; }

	bool IsExistTalkBackRtspHandle(IVS_ULONG ulRtspHandle) const;

	bool IsExistBroadCastRtspHandle(IVS_ULONG ulRtspHandle);

    bool IsExistTalkBackNetHandle(IVS_ULONG ulNetHandle) const;

    bool IsExistBroadCastNetHandle(IVS_ULONG ulNetHandle);

	// int GetBroadCastVoiceCodeByRtspHandle(IVS_ULONG ulRtspHandle, std::string& strVoiceCode);

    int GetBroadCastDeviceCodeByConnHandle(IVS_ULONG ulNetHandle, std::string& strDeviceCode);

private:
    /******************************************************************
	function : StartPlay
	description: 开始语音对讲通道
	input : pstMediaPara，媒体信息；pCameraCode，摄像机编号；stParam，播放参数
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    int StartPlay(const IVS_MEDIA_PARA* pstMediaPara, 
                  const char* pCameraCode, 
                  const START_PLAY_PARAM& stParam);

    int StopPlay(const BROADCAST_RTSP_INFO* pRtspInfo) const;

    int DoGetUrl(IVS_SERVICE_TYPE enServiceType);

    // add by wanglei 00165153:2013.5.3 用于获取指定摄像机支持的音频编码格式
    int GetSupportAudioType(const char* pszCameraCode, unsigned int& unAudioType);

    int GetSupportAudioType(unsigned int &uAudioType);

    int CheckAudioType();

	
private:
	//媒体参数保留下来，启动广播后还有可能通过增加设备来启动新的广播设备，需要用到此参数
	IVS_MEDIA_PARA m_mediaPara;      
	//文件名参数保留下来，启动广播后还有可能通过增加设备来启动新的广播设备，需要用到此参数
	char m_fileName[FILE_NAME_LEN+1];
	//文件广播参数保留下来，启动广播后还有可能通过增加设备来启动新的广播设备，需要用到此参数 0 不循环播放 1 循环播放
	int m_iFileBroadcastCycle;
	//广播类型
	int m_iBroadcastType;
	CODE_HANDLE_MAP m_CodeHandle;		      //CameraCode与HANDLE的MAP
	CRITICAL_SECTION m_csCodeHandleLock;      //CameraCode与HANDLE的MAP的锁

	//是否已经开启广播标志，未开启为false，开启为true
	bool m_bIsBroadcastOpenedFlag;//需要锁保护

    static unsigned int m_stcAudioType;
};
