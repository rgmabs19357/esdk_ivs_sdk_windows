/********************************************************************
 filename    :    ControlBus.h
 author      :    g00209332
 created     :    2012/12/19
 description :    控制对象
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 从C30继承
*********************************************************************/

#ifndef __CONTROL_BUS_H__
#define __CONTROL_BUS_H__

#include "hwsdk.h"
#include "ivs_error.h"

/*!< 媒体管理的父类头文件 */
#include "StreamMgr.h"

/*!< RDA协议头文件 */
#include "RDAProt.h"

#define THREAD_MAX_COUNT 81

namespace CUMW
{
    //通知类型定义
    typedef enum
    {
        LOGIN_STATUS_INIT = 0,  ///<初始化
        LOGIN_STATUS_INITED    ///<已初始化
    } LOGIN_STATUS;

    //事件类型的显示字符串最大长度
    const unsigned long STRING_LOGIN_STATUS = sizeof("CUMW Query Platform Info Success");

    //事件类型的显示字符串数组定义
    const char STR_LOGIN_STATUS[][STRING_LOGIN_STATUS]
    = {
        "CUMW Not Initial",
        "CUMW Initialize Success",
        "CUMW Relogining",
        "CUMW Register Success",
        "CUMW Query Platform Info Success",
        "CUMW Logined Success",
    };
}

typedef struct EventThreadPara
{
    int i;
    void* pThread;
}EVENT_THREAD_PARA;

//ControlBus接口实现类
class CControlBus
{
public:

    //业务异步信息结构体
    typedef struct _BUSINESS_ASYNC_INFO
    {
        unsigned long       ulAsyncSeq;     //异步序号
        CUMW::ASYNC_TYPE    enAsyncType;    //异步类型
        unsigned long       ulBusinessId;   //业务ID
        void*               pParameter;     //参数指针

        _BUSINESS_ASYNC_INFO()
        {
            ulAsyncSeq   = 0;
            enAsyncType  = CUMW::ASYNC_TYPE_BUSINESS_START;
            ulBusinessId = 0;
            pParameter = NULL;
        }
    } BUSINESS_ASYNC_INFO;

    CControlBus();

    virtual ~CControlBus();

    /**************************************************************************
    * name       : InitControlBus
    * description: 初始化ControlBus
    * input      : pParaInfo   初始化参数
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32 InitControlBus(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);

    /**************************************************************************
    * name       : ReleaseControlBus
    * description: 释放ControlBus
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    virtual IVS_INT32 ReleaseControlBus();

    /**************************************************************************
    * name       : IniBusiness
    * description: 初始化解码器
    * input      : pParaInfo   参数
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32         IniBusiness(IPI_INIT_BUSINESS_REQ* pParaInfo);

    /**************************************************************************
    * name       : HandleEventIniBusiness
    * description: 处理初始化解码器消息
    * input      : objBusiness     业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32         HandleEventIniBusiness(BUSINESS_ASYNC_INFO& stAsyncInfo);

    /**************************************************************************
    * name       : StartBusiness
    * description: 远程解码器开始解码
    * input      : pParaInfo   参数
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32         StartBusiness(IPI_START_BUSINESS_REQ* pParaInfo);

    /**************************************************************************
    * name       : HandleEventStartDecoder
    * description: 处理启动解码器消息
    * input      : objBusiness     业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32         HandleEventStartBusiness(BUSINESS_ASYNC_INFO& stAsyncInfo);

    /**************************************************************************
    * name       : StopBusiness
    * description: 停止解码
    * input      : objBusiness     业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32         StopBusiness(IPI_STOP_BUSINESS_REQ* pParaInfo);

    /**************************************************************************
    * name       : HandleEventStopBusiness
    * description: 处理停止解码的消息
    * input      : objBusiness     业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32         HandleEventStopBusiness(BUSINESS_ASYNC_INFO& stAsyncInfo);

    /**************************************************************************
    * name       : PlayAudio
    * description: 播放/停止播放声音
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : RET_OK-成功     RET_FAIL-失败
    * remark     : NA
    **************************************************************************/
    IVS_INT32         PlayAudio(IPI_AUDIO_CONTROL_REQ *pParaInfo);

    /**************************************************************************
    * name       : PlayAudio
    * description: 处理播放/停止播放声音信息
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : RET_OK-成功     RET_FAIL-失败
    * remark     : NA
    **************************************************************************/
    IVS_INT32         HandleEventPlayAudio(BUSINESS_ASYNC_INFO& stAsyncInfo);

    /**************************************************************************
    * name       : SetVolume
    * description: 设置音量
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : RET_OK-成功     RET_FAIL-失败
    * remark     : NA
    **************************************************************************/
    IVS_INT32         SetVolume(IPI_TUNEUP_VOLUME_REQ *pParaInfo);

    /**************************************************************************
    * name       : SetVolume
    * description: 处理设置音量信息
    * input      : pParaInfo 初始化参数
    * output     : void
    * return     : RET_OK-成功     RET_FAIL-失败
    * remark     : NA
    **************************************************************************/
    IVS_INT32         HandleEventSetVolume(BUSINESS_ASYNC_INFO& stAsyncInfo);

private:

    //业务ID到业务对象的映射表定义
    typedef map <unsigned long, CBusiness*> MAP_ID_TO_BUSINESS;

    //BEGIN R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化
    //业务ID组
    typedef vector <unsigned long> VECTOR_BUSINESS_ID;

    //业务ID到其共享业务ID组的映射表定义
    typedef map <unsigned long, VECTOR_BUSINESS_ID> MAP_SHARED_BUSINESS_ID;
    //END   R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化

private:

    /**************************************************************************
    * name       : InitControlBusResource
    * description: 初始化ControlBus的资源
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           InitControlBusResource();

    /**************************************************************************
    * name       : ReleaseControlBusResource
    * description: 释放ControlBus的资源
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    void                           ReleaseControlBusResource();

    /**************************************************************************
    * name       : InitNvsTimer
    * description: 初始化定时器
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           InitNvsTimer() const;

    /**************************************************************************
    * name       : InitConnMgr
    * description: 初始化通信管理模块
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           InitConnMgr() const;

    /**************************************************************************
    * name       : ProcEventThreadEntry
    * description: 处理事件线程入口
    * input      : pPara   线程入口参数
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    static unsigned long __stdcall ProcEventThreadEntry(void* pPara);

    /**************************************************************************
    * name       : ProcEventThread
    * description: 处理事件线程
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    void                           ProcEventThread(int i);

    /**************************************************************************
    * name       : AddBuesinessEvent
    * description: 添加业务事件
    * input      : ulAsynSeq      异步序号
                   enAsynType  异步类型
                   ulBusinessId    业务ID
                   pParameter  参数指针，取决于 异步类型
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           AddBuesinessEvent
    (   IN unsigned long       ulAsynSeq,
        IN CUMW::ASYNC_TYPE    enAsynType,
        IN unsigned long       ulBusinessId,
        IN void*               pParameter = NULL);

    /**************************************************************************
    * name       : AddGeneralAsynEvent
    * description: 添加通用异步事件
    * input      : ulAsynSeq      异步序号
                   enAsynType  异步类型
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           AddGeneralAsynEvent
    (   IN unsigned long    ulAsynSeq,
        IN CUMW::ASYNC_TYPE enAsynType);

    /**************************************************************************
    * name       : HandleNotifyCallBack
    * description: 处理通知回调
    * input      : pstNotifyInfo   通知信息指针
                   pUserData       用户数据指针
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    static IVS_INT32 __stdcall          HandleNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData);

    /**************************************************************************
    * name       : CheckStatus
    * description: 检测是否达到达到预期状态
    * input      : enExpectedStatus 预期状态
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           CheckStatus(CUMW::LOGIN_STATUS enExpectedStatus);

    /**************************************************************************
    * name       : StartBusiness
    * description: 启动业务
    * input      : objBusiness     业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           StartBusiness
    (   IN CBusiness& objBusiness);

    /**************************************************************************
    * name       : RunBusiness
    * description: 运行业务
    * input      : objBusiness     业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           RunBusiness
    (   IN CBusiness& objBusiness);

    /**************************************************************************
    * name       : StopBusinessWhole
    * description: 停止整个业务，包括发送停止信令到平台
    * input      : objBusiness     待停止的业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           StopBusinessWhole
    (   IN CBusiness& objBusiness);

    /**************************************************************************
    * name       : GetNewBusinessId
    * description: 获取新的业务ID
    * input      : objBusiness     待停止的业务对象引用
    * output     : NA
    * return     : 业务ID
    * remark     : NA
    **************************************************************************/
    unsigned long                  GetNewBusinessId();

    /**************************************************************************
    * name       : HandleEventBusinessStop
    * description: 根据业务ID获取业务对象
    * input      : ulBusinessId    业务ID
    * output     : pBusiness       返回的业务对象指针
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           GetBusinessById
    (   IN unsigned long    ulBusinessId,
        OUT CBusiness*&     pBusiness);

    /**************************************************************************
    * name       : AddBusinessCtrlEvent
    * description: 添加业务控制事件
    * input      : ulAsyncSeq                异步序号
                   ulBusinessID              业务ID
                   pParaInfo                 控制参数
                   ulParaLength              控制参数长度
                   enCtrlCode                控制码
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           AddBusinessCtrlEvent
    (   IN const unsigned long              ulAsyncSeq,
        IN const unsigned long              ulBusinessID,
        IN const void*                      pParaInfo,
        IN const unsigned long              ulParaLength,
        IN const CUMW::BUSINESS_CTRL_CODE   enCtrlCode);

    /**************************************************************************
    * name       : GetBusinessByID4Invite
    * description: 获取媒体业务re-invite相关业务
    * input      : ulBusinessID   业务ID
    * output     : pBusiness      异步信息
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           GetBusinessById4Invite
    (   IN unsigned long ulBusinessID,
        IO CBusiness*&   pBusiness);

    /**************************************************************************
    * name       : StopAllBusiness
    * description: 停止所有业务，只能在处理事件线程中处理
    * input      : NA
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    void                           StopAllBusiness();

    /**************************************************************************
    * name       : StopAllReuseBusiness
    * description: 停止指定业务ID的所有复用业务
    * input      : objBusiness 被复用的业务对象引用
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    void                           StopAllReuseBusiness
    (   IN CBusiness& objBusiness);

private:

    /**************************************************************************
    * name       : HandleEventBusiness
    * description: 处理媒体业务事件
    * input      : pEventInfo      事件信息指针
                   ulEventInfoLen  事件信息内容长度
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           HandleEventBusiness
    (   IN void*           pEventInfo,
        IN unsigned long   ulEventInfoLen);

    /**************************************************************************
    * name       : HandleEventBusinessStart
    * description: 处理媒体业务开始事件
    * input      : pParameter  参数指针
                   ulAsyncSeq  异步序号
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           HandleEventBusinessStart
    (   IN void*           pParameter,
        IN unsigned long   ulAsyncSeq);

    /**************************************************************************
    * name       : HandleEventBusinessRun
    * description: 处理媒体业务运行事件
    * input      : ulBusinessId    业务ID
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           HandleEventBusinessRun
    (   IN unsigned long ulBusinessId);

private:    //通知处理函数

    /**************************************************************************
    * name       : HandleNotifyAsyncRet
    * description: 处理异步返回通知
    * input      : stNotifyInfo   通知信息
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           HandleNotifyAsyncRet
    (   IN CUMW_NOTIFY_INFO& stNotifyInfo);

    /**************************************************************************
    * name       : HandleNotifyEvent
    * description: 处理事件通知
    * input      : stNotifyInfo   通知信息
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32                           HandleNotifyEvent
    (   IN CUMW_NOTIFY_INFO& stNotifyInfo);

    IVS_INT32 GetIdleThread(long& lIndex);
    IVS_INT32 FindThread(const char* pszDecoderID, long& lIndex);

private:

    //通知回调函数状态类
    CStatusNotifyCallBack m_objStatusNotifyCallBack;

    //处理事件线程状态类
    CStatusThread m_objProcEventThread[THREAD_MAX_COUNT];

    //接口锁
    CUMW::CCriticalSection m_objInterfaceLock;

    //StreamMgr对象指针
    CStreamMgr* m_pStreamMgr;

    //是否已登录
    CUMW::LOGIN_STATUS m_enLoginStatus;

    BOOL m_bisFirstLogin;

    //业务ID
    unsigned long m_ulBusinessId;

    //业务ID到业务对象的映射表
    MAP_ID_TO_BUSINESS m_mapIdToBusiness;

    //BEGIN R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化
    //业务ID到其共享业务ID组的映射表定义
    MAP_SHARED_BUSINESS_ID m_mapSharedBusinessId;
    //END   R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化

    //锁
    VOS_Mutex *m_pThreadMutex;     

    //业务ID锁
    VOS_Mutex* m_pBusinessIdMutex;		

    EVENT_THREAD_PARA m_ThreadPara[THREAD_MAX_COUNT];
};

#endif //__CONTROL_BUS_H__

