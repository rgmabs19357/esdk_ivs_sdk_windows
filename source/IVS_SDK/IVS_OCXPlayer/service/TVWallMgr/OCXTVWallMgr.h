#pragma once
#include <afx.h>
#include "SDKDef.h"
#include "IVS_SDKTVWall.h"
#include "OCXTVWallXMLProcess.h"
class COCXTVWallMgr
{
public:
	COCXTVWallMgr(void);
	~COCXTVWallMgr(void);
	// 初始化电视墙
	static IVS_INT32 InitTVWall(IVS_INT32 iSessionID, PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
	// 释放电视墙
	static IVS_INT32 ReleaseTVWall(IVS_INT32 iSessionID);
	// 获取电视墙列表
	static IVS_VOID GetTVWallList(CString &strResult);
	// 获取电视墙信息
	static IVS_VOID GetTVWallByID(IVS_UINT32 uiTVWallID, CString &strResult);
	// 增加电视墙
	static CString AddTVWall(const CString& strTVWall);
	// 删除电视墙
	static IVS_INT32 DeleteTVWall(IVS_ULONG uiTVWallID);
	// 更改电视墙信息
	static IVS_INT32 ModifyTVWallInfo(const CString& strResult);
	// 更改电视墙布局
	static IVS_INT32 ModifyTVWallLayout(const CString& strResult);
	// 启动电视墙实时浏览
	static IVS_INT32 StartRealPlayTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
                                        const CString& strRealplayParam, const TVWall_VPANEID& vPaneID);
	// 停止电视墙实时浏览
	static IVS_INT32 StopRealPlayTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// 开始电视墙平台录像回放
	static IVS_INT32 StartPlatformPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
                                                const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID);
	// 停止电视墙平台录像回放
	static IVS_INT32 StopPlatformPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// 开始电视墙前端录像回放
	static IVS_INT32 StartPUPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
		const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID);
	// 停止电视墙前端录像回放
	static IVS_INT32 StopPUPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// 开始电视墙备份录像回放
	static IVS_INT32 StartBackupPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strDomainCode, 
												const CString& strNVRCode, const CString& strCameraCode, 
												const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID);
	// 停止电视墙备份录像回放
	static IVS_INT32 StopBackupPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// 暂停电视墙录像回放
	static IVS_INT32 PlayBackPauseTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// 恢复电视墙录像回放
	static IVS_INT32 PlayBackResumeTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// 单帧播放
	static IVS_INT32 PlaybackFrameStepForwardTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// 根据解码器ID、解码器通道和电视墙ID 获取电视墙窗格ID
	static IVS_INT32 GetTVWallWndIDByDecoderIDChannel(const IVS_ULONG& ulTVWallID,const CUMW_NOTIFY_REMOTE_DECODER_STATUS& decoder,TVWall_VPANEID_LIST& VPaneIDList);
	static IVS_INT32 GetTVWallWndIDByDecoderIDChannel(const IVS_ULONG& ulTVWallID,const CString& strDecoderID,IVS_UINT32 uiChannel,TVWall_VPANEID& stPaneID);
	// 根据解码器ID和电视墙ID 获取电视墙ID
	static IVS_INT32 GetTVWallWndIDByDecoderID(const IVS_ULONG& ulTVWallID,const CString& cstrDecoderID,std::vector<IVS_ULONG>& ulWndIDVec);
	// 解码器注册信息列表
	static IVS_VOID GetDecoderList(IVS_INT32 iSessionID, CString &strResult);
	// 获取在线解码器ID列表
	static IVS_INT32 GetOnlineDecoderIDList(IVS_INT32 iSessionID,std::list<CUMW_NOTIFY_REMOTE_DECODER_STATUS>& decoderIDList);

	// 开启声音
	static IVS_INT32 PlaySoundTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID);
	// 停止声音
	static IVS_INT32 StopSoundTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID);
	// 设置音量
	static IVS_INT32 SetVolumeTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID, IVS_UINT32 uiVolumeValue);
};
