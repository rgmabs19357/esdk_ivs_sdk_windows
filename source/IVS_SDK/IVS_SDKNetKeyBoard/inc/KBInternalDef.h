/********************************************************************
	filename	: 	KBInternalDef.h
	author		:	
	created		:	2011/10/29	
	description	:	提供IVS SDK中不需要暴露给第三方使用的数据类型（IVSSDK接口中需要用到的数据类型）
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/29 初始版本
*********************************************************************/
#ifndef KB_INTERNAL_DEF_H
#define KB_INTERNAL_DEF_H

#include "IVSSDKNetKeyBoardDef.h"
#include "ivs_error.h"
#include "ivs_log.h"

/*
 *	删除指针的宏定义 
 */
#define DELETEP(p)    \
if(NULL != p)         \
{                     \
    delete p;         \
    p = NULL;         \
}                     \

/*
 *	判断指针是否为空 
 */
#define CHECK_POINTER( pPointer, retValue ) \
if ( NULL == ( pPointer ) )                 \
{                                           \
    BP_DBG_LOG("NetKeyBoard Pointer is null.");       \
    return retValue;                        \
}                                           \

/*
 *	判断指针是否为空,无返回值
 */
#define CHECK_POINTER_VOID( pPointer )      \
if ( NULL == ( pPointer ) )                 \
{                                           \
    BP_DBG_LOG("NetKeyBoard Point is null.");       \
    return ;                                \
}                                           \

/*
 *	New对象的宏定义，带try catch
 */
#define NEW_TRYCATCH(pPointer, stType, retValue)\
try                                             \
{                                               \
    pPointer = new stType();                    \
}                                               \
catch (...)                                     \
{                                               \
    BP_DBG_LOG("NetKeyBoard new object fail");              \
    return retValue;                            \
}                                               \

/*
 *	New对象的宏定义，带try catch，无返回值
 */
#define NEW_TRYCATCH_VOID(pPointer, stType)     \
try                                             \
{                                               \
    pPointer = new stType();                    \
}                                               \
catch (...)                                     \
{                                               \
    BP_DBG_LOG("NetKeyBoard new object fail");              \
    return ;                                    \
}                                               \

//分页查询通用请求头
#define LOGIN_ID_LEN            64          //用户登录唯一标识长度
#define ORG_COD_LEN             10          //组织编码长度
#define ALARM_NAME_LEN       21          //告警输入设备名称长度
#define ENABLE_LEN           4           //使能长度，“ON”或“OFF”
#define  SERVICE_TYPE_LEN 16
#define  CODE_LEN 36
#define  STREAM_ID_LEN 8
#define  PROTOCOL_LEN 16
#define  IP_LEN 17
#define  PORT_LEN 6
#define  CODE_PROTOCOL_LEN 28
#define  TIME_SPAN_LEN 36
#define  URL_LEN  1024
#define  SERVER_IP_LEN 20
#define  SERVER_PORT_LEN 10

/*
 *	通用返回值宏定义
 */
#define RET_FALSE   -1
#define RET_TRUE    0

/*
 *	平台错误码
 */
// #define IVS_SUCCEED              0
// #define IVS_FAIL                 1
// #define IVS_PARA_INVALID         2
#define SDK_SOCKET_TIMEOUT       3
#define SDK_SOCKET_INVALID       4
#define SDK_NULL_CALLBACK        5
/*
 *	媒体信息结构体
 */
typedef struct stMediaInfo
{
    char szServiceType[SERVICE_TYPE_LEN];
    char szSrcCode[CODE_LEN];
    char szDstCode[CODE_LEN];
    char szStreamID[STREAM_ID_LEN];
    char szDstTP[PROTOCOL_LEN];
    char szDstPP[PROTOCOL_LEN];
    char szDstCast[PROTOCOL_LEN];
    char szDstIP[IP_LEN];
    char szDstPort[PORT_LEN];
    char szVMedia[CODE_PROTOCOL_LEN];
    char szAMedia[CODE_PROTOCOL_LEN];
    char szTimeSpan[TIME_SPAN_LEN];
	char szServerIP[IP_LEN];
	char szServerPort[PORT_LEN];
}ST_MEDIA_INFO;

/*
 *	媒体信息响应结构体
 */
typedef struct stMediaRsp
{
    char szURL[URL_LEN];
    char szRTSPIP[IP_LEN];
    char szRTSPPORT[PORT_LEN];
}ST_MEDIA_RSP;

/*
 *	实况、回放、下载消息
 */
typedef struct stMessageInfo
{

}ST_MSG_INFO;


#endif //_HTTP_DEF_H_