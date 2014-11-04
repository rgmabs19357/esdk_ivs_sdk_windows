/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_MLIB_Player
//  文 件 名：  IVS_Sdp.cpp
//  文件说明：  SDP解析函数
//  版      本： eIVS V100R001C30
//  作      者： yangjunhai/y00182336
//  创建日期：2012/08/14
//
//  修改标识：
//  修改说明：
*******************************************************************************/
#include "ParseSdp.h"
#include "bp_log.h"
#include "ivs_error.h"

/***************************************************************************
 函 数 名 : parseSdp
 功能描述 : 解析SDP
 输入参数 : pSdpMsg                 待解析的字符串
 输出参数 : pMediaKeyInfo           媒体密钥信息
			pMediaInfo              获取到的媒体编码类型与第三方SDK相对路径
 返 回 值 : IVS_SUCCEED-成功           其他-失败
 SDP格式如下：
 m=video 51372  RTP/AVP/TCP 31
 a=crypto:1 HW_IVS_IPC_ENCRYPT_SIMPLE StartTime:YYYYMMddHHmmss StopTime:YYYYMMddHHmms
 m=audio 49170  RTP/AVP/TCP 0
 a=crypto:1 HW_IVS_IPC_ENCRYPT_SIMPLE StartTime:YYYYMMddHHmmss StopTime:YYYYMMddHHmms
***************************************************************************/
IVS_INT32 parseSdp(const char* pSdp, MEDIA_KEY_INFO* pMediaKeyInfo,
							MEDIA_INFO_SDP* pMediaInfo,STORAGESECRET_INFO* pStorageSecretInfo)
{
	if (NULL == pMediaKeyInfo || NULL == pMediaInfo || NULL == pStorageSecretInfo)
	{
		return IVS_FAIL;
	}
    std::string strSdp(pSdp);
    std::string strBuff = "";
    unsigned int nNextStart = 0;
    bool bRet = getNextLine(strSdp, nNextStart, strBuff);
    int nRet = IVS_SUCCEED;
    IVS_ULONG ulKeyType = KEY_TYPE_VIDEO;
    char* pTmp  = NULL;
  //  bool bFirst = true;

    while (bRet && (IVS_SUCCEED == nRet))
    {
        pTmp = const_cast <char*> (strBuff.c_str());
        JUMP_SPACE(pTmp);

        //全部都是空格，进行下一行
        if ('\0' == *pTmp)
        {
            bRet = getNextLine(strSdp, nNextStart, strBuff);
            continue;
        }

        //记录第一个不为空的字符
        char cCode = *pTmp;
        pTmp++;
        JUMP_SPACE(pTmp);
        if ('=' == *pTmp)
        {
            pTmp++;
            JUMP_SPACE(pTmp);

            //if (('v' != tolower(cCode)) && bFirst)
            //{
            //    return IVS_FAIL;
            //}

            switch (tolower(cCode))
            {
            case 'o':
                nRet = parseStreamID(pTmp, pMediaInfo->SessionId);
                break;
            case 'u':
                strncpy(pMediaInfo->SessionUrl, pTmp, strlen(pTmp));
                break;
            case 'm':

                // 解析出媒体信息
                nRet = parseMediaInfo(pTmp, &ulKeyType, &pMediaInfo->AudioDecoderType,
                                      &pMediaInfo->VideoDecoderType,
                                      &pMediaInfo->port);
                break;
            case 'a':
            {
                if (0 == strncmp(pTmp, SDP_ATTRIBUTES_CRYPTOSTORAGE, strlen(SDP_ATTRIBUTES_CRYPTOSTORAGE)))
                {
                    (void)parseStorageSecretKey(pTmp, pStorageSecretInfo);
                    break;
                }

                if (0 == strncmp(pTmp, SDP_ATTRIBUTES_CRYPTO, strlen(SDP_ATTRIBUTES_CRYPTO)))
                {
                    nRet  = parseManuType(pTmp, &pMediaInfo->ManuType);
                    nRet += parseTransSecretKey(pTmp, ulKeyType, pMediaKeyInfo);
                    break;
                }

                if (0 == strncmp(pTmp, SDP_CER_INDEX, strlen(SDP_CER_INDEX)))
                {
                    nRet = parseCerIndex(pTmp, pStorageSecretInfo);
                    break;
                }
				if (0 == strncmp(pTmp, SDP_RTP_MAP, strlen(SDP_RTP_MAP)))
				{
					//解析视频流类型
					nRet = parseRtpMap(pTmp, pMediaKeyInfo);
					break;
				}

                break;
            }
            case 'b':
            {
                break;
            }
            default:
                break;
            }

            if (IVS_SUCCEED != nRet)
            {
                return IVS_FAIL;
            }

          //  bFirst = false;
        }
        else
        {
            return IVS_FAIL;
        }

        // 进行下一行的操作
        bRet = getNextLine(strSdp, nNextStart, strBuff);
    }

    return IVS_SUCCEED;
}

/***************************************************************************
 函 数 名 : getNextLine
 功能描述 : 切分SDP消息
 输入参数 : strSdpMsg                   待解析的字符串
            nNextStart                  偏移值
 输出参数 : strBuff                     获取到的下一行SDP字符串
 返 回 值 : true 成功           fail 失败
***************************************************************************/
bool getNextLine(const std::string &strSdpMsg,
                 unsigned int &     nNextStart,
                 std::string  &     strBuff)
{
    unsigned int nLen = 0;
	unsigned int Sdplen = strSdpMsg.length();
	if (nNextStart >= Sdplen)
	{
		return false;
	}
    const char *pszStart = strSdpMsg.c_str() + nNextStart;
    while (('\0' != *pszStart) && ('\n' != *pszStart) && ('\r' != *pszStart))
    {
        pszStart++;
        nLen++;
    }
    strBuff = strSdpMsg.substr(nNextStart, nLen);
    nNextStart += nLen;
    nNextStart++;
    return true;
}

/***************************************************************************
 函 数 名 : parseStreamID
 功能描述 : 解析SessionID
 输入参数 : pSdpMsg                     待解析的字符串
 输出参数 : pSessionId                  获取到的SessionID
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseStreamID(const char* pSdpMsg, char* pSessionId)
{
    if ((NULL == pSdpMsg) || (NULL == pSessionId))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse Stream ID", "parse streamID fail.");
        return IVS_FAIL;
    }

    std::string strTmp(pSdpMsg);

    //std::string::size_type Pos = ;
    std::string::size_type npos = strTmp.find(" ");
    if (std::string::npos == npos)
    {
        return IVS_FAIL;
    }

    strTmp = strTmp.substr(npos + 1);

    //  IVS_LOG(LOG_INFO, "strTmp=%s", strTmp.c_str());
    npos = strTmp.find(" ");
    if (std::string::npos == npos)
    {
        return IVS_FAIL;
    }

    std::string strValue = "";
    strValue = strTmp.substr(0, npos);

    //(LOG_INFO, "strValue=%s", strValue.c_str());
    strncpy(pSessionId, strValue.c_str(), strValue.length());

    //IVS_LOG(LOG_INFO, "streamID=%s", pSessionId);
    return IVS_SUCCEED;
} //lint !e818

/***************************************************************************
 函 数 名 : parseApplications
 功能描述 : 解析媒体类型，获取TCP端口号
 输入参数 : pAppliMsg                   待解析的字符串
 输出参数 : pPort                       获取到的TCP端口号
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseApplications(const char* pTmp, unsigned long* pPort)
{
    if ((NULL == pTmp) || (NULL == pPort))
    {
        return IVS_FAIL;
    }

    std::string strTmp(pTmp);
    std::string::size_type Pos = strTmp.find(" ");
    if (std::string::npos == Pos)
    {
        return IVS_FAIL;
    }

    strTmp = strTmp.substr(Pos + 1);
    Pos = strTmp.find("TCP");
    if (std::string::npos == Pos)
    {
        return IVS_FAIL;
    }

    std::string strValue = "";
    strValue = strTmp.substr(0, Pos);
    *pPort = (unsigned long)atoi(strValue.c_str());
    return IVS_SUCCEED;
}

/***************************************************************************
 函 数 名 : parseDecoderType
 功能描述 : 解析编码类型
 输入参数 : pTmpMsg                     待解析的字符串
 输出参数 : pDecoderType                获取到的编码类型
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseDecoderType(const char* pTmp, IVS_ULONG* pDecoderType)
{
    if ((NULL == pTmp) || (NULL == pDecoderType))
    {
        return IVS_FAIL;
    }

    std::string strTmp(pTmp);
    std::string::size_type pos = strTmp.rfind(" ");
    if (std::string::npos == pos)
    {
        return IVS_FAIL;
    }

    std::string strValue = strTmp.substr(pos + 1);
    *pDecoderType = (IVS_ULONG)atoi(strValue.c_str());
    return IVS_SUCCEED;
}

/***************************************************************************
 函 数 名 : parseManuType
 功能描述 : 解析厂商类型
 输入参数 : pTmpMsg                     待解析的字符串
 输出参数 : pManuType                   获取到的厂商类型
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseManuType(char* pTmp, IVS_ULONG* ulManuType)
{
    if ((NULL == pTmp) || (NULL == ulManuType))
    {
        return IVS_FAIL;
    }

    std::string strTmp(pTmp);
    std::string::size_type pos = strTmp.find(SDP_ATTRIBUTES_DECODER_TYPE);
    if (std::string::npos == pos)
    {
        return IVS_SUCCEED;
    }

    strTmp = strTmp.substr(pos);
    *ulManuType = (IVS_ULONG)atoi(strTmp.c_str());
    return IVS_SUCCEED;
} //lint !e818

/***************************************************************************
 函 数 名 : parseRtpMap
 功能描述 : 解析音视频类型
 输入参数 : pTmp                     待解析的字符串
 输出参数 : pMediaKeyInfo            获取到的视频、音频流类型
 返 回 值 : IVS_SUCCEED-成功           其他-失败
***************************************************************************/
int parseRtpMap(const char* pTmp,MEDIA_KEY_INFO* pMediaKeyInfo)
{
	if (NULL == pTmp || NULL == pMediaKeyInfo)
	{
		return IVS_FAIL;
	}

	/*
	 * decode rtp payload value, add by w00210470
	 */
	unsigned int PtVal = 0;
	const char *pVal = strchr(pTmp, ':');
	if (NULL != pVal)
	{
		pVal++;
		PtVal = atoi(pVal);//lint !e732
	}

	std::string strTmp(pTmp);
	std::string::size_type pEndPos = strTmp.find(SDP_VIDEO_H264_TYPE);
	do 
	{
		//H264
		if (std::string::npos != pEndPos)
		{
			pMediaKeyInfo->enVideoDecoderType = VIDEO_DEC_H264;
			pMediaKeyInfo->uPTVideo = PtVal;
			break;
		}
		//Mjpeg
		pEndPos = strTmp.find(SDP_VIDEO_MJPEG_TYPE);
		if (std::string::npos != pEndPos)
		{
			pMediaKeyInfo->enVideoDecoderType = VIDEO_DEC_MJPEG ;
			pMediaKeyInfo->uPTVideo = PtVal;
			break;
		}
		//PCMU
		pEndPos = strTmp.find(SDP_AUDIO_PCMU_TYPE);
		if (std::string::npos != pEndPos)
		{
			pMediaKeyInfo->enAudioDecoderType = AUDIO_DEC_G711U;
			pMediaKeyInfo->uPTAudio = PtVal;
			break;
		}
		//PCMA
		pEndPos = strTmp.find(SDP_AUDIO_PCMA_TYPE);
		if (std::string::npos != pEndPos)
		{
			pMediaKeyInfo->enAudioDecoderType = AUDIO_DEC_G711A;
			pMediaKeyInfo->uPTAudio = PtVal;
			break;
		} 
	} while (false); //lint !e717
	return IVS_SUCCEED;
}


/***************************************************************************
 函 数 名 : parseAttributes
 功能描述 : 解析加密类类型与密钥
 输入参数 : pTmpMsg                     待解析的字符串
            ulKeyType                   媒体类型
输出参数 : pMediaKeyInfo           媒体密钥信息
返 回 值 : IVS_SUCCEED-成功           其他-失败

			字符串类型：
 //a=crypto:1 HW_IVS_IPC_ENCRYPT_SIMPLE inline:d0RmdmcmVCspeEc3QGZiNWpVLFJhQX1cfHAwJSoj
***************************************************************************/

//a=crypto:1 HW_IVS_IPC_ENCRYPT_SIMPLE inline:d0RmdmcmVCspeEc3QGZiNWpVLFJhQX1cfHAwJSoj
int parseTransSecretKey(const char* pTmp, IVS_ULONG ulKeyType, MEDIA_KEY_INFO* pMediaKeyInfo)
{
    if (NULL == pTmp || NULL == pMediaKeyInfo)
    {
        return IVS_FAIL;
    }
    std::string strTmp(pTmp);
	//扰码流--明文头
	std::string::size_type pEndPos = strTmp.find(IVS_SECRET_KEY_TYPE_SIMPLE_HEADER_PLAIN);
	int iSecretType = HW_NO_DEC_TYPE;
	do 
	{
		if (std::string::npos != pEndPos)
		{
			iSecretType = HW_IPC_ENCRYPT_SIMPLE_HEADER;
			break;
		}
		//扰码流--密文头
		pEndPos = strTmp.find(IVS_SECRET_KEY_TYPE_SIMPLE);
		if (std::string::npos != pEndPos)
		{
			iSecretType = HW_IPC_ENCRYPT_SIMPLE ;
			break;
		} 

		//AES-256部分加密
		pEndPos = strTmp.find(IVS_SECRET_KEY_TYPE_AES256_PARTIAL);
		if (std::string::npos != pEndPos)
		{
			iSecretType = HW_IPC_ENCRYPT_AES256_PARTIAL;
			break;
		}

		//AES256(明文头)
		pEndPos = strTmp.find(IVS_SECRET_KEY_TYPE_AES256);
		if (std::string::npos != pEndPos)
		{
			iSecretType = HW_IPC_ENCRYPT_AES256;
			break;
		} 
		return IVS_SUCCEED;
	} while (false); //lint !e717
	//没有找到crypto,返回失败
    std::string::size_type pStartPos = strTmp.find(SDP_ATTRIBUTES_CRYPTO);
    if (std::string::npos == pStartPos)
    {
        return IVS_FAIL;
    }

    //IVS_ULONG* pKeyType = NULL;
    //char* pSecretKey = NULL;
    if (KEY_TYPE_AUDIO == ulKeyType)
    {
		pMediaKeyInfo->iAudioKeyType = iSecretType;
    }
    else if (KEY_TYPE_VIDEO == ulKeyType)
    {
		pMediaKeyInfo->iVideoKeyType = iSecretType;
    }
	//(void)parseStarTimeAndEndTime(pTmp,pMediaKeyInfo);
	(void)parseSessionID(pTmp,pMediaKeyInfo);
    return IVS_SUCCEED;
}

/***************************************************************************
 函 数 名 : parseStorageSecretKey
 功能描述 : 解析存储加密密钥
 输入参数 : pTmpMsg                     待解析的字符串
 输出参数 : pStorageSecretInfo          获取到存储加密信息
 返 回 值 : IVS_SUCCEED-成功           其他-失败

 字符串类型：
 a=cryptostorage:1 ASYM_RSA_2048_AES_CM_256 inline:加密码流的Key的密文
 b=cerindex:证书索引号
***************************************************************************/
int parseStorageSecretKey(const char* pTmp, STORAGESECRET_INFO* pStorageSecretInfo)
{
	CHECK_POINTER(pTmp, IVS_FAIL);
	CHECK_POINTER(pStorageSecretInfo, IVS_FAIL);

    std::string strTmp(pTmp);

    std::string::size_type pStartPos = strTmp.find(SDP_ATTRIBUTES_CRYPTOSTORAGE);

    if (std::string::npos == pStartPos)
    {
        return IVS_FAIL;
    }

    std::string::size_type pEndPos = strTmp.find(IVS_SECRET_STORAGE_KEY_TYPE);
    if (std::string::npos == pEndPos)
    {
        return IVS_SUCCEED;
    }

    std::string strValue = "";
    strValue = strTmp.substr((pStartPos + strlen(SDP_ATTRIBUTES_CRYPTOSTORAGE)), (pEndPos - 1));

    pStorageSecretInfo->keyTpye = (IVS_ULONG)atoi(strValue.c_str()); //lint !e613

    pStartPos = strTmp.find(SDP_ATTRIBUTES_INLINE);
    if (std::string::npos == pStartPos)
    {
        return IVS_FAIL;
    }

    strValue.clear();
    strValue = strTmp.substr(pStartPos + strlen(SDP_ATTRIBUTES_INLINE));
    strncpy(pStorageSecretInfo->keyValue, strValue.c_str(), strValue.length());

    return IVS_SUCCEED;
}

/***************************************************************************
 函 数 名 : parseCerIndex
 功能描述 : 解析存储加密证书号
 输入参数 : pTmpMsg                     待解析的字符串
 输出参数 : pStorageSecretInfo          获取到存储加密信息
 返 回 值 : IVS_SUCCEED-成功           其他-失败

 字符串类型：
 b=cerindex:证书索引号
***************************************************************************/
int parseCerIndex(const char* pTmp, STORAGESECRET_INFO* pStorageSecretInfo)
{
    std::string strTmp(pTmp);

    std::string::size_type pStartPos = strTmp.find(SDP_CER_INDEX);

    if (std::string::npos == pStartPos)
    {
        return IVS_FAIL;
    }

    std::string strValue = "";
    strValue = strTmp.substr((pStartPos + strlen(SDP_CER_INDEX)), strTmp.length());

    pStorageSecretInfo->cerIndex = (IVS_ULONG)atoi(strValue.c_str()); //lint !e613

    return IVS_SUCCEED;
}

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
                   IVS_ULONG* pVideoDecoderType, IVS_ULONG* pPort)
{
	CHECK_POINTER(pTmp, IVS_FAIL);
	CHECK_POINTER(pKeyType, IVS_FAIL);
	CHECK_POINTER(pAudioDecoderType, IVS_FAIL);
	CHECK_POINTER(pVideoDecoderType, IVS_FAIL);
	CHECK_POINTER(pPort, IVS_FAIL);

    int nRet = 0;

    if (0 == strncmp(pTmp, SDP_MEDIA_TYPE_AUDIO, strlen(SDP_MEDIA_TYPE_AUDIO)))
    {
        *pKeyType = KEY_TYPE_AUDIO;
        nRet = parseDecoderType(pTmp, pAudioDecoderType);
    }
    else if (0 == strncmp(pTmp, SDP_MEDIA_TYPE_VIDEO, strlen(SDP_MEDIA_TYPE_VIDEO)))
    {
        *pKeyType = KEY_TYPE_VIDEO;
        nRet = parseDecoderType(pTmp, pVideoDecoderType);
    }
    else if (0 == strncmp(pTmp, SDP_MEDIA_SYPE_APPLICATION, strlen(SDP_MEDIA_SYPE_APPLICATION)))
    {
        nRet = parseApplications(pTmp, pPort);
    }

    return nRet; //lint !e438 !e550 !e818
} //lint !e438 !e550 !e818

/***************************************************************************
 函 数 名 : parseStarTimeAndEndTime
 功能描述 : 解析媒体媒体开始结束时间
 输入参数 : pTmp                    待解析的字符串
 输出参数 : pMediaKeyInfo           媒体密钥信息

 返 回 值 : IVS_SUCCEED-成功        其他-失败
***************************************************************************/
int parseStarTimeAndEndTime(const char* pTmp,MEDIA_KEY_INFO* pMediaKeyInfo)
{
	if (NULL == pMediaKeyInfo)
	{
		return IVS_FAIL;
	}
	std::string strTmp(pTmp);
	std::string::size_type pStartPos = strTmp.find(SDP_MEDIA_STAR_TIME);
	if (std::string::npos == pStartPos)
	{
		return IVS_FAIL;
	}
	std::string strValue = "";
	strValue = strTmp.substr((pStartPos + strlen(SDP_MEDIA_STAR_TIME)), SDP_TIME_LEN);
	//获取starTime
	memcpy_s(pMediaKeyInfo->szStarTime,SDP_TIME_LEN,strValue.c_str(), SDP_TIME_LEN);
	//准备获取endTime
	strValue.clear();
	pStartPos = strTmp.find(SDP_MEDIA_END_TIME);
	if (std::string::npos == pStartPos)
	{
		return IVS_FAIL;
	}
	strValue = strTmp.substr((pStartPos + strlen(SDP_MEDIA_END_TIME)), SDP_TIME_LEN);
	memcpy_s(pMediaKeyInfo->szEndTime,SDP_TIME_LEN,strValue.c_str(), SDP_TIME_LEN);
	return IVS_SUCCEED;
}

int parseSessionID(const char* pTmp,MEDIA_KEY_INFO* pMediaKeyInfo)
{
	if (NULL == pMediaKeyInfo)
	{
		return IVS_FAIL;
	}
	std::string strTmp(pTmp);
	std::string::size_type pStartPos = strTmp.find(SDP_SESSION_ID);
	if (std::string::npos == pStartPos)
	{
		return IVS_FAIL;
	}
	unsigned int uiStrSize = strTmp.size();
	unsigned int uiStartIdex = pStartPos + strlen(SDP_SESSION_ID);
	std::string strValue = "";
	strValue = strTmp.substr(uiStartIdex, uiStrSize - uiStartIdex);
	memset(&pMediaKeyInfo->szSessionID,0,SESSION_ID_LEN);//lint !e545
	memcpy_s(pMediaKeyInfo->szSessionID,SESSION_ID_LEN,strValue.c_str(), strValue.length());
	return IVS_SUCCEED;
}
