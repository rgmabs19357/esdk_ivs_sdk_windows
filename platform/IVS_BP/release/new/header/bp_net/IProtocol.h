/********************************************************************
filename    :    IProtocol.h
author      :    j00213308
created     :    2012/04/06
description :    协议栈接口
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/04/06 初始版本
*********************************************************************/
#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#include "bp_def.h"
#include "Cmd.h"
#include "IMsgHandle.h"
#include "ILink.h"

NAMESPACE_BP_BEGIN        // IVS NAMESPACE

// 协议接口
class IProtocol
{
public:
    IProtocol(void)
        : m_pILink(NULL)
        , m_pMsgHandle(NULL)
    {}
    virtual ~IProtocol(void) { HW_DELETE(m_pMsgHandle); }
private:
    IProtocol(const IProtocol&src)
    {
    }

    IProtocol& operator=(const IProtocol&)
    {
        return *this;
    }

public:

    /**************************************************************************
    * name       : HandleCmd
    * description: 处理命令消息
    * input      : pCmd : 需处理的命令消息
    * output     : NA
    * return     : NA
    * remark     : 可以在此通过协议栈发送消息等
    **************************************************************************/
    virtual void HandleCmd(CCmd* pCmd) = 0;
    
    /**************************************************************************
    * name       : Routing
    * description: 列程处理
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : 处理协议栈心跳等
    **************************************************************************/
    virtual void Routing(void) = 0;

    /**************************************************************************
    * name       : NotifyOpen
    * description: 通知协议栈连接建立
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : 连接建立时，被回调，通知协议栈
    **************************************************************************/
    virtual void NotifyOpen(void) = 0;

    /**************************************************************************
    * name       : NotifyClose
    * description: 通知协议栈连接断开
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : 连接断开时，被回调，通知协议栈
    **************************************************************************/
    virtual void NotifyClose(void) = 0;

    /**************************************************************************
    * name       : NotifyTimeout
    * description: 通知协议栈连接超时
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : 连接超时时，被回调，通知协议栈
    **************************************************************************/
    virtual void NotifyTimeout(void) = 0;
    
    /**************************************************************************
    * name       : NotifyRecv
    * description: 通知协议栈有数据到达
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : 连接上有数据到达时，被回调，通知协议栈，
    *              此时，协议栈应调用连接对象的接收数据函数
    **************************************************************************/
    virtual int NotifyRecv(void) = 0;

	/**************************************************************************
	* name       : SetLink
	* description: 设置连接对象
	* input      : pILink：连接对象
	* output     : NA
	* return     : NA
	* remark     : NA
	**************************************************************************/
    inline void SetLink(ILink* pILink) { m_pILink = pILink; }
    
	/**************************************************************************
	* name       : SetMsgHandle
	* description: 设置消息处理对象
	* input      : pMsgHandle：消息处理对象
	* output     : NA
	* return     : NA
	* remark     : NA
	**************************************************************************/
    inline void SetMsgHandle(IMsgHandle* pMsgHandle) { m_pMsgHandle = pMsgHandle; }

	/**************************************************************************
	* name       : LinkStatusReport
	* description: 连接状态上报
	* input      : linkState： 连接状态，现在有打开，完成，超时三个
	*              pszTransID：事务ID
	* output     : NA
	* return     : NA
	* remark     : NA
	**************************************************************************/
    virtual void LinkStatusReport(enum enLinkState linkState, const char* pszTransID) = 0;

protected:
    ILink*        m_pILink;      // 连接对象指针
    IMsgHandle*   m_pMsgHandle;  // 消息处理者指针
};

NAMESPACE_BP_END

#endif    // IPROTOCOL_H
