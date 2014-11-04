/********************************************************************
 filename    :    StreamMgr.h
 author      :    g00209332
 created     :    2012/12/19
 description :    通信管理类
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 从C30继承
*********************************************************************/

#ifndef STREAM_MGR_H
#define STREAM_MGR_H

/*!< 远程解码器管理头文件 */
#include "RemoteDecoderMgr.h"

//StreamMgr接口实现类
class CStreamMgr
{
public:

    CStreamMgr();

    virtual ~CStreamMgr();

    /**************************************************************************
    * name       : InitStreamMgr
    * description: 初始化StreamMgr
    * input      : pFunNotifyCallBack  回调函数指针
                   pCallBackUserData   回调函数用户数据指针
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          InitStreamMgr
    (   IN PCUMW_CALLBACK_NOTIFY   pFunNotifyCallBack,
        IN void*                   pCallBackUserData);

    /**************************************************************************
    * name       : ReleaseStreamMgr
    * description: 释放StreamMgr
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          ReleaseStreamMgr();

    /**************************************************************************
    * name       : InitBusiness
    * description: 初始化业务
    * input      : objBusiness 业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          InitBusiness
    (   IN CBusiness& objBusiness);

    /**************************************************************************
    * name       : StartBusiness
    * description: 启动业务
    * input      : objBusiness 业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          StartBusiness
    (   IN CBusiness& objBusiness);

    /**************************************************************************
    * name       : ControlBusiness
    * description: 控制业务
    * input      : objBusiness 业务对象引用
                   stCtrlPara  控制参数
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          ControlBusiness
    (   IO CBusiness&                  objBusiness,
        IN CUMW::BUSINESS_CTRL_PARA&   stCtrlPara);

    /**************************************************************************
    * name       : StopBusiness
    * description: 停止业务
    * input      : objBusiness 业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          StopBusiness
    (   IN const CBusiness& objBusiness);

    /**************************************************************************
    * name       : StartRemoteDecoderMgr
    * description: 启动远程解码器管理
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          StartRemoteDecoderMgr();

    /**************************************************************************
    * name       : StopRemoteDecoderMgr
    * description: 停止远程解码器管理
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32          StopRemoteDecoderMgr();

private:

    /**************************************************************************
    * name       : HandleNotifyCallBack
    * description: 处理通知回调
    * input      : pstNotifyInfo   通知信息指针
                   pUserData       用户数据指针
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    static IVS_INT32 __stdcall HandleNotifyCallBack
    (   IN CUMW_NOTIFY_INFO* pstNotifyInfo,
        IN void*             pUserData);

private:

    //通知回调函数状态类
    CStatusNotifyCallBack m_objStatusNotifyCallBack;

    //远程解码器
    CRemoteDecoderMgr* m_pRemoteDecoderMgr;
};

#endif //STREAM_MGR_H
