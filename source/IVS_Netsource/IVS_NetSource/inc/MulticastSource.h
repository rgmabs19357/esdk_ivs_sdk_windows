/********************************************************************
filename    :    MulticastSource.h
author      :    z90003203
created     :    2013/1/27
description :    组播来源类定义
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/10 初始版本
*********************************************************************/

#pragma once

#ifndef _C_MULTICAST_SOURCE_H_
#define _C_MULTICAST_SOURCE_H_

#include "UdpSource.h"

class CChannelInfo;
class CMulticastSource : public CUdpSource
{
public:
    CMulticastSource(void);
    ~CMulticastSource(void);

    /******************************************************************
     function   : Init
     description: 初始化
     input      : CChannelInfo * pChannelInfo
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    virtual int Init(CChannelInfo* pChannelInfo);

private:
    /******************************************************************
     function   : InitMulticast
     description: 初始化组播设置
     input      : NA
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int InitMulticast();
};

#endif
