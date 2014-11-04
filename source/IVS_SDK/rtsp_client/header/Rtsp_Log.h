/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NSS_Log.h
  版 本 号   : 初稿
  作    者   : qianjiuyun
  生成日期   : 2010年11月27日
  最近修改   :
  功能描述   : 日志头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2010年11月27日
    作    者   : qianjiuyun
    修改内容   : 创建文件

******************************************************************************/
#ifndef __RTSP_LOG_H__
#define __RTSP_LOG_H__

#include "rtsp_client_datatype.h"

// 日志类型
typedef enum
{
    LOG_TYPE_SEC = 0,   // 安全日志
    LOG_TYPE_OPR,       // 操作日志
    LOG_TYPE_RUN,       // 运行日志
    LOG_TYPE_USR,       // 用户日志
    LOG_TYPE_DBG,       // 调试日志
    LOG_TYPE_MAX
}NSS_LOG_TYPE;

// 输出日志
void nssPrintLog(const NSS_LOG_TYPE logType, const RTSP_LOG_LEVEL_E logLevel, 
                const char * usr, const char* ip, const char* ret,
                const char* flowID,const char* sessionID,
                const char * file, const long line, const char * format, ...);


// 安全日志:Error 级别的运行日志；Info 级别的运行日志。
#define IVS_SEC_LOG_ERR(usr, ret, ...) \
    nssPrintLog(LOG_TYPE_SEC,RTSP_LOG_LEVEL_ERROR, usr,"", ret, "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_SEC_LOG_INF(usr, ret, ...) \
    nssPrintLog(LOG_TYPE_SEC,RTSP_LOG_LEVEL_INFO, usr,"", ret, "", "", __FILE__, __LINE__, __VA_ARGS__);

// 操作日志:Error 级别的运行日志；Info 级别的运行日志。
#define IVS_OPR_LOG_ERR(usr, ip,ret,...) \
    nssPrintLog(LOG_TYPE_OPR,RTSP_LOG_LEVEL_ERROR, usr,ip, ret, "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_OPR_LOG_INF(usr, ip,ret,...) \
    nssPrintLog(LOG_TYPE_OPR,RTSP_LOG_LEVEL_INFO, usr,ip, ret, "", "", __FILE__, __LINE__, __VA_ARGS__);

// 运行日志: Emergency 级别的运行日志；Alert 级别的运行日志；Critical 级别的运行日志；Error 级别的运行日志；
            // Warning 级别的运行日志； Notice 级别的运行日志；Info 级别的运行日志。
#define IVS_RUN_LOG_EMR(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_EMERGENCY, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_RUN_LOG_ALT(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_ALART, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_RUN_LOG_CRI(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_CRITICAL, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_RUN_LOG_ERR(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_ERROR, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_RUN_LOG_WAR(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_WARNING, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_RUN_LOG_NTC(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_NOTICE, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);
#define IVS_RUN_LOG_INF(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_INFO, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);

// 用户日志:Critical 级别的用户日志； Error 级别的用户日志；Info 级别的用户日志。
#define IVS_USR_LOG_CRI(usr,flowID,sessionID,...) \
    nssPrintLog(LOG_TYPE_USR,RTSP_LOG_LEVEL_CRITICAL, usr, "", "", flowID, sessionID, __FILE__, __LINE__, __VA_ARGS__);
#define IVS_USR_LOG_ERR(usr,flowID,sessionID,...) \
    nssPrintLog(LOG_TYPE_USR,RTSP_LOG_LEVEL_ERROR, usr, "", "", flowID, sessionID, __FILE__, __LINE__, __VA_ARGS__);
#define IVS_USR_LOG_INF(usr,flowID,sessionID,...) \
    nssPrintLog(LOG_TYPE_USR,RTSP_LOG_LEVEL_INFO, usr, "", "", flowID, sessionID, __FILE__, __LINE__, __VA_ARGS__);

// 调试日志:不分级别  rtsp只记录run日志，调试日志记录到run目录下
#define IVS_DBG_LOG(...) \
    nssPrintLog(LOG_TYPE_RUN,RTSP_LOG_LEVEL_DEBUG, "", "", "", "", "", __FILE__, __LINE__, __VA_ARGS__);

#endif // __RTSP_LOG_H__

