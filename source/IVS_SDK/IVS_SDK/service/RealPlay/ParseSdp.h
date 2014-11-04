/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_MLIB_Player
//  文 件 名：  IVS_Sdp.h
//  文件说明：  SDP解析函数
//  版      本： eIVS V100R001C30
//  作      者： yangjunhai/y00182336
//  创建日期：2012/08/14
//
//  修改标识：
//  修改说明：
*******************************************************************************/
#ifndef _IVS_SDP_H_
#define _IVS_SDP_H_
#include "SDKDef.h"


// 定义SDP相关关键字
#define SDP_MEDIA_TYPE_AUDIO         "audio"                 
#define SDP_MEDIA_TYPE_VIDEO         "video"
#define SDP_MEDIA_SYPE_APPLICATION   "application"
#define SDP_MEDIA_STAR_TIME			 "StartTime:"
#define SDP_MEDIA_END_TIME			 "StopTime:"
#define SDP_SESSION_ID               "SessionID:"
#define SDP_ATTRIBUTES_CRYPTO        "crypto:"
#define SDP_ATTRIBUTES_CRYPTOSTORAGE "videokeycipher:"
#define SDP_CER_INDEX                "cerindex:"
#define SDP_RTP_MAP                  "rtpmap:"
#define SDP_ATTRIBUTES_INLINE        "inline:"
#define SDP_ATTRIBUTES_CONTROL       "control:"
#define SDP_ATTRIBUTES_DECODER_TYPE  "decoder-type="
#define SDP_VIDEO_H264_TYPE			 "H264"
#define SDP_VIDEO_MJPEG_TYPE		 "JPEG"
#define SDP_AUDIO_PCMU_TYPE			 "PCMU"
#define SDP_AUDIO_PCMA_TYPE		     "PCMA"
#define IVS_SECRET_KEY_TYPE_SIMPLE					"HW_IVS_IPC_ENCRYPT_SIMPLE"		//扰码流(密文头)
#define IVS_SECRET_KEY_TYPE_SIMPLE_HEADER_PLAIN		"HW_IVS_IPC_ENCRYPT_SIMPLE_HEADER_PLAIN" //扰码流(明文头)
#define IVS_SECRET_KEY_TYPE_AES256					"HW_IVS_IPC_ENCRYPT_AES256_HEADER_PLAIN"		//AES256(明文头)
#define IVS_SECRET_KEY_TYPE_AES256_PARTIAL			"HW_IVS_IPC_ENCRYPT_AES256_PARTIAL_HEADER_PLAIN"//AES-256部分加密
#define IVS_SECRET_STORAGE_KEY_TYPE  "ASYM_RSA_2048_AES_CM_256"
#define RTP_INTERLEAVE_LENGTH               4              // 定义交织头长度
#define MAX_LEN_SESSION_URL                 512            // 定义URL长度
#define KEY_MAX_LEN							512             // 定义密钥最大长度
#define MAX_LEN_SESSION_ID                  32             // 定义SessionID长度
#define KEY_TYPE_VIDEO                      1              // 定义视频密钥类型
#define KEY_TYPE_AUDIO                      2              // 定义音频密钥类型
#define SDP_TIME_LEN						14			   // 定义时间长度

// 媒体类型
#define PLAYER_MEDIA_TPYE_AUDIO     1        // 音频
#define PLAYER_MEDIA_TPYE_VIDEO     0        // 视频

// 跳过space宏
#define JUMP_SPACE(psz) {                                                   \
	while (isspace(*(psz)) && ('\0' != *(psz)))     \
							{                                               \
							(psz)++;                                    \
							}                                               \
						}


// 定义解密对象，包简单加密，帧简单加密还是帧复杂加密
typedef enum
{
	HW_NO_DEC_TYPE = -1,
	HW_IPC_ENCRYPT_SIMPLE = 0,
	HW_IPC_ENCRYPT_SIMPLE_HEADER = 1,
	HW_IPC_ENCRYPT_AES256 = 2,
	HW_IPC_ENCRYPT_AES256_PARTIAL = 3
} SECRET_TYPE_SDP;

//媒体信息
typedef struct _stMEDIA_INFO_SDP
{
	char    SessionUrl[MAX_LEN_SESSION_URL + 1];      ///< 媒体会话URL
	char    SessionId[MAX_LEN_SESSION_ID + 1];        ///< 媒体会话ID
	IVS_ULONG   port;                                       ///< 本地IP
	IVS_ULONG   ManuType;
	IVS_ULONG   VideoDecoderType;
	IVS_ULONG   AudioDecoderType;
}MEDIA_INFO_SDP;

//存储加密信息
typedef struct _stSTORAGESECRET_INFO
{
	IVS_ULONG	keyTpye;
	char		keyValue[KEY_MAX_LEN];
	IVS_ULONG	cerIndex;
}STORAGESECRET_INFO;

//媒体加密信息
typedef struct _stMEDIA_KEY_INFO
{
	IVS_UINT32 uPTVideo;    // payload value of video  add by w00210470
	IVS_UINT32 uPTAudio;    // payload value of auido 

	IVS_INT32 iVideoKeyType;
	IVS_VIDEO_DEC_TYPE enVideoDecoderType; //视频码流类型
	IVS_INT32 iAudioKeyType;
	IVS_AUDIO_DEC_TYPE enAudioDecoderType; //音频类型
	IVS_CHAR  szStarTime[IVS_TIME_LEN];
	IVS_CHAR  szEndTime[IVS_TIME_LEN];
	IVS_CHAR  szSessionID[SESSION_ID_LEN];
}MEDIA_KEY_INFO;

/***************************************************************************
 函 数 名 : parseSdp
 功能描述 : 解析SDP
 输入参数 : pSdpMsg                 待解析的字符串
 输出参数 : pMediaKeyInfo           媒体密钥信息
            pMediaInfo              获取到的媒体编码类型与第三方SDK相对路径
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
IVS_INT32 parseSdp(const char* pSdpMsg, MEDIA_KEY_INFO* pMediaKeyInfo,
						MEDIA_INFO_SDP* pMediaInfo,STORAGESECRET_INFO* pStorageSecretInfo);


/***************************************************************************
 函 数 名 : getNextLine
 功能描述 : 切分SDP消息
 输入参数 : strSdpMsg                   待解析的字符串
            nNextStart                  偏移值
 输出参数 : strBuff                     获取到的下一行SDP字符串
 返 回 值 : true 成功           fail 失败
***************************************************************************/
bool getNextLine(const std::string &strSdpMsg,
                 unsigned int &nNextStart,
                 std::string  &strBuff);


/***************************************************************************
 函 数 名 : parseApplications
 功能描述 : 解析媒体类型，获取TCP端口号
 输入参数 : pAppliMsg                   待解析的字符串
 输出参数 : pPort                       获取到的TCP端口号
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseApplications(const char* pAppliMsg, unsigned long* pPort);


/***************************************************************************
 函 数 名 : parseDecoderType
 功能描述 : 解析编码类型
 输入参数 : pTmpMsg                     待解析的字符串
 输出参数 : pDecoderType                获取到的编码类型
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseDecoderType(const char* pTmpMsg, IVS_ULONG* pDecoderType);



/***************************************************************************
 函 数 名 : parseStreamID
 功能描述 : 解析SessionID
 输入参数 : pSdpMsg                     待解析的字符串
 输出参数 : pSessionId                  获取到的SessionID
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseStreamID(const char* pSdpMsg,  char* pSessionId);


/***************************************************************************
 函 数 名 : parseManuType
 功能描述 : 解析厂商类型
 输入参数 : pTmpMsg                     待解析的字符串
 输出参数 : pManuType                   获取到的厂商类型
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseManuType(char* pTmp, IVS_ULONG* ulManuType);


/***************************************************************************
 函 数 名 : parseTransSecretKey
 功能描述 : 解析加密类类型与密钥
 输入参数 : pTmpMsg                     待解析的字符串
            ulKeyType                   媒体类型
输出参数 : pMediaKeyInfo           媒体密钥信息
返 回 值 : IVS_SUCCEED-成功           其他-失败

字符串类型：
 //a=crypto:1 HW_IVS_IPC_ENCRYPT_SIMPLE inline:d0RmdmcmVCspeEc3QGZiNWpVLFJhQX1cfHAwJSoj
***************************************************************************/
int parseTransSecretKey(const char* pTmp, IVS_ULONG ulKeyType, MEDIA_KEY_INFO* pMediaKeyInfo);

/***************************************************************************
 函 数 名 : parseRtpMap
 功能描述 : 解析音视频类型
 输入参数 : pTmp                     待解析的字符串
 输出参数 : pMediaKeyInfo            获取到的视频、音频流类型
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseRtpMap(const char* pTmp,MEDIA_KEY_INFO* pMediaKeyInfo);

/***************************************************************************
 函 数 名 : parseStorageSecretKey
 功能描述 : 解析存储加密密钥
 输入参数 : pTmpMsg                     待解析的字符串           
 输出参数 : pStorageSecretInfo          获取到存储加密信息
 返 回 值 : IVS_SUCCEED-成功           其他-失败

 字符串类型：
 a=cryptostorage:1 ASYM_RSA_2048_AES_CM_256 inline:加密码流的Key的密文 
***************************************************************************/
int parseStorageSecretKey(const char* pTmp,STORAGESECRET_INFO* pStorageSecretInfo);

/***************************************************************************
 函 数 名 : parseCerIndex
 功能描述 : 解析存储加密证书号
 输入参数 : pTmpMsg                     待解析的字符串           
 输出参数 : pStorageSecretInfo          获取到存储加密信息
 返 回 值 : IVS_SUCCEED-成功           其他-失败

 字符串类型：
 b=cerindex:证书索引号
***************************************************************************/
int parseCerIndex(const char* pTmp,STORAGESECRET_INFO* pStorageSecretInfo);

/***************************************************************************
 函 数 名 : parseMediaInfo
 功能描述 : 解析媒体信息
 输入参数 : pTmpMsg                     待解析的字符串
            ulKeyType                   媒体类型
 输出参数 : pAudioDecoderType           获取到的音频编码类型
            pVideoDecoderType           获取到的视频编码类型
            pPort                       获取到的端口号
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseMediaInfo(char* pTmp, IVS_ULONG* pKeyType, IVS_ULONG* pAudioDecoderType, 
				IVS_ULONG* pVideoDecoderType, IVS_ULONG* pPort);

/***************************************************************************
 函 数 名 : parseStarTimeAndEndTime
 功能描述 : 解析媒体媒体开始结束时间
 输入参数 : pTmp                    待解析的字符串
 输出参数 : pMediaKeyInfo           媒体密钥信息

 返 回 值 : IVS_SUCCEED-成功        其他-失败
***************************************************************************/
int parseStarTimeAndEndTime(const char* pTmp,MEDIA_KEY_INFO* pMediaKeyInfo);

/***************************************************************************
 函 数 名 : parseSessionID
 功能描述 : 解析媒体媒体SessionID
 输入参数 : pTmp                    待解析的字符串
 输出参数 : pMediaKeyInfo           媒体密钥信息

 返 回 值 : IVS_SUCCEED-成功        其他-失败
***************************************************************************/
int parseSessionID(const char* pTmp,MEDIA_KEY_INFO* pMediaKeyInfo);

#endif  // _IVS_SDP_H_


