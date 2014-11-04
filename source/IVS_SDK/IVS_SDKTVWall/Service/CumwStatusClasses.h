/********************************************************************
    filename	: 	CumwStatusClasses.h
    author		:	wx58056
    created		:	2011/10/28
    description	:	所有状态类的实现
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/10/28 初始版本
 *********************************************************************/
#ifndef CUMW_STATUS_CLASSES_H
#define CUMW_STATUS_CLASSES_H

/*!< 模块所引用的基本头文件 */
#include "CumwCommon.h"

using namespace CUMW;

//CStatusConfig类相关定义
namespace CUMW
{
    const unsigned short PORT_MIN = 1024;   //端口的最小值

    //配置文件中section和key常量定义，以及默认值
    const char SECTION_LOG_CFG[] = "LOG_CFG";    //日志配置
    //日志级别
    const char KEY_LOG_LM[] = "LogLM";      //日志级别的key字符串
    const unsigned long DEFAULT_VALUE_LOG_LM = IVSLOG_ERR;    //日志级别的默认值

    //远程解码器配置
    const char SECTION_REMOTE_DECODER[] = "REMOTE_DECODER";     //远程解码器配置
    const char KEY_REMOTE_DECODER_LISTEN_PORT[] = "ListenPort"; //远程解码器监听端口的key字符串
    const unsigned short DEFAULT_VALUE_REMOTE_DECODER_LISTEN_PORT = 5050;         //远程解码器监听端口的默认值
}

//配置状态类
class CStatusConfig
{
public:

    /**************************************************************************
    * name       : CStatusConfig
    * description: Default constructor
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    CStatusConfig();

    /**************************************************************************
    * name       : InitConfig
    * description: 从配置文件从读取初始化配置
    * input      : strConfigFileFullName   配置文件名
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long           InitConfig(const string& strConfigFileFullName);

    /**************************************************************************
    * name       : SaveConifg
    * description: 保存配置到配置文件
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long           SaveConifg();

    /**************************************************************************
    * name       : ReloadConfig
    * description: 重新加载配置项
    * input      : NA
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long           ReloadConfig();

    /**************************************************************************
    * name       : GetLogLM
    * description: 获取日志级别
    * input      : NA
    * output     : NA
    * return     : 日志级别
    * remark     : NA
    **************************************************************************/
    unsigned long  GetLogLM() const;

    /**************************************************************************
    * name       : SetLogLM
    * description: 设置日志级别
    * input      : ulLogLM     日志级别
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void           SetLogLM(const unsigned long ulLogLM);

    /**************************************************************************
    * name       : GetRemoteDecoderListenPort
    * description: 获取远程解码器监听端口
    * input      : NA
    * output     : NA
    * return     : 监听端口
    * remark     : NA
    **************************************************************************/
    unsigned short GetRemoteDecoderListenPort() const;

    /**************************************************************************
    * name       : SetRemoteDecoderListenPort
    * description: 设置远程解码器监听端口
    * input      : usListenPort    监听端口
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void           SetRemoteDecoderListenPort
    (   IN const unsigned short usListenPort);

protected:

    /**************************************************************************
    * name       : GetConfigValue
    * description: 获取配置项的值
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void   GetConfigValue();

    /**************************************************************************
    * name       : SetConfigValue
    * description: 设置配置项的值
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void   SetConfigValue();

private:

    //配置文件全路径
    string m_strConfigFileFullName;

    //配置项
    unsigned long m_ulLogLM;                      // 日志级别
    unsigned short m_usRemoteDecoderListenPort;    //远程解码器监听端口
};

//单例状态类
class CStatusSingleton
{
private:

    /**************************************************************************
    * name       : CStatusSingleton
    * description: 私有构造函数，不允许外部创建对象
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    CStatusSingleton();

public:

    /**************************************************************************
    * name       : Instance
    * description: 获取实例
    * input      : NA
    * output     : NA
    * return     : 单体实例引用
    * remark     : NA
    **************************************************************************/
    static CStatusSingleton& Instance()
    {
        static CStatusSingleton instance;

        return instance;
    }

    /**************************************************************************
    * name       : GetConfig
    * description: 获取配置状态类实例
    * input      : NA
    * output     : NA
    * return     : 配置状态类实例引用
    * remark     : NA
    **************************************************************************/
    CStatusConfig& GetConfig()
    {
        return m_objStatusConfig;//lint !e1536
    };

    /**************************************************************************
    * name       : GetConnMgr
    * description: 获取通信管理类实例
    * input      : NA
    * output     : NA
    * return     : 通信管理类实例引用
    * remark     : NA
    **************************************************************************/
    CConnMgr& GetConnMgr()
    {
        return m_objConnMgr;//lint !e1536
    };

    /**************************************************************************
    * name       : GetNVSTimer
    * description: 获取定时器实例
    * input      : NA
    * output     : NA
    * return     : 通信定时器实例引用
    * remark     : NA
    **************************************************************************/
    CNVSTimer& GetNVSTimer()
    {
        return m_objTimer;//lint !e1536
    };

    /**************************************************************************
    * name       : GetLocalIp
    * description: 获取本地IP
    * input      : NA
    * output     : NA
    * return     : 本地IP字符串
    * remark     : NA
    **************************************************************************/
    const char* GetLocalIp();

    /**************************************************************************
    * name       : SetLocalIp
    * description: 设置本地IP
    * input      : pszIp    本地IP
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void		SetLocalIp(const char* pszIp);

private:
    CStatusConfig m_objStatusConfig;  //配置状态类
    CConnMgr m_objConnMgr;       //公共通信器
    CNVSTimer m_objTimer;         //定时器
    char m_szLocalIp[IVS_IP_LEN];   //本地IP
};

//通知回调状态类
class CStatusNotifyCallBack
{
public:

    /**************************************************************************
    * name       : CStatusNotifyCallBack
    * description: Default constructor
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    CStatusNotifyCallBack();

    /**************************************************************************
    * name       : ResetStatus
    * description: 重置状态
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void ResetStatus();

    /**************************************************************************
    * name       : SetStatusValue
    * description: 设置状态值
    * input      : pFunCallBack    回调函数指针
                   pUserData       用户数据指针
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void SetStatusValue(PCUMW_CALLBACK_NOTIFY pFunCallBack, void* pUserData);

    /**************************************************************************
    * name       : NotifyCallBack
    * description: 调用回调函数
    * input      : stInfo  通知信息结构体
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyCallBack(CUMW_NOTIFY_INFO& stInfo);

    /**************************************************************************
    * name       : NotifyAsynRet
    * description: 通知异步返回
    * input      : stNotifyInfo        异步返回通知结构体
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyAsynRet
    (   const CUMW_NOTIFY_ASYNC_RET& stNotifyInfo);

    /**************************************************************************
    * name       : NotifyAsynRet
    * description: 通知异步返回
    * input      : ulAsynSeq        异步调用序号
                   lAsynRetCode     异步返回码
                   ulBusinessId     业务ID
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyAsynRetNoInfo
    (   IN unsigned long ulAsynSeq,
        IN long          lAsynRetCode,
        IN unsigned long ulBusinessId = 0);

    /**************************************************************************
    * name       : NotifyBusinessAsynRet
    * description: 通媒体业务异步返回通知
    * input      : ulAsynSeq        异步调用序号
                   lAsynRetCode     异步返回码
                   ulBusinessId     业务ID
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyBusinessAsynRet
    (   IN unsigned long ulAsyncSeq,
        IN long          lAsyncRetCode,
        IN unsigned long ulBusinessID);

    /**************************************************************************
    * name       : NotifyEvent
    * description: 事件通知
    * input      : ulEventType     事件类型
                   ulEventPara     事件参数
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyEvent
    (   IN unsigned long ulEventType,
        IN unsigned long ulEventPara = 0);

    /**************************************************************************
    * name       : NotifyDecoderStatus
    * description: 解码器状态通知
    * input      : stNotifyInfo  通知内容
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyDecoderDisConnect
    (   IN CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO& stNotifyInfo);

    /**************************************************************************
    * name       : NotifyDecoderStatus
    * description: 解码器状态通知
    * input      : stNotifyInfo  通知内容
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyDecoderRegister
    (   IN CUMW_NOTIFY_REMOTE_DECODER_STATUS& stNotifyInfo);

    /**************************************************************************
    * name       : NotifyDecoderUnRegister
    * description: 解码器状态通知
    * input      : stNotifyInfo  通知内容
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long NotifyDecoderUnRegister
    (   IN CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO& stNotifyInfo);
private:
    PCUMW_CALLBACK_NOTIFY m_pFunCallBack; //回调函数指针
    void*                   m_pUserData;    //用户数据指针
};

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之定时触发器状态类相关定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    ///////////////////////////////////////////////////////////////////////////
    /// @brief 触发器类型
    ///
    /// Description.
    ///////////////////////////////////////////////////////////////////////////
    enum TRIGGER_TYPE
    {
        TRIGGER_TYPE_DECODER_CONN_TIME_OUT //解码器连接超时触发器
    };
}

///////////////////////////////////////////////////////////////////////////////
/// @brief 定时触发器状态类
///
/// Description:
///////////////////////////////////////////////////////////////////////////////
class CStatusTrigger : public ITrigger
{
private:

    /**************************************************************************
    * name       : CStatusTrigger
    * description: 禁止调用默认构造函数
    * input      : enTriggerType   触发器类型
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    CStatusTrigger();

public:

    /**************************************************************************
    * name       : CStatusTrigger
    * description: 禁止调用默认构造函数
    * input      : enTriggerType   触发器类型
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    CStatusTrigger
    (
        IN CUMW::TRIGGER_TYPE enTriggerType
    );

    /**************************************************************************
    * name       : onTrigger
    * description: 触发定时事件处理
    * input      : pArg        具体处理类
                   ullScales   定时器指针
                   enStyle     触发类型
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void onTrigger
    (   IN void*           pArg,
        IN ULONGLONG       ullScales,
        IN TriggerStyle    enStyle);

private:

    /**************************************************************************
    * name       : HandleTriggerDecoderConnTimeOut
    * description: 处理解码器连接超时触发器
    * input      : pArg    具体处理类
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void         HandleTriggerDecoderConnTimeOut
    (   IN void* pArg) const;

private:
    CUMW::TRIGGER_TYPE m_enTriggerType; //触发器类型
};

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之线程状态类相关定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    //线程处理函数类型
    typedef DWORD (WINAPI * PTHREAD_FUN)(LPVOID lpParameter);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief 线程状态类
///
/// Description:
///////////////////////////////////////////////////////////////////////////////
class CStatusThread
{
public:

    /**************************************************************************
    * name       : CStatusThread
    * description: Default constructor
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    CStatusThread();

    /**************************************************************************
    * name       : ~CStatusThread
    * description: Destructor
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual ~CStatusThread();

    /**************************************************************************
    * name       : StartThread
    * description: 启动线程
    * input      : pThreadFun      处理线程函数指针
                   pThreadEntryPara    线程入口参数
                   ulQueueSize     队列长度
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long StartThread
    (   IN CUMW::PTHREAD_FUN   pThreadFun,
        IN void*               pThreadEntryPara,
        IN unsigned long       ulQueueSize = 0);

    /**************************************************************************
    * name       : StopThread
    * description: 停止线程
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void StopThread();

    /**************************************************************************
    * name       : AddEvent
    * description: 添加事件
    * input      : enEventType    事件类型
                   pEventInfo     事件信息指针
                   ulEventInfoLen 事件信息内容长度
    * output     : NA
    * return     : 错误码
    * remark     : NA
    **************************************************************************/
    long AddEvent
    (   IN CUMW::EVENT_TYPE    enEventType,
        IN void*               pEventInfo = NULL,
        IN unsigned long       ulEventInfoLen = 0);

public:     //以下成员变量由外界直接访问
    //同步事件队列
    CUMW::CEventSyncQueue m_objEventSyncQueue;

    //处理事件线程是否运行
    BOOL m_bThreadRun;

    bool m_bIdle;

    //解码器ID（20位，不包括\0）
    char m_szDecoderID[CUMW_DECODER_ID_LEN + 1];

private:    //以下成员变量只能通过成员函数访问
    //处理事件线程句柄
    HANDLE m_hThread;

    //队列大小
    unsigned long m_ulQueueSize;
};

#endif //CUMW_STATUS_CLASSES_H
