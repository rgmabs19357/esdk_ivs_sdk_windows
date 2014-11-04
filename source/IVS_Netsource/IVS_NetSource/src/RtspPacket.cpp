///////////////////////////////////////////////////////////////////////////////
/// @file  RtspPacket.cpp
/// @brief RTSP封装解析类的实现  
///
/// Copyright (c) 2010
/// Huawei Tech.Co.,Ltd
/// 
/// @author   Li GengQiang/l00124479
/// @date     2011/01/21
/// @version  IVS V100R002C01
///////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include "RtspPacket.h"

/**
* Description:    CRtspPacket(). Default constructor
*/
CRtspPacket::CRtspPacket()
{
    m_ulCSeq = 0;

    memset(&m_stMediaInfo, 0, sizeof(m_stMediaInfo));
}

/**
* Description:  GetCSeq().  获取新的CSeq
* @return       string.     新的CSeq
*/
string CRtspPacket::GetCSeq()
{
    const unsigned long ulCSeq = ++m_ulCSeq;
    char szCSeq[16] = {0};
    (void)snprintf(szCSeq, sizeof(szCSeq) - 1, "%lu", ulCSeq);

    return szCSeq;
}

/**
* Description:  SetMediaInfo(). 设置媒体信息
* @param  [in]  stMediaInfo 媒体信息
* @return       void.
*/
void CRtspPacket::SetMediaInfo
(
    IN  const RTSP::MEDIA_INFO& stMediaInfo
)
{
    m_stMediaInfo = stMediaInfo;
}

/**
* Description:  SetCommonField().   设置公共头域
* @param  [in]  strMethod   请求方法名
* @param  [io]  strPacket   封装后的字符串
* @return       void.
*/
void CRtspPacket::SetCommonField
(
    IN  const string&   strMethod,
    IO  string&         strPacket
)
{
    //Request-Line
    strPacket = strMethod;
    strPacket += " ";
    strPacket += m_stMediaInfo.szSessionUrl;
    strPacket += " RTSP/1.0\r\n";

    //CSeq
    strPacket += "CSeq: ";
    strPacket += GetCSeq();
    strPacket += RTSP::CRLF;

    //Session
	strPacket += "Session: ";
    strPacket += m_stMediaInfo.szSessionId;
    strPacket += RTSP::CRLF;

    //User-Agent
    strPacket += "User-Agent: ";
    strPacket += RTSP::USER_AGENT;
    strPacket += RTSP::CRLF;

    //Content-Length
    strPacket += "Content-Length:0\r\n";

	//Range
	strPacket += "Range:npt=0-\r\n";
}

/**
* Description:  SetRangeField().    设置Range头域
* @param  [in]  ulStartTime         起始时间
* @param  [in]  bRelativeTime   是否为相对时间
* @param  [io]  strRange        设置后的字符串
* @return       long.   返回码
*/
long CRtspPacket::SetRangeField
(
    IN  unsigned long   ulStartTime,
    IN  BOOL            bRelativeTime,
    IO  string&         strRange
)const
{
    //开始时间转换为字符串，最大长度不会超过32字节
    char szStartTime[32] = {0};
    
    if (bRelativeTime)
    {
        (void)snprintf(szStartTime, sizeof(szStartTime) - 1, "%u-", ulStartTime);

        //Range使用npt相对时间
        strRange += "Range:npt=";
    }
    else
    {
        time_t tStartTime = ulStartTime;
        
        //localtime返回NULL的条件:  Before midnight, January 1, 1970.  After 03:14:07, January 19, 2038,
        struct tm _tm;
		memset(&_tm, 0, sizeof(tm));
		localtime_s(&_tm, &tStartTime);
        
        (void)snprintf(szStartTime, sizeof(szStartTime) - 1, 
                        "%04d%02d%02dT%02d%02d%02dZ-", 
                        _tm.tm_year + 1900, 
                        _tm.tm_mon + 1, 
                        _tm.tm_mday, 
                        _tm.tm_hour, 
                        _tm.tm_min, 
                        _tm.tm_sec);

        //Range使用clock绝对时间
        strRange += "Range:clock=";
    }
    
    strRange += szStartTime;
    strRange += RTSP::CRLF;

    return RTSP::RET_CODE_OK; 
}

/**
* Description:  GetNatPacket(). 获取NAT请求消息包
* @param  [in]  stNatReqPara    NAT请求的参数
* @param  [out] strPacket       封装后的字符串
* @return       long.       返回码
*/
long CRtspPacket::GetNatPacket
(
    IN  const RTSP::NAT_REQ_PARA&   stNatReqPara,
    OUT string&                     strPacket
)
{
    string strProtocalType;
    switch (stNatReqPara.ulProtocalType)
    {
    case RTSP::PROTOCAL_TYPE_RTP:
        {
            strProtocalType = "RTP";
            break;
        }

    case RTSP::PROTOCAL_TYPE_RTCP:
        {
            strProtocalType = "RTCP";
            break;
        }

    default:
        {
            return RTSP::RET_CODE_FAIL;
            //break;
        }
    }

    //本地端口转换为字符串
    char szLocalPort[16] = {0};
    (void)snprintf(szLocalPort, sizeof(szLocalPort) - 1, "%u", stNatReqPara.usLocalPort);

    //设置公共头域
    SetCommonField(RTSP::METHOD_PLAY, strPacket);

    //Range
    //strPacket += "Range:npt=0-\r\n";

    //x-NAT-Info
    strPacket += "x-NAT-Info:type=";
    strPacket += strProtocalType;
    strPacket += ";local_addr=";
    strPacket += m_stMediaInfo.szLocalIp;
    strPacket += ";local_port=";
    strPacket += szLocalPort;
    strPacket += RTSP::CRLF;

    //加上空行
    strPacket += RTSP::CRLF;

    return RTSP::RET_CODE_OK;
}

/**
* Description:  GetVcrPacket(). 获取VCR请求消息包
* @param  [in]  stVcrReqPara    VCR请求的参数
* @param  [out] strPacket       封装后的字符串
* @return       long.   返回码
*/
long CRtspPacket::GetVcrPacket
(
    IN  const RTSP::VCR_REQ_PARA&   stVcrReqPara,
    OUT string&                     strPacket
)
{
    long lResult = RTSP::RET_CODE_OK;
    
    //请求方法，默认为PLAY
    string strMethod = RTSP::METHOD_PLAY;
    //Range，默认为空
    string strRange;
    //下载或回放速度，默认为空
    string strSpeed;

    //速度转换为字符串
    char szSpeed[16] = {0};
    (void)snprintf(szSpeed, sizeof(szSpeed) - 1, "%f", stVcrReqPara.fSpeed);

    switch (stVcrReqPara.ulVcrCode)
    {
    case RTSP::VCR_CODE_SPEED:
        {
            //Speed
            strSpeed = "Speed:";
            strSpeed += szSpeed;
            strSpeed += RTSP::CRLF;
            break;
        }

    case RTSP::VCR_CODE_SCALE:
        {
            //Scale
            strSpeed = "Scale:";
            strSpeed += szSpeed;
            strSpeed += RTSP::CRLF;
            break;
        }

    case RTSP::VCR_CODE_DROP:
        {
            lResult = SetRangeField(stVcrReqPara.ulStartTime, stVcrReqPara.bRelativeTime, strRange);
            break;
        }

    case RTSP::VCR_CODE_PAUSE:
        {
            //重置为PAUSE
            strMethod = RTSP::METHOD_PAUSE;
            break;
        }

    case RTSP::VCR_CODE_CONTINUE:
        {
            //直接使用默认值
            break;
        }

    default:
        {
            return RTSP::RET_CODE_FAIL;
            //break;
        }
    }

    //设置公共头域
    SetCommonField(strMethod, strPacket);
    //Range
    strPacket += strRange;
    //Speed
    strPacket += strSpeed;
    //空行
    strPacket += RTSP::CRLF;

    return lResult;
}

/**
* Description:  GetKeepAlivePacket(). 获取保活请求消息包
* @param  [out] strPacket       封装后的字符串
* @return       long.       返回码
*/
void CRtspPacket::GetKeepAlivePacket
(
    OUT string& strPacket
)
{
    //设置公共头域
    SetCommonField(RTSP::METHOD_OPTIONS, strPacket);

    //加上空行
    strPacket += RTSP::CRLF;
}

/**
* Description:  DecodePacket().     解析RTSP包
* @param  [in]  strPacket   RTSP包
* @param  [out] stResult    解析结果
* @return       long.   返回码
*/
long CRtspPacket::DecodePacket
(
    IN  const string&           strPacket,
    OUT RTSP::DECODE_RESULT&    stResult
)const
{
    //long lResult = RTSP::RET_CODE_OK;

    string strFirstHeader;
    string strField;

    //获取首行首字段和头域内容
    long lResult = GetFirstLine(strPacket, strFirstHeader, strField);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return lResult;
    }

    //响应消息直接返回
    if (0 == strFirstHeader.compare(RTSP::RTSP_VERSION))
    {
        stResult.ulMsgType = RTSP::MSG_TYPE_RESP;

        //获取RTP-Info，只有NAT穿越响应消息才有
        (void)GetRtpInfo(strField, stResult);
    
        return RTSP::RET_CODE_OK;
    }
    //只解析SET_PARAMETER消息
    else if (0 != strFirstHeader.compare("SET_PARAMETER"))
    {
        return RTSP::RET_CODE_FAIL;
    }

    stResult.ulMsgType = RTSP::MSG_TYPE_SET_PARAMETER;

    //将头域内容的多行转为单行
    MuiltLineToOneLine(strField);

    //获取CSeq
    lResult = GetFieldValue(strField, "CSeq", stResult.strCSeq);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //获取播放信息类型
    lResult = GetPlayInfoType(strField, stResult.ulPlayInfoType);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    return RTSP::RET_CODE_OK;
}

/**
* Description:  GetRspPacket().     获取响应消息包
* @param  [in]  strCSeq         请求的CSeq
* @param  [out] strPacket   封装后的字符串
* @return       void.
*/
void CRtspPacket::GetRspPacket
(
    IN  const string&   strCSeq,
    OUT string&         strPacket
)const
{
    //响应头
    strPacket = "RTSP/1.0 200 OK\r\n";

    //CSeq
    strPacket += "CSeq: ";
    strPacket += strCSeq;
    strPacket += RTSP::CRLF;

    //Session
    strPacket += "Session: ";
    strPacket += m_stMediaInfo.szSessionId;
    strPacket += RTSP::CRLF;

    //空行
    strPacket += RTSP::CRLF;
}

/**
* Description:  GetFirstLine(). 获取首行首字段和随后的头域字符串
* @param  [in]  strPacket       消息包
* @param  [out] strFirstHeader  首行首字段
* @param  [out] strField        头域
* @return       long.   返回码
*/
long CRtspPacket::GetFirstLine
(
    IN  const string&   strPacket,
    OUT string&         strFirstHeader,
    OUT string&         strField
)const
{
    size_t nPosLineStart = 0;
    size_t nPosLineEnd = strPacket.find(RTSP::CRLF, nPosLineStart);
    if (string::npos == nPosLineEnd)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //首行
    string strFirstLine = strPacket.substr(nPosLineStart, (nPosLineEnd - nPosLineStart));   //lint !e845
    
    size_t nPosStart = 0;
    size_t nPosEnd = strFirstLine.find(RTSP::SPACE, nPosStart);
    if (string::npos == nPosEnd)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //首行首字段
    strFirstHeader = strFirstLine.substr(nPosStart, (nPosEnd - nPosStart));   //lint !e845
    //头域内容
    strField = strPacket.substr(nPosLineEnd + RTSP::CRLF_LEN);

    return RTSP::RET_CODE_OK;
}

/**
* Description:  GetFieldValue().    获取指定头域的内容
* @param  [in]  strField    所有头域字符串
* @param  [in]  strKey      头域Key值
* @param  [out] strContent  头域内容
* @param  [in]  strEnd          结束符
* @param  [in]  strInterval     分隔符
* @return       long.   返回码
*/
long CRtspPacket::GetFieldValue
(
    IN  const string&   strField,
    IN  const string&   strKey,
    OUT string&         strContent,
    IN  const string&   strEnd,
    IN  const string&   strInterval
)
{
    //先加一个结束符
    std::string strFieldTmp = strField + strEnd;//lint !e1036
    
    string strLine;
    string strValue;
    size_t nPosLineStart = 0;
    size_t nPosLineEnd = strFieldTmp.find(strEnd, nPosLineStart);

    long lResult = RTSP::RET_CODE_FAIL;
    
    while(string::npos != nPosLineEnd)
    {
        strLine = strFieldTmp.substr(nPosLineStart, nPosLineEnd - nPosLineStart);
    
        size_t nPosStart = 0;
        size_t nPosEnd = strLine.find(strInterval, nPosStart);

        //找到冒号，说明是某头域，进一步解析头域的值
        if (string::npos != nPosEnd)
        {
            strValue = strLine.substr(nPosStart, (nPosEnd - nPosStart));   //lint !e845

            //去掉字符串首尾的空格、Tab
            TrimString(strValue);

            //不区分大小写的比较，找到则直接退出
            if (0 == _strnicmp(strKey.c_str(), strValue.c_str(), strKey.length()))
            {
                //提取内容
                strContent = strLine.substr(nPosEnd + 1, nPosLineEnd - nPosEnd);
                //去掉字符串首尾的空格、Tab
                TrimString(strContent);
                
                lResult = RTSP::RET_CODE_OK;
                break;
            }
        }        

        //下一个行
        nPosLineStart = nPosLineEnd + strEnd.size();
        nPosLineEnd = strFieldTmp.find(strEnd, nPosLineStart);
    }
    
    return lResult;
}

/**
* Description:  GetPlayInfoType().  获取播放信息类型
* @param  [in]  strField        所有头域字符串
* @param  [out] ulPlayInfoType  播放信息类型
* @return       long.   返回码
*/
long CRtspPacket::GetPlayInfoType
(
    IN  const string&   strField,
    OUT unsigned long&  ulPlayInfoType
)const
{
    //获取X-PLAYINFO头域
    string strPlayInfo;
    long lResult = GetFieldValue(strField, "X-PLAYINFO", strPlayInfo);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //提取首个属性
    size_t nPosStart = strPlayInfo.find(RTSP::DOUBLE_QUOTE, 0);
    if (string::npos == nPosStart)
    {
        return RTSP::RET_CODE_FAIL;
    }
    nPosStart += 1;

    size_t nPosEnd = strPlayInfo.find(RTSP::DOUBLE_QUOTE, nPosStart);
    if (string::npos == nPosEnd)
    {
        return RTSP::RET_CODE_FAIL;
    }

    string strValue = strPlayInfo.substr(nPosStart, nPosEnd - nPosStart);

    //转换首个属性为播放信息类型
    if (0 == strValue.compare("EOS"))
    {
        ulPlayInfoType = RTSP::PLAY_INFO_TYPE_EOS;
    }
    else if (0 == strValue.compare("BOS"))
    {
        ulPlayInfoType = RTSP::PLAY_INFO_TYPE_BOS;
    }
    else
    {
        nPosStart = 0;
        nPosEnd = strValue.find(RTSP::SPACE, nPosStart);
        if (string::npos == nPosEnd)
        {
            return RTSP::RET_CODE_FAIL;
        }

        strValue = strValue.substr(nPosStart, (nPosEnd - nPosStart));   //lint !e845

        //非法消息
        if (0 != strValue.compare("CLOSE"))
        {
            return RTSP::RET_CODE_FAIL;
        }

        ulPlayInfoType = RTSP::PLAY_INFO_TYPE_CLOSE;
    }

    return RTSP::RET_CODE_OK;
}

/**
* Description:  GetRtpInfo().  获取RTP-Info
* @param  [in]  strField        所有头域字符串
* @param  [out] stResult    解析结果
* @return       long.   返回码
*/
long CRtspPacket::GetRtpInfo
(
    IN  const string&           strField,
    OUT RTSP::DECODE_RESULT&    stResult
)const
{
    string strRtpInfo;
    //获取RTP-Info头域
    long lResult = GetFieldValue(strField, "RTP-Info", strRtpInfo);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //获取RTP-Info头域中第一个seq和rtptime
    string strRtpSeq;
    string strRtpTime;
    
    lResult = GetFieldValue(strRtpInfo, "seq", strRtpSeq, RTSP::SEMI_COLON, RTSP::EQUAL_SIGN);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    lResult = GetFieldValue(strRtpInfo, "rtptime", strRtpTime, RTSP::SEMI_COLON, RTSP::EQUAL_SIGN);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    stResult.bHasRtpInfo = 1;
    stResult.ulRtpSeq    = strtoul(strRtpSeq.c_str(), NULL, 0);
    stResult.ulRtpTime   = strtoul(strRtpTime.c_str(), NULL, 0);

    return RTSP::RET_CODE_OK;
}

/**
* Description:  TrimString().   去掉字符串首尾的空格、Tab
* @param  [io]  str     待处理的字符串
* @return       void.
*/
void CRtspPacket::TrimString
(
    IO  string& str
)
{
    //空字符串直接返回
    if (str.empty())
    {
        return;
    }
    
    //去掉尾部的空格和Tab
    for (size_t nPosLast = str.length() - 1; 0 < nPosLast; nPosLast = str.length() - 1)
    {
        const char cLast = str.at(nPosLast);

        //非空格和Tab，直接退出循环
        if (RTSP::SPACE != cLast && RTSP::TAB != cLast)
        {
            break;
        }

        //去掉空格或Tab
        (void)str.erase(nPosLast, 1);
    }

    //去掉头部的空格和Tab
    for (size_t nPosFirst = 0; nPosFirst < str.length(); )
    {
        const char cFirst = str.at(nPosFirst);

        //非空格和Tab，直接退出循环
        if (RTSP::SPACE != cFirst && RTSP::TAB != cFirst)
        {
            break;
        }

        //去掉空格或Tab
        (void)str.erase(nPosFirst, 1);
    }
}

/**
* Description:  MuiltLineToOneLine().   将头域内容的多行转为单行
* @param  [io]  strFieldContent     待处理的头域内容字符串
* @return       void.
*/
void CRtspPacket::MuiltLineToOneLine
(
    IO  string& strFieldContent
)
{
    size_t nPosStart = 0;
    size_t nPosEnd = strFieldContent.find(RTSP::CRLF, nPosStart);

    //遍历替换
    while (string::npos != nPosEnd)
    {
        //计算LWS的长度
        size_t nLwsCount = 0;
        for (size_t nPosNext = nPosEnd + RTSP::CRLF_LEN; strFieldContent.length() - 1 > nPosNext; ++nPosNext)
        {
            if (RTSP::SPACE != strFieldContent.at(nPosNext) && RTSP::TAB != strFieldContent.at(nPosNext))
            {
                break;
            }

            nLwsCount++;
        }

        //多行，替换为单行
        if (0 < nLwsCount)
        {
            //替换为空格
            (void)strFieldContent.replace(nPosEnd, nLwsCount + RTSP::CRLF_LEN, " ");
            //替换后的值
            nPosStart = nPosEnd + 1;
        }
        else
        {
            //未替换的值
            nPosStart = nPosEnd + RTSP::CRLF_LEN;
        }
        
        //查找下一个CRLF
        nPosEnd = strFieldContent.find(RTSP::CRLF, nPosStart);
    }
}

/**
* Description:  GetNatPacket(). 解析NAT穿越包
* @param  [in]  strPacket      RTSP的NAT包
* @param  [out] stNatResult    NAT解析结果
* @return       long.   返回码
*/
long CRtspPacket::DecodeNatPacket
(
    IN  const string&           strPacket,
    OUT RTSP::NAT_DECODE_RESULT&    stNatResult
) const
{
    //long lResult = RTSP::RET_CODE_OK;

    string strFirstHeader;
    string strField;

    //获取首行首字段和头域内容
    long lResult = GetFirstLine(strPacket, strFirstHeader, strField);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return lResult;
    }

    //只解析NAT穿越包
    if (0 != strFirstHeader.compare(RTSP::METHOD_RECORD))
    {
        return RTSP::RET_CODE_FAIL;
    }

    //将头域内容的多行转为单行
    MuiltLineToOneLine(strField);

    //获取CSeq
    lResult = GetFieldValue(strField, "CSeq", stNatResult.strCSeq);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //获取Session
    lResult = GetFieldValue(strField, "Session", stNatResult.strSession);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }
    //获取x-NAT-Info
    string strNatInfo;
    lResult = GetFieldValue(strField, "x-NAT-Info", strNatInfo);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }
    //获取协议类型
    lResult = GetFieldValue(strNatInfo, "type", stNatResult.strProtocalType, RTSP::SEMI_COLON, RTSP::EQUAL_SIGN);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //获取IP
    lResult = GetFieldValue(strNatInfo, "local_addr", stNatResult.strRemoteIp, RTSP::SEMI_COLON, RTSP::EQUAL_SIGN);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    //获取端口号
    lResult = GetFieldValue(strNatInfo, "local_port", stNatResult.strRemotePort, RTSP::SEMI_COLON, RTSP::EQUAL_SIGN);
    if (RTSP::RET_CODE_OK != lResult)
    {
        return RTSP::RET_CODE_FAIL;
    }

    return RTSP::RET_CODE_OK;
}

/**
* Description:  GetNATRspPacket().     获取NAT响应消息包
* @param  [in]  stNatDecodeResult      解析NAT穿越包的结果
* @param  [out] strPacket   封装后的字符串
* @return       void.
*/
void CRtspPacket::GetNATRspPacket
(
    IN  const RTSP::NAT_DECODE_RESULT&   stNatDecodeResult,
    OUT string&         strPacket
)const
{
    //响应头
    strPacket = "RTSP/1.0 200 OK\r\n";

    //CSeq
    strPacket += "CSeq: ";
    strPacket += stNatDecodeResult.strCSeq;
    strPacket += RTSP::CRLF;

    //Session
    strPacket += "Session: ";
    strPacket += stNatDecodeResult.strSession;
    strPacket += RTSP::CRLF;

    //x-NAT-Info
    strPacket += "x-NAT-Info:";
    strPacket += RTSP::CRLF;

    //type
    strPacket += "type=";
    strPacket += stNatDecodeResult.strProtocalType;
    strPacket += ";";
    //local_addr
    strPacket += "local_addr=";
    strPacket += m_stMediaInfo.szLocalIp;
    strPacket += ";";
    //local_port
    char strTmpPort[6] = {0};
    //UIntToChar(m_stMediaInfo.usLocalPort,strTmpPort);
    strPacket += "local_port=";
	char* pszLocalPort =  itoa(m_stMediaInfo.usLocalPort, strTmpPort, 10); //lint !e734 !e1055
    if (NULL != pszLocalPort)
    {
		strPacket += pszLocalPort;
    }
    strPacket += ";";
    //src_addr
    strPacket += "src_addr=";
    strPacket += stNatDecodeResult.strRemoteIp;
    strPacket += ";";
    //src_port
    strPacket += "src_port=";
    strPacket += stNatDecodeResult.strRemotePort;
    strPacket += ";";
    
    //空行
    strPacket += RTSP::CRLF;
}
