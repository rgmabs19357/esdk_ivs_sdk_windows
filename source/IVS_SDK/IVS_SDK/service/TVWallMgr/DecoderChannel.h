/********************************************************************
 filename    :    DecoderChannel.h
 author      :    g00209332
 created     :    2012/12/19
 description :    解码器通道类，针对单个窗格的视频（实况、回放）上墙下墙处理
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/27 初始版本
*********************************************************************/
#pragma once
#include "hwsdk.h"
#include "SDKDef.h"
#include "IVS_SDKTVWall.h"
#include "rtsp_client_api.h"


//在StartRealPlay中使用
#define CHECK_STOP_FLAG_FOR_START() \
{\
    do\
    {\
    if(m_bReqStop)\
        {\
        HandlePlayException();\
        (void)StopChannel();\
        BP_DBG_LOG("Recive stop request, stop play，AsyncSeq=%lu", m_ulAsyncSeq);\
        return IVS_TVWALL_STOP_FLAG;\
        }\
    } while (false);\
}

//在StartRealPlay中使用
#define CHECK_STOP_FLAG_FOR_PLAY() \
{\
    do\
    {\
    if(m_bReqStop)\
        {\
        if (0 != iHandle)\
            {\
            (void)RTSP_CLIENT_SendTeardownMsg(iHandle, mediaRsp.szURL, 1);\
            (void)RTSP_CLIENT_FreeHandle(iHandle);\
            }\
            BP_DBG_LOG("CDecoderChannel::StartRealPlay, Recive stop request, stop play，AsyncSeq=%lu", m_ulAsyncSeq);\
            return IVS_TVWALL_STOP_FLAG;\
        }\
    } while (false);\
}

//窗格当前状态
typedef enum _wnd_status
{
	WND_STATUS_IDLE,		//空闲
	WND_STATUS_STARTING,	//正在启动
	WND_STATUS_PLAYING     //正在播放
}WND_STATUS;

//播放类型
typedef enum
{
	PLAY_TYPE_NONE = 0,	//无播放
	PLAY_TYPE_REAL = 1,			//实况
	PLAY_TYPE_REPLAY = 2		//回放
}PLAY_TYPE;

//回放类型
typedef enum
{
	REPLAY_TYPE_PLATFORM = 0,	//平台回放
	REPLAY_TYPE_PU = 1,			//前端回放
	REPLAY_TYPE_BACKUP = 2      //备份服务器回放
}REPLAY_TYPE;

//电视墙窗格的rtsp连接信息
typedef struct
{
	IVS_ULONG   ulAsyncSeq;                   //异步通信序号
	IVS_INT32   iPlayType;                    //播放类型，0 无播放，1 实况，2 回放
	IVS_INT32   iHandle;                      //RTSP连接Handle
	IVS_UINT32  uiChannelNo;                  //解码器通道号
	IVS_UINT32  uiStreamType;                 //码流类型， 1-主码流，2-辅码流1，3-辅码流2，… 
    IVS_UINT32  uiProtocolType;               //协议 1-UDP 2-TCP
	IVS_BOOL	bDirectFirst;				  //是否直连优先
	IVS_CHAR    szDecoderID[CUMW_DECODER_ID_LEN+1];//解码器ID
	IVS_CHAR    szCameraCode[IVS_DEV_CODE_LEN +1];  //镜头CODE
	IVS_CHAR    szScuUrl[URL_LEN+1];            //SCU媒体URL
	IVS_CHAR    szTimeSpan[TIME_SPAN_LEN+1];       // 回放时间区间
	IVS_FLOAT   fSpeed;							// 回放速率
	IVS_BOOL    bException;                    //发生了异常，等待重连
}ST_WND_CONNECT_INFO;

//播放参数
typedef struct
{
	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN +1];//摄像机CODE
	IVS_CHAR szDomainCode[IVS_DEV_CODE_LEN +1];//域编码
	IVS_CHAR szNVRCode[IVS_DEV_CODE_LEN +1];   //NVR编码
	PLAY_TYPE		iPlayType;                    //播放类型
	IVS_UINT32      uiStreamType;       // 码流类型，值参考 IVS_STREAM_TYPE
	IVS_UINT32      uiProtocolType;     // 协议类型，1-UDP 2-TCP，默认为1
	IVS_BOOL        bDirectFirst;       // 是否直连优先，0-否 1-是，默认为0
	IVS_BOOL        bMultiCast;         // 是否组播，0-单播，1-组播，默认为0
	IVS_TIME_SPAN   stTimeSpan;         // 实况启动、结束时间
	IVS_FLOAT       fSpeed;             // 回放速率
	REPLAY_TYPE     iReplayType;		// 回放类型
}TVWALL_PLAY_PARAM;

//自动恢复参数
typedef struct  
{
    IVS_BOOL bAutoResuming;                    //正在自动恢复
	IVS_INT32 iPlayType;                       //播放类型
    IVS_INT32   iHandle;                       //RTSP连接Handle
	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN +1];//摄像机CODE
    IVS_CHAR szScuUrl[URL_LEN+1];              //SCU媒体URL
    IVS_TVWALL_PARAM stTVWallParam;            //电视墙参数
    IVS_REALPLAY_PARAM stRealplayParam;        //实况参数
	CUMW_BREAKDOWN_REASON iReson;			   //异常原因 1-解码器下线 2-视频流中断
}AUTO_RESUME_PARAM;

class CTVWallMgr; //lint !e763
class CDecoderChannel
{
public:
	CDecoderChannel(IVS_ULONG ulAsyncSeq, 
					const IVS_CHAR* pDecoderID, 
					IVS_UINT32 uiChannel,
					CTVWallMgr* pTVWallMgr);
	virtual ~CDecoderChannel(void);
	// 启动电视墙实时浏览
	IVS_INT32 StartRealPlay(const IVS_CHAR* pCameraCode, 
                            const IVS_REALPLAY_PARAM& stRealplayParam, 
                            const IVS_TVWALL_PARAM& stTVWallParam);

    // 启动电视墙录像回放
	IVS_INT32 StartPlayBack(const IVS_CHAR* pCameraCode, 
                                    const IVS_PLAYBACK_PARAM& stPlaybackParam, 
                                    const IVS_TVWALL_PARAM& stTVWallParam, 
									REPLAY_TYPE iReplayType,
									const IVS_CHAR* pDomainCode=NULL,
									const IVS_CHAR* pNVRCode=NULL,
                                    const IVS_CHAR* pFileName=NULL);

	// 停止播放
    IVS_INT32 StopPlay();
	// 单帧播放
	IVS_INT32 PlaybackFrameStepForward();
	// 暂停回放
	IVS_INT32 PausePlayback();
	// 恢复回放
	IVS_INT32 ResumePlayback();
	// 开启声音
	IVS_INT32 StartSound();
	// 停止声音
	IVS_INT32 StopSound();
	// 设置音量
	IVS_INT32 SetVolume(IVS_UINT32 uiVolumeValue);
public:
	//获取AsyncSeq
	IVS_ULONG GetAsyncSeq() const;
    //获取解码器ID
    const IVS_CHAR* GetDecoderID() const;
	//获取通道号
	IVS_UINT32 GetChannel() const;
    //获取正在上墙的Camera
    const IVS_CHAR* GetCameraCode() const;
    //是否正在启动
    bool IsStarting() const;
    //是否正在播放
    bool IsPlaying() const;
    //获取自动恢复参数
    void GetAutoResumeParam(AUTO_RESUME_PARAM* pParam) const;
	//出现异常
	void SetException(bool bException=true);
protected:
	CDecoderChannel(void);//pc-lint
	// 初始化通道
	virtual IVS_INT32 InitChannel(IVS_UINT32 uiProtocolType, MEDIA_ADDR& stDecoderAddr) = 0;
	// 启动通道开始解码
	virtual IVS_INT32 StartChannel(const std::string strURL, const MEDIA_ADDR stSourceMediaAddr) = 0;
	// 结束通道解码
	virtual IVS_INT32 StopChannel() = 0;
	// 开启通道声音
	virtual IVS_INT32 StartChannelSound() = 0;
	// 停止通道声音
	virtual IVS_INT32 StopChannelSound() = 0;
	// 设置通道声音音量
	virtual IVS_INT32 SetChannelVolume(IVS_UINT32 uiVolumeValue) = 0;

	//处理异常，将相关状态、数据重置（构造函数和异常发生时调用）
	void HandlePlayException();
	//解析URL,pKey是想要得到的值
	IVS_INT32 ParseURL(const char* pUrl, const char* pKey, std::string& strValue);
private:
	/**************************************************************************
    * name       : DealRuningPlayHard
    * description:  检查并处理当前窗格的播放
                    若播放类型不同，则关闭原有播放，否则具体按实况和回放去处理：
                    请求实况时：
                        若当前窗格无实况，直接返回true
                        若正在实况，且请求实况的镜头ID相同，码流不同，则关闭当前实况，返回 true
                        若正在实况，且请求实况的镜头ID不同，则关闭当前实况，返回 true
                        若正在实况，且请求实况的镜头ID相同，码流相同，返回 false，后期无需开启新的实况，直接返回
                    请求回放时：
                        若镜头ID相同，直接返回FALSE，后期无需开启新的回放
                        若镜头ID不同，则关闭当前回放，返回 true
    * input      : ulPlayType 新的播放请求类型，1为实况，2为回放
    * output     : NA
    * return     : RET_OK-成功     RET_FAIL-失败
    * remark     : NA
    **************************************************************************/
    bool DealRuningPlayHard(const IVS_CHAR *pCameraCode, IVS_UINT32 uiStreamType, IVS_INT32 iPlayType, IVS_BOOL bDirect);

    //开始播放
    IVS_INT32 StartPlay(const TVWALL_PLAY_PARAM& stPlayParam,
                        const IVS_TVWALL_PARAM& stTVWallParam,
                        const IVS_CHAR* pFileName = NULL);
	//通知播放(rtsp通知mu，通知解码器解码）
	IVS_INT32 NotifyPlay(const TVWALL_PLAY_PARAM& stPlayParam, const IVS_TVWALL_PARAM& stTVWallParam, 
						const ST_MEDIA_RSP& mediaRsp, const MEDIA_ADDR& stDecoderAddr);
    //获取URL，根据iPlayType判断是实况还是回放
	IVS_INT32 GetPlayURL(const TVWALL_PLAY_PARAM& stPlayParam,
                        ST_MEDIA_RSP& stMediaRsp, const IVS_CHAR* pFileName = NULL);
public:
    ST_WND_CONNECT_INFO m_wndConnectInfo;	//连接信息
    IVS_ULONG m_ulHandle;
    WND_STATUS m_wndStatus;					//窗格状态
    bool m_bReqStop;						//请求停止
protected:
	IVS_ULONG m_ulAsyncSeq;					//与电视墙模块通信异步业务序号
    std::string m_strDecoderID;             //解码器ID
    IVS_UINT32 m_uiChannel;                 //解码器通道号
	bool m_bStoping;						//正在停止
	CTVWallMgr* m_pTVWallMgr;				//电视墙管理对象
public:
	IVS_ULONG GetHandle()const {return m_ulHandle;}
};
