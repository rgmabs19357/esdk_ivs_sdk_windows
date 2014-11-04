#include "StdAfx.h"
#include "CumwCommon.h"
#include "RDAProtStack.h"
#include "ivs_security_md5.h"
#include "ToolsHelp.h"

//PC-LINT说明，本CPP代码中多处非错误，在代码末尾恢复

/*lint -e788*/
/*lint -e826*/
/*lint -e1774*/
CRDAMsgAttr::CRDAMsgAttr():m_usLength  (0)
							,m_pValue  (NULL)
							,m_bDecoded(VOS_FALSE)
							,m_usTag(0)
							,m_enDataType(enRDA_CHAR)
{
    (void)memset(m_szTagName, 0x00, LEN_RDA_ATTRTAG_NAME);
};

LONG CRDAMsgAttr::DecodeAttr(const char *pBuffer, ULONG &ulUsedLength)
{
    LONG lRet = VOS_FAIL;

    switch (m_enDataType)
    {
    case enRDA_CHAR:
    case enRDA_UCHAR:
    case enRDA_INT2:
    case enRDA_UINT2:
    case enRDA_INT4:
    case enRDA_UINT4:
    case enRDA_STRING:
    case enRDA_STRING_NAME:
    case enRDA_STRING_ID:
    case RDA_TYPE_PAY_LOAD_TYPES_INFO:
    {
        lRet = DecodeAttrBaseType(pBuffer, ulUsedLength);
        break;
    }

    case enRDA_CHANNELS_INFO:
    {
        lRet = DecodeAttrChannelsInfo(pBuffer, ulUsedLength);
        break;
    }

    default:
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttr fail, unkwon type(0x%04x)", m_enDataType);
        return VOS_FAIL;
    }
    }

    return lRet;
};

LONG CRDAMsgAttr::EncodeAttr(char *pBuffer, ULONG &ulUsedLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr EncodeAttr Input is invalid.");
        return VOS_FAIL;
    }

    LONG lRet = VOS_FAIL;
    switch (m_enDataType)
    {
    case enRDA_CHAR:
    case enRDA_UCHAR:
    case enRDA_INT2:
    case enRDA_UINT2:
    case enRDA_INT4:
    case enRDA_UINT4:
    case enRDA_STRING:
    case enRDA_STRING_NAME:
    case enRDA_STRING_ID:
    case RDA_TYPE_PAY_LOAD_TYPES_INFO:
    {
        lRet = EncodeAttrBaseType(pBuffer, ulUsedLength);
        break;
    }

    case enRDA_CHANNELS_INFO:
    {
        lRet = EncodeAttrChannelsInfo(pBuffer, ulUsedLength);
        break;
    }

    default:
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr EncodeAttr fail, unkwon datatype(0x%04x)", m_enDataType);
        return VOS_FAIL;
    }
    }

    return lRet;
}

LONG CRDAMsgAttr::DecodeAttrBaseType(const char *pBuffer,
                                     ULONG &     ulUsedLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrBaseType Input is invalid.");
        return VOS_FAIL;
    }

    char *pBufferPos = (char *)pBuffer;

    LONG lRet = DecodeAttrTL(pBufferPos, ulUsedLength);
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    pBufferPos = pBufferPos + TLV_HEAD_LENGTH;

    //获取属性值
    lRet = DecodeAttrValue(pBufferPos, m_enDataType,
                           (USHORT)(ulUsedLength - TLV_HEAD_LENGTH), m_pValue);
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    return VOS_SUCCESS;
}

LONG CRDAMsgAttr::EncodeAttrBaseType(char *pBuffer, ULONG &ulUsedLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr EncodeAttrBaseType input is invalid.");
        return VOS_FAIL;
    }

    char *pBufferPos = pBuffer;
    LONG lRet = EncodeAttrTL(pBufferPos, ulUsedLength);
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    pBufferPos = pBufferPos + TLV_HEAD_LENGTH;

    lRet = EncodeAttrValue(pBufferPos, m_enDataType,
                           (m_usLength - TLV_HEAD_LENGTH), m_pValue);
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    return VOS_SUCCESS;
}

LONG CRDAMsgAttr::DecodeAttrChannelsInfo(const char *pBuffer,
                                         ULONG &     ulUsedLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrChannelsInfo input is invalid.");
        return VOS_FAIL;
    }

    if (NULL == m_pValue)
    {
        IVS_RUN_LOG_ERR("CRDAMsgAttr DecodeAttrChannelsInfo Value is NULL");
        return VOS_FAIL;
    }

    char *pBufferPos = (char *)pBuffer;

    LONG lRet = DecodeAttrTL(pBufferPos, ulUsedLength);
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    pBufferPos = pBufferPos + TLV_HEAD_LENGTH;

    //PCLINT注释说明：使用TLV消息封装，产生了强制类型转换，代码无问题
    RDA_CHANNELS_INFO *pstChannelsInfo = (RDA_CHANNELS_INFO *)m_pValue;/*lint !e826*/

    //获取通道数目
    lRet = DecodeAttrValue(pBufferPos, enRDA_UINT2, sizeof(RDA_UINT2),
                           (char *)&(pstChannelsInfo->usChannelNum));
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    //通道数超过16
    if (MAX_CHANNEL_NUM < pstChannelsInfo->usChannelNum)
    {
        return VOS_FAIL;
    }

    pBufferPos = pBufferPos + sizeof(RDA_UINT2);
    RDA_CHANNEL_INFO *pstChannelInfo =
        (RDA_CHANNEL_INFO *)pstChannelsInfo->stChannelInfo;

    for (long i = 0; i < (long)(pstChannelsInfo->usChannelNum); i++)
    {
        lRet = DecodeAttrValue(pBufferPos, enRDA_UINT2, sizeof(RDA_UINT2),
                               (char *)&(pstChannelInfo->usChannelNo));
        if (VOS_SUCCESS != lRet)
        {
            return VOS_FAIL;
        }

        pBufferPos = pBufferPos + sizeof(RDA_INT2);

        lRet = DecodeAttrValue(pBufferPos, enRDA_STRING_NAME, LEN_RDA_NAME,
                               (char *)pstChannelInfo->szChannelName);
        if (VOS_SUCCESS != lRet)
        {
            return VOS_FAIL;
        }

        pBufferPos = pBufferPos + LEN_RDA_NAME;

        pstChannelInfo++;
    }

    return VOS_SUCCESS;
}

LONG CRDAMsgAttr::EncodeAttrChannelsInfo( char * pBuffer,
                                          ULONG &ulUsedLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr EncodeAttrChannelsInfo input is invalid.");
        return VOS_FAIL;
    }

    if (NULL == m_pValue)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr EncodeAttrChannelsInfo Value is NULL");
        return VOS_FAIL;
    }

    char *pBufferPos = pBuffer;

    LONG lRet = EncodeAttrTL(pBufferPos, ulUsedLength);
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    pBufferPos = pBufferPos + TLV_HEAD_LENGTH;

    RDA_CHANNELS_INFO *pstChannelsInfo = (RDA_CHANNELS_INFO *)m_pValue;
    lRet = EncodeAttrValue(pBufferPos, enRDA_UINT2, sizeof(RDA_UINT2),
                           (char *)&(pstChannelsInfo->usChannelNum));
    if (VOS_SUCCESS != lRet)
    {
        return VOS_FAIL;
    }

    pBufferPos = pBufferPos + sizeof(RDA_UINT2);

    RDA_CHANNEL_INFO *pstChannelInfo
    = (RDA_CHANNEL_INFO *)(pstChannelsInfo->stChannelInfo);        //@@ 454

    for (long i = 0; i < (long)(pstChannelsInfo->usChannelNum); i++)
    {
        lRet = EncodeAttrValue(pBufferPos, enRDA_UINT2, sizeof(RDA_UINT2),
                               (char *)&(pstChannelInfo->usChannelNo));
        if (VOS_SUCCESS != lRet)
        {
            return VOS_FAIL;
        }

        pBufferPos = pBufferPos + sizeof(RDA_INT2);

        lRet = EncodeAttrValue(pBufferPos, enRDA_STRING_NAME, LEN_RDA_NAME,
                               (char *)pstChannelInfo->szChannelName); //@@ 454
        if (VOS_SUCCESS != lRet)
        {
            return VOS_FAIL;
        }

        pBufferPos = pBufferPos + LEN_RDA_NAME;
        pstChannelInfo++;
    }

    return VOS_SUCCESS;
}

LONG CRDAMsgAttr::DecodeAttrTL(const char *pBuffer, ULONG &ulUsedLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrTL Input is invalid");
        return VOS_FAIL;
    }

    char *pBufferPos = ( char *)pBuffer;

    //获取属性标识
    USHORT usTag = (USHORT)VOS_ntohs(*(RDA_INT2*)pBufferPos);

    if (m_usTag != usTag)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrTL Fail "
                       " usTag(0x%04x) != m_usTag(0x%04x).",
                       usTag, m_usTag, VOS_FAIL);
        return VOS_FAIL;
    }

    //获取报文中的属性长度
    pBufferPos   = pBufferPos + sizeof(RDA_INT2);
    ulUsedLength = (USHORT)VOS_ntohs(*(RDA_INT2*)pBufferPos);

    if ((enRDA_STRING != m_enDataType)
        && (enRDA_CHANNELS_INFO != m_enDataType)
        && (RDA_TYPE_PAY_LOAD_TYPES_INFO != m_enDataType))
    {
        if (m_usLength != ulUsedLength)
        {
            IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrTL Fail "
                           " ulUsedLength(0x%04x)!= m_usLength(0x%04x),datatype(0x%04x).",
                           ulUsedLength, m_usLength, m_enDataType);
            return VOS_FAIL;
        }
    }

    m_usLength = (USHORT)ulUsedLength;

    return VOS_SUCCESS;
}

LONG CRDAMsgAttr::EncodeAttrTL(char *pBuffer, ULONG &ulUsedLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrTL input is invalid.");
        return VOS_FAIL;
    }

    ulUsedLength = m_usLength;
    char *pBufferPos = pBuffer;

    USHORT usAttrTag = VOS_htons(m_usTag);

    /* 填充属性标识 */
    if(!CToolsHelp::Memcpy(pBufferPos, sizeof(RDA_INT2), (char *)&usAttrTag, sizeof(RDA_INT2)))
	{
		IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrTL Copy usAttrTag failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    pBufferPos = pBufferPos + sizeof(RDA_INT2);

    USHORT usAttrLen = VOS_htons(m_usLength);

    /* 填充属性长度 */
    if(!CToolsHelp::Memcpy(pBufferPos, sizeof(RDA_INT2), (char *)&usAttrLen, sizeof(RDA_INT2)))
	{
		IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrTL Copy usAttrLen failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    return VOS_SUCCESS;
}

LONG CRDAMsgAttr::DecodeAttrValue
(   const char*     pBuffer,
    RDA_DATA_TYPE   enDataType,
    USHORT          usLength,
    char*           pValue) const
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrValue input is invalid");
        return VOS_FAIL;
    }

    char *pBufferPos = (char *)pBuffer;

    //获取属性值
    if(!CToolsHelp::Memcpy(pValue, usLength, (char *)pBufferPos, usLength))
	{
		IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrValue Copy AttrValue failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //做网络序主机序转换
    //PCLINT注释说明：枚举定义中类型较全面，用于扩展，但本函数只使用部分类型
    switch (enDataType)/*lint !e788*/
    {
    case enRDA_INT2:
    case enRDA_UINT2:
    {
        RDA_UINT2 *pShortValue = (RDA_UINT2 *)pValue;
        *pShortValue = VOS_ntohs(*pShortValue);
        break;
    }
    case enRDA_INT4:
    case enRDA_UINT4:
    {
        RDA_UINT4 *pLongValue = (RDA_UINT4 *)pValue;
        *pLongValue = VOS_ntohl(*pLongValue);
        break;
    }
    default:
    {
		//字符串等会走该分支，不能打印日志
		//IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrValue unknown type %d", enDataType);
        break;
    }
    }

    return VOS_SUCCESS;
}

LONG CRDAMsgAttr::EncodeAttrValue
(   char*           pBuffer,
    RDA_DATA_TYPE   enDataType,
    USHORT          usLength,
    const char*     pValue) const
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttr EncodeAttrValue input is invalid");
        return VOS_FAIL;
    }

    char *pBufferPos = pBuffer;

    /* 填充属性值 */
    if(!CToolsHelp::Memcpy(pBufferPos, usLength, pValue, usLength))
	{
		IVS_RUN_LOG_ERR("RDAMsgAttr EncodeAttrValue Copy AttrValue failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    //PCLINT注释说明：枚举定义中类型较全面，用于扩展，但本函数只使用部分类型
    switch (enDataType)/*lint !e788*/
    {
    case enRDA_INT2:
    case enRDA_UINT2:
    {
        RDA_UINT2 *pShortValue = (RDA_UINT2 *)pBufferPos;
        *pShortValue = VOS_htons(*pShortValue);
        break;
    }
    case enRDA_INT4:
    case enRDA_UINT4:
    {
        RDA_UINT4 *pLongValue = (RDA_UINT4 *)pBufferPos;
        *pLongValue = VOS_htonl(*pLongValue);
        break;
    }
    default:
    {
		//字符串等会走该分支，不能打印日志
		//IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrValue unknown type %d", enDataType);
        break;
    }
    }

    return VOS_SUCCESS;
}

VOID CRDAMsgAttr::PrintAttrChannelsInfo( )
{
    RDA_CHANNELS_INFO *pstChannelsInfo = (RDA_CHANNELS_INFO *)m_pValue;

    if (NULL == pstChannelsInfo)
    {
        return;
    }

    IVS_RUN_LOG_INF("ChannelsInfo:  ChannelNum %d",
                   pstChannelsInfo->usChannelNum);
    RDA_CHANNEL_INFO *pstChannelInfo =
        (RDA_CHANNEL_INFO *)(pstChannelsInfo->stChannelInfo);
    for (long i = 0; i < (long)(pstChannelsInfo->usChannelNum); i++)
    {
        IVS_RUN_LOG_INF("         ChannelInfo  %d,  %s",
                       pstChannelInfo->usChannelNo, pstChannelInfo->szChannelName);
        pstChannelInfo++;
    }
}

void CRDAMsgAttr::PrintAttrPayloadTypesInfo()
{
    if (NULL == m_pValue)
    {
        return;
    }

    RDA_PAY_LOAD_TYPES_INFO* pstInfo = (RDA_PAY_LOAD_TYPES_INFO*)m_pValue;

    IVS_RUN_LOG_INF("PayloadTypesInfo: PayloadTypeNum = %u.", pstInfo->ucPayloadTypeNum);

    unsigned char ucNum = pstInfo->ucPayloadTypeNum;
    if (MAX_PAYLOAD_TYPE_NUM < ucNum)
    {
        ucNum = MAX_PAYLOAD_TYPE_NUM;
    }

    for (unsigned char index = 0; index < ucNum; ++index)
    {
        IVS_RUN_LOG_INF("                 PayloadType = %u.", pstInfo->stPayloadTypeArry[index]);
    }
}

VOID CRDAMsgAttr::Print( )
{
    if (NULL == m_pValue)
    {
        return;
    }

    switch (m_enDataType)
    {
    case enRDA_CHAR:
    case enRDA_UCHAR:
    {
        IVS_RUN_LOG_INF(
                       "Tag[0x%x, %s] Length[%d] Value[%d, 0x%x]",
                       m_usTag, m_szTagName, m_usLength,
                       *(RDA_UCHAR *)m_pValue, *(RDA_UCHAR *)m_pValue);
        break;
    }
    case enRDA_INT2:
    case enRDA_UINT2:
    {
        IVS_RUN_LOG_INF(
                       "Tag[0x%x, %s] Length[%d] Value[%d, 0x%x]",
                       m_usTag, m_szTagName, m_usLength,
                       *(RDA_UINT2 *)m_pValue, *(RDA_UINT2 *)m_pValue);
        break;
    }
    case enRDA_INT4:
    case enRDA_UINT4:
    {
        IVS_RUN_LOG_INF(
                       "Tag[0x%x, %s] Length[%d] Value[%lu, 0x%x]",
                       m_usTag, m_szTagName, m_usLength,
                       *(RDA_UINT4 *)m_pValue, *(RDA_UINT4 *)m_pValue);
        break;
    }
    case enRDA_STRING:
    case enRDA_STRING_NAME:
    case enRDA_STRING_ID:
    {
        IVS_RUN_LOG_INF("Tag[0x%x, %s] Length[%d] Value[%s]",
                       m_usTag, m_szTagName, m_usLength, (char *)m_pValue);
        break;
    }
    case enRDA_CHANNELS_INFO:
    {
        IVS_RUN_LOG_INF("Tag[0x%x, %s] Length[%d] ",
                       m_usTag, m_szTagName, m_usLength);

        PrintAttrChannelsInfo();
        break;
    }

    case RDA_TYPE_PAY_LOAD_TYPES_INFO:
    {
        IVS_RUN_LOG_INF("Tag[0x%x, %s] Length[%d] ",
                       m_usTag, m_szTagName, m_usLength);

        PrintAttrPayloadTypesInfo();
        break;
    }

    default:
    {
		IVS_RUN_LOG_ERR("RDAMsgAttr DecodeAttrValue unknown type %d", m_enDataType);
        return;
    }
    }

    return;
}

void CRDAMsgAttrMgr::RegisterAttr
(   USHORT          usTag,
    USHORT          usLength,
    const char*     pszTagName,
    RDA_DATA_TYPE   enDataType,
    char*           pValue)
{
    if (NULL == pValue)
    {
        IVS_RUN_LOG_ERR("CRDAMsgAttrMgr RegisterAttr input is invalid.");
        return;
    }

    CRDAMsgAttr RDAMsgAttr;

    RDAMsgAttr.m_usTag = usTag;
    RDAMsgAttr.m_usLength = usLength + TLV_HEAD_LENGTH;
    RDAMsgAttr.m_pValue = pValue;
    RDAMsgAttr.m_enDataType = enDataType;

    if(!CToolsHelp::Strncpy(RDAMsgAttr.m_szTagName, sizeof(RDAMsgAttr.m_szTagName), pszTagName, sizeof(RDAMsgAttr.m_szTagName)-1))
	{
		IVS_RUN_LOG_ERR("Copy TagName failed.");
	}

    m_listAttrs.push_back(RDAMsgAttr);

    return;
}

LONG CRDAMsgAttrMgr::DecodeBody(const char *pBuffer,
                                const ULONG ulValidLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttrMgr DecodeBody input is invalid.");
        return VOS_FAIL;
    }

    ;

    RDA_UINT2 usAttrTag = 0;

    ULONG ulUsedLength = 0;
    ULONG ulAttrLength = 0;

    const char *pBufferPos = pBuffer;
    LONG lRet = VOS_FAIL;

    CRDAMsgAttrListIte itAttrList;
    CRDAMsgAttr *pDAMsgAttr = NULL;

    for (itAttrList = m_listAttrs.begin(); itAttrList != m_listAttrs.end(); //lint !e81
         ++itAttrList) //lint !e53
    {
        pDAMsgAttr = &(*itAttrList); //lint !e48 !e50
        pDAMsgAttr->m_bDecoded = VOS_FALSE;
    }

    while (ulValidLength > ulUsedLength)
    {
        if (ulValidLength - ulUsedLength < TLV_HEAD_LENGTH)
        {
            IVS_RUN_LOG_ERR("RDAMsgAttrMgr DecodeBody fail "
                           "the rest length(0x%04x) <= TLV_HEAD_LENGTH(0x%04x)",
                           ulValidLength - ulUsedLength, TLV_HEAD_LENGTH);
            return VOS_FAIL;
        }

        usAttrTag = VOS_ntohs(*(RDA_UINT2*)pBufferPos);

        for (itAttrList = m_listAttrs.begin(); itAttrList != m_listAttrs.end(); //lint !e81
             ++itAttrList) //lint !e53
        {
            pDAMsgAttr = &(*itAttrList); //lint !e48 !e50
            if (((pDAMsgAttr->m_usTag) != usAttrTag)
                || (VOS_FALSE != (pDAMsgAttr->m_bDecoded)))
            {
                continue;
            }

            lRet = pDAMsgAttr->DecodeAttr(pBufferPos, ulAttrLength);
            if (lRet != VOS_SUCCESS)
            {
                IVS_RUN_LOG_ERR("RDAMsgAttrMgr DecodeAttr  fail! pBuffer(0x%04x),error code(0x%04x)",
                               pBuffer, lRet);
                return lRet;
            }

            (*itAttrList).m_bDecoded = VOS_TRUE; //lint !e40 !e48 !e63

            break;
        }
		if(0==ulAttrLength)
		{//没能解出tag
			return VOS_FAIL;
		}
        pBufferPos   += ulAttrLength;
        ulUsedLength += ulAttrLength;
    }

    for (itAttrList = m_listAttrs.begin(); itAttrList != m_listAttrs.end(); //lint !e81
         ++itAttrList) //lint !e53
    {
		// 自研解码器增加Encoding字段，其它厂商没有该字段，将该字段设置为VOS_TRUE，以兼容其它厂商
		pDAMsgAttr = &(*itAttrList);
		if (((pDAMsgAttr->m_usTag) == TagEncoding) && (VOS_FALSE == (pDAMsgAttr->m_bDecoded)))
		{
			(*itAttrList).m_bDecoded = VOS_TRUE;
		}

        if (VOS_FALSE == (*itAttrList).m_bDecoded) //lint !e40 !e48
        {
            IVS_RUN_LOG_ERR("RDAMsgAttrMgr DecodeAttr, fail"
                           " RDAMsgAttr(%.32s) is not decoded.",
                           (*itAttrList).m_szTagName); //lint !e40 !e48
            return VOS_FAIL;
        }
    }

    return VOS_SUCCESS;
}

LONG CRDAMsgAttrMgr::EncodeBody(char *pBuffer, const ULONG ulMaxLength,
                                ULONG &ulValidLength)
{
    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("RDAMsgAttrMgr EncodeBody input is invalid.");
        return VOS_FAIL;
    }

    ULONG ulAttrLength = 0;
    CRDAMsgAttrListIte itAttrList;
    CRDAMsgAttr *pDAMsgAttr = NULL;

    char *pBufferPos = (char *)pBuffer;
    LONG lRet = VOS_FAIL;

    for (itAttrList = m_listAttrs.begin(); itAttrList != m_listAttrs.end(); //lint !e81
         ++itAttrList) //lint !e53
    {
        pDAMsgAttr = &(*itAttrList); //lint !e48 !e50

        lRet = pDAMsgAttr->EncodeAttr(pBufferPos, ulAttrLength);
        if (VOS_SUCCESS != lRet)
        {
            IVS_RUN_LOG_ERR("RDAMsgAttrMgr EncodeBody fail, errorcode(0x%04x)", lRet);
            return VOS_FAIL;
        }

        pBufferPos    += ulAttrLength;
        ulValidLength += ulAttrLength;
        if (ulValidLength > ulMaxLength)
        {
            IVS_RUN_LOG_ERR("RDAMsgAttrMgr EncodeBody, fail!"
                           " ulValidLength(0x%04x) exceed ulMaxLength(0x%04x),"
                           "pBuffer(0x%04x).", ulValidLength, ulMaxLength, pBuffer);
            return VOS_FAIL;
        }
    }

    return VOS_SUCCESS;
}

VOID CRDAMsgAttrMgr::PrintBody( )
{
    CRDAMsgAttrListIte itAttrList;

    for (itAttrList = m_listAttrs.begin(); itAttrList != m_listAttrs.end(); //lint !e81
         ++itAttrList) //lint !e48 !e53 !e55
    {
        CRDAMsgAttr *pRDAMsgAttr = &(*itAttrList);  //lint !e50 !e48 !e55
        pRDAMsgAttr->Print();
    }
}

CRDAProtStack::CRDAProtStack()
{
    (void)memset(m_pszMD5Key, 0, MAX_MD5KEY_LENGTH);
    if(!CToolsHelp::Strncpy(m_pszMD5Key, sizeof(m_pszMD5Key),DEFAULT_MD5_KEY, sizeof(m_pszMD5Key)-1))
	{
		IVS_RUN_LOG_ERR("Copy MD5Key failed.");
	}
    m_ulKeyLen = strlen(m_pszMD5Key);
}

LONG CRDAProtStack::Init(const char * pszMD5Key, ULONG ulKeyLen)
{
    if ((ulKeyLen >= MAX_MD5KEY_LENGTH))
    {
        IVS_RUN_LOG_ERR("RDAProtStack Init, fail. "
                       "ulKeyLen(0x%04x) > MAX_MD5KEY_LENGTH(0x%04x).",
                       ulKeyLen, MAX_MD5KEY_LENGTH);
        return VOS_FAIL;
    }

    if (NULL == pszMD5Key)
    {
        return VOS_SUCCESS;
    }

    if(!CToolsHelp::Memcpy(m_pszMD5Key, MAX_MD5KEY_LENGTH, pszMD5Key, ulKeyLen))
	{
		IVS_RUN_LOG_ERR("RDAProtStack Init Copy MD5Key failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    m_ulKeyLen = ulKeyLen;

    return VOS_SUCCESS;
}

long CRDAProtStack::DecodeMsg
(   const char*         pBuffer,
    const unsigned long ulMaxLength,
    CRDAMsg&            objRDAMsg) const
{
    IVS_DBG_LOG("Decode RDA Message(%s) Begin.", objRDAMsg.m_szRDAMsgTypeName);

    if (NULL == pBuffer)
    {
        IVS_RUN_LOG_ERR("Decode RDA Message(%s) Failed. Message is Empty.", objRDAMsg.m_szRDAMsgTypeName);
        return VOS_FAIL;
    }

    if (ulMaxLength < LEN_RDA_MSG_HEAD)
    {
        IVS_RUN_LOG_ERR("Decode RDA Message(%s) Failed. Message Length[%lu] < RDA Header Length[%u].",
                       objRDAMsg.m_szRDAMsgTypeName, ulMaxLength, LEN_RDA_MSG_HEAD);
        return VOS_FAIL;
    }

    long lResult = VOS_SUCCESS;
    char* pBufferPos = (char *)pBuffer;
    char szMd5Digset[MD5_DIGEST_LENGTH] = {0};

    //解析消息头
    RDAMsgHead& stMsgHead = objRDAMsg.m_stMsgHead;

    stMsgHead.usPackType = VOS_ntohs(*(RDA_UINT2*)pBufferPos);
    pBufferPos += sizeof(stMsgHead.usPackType);

    stMsgHead.usPackID = VOS_ntohs(*(RDA_UINT2*)pBufferPos);
    pBufferPos += sizeof(stMsgHead.usPackID);

    stMsgHead.ulPackLength = VOS_ntohl(*(RDA_UINT4*)pBufferPos);
    pBufferPos += sizeof(stMsgHead.ulPackLength);

    //MD5校验和：MD5哈希计算整个包得到的代码，占16个字节。
    char* pMd5Pos = pBufferPos;
    if(!CToolsHelp::Memcpy(szMd5Digset, MD5_DIGEST_LENGTH, pBufferPos, MD5_DIGEST_LENGTH))
	{
		IVS_RUN_LOG_ERR("Decode RDA Message Copy Md5Digset failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pBufferPos, 0, MD5_DIGEST_LENGTH);
    pBufferPos += MD5_DIGEST_LENGTH;

    stMsgHead.ucProtType = *(RDA_UCHAR*)pBufferPos;
    pBufferPos += sizeof(stMsgHead.ucProtType);

    stMsgHead.ucVersion = *(RDA_UCHAR*)pBufferPos;
    pBufferPos += sizeof(stMsgHead.ucVersion);

    const unsigned short usMsgType = VOS_ntohs(*(RDA_UINT2*)pBufferPos);
    pBufferPos += sizeof(stMsgHead.usMsgType);

    if (usMsgType != stMsgHead.usMsgType)
    {
        IVS_RUN_LOG_ERR("Decode RDA Message(%s) Failed. Message Type[%u] Must be [%u].",
                       objRDAMsg.m_szRDAMsgTypeName, usMsgType, stMsgHead.usMsgType);
        return VOS_FAIL;
    }

    if (stMsgHead.ulPackLength > ulMaxLength)
    {
        IVS_RUN_LOG_ERR(
                       "Decode RDA Message(%s) Failed. Message Packet Length[%lu] > Message Total Length[%lu].",
                       objRDAMsg.m_szRDAMsgTypeName, stMsgHead.ulPackLength, ulMaxLength);
        return VOS_FAIL;
    }

    //协议消息的长度

    unsigned long ulPktLength = stMsgHead.ulPackLength;

	
    //检查MD5摘要
	if (VOS_SUCCESS != md5_check(pBuffer,ulPktLength, szMd5Digset))
	{
		IVS_RUN_LOG_ERR("Decode RDA Message(%s) Failed. MD5 Authorization Check Failed.", objRDAMsg.m_szRDAMsgTypeName);
		return VOS_FAIL;
	}

    //恢复MD5摘要
    if(!CToolsHelp::Memcpy(pMd5Pos, MD5_DIGEST_LENGTH, szMd5Digset, MD5_DIGEST_LENGTH))
	{
		IVS_RUN_LOG_ERR("Decode RDA Message Copy Md5Digset failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    lResult = objRDAMsg.Init();
    if (VOS_SUCCESS != lResult)
    {
        IVS_RUN_LOG_ERR("Init RDA Message Object Failed On Decode RDA Message(%s).",
                       objRDAMsg.m_szRDAMsgTypeName);
        return lResult;
    }

    //解码缓冲数据
    lResult = objRDAMsg.DecodeBody(pBufferPos, (stMsgHead.ulPackLength - LEN_RDA_MSG_HEAD));
    if (VOS_SUCCESS != lResult)
    {
        IVS_RUN_LOG_ERR("Decode Body Failed On Decode RDA Message(%s). ", objRDAMsg.m_szRDAMsgTypeName);
        return lResult;
    }

    IVS_DBG_LOG("Decode RDA Message(%s) Success.", objRDAMsg.m_szRDAMsgTypeName);
    return VOS_SUCCESS;
}

//计算MD5摘要
LONG CRDAProtStack::md5_auth(const char* pBuf, unsigned long bufLen, char* pMd5Digset) const
{
	unsigned long dataLen = bufLen + 3;
	unsigned char* pData = VOS_NEW(pData, dataLen);
	memset(pData, 0, dataLen);
	if(!CToolsHelp::Memcpy(pData, dataLen, pBuf, bufLen))
	{
		IVS_RUN_LOG_ERR("MD5 Copy Buffer failed");
        VOS_DELETE(pData, MULTI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if(!CToolsHelp::Memcpy(pData+bufLen, 3, "NVS", 3))
	{
		IVS_RUN_LOG_ERR("MD5 Copy Buffer failed");
        VOS_DELETE(pData, MULTI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	int iLen = IVS_Security_MD5(pData, (int)dataLen, (unsigned char*)pMd5Digset);
	VOS_DELETE(pData);
	if(MD5_DIGEST_LENGTH==iLen)
	{
		return VOS_SUCCESS;
	}
	else
	{
		return VOS_FAIL;
	}
}

LONG CRDAProtStack::md5_check(const char* pBuf, unsigned long bufLen, const char* pMd5Digset) const
{
	LONG lRet = VOS_SUCCESS;
	char md5buf[16] = {0};
	lRet = md5_auth(pBuf, bufLen, md5buf);
	if(VOS_FAIL==lRet) return lRet;
	int i=0;
	while(i<16)
	{
		if(md5buf[i]!=pMd5Digset[i])
		{
			lRet = VOS_FAIL;
			break;
		}
		i++;
	}
	return lRet;
}
LONG CRDAProtStack::EncodeMsg(IVS_CHAR *pBuffer, const IVS_ULONG ulMaxLength,
								CRDAMsg *pRDAMsg, IVS_ULONG &ulValidLength) const
{
    if ((NULL == pBuffer) || (NULL == pRDAMsg))
    {
        IVS_RUN_LOG_ERR("RDAProtStack EncodeMsg input is invalid.");
        return VOS_FAIL;
    }

    if (ulMaxLength < LEN_RDA_MSG_HEAD)
    {
        IVS_RUN_LOG_ERR("RDAProtStack EncodeMsg, fail "
                       "ulMaxLength(0x%04x) < LEN_RDA_MSG_HEAD(0x%04x)",
                       ulMaxLength, LEN_RDA_MSG_HEAD);
        return VOS_FAIL;
    }

    long lRet = pRDAMsg->Init();
    if (VOS_SUCCESS != lRet)
    {
        IVS_RUN_LOG_ERR("RDAProtStack EncodeMsg Init fail!errorcode(0x%04x)", lRet);
        return lRet;
    }

    char* pBufferPos = pBuffer;
    RDAMsgHead stMsgHead;
    if(!CToolsHelp::Memcpy(&stMsgHead, sizeof(stMsgHead), &(pRDAMsg->m_stMsgHead), sizeof(stMsgHead)))
	{
		IVS_RUN_LOG_ERR("RDAProtStack EncodeMsg Copy MsgHead failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    *(RDA_UINT2*)pBufferPos = VOS_htons(stMsgHead.usPackType);
    pBufferPos += sizeof(stMsgHead.usPackType);

    *(RDA_UINT2*)pBufferPos = VOS_htons(stMsgHead.usPackID);
    pBufferPos += sizeof(stMsgHead.usPackID);

    *(RDA_UINT4*)pBufferPos = VOS_htonl(stMsgHead.ulPackLength);
    RDA_UINT4 *pulMsgLength = (RDA_UINT4 *)pBufferPos;
    pBufferPos += sizeof(stMsgHead.ulPackLength);

    char *pMd5Digest = NULL;
    pMd5Digest = pBufferPos;
    (void)memset(pMd5Digest, 0, sizeof(stMsgHead.arrayMD5));
    pBufferPos += sizeof(stMsgHead.arrayMD5);

    *pBufferPos = (char)(stMsgHead.ucProtType);
    pBufferPos += sizeof(stMsgHead.ucProtType);

    *pBufferPos = (char)(stMsgHead.ucVersion);
    pBufferPos += sizeof(stMsgHead.ucVersion);

    *(RDA_UINT2*)pBufferPos = VOS_htons(stMsgHead.usMsgType);
    pBufferPos += sizeof(stMsgHead.usMsgType);

    ulValidLength = LEN_RDA_MSG_HEAD;

    ULONG ulMsgBodyLength = 0;
    lRet = pRDAMsg->EncodeBody(pBufferPos, ulMaxLength - LEN_RDA_MSG_HEAD,
                               ulMsgBodyLength);
    if (VOS_SUCCESS != lRet)
    {
        IVS_RUN_LOG_ERR("RDAProtStack EncodeMsg EncodeBody fail. errorcode(0x%04x)", lRet);
        return lRet;
    }

    //更新消息长度
    ulValidLength += ulMsgBodyLength;

    //回填报头长度
    (pRDAMsg->m_stMsgHead).ulPackLength = ulValidLength;
    *pulMsgLength = VOS_htonl(ulValidLength);

	//回填MD5摘要
    if (VOS_SUCCESS != md5_auth(pBuffer, ulValidLength, pMd5Digest))
    {
        IVS_RUN_LOG_ERR("RDAProtStack EncodeMsg, insert md5 auth fail.\n");
        return VOS_FAIL;
    }

    return VOS_SUCCESS;
}

LONG CRDAProtStack::GetMsgHeadFromBuff(const char * pBuffer,
                                       const ULONG ulLength, RDAMsgHead &stRDAMsgHead) const
{
    if (ulLength < LEN_RDA_MSG_HEAD)
    {
        IVS_RUN_LOG_ERR("RDAProtStack GetMsgHeadFromBuff, "
                       "ulLength(%u) < LEN_RDA_MSG_HEAD", ulLength);
        return VOS_FAIL;
    }

    char * pBufferPos = (char * )pBuffer;

    //解析消息头
    (void)memset(&stRDAMsgHead, 0x00, sizeof(stRDAMsgHead));

    stRDAMsgHead.usPackType = VOS_ntohs(*(RDA_UINT2*)pBufferPos);
    pBufferPos += sizeof(stRDAMsgHead.usPackType);

    stRDAMsgHead.usPackID = VOS_ntohs(*(RDA_UINT2*)pBufferPos);
    pBufferPos += sizeof(stRDAMsgHead.usPackID);

    stRDAMsgHead.ulPackLength = VOS_ntohl(*(RDA_UINT4*)pBufferPos);
    pBufferPos += sizeof(stRDAMsgHead.ulPackLength);

    pBufferPos += MD5_DIGEST_LENGTH;

    stRDAMsgHead.ucProtType = *(RDA_UCHAR*)pBufferPos;
    pBufferPos += sizeof(stRDAMsgHead.ucProtType);

    stRDAMsgHead.ucVersion = *(RDA_UCHAR*)pBufferPos;
    pBufferPos += sizeof(stRDAMsgHead.ucVersion);

    stRDAMsgHead.usMsgType = VOS_ntohs(*(RDA_UINT2*)pBufferPos);
    pBufferPos += sizeof(stRDAMsgHead.usMsgType);

    return VOS_SUCCESS;
}

void CRDAProtStack::Print( CRDAMsg *pRDAMsg) const
{
    if (NULL == pRDAMsg)
    {
        IVS_RUN_LOG_ERR("RDAProtStack Print input is invalid.");
        return;
    }

    PrintHead(pRDAMsg->m_stMsgHead);

    pRDAMsg->Print();
};

void CRDAProtStack::PrintHead(const RDAMsgHead& stHead) const
{
    IVS_RUN_LOG_INF("Pack Header: Type = %d(1 req, 2 resp). ID = %d. "
                   "Length = %lu(0x%04x). ProtType = %d. Version = %d. MsgType = 0x%04x",
                   stHead.usPackType, stHead.usPackID, stHead.ulPackLength, stHead.ulPackLength,
                   stHead.ucProtType, stHead.ucVersion, stHead.usMsgType);
};

//恢复错误检查

/*lint +e788*/
/*lint +e826*/
/*lint +e1774*/
