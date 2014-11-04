/********************************************************************
    filename	: 	CumwInterface.h
    author		:	wx58056
    created		:	2011/10/28
    description	:	类型定义
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/10/28 初始版本
 *********************************************************************/
#ifndef CUMW_INTERFACE_H
#define CUMW_INTERFACE_H


#pragma pack(push,1)


////******************************接口相关常量定义开始*************************
#define CUMW_DEV_ID_LEN              20   //设备ID的长度
#define CUMW_DEV_NAME_LEN            256  //设备名的最大长度

#define CUMW_CONTENT_ID_LEN          256  //内容ID的最大长度
#define CUMW_PATH_LEN                256  //路径的最大长度

#define CUMW_MSG_TYPE_LEN            32  //消息类型的最大长度
#define CUMW_MSG_DST_CODE_LEN        10  //监视器编码最大长度

//媒体连接模式
typedef enum
{
    CUMW_LINK_MODE_TRANSMIT = 0,  //经过MDU中转
    CUMW_LINK_MODE_DIRECT = 1,    //直连

    CUMW_LINK_MODE_MAX           //最大值，校验用
} CUMW_LINK_MODE;


//媒体播放类型
typedef enum
{
    CUMW_PLAY_TYPE_LOCAL   = 0,              //本地显示
    CUMW_PLAY_TYPE_REMOTE  = 1,              //远程上墙显示
    CUMW_PLAY_TYPE_PREVIEW = 2,              //远程上墙预览
    CUMW_PLAY_TYPE_REMOTE_DIRECT_CONNECT = 4,//远程上墙直连视频源，可以是MDU或PU

    CUMW_PLAY_TYPE_MAX                      //最大值，校验用
} CUMW_PLAY_TYPE;


//音频设备类型
typedef enum
{

    CUMW_AUDIO_TYPE_OUT = 0,   //音频输出
    CUMW_AUDIO_TYPE_IN = 1,    //音频输入

    CUMW_AUDIO_TYPE_MAX       //最大值，校验用
} CUMW_AUDIO_TYPE;


//音频控制码
typedef enum
{
    CUMW_AUDIO_CTRL_CODE_CLOSE = 0,       //关闭音频设备
    CUMW_AUDIO_CTRL_CODE_OPEN = 1,        //打开音频设备
    CUMW_AUDIO_CTRL_CODE_SET_VOLUME = 2,  //调节音量

    CUMW_AUDIO_CTRL_CODE_MAX             //最大值，校验用
} CUMW_AUDIO_CTRL_CODE;


//音量范围值定义
typedef enum
{
    CUMW_AUDIO_VOLUME_MIN = 0,    //音量最小值
    CUMW_AUDIO_VOLUME_MAX = 100   //音量最大值
} CUMW_AUDIO_VOLUME;


////******************************接口相关常量定义结束*************************

////******************************通用结构体定义开始***************************


//结构体数组信息
typedef struct
{
    unsigned long   ulArraySize;    //数组元素个数
    void*           pArrayAddr;     //指向某特定结构体的数组的指针，数组元素个数为ulNum
    unsigned long   ulArrayLen;     //pArrayAddr内容的实际大小
} CUMW_STRUCT_ARRAY;




////******************************通用结构体定义开始***************************

////******************************通知回调相关定义开始*************************

//异步接口类型
typedef enum
{
    CUMW_ASYNC_TYPE_NO_RET_INFO = 0x0000,         //无返回信息的异步接口
    CUMW_ASYNC_TYPE_START_MEDIA_BUSINESS = 0x0001,//开始媒体业务异步接口
                                                  //对应信息为: 无。业务ID信息通过ulBusinessID字段返回

    CUMW_ASYNC_TYPE_INTERFACE_MAX = 0x8000        //接口最大值
} CUMW_ASYNC_TYPE;



//通用处理响应上报结构体信息
typedef struct
{
    ULONG            ulMsgSeq;                            //消息序列号
    ULONG            ulResultCode;                        //错误码，即返回码
    char             pMsgType[CUMW_MSG_TYPE_LEN];         //消息类型，字符串
    char             pDstCode[CUMW_MSG_DST_CODE_LEN];     //pArrayAddr内容的实际大小
} COMMON_RETURN_STRUCT; 


//远程解码器状态通知
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_MEDIA_DATA
typedef struct
{
    unsigned long   ulBusinessID;   //业务ID
    unsigned long   ulDataType;     //媒体数据类型
    char*           pDataBuf;       //指向媒体数据的指针
    unsigned long   ulDataBufLen;   //媒体数据（即pDataBuf所指内容）的长度
    unsigned long   ulVideoWidth;   //视频宽度
    unsigned long   ulVideoHeight;  //视频高度
} CUMW_NOTIFY_MEDIA_DATA;

//远程解码器注册通知
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER
typedef struct
{
    char*  pszDecoderInfo;          //解码器注册信息，这是XML格式
    unsigned long ulLen;            //数据长度
} CUMW_NOTIFY_REMOTE_DECODER_REGISTER_INFO;

////******************************通知回调相关定义结束*************************

////******************************接口结构体相关定义开始***********************


//异步接口参数公共头
typedef struct
{
    unsigned long ulStructLen;    //结构体本身长度
    unsigned long ulAsyncSeq;     //异步调用序号
} IPI_ASYNC_HEADER;


//初始化中间件接口参数
typedef struct
{
    unsigned long           ulClientType;       //客户端类型，参见CUMW_CLIENT_TYPE
    PCUMW_CALLBACK_NOTIFY   pFunNotifyCallBack; //通知回调函数指针
    void*                   pCallBackUserData;  //通知回调函数的用户数据指针
} IPI_INIT_CUMW;


//控制本地音频
typedef struct
{
    unsigned long ulAsyncSeq;     //异步调用序号
    unsigned long ulAudioType;    //音频类型，定义参见CUMW_AUDIO_TYPE
    unsigned long ulCtrlCode;     //音频控制码，定义参见CUMW_AUDIO_CTRL_CODE
    unsigned long ulVolume;       //音量，取值范围为0-100，即从0%到100%。ulCtrlCode为调节音量（参见音频控制码）时有效
} IPI_CTRL_LOCAL_AUDIO;

////******************************接口结构体相关定义结束***********************


#pragma pack(pop)


#endif //CUMW_INTERFACE_H
