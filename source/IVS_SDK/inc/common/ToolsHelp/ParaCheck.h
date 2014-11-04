/********************************************************************
	filename	: 	ParaCheck.h
	author		:	z00201790
	created		:	2013/01/17
	description	:	对参数进行校验的类
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/1/17 初始版本
*********************************************************************/

#ifndef __PARACHECK_H__
#define __PARACHECK_H__

#include "hwsdk.h"
class CParaCheck
{
public:

    /***********************************************************************************
    * name       : CheckIsIvsBool
    * description: 检查输入的参数是否为0/1型的IVS_BOOL
    * input      : bIvsBool:
    * output     : NA
    * return     : 如果在返回内返回IVS_SUCCEED,失败返回IVS_PARA_INVALID
    * remark     : NA
    ***********************************************************************************/
    static IVS_INT32 CheckIsIvsBool(IVS_BOOL bIvsBool);
};

#endif

