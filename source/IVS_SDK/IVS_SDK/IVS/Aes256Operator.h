/********************************************************************
	filename	: 	Aes256Operator.h
	author		:	x00192614
	created		:	2012/11/24
	description	:	AES256操作类
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/24 初始版本
*********************************************************************/
#ifndef __AES256OPERATOR_H__
#define __AES256OPERATOR_H__

#include "hwsdk.h"
#include "ivs_security_aes.h"
#include "SDKDef.h"

class Aes256Operator
{
public:
    Aes256Operator(void);
    ~Aes256Operator(void);

    static Aes256Operator& instance();

    /******************************************************************
     function   : Aes256CbcEncode
     description: AES256加密
     input      : const char * pszPlain
     input      : char * pszCipher
     input      : IVS_UINT64 ullLenCipher
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 Aes256CbcEncode(const char* pszPlain, char* pszCipher, IVS_UINT64 ullLenCipher);

    /******************************************************************
     function   : Aes256CbcDecode
     description: AES256解密
     input      : const char * pszCipher
     input      : char * pszPlain
     input      : IVS_UINT64 ullLenPlain
     output     : NA
     return     : IVS_UINT32
    *******************************************************************/
    IVS_UINT32 Aes256CbcDecode(const char* pszCipher, char* pszPlain, IVS_UINT64 ullLenPlain);

private:
    IVS_UINT32 m_keyBytes;
    IVS_UCHAR m_userKey[AES256_KEYLEN];
    CBB::CAES* m_pAes;
};

#endif //__AES256OPERATOR_H__
