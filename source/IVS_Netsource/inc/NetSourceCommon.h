/********************************************************************
filename    :    NetSourceCommon.h
author      :    s00191067
created     :    2012/11/01
description :    网络收流库的公共头文件
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/01 初始版本
*********************************************************************/
#ifndef NET_SOURCE_COMMON_H_
#define NET_SOURCE_COMMON_H_

//去掉map告警
#pragma warning(disable : 4786 4503 4996)

//标准库头文件
#include <string>
#include <sstream>
#include <map>
#include <WinSock2.h>
#include <windows.h>

#include "hwsdk.h"

//端口号
#define NET_SOURCE_PORT_MIN_NUMBER 1024             //最小端口号;
#define NET_SOURCE_PORT_MAX_NUMBER 65535            //最大端口号;
#define NET_SOURCE_BEGIN_PORT 35000                 //起始端口号;
#define NET_SOURCE_END_PORT   39000                 //结束端口号;
#define NET_SOURCE_TOTAL_CHANNEL_NUM 128            //最大的通道数量（0--127）;

#define NET_SOURCE_BIND_PORT_RETRY   3              //尝试绑定端口的次数;
#define NET_SOURCE_UDP_SOCKET_NUM    4              //UDP socket数量;

#define RECV_TOTAL_TIMEOUT_TIME     (30*1000)       //接收总共超时时间，单位为毫秒
#define RECV_MEDIA_DATA_TIMEOUT     1               //单次socket收流超时时间，单位为秒
#define SEND_NAT_INTERVAL           (5*1000)       // 发送NAT穿越的间隔时间，单位为毫秒

#define RTP_MEDIA_BUF_LEN           (16 * 1024)            //接收和回调缓冲区长度

// SOCKET 句柄索引;
typedef enum SocketType
{
    VIDEO_RTP_SOCKET = 0,
    VIDEO_RTCP_SOCKET,
    AUDIO_RTP_SOCKET,
    AUDIO_RTCP_SOCKET,
    MULTICAST_SOCKET
}SOCKET_TYPE;

//端口号索引
typedef enum PortType
{
    VIDEO_RTP_PORT = 0,
    VIDEO_RTCP_PORT,
    AUDIO_RTP_PORT,
    AUDIO_RTCP_PORT,
    MULTICAST_PORT
}PORT_TYPE;

//流IP
typedef enum StreamIPType
{
    LOCAL_RECV_STREAM_IP,   // 本地收流IP
    REMOTE_SEND_STREAM_IP   // 远端发流IP
}STREAM_IP_TYPE;

#define NET_RTP_MAX_SOCKET_BUFFER (1024 * 1024 * 2) //先申请1MB的视频socket的缓冲区


// 异常回调函数
typedef  IVS_VOID (CALLBACK *NET_EXCEPTION_CALLBACK)(IVS_ULONG  ulChannel,     // 通道号;
                                                     IVS_INT32  iMsgType,      // 异常消息类型;
                                                     IVS_VOID*  pParam,        // 对应异常的信息，可扩展为结构体指针;
                                                     IVS_VOID*  pUser);        // 用户数据;

// 媒体流回调
typedef  IVS_VOID (CALLBACK *NET_DATA_CALLBACK)(IVS_CHAR    *pBuf,      // 回调数据;
                                                IVS_UINT32  uiSize,     // 数据长度;
                                                IVS_VOID    *pUser);    // 用户数据;

#endif //NET_SOURCE_COMMON_H_
