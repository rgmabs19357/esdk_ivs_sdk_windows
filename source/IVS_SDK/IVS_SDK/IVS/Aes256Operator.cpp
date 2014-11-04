/********************************************************************
	filename	: 	Aes256Operator.cpp
	author		:	x00192614
	created		:	2012/11/24
	description	:	AES256操作类
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/24 初始版本
*********************************************************************/
#include "Aes256Operator.h"
#include "ivs_error.h"
#include "bp_def.h"

//构造函数，初始化默认的key值
Aes256Operator::Aes256Operator(void)
    :m_keyBytes(AES256_KEYLEN)/*, m_pUserKey(NULL)*/
{
    memset(m_userKey,0,AES256_KEYLEN);
    memcpy(m_userKey, AES256_INITKEY, AES256_KEYLEN);
	try
	{
		m_pAes = new CBB::CAES;  //lint !e1732 !e1733
	}
	catch (...)
	{
		m_pAes  = NULL;
		BP_RUN_LOG_ERR(IVS_FAIL, "exception","alloc aes failed");
	}
	if(NULL != m_pAes)
	{
		m_pAes->SetKey(m_userKey, static_cast<int>(m_keyBytes));
	}
    
    //m_pUserKey = new IVS_CHAR[AES256_KEYLEN];
}

//析构函数，清理uchar数组
Aes256Operator::~Aes256Operator(void)
{
	if (NULL != m_pAes)
	{
		delete m_pAes;
		m_pAes = NULL;
	}
}
//lint +e1540 +e423
// 获取操作NSS的实例对象
Aes256Operator& Aes256Operator::instance()
{ 
    static Aes256Operator _instance; 
    return _instance; 
} 

//加密数据
IVS_UINT32 Aes256Operator::Aes256CbcEncode(const char* pszPlain, char* pszCipher, IVS_UINT64 ullLenCipher)
{
    CHECK_POINTER(pszPlain,IVS_PARA_INVALID);
    CHECK_POINTER(pszCipher,IVS_PARA_INVALID);
    int enc_lenght = CBB::CAES::GetStringEncryptedBufferLen(pszPlain);
    if (enc_lenght > static_cast<IVS_INT32>(ullLenCipher))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Aes256 Cbc Encode","ullLenCipher is too small");
        return IVS_FAIL;
    }
	CHECK_POINTER(m_pAes,IVS_PARA_INVALID);
    m_pAes->EncryptString(pszPlain, pszCipher, enc_lenght);
    return IVS_SUCCEED;
}//lint !e1762 不需要

//解密数据
IVS_UINT32 Aes256Operator::Aes256CbcDecode(const char* pszCipher, char* pszPlain, IVS_UINT64 ullLenPlain)
{
    CHECK_POINTER(pszCipher,IVS_PARA_INVALID);
    CHECK_POINTER(pszPlain,IVS_PARA_INVALID);
	CHECK_POINTER(m_pAes,IVS_PARA_INVALID);
    m_pAes->DecryptString(pszCipher, pszPlain, static_cast<int>(ullLenPlain));
    return IVS_SUCCEED;
}//lint !e1762 不需要
