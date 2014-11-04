/********************************************************************
 filename    :    ConnMgr.h
 author      :    g00209332
 created     :    2012/12/19
 description :    连接管理
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 从C30继承
*********************************************************************/

#ifndef CONN_MGR
#define CONN_MGR

#ifdef ENV_LINUX
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <sys/epoll.h>
#endif //VOS_OS_LINUX

// #ifdef WIN32
// #include <winsock2.h>
// #endif

/*!< 链表头文件 */
#include <list>

/*!< 配置信息头文件 */
#include "vos_config.h"

/*!< 基本数据类型定义头文件 */
#include "vos_basetype.h"

/*!< 模块所引用的基本头文件 */
#include "vos_common.h"

/*!< 模块基础头文件 */
#include "vos.h"
#include "ToolsHelp.h"

#define InvalidFd -1
#define InvalidSocket -1
#define InvalidIp INADDR_ANY
#define Invalidport 0
#define DEFAULT_SENDRECV_SIZE (1024 * 1024)

//BEGIN R001C05LITSB010 ADD 2010-07-12 ligengqiang 00124479 for 录像下载流量限制，自适应缓冲区大小
#define MIN_RECV_SIZE (16 * 1024)   //最小的接收缓冲区大小
//END   R001C05LITSB010 ADD 2010-07-12 ligengqiang 00124479 for 录像下载流量限制，自适应缓冲区大小

#define SendRecvError -1 //发送或接收错误
#define SendRecvErrorTIMEO -2 //发送或接收超时
#define SendRecvErrorEBADF -3 //socket句柄错误
#define SendRecvErrorEOF -4 //tcp断连

#define MAX_LISTEN_QUEUE_SIZE 2000
#define EPOLL_MAX_EVENT (MAX_LISTEN_QUEUE_SIZE + 1000)
#define MAX_EPOLL_FD_SIZE 3000
#define LINGER_WAIT_SECONDS 1 //LINGER等待时间(seconds)

//定义连接管理器得错误码
#if VOS_APP_OS == VOS_OS_LINUX
 #define CONN_ERR_TIMEO
 #define CONN_ERR_EBADF EBADF
#elif VOS_APP_OS == VOS_OS_WIN32
 #define CONN_ERR_EBADF WSAEINTR
 #define CONN_ERR_TIMEO WSAETIMEDOUT
#endif

#if VOS_APP_OS == VOS_OS_WIN32
enum tagSockEvent
{
    EPOLLIN  = 0x1,
    EPOLLOUT = 0x2
};

 #ifndef INET_ADDRSTRLEN
  #define  INET_ADDRSTRLEN 16
 #endif

 #ifndef MSG_WAITALL
  #define MSG_WAITALL 0
 #endif

 #ifndef MSG_DONTWAIT
  #define MSG_DONTWAIT 0
 #endif

 #ifndef MSG_NOSIGNAL
  #define MSG_NOSIGNAL 0
 #endif

 #ifndef socklen_t
  #define socklen_t int
 #endif

#endif //#if win32

class CNetworkAddr
{
public:
    CNetworkAddr();
    ~CNetworkAddr();
public:
    long m_lIpAddr;   //网络序列
    USHORT m_usPort;  //网络序列
};

typedef enum tagConnStatus
{
    enIdle = 0,
    enConnecting,
    enConnected,
    enConnFailed,
    enClosed
} ConnStatus;

typedef enum tagEnumSyncAsync
{
    enSyncOp  = 1,
    enAsyncOp = 0
} EnumSyncAsync;

typedef enum tagEpollEventType
{
    enEpollRead  = 0,
    enEpollWrite = 1
} EpollEventType;

class CHandle;
class CHandleNode;
typedef std::list <CHandleNode *> ListOfHandle;
typedef ListOfHandle::iterator    ListOfHandleIte;

class CHandle
{
public:
    CHandle();
    virtual ~CHandle();

public:

    /**************************************************************************
    * name       : initHandle
    * description: 初始化函数
    * input      : NA
    * output     : NA
    * return     : VOS_SUCCESS    init success
                   VOS_FAIL       init fail
    * remark     : NA
    **************************************************************************/
    virtual long initHandle(void);

    /**************************************************************************
    * name       : setHandleSend
    * description: 设置是否检测写事件
    * input      : bHandleSend: VOS_TRUE表示检测，VOS_FALSE表示不检测
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void setHandleSend(VOS_BOOLEAN bHandleSend);

    /**************************************************************************
    * name       : setHandleRecv
    * description: 设置是否检测读事件
    * input      : bHandleSend: VOS_TRUE表示检测，VOS_FALSE表示不检测
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void setHandleRecv(VOS_BOOLEAN bHandleRecv);

    /**************************************************************************
    * name       : getEvents
    * description: 取事件值
    * input      : NA
    * output     : NA
    * return     : 事件值
    * remark     : NA
    **************************************************************************/
    ULONG getEvents(void)
    {
        if (m_pMutexHandle != NULL)
        {
            (void)VOS_MutexLock(m_pMutexHandle);
        }

        ULONG ulEvents = m_ulEvents;

        if (m_pMutexHandle != NULL)
        {
            (void)VOS_MutexUnlock(m_pMutexHandle);
        }

        return ulEvents;
    };

    /**************************************************************************
    * name       : close
    * description: 关闭网络连接
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void close(void);

public:
    long m_lSockFD;					//关联SOCKET
    CHandleNode *m_pHandleNode;		//Node节点
    CNetworkAddr m_localAddr;		//网络地址

#if VOS_APP_OS == VOS_OS_WIN32
    VOS_BOOLEAN m_bReadSelected;
    VOS_BOOLEAN m_bWriteSelected;
#endif  //#if

#if VOS_APP_OS == VOS_OS_LINUX
    long m_lEpfd;
#endif  //#if
    ULONG m_ulEvents;				//对应事件
    VOS_Mutex *m_pMutexHandle;		//处理锁
};

class CHandleNode
{
public:
    CHandleNode() : m_pHandle(NULL), m_bRemoved(VOS_FALSE)
    {
    };

public:
    CHandle *m_pHandle;			//关联Handle
    VOS_BOOLEAN m_bRemoved;		//是否已经删除
};

class CNetworkHandle : public CHandle
{
public:
    CNetworkHandle();
    virtual ~CNetworkHandle(){};

public:

    /**************************************************************************
    * name       : initHandle
    * description: 初始化网络连接
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual long initHandle(void);

    /**************************************************************************
    * name       : getSockFD
    * description: 取SockFD
    * input      : NA
    * output     : NA
    * return     : SockFD
    * remark     : NA
    **************************************************************************/
    long getSockFD(void) const /*lint -e1714*/ //接口函数，本类不调用
    {
        return m_lSockFD;
    };
#if VOS_APP_OS == VOS_OS_LINUX
    long         sendMsg(const struct msghdr *pMsg);
#endif

    /**************************************************************************
    * name       : recv
    * description: 接收函数
    * input      : ulDataSize  数据长度
                   bSyncRecv   VOS_TRUE表示同步，VOS_FALSE表示异步
    * output     : pArrayData  数据buffer
                   pPeerAddr   对端地址
    * return     : 接收字节数(>0)
                   SendRecvError  接收失败
    * remark     : NA
    **************************************************************************/
    virtual long recv(char *pArrayData, CNetworkAddr *pPeerAddr, const ULONG ulDataSize,
                      const EnumSyncAsync bSyncRecv);

public:

    /**************************************************************************
    * name       : handle_recv
    * description: 把接收到的事件放到事件队列中函数
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void handle_recv(void);

    /**************************************************************************
    * name       : handle_send
    * description: 发送数据
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void handle_send(void);
};

class CTcpConnHandle : public CNetworkHandle
{
public:
    CTcpConnHandle();
    virtual ~CTcpConnHandle();

public:

    /**************************************************************************
    * name       : initHandle
    * description: 初始化函数
    * input      : NA
    * output     : NA
    * return     : VOS_SUCCESS      init success
                   VOS_FAIL         init fail
    * remark     : NA
    **************************************************************************/
    virtual long initHandle(void);

    /**************************************************************************
    * name       : conn
    * description: 创建连接函数
    * input      : pLocalAddr  本地地址
                   pPeerAddr   对端地址
                   bSyncConn   VOS_TRUE表示同步连接，VOS_FALSE表示异步连接
                   ulTimeOut   超时时间
    * output     : NA
    * return     : VOS_SUCCESS      connect success
                   VOS_FAIL         connect fail
    * remark     : NA
    **************************************************************************/
    virtual long conn( const CNetworkAddr *pLocalAddr, const CNetworkAddr *pPeerAddr,
                       const EnumSyncAsync bSyncConn, ULONG ulTimeOut);

    long syn2Asy(const long lSockFd, const EnumSyncAsync bSyncConn, ULONG ulTimeOut);

    long connPeer(const long lSockFd, const CNetworkAddr *pPeerAddr, 
                  const EnumSyncAsync bSyncConn, ULONG ulTimeOut);

    long connMode(const long lSockFd, const EnumSyncAsync bSyncConn, ULONG ulTimeOut);
    
    /**************************************************************************
    * name       : send
    * description: 发送函数
    * input      : pArrayData  数据buffer
                   ulDataSize  数据长度，
                   bSyncSend   VOS_TRUE表示同步发送，VOS_FALSE表示异步发送
                   bSyncSend   同步/异步
    * output     : NA
    * return     : 发送字节数(>0)
                   SendRecvError  发送失败
    * remark     : NA
    **************************************************************************/
    virtual long send(const char *pArrayData, const ULONG ulDataSize,
                      const EnumSyncAsync bSyncSend);

    /**************************************************************************
    * name       : send_n
    * description: 发送全部函数
    * input      : pArrayData  数据buffer
                   ulDataSize  数据长度
    * output     : NA
    * return     : 发送字节数(>0)
                   SendRecvError  发送失败
    * remark     : NA
    **************************************************************************/
    virtual long send_n(const char *pArrayData, const ULONG ulDataSize);

    /**************************************************************************
    * name       : recv
    * description: 接收函数
    * input      : ulDataSize  数据长度
                   bSyncRecv   VOS_TRUE表示同步，VOS_FALSE表示异步
    * output     : pArrayData  数据buffer
                   pPeerAddr   对端地址
    * return     : 接收字节数(>0)
                   SendRecvError  接收失败
    * remark     : NA
    **************************************************************************/
    virtual long recv(char *pArrayData, CNetworkAddr *pPeerAddr, const ULONG ulDataSize,
                      const EnumSyncAsync bSyncRecv);

    /**************************************************************************
    * name       : recvWithTimeout
    * description: 接收函数
    * input      : ulDataSize   数据长度
                   ulTimeOut    等待时长
                   ulSleepTime  检测间隔(ms)
    * output     : pArrayData  数据buffer
                   pPeerAddr   对端地址
    * return     : 接收字节数(>0)
                   SendRecvError  接收失败
    * remark     : NA
    **************************************************************************/
    virtual long recvWithTimeout(char *pArrayData, CNetworkAddr *pPeerAddr,
                                 const ULONG ulDataSize, const ULONG ulTimeOut, const ULONG ulSleepTime);

    /**************************************************************************
    * name       : getStatus
    * description: 接收函数
    * input      : NA
    * output     : NA
    * return     : 连接状态
    * remark     : NA
    **************************************************************************/
    virtual ConnStatus getStatus(void) const
    {
        return m_lConnStatus;
    };

    /**************************************************************************
    * name       : close
    * description: 关闭连接
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void close(void);

    //BEGIN R001C05LITSB010 ADD 2010-07-12 ligengqiang 00124479 for 录像下载流量限制，自适应缓冲区大小
    //设置接收 Buffer大小
    void         SetRecvBufferSize(unsigned long ulSize);
    //END   R001C05LITSB010 ADD 2010-07-12 ligengqiang 00124479 for 录像下载流量限制，自适应缓冲区大小

public:
    ConnStatus m_lConnStatus;	//连接状态
    CNetworkAddr m_peerAddr;	//对端地址

    //BEGIN R001C05LITSB010 ADD 2010-07-12 ligengqiang 00124479 for 录像下载流量限制，自适应缓冲区大小
    unsigned long m_ulRecvBufferSize;		//接收Buffer大小
    //END   R001C05LITSB010 ADD 2010-07-12 ligengqiang 00124479 for 录像下载流量限制，自适应缓冲区大小
};

class CTcpServerHandle : public CHandle
{
public:

    /**************************************************************************
    * name       : listen
    * description: 启动等待对端连接
    * input      : pLocalAddr  本地地址
    * output     : NA
    * return     : VOS_SUCCESS       listen success
                   VOS_FAIL          listen fail
    * remark     : NA
    **************************************************************************/
    long         listen(const CNetworkAddr *pLocalAddr);

public:

    /**************************************************************************
    * name       : handle_accept
    * description: 实现基类的handle_accept，用于接收到tcp连接后，产生TcpConnHandle
    * input      : pRemoteAddr  远程解码器地址
                   pTcpConnHandle 连接句柄
    * output     : NA
    * return     : VOS_SUCCESS     成功
                   VOS_FAIL        失败
    * remark     : NA
    **************************************************************************/
    virtual long handle_accept(const CNetworkAddr *pRemoteAddr,
                               CTcpConnHandle *&   pTcpConnHandle);

    /**************************************************************************
    * name       : close
    * description: 关闭连接
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void close(void);
};

#define MAX_HANDLE_MGR_TYPE_LEN 20
class CHandleManager
{
public:
    CHandleManager();
    virtual ~CHandleManager();

public:

    /**************************************************************************
    * name       : init
    * description: 初始化函数
    * input      : ulSelectPeriod    事件检测间隔，单位为ms
    * output     : NA
    * return     : VOS_SUCCESS       init success
                   VOS_FAIL          init fail
    * remark     : NA
    **************************************************************************/
    long          init(const ULONG ulMilSeconds);

    /**************************************************************************
    * name       : run
    * description: 创建线程，启动事件检测主循环
    * input      : ulSelectPeriod    事件检测间隔，单位为ms
    * output     : NA
    * return     : VOS_SUCCESS       success
                   VOS_FAIL          fail
    * remark     : NA
    **************************************************************************/
    long          run();

    /**************************************************************************
    * name       : exit
    * description: 发送退出通知给事件检测主循环，退出线程
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void          exit();

public:

    /**************************************************************************
    * name       : addHandle
    * description: 注册需要检测事件的handle
    * input      : pHandle  需要检测事件的handle
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    long          addHandle(CHandle *pHandle,
                            VOS_BOOLEAN       bIsListOfHandleLocked = VOS_FALSE);

    long addHandle2List(CHandle *   pHandle,
                        CHandleNode *pHandleNode);
    /**************************************************************************
    * name       : removeHandle
    * description: 取消注册需要检测事件的handle
    * input      : pHandle  需要检测事件的handle
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void          removeHandle(CHandle *pHandle);

    /**************************************************************************
    * name       : checkSelectResult
    * description: 根据得到的事件调用相应的handle处理事件
    * input      : enEpEvent   检测到的事件
                   pHandle     需要检测事件的handle
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void  checkSelectResult(const EpollEventType enEpEvent,
                                    CHandle *             pHandle);

protected:

    /**************************************************************************
    * name       : invoke
    * description: 创建线程，启动事件检测主循环
    * input      : rgc    保存对象实例指针
    * output     : NA
    * return     : VOS_SUCCESS       success
                   VOS_FAIL          fail
    * remark     : NA
    **************************************************************************/
    static void * invoke(void *argc);

    /**************************************************************************
    * name       : mainLoop
    * description: 事件检测主循环
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void          mainLoop();
    void setSelectEvent();

    void netEventDeal();

protected:
    ListOfHandle m_listHandle;				//Handle列表
	ListOfHandleIte m_curListIte;			//当前迭代器位置 2012-12-26 高书明 fd_set只有64个，超过64个连接就不处理了
    VOS_Mutex *m_pMutexListOfHandle;		//Handle处理锁

#if VOS_APP_OS == VOS_OS_LINUX
    long m_lEpfd; //用于epoll的句柄
    struct epoll_event m_epEvents[EPOLL_MAX_EVENT];
#elif VOS_APP_OS == VOS_OS_WIN32
    fd_set m_readSet;
    fd_set m_writeSet;
    timeval m_stSelectPeriod;            //select周期
#endif

    ULONG m_ulSelectPeriod;
    VOS_Thread *m_pVosThread;			//处理线程对象
    VOS_BOOLEAN m_bExit;				//是否退出
    char m_szMgrType[MAX_HANDLE_MGR_TYPE_LEN];//管理类型
};

class CTcpConnMgr : public CHandleManager
{
public:
    CTcpConnMgr()
    {
        if(!CToolsHelp::Strncpy(m_szMgrType, MAX_HANDLE_MGR_TYPE_LEN, "CTcpConnMgr", strlen("CTcpConnMgr")))
		{
			
		}
    };

    /**************************************************************************
    * name       : lockListOfHandle
    * description: 对List Handle 加锁
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void         lockListOfHandle();

    /**************************************************************************
    * name       : unlockListOfHandle
    * description: 对List Handle 解锁
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void         unlockListOfHandle();

protected:

    /**************************************************************************
    * name       : checkSelectResult
    * description: 根据得到的事件调用相应的handle处理事件
    * input      : enEpEvent   检测到的事件
                   pHandle     需要检测事件的handle
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void checkSelectResult(const EpollEventType enEpEvent,
                                   CHandle *            pHandle); /*lint !e1768*/ //需要对外屏蔽该接口
};

class CTcpServerMgr : public CHandleManager
{
public:
    CTcpServerMgr() : m_pTcpConnMgr(NULL)
    {
        if(!CToolsHelp::Strncpy(m_szMgrType, MAX_HANDLE_MGR_TYPE_LEN, "CTcpServerMgr", strlen("CTcpServerMgr")))
		{
			
		}
    };

public:

    /**************************************************************************
    * name       : setTcpClientMgr
    * description: 设置TCP连接管理对象
    * input      : pTcpConnMgr  TCP连接管理对象
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void setTcpClientMgr(CTcpConnMgr *pTcpConnMgr)
    {
        m_pTcpConnMgr = pTcpConnMgr;
    };

protected:

    /**************************************************************************
    * name       : checkSelectResult
    * description: 根据得到的事件调用相应的handle处理事件
    * input      : enEpEvent   检测到的事件
                   pHandle     需要检测事件的handle
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void checkSelectResult(const EpollEventType enEpEvent,
                                   CHandle *            pHandle); /*lint !e1768*/ //需要对外屏蔽该接口

    long readEvent(const EpollEventType enEpEvent, CTcpServerHandle *pTcpServerHandle);


protected:
    CTcpConnMgr *m_pTcpConnMgr;			// 连接管理
};

#define DEFAULT_SELECT_PERIOD 20

// 4种日志操作
#define    CONN_OPERATOR_LOG 16
#define    CONN_RUN_LOG 17
#define    CONN_SECURITY_LOG 20
#define    CONN_USER_LOG 19

class CConnMgr
{
public:
    CConnMgr();
    ~CConnMgr();

public:

    /**************************************************************************
    * name       : init
    * description: 初始化函数
    * input      : NA
    * output     : NA
    * return     : VOS_SUCCESS    init success
                   VOS_FAIL       init fail
    * remark     : NA
    **************************************************************************/
    virtual long init(const ULONG ulSelectPeriod, const VOS_BOOLEAN bHasUdpSock,
                      const VOS_BOOLEAN bHasTcpClient, const VOS_BOOLEAN bHasTcpServer);

    /**************************************************************************
    * name       : run
    * description: 启动各个manager
    * input      : NA
    * output     : NA
    * return     : VOS_SUCCESS   start success
                   VOS_FAIL      start fail
    * remark     : NA
    **************************************************************************/
    virtual long run(void);

    /**************************************************************************
    * name       : exit
    * description: 退出各个manager
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void exit(void);

public:

    /**************************************************************************
    * name       : setDefaultLocalAddr
    * description: 设置本地缺省地址
    * input      : szLocalIpAddr  本地地址
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void setDefaultLocalAddr(const char *szLocalIpAddr);

    /**************************************************************************
    * name       : regTcpClient
    * description: 创建TCP客户端
    * input      : pLocalAddr  本地地址，
                   pPeerAddr   对端地址，
                   pTcpConnHandle   连接对应的handle
                   bSyncConn        VOS_TRUE表示同步连接，VOS_FALSE表示异步连接
    * output     : NA
    * return     : VOS_SUCCESS  connect success
                   VOS_FAIL     connect fail
    * remark     : NA
    **************************************************************************/
    virtual long regTcpClient( const CNetworkAddr *pLocalAddr,
                               const CNetworkAddr *pPeerAddr, CTcpConnHandle *pTcpConnHandle,
                               const EnumSyncAsync bSyncConn, ULONG ulTimeOut);

    /**************************************************************************
    * name       : removeTcpClient
    * description: 注销连接函数
    * input      : pTcpConnHandle  连接对应的handle
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void removeTcpClient(CTcpConnHandle *pTcpConnHandle);

    /**************************************************************************
    * name       : regTcpServer
    * description: 创建TCP服务器
    * input      : pLocalAddr  本地地址
                   pTcpServerHandle  TCP服务器对应的handle
    * output     : NA
    * return     : VOS_SUCCESS    listen success
                   VOS_FAIL       listen fail
    * remark     : NA
    **************************************************************************/
    virtual long regTcpServer(const CNetworkAddr *pLocalAddr,
                              CTcpServerHandle *  pTcpServerHandle);

    /**************************************************************************
    * name       : regTcpClient
    * description: 注销TCP服务器
    * input      : pTcpServerHandle  TCP服务器对应的handle
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void removeTcpServer(CTcpServerHandle *pTcpServerHandle);

protected:
    long m_lLocalIpAddr;			// 本地IP地址
    CTcpConnMgr *m_pTcpConnMgr;		// TCP客户端连接管理
    CTcpServerMgr *m_pTcpServerMgr; // TCP服务端管理
};

#endif //CONN_MGR
