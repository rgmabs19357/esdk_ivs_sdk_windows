/********************************************************************
 filename    :    RemoteDecoderMgr.h
 author      :    g00209332
 created     :    2012/12/19
 description :    解码器管理类
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 从C30继承
*********************************************************************/

#ifndef REMOTE_DECODER_MGR_H
#define REMOTE_DECODER_MGR_H

/*!< 模块基础头文件 */
#include "vos.h"

/*!< 内存分配相关函数的头文件 */
#include "vos_common.h"

/*!< 配置信息头文件 */
#include "vos_config.h"

/*!< 基本数据类型定义头文件 */
#include "vos_basetype.h"

/*!< 模块所引用的基本头文件 */
#include "CumwCommon.h"

/*!< RDA协议头文件 */
#include "RDAProtStack.h"

//远程解码器信息
typedef CUMW_NOTIFY_REMOTE_DECODER_STATUS CUMW_DECODER_INFO;

//与远程解码器通信相关设置
#define WAITEVENT_TIMEOUT (5 * 1000)   //等待读包事件，超时时间5秒
#define MAX_RECV_WAITTIME (5 * 1000)   //recv超时，超时事件为5秒
#define MAX_HEARTBEAT_INTERVAL (4 * 1000) //心跳检测间隔时间4秒(DTS2013073008710)  //(60 * 1000)  //心跳检测间隔时间60秒
#define MAX_HEARTBEAT_RETRY_COUNT 3    //最大心跳重试次数

//RDA协议相关定义
#define RDA_MSGHEAD_LEN sizeof(RDAMsgHead)
#define MAX_RDAMSG_LEN (16 * 1024)  //最大消息长度
#define MAX_RDAPACKET_NUM 81         //最大同时使用81个包
#define MAX_LIST_NUM 81         //解码器列表长度
#define MAX_RD_NUM 81         //最大支持81个解码器
#define MAX_RDMGR_EVENT_NUM 1024       //CRemoteDecoderMgr中的消息队列长度
#define RDA_PACKTAG 0x8000
#define RDA_REQPACK 0
#define RDA_RSPPACK 1
#define TRANSFER_REQNO_TO_RSPNO(x) (RDA_PACKTAG | (x))
#define TRANSFER_RSPNO_TO_REQNO(x) ((~RDA_PACKTAG) & (x))

#define GARBAGE_PARAM 0        //无效参数定义

#define RDA_ERROR_CODE_OK 0x00
#define RDA_ERROR_DISCONNECT 0x01

typedef enum tagRDA_PACKETMGR_EVENT_TYPE
{
    RDA_PACKETMGR_EVENT_CHECK_RD_INVALID = 0x00, //检测解码器吊死,即心跳检测
    RDA_PACKETMGR_EVENT_RD_CONN_TIMEO = 0x01, //解码器只是建立连接，不发消息，心跳超时
    RDA_PACKETMGR_EVENT_RECV_RD_DATA   = 0x02,   //处理解码器发送过来的数据
    RDA_PACKETMGR_EVENT_SEND_RD_DATA   = 0x03,   //向远程解码器发送数据
    RDA_PACKETMGR_EVENT_SEND_HEARTBEAT = 0x04, //向远程解码器发送心跳
    RDA_PACKETMGR_EVENT_EXIT_THREAD = 0x05, //退出线程

    RDA_PACKETMGR_EVENT_BUTT              //保留
} RDA_PACKETMGR_EVENT_TYPE;

//定义RDAPacket，用于同步消息的等待接收
class CRDAPacket
{
public:
    CRDAPacket();
    virtual ~CRDAPacket();

public:

    //初始化
    IVS_INT32 InitRDAPacket(void);

    //等待读包事件
    IVS_INT32 WaitReadPacketEv(void);

    //触发读包事件
    IVS_INT32 SetReadPacketEv(void);

public:
    IVS_ULONG m_ulRecvBuffLen;     //接收到的数据长度，包括包头
    IVS_CHAR      * m_pRecvBuff;         //接收到的数据，包括包头

    IVS_ULONG m_ulSendBuffLen;     //发送的数据长度，包括包头
    IVS_CHAR      * m_pSendBuff;         //发送的数据，包括包头

    IVS_USHORT m_usPacketNo;        //包序号

private:
    VOS_Event * m_pReadEvent;    //读包事件，用于通知等待方获取包中数据
};

typedef std::list <CRDAPacket *> ListRDAPacket;
typedef ListRDAPacket::iterator  ListRDAPacketIte;

//管理RDAPacket的分配、释放
class CRDAPacketMgr
{
public:
    CRDAPacketMgr() : m_pMutexList(NULL), m_pArrayRDAPacket(NULL)
    {
    };
    ~CRDAPacketMgr()
    {
        //确认不会造成异常，VOS_DELETE会保护的
        VOS_DELETE(m_pArrayRDAPacket, MULTI);/*lint !e1551，确认不会造成异常，VOS_DELETE会有try-catch捕获异常的*/
        if (NULL != m_pMutexList)
        {
            (void)VOS_DestroyMutex(m_pMutexList);
        }

        m_pMutexList = NULL;
        m_pArrayRDAPacket = NULL;
    };

public:

    //初始化
    IVS_INT32		InitRDAPacketMgr(void);

    //获取包
    CRDAPacket* GetRDAPacket(void);

    //归还包
    void		ReleaseRDAPacket( CRDAPacket* pRDAPacket);

    //根据包序号查找CRDAPacket
    CRDAPacket* FindRDAPacket(IVS_USHORT usPacketNo);

private:
    ListRDAPacket m_listFreeRDAPacket;     //空闲RDA包队列
    ListRDAPacket m_listWorkRDAPacket;     //工作RDA包队列

    VOS_Mutex    * m_pMutexList;        //队列锁
    CRDAPacket   * m_pArrayRDAPacket;   //包数组内存地址，主要是为了方便分配和释放
};

//公共事件定义
class CCUMWEvent
{
public:
    CCUMWEvent(): m_ulEventType(0)
    {
    };
    virtual ~CCUMWEvent(){};

public:
    IVS_ULONG m_ulEventType; //消息类型
};

class CDecoderConnHandle;

//自定义事件
class CDecoderMgrEvent : public CCUMWEvent
{
public:
    CDecoderMgrEvent() : m_pDecoderConnHandle(NULL), m_pRDAPacket(NULL)
    {
    };
    ~CDecoderMgrEvent()
    {
        m_pDecoderConnHandle = NULL;
        m_pRDAPacket = NULL;
    };

public:
    CDecoderConnHandle * m_pDecoderConnHandle; //客户端连接指针
    CRDAPacket         * m_pRDAPacket;
};

typedef CSyncQueue <CCUMWEvent> CDecoderMgrEventQueue;

//检测连接超时类
class CCheckConnTimeOTrigger : public ITrigger
{
public:

    //检测客户端连接超时但未发送请求
    void onTrigger(void* pArg, ULONGLONG ullScales,
                   TriggerStyle style);
};

class CRemoteDecoder;

//客户端连接定义
class CDecoderConnHandle : public CTcpConnHandle
{
public:
    CDecoderConnHandle();
    ~CDecoderConnHandle();

public:

    //初始化
    LONG		Init(CDecoderMgrEventQueue* pEventQ);

    //一旦tcp缓冲区有数据时，要做的事情
    void		handle_recv(void);

    //一旦tcp缓冲区可以发送数据时，要做的事情
    void handle_send(void){};

    /*设置连接超时时间，主要是为了解决下面这种攻击:
        一个客户端只是连接到主控，但是没有发消息*/

    /**
     * Description:  SetTimeOut().    设置连接超时时间，主要是为了解决下面这种攻击:
     *        一个客户端只是连接到主控，但是没有发消息
     * @param  [in]  ulInterval  超时时长，单位为秒
     * @return      void
     */
    void		SetTimeOut
    (   IN unsigned long ulInterval);

    //如果注册消息已经来了则，取消超时机制
    void		CancelTimeOut(void);

    //一旦客户端连接超时了，要做的事情
    void		HandleTimeout(void);

    //放到任务队列前，要将此连接的引用计数加1
    LONG		AddRef(void);

    //处理完任务后，将引用计数减1，同时根据返回值判断此连接是否要删除
    VOS_BOOLEAN ReleaseRef(void);

    //BEGIN R002C01SPC001 ADD 2011-08-16 ligengqiang 00124479 for 解码器支持本地双网卡

    /**
     * Description:  GetLocalIp().    获取本地IP
     * @param  [out]  strIp  本地IP
     * @return      void.
     */
    void		GetLocalIp(string& strIp);
    //END   R002C01SPC001 ADD 2011-08-16 ligengqiang 00124479 for 解码器支持本地双网卡

public:
    volatile VOS_BOOLEAN m_bNeedUse;      //是否需要使用
    CRemoteDecoder         * m_pRemoteDecoder; //解码器对象

private:
    CDecoderMgrEventQueue  * m_pEventQ;            //事件队列
    volatile ULONG m_ulReference;        //引用计数
    VOS_Mutex              * m_pMutexDecoderConnHandle; //主要用于ReleaseRef中临界区保护

    CStatusTrigger m_objTriggerConnTimeOut;    //连接超时触发器
    //BEGIN R002C01SPC001 ADD 2011-08-16 ligengqiang 00124479 for 解码器支持本地双网卡
    char m_szLocalIp[IVS_IP_LEN];   //本地IP
    //END   R002C01SPC001 ADD 2011-08-16 ligengqiang 00124479 for 解码器支持本地双网卡
};

typedef std::list <CDecoderConnHandle *> ListDecoderConnHandle;
typedef ListDecoderConnHandle::iterator  ListDecoderConnHandleIte;

//管理客户端连接资源管理
class CDecoderConnHandleMgr
{
public:
    CDecoderConnHandleMgr();
    ~CDecoderConnHandleMgr();

public:

    //初始化 CDecoderConnHandleMgr
    LONG				Init(CDecoderMgrEventQueue* pEventQ);

    //获取一个客户端连接
    CDecoderConnHandle* GetConnHandle(void);

    //归还一个客户端连接
    void				ReleaseConnHandle(CDecoderConnHandle* pDecoderConnHandle);

private:
    ListDecoderConnHandle m_ListDecoderConnHandle;  //客户端连接队列
    VOS_Mutex             * m_pMutexList;             //队列锁
    CDecoderConnHandle    * m_pArrayDecoderConnHandle; //数组内存地址
};

//tcp服务器定义
class CDecoderMgrServerHandle : public CTcpServerHandle
{
public:
    CDecoderMgrServerHandle() : m_pDecoderConnHandleMgr(NULL)
    {

    };
    virtual ~CDecoderMgrServerHandle()
    {
        m_pDecoderConnHandleMgr = NULL;
    };

public:

    //初始化 CDecoderMgrServerHandle
    LONG Init(CDecoderConnHandleMgr* pDecoderConnHandleMgr);

    LONG handle_accept(const CNetworkAddr* pRemoteAddr,
                       CTcpConnHandle* &   pTcpConnHandle);

private:
    CDecoderConnHandleMgr* m_pDecoderConnHandleMgr;
};

//远程解码器类
class CRemoteDecoder
{
public:
    CRemoteDecoder() : m_pDecoderConnHandle(NULL), m_ulHeartBeatCount(0)
    {
        memset(&m_stDecoderInfo, 0, sizeof(m_stDecoderInfo));
    };

    ~CRemoteDecoder()
    {
        m_pDecoderConnHandle = NULL;
    };

public:
    CUMW_DECODER_INFO m_stDecoderInfo;              //解码器信息
    CDecoderConnHandle*   m_pDecoderConnHandle;     //解码器信息
    volatile ULONG m_ulHeartBeatCount;             //心跳计数
};

typedef std::list <CRemoteDecoder *> ListRemoteDecoder;
typedef ListRemoteDecoder::iterator  ListRemoteDecoderIte;

//远程解码器资源管理
class CRemoteDecoderArry
{
public:
	CRemoteDecoderArry():m_pMutexList(NULL)
		,m_pArrayRemoteDecoder(NULL)
		,m_pEventQ(NULL)
		,m_pDecoderConnHandleMgr(NULL)
    {

    };
    ~CRemoteDecoderArry();

public:

    //初始化
    LONG				Init(CDecoderMgrEventQueue*                pEventQ,
                             CDecoderConnHandleMgr *               pDecoderConnHandleMgr);

    //获取一个空远程解码器对象
    CRemoteDecoder*		GetRemoteDecoder(void);

    //释放远程解码器对象
    void				ReleaseRemoteDecoder(CRemoteDecoder* pRemoteDecoder);

    //获取连接句柄
    CDecoderConnHandle* GetConnHandle(const char* pszDecoderID);

    //获取解码器信息
    LONG				GetDecoderInfo
    (   char*               pszDecoderID,
        CUMW_DECODER_INFO&  stDecoderInfo);

    //检查远程解码器是否存活
    void				CheckHeartBeat( void );

    //清空心跳计数
    void				ClearHeartBeatCount( char* pszDecoderID );

    //打印解码器信息
    void				PrintDecoderInfo
    (   const CRemoteDecoder& objRemoteDecoder, bool bChannel=true) const;

    //释放Init时生成的资源
    void				Release();

    //BEGIN V100R001C05 ADD 2010-01-18 zhaoyuzhen z00137994 for 消除代码重复度
    //根据解码器ID查找解码器队列中的对应解码器信息结构体
    long				FindDecoderInfoByID(const char*                pszDecoderID,
                                            CRemoteDecoder*&           pDecoderInfo);
    //END V100R001C05 ADD 2010-01-18 zhaoyuzhen z00137994 for 消除代码重复度

private:
    ListRemoteDecoder m_FreeListRemoteDecoder;
    ListRemoteDecoder m_WorkListRemoteDecoder;

    VOS_Mutex             *m_pMutexList;        //队列锁
    //包数组内存地址，主要是为了方便分配和释放
    CRemoteDecoder        *m_pArrayRemoteDecoder;
    CDecoderMgrEventQueue *m_pEventQ;
    CDecoderConnHandleMgr *m_pDecoderConnHandleMgr; //连接资源管理
};

//此类主要用于处理远程解码器发送过来的消息
class CRemoteDecoderReqProc
{
public:
    CRemoteDecoderReqProc();
    ~CRemoteDecoderReqProc();

public:

    //初始化
    long  Init
    (   IN PCUMW_CALLBACK_NOTIFY   pFunNotifyCallBack,
        IN void*                   pCallBackUserData,
        CRDAPacketMgr*             pRDAPacketMgr,
        CDecoderConnHandleMgr*     pDecoderConnHandleMgr,
        CRemoteDecoderArry*        pRemoteDecoderArry,
        CRDAProtStack*             pRDAProtStack);

    //退出
    void  Exit(void);

    //处理解码器发送来的消息
    LONG  ProcMsg( CDecoderMgrEvent* pEventRecv);

    //解码器断连
    VOID  RemoteDecoderDisConnect( CDecoderConnHandle *pDecoderConnHandle, BOOL bUnRigister = FALSE );

protected:

    //检查是否已经初始化
    BOOL  CheckIsInited();

private:

    //处理注册消息
    LONG  ProcRegisterMsg(CDecoderMgrEvent& objEvent);

    //处理注销消息
    LONG  ProcUnRegisterMsg(CDecoderMgrEvent* pEventRecv);

    //处理同步消息的响应
    LONG  ProcSyncRspMsg(CDecoderMgrEvent* pEventRecv);

    /**************************************************************************
    * name       : ProcHeartBeatReq
    * description: 处理心跳请求消息
    * input      : objEvent    事件对象
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    long  ProcHeartBeatReq
    (   IN CDecoderMgrEvent& objEvent);

    /**************************************************************************
    * name       : ProcHeartBeatRsp
    * description: 处理心跳响应消息
    * input      : objEvent    事件对象
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    long  ProcHeartBeatRsp();

    /**************************************************************************
    * name       : ProcErrorNotifyReq
    * description: 处理错误上报请求消息
    * input      : objEvent    事件对象
    * output     : NA
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    long  ProcErrorNotifyReq
    (   IN CDecoderMgrEvent& objEvent);

    /**************************************************************************
    * name       : GetDecoderInfoFromRegisterReq
    * description: 从注册请求消息中提取解码器信息
    * input      : ojRegisterReq   注册请求消息
    * output     : stDecoderInfo   解码器信息
    * return     : long 错误码
    * remark     : NA
    **************************************************************************/
    long  GetDecoderInfoFromRegisterReq
    (   IN CRDARegisteReq&      ojRegisterReq,
        OUT CUMW_DECODER_INFO&  stDecoderInfo) const;

private:
    char              * m_pRecvBuffer;  //接收数据缓冲区
    ULONG m_ulRecvBuffLen; //接收数据缓冲区长度

    char              * m_pSendBuffer;  //发送数据缓冲区
    ULONG m_ulSendBuffLen; //发送数据缓冲区长度

    CRDAPacketMgr     * m_pRDAPacketMgr; //包管理类指针
    CDecoderConnHandleMgr * m_pDecoderConnHandleMgr;
    CRemoteDecoderArry    * m_pRemoteDecoderArry;

    CRDAProtStack         * m_pRDAProtStack;

    //通知回调函数状态类
    CStatusNotifyCallBack m_objStatusNotifyCallBack;
};

//心跳探测触发类
class CCheckHeartBeatTrigger : public ITrigger
{
public:

    //初始化
    LONG Init(CDecoderMgrEventQueue* pEventQ);

    //产生心跳检测事件放入队列
    void onTrigger(void* pArg, ULONGLONG ullScales,
                   TriggerStyle style);

private:
    CDecoderMgrEventQueue* m_pEventQ; //事件队列指针
};

//初始化参数信息结构
typedef struct
{
    CConnMgr*       pConnMgr;       //公共通信平台
    //BEGIN R001C05LYYS01 MOD 2010-03-08 ligengqiang 00124479 for 新增配置状态类
    CStatusConfig*  pStatusConfig;  //配置状态类
    //END   R001C05LYYS01 MOD 2010-03-08 ligengqiang 00124479 for 新增配置状态类
    CNVSTimer*      pTimer;         //定时器
    VOS_Mutex*      pCumwMutex;     //中间件锁
} INITPARAMINFO;

//远程解码器管理
class CRemoteDecoderMgr
{
public:

    CRemoteDecoderMgr();

    virtual ~CRemoteDecoderMgr();

    /**************************************************************************
    * name       : InitRemoteDecoderMgr
    * description: 初始化RemoteDecoderMgr
    * input      : pFunNotifyCallBack  回调函数指针
                   pCallBackUserData   回调函数用户数据指针
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  InitRemoteDecoderMgr
    (   IN PCUMW_CALLBACK_NOTIFY   pFunNotifyCallBack,
        IN void*                   pCallBackUserData);

    /**************************************************************************
    * name       : StartRemoteDecoderMgr
    * description: 启动RemoteDecoderMgr
    * input      : NA
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  StartRemoteDecoderMgr();

    /**************************************************************************
    * name       : StopRemoteDecoderMgr
    * description: 停止RemoteDecoderMgr
    * input      : NA
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  StopRemoteDecoderMgr();

    /**************************************************************************
    * name       : InitBusiness
    * description: 初始化业务
    * input      : objBusiness 业务对象引用
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  InitBusiness
    (   IN CBusiness& objBusiness);

    /**************************************************************************
    * name       : StartBusiness
    * description: 启动业务
    * input      : objBusiness 业务对象引用
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  StartBusiness
    (   IN const CBusiness& objBusiness);

    /**************************************************************************
    * name       : StopBusiness
    * description: 停止业务
    * input      : objBusiness 业务对象引用
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  StopBusiness
    (   IN const CBusiness& objBusiness);

    /**************************************************************************
    * name       : ControlBusiness
    * description: 控制业务
    * input      : objBusiness 业务对象引用
                   stCtrlPara  控制参数
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  ControlBusiness
    (   IN const CBusiness&            objBusiness,
        IN CUMW::BUSINESS_CTRL_PARA&   stCtrlPara);

    /**************************************************************************
    * name       : PlayAudio
    * description: 播放声音
    * input      : objBusiness 业务对象引用
                   bStart          开始还是停止
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  PlayAudio
    (   IN const CBusiness&    objBusiness,
        IN BOOL                bStart);

    /**************************************************************************
    * name       : SetVolume
    * description: 设置音量
    * input      : objBusiness 业务对象引用
                   ulVolume    音量
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  SetVolume
    (   IN const CBusiness&    objBusiness,
        IN unsigned long       ulVolume);

    //BEGIN R002C01SPC001 ADD 2011-08-16 ligengqiang 00124479 for 解码器支持本地双网卡

    /**************************************************************************
    * name       : GetLocalIp
    * description: 根据获取本地IP
    * input      : pszDecoderId    解码器ID
    * output     : strLocalIp      本地IP
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void                  GetLocalIp
    (   IN const char*  pszDecoderId,
        OUT string&     strLocalIp);
    //END   R002C01SPC001 ADD 2011-08-16 ligengqiang 00124479 for 解码器支持本地双网卡

public:

    //线程入口函数
    static void*          invoke(void* argc);

    //循环接受消息线程
    void                  ProcEventThread();

protected:

    //检查是否已经初始化
    BOOL                  CheckIsInited();

private:

    /**************************************************************************
    * name       : HandleNotifyCallBack
    * description: 处理通知回调
    * input      : pstNotifyInfo   通知信息指针
                   pUserData       用户数据指针
    * output     : strLocalIp      本地IP
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    static IVS_INT32 __stdcall HandleNotifyCallBack
    (   IN CUMW_NOTIFY_INFO* pstNotifyInfo,
        IN void*             pUserData);

    //这里只是放入任务队列中，由事件处理线程处理发送任务
    LONG                  SendRDAPacket( CRDAPacket*                  pRDAPacket,
                                         const char*                  pszDecoderID);

    //处理同步消息发送任务
    LONG                  ProcSendMsg( CDecoderMgrEvent* pEventRecv );

    /**************************************************************************
    * name       : SendHeartBeatMsg
    * description: 发送心跳消息
    * input      : pDecoderConnHandle  解码器连接处理对象句柄
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  SendHeartBeatMsg
    (   IN CDecoderConnHandle* pDecoderConnHandle);

    //处理连接超时任务
    LONG                  ProcConnTimeOut(const CDecoderMgrEvent* pEventRecv);

    //初始化解码器管理事件队列
    LONG                  InitDecoderMgrEventQ();

    //初始化服务监听
    LONG                  InitDecoderMgrServerHandle();

    //初始化解码器链接管理
    LONG                  InitDecoderConnHandleMgr();

    //初始化远程解码器发送过来的消息
    LONG                  InitRemoteDecoderReqProc();

    //初始化RDA协议包资源池
    LONG                  InitRDAPacketMgr();

    //初始化解码器资源管理
    LONG                  InitRemoteDecoderArry();

    //初始化心跳定时任务
    LONG                  InitCheckHeartBeatTrigger();

    //初始化RDA协议栈
    LONG                  InitRDAProtStack();

    /**************************************************************************
    * name       : SendMsg
    * description: 发送请求消息，同步等待响应消息并解析
    * input      : objRDAMsgReq    请求消息对象
                   pszDecoderID     解码器ID
                   objRDAMsgResp   响应消息对象
    * output     : NA
    * return     : long       错误码
    * remark     : NA
    **************************************************************************/
    long                  SendReqMsg
    (   IN CRDAMsg&    objRDAMsgReq,
        IN const char* pszDecoderID,
        IN CRDAMsg&    objRDAMsgResp);

    /**************************************************************************
    * name       : GetTransNo
    * description: 获取传输编号
    * input      : NA
    * output     : NA
    * return     : unsigned short.     传输编号
    * remark     : NA
    **************************************************************************/
    unsigned short        GetTransNo();

    /**************************************************************************
    * name       : ControlAudio
    * description: 控制音频
    * input      : objBusiness     业务对象
                   stCtrlPara      控制参数
    * output     : NA
    * return     : long            错误码
    * remark     : NA
    **************************************************************************/
    long                  ControlAudio
    (   IN const CBusiness&            objBusiness,
        IN CUMW::BUSINESS_CTRL_PARA&   stCtrlPara);

private:

    //解码器业务结构体
    struct DECODER_BUSINESS
    {
        char          m_szDecoderId[CUMW_DECODER_ID_LEN + 1];   // 远程解码器ID
        unsigned long m_ulChannelId;  //远程解码器的通道编号
        unsigned long m_ulBusinessId; //第一个业务ID
        long          m_lReferNum;      //引用计数

        DECODER_BUSINESS()
        {
            memset(m_szDecoderId, 0, sizeof(m_szDecoderId));

            m_ulChannelId  = 0;
            m_ulBusinessId = 0;
            m_lReferNum = 1;    //默认计数为1
        }

        DECODER_BUSINESS(const char* szDecoderId, unsigned long ulChannelId)
        {
            memset(m_szDecoderId, 0, sizeof(m_szDecoderId));
            if(!CToolsHelp::Strncpy(m_szDecoderId, sizeof(m_szDecoderId), szDecoderId, sizeof(m_szDecoderId) - 1))
			{
				IVS_RUN_LOG_ERR("DECODER_BUSINESS Copy DecoderId failed");
			}

            m_ulChannelId  = ulChannelId;
            m_ulBusinessId = 0;
            m_lReferNum = 1;    //默认计数为1
        }
    };

    //解码器业务列表定义
    typedef list <DECODER_BUSINESS> LIST_DECODER_BUSINESS;

    //解码器业务查找比较方法类
    class CFindDecoderBusiness
    {
public:
        CFindDecoderBusiness (const DECODER_BUSINESS& stDecoderBusiness) : m_stDecoderBusiness(stDecoderBusiness)
        {}

        bool operator()    (const DECODER_BUSINESS& stDecoderBusiness)
        {
            if (0
                == strncmp(m_stDecoderBusiness.m_szDecoderId, stDecoderBusiness.m_szDecoderId,
                           sizeof(m_stDecoderBusiness.m_szDecoderId))
                && (m_stDecoderBusiness.m_ulChannelId == stDecoderBusiness.m_ulChannelId))
            {
                return true;
            }

            return false;
        }//lint !e1762

private:
		CFindDecoderBusiness(){}//满足pc-lint
		const DECODER_BUSINESS m_stDecoderBusiness;
    };

    //引用控制码
    enum REFER_CTRL_CODE
    {
        REFER_CTRL_CODE_NONE = 0,    //不变化
        REFER_CTRL_CODE_INC,         //递增
        REFER_CTRL_CODE_DEC          //递减
    };

private:

    /**************************************************************************
    * name       : HasDecoderBusiness
    * description: 检测是否已有解码器业务
    * input      : stDecoderBusiness   解码器业务
                   eReferCtrlCode      引用控制码
    * output     : NA
    * return     : BOOL  是否已有解码器业务
    * remark     : NA
    **************************************************************************/
    BOOL HasDecoderBusiness
    (   DECODER_BUSINESS&   stDecoderBusiness,
        REFER_CTRL_CODE     eReferCtrlCode = REFER_CTRL_CODE_NONE);

private:

    VOS_Thread              * m_pVosThread;
    volatile VOS_BOOLEAN m_bExit;    //线程运行状态标志

    CDecoderMgrEventQueue   * m_pDecoderMgrEventQ;   //事件队列
    CDecoderMgrServerHandle * m_pDecoderMgrServerHandle; //服务器句柄
    CDecoderConnHandleMgr   * m_pDecoderConnHandleMgr; //连接资源管理
    CRemoteDecoderReqProc   * m_pRemoteDecoderReqProc; //处理远程解码器发送过来的消息
    CRDAPacketMgr           * m_pRDAPacketMgr; //RDA包资源管理
    CRemoteDecoderArry      * m_pRemoteDecoderArry; //解码器器对象资源管理
    CCheckHeartBeatTrigger  * m_pCheckHeartBeatTrigger; //心跳定时任务

    //传输编号
    volatile unsigned short m_usTransNo;

    //RDA协议栈
    CRDAProtStack     * m_pRDAProtStack;

    char              * m_pSendBuffer;  //发送数据缓冲区
    ULONG m_ulSendBuffLen; //发送数据缓冲区长度

    //通知回调函数状态类
    CStatusNotifyCallBack m_objStatusNotifyCallBack;

    //解码器业务列表
    LIST_DECODER_BUSINESS m_listDecoderBusiness;
};

#endif  //REMOTE_DECODER_MGR_H
