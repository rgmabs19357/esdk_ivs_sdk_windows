/********************************************************************
    filename	: 	IVSPlayerDataType.h
    author		:	z00201790
    created		:	2012/10/23
    description	:	提供IVS播放库数据结构、错误码、回调等
    copyright	:	Copyright (C) 2011-2015
    history		:	2012/10/23 初始版本;
 *********************************************************************/
#ifndef IVS_PLAYER_DATA_TYPE_H
#define IVS_PLAYER_DATA_TYPE_H

#include <windows.h>
#include "hwsdk.h"
#include "ivs_error.h"

#pragma pack(push, 1)

#define IVS_TRUE 1

//图片格式
#define PLAYER_PICTURE_FORMAT_JPEG 0         //JPEG格式
#define PLAYER_PICTURE_FORMAT_BMP 1         //BMP格式

//音量取值范围
#define PLAYER_AUDIO_MIN_VOLUM 0                //最小音量
#define PLAYER_AUDIO_MAX_VOLUM 100              //最大音量

//日志文件大小
#define MIN_LOG_FILE_SIZE 100             //最小100KB
#define MAX_LOG_FILE_SIZE 100000          //最大100MB

//日志级别
#define MIN_LOG_LEVEL 0
#define MAX_LOG_LEVEL 8
#define IVS_LOG_LEVEL_EMERGENCY 0    //系统不可用
#define IVS_LOG_LEVEL_ALERT 1    //必须立刻采取行动，否则系统不可用
#define IVS_LOG_LEVEL_CRITICAL 2    //严重错误
#define IVS_LOG_LEVEL_ERR 3    //一般错误
#define IVS_LOG_LEVEL_WARNING 4    //警告
#define IVS_LOG_LEVEL_NOTICE 5    //重要的正常信息
#define IVS_LOG_LEVEL_INFO 6    //一般的正常信息
#define IVS_LOG_LEVEL_DEBUG 7    //调试信息

#define DISK_WARNING_SPACE_MB 500   //磁盘告警空间

#define MIN_INPUT_SOURCE_BUFFER_SIZE (50 * 1024)         //最小的输入流缓冲
#define MAX_INPUT_SOURCE_BUFFER_SIZE (10 * 1024 * 1024)  //最大的输入流缓冲

//文件广播类型
#define VOICE_FILE_BROADCAST 0          //文件语音广播
#define VOICE_REAL_BROADCAST 1          //实时语音广播

//语音类型
#define VOICE_TALKBACK  0				//语音对讲
#define VOICE_BROADCAST 1				//语音广播
#define VOICE_NO_CIRCLE_BROADCAST 0     //不循环语音广播
#define VOICE_IS_CIRCLE_BROADCAST 1		//循环语音广播

// 定义需要的常量
#define RTCP_PACKET_VERSION     2
#define RTCP_LEN_BASE           4
//#define RTCP_INVALID_PT         0xFF
#define SDES_ITEM_TYPE_SIZE     1
#define SDES_ITEM_LEN_SIZE      1
#define SDES_ITEM_END_SIZE      2

//RTCP_PACKET_TYPE
#define RTCP_PACKET_SR   200           // RTCP Sender report
#define RTCP_PACKET_RR   201           // RTCP Receiver report
#define RTCP_PACKET_SDES 202           // RTCP Source description packet
#define RTCP_PACKET_BYE  203           // RTCP Bye packet
#define RTCP_PACKET_APP  204           // RTCP App packet

#define PLAYER_AUDIO_SAMPLE_PER_SECOND 8000           //音频采样频率

#define DECODER_FRAME_COUNT (100)
#define VIDEO_BUF_EX_COUNT  (20)

/*****************************通用模块*********************************/

// 模块句柄;
typedef void * IVS_HANDLE;

//最大的通道数量（0--127）;
#define PLAYER_TOTAL_CHANNEL_NUM 128         //最大的通道数量（0--127）

// 输入数据类型,RTP包或帧数据;
typedef enum SourceType
{
    SOURCE_RTP,         // RTP数据;
    SOURCE_FRAME,        // 帧数据;
	SOURCE_FILE         // 文件数据
} SOURCE_TYPE;

// 媒体类型;
typedef enum tagStreamType
{
    STREAM_VIDEO = 0,       // 视频;
    STREAM_AUDIO            // 音频;
}STREAM_TYPE;

// 视频解码模式
typedef enum tagVideoDecodeMode
{
    DECODE_MODE_NONE = 0,
    DECODE_SW_HICILICON,    // 海思软解
    DECODE_HW_INTEL,        // 英特尔硬解
    DECODE_SW_INTEL,        // 英特尔软解
    DECODE_HW_NVDIA,        // 英伟达硬解
    DECODE_SW_NVDIA,        // 英伟达软解
} VIDEO_DECODE_MODE;

// 文件类型;
typedef enum tagFileType
{
    FILE_TYPE_MP4 = 0,           // Mp4文件
    FILE_TYPE_AVI                // AVI文件
} FILE_TYPE;

typedef enum
{
	H264_NALU_TYPE_UNDEFINED = 0,
	H264_NALU_TYPE_SLICE = 1,
	H264_NALU_TYPE_DPA   = 2,
	H264_NALU_TYPE_DPB   = 3,
	H264_NALU_TYPE_DPC   = 4,
    H264_IDR_NALU_TYPE   = 5,
    H264_SEI_NALU_TYPE   = 6,
    H264_SPS_NALU_TYPE   = 7,
    H264_PPS_NALU_TYPE   = 8,
    H264_NALU_TYPE_AUD   = 9,
    H264_NALU_TYPE_EOSEQ = 10,
    H264_NALU_TYPE_EOSTREAM = 11,
    H264_NALU_TYPE_FILL   = 12,

    H264_NALU_TYPE_STAP_A = 24,
    H264_NALU_TYPE_STAP_B = 25,
    H264_NALU_TYPE_MTAP16 = 26,
    H264_NALU_TYPE_MTAP24 = 27,
    H264_NALU_TYPE_FU_A = 28,
    H264_NALU_TYPE_FU_B = 29,
    H264_NALU_TYPE_END
} H264_NALU_TYPE; //lint !e751


/*
 * 媒体属性，从SDP消息中解析
 */
typedef struct {
	unsigned int  type;
    unsigned char rtpmap;     // RTP PayLoad
	unsigned int  samplerate; // 采样率
} MEDIA_ATTR;


/*****************************************************************************
函数名称：PLAYER_EXCEPTION_CALLBACK()
功能描述：回调函数处理异常消息
输入参数：ulPort       通道号
ulMsgType    异常消息类型
pHandle      出现异常的相应类型的句柄
pUserData    用户数据
输出参数：无
返 回 值： ULONG
*****************************************************************************/
typedef  IVS_VOID (CALLBACK * PLAYER_EXCEPTION_CALLBACK)(IVS_ULONG  ulChannel,     // 通道号;
                                                         IVS_INT32  iMsgType,      // 异常消息类型;
                                                         IVS_VOID*  pParam,        // 对应异常的信息，可扩展为结构体指针;
                                                         IVS_VOID*  pUser);        // 用户数据;


/*****************************************************************************
函数名称：PLAYER_EVENT_CALLBACK()
功能描述：回调函数处理事件消息
输入参数：ulChannel    通道号
          iMsgType     事件消息类型
          pParam       对应事件的信息，可扩展为结构体指针
          pUser        用户数据
输出参数：无
返 回 值： ULONG
*****************************************************************************/
typedef  IVS_VOID (CALLBACK * PLAYER_EVENT_CALLBACK)(IVS_ULONG  ulChannel,
                                                     IVS_INT32  iMsgType,
                                                     IVS_VOID*  pParam,
                                                     IVS_VOID*  pUser);

/*****************************拼帧模块开始*********************************/


// 组帧回调结构体
typedef struct
{
    int             StreamType;     //码流类型，音频/视频，参考STREAM_TYPE
    char*           Frame;          //数据流
    unsigned int    Size;           //数据长度
    int             FrameType;      //帧数据类型，SPS、PPS、IDR、P
    unsigned int    TimeStamp;      //时间戳
    IVS_UINT64      TimeTick;       //绝对时间戳
	unsigned long   WaterMarkValue; //水印数据 ，0表示无水印数据
} RAW_FRAME_INFO;

// 定义解密对象，包简单加密，帧简单加密还是帧复杂加密
typedef enum
{
	NO_DEC_TYPE = -1,
    IPC_ENCRYPT_SIMPLE = 0,
    IPC_ENCRYPT_SIMPLE_HEADER = 1,
    IPC_ENCRYPT_AES256 = 2,
    IPC_ENCRYPT_AES256_PARTIAL = 3
} DECRYP_TYPE;

typedef enum tagWaterMarkType
{
	WATER_MARK = 0,                 //使用数字水印
	WATER_MARK_NULL = 1,            //不使用数字水印
}WATER_MARK_TYPE_E;

typedef struct  
{
	unsigned short  uAllPacketNum;   //理应收到的全部包数
	unsigned short  uLostPacketNum;  //未收到的包数
}REC_PACKET_INFO;



typedef struct
{
	unsigned int uVideoCodeRate;
	unsigned int uAvgVideoCodeRate;

	unsigned int uAudioCodeRate;
	unsigned int uAvgAudioCodeRate;

	unsigned int uFrameListSize;
	unsigned int uUsedMemBlockNum;

}CODE_RATE_INFO;

// 组帧回调
typedef IVS_VOID (CALLBACK * REASSM_CALLBACK)(IVS_RAW_FRAME_INFO* pstRawFrameInfo,
											  IVS_CHAR*           pFrame,
											  IVS_UINT32          uFrameSize,
                                              REC_PACKET_INFO*    pstRecPacketInfo,
                                              IVS_VOID*       pUser);

/*****************************拼帧模块结束*****************************/

/*****************************解码模块*********************************/
typedef struct tagVideoParam
{
    IVS_VIDEO_DEC_TYPE VideoFormat;      // 视频编码格式
    IVS_BOOL       HardwareDec;           // 启动硬解标志
    //VIDEO_DECODE_MODE   VideoDecodeMode;  // 视频解码模式
} VIDEO_PARAM, *pVIDEO_PARAM;

typedef struct tagAudioParam
{
    IVS_AUDIO_DEC_TYPE AudioFormat; // 音频编码格式
    IVS_UINT32      SampleRate;  // 音频采样率
    IVS_UINT32      Channels;    // 音频声道数
    IVS_UINT32      BitRate;     // 音频比特率
} AUDIO_PARAM, *pAUDIO_PARAM;

typedef struct
{
    unsigned long ulExceptionCode;
    unsigned long long ullTimeStamp;
    unsigned long ulReseved1;
    unsigned long ulReseved2;
} WATER_MARK_EXCEPTION_DATA;

typedef struct  
{
    int iExceptionCode;
    unsigned long ulReseved1;
    unsigned long ulReseved2;
    unsigned long ulReseved3;
}VOICE_EXCEPTION_DATA;

typedef IVS_VOID (CALLBACK * DECODER_CALLBACK)(IVS_INT32   iStreamType,
                                               IVS_VOID*   pFrameInfo,
                                               IVS_VOID*   pUser);

/*****************************渲染模块*********************************/

//typedef int RENDER_SPEED;

//显示比例
#define DISPLAY_ASPECT_RATIO_FULLSCREEN 0  //铺满
#define DISPLAY_ASPECT_RATIO_ORIGINAL 1  //原始比例
#define DISPLAY_ASPECT_RATIO_NONSPECIFIC 2  //未指定，使用当前值

/*
 * 视频渲染器类型
 */
typedef enum
{
    VOUT_GDIDRAW = 1,
    VOUT_DDRAW   = 2,
    VOUT_D3DDRAW = 3,
} VOUT_DRAW_TYPE;

/* 播放器类型 */
typedef enum
{
    RENDER_MODE_REALTIME = 0, // 实时模式
    RENDER_MODE_SMOOTH  = 1,  // 流畅模式
    RENDER_MODE_REVERSE = 2, // 倒放模式
} RENDER_MODE;


typedef enum {
	STREAM_REALTIME     = 0, // 实时视频
	STREAM_LOCALFILE    = 1, // 本地文件
	STREAM_REMOTEFILE   = 2  // 平台文件 
} STREAM_MODE;


// 播放信息
typedef struct
{
    unsigned int       Frames; // 已经播放帧数
	unsigned long long PalyTime; //毫秒
    float              FrameRate;
    unsigned int  Width;
    unsigned int  Height;
	long          VideoStatus;
	long          AudioStatus;
	unsigned long Reserved1;
	unsigned long Reserved2;
} MEDIA_INFO;

// 播放业务布局类型（电视墙模拟）
typedef enum
{
    DRAW_NORMAL  = 1,
    DRAW_SPLIT   = 2,
    DRAW_JOINT   = 3,
    DRAW_ZOOMIN  = 4,
	DRAW_PARTIAL = 5,
} LAYOUT_TYPE;

// 裂屏显示, 将一路视频分割成多个窗口渲染
typedef struct
{
    unsigned int row;  // 窗口布局 row * col
    unsigned int col;
    unsigned int x;    // 本窗口显示的视频区域(x,y)
    unsigned int y;
} LAYOUT_DRAW_SPLIT;

// 组合显示，将多路视频组合在同一个窗口中渲染
typedef struct
{
    unsigned int row;  // 播放布局 row * col
    unsigned int col;
    unsigned int x;    // 本路视频所处位置(x, y)
    unsigned int y;
} LAYOUT_DARW_JOINT;

// 正常显示
typedef struct
{} LAYOUT_DRAW_NORMAL;

// 局部放大播放业务
// 包括对正常显示，裂屏、组合屏幕显示的局部放大
typedef struct
{
    RECT Base;  // 视频所占所有窗口矩形
    RECT Zoom;  // 被框选的放大区矩形
} LAYOUT_DRAW_ZOOMIN;

// 显示局部图片 
// 参数为百分比
typedef struct 
{ 
	unsigned int left;   
	unsigned int top;
	unsigned int right;
	unsigned int bottom;
}LAYOUT_DRAW_PARTIAL;


// 渲染统计参数，用于DEBUG
typedef struct 
{
	unsigned long InFrames;
	unsigned long DropFrames;
	unsigned long InFrameRate;
	unsigned long OutFrames;
	unsigned long SkipFrames;
	double        OutFrameRate;
	unsigned int  Width;
	unsigned int  Height;
    unsigned long VListSz;
	unsigned long AListSz;
	unsigned int  uLastRtpStamp;
	unsigned long long PlayedTime;
	unsigned long long FrameTick;
} RENDER_STATIS;

/* 播放回调 */
typedef IVS_VOID (CALLBACK * DISPLAY_CALLBACK)(IVS_UINT32 nType, IVS_UCHAR *pBuf,
                                               IVS_UINT32 nWidth, IVS_UINT32 nHeight,
                                               IVS_UINT32 nTimeStamp, IVS_VOID *pUser);

/* 画图回调 */
typedef IVS_VOID (CALLBACK * DRAW_CALLBACK)(HDC hDc, RECT *rc, IVS_UINT64 FrameID, IVS_VOID *pUser);

/* 播放结束回调 */
typedef IVS_VOID (CALLBACK * PLAYEND_CALLBACK)(IVS_VOID *pUser);


/*****************************本地录像模块*********************************/

typedef struct 
{
	time_t tmDownLoadBeg;
	time_t tmDwonLoadEnd;
	int    bReportFinish;
} DOWNLOAD_PARAM;


typedef struct {
	unsigned int FileSize;
	unsigned int TimeSpan;
	unsigned int DiskWarnMB;
	unsigned int DiskFullMB;
	char         Directory[MAX_PATH]; 
} RECORD_CONFIG;

#pragma pack(pop)

#endif //IVS_PLAYER_DATA_TYPE_H

