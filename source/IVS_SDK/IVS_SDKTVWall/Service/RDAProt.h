/********************************************************************
    filename	: 	CRDAProt.h
    author		:	wx58056
    created		:	2011/10/28
    description	:	RDA协议栈
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/10/28 初始版本
 *********************************************************************/

#ifndef RDAPROT_H
#define RDAPROT_H

/*!< 模块基础头文件 */
#include "vos.h"

/*!< 内存分配相关函数的头文件 */
#include "vos_common.h"

#define RDA_PROT_VERSION 0x02  //协议版本号
#define RDA_PROT_TYPE 0x01  //协议类型

//BEGIN协议长度相关定义
#define LEN_RDA_MSG_HEAD (2 + 2 + 4 + MD5_DIGEST_LENGTH + 1 + 1 + 2)

/* 2个字节表示tag，2个字节表示length */
#define TLV_HEAD_LENGTH (4 * sizeof(RDA_CHAR))
#define LEN_RDA_NAME 32   //RDA协议中用到的名字长度
#define LEN_RDA_ID 20   //解码器id长度
#define LEN_MAX_RDA_STR 512  //一个字符串属性的值最大是3K字节

#define LEN_RDA_ATTRTAG_NAME 32  //属性标识的名称长度
#define LEN_RDA_MSGTYPE_NAME 32  //消息类型的名称长度
#define LEN_RDA_SESSION_ID 20  //Sessionid长度

//END协议长度相关定义

#define MAX_CHANNEL_NUM CUMW_DECODER_CHANNEL_MAX_NUM //一个解码器最大支持的通道数目

const unsigned char MAX_PAYLOAD_TYPE_NUM = CUMW_PAY_LOAD_TYPE_NUM;  //支持的最大音频/视频的PayloadType

//BEGIN MD5相关定义
#define MAX_MD5KEY_LENGTH 128
#define MD5_DIGEST_LENGTH 16

//MD5 密钥和密钥长度
#define DEFAULT_MD5_KEY "NVS"
#define DEFAULT_MD5_KEY_LENGTH strlen(DEFAULT_MD5_KEY)

//是否进行MD5校验开关,1表示要进行MD5校验，0表示不需要进行MD5校验
#define NO_MD5_CHECK 1

//解码器编码格式长度
#define LEN_RDA_ENCODING 16

//END  MD5相关定义

//包类型定义
enum RDA_PACK_TYPE
{
    RDA_MSG_REQ = 0x0001,  //请求
    RDA_MSG_RSP = 0x0002   //响应
};

//定义消息类型名称
#define RDA_MSG_NAME_INIT_BUSINESS_REQ "RDAInitBusinessReq"
#define RDA_MSG_NAME_INIT_BUSINESS_RSP "RDAInitBusinessRsp"
#define RDA_MSG_NAME_REGISTE_REQ "RDARegisteReq"
#define RDA_MSG_NAME_REGISTE_RSP "RDARegisteRsp"
#define RDA_MSG_NAME_HEARTBEAT_REQ "RDAHeartBeatReq"
#define RDA_MSG_NAME_HEARTBEAT_RSP "RDAHeartBeatRsp"
#define RDA_MSG_NAME_START_BUSINESS_REQ "RDAStartBusinessReq"
#define RDA_MSG_NAME_START_BUSINESS_RSP "RDAStartBusinessRsp"
#define RDA_MSG_NAME_STOP_BUSINESS_REQ "RDAStopBusinessReq"
#define RDA_MSG_NAME_STOP_BUSINESS_RSP "RDAStopBusinessRsp"
#define RDA_MSG_NAME_AUDIO_CONTROL_REQ "RDAAudioControlReq"
#define RDA_MSG_NAME_AUDIO_CONTROL_RSP "RDAAudioControlRsp"
#define RDA_MSG_NAME_TUNEUP_VOLUME_REQ "RDATuneupVolumeReq"
#define RDA_MSG_NAME_TUNEUP_VOLUME_RSP "RDATuneupVolumeRsp"
#define RDA_MSG_NAME_UNREGISTER_REQ "RDAUnRegisteReq"
#define RDA_MSG_NAME_UNREGISTER_RSP "RDAUnRegisteRsp"
#define RDA_MSG_NAME_CONSOLE_SHUTDOWN_REQ "RDAConsoleShutDownReq"
#define RDA_MSG_NAME_CONSOLE_SHUTDOWN_RSP "RDAConsoleShutDownRsp"
#define RDA_MSG_NAME_ERROR_NOTIFY_REQ "RDAErrorNotifyReq"
#define RDA_MSG_NAME_ERROR_NOTIFY_RSP "RDAErrorNotifyRsp"
#define RDA_MSG_NAME_SET_RD_DISPLAY_MODE_REQ "RDASetRDDisplayModeReq"
#define RDA_MSG_NAME_SET_RD_DISPLAY_MODE_RSP "RDASetRDDisplayModeRsp"

//定义属性名称
#define RDA_TAG_NAME_BUSINESSID "BusinessID"
#define RDA_TAG_NAME_DECODERID "DecoderID"
#define RDA_TAG_NAME_DECODERNAME "DecoderName"
#define RDA_TAG_NAME_CHANNELNO "ChannelNo"
#define RDA_TAG_NAME_CHANNELSINFO "ChannelsInfo"
#define RDA_TAG_NAME_AUDIOCONTROL "AudioControl"
#define RDA_TAG_NAME_TUNEUPVOLUME "TuneupVolume"
#define RDA_TAG_NAME_RD_DISPLAY_MODE "RDDisplayMode"
#define RDA_TAG_NAME_ENCODING "Encoding"

const char RDA_TAG_NAME_TRANSPROT_PROTOCOL[] = "TransportProtocol";
const char RDA_TAG_NAME_ERROR_CODE[] = "ResultCode";
const char RDA_TAG_NAME_ERROR_DESCRIPTION[] = "ResultDescription";
const char RDA_TAG_NAME_VIDEO_PAY_LOAD_TYPES[] = "VideoPayloadTypes";
const char RDA_TAG_NAME_AUDIO_PAY_LOAD_TYPES[] = "AudioPayloadTypes";
const char RDA_TAG_NAME_MEDIA_IP[] = "MediaIP";
const char RDA_TAG_NAME_TCP_INTERLEAVED_PORT[] = "TcpInterleavedPort";
const char RDA_TAG_NAME_UDP_VIDEO_RTP_PORT[]  = "UdpVideoRtpPort";
const char RDA_TAG_NAME_UDP_VIDEO_RTCP_PORT[] = "UdpVideoRtcpPort";
const char RDA_TAG_NAME_UDP_AUDIO_RTP_PORT[]  = "UdpAudioRtpPort";
const char RDA_TAG_NAME_UDP_AUDIO_RTCP_PORT[] = "UdpAudioRtcpPort";
const char RDA_TAG_NAME_VIDEO_PAY_LOAD_TYPE[] = "VideoPayloadType";
const char RDA_TAG_NAME_AUDIO_PAY_LOAD_TYPE[] = "AudioPayloadType";
const char RDA_TAG_NAME_SESSION_URL[] = "SessionUrl";
const char RDA_TAG_NAME_SESSION_ID[] = "SessionID";

//基本数据类型重定义
typedef char           RDA_CHAR;   //Integer1: 1个字节整型
typedef unsigned char  RDA_UCHAR;  //unsigned Integer1: 1个字节无符号整型；Enum: 1个字节枚举类型
typedef short          RDA_INT2;   //Integer2: 2个字节整型
typedef unsigned short RDA_UINT2;  //unsigned Integer2: 2个字节无符号整型
typedef long           RDA_INT4;   //Integer4: 4个字节整型
typedef unsigned long  RDA_UINT4;  //Unsigned Integer: 4个字节无符号整型

//字符串相关数据类型定义
typedef char RDA_STRING_NAME[LEN_RDA_NAME + 1]; //解码器名称类型定义
typedef char RDA_STRING_ID[LEN_RDA_ID + 1]; //解码器id类型定义
typedef char RDA_STRING[LEN_MAX_RDA_STR];
typedef char RDA_STRING_ENCODING[LEN_RDA_ENCODING + 1]; //解码器编码格式类型定义

//结构体类型定义
typedef struct tagChannelInfo
{
    RDA_UINT2       usChannelNo;
    RDA_STRING_NAME szChannelName;
} RDA_CHANNEL_INFO;

typedef struct tagChannelsInfo
{
    RDA_UINT2        usChannelNum;
    RDA_CHANNEL_INFO stChannelInfo[MAX_CHANNEL_NUM];
} RDA_CHANNELS_INFO;

//支持的PayloadType信息
struct RDA_PAY_LOAD_TYPES_INFO
{
    RDA_UCHAR ucPayloadTypeNum;   //支持的PayloadType个数，最大不超过MAX_PAYLOAD_TYPE_NUM
    RDA_UCHAR stPayloadTypeArry[MAX_PAYLOAD_TYPE_NUM];    //支持的PayloadType值，元素个数为ucPayloadTypeNum
};

//数据类型标识定义
enum RDA_DATA_TYPE
{
    enRDA_CHAR  = 0x00,
    enRDA_UCHAR = 0x01,
    enRDA_INT2  = 0x02,
    enRDA_UINT2 = 0x03,
    enRDA_INT4   = 0x04,
    enRDA_UINT4  = 0x05,
    enRDA_STRING = 0x06,
    enRDA_STRING_NAME = 0x07,
    enRDA_STRING_ID = 0x08,
    enRDA_CHANNELS_INFO = 0x09,
    RDA_TYPE_PAY_LOAD_TYPES_INFO = 0x0A
};

//RDA消息属性TAG定义
enum RDA_ATTR_TAG
{
    TagBusinessID = 0x01,
    TagDecoderID   = 0x02,
    TagDecoderName = 0x03,
    TagChannelNo   = 0x04,
    TagChannelName = 0x05,
    TagChannelNum  = 0x06,

    //TagDecoderType = 0x07,
    //TagLensID               = 0x08,
    TAG_TRANSPORT_PROTOCOL = 0x09, //媒体流传输协议
    //TagSendMediaPort        = 0x0A,
    TagVolume = 0x0B,
    TagChannelInfo  = 0x0C,
    TagChannelsInfo = 0x0D,
    TagErrorCode = 0x0E,
    TagErrorDescription = 0x0F,
    TagAudioCmd = 0x10,
    //TagSendMediaIP          = 0x11,
    TagRDDispayMode = 0x12,
    TAG_SESSION_ID = 0x13,     //会话ID
    //TagStreamID             = 0x14, //流ID
    TAG_VIDEO_PAY_LOAD_TYPES = 0x15, //解码器支持的视频PayloadType，类型参见RDA_PAY_LOAD_TYPES_INFO
    TAG_AUDIO_PAY_LOAD_TYPES = 0x16, //解码器支持的音频PayloadType，类型参见RDA_PAY_LOAD_TYPES_INFO
    TAG_MEDIA_IP = 0x17, //解码器/服务端的媒体IP
    TAG_TCP_INTERLEAVED_PORT = 0x18, //解码器/服务端的TCP交织端口。TCP方式才有该属性
    TAG_UDP_VIDEO_RTP_PORT  = 0x19,     //解码器/服务端的UDP视频RTP端口。UDP方式才有该属性
    TAG_UDP_VIDEO_RTCP_PORT = 0x1A, //解码器/服务端的UDP视频RTCP端口。UDP方式才有该属性
    TAG_UDP_AUDIO_RTP_PORT  = 0x1B,     //解码器/服务端的UDP音频RTP端口。UDP方式才有该属性
    TAG_UDP_AUDIO_RTCP_PORT = 0x1C, //解码器/服务端的UDP音频RTCP端口。UDP方式才有该属性
    TAG_VIDEO_PAY_LOAD_TYPE = 0x1D, //协商后的视频PayloadType
    TAG_AUDIO_PAY_LOAD_TYPE = 0x1E, //协商后的音频PayloadType
    TAG_SESSION_URL = 0x1F, //会话URL。作为RTSP的Request-URI
	TagEncoding = 0x20   //解码器名称编码类型
};

//消息类型定义
enum RDAMsgType
{
    RDA_HEARTBEAT_REQ = 0x0001, //心跳请求
    RDA_HEARTBEAT_RSP = 0x1001, //心跳响应

    RDA_REGISTER_REQ = 0x0002, //远程解码器注册消息
    RDA_REGISTER_RSP = 0x1002,

    RDA_INIT_BUSINESS_REQ = 0x0003, //主控初始化远程解码业务请求消息
    RDA_INIT_BUSINESS_RSP = 0x1003, //主控初始化远程解码业务响应消息

    RDA_START_BUSINESS_REQ = 0x0004, //主控通知启动远程解码业务
    RDA_START_BUSINESS_RSP = 0x1004,

    RDA_STOP_BUSINESS_REQ = 0x0005, //主控停止远程解码业务
    RDA_STOP_BUSINESS_RSP = 0x1005,

    RDA_AUDIO_CONTROL_REQ = 0x0006, //主控开始/停止远程解码音量播放请求消息
    RDA_AUDIO_CONTROL_RSP = 0x1006,

    RDA_TUNEUP_VOLUME_REQ = 0x0007, //主控发起音量调节请求消息
    RDA_TUNEUP_VOLUME_RSP = 0x1007,

    RDA_UNREGISTER_REQ = 0x0008, //远程解码器注销消息
    RDA_UNREGISTER_RSP = 0x1008,

    RDA_CONSOLE_SHUTDOWN_REQ = 0x0009, //主控关闭通知消息
    RDA_CONSOLE_SHUTDOWN_RSP = 0x1009,

    RDA_ERROR_NOTIFY_REQ = 0x000A, //远程解码器错误上报主控
    RDA_ERROR_NOTIFY_RSP = 0x100A,

    RDA_SET_RD_DISPLAY_MODE_REQ = 0x0010, //主控发起设置远程解码器显示模式请求消息
    RDA_SET_RD_DISPLAY_MODE_RSP = 0x1010
};

typedef struct tagRDAMsgHead
{
    RDA_UINT2 usPackType; //包类型：标识该包类型，占用2字节。1 请求包, 2 响应包
    RDA_UINT2 usPackID; //包流水号，标识该包，占用2字节。
    RDA_UINT4 ulPackLength; //包长度：定义消息包总长度（字节数），包括包头和消息体。占用4字节。
    RDA_CHAR  arrayMD5[MD5_DIGEST_LENGTH]; //MD5校验和
    RDA_UCHAR ucProtType; //协议类型: 占1字节, RDA是0x01
    RDA_UCHAR ucVersion; //协议版本定义，占用1字节, RDA是0x01
    RDA_UINT2 usMsgType; //消息类型，占2字节
} RDAMsgHead;

class CRDAMsgAttrMgr;
class CRDAMsg
{
public:
    CRDAMsg();
    virtual ~CRDAMsg();
    // 会出现warning C4150: 删除指向不完整“m_pRDAMsgAttrMgr”类型的指针；没有调用析构函数 
    // 这个waring会导致内存泄露。前向申明的类的析构函数没有被调用
    // class CRDAMsgAttrMgr;这种方式向前申明，其后面的类只能申明其指针，前向申明以后的类无法看到其类实体。
    // 所以，delete的时候，CRDAMsgAttrMgr的析构函数对后面的类是透明不可见的，除非使用头文件包含。
    // sxj 将析构函数移到cpp里
    //{
    //    VOS_DELETE(m_pRDAMsgAttrMgr); /*lint !e1551*/ //经过确认VOS_DELETE中有保护
    //};
public:

    /**************************************************************************
    * name       : Init
    * description: 初始化
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    LONG         Init( );

    /**************************************************************************
    * name       : DecodeBody
    * description: 解码
    * input      : pBuffer  要解码的数据缓冲区
                   ulLength 要解码的数据长度
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    LONG         DecodeBody(const char * pBuffer, const ULONG ulLength);

    /**************************************************************************
    * name       : EncodeBody
    * description: 编码
    * input      : pBuffer      要编码的数据缓冲区
                   ulMaxLength  最大长度
                   ulUsedLength 已用的长度
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    LONG         EncodeBody(char *pBuffer, const ULONG ulMaxLength,
                            ULONG &ulUsedLength);

    /**************************************************************************
    * name       : Print
    * description: 写日志
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    VOID         Print();

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual long RegisterAttrs( );

public:
    RDAMsgHead m_stMsgHead; //消息头
    char m_szRDAMsgTypeName[LEN_RDA_MSGTYPE_NAME+1]; //消息名称
public:
    CRDAMsgAttrMgr * m_pRDAMsgAttrMgr;
};

/* 解码器注册消息 */
class CRDARegisteReq : public CRDAMsg
{
public:
    CRDARegisteReq();
    ~CRDARegisteReq(){};
public:
    RDA_STRING_ID m_szDecoderID;          //解码器ID（20位，不包括\0）
    RDA_STRING_NAME m_szDecoderName;        //解码器名称（32位，不包括\0）
    RDA_CHANNELS_INFO m_stChannelsInfo;       //多个通道信息
    RDA_PAY_LOAD_TYPES_INFO m_stVideoPayloadTypes;  //解码器支持的视频PayloadType信息，最多16个
    RDA_PAY_LOAD_TYPES_INFO m_stAudioPayloadTypes;  //解码器支持的音频PayloadType信息，最多16个
	RDA_STRING_ENCODING m_szDecoderEncoding; //解码器编码格式
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 注册消息的响应消息 */
class CRDARegisteRsp : public CRDAMsg
{
public:
    CRDARegisteRsp();
    ~CRDARegisteRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 解码初始化消息 */
class CRDAInitBusinessReq : public CRDAMsg
{
public:
    CRDAInitBusinessReq( );
    ~CRDAInitBusinessReq( ){};
public:
    RDA_UINT4 m_ulBusinessID; //业务ID
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
    RDA_UINT2 m_usChannelNo; //通道编号
    RDA_UCHAR m_ucTransportProtocol; //视频传输协议
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 解码初始化响应消息 */
class CRDAInitBusinessRsp : public CRDAMsg
{
public:
    CRDAInitBusinessRsp();
    ~CRDAInitBusinessRsp(){};

public:
    RDA_STRING_ID m_szDecoderID;          //解码器ID（20位，不包括\0）
    RDA_UINT4 m_ulErrorCode;          //返回码，0代表成功
    RDA_STRING m_szErrorDescription;   //错误码对应错误描述
    RDA_UCHAR m_ucTransportProtocol;  //视频传输协议
    RDA_STRING m_szMediaIP;            //解码器的媒体通道IP
    RDA_UINT2 m_usTcpInterleavedPort; //TCP交织端口
    RDA_UINT2 m_usUdpVideoRtpPort;    //UDP视频RTP端口
    RDA_UINT2 m_usUdpVideoRtcpPort;   //UDP视频RTCP端口
    RDA_UINT2 m_usUdpAudioRtpPort;    //UDP音频RTP端口
    RDA_UINT2 m_usUdpAudioRtcpPort;   //UDP音频RTCP端口

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 启动音视频解码消息 */
class CRDAStartBusinessReq : public CRDAMsg
{
public:
    CRDAStartBusinessReq( );
    ~CRDAStartBusinessReq( ){};
public:
    RDA_UINT4 m_ulBusinessID; //业务ID
    RDA_STRING_ID m_szDecoderID;  //解码器ID（20位，不包括\0）
    RDA_UINT2 m_usChannelNo;  //通道编号
    RDA_UCHAR m_ucTransportProtocol;  //视频传输协议
    RDA_STRING m_szMediaIP;            //解码器的媒体通道IP
    RDA_UINT2 m_usTcpInterleavedPort; //TCP交织端口
    RDA_UINT2 m_usUdpVideoRtpPort;    //UDP视频RTP端口
    RDA_UINT2 m_usUdpVideoRtcpPort;   //UDP视频RTCP端口
    RDA_UINT2 m_usUdpAudioRtpPort;    //UDP音频RTP端口
    RDA_UINT2 m_usUdpAudioRtcpPort;   //UDP音频RTCP端口
    RDA_UCHAR m_ucVideoPayloadType;   //视频Payload类型
    RDA_UCHAR m_ucAudioPayloadType;   //音频Payload类型
    RDA_STRING m_szSessionId;          //会话ID
    RDA_STRING m_szSessionUrl;         //会话URL

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs();
};

/* 启动解码消息的响应消息 */
class CRDAStartBusinessRsp : public CRDAMsg
{
public:
    CRDAStartBusinessRsp();
    ~CRDAStartBusinessRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 停止音视频解码消息 */
class CRDAStopBusinessReq : public CRDAMsg
{
public:
    CRDAStopBusinessReq( );
    ~CRDAStopBusinessReq( ){};
public:
    RDA_UINT4 m_ulBusinessID; //业务ID
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
    RDA_UINT2 m_usChannelNo; //通道编号
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 停止音视频解码消息 */
class CRDAStopBusinessRsp : public CRDAMsg
{
public:
    CRDAStopBusinessRsp();
    ~CRDAStopBusinessRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //业务ID
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 播放/停止播放音频消息 */
class CRDAAudioCtrolReq : public CRDAMsg
{
public:
    CRDAAudioCtrolReq( );
    ~CRDAAudioCtrolReq( ){};
public:
    RDA_UINT4 m_ulBusinessID; //业务ID
    RDA_STRING_ID m_szDecoderID; //解码器ID
    RDA_UINT2 m_usChannelNo; //通道编号
    RDA_UCHAR m_ucAudioCmd; //0x00 停止播放; 0x01开始播放

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 播放/停止播放音频消息的响应消息 */
class CRDAAudioCtrolRsp : public CRDAMsg
{
public:
    CRDAAudioCtrolRsp();
    ~CRDAAudioCtrolRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 解码器的音量调解消息 */
class CRDATuneupVolumeReq : public CRDAMsg
{
public:
    CRDATuneupVolumeReq( );
    ~CRDATuneupVolumeReq( ){};
public:
    RDA_UINT4 m_ulBusinessID; //业务ID
    RDA_STRING_ID m_szDecoderID;  //解码器ID（20位，不包括\0）
    RDA_UINT2 m_usChannelNo;  //通道编号
    RDA_UCHAR m_ucVolume;     //音量大小（0~100）
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 音量调节消息的响应消息 */
class CRDATuneupVolumeRsp : public CRDAMsg
{
public:
    CRDATuneupVolumeRsp();
    ~CRDATuneupVolumeRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 设置远程解码器显示模式请求消息 */
class CRDASetRDDisplayModeReq : public CRDAMsg
{
public:
    CRDASetRDDisplayModeReq();
    ~CRDASetRDDisplayModeReq( ){};
public:
    RDA_UINT4 m_ulBusinessID; //业务ID
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
    RDA_UINT2 m_usChannelNo; //通道编号
    RDA_UINT2 m_usMode; //显示模式
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 设置远程解码器显示模式响应消息 */
class CRDASetRDDisplayModeRsp : public CRDAMsg
{
public:
    CRDASetRDDisplayModeRsp();
    ~CRDASetRDDisplayModeRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 解码器注销消息 */
class CRDAUnregisterReq : public CRDAMsg
{
public:
    CRDAUnregisterReq( );
    ~CRDAUnregisterReq( ){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 音量调节消息的响应消息 */
class CRDAUnregisterRsp : public CRDAMsg
{
public:
    CRDAUnregisterRsp();
    ~CRDAUnregisterRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 控制台关闭消息 */
class CRDAConsoleShutdownReq : public CRDAMsg
{
public:
    CRDAConsoleShutdownReq( );
    ~CRDAConsoleShutdownReq( ){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 控制台关闭消息 */
class CRDAConsoleShutdownRsp : public CRDAMsg
{
public:
    CRDAConsoleShutdownRsp();
    ~CRDAConsoleShutdownRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 错误上报消息 */
class CRDAErrorNotifyReq : public CRDAMsg
{
public:
    CRDAErrorNotifyReq( );
    ~CRDAErrorNotifyReq( ){};
public:
    RDA_UINT4 m_ulBusinessID; //业务ID
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
    RDA_UINT2 m_usChannelNo; //通道编号
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs( );
};

/* 错误上报的响应消息 */
class CRDAErrorNotifyRsp : public CRDAMsg
{
public:
    CRDAErrorNotifyRsp();
    ~CRDAErrorNotifyRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID
    RDA_UINT4 m_ulErrorCode; //返回码，0代表成功
    RDA_STRING m_szErrorDescription; //错误码对应错误描述

protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 心跳请求消息 */
class CRDAHeartBeatReq : public CRDAMsg
{
public:
    CRDAHeartBeatReq();
    ~CRDAHeartBeatReq(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

/* 心跳响应消息 */
class CRDAHeartBeatRsp : public CRDAMsg
{
public:
    CRDAHeartBeatRsp();
    ~CRDAHeartBeatRsp(){};
public:
    RDA_STRING_ID m_szDecoderID; //解码器ID（20位，不包括\0）
protected:

    /**************************************************************************
    * name       : RegisterAttrs
    * description: 注册字段到list中
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    virtual LONG RegisterAttrs(void);
};

#endif //RDAPROT_H
