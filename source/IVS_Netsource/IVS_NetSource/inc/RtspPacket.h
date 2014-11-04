///////////////////////////////////////////////////////////////////////////////
/// @file  RtspPacket.h
/// @brief RTSP封装解析类定义  
///
/// Copyright (c) 2010
/// Huawei Tech.Co.,Ltd
/// 
/// @author   Li GengQiang/l00124479
/// @date     2011/01/21
/// @version  IVS V100R002C01
///////////////////////////////////////////////////////////////////////////////

#ifndef _RTSP_PACKET_H_HEADER_INCLUDED_
#define _RTSP_PACKET_H_HEADER_INCLUDED_

#pragma warning(disable : 4786 4503 4996)

#include <string>
using namespace std;

#ifndef snprintf
#define snprintf _snprintf
#endif

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

typedef int BOOL;

namespace RTSP
{
    const unsigned long MAX_LEN_SESSION_URL     = 256;  //媒体会话URL的最大长度
    const unsigned long MAX_LEN_SESSION_ID      = 32;   //媒体会话ID的最大长度
    const unsigned long MAX_LEN_IP              = 16;   //IP的最大长度

    const char USER_AGENT[]     = "HUAWEI CU/V100R002C02";  //User-Agent字段
    const char RTSP_VERSION[]   = "RTSP/1.0";   //RTSP及其版本号
    const char METHOD_RECORD[]  = "RECORD";     //PLAY方法字符串

    const char TAB          = '\t';
    const char SPACE        = ' ';
    const char DOUBLE_QUOTE = '"';

    const string SEMI_COLON = ";";
    const string COLON      = ":";
    const string EQUAL_SIGN = "=";
    const string CRLF       = "\r\n";
    const unsigned long CRLF_LEN = 2;   //CRLF的长度

    const string METHOD_PLAY    = "PLAY";       //PLAY方法字符串
    const string METHOD_OPTIONS = "OPTIONS";    //OPTINS方法字符串
    const string METHOD_PAUSE   = "PAUSE";      //PAUSE方法字符串

    //返回码
    enum RET_CODE
    {
        RET_CODE_OK,    ///< 成功
        RET_CODE_FAIL   ///< 失败
    };

    //协议类型
    enum PROTOCAL_TYPE
    {
        PROTOCAL_TYPE_RTP,  ///< RTP
        PROTOCAL_TYPE_RTCP  ///< RTCP
    };

    //VCR控制码
    enum VCR_CODE
    {
        VCR_CODE_SPEED,     ///< 改变录像下载速度
        VCR_CODE_SCALE,     ///< 改变录像回放速度
        VCR_CODE_DROP,      ///< 拖动
        VCR_CODE_PAUSE,     ///< 暂停
        VCR_CODE_CONTINUE  ///< 继续
    };

    //消息类型
    enum MSG_TYPE
    {
        MSG_TYPE_RESP,          ///< 响应消息
        MSG_TYPE_SET_PARAMETER  ///< 设置参数消息
    };

    enum PLAY_INFO_TYPE
    {
        PLAY_INFO_TYPE_EOS,     ///<    EOS
        PLAY_INFO_TYPE_BOS,     ///<    BOS
        PLAY_INFO_TYPE_CLOSE   ///<    服务端主动断链
    };

    //媒体信息
    struct MEDIA_INFO
    {
        char            szSessionUrl[MAX_LEN_SESSION_URL + 1];      ///< 媒体会话URL
        char            szSessionId[MAX_LEN_SESSION_ID + 1];        ///< 媒体会话ID
        char            szLocalIp[MAX_LEN_IP + 1];                  ///< 本地IP
        unsigned short  usLocalPort;                                ///< 本地端口号
    };

    //NAT请求参数
    struct NAT_REQ_PARA
    {
        unsigned long   ulProtocalType; ///< 协议类型，定义参见@ref RTSP::PROTOCAL_TYPE
        unsigned short  usLocalPort;    ///< 本地端口
    };

    //VCR请求参数
    struct VCR_REQ_PARA
    {
        unsigned long   ulVcrCode;      ///< VCR控制码，定义参见@ref RTSP::VCR_CODE
        unsigned long   ulStartTime;    ///< 开始时间，单位为秒，@ref RTSP::VCR_CODE_DROP时有效
        float           fSpeed;         ///< 下载或回放速度，@ref RTSP::VCR_CODE_SPEED和@ref RTSP::VCR_CODE_SCALE时有效
        BOOL            bRelativeTime;  ///< 是否为相对时间，@ref RTSP::VCR_CODE_DROP时有效
    };

    struct NAT_DECODE_RESULT
    {
        string          strProtocalType;///< 协议类型，定义参见@ref RTSP::PROTOCAL_TYPE
        string          strRemoteIp;    ///< 远端IP
        string          strRemotePort;  ///< 远程端口
        string          strCSeq;        ///< CSeq，在响应消息中原值返回
        string          strSession;     ///< Session,在响应消息中原值返回
    };

    //解码结果
    struct DECODE_RESULT
    {
        unsigned long   ulMsgType;      ///< 消息类型，定义参见@ref RTSP::MSG_TYPE
        unsigned long   ulPlayInfoType; ///< 播放信息类型
        BOOL            bHasRtpInfo;    ///< 是否有RTP-Info信息
        unsigned long   ulRtpSeq;       ///< RTP包的序列号，bHasRtpInfo为TRUE时有效
        unsigned long   ulRtpTime;      ///< RTP包的时间戳，bHasRtpInfo为TRUE时有效
        string          strCSeq;        ///< CSeq，在响应消息中原值返回

        //构造函数初始化
        DECODE_RESULT()
        {
            ulMsgType       = MSG_TYPE_RESP;
            ulPlayInfoType  = PLAY_INFO_TYPE_EOS;
            bHasRtpInfo     = 0;
            ulRtpSeq        = 0;
            ulRtpTime       = 0;
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
/// @brief RTSP的封装解析类
/// 
/// Description:  
///////////////////////////////////////////////////////////////////////////////
class CRtspPacket
{
public:
    /**
    * Description:    CRtspPacket(). Default constructor
    */
    CRtspPacket();

    /**
    * Description:  SetMediaInfo(). 设置媒体信息
    * @param  [in]  stMediaInfo 媒体信息
    * @return       void.
    */
    void SetMediaInfo
    (
        IN  const RTSP::MEDIA_INFO& stMediaInfo
    );

    /**
    * Description:  GetNatPacket(). 获取NAT请求消息包
    * @param  [in]  stNatReqPara    NAT请求的参数
    * @param  [out] strPacket       封装后的字符串
    * @return       long.   返回码
    */
    long GetNatPacket
    (
        IN  const RTSP::NAT_REQ_PARA&   stNatReqPara,
        OUT string&                     strPacket
    );

    /**
    * Description:  GetVcrPacket(). 获取VCR请求消息包
    * @param  [in]  stVcrReqPara    VCR请求的参数
    * @param  [out] strPacket       封装后的字符串
    * @return       long.   返回码
    */
    long GetVcrPacket
    (
        IN  const RTSP::VCR_REQ_PARA&   stVcrReqPara,
        OUT string&                     strPacket
    );

    /**
    * Description:  GetKeepAlivePacket(). 获取保活请求消息包
    * @param  [out] strPacket       封装后的字符串
    * @return       void.
    */
    void GetKeepAlivePacket
    (
        OUT string& strPacket
    );

    /**
    * Description:  DecodePacket().     解析RTSP包
    * @param  [in]  strPacket   RTSP包
    * @param  [out] stResult    解析结果
    * @return       long.   返回码
    */
    long DecodePacket
    (
        IN  const string&           strPacket,
        OUT RTSP::DECODE_RESULT&    stResult
    )const;

    /**
    * Description:  GetRspPacket().     获取响应消息包
    * @param  [in]  strCSeq         请求的CSeq
    * @param  [out] strPacket   封装后的字符串
    * @return       void.
    */
    void GetRspPacket
    (
        IN  const string&   strCSeq,
        OUT string&         strPacket
    )const;

    /**
    * Description:  GetFieldValue().    获取指定头域的内容
    * @param  [in]  strField    所有头域字符串
    * @param  [in]  strKey      头域Key值
    * @param  [out] strContent  头域内容
    * @param  [in]  strEnd          结束符
    * @param  [in]  strInterval     分隔符
    * @return       long.   返回码
    */
    static long GetFieldValue
    (
        IN  const string&   strField,
        IN  const string&   strKey,
        OUT string&         strContent,
        IN  const string&   strEnd      = RTSP::CRLF,
        IN  const string&   strInterval = RTSP::COLON
    );

    /**
    * Description:  TrimString().   去掉字符串首尾的空格、Tab
    * @param  [io]  str     待处理的字符串
    * @return       void.
    */
    static void TrimString
    (
        IO  string& str
    );

    /**
    * Description:  MuiltLineToOneLine().   将头域内容的多行转为单行
    * @param  [io]  strFieldContent     待处理的头域内容字符串
    * @return       void.
    */
    static void MuiltLineToOneLine
    (
        IO  string& strFieldContent
    );

    /**
    * Description:  DecodeNatPacket(). 解析NAT穿越包
    * @param  [in]  strPacket      RTSP的NAT包
    * @param  [out] stNatResult    NAT解析结果
    * @return       long.   返回码
    */
    long DecodeNatPacket
    (
    IN  const string&           strPacket,
    OUT RTSP::NAT_DECODE_RESULT&    stNatResult
    ) const;

    /**
    * Description:  GetNATRspPacket().     获取NAT响应消息包
    * @param  [in]  stNatDecodeResult      解析NAT穿越包的结果
    * @param  [out] strPacket   封装后的字符串
    * @return       void.
    */
    void GetNATRspPacket
    (
        IN  const RTSP::NAT_DECODE_RESULT&   stNatDecodeResult,
        OUT string&         strPacket
    )const;
private:    
    /**
    * Description:  GetCSeq().  获取新的CSeq
    * @return       string.     新的CSeq字符串
    */
    string GetCSeq();

    /**
    * Description:  SetCommonField().   设置公共头域
    * @param  [in]  strMethod   请求方法名
    * @param  [io]  strPacket   封装后的字符串
    * @return       void.
    */
    void SetCommonField
    (
        IN  const string&   strMethod,
        IO  string&         strPacket
    );

    /**
    * Description:  SetRangeField().    设置Range头域
    * @param  [in]  ulStartTime         起始时间
    * @param  [in]  bRelativeTime   是否为相对时间
    * @param  [io]  strRange        设置后的字符串
    * @return       long.   返回码
    */
    long SetRangeField
    (
        IN  unsigned long   ulStartTime,
        IN  BOOL            bRelativeTime,
        IO  string&         strRange
    )const;

    /**
    * Description:  GetFirstLine(). 获取首行首字段和随后的头域字符串
    * @param  [in]  strPacket       消息包
    * @param  [out] strFirstHeader  首行首字段
    * @param  [out] strField        头域
    * @return       long.   返回码
    */
    long GetFirstLine
    (
        IN  const string&   strPacket,
        OUT string&         strFirstHeader,
        OUT string&         strField
    )const;

    /**
    * Description:  GetPlayInfoType().  获取播放信息类型
    * @param  [in]  strField        所有头域字符串
    * @param  [out] ulPlayInfoType  播放信息类型
    * @return       long.   返回码
    */
    long GetPlayInfoType
    (
        IN  const string&   strField,
        OUT unsigned long&  ulPlayInfoType
    )const;

    /**
    * Description:  GetRtpInfo().  获取RTP-Info
    * @param  [in]  strField        所有头域字符串
    * @param  [out] stResult    解析结果
    * @return       long.   返回码
    */
    long GetRtpInfo
    (
        IN  const string&           strField,
        OUT RTSP::DECODE_RESULT&    stResult
    )const;

private:
    //序列号
    unsigned long m_ulCSeq;
    //媒体信息
    RTSP::MEDIA_INFO m_stMediaInfo;
};

#endif //_RTSP_PACKET_H_HEADER_INCLUDED_

