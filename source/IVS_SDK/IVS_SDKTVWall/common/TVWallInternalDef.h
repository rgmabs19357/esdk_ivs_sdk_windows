/********************************************************************
	filename	: 	TVWallInternalDef.h
	author		:	
	created		:	2011/10/29	
	description	:	提供IVS SDK中不需要暴露给第三方使用的数据类型（IVSSDK接口中需要用到的数据类型）
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/29 初始版本
*********************************************************************/
#ifndef TV_WALL_INTERNAL_DEF_H
#define TV_WALL_INTERNAL_DEF_H

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
#define TVWALL_CHECK_POINTER( pPointer, retValue ) \
if ( NULL == ( pPointer ) )                 \
{                                           \
    IVS_RUN_LOG_ERR( "指针为NULL");       \
    return retValue;                        \
}                                           \

/*
 *	判断指针是否为空,无返回值
 */
#define TVWALL_CHECK_POINTER_VOID( pPointer )      \
if ( NULL == ( pPointer ) )                 \
{                                           \
    IVS_RUN_LOG_ERR( "指针为NULL");       \
    return ;                                \
}                                           \

/*
 *	New对象的宏定义，带try catch
 */
#define TVWALL_NEW_TRYCATCH(pPointer, stType, retValue)\
try                                             \
{                                               \
    pPointer = new stType();                    \
}                                               \
catch (...)                                     \
{                                               \
    IVS_RUN_LOG_ERR( "创建对象失败");         \
    return retValue;                            \
}                                               \

/*
 *	New对象的宏定义，带try catch，无返回值
 */
#define TVWALL_NEW_TRYCATCH_VOID(pPointer, stType)     \
try                                             \
{                                               \
    pPointer = new stType();                    \
}                                               \
catch (...)                                     \
{                                               \
    IVS_RUN_LOG_ERR( "创建对象失败");         \
    return ;                                    \
}                                               \

//分页查询通用请求头
#define LOGIN_ID_LEN            64          //用户登录唯一标识长度
#define ORG_COD_LEN             10          //组织编码长度
#define ALARM_NAME_LEN       21          //告警输入设备名称长度
#define ENABLE_LEN           4           //使能长度，“ON”或“OFF”
#define  SERVICE_TYPE_LEN 16
#define  CODE_LEN 36
//#define  STREAM_ID_LEN 8
#define  PROTOCOL_LEN 16
#define  IP_LEN 17
#define  PORT_LEN 6
#define  CODE_PROTOCOL_LEN 28
#define  TIME_SPAN_LEN 36
#define  SERVER_IP_LEN 20
#define  SERVER_PORT_LEN 10

/*
 *	通用返回值宏定义
 */
#define TVWALL_RET_FALSE   -1
#define TVWALL_RET_TRUE    0

/*
 *	平台错误码
 */
#define SDK_SOCKET_TIMEOUT       3
#define SDK_SOCKET_INVALID       4
#define SDK_NULL_CALLBACK        5


#endif //_HTTP_DEF_H_