//lint -e1795
/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名 : RtspSession.h
  作    者 : y00182336
  功能描述 : RTSP连接
  修改历史 :
    1 日期 : 2011-10-12
      作者 : y00182336
      修改 : 创建

 ******************************************************************************/

#ifndef __RTSPSESSION_H_
#define __RTSPSESSION_H_

#include "ace_header.h"
#include <string>
#include <sstream>
#include "RtspMessage.h"
#include "Static_PreAssign_Buf.h"
#include "rtsp_client_datatype.h"
#include "RtspDef.h"

#ifndef ETIME
#define ETIME               137
#endif

// 定义接收缓冲的大小
#define RECV_BUFFER_SIZE        2048

// 同步方式下，单次接收消息的超时时间
#define RECV_SINGLE_TIMEOUT     1

// 定义ANNOUNCE请求
#define RTSP_ANNOUNCE_MSG       "ANNOUNCE "

// 定义RTP交织标志
#define RTP_INTERLEAVE_FLAG    '$'

// 定义交织信息的长度
#define RTP_INTERLEAVE_LENGTH   4

// RTP 数据接收
#define  RTP_DATA_INTERVAL_MS  5 // RTP流接收超时检测 间隔5s
#define  RTP_TIME_OUT_MAX      6 // 累积超时6次 30s，认为断流   

class CRtspSessionManager;


class CRtspSession : public ACE_Event_Handler
{
    friend CRtspSessionManager;

public:
    CRtspSession();
    virtual ~CRtspSession();

    // 建立TCP连接
    int connectPeer(const ACE_INET_Addr &remoteAddr);

    // 断开TCP连接
    int disconnectPeer();

    // 创建RTSP 消息类
    template <typename T>
    T* createRtspMessage(T* )
    {
        if (NULL != m_pRtspMsg)
        {
            delete m_pRtspMsg;
            m_pRtspMsg = NULL;
        }
        try
        {
            m_pRtspMsg = new T;
        }
        catch (...)
        {
            IVS_RUN_LOG_CRI("create rtsp message fail.");
            return NULL;
        }
        m_pRtspMsg->setCSeq(getCSeq());
        return dynamic_cast<T*>(m_pRtspMsg);
    }

    // 设置RTSP SETUP消息
    int setRtspSetupMsg();


    // 删除消息
    void destroyRtspMsg();

    // 发送消息消息
    int sendMessage(const char *pDataBuffer, unsigned int unDataSize);
    
    // 接收响应消息（同步方式）
    int recvMessage(unsigned int timeout);

    // 取消超时定时器
    void cancelRecvTimer();

    // 异步方式下，响应消息超时回调函数
    int handle_timeout(const ACE_Time_Value &tv, const void *arg);

    // 响应消息接收回调函数
    int handle_input(ACE_HANDLE handle);

    // 关闭连接回调函数
    int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

    // 设置超时定时器
    int setRecvTimer(unsigned int timeout);

    // 获取句柄，注册反应器时需要使用
    ACE_HANDLE get_handle() const;

    // 设置连接的状态
    void setStatus(unsigned int status);

    // 获取连接状态
    RTSP_SESSION_STATUS getStatus() const;
    
    unsigned int getCSeq();

    // 处理接收的消息
    int handleRecvedData(const char* pData, unsigned int unDataSize);

    // 处理媒体消息
    int parseRtpRtcpPacket(const char* pData, unsigned int unDataSize);

    // 处理RTSP消息
    unsigned int parseRtspPacket(const char* pData, unsigned int unDataSize);

    // 接收回调函数中处理RTSP响应消息
    int handleRtspRespMsg(const int nRet,const char* psRtspMsg, const unsigned int nLen);

    // 单独解析和处理ANNOUNCE请求
    int handleRtspAnnounceMsg(const char* psRtspMsg, const unsigned int nLen);

    // 处理RTSP Setup响应
    void handleRtspSetupMsg(const int nRet);
    
    // 处理RTSP Play响应
    void handleRtspPlayMsg(const int nRet);

    // 处理RTSP Options响应消息
    void handleRtspOptionsMsg(int nRet, const char* psRtspMsg, const unsigned int nLen);

    // 处理RTSP Pause响应消息
    void handleRtspPauseMsg(const int nRet);

    // 处理RTSP Teardown响应消息
    void handleRtspTeardownMsg(const int nRet);

    // 组装RTSP响应消息
    int encodeRtspRespMsg(const unsigned int unStatusCode, std::string &strMessage);

    void setDelFlag();

    bool getDelFalg();

	int  SetupRtpTimer();
	void KillRtpTimer();
	bool CheckRtpTimeout();

	void EnableRtpTimeoutCheck(bool enable) { m_bEnableTmoCheck = enable; }

	int InitData();

	bool  IsConnectFail() const  { return m_bConnectFail; }

private:

    // 判断连接是否正常
    bool checkIsDisconnect(int nErrNo) const;

    // 解析是否为RTSP请求消息
    bool isRtspMessage(const char* psRtspMsg,const unsigned int nLen);

public:
    bool                    m_bBlocked;                 // 同步异步标志位：true代表同步，false代表异步。

    unsigned int            m_unSessionIndex;           // 连接号

    ACE_Thread_Mutex        m_SessionMutex;             // 连接线程锁
    ACE_Thread_Mutex        m_StreamMutex;              // 避免多线程调用
    ACE_SOCK_Stream         m_tcpStream;                // tcp连接的Stream
    ACE_INET_Addr           m_remoteAddr;               // 远端地址
    ACE_INET_Addr           m_localAddr;

    fDataCallBack           m_fDataCallBack;            // 媒体数据回调函数
    void*                   m_pUser;                    // 用户参数

    CRtspMessage*           m_pRtspMsg;                 // RTSP消息类

    //std::string             m_strDate;                  // playing状态下，响应消息中时间参数
    unsigned int            m_unPlayTimePos;            // options响应消息中的觉对时间

    ACE_Time_Value          m_NowTime;                  // 连接记录的当前时间，用于options检查超时

    ACE_Thread_Semaphore*   m_pRtspSemaphore;           // 信号量
    bool                    m_bRecvRetFlag;             // 消息接收结果标志位
	int						m_iRetCode;
    std::string             m_strTimeInfo;

    ACE_Thread_Mutex            m_Mutex;                        // 线程锁
private:
    RTSP_SESSION_STATUS     m_enSessionStatus;          // 连接状态
    ACE_Thread_Mutex        m_StatusMutex;              // 状态锁  
    
    unsigned int            m_unCSeq;                   // RTSP消息的序列号

    bool                    m_bDelete;

    long                    m_lTimerId;                 // RTSP超时定时器
    Static_PreAssign_Buffer*    m_pRecvBuffer;          // 接收缓冲区

    static const char*      m_strRtspMethod[];          // RTSP 消息类型

	bool                    m_bDataRecved;              // 接收数据标记，用于超时检测
	unsigned int            m_uTmoCounter;              // 超时计数器
	long                    m_lDataTimer;               // 数据接收检测定时器
	bool                    m_bEnableTmoCheck;          // 使能RTP超时检测

	bool                    m_bConnectFail;
};


#endif //__RTSPSESSION_H_
