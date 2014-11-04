#pragma once
// #include "IVSSDKDef.h"
#include "StreamData.h"
#include "AudioObj.h"

class CAudioMgr 
{
public:
	CAudioMgr(void);
	virtual ~CAudioMgr(void);
	//清除数据
	void ClearUp();

	//设置用户对象
	//void SetUserObj(void* pUserObj);

    void SetUserMgr(CUserMgr* pUserMgr);

	//开始语音对讲
    IVS_INT32 StartVoiceTalkback(const IVS_VOICE_TALKBACK_PARAM* pTalkbackParam,
                                 const IVS_CHAR* pCameraCode,
                                 IVS_ULONG* pHandle);
	
	//停止语音对讲
	IVS_INT32 StopVoiceTalkback(INT32 iTalkbackHandle);

	//添加语音设备到广播组
	IVS_INT32 AddBroadcastDevice(const IVS_CHAR* pCameraCode);	

    //删除语音设备
	IVS_INT32 DeleteBroadcastDevice(const IVS_CHAR* pCameraCode);

	//开始实时语音广播
	IVS_INT32 StartRealBroadcast();

    //停止实时语音广播
	IVS_INT32 StopRealBroadcast();

	//开始文件语音广播
	IVS_INT32 StartFileBroadcast(const IVS_CHAR* pFileName,IVS_INT32 iCycle);

    //停止文件语音广播
	IVS_INT32 StopFileBroadcast();

    //设置音量
    IVS_INT32 SetVolume(unsigned int uiPlayHandle, unsigned int uiVolumeValue);

    //获取音量
    IVS_INT32 GetVolume(unsigned int uiPlayHandle, unsigned int* puiVolumeValue);

	CAudioObj* GetAudioObj(IVS_ULONG ulHandle);

	IVS_ULONG GetTalkBackPlayHandleByRtspHandle(IVS_ULONG ulRtspHandle);

	IVS_ULONG GetBroadCastPlayHandleByRtspHandle(IVS_ULONG ulRtspHandle);

    IVS_ULONG GetTalkBackPlayHandleByNetHandle(IVS_ULONG ulNetHandle);

    IVS_ULONG GetBroadCastPlayHandleByNetHandle(IVS_ULONG ulNetHandle);

    // 根据RTSP或NetSource等Handle获取对应的VoiceCode
    IVS_INT32 GetBroadCastDeviceCodeByConnHandle(IVS_ULONG ulRtspOrNetHandle, std::string& strDeviceCode);
	//IVS_INT32 GetBroadCastVoiceCodeByRtspHandle(IVS_ULONG ulRtspHandle, std::string& strVoiceCode);
    IVS_INT32 GetTalkBackDeviceCodeByConnHandle(IVS_ULONG ulRtspOrNetHandle, std::string& strDeviceCode);

	bool IsBroadCast(IVS_ULONG ulPlayHandle) const;
private:
    CAudioObj* GetFreeTalkBack(IVS_ULONG& ulHandle);

protected:
	CStreamData m_StreamData;

	//指向用户对象
	//void *m_pUserObj;

	CAudioObj m_audioObj;

    CUserMgr *m_pUserMgr;
	
    typedef std::map<unsigned long, CAudioObj*> HandleTalkBackMap;
    typedef HandleTalkBackMap::iterator HandleTalkBackMapIter;
    HandleTalkBackMap m_HanleTalkBackMap;

    VOS_Mutex* m_pHandleTalkBackMutex;
};


