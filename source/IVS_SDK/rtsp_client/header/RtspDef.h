#ifndef __RTSPDEF_H_
#define __RTSPDEF_H_


// 定义标准RTSP方法字符串
#define RTSP_METHOD_STRING {\
    "SETUP",\
    "PLAY",\
    "PAUSE",\
    "TEARDOWN",\
    "ANNOUNCE",\
    "DESCRIBE",\
    "OPTIONS"\
}


// 定义ANNOUNCE正常结束
#define RTSP_NORMAL_STOP             2002

// 定义ANNOUNCE异常结束
#define RTSP_ABNORMAL_STOP           2003

// 定义RTSP版本
#define RTSP_PROTOCOL_VERSION        "RTSP/1.0"

// 定义RTSP协议头
#define RTSP_PROTOCOL_HEADER         "rtsp://"

// 定义RTSP行结束符
#define RTSP_END_TAG                 "\r\n"

// 定义RTSP消息结束符
#define RTSP_MSG_END_TAG             "\r\n\r\n"

// 定义Server Agent
#define RTSP_SERVER_AGENT            "HUAWEI RTSP_CLIENT"


// 定义Transport字段中的RTP关键字
#define RTSP_TRANSPORT_RTP           "RTP"

// 定义Transport字段中的profile关键字
#define RTSP_TRANSPORT_PROFILE_AVP   "AVP"

// 定义Transport字段中的MP2T关键字
#define RTSP_TRANSPORT_MP2T          "MP2T"

// 定义Transport字段中的Ts over RTP关键字
#define RTSP_TRANSPORT_TS_OVER_RTP   "MP2T/RTP"

// 定义Transport字段中的TCP关键字
#define RTSP_TRANSPORT_TCP           "TCP"

// 定义Transport字段中的传输类型域的分隔符
#define RTSP_TRANSPORT_SPEC_SPLITER  "/"

// 定义;分号
#define SIGN_SEMICOLON               ";"

// 定义，逗号
#define SIGN_COMMA                   ","

// 定义-横线
#define SIGN_H_LINE                  "-"

// 定义Transport字段中的client_port关键字
#define RTSP_TRANSPORT_CLIENT_PORT   "client_port="

// 定义Transport字段中的server_port关键字
#define RTSP_TRANSPORT_SERVER_PORT   "server_port="

// 定义Transport字段中的source关键字
#define RTSP_TRANSPORT_SOURCE        "source="

// 定义Transport字段中的destination关键字
#define RTSP_TRANSPORT_DESTINATIION  "destination="

// 定义Transport字段中的interleaved关键字
#define RTSP_TRANSPORT_INTERLEAVED   "interleaved="

// 定义Transport字段中的ssrc关键字
#define RTSP_TRANSPORT_SSRC          "ssrc="

// 定义Transport字段中的unicast关键字
#define RTSP_TRANSPORT_UNICAST       "unicast"

// 定义Transport字段中的multicast关键字
#define RTSP_TRANSPORT_MULTICAST     "multicast"

// 定义Transport字段中的ttl关键字
#define RTSP_TRANSPORT_TTL           "ttl="

// 定义Range字段中的npt关键字
#define RTSP_RANGE_NPT               "npt="

// 定义Range字段中的clock关键字
#define RTSP_RANGE_CLOCK             "clock="

// 定义Range字段中的now参数
#define RTSP_RANGE_NOW               "now"

// 定义Range字段中的beginning参数
#define RTSP_RANGE_BEGINNING         "beginning"

// 定义Range字段中UTC时间的秒与毫秒分隔符
#define RTSP_RANGE_UTC_SEC_DOT       "."

// 定义playseek接口关键字(TVOD节目访问接口)
#define RTSP_PLAY_SEEK               "playseek="

// 定义ANNOUNCE的CONTENT字段
#define RTSP_ANNOUNCE_CONTENT        "HW_ANNOUNCE_QUIT:"

// 定义Options消息的URL
#define RTSP_OPTIOS_URL              "rtsp://ivs.huawei.com/1.sdp"

#define RTSP_RESPONSE_OK             "200 OK"

#define RTSP_RESPONSE_BAD_REQUEST    "400 Bad Request"

// 定义标准RTSP头域字符串
#define RTSP_TOKEN_STR_CSEQ          "CSeq: "
#define RTSP_TOKEN_STR_ACCEPT        "Accept: "
#define RTSP_TOKEN_STR_USERAGENT     "User-Agent: "
#define RTSP_TOKEN_STR_DATE          "Date: "
#define RTSP_TOKEN_STR_SERVER        "Server: "
#define RTSP_TOKEN_STR_PUBLIC        "Public: "
#define RTSP_TOKEN_STR_SESSION       "Session: "
#define RTSP_TOKEN_STR_CONTENT_LENGTH "Content-Length: "
#define RTSP_TOKEN_STR_CONTENT_TYPE  "Content-Type: "
#define RTSP_TOKEN_STR_CONTENT_BASE  "Content-Base: "
#define RTSP_TOKEN_STR_LOCATION      "Location: "
#define RTSP_TOKEN_STR_TRANSPORT     "Transport: "
#define RTSP_TOKEN_STR_RANGE         "Range: "
#define RTSP_TOKEN_STR_SCALE         "Scale: "
#define RTSP_TOKEN_STR_SPEED         "Speed: "
#define RTSP_TOKEN_STR_RTPINFO       "RTP-Info: "
#define RTSP_TOKEN_STR_REQUIRE       "Require: "
#define RTSP_TOKEN_STR_PROXY         "Proxy-Require: "
#define RTSP_TOKEN_STR_GET           "GetVideoTime: "

// 传输类型
enum TRANS_TYPE
{
    TRANS_TYPE_UDP                = 0, //UDP
    TRANS_TYPE_TCP                = 1, //TCP
    TRANS_TYPE_MAX
};


// 定义RTSP协议栈返回码
typedef enum
{
    RTSP_CONTINUE                           = 0,        /*100*/
    RTSP_SUCCESS_OK                         = 1,        /*200*/
    RTSP_SUCCESS_CREATED                    = 2,        /*201*/
    RTSP_SUCCESS_ACCEPTED                   = 3,        /*202*/
    RTSP_SUCCESS_NOCONTENT                  = 4,        /*203*/
    RTSP_SUCCESS_PARTIALCONTENT             = 5,        /*204*/
    RTSP_SUCCESS_LOWONSTORAGE               = 6,        /*250*/
    RTSP_MULTIPLE_CHOICES                   = 7,        /*300*/
    RTSP_REDIRECT_PERMMOVED                 = 8,        /*301*/
    RTSP_REDIRECT_TEMPMOVED                 = 9,        /*302*/
    RTSP_REDIRECT_SEEOTHER                  = 10,       /*303*/
    RTSP_REDIRECT_NOTMODIFIED               = 11,       /*304*/
    RTSP_USEPROXY                           = 12,       /*305*/
    RTSP_CLIENT_BAD_REQUEST                 = 13,       /*400*/
    RTSP_CLIENT_UNAUTHORIZED                = 14,       /*401*/
    RTSP_PAYMENT_REQUIRED                   = 15,       /*402*/
    RTSP_CLIENT_FORBIDDEN                   = 16,       /*403*/
    RTSP_CLIENT_NOTFOUND                    = 17,       /*404*/
    RTSP_CLIENT_METHOD_NOTALLOWED           = 18,       /*405*/
    RTSP_NOTACCEPTABLE                      = 19,       /*406*/
    RTSP_PROXY_AUTHENTICATION_REQUIRED      = 20,       /*407*/
    RTSP_REQUEST_TIMEOUT                    = 21,       /*408*/
    RTSP_CLIENT_CONFLICT                    = 22,       /*409*/
    RTSP_GONE                               = 23,       /*410*/
    RTSP_LENGTH_REQUIRED                    = 24,       /*411*/
    RTSP_PRECONDITION_FAILED                = 25,       /*412*/
    RTSP_REQUEST_ENTITY_TOO_LARGE           = 26,       /*413*/
    RTSP_REQUEST_URI_TOO_LARGE              = 27,       /*414*/
    RTSP_UNSUPPORTED_MEDIA_TYPE             = 28,       /*415*/
    RTSP_CLIENT_PARAMETER_NOTUNDERSTOOD     = 29,       /*451*/
    RTSP_CLIENT_CONFERENCE_NOTFOUND         = 30,       /*452*/
    RTSP_CLIENT_NOTENOUGH_BANDWIDTH         = 31,       /*453*/
    RTSP_CLIENT_SESSION_NOTFOUND            = 32,       /*454*/
    RTSP_CLIENT_METHOD_NOTVALID_INSTATE     = 33,       /*455*/
    RTSP_CLIENT_HEADER_FIELD_NOTVALID       = 34,       /*456*/
    RTSP_CLIENT_INVALID_RANGE               = 35,       /*457*/
    RTSP_CLIENT_READONLY_PARAMETER          = 36,       /*458*/
    RTSP_CLIENT_AGGREGATE_OPTION_NOTALLOWED = 37,       /*459*/
    RTSP_CLIENT_AGGREGATE_OPTION_ALLOWED    = 38,       /*460*/
    RTSP_CLIENT_UNSUPPORTED_TRANSPORT       = 39,       /*461*/
    RTSP_CLIENT_DESTINATION_UNREACHABLE     = 40,       /*462*/
    RTSP_SERVER_INTERNAL                    = 41,       /*500*/
    RTSP_SERVER_NOTIMPLEMENTED              = 42,       /*501*/
    RTSP_SERVER_BAD_GATEWAY                 = 43,       /*502*/
    RTSP_SERVICE_UNAVAILABLE                = 44,       /*503*/
    RTSP_SERVER_GATEWAY_TIMEOUT             = 45,       /*505*/
    RTSP_RTSP_VERSION_NOTSUPPORTED          = 46,       /*504*/
    RTSP_SERVER_OPTION_NOTSUPPORTED         = 47,       /*551*/

    RTSP_STATUS_CODES_BUTT
}RTSP_STATUS_CODE_E;


#define RTSP_CODE_STRING \
{\
    "100 Continue",\
    "200 OK",\
    "201 Created",\
    "202 Accepted",\
    "203 No Content",\
    "204 Partial Content",\
    "250 Low on Storage Space",\
    "300 Multiple Choices",\
    "301 Moved Permanently",\
    "302 Moved Temporarily",\
    "303 See Other",\
    "304 Not Modified",\
    "305 Use Proxy",\
    "400 Bad Request",\
    "401 Unauthorized",\
    "402 Payment Required",\
    "403 Forbidden",\
    "404 Not Found",\
    "405 Method Not Allowed",\
    "406 Not Acceptable",\
    "407 Proxy Authentication Required",\
    "408 Request Time-out",\
    "409 Conflict",\
    "410 Gone",\
    "411 Length Required",\
    "412 Precondition Failed",\
    "413 Request Entity Too Large",\
    "414 Request-URI Too Large",\
    "415 Unsupported Media Type",\
    "451 Parameter Not Understood",\
    "452 Conference Not Found",\
    "453 Not Enough Bandwidth",\
    "454 Session Not Found",\
    "455 Method Not Valid in this State",\
    "456 Header Field Not Valid For Resource",\
    "457 Invalid Range",\
    "458 Parameter Is Read-Only",\
    "459 Aggregate Option Not Allowed",\
    "460 Only Aggregate Option Allowed",\
    "461 Unsupported Transport",\
    "462 Destination Unreachable",\
    "500 Internal Server Error",\
    "501 Not Implemented",\
    "502 Bad Gateway",\
    "503 Service Unavailable",\
    "504 Gateway Timeout",\
    "505 RTSP Version not supported",\
    "551 Option Not Supported"\
}


#endif //__RTSPDEF_H_
