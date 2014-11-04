/********************************************************************
    filename	: 	CumwTypes.h
    author		:	wx58056
    created		:	2011/10/28
    description	:	定义所有中间件内部类型
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/10/28 初始版本
 *********************************************************************/

#ifndef CUMWT_YPES_H
#define CUMWT_YPES_H

#include <list>
using namespace std;

//参数的输入/输出属性
#ifndef IN
 #define IN
#endif

#ifndef OUT
 #define OUT
#endif

#ifndef IO
 #define IO
#endif

#pragma pack(push,1)

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之常量定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    const char PRODUCT_NAME[] = "Huawei CU";      //产品名称
    const char PRODUCT_VERSION[]  = "V100R002C01";         //产品版本号
    const char FILE_PATH_CONFIG[] = "conf";           //配置文件相对路径
    const char FILE_NAME_INI_CONFIG[] = "conf\\CUMW.ini"; //配置文件名
    const char FILE_PATH_LOG[] = "logs";           //日志文件相对路径
    const char FILE_NAME_LOG[] = ".log";           //日志文件名后缀
    //BEGIN R002C01SPC100 DEL 2011-07-15 ligengqiang 00124479 for Purify检查问题
    //const char FILE_NAME_CRASH_DLL[]    = "XCPTHLR.DLL";    //捕获异常DLL名称
    //END   R002C01SPC100 DEL 2011-07-15 ligengqiang 00124479 for Purify检查问题

    const unsigned long FILE_PATH_LEN = 256;      //文件路径最大长度

    const unsigned long CONN_MGR_SELECT_PERIOD = 10;   //连接管理器的Select周期
    const unsigned long TIMER_INTERVAL = 100;  //定时器间隔
    const unsigned long TIMER_SCALES_SECOND = 10;   //1秒的scale计数
    const unsigned long INTERVAL_DECODER_CONN_TIME_OUT = 15;   //解码器连接超时时长，单位为秒

    const unsigned long WAIT_FOR_THREAD_EXIT_TIME = 30000;    //线程退出等待时间，单位：毫秒
    const unsigned long EVENT_SYNC_QUEUE_NUM = 1024;     //事件同步队列的个数

    const unsigned long HEARTBEAT_SEND_INTERVAL = 60;   //心跳发送间隔, 60秒
    const unsigned long ASYNC_TIMEOUT_CHECK_INTERVAL = 10;   //异步超时检查间隔,秒
    const unsigned long TRY_REGISGER_INTERVAL = 60;   //重试register 定时器间隔 ，秒

    const unsigned long HEARTBEAT_TIMEOUT_COUNT = 3;     //心跳超时计数
    const unsigned long ASYNC_MAX_RESEND_COUNT = 0;     //重发最大次数

    const unsigned long ASYNC_TIMEOUT_COUNT = 20 / ASYNC_TIMEOUT_CHECK_INTERVAL; //异步响应超时计数5s
    const unsigned long ASYNC_KEEP_ALIVE_COUNT = ASYNC_TIMEOUT_COUNT * 10; //异步消息最大生命周期

    const unsigned long MAX_VOLUME = 100;   //音量最大值

    const unsigned long MAX_LEN_SESSION_ID = 32;    //媒体会话ID的最大长度
}

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之数组及其引用类型定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    typedef char PATH_ARRAY[FILE_PATH_LEN];               //路径数组类型
    typedef char (&PATH_ARRAY_REF)[FILE_PATH_LEN];        //路径数组引用类型
    typedef const char (&PATH_CARRAY_REF)[FILE_PATH_LEN]; //路径常量数组引用类型

    //业务ID列表定义
    typedef list <unsigned long> LIST_BUSINESS_ID;
}

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之枚举类型定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    ///////////////////////////////////////////////////////////////////////////
    /// @brief 信令协议栈消息状态
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    enum PACK_STATUS
    {
        PACK_STATUS_INIT,           ///< 消息初始化
        PACK_STATUS_SENDING,        ///< 消息发送中
        PACK_STATUS_SENDED,         ///< 消息已发送
        PACK_STATUS_FIN             ///< 消息发送结束
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 事件类型
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    enum EVENT_TYPE
    {
        //通用事件类型
        EVENT_TYPE_INVALID,                     ///< 无效事件
        EVENT_TYPE_EXIT_THREAD,                 ///< 退出线程。对应的结构体为NULL
        EVENT_TYPE_GENERAL_ASYN,                ///< 通用异步事件。对应的结构体为@ref CUMW::GENERAL_ASYN_INFO

        //ControlBus的事件类型
        EVENT_TYPE_CTRLBUS_BUSINESS            ///< 媒体业务事件。对应的结构体为@ref CControlBus::BUSINESS_ASYNC_INFO
    };

    //事件类型的显示字符串最大长度
    const unsigned long STRING_LEN_EVENT_TYPE = sizeof("Event Type is Check Asynchronous Time Out");

    //事件类型的显示字符串数组定义
    const char STR_ARR_EVENT_TYPE[][STRING_LEN_EVENT_TYPE]
    = {
        "Event Type is Invalid",
        "Event Type is Exit Thread",
        "Event Type is General Asynchronous Event",
        "Event Type is Media Business",
        "Event Type is Logout",
        "Event Type is Send Message",
        "Event Type is Send Response",
        "Event Type is Receive Response Message",
        "Event Type is Receive Request Message",
        "Event Type is Handle ReLogin Event",
        "Event Type is Check Asynchronous Time Out", //STRING_LEN_EVENT_TYPE为该字符串的长度
        "Event Type is Dispatch Stream Data",
        "Event Type is Insert Data Buffer"
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 业务控制码
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    enum BUSINESS_CTRL_CODE
    {
        BUSINESS_CTRL_CODE_INVALID = 0x00000000,   ///< 无效控制码

        BUSINESS_CTRL_CODE_MASK_INGRESS = 0x0000FFFF,   ///< Ingress掩码
        BUSINESS_CTRL_CODE_MASK_EGRESS = 0xFFFF0000,   ///< Egress掩码

        BUSINESS_CTRL_CODE_E_LOCAL_AUDIO = 0x00010000,   ///< Egress的本地音频控制。对应的vCtrlPara为@ref IPI_CTRL_LOCAL_AUDIO
        //BUSINESS_CTRL_CODE_E_CAMPTURE_IMAGE = 0x00020000,   ///< Egress的抓图。对应的vCtrlPara为@ref IPI_CAPTURE_IMAGE
        BUSINESS_CTRL_CODE_I_LOCAL_AUDIO = 0x00000001   ///< Ingress的本地音频控制。对应的vCtrlPara为@ref IPI_CTRL_LOCAL_AUDIO
        //BUSINESS_CTRL_CODE_I_RECORD_PLAY    = 0x00000002,   ///< Ingress的录像回放控制。对应的vCtrlPara为@ref IPI_RECORD_PLAY_CTRL
        //BUSINESS_CTRL_CODE_I_RE_INVITE      = 0x00000003,   ///< Ingress的实时浏览reinvite。对应的vCtrlPara为@ref CUMW::NOTIFY_INVITE
        //BUSINESS_CTRL_CODE_I_RE_INVITE_ACK  = 0x00000004,   ///< Ingress的实时浏览reinviteAck。对应的vCtrlPara为空
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief PayloadType定义
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    enum PAY_LOAD_TYPE
    {
        PAY_LOAD_TYPE_PCMU   = 0,   ///< G711的u率
        PAY_LOAD_TYPE_G723   = 4,   ///< G723
        PAY_LOAD_TYPE_PCMA   = 8,   ///< G711的a率
        PAY_LOAD_TYPE_G722   = 9,   ///< G722
        PAY_LOAD_TYPE_MJPEG  = 24,  ///< MJPEG
        PAY_LOAD_TYPE_AMR_MB = 97,   ///< AMR_NB
        PAY_LOAD_TYPE_MPEG4  = 98,  ///< MPEG4
        PAY_LOAD_TYPE_H264 = 99,   ///< H264
        PAY_LOAD_TYPE_AVS  = 100,   ///< AVS
        PAY_LOAD_TYPE_MP3  = 101,   ///< MP3
        PAY_LOAD_TYPE_3GPP = 102  ///< 3GPP
    };
}

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之接口中枚举类型的对应的常量数组定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    //连接模式的显示字符串最大长度
    const unsigned long STRING_LEN_LINK_MODE = sizeof("Link Mode is Transmit Through MDU");

    //连接模式的显示字符串数组定义，参见CUMW_LINK_MODE的定义
    const char STR_ARR_LINK_MODE[][STRING_LEN_LINK_MODE]
    = {
        "Link Mode is Transmit Through MDU", //STRING_LEN_LINK_MODE为该字符串的长度
        "Link Mode is Direct To PU",
    };

    //协议类型的显示字符串最大长度
    const unsigned long STRING_LEN_PROTOCOL_TYPE = sizeof("Protocol Type is UDP");

    //协议类型的显示字符串数组定义，参见CUMW_PROTOCOL_TYPE定义
    const char STR_ARR_PROTOCOL_TYPE[][STRING_LEN_PROTOCOL_TYPE]
    = {
        "Protocol Type is UDP", //STRING_LEN_PROTOCOL_TYPE为该字符串的长度
        "Protocol Type is TCP",
    };

    //协议类型的显示字符串最大长度
    const unsigned long STRING_LEN_PLAY_TYPE = sizeof("Play Type is Remote Video Direct Connect to MDU");

    //协议类型的显示字符串数组定义，参见CUMW_PLAY_TYPE定义
    const char STR_ARR_PLAY_TYPE[][STRING_LEN_PLAY_TYPE]
    = {
        "Play Type is Local Video",
        "Play Type is Remote Video",
        "Play Type is Local Preview Of Remote Video",
        "Play Type is Video Data Call Back",
        "Play Type is Remote Video Direct Connect to MDU", //STRING_LEN_PLAY_TYPE为该字符串的长度
    };

    //异步类型的显示字符串最大长度CUMW_ASYNC_TYPE
    const unsigned long STRING_LEN_ASYNC_TYPE = sizeof("Async Type is Handle Signal Control Info");

    //异步类型的显示字符串数组定义
    const char STR_ARR_ASYNC_TYPE[][STRING_LEN_ASYNC_TYPE]
    = {
        "Async Type is NON Return Info",
        "Async Type is User Login Register",
        "Async Type is Query Plat Info",
        "Async Type is Query User Info",
        "Async Type is Handle Signal Control Info", //STRING_LEN_EVENT_TYPE为该字符串的长度
        "Async Type is Start Media Business",
    };

    //音频设备类型的显示字符串最大长度
    const unsigned long STRING_LEN_AUDIO_TYPE = sizeof("Audio Type is Audio Out");

    //协议类型的显示字符串数组定义，参见CUMW_PLAY_TYPE定义
    const char STR_ARR_AUDIO_TYPE[][STRING_LEN_AUDIO_TYPE]
    = {
        "Audio Type is Audio Out", //STRING_LEN_AUDIO_TYPE为该字符串的长度
        "Audio Type is Audio In",
    };

    //音频设备类型的显示字符串最大长度
    const unsigned long STRING_LEN_AUDIO_CTRL_CODE = sizeof("Audio Control Code is Set Volume");

    //协议类型的显示字符串数组定义，参见CUMW_PLAY_TYPE定义
    const char STR_ARR_AUDIO_CTRL_CODE[][STRING_LEN_AUDIO_CTRL_CODE]
    = {
        "Audio Control Code is Close",      //STRING_LEN_AUDIO_TYPE为该字符串的长度
        "Audio Control Code is Open",
        "Audio Control Code is Set Volume",
    };
}

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之异步返回相关定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    ///////////////////////////////////////////////////////////////////////////
    /// @brief 媒体端口信息
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    struct PORT_INFO
    {
        unsigned short usInterleavedPort;   ///< 交织端口
        unsigned short usVideoRtpPort;      ///< 视频RTP端口
        unsigned short usVideoRtcpPort;     ///< 视频RTCP端口
        unsigned short usAudioRtpPort;      ///< 音频RTP端口
        unsigned short usAudioRtcpPort;     ///< 音频RTCP端口
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 服务端媒体信息
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    struct SERVER_MEDIA_INFO
    {
        char          szSessionId[MAX_LEN_SESSION_ID + 1];      ///< 会话ID
        char          szServerIp[IVS_IP_LEN];      ///< 媒体服务器IP
        PORT_INFO     stServerPortInfo;             ///< 媒体服务器的端口信息
        PAY_LOAD_TYPE enVideoPt;                  ///< 视频PT
        PAY_LOAD_TYPE enAudioPt;                  ///< 音频PT
        char          szSessionUrl[CUMW_PATH_LEN];  ///< 会话URL
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 异步接口类型,其他见CUMW_ASYNC_TYPE
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    enum ASYNC_TYPE
    {
        ASYNC_TYPE_INTERFACE_MAX = 0x8000,  ///<接口最大值

        ASYNC_TYPE_HEART_BEAT,              ///<心跳
        ASYNC_TYPE_LOGOUT,                  ///<注销
        ASYNC_TYPE_USER_INFO,               ///<异步类型用户信息
        ASYNC_TYPE_PLAT_INFO,               ///<异步类型平台信息
        ASYNC_TYPE_SIGNAL_CTRL,             ///<异步类型信令控制

        ASYNC_TYPE_MEDIA_CHANNEL_OPEN,      ///< 打开媒体通道。对应的pRetInfo结构体为@ref CUMW::ASYNC_RET_MEDIA_CHANNEL_OPEN
        ASYNC_TYPE_MEDIA_CHANNEL_CLOSE,     ///< 关闭媒体通道。对应的pRetInfo结构体为NULL
        ASYNC_TYPE_BUSINESS_START,          ///< 业务开始
        ASYNC_TYPE_BUSINESS_RUN,            ///< 业务运行
        ASYNC_TYPE_BUSINESS_STOP_INTERFACE, ///< 接口触发的业务停止
        ASYNC_TYPE_BUSINESS_STOP_INNER,     ///< 内部触发的业务停止
        ASYNC_TYPE_BUSINESS_CTRL,           ///< 业务控制
        ASYNC_TYPE_BUSINESS_INVITE,         ///< invite
        ASYNC_TYPE_BUSINESS_INVITE_ACK,     ///< invite Ack

        ASYNC_TYPE_TV_WALL_SVR_START,       ///< 启动电视墙服务
        ASYNC_TYPE_TV_WALL_SVR_STOP,        ///< 停止电视墙服务

        ASYNC_TYPE_DECORDER_INIT,           //解码器初试化
        ASYNC_TYPE_DECORDER_START,          //启动解码器
        ASYNC_TYPE_DECORDER_STOP,          //停止解码器
        ASYNC_TYPE_AUDIO_CONTROL,           //播放/停止播放音频消息
        ASYNC_TYPE_TUNEUP_VOLUME           //音量调节消息
    };

    //收到的消息体
    typedef CUMW_NOTIFY_ASYNC_RET RECEIVE_MSG;

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 打开媒体通道的异步返回
    ///
    /// Description.    直接返回服务端媒体信息
    ///////////////////////////////////////////////////////////////////////////
    typedef SERVER_MEDIA_INFO ASYNC_RET_MEDIA_CHANNEL_OPEN;
}

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之结构体定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    ///////////////////////////////////////////////////////////////////////////
    /// @brief 事件信息
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    struct EVENT_INFO
    {
        EVENT_TYPE      enEventType;        ///< 事件类型
        void*           pEventInfo;         ///< 事件信息指针，其意义取决于ulEventType
        unsigned long   ulEventInfoLen;     ///< 事件信息内容长度，校验用

        //构造函数
        EVENT_INFO(EVENT_TYPE enType = EVENT_TYPE_INVALID, void* pInfo = NULL, unsigned long ulInfoLen = 0)
        {
            enEventType = enType;
            pEventInfo = pInfo;
            ulEventInfoLen = ulInfoLen;
        }
    };

    //事件同步队列
    typedef CSyncQueue <EVENT_INFO> CEventSyncQueue;

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 业务控制参数
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    struct BUSINESS_CTRL_PARA
    {
        BUSINESS_CTRL_CODE enCtrlCode;     ///< 业务控制码
        unsigned long      ulCtrlParaLen;   ///< 控制参数长度，即vCtrlPara的长度
        char               vCtrlPara[1];    ///< 控制参数值，可变数组
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 打开媒体通道参数
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    struct OPEN_MEDIA_CHANNEL_PARA
    {
        unsigned long ulAsyncSeq;         ///< 异步序号
        unsigned long ulBusinessId;       ///< 业务ID
        unsigned long ulBusinessSrcType;  ///< 业务来源类型
        unsigned long ulLinkMode;         ///< 连接模式
        unsigned long ulProtocolType;     ///< 媒体通道协议类型。定义参见CUMW_PROTOCOL_TYPE
        PORT_INFO     stLocalPortInfo;      ///< 本地端口信息
        char          szLocalIp[IVS_IP_LEN];   ///< 本地IP
        char          szCameraId[CUMW_DEV_ID_LEN + 1];          ///< 镜头ID。ulBusinessType为实时浏览时有效
        unsigned long ulStreamId;                             ///< 码流ID。ulBusinessType为实时浏览时有效
        char          szContentId[CUMW_CONTENT_ID_LEN + 1];     ///< 内容ID。ulBusinessType为录像回放下载、音频广播时有效
        unsigned long ulStartTime;        ///< 开始时间
        unsigned long ulEndTime;          ///< 结束时间
        char          szDevID[CUMW_DEV_ID_LEN + 1];             ///< 镜头所属设备ID, 包含字符串结束符
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 关闭媒体通道参数
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    struct CLOSE_MEDIA_CHANNEL_PARA
    {
        unsigned long ulBusinessId;       ///< 业务ID
        //媒体会话ID
        char szSessionId[CUMW::MAX_LEN_SESSION_ID + 1];
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 通用的异步信息参数
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    struct GENERAL_ASYN_INFO
    {
        unsigned long    ulAsynSeq;   //异步序号
        CUMW::ASYNC_TYPE enAsynType;  //异步类型

        GENERAL_ASYN_INFO()
        {
            ulAsynSeq  = 0;
            enAsynType = CUMW::ASYNC_TYPE_BUSINESS_START;
        }
    };
}

#pragma pack(pop)

#endif //CUMWT_TYPES_H
