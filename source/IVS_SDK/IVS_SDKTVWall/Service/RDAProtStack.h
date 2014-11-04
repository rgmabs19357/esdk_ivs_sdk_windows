/********************************************************************
    filename	: 	CRDAProtStack.h
    author		:	wx58056
    created		:	2011/10/28
    description	:	RDA协议栈
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/10/28 初始版本
 *********************************************************************/
#ifndef RDAPROTSTACK_H
#define RDAPROTSTACK_H

/*!< 链表头文件 */
#include <list>

/*!< 模块基础头文件 */
#include "vos.h"

/*!< RDA协议头文件 */
#include "RDAProt.h"

#include "hwsdk.h"
using namespace std;

typedef enum enProtStackLogLevel
{
    PROT_LOG_ERROR   = 3,        //一般错误
    PROT_LOG_WARNING = 4,    //警告
    PROT_LOG_INFO  = 6,          //一般的正常信息
    PROT_LOG_DEBUG = 7     //调试信息
} ProtStackLogLevel;

//RDA协议消息属性对象
class CRDAMsgAttr
{
public:
    CRDAMsgAttr();
    ~CRDAMsgAttr()
    {
        m_usTag = 0;
        m_usLength   = 0;
        m_enDataType = enRDA_CHAR;
        m_pValue   = NULL;
        m_bDecoded = VOS_FALSE;
        memset(m_szTagName, 0x00, LEN_RDA_ATTRTAG_NAME);
    };

public:

    /* 解析属性 */
    LONG DecodeAttr(const char *pBuffer, ULONG &ulUsedLength);

    /* 封装属性 */
    LONG EncodeAttr(char *pBuffer, ULONG &ulUsedLength);

    VOID Print( );

private:
    LONG DecodeAttrBaseType(const char *pBuffer, ULONG &ulUsedLength);

    LONG EncodeAttrBaseType(char *pBuffer, ULONG &ulUsedLength);

    LONG DecodeAttrChannelsInfo(const char *pBuffer, ULONG &ulUsedLength);

    LONG EncodeAttrChannelsInfo(char *pBuffer, ULONG &ulUsedLength);

    LONG DecodeAttrTL( const char *pBuffer, ULONG &ulUsedLength );
    LONG EncodeAttrTL( char *pBuffer, ULONG &ulUsedLength);

    LONG DecodeAttrValue
    (   const char*     pBuffer,
        RDA_DATA_TYPE   enDataType,
        USHORT          usLength,
        char*           pValue) const;

    LONG EncodeAttrValue
    (   char*           pBuffer,
        RDA_DATA_TYPE   enDataType,
        USHORT          usLength,
        const char*     pValue) const;

    VOID PrintAttrChannelsInfo();

    /**
     * Description:  PrintAttrPayloadTypesInfo().    打印Payload类型信息属性
     * @return       void.
     */
    void PrintAttrPayloadTypesInfo();

public:
    USHORT m_usLength;   //L 属性长度，包括 T 和 V
    char        * m_pValue;     //V 属性值

    VOS_BOOLEAN m_bDecoded;

public:
    USHORT m_usTag;      //T 属性标识
    char m_szTagName[LEN_RDA_ATTRTAG_NAME];

    RDA_DATA_TYPE m_enDataType; //属性的数据类型
};

typedef std::list <CRDAMsgAttr>   CRDAMsgAttrList;
typedef CRDAMsgAttrList::iterator CRDAMsgAttrListIte;

class CRDAMsgAttrMgr
{
public:
    CRDAMsgAttrMgr(){};
    ~CRDAMsgAttrMgr(){};

    void RegisterAttr
    (   USHORT          usTag,
        USHORT          usLength,
        const char*     pszTagName,
        RDA_DATA_TYPE   enDataType,
        char*           pValue);

    LONG DecodeBody(const char *pBuffer, const ULONG ulValidLength);

    LONG EncodeBody(char *pBuffer, const ULONG ulMaxLength,
                    ULONG &ulValidLength);

    VOID PrintBody( );
public:

    CRDAMsgAttrList m_listAttrs;
};

class CRDAProtStack
{
public:
    CRDAProtStack();
    ~CRDAProtStack(){};

public:
    LONG Init(const char * pArrayMD5Key, const ULONG ulKeyLen);

    /**
     * Description:  DecodeMsg().   解析消息
     * @param  [in]  pBuffer             消息buffer
     * @param  [in]  ulMaxLength     消息 长度
     * @param  [in]  objRDAMsg       解析结果保存在消息对象引用中
     * @return       long.       返回码
     */
    long DecodeMsg
    (   const char*         pBuffer,
        const unsigned long ulMaxLength,
        CRDAMsg&            objRDAMsg) const;

    LONG EncodeMsg( IVS_CHAR *pBuffer, const IVS_ULONG ulMaxLength,
                    CRDAMsg *pRDAMsg, IVS_ULONG &ulValidLength) const;

    LONG GetMsgHeadFromBuff(const char * pBuffer, const ULONG ulLength,
                            RDAMsgHead &stRDAMsgHead) const;

    void Print( CRDAMsg *pRDAMsg) const;

    void PrintHead(const RDAMsgHead& stHead) const;

private:
	//计算MD5摘要
	LONG md5_auth(const char* pBuf, unsigned long bufLen, char* pMd5Digset) const;
	LONG md5_check(const char* pBuf, unsigned long bufLen, const char* pMd5Digset) const;
    char m_pszMD5Key[MAX_MD5KEY_LENGTH];
    ULONG m_ulKeyLen;
};

#endif //RDAPROTSTACK_H
