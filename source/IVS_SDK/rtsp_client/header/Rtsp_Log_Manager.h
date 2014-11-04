/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NSS_Log_Manager.h
  版 本 号   : 初稿
  作    者   : qianjiuyun
  生成日期   : 2010年11月27日
  最近修改   :
  功能描述   : 日志管理类
  函数列表   :
  修改历史   :
  1.日    期   : 2010年11月27日
    作    者   : qianjiuyun
    修改内容   : 创建文件

******************************************************************************/
#ifndef __RTSP_LOG_MANAGER_H__
#define  __RTSP_LOG_MANAGER_H__

#include <string>
#include <list>

#ifdef WIN32
typedef unsigned int uint32_t;
typedef _ULonglong uint64_t;
#endif

#include "ace_header.h"
#include "Rtsp_Log.h"

using namespace std;

#define NSS_LOG_RET_SUC   "SUCCESS"
#define NSS_LOG_RET_FAI   "FAILURE"

typedef enum
{
    LOG_DEBUG_OPENED = 0,
    LOG_DEBUG_CLOSED = 1
}LOG_DEBUG_SWITCH;

// 单条日志的最大长度
#define NSS_SINGLE_LOG_LEN_MAX     1024
#define NSS_LOG_HEADER_LEN_MAX     512

// 日志缓存区大小,单位:B
#define NSS_LOG_BUFF_SIZE      (1024 * 1024)

// 定时刷新日志时长,单位:us
#define NSS_WRITE_LOG_TIME     (100 * 1000)

// 日志文件路径最大长度
#define NSS_LOG_FILE_PATH_LEN       (1024)

#define NSS_LOG_SERVER_NAME_PATH 128

// 定义线程堆栈大小
#define DEFAULT_LOG_STACK_SIZE   (128 * 1024)

// 备份日志文件大小,单位:B
#define NSS_LOG_SIZE_MIN        (10 * 1024 *1024)
#define NSS_LOG_SIZE_MAX        (1024 * 1024 *1024)
#define NSS_LOG_SIZE_DEFAULT    (10 * 1024 *1024)

// 历史日志文件数
#define NSS_LOG_NUM_MIN         1
#define NSS_LOG_NUM_MAX         1024
#define NSS_LOG_NUM_DEFAULT     5

// 日志检测周期,单位:秒
#define NSS_LOG_CHECK_TIME_MIN      (5)
#define NSS_LOG_CHECK_TIME_MAX      (10*60)
#define NSS_LOG_CHECK_TIME_DEFAULT  (1*60)

// 日志文件后缀
#define NSS_LOG_NAME_SUFFIX   ".log"

#define NSS_LOG_TEMP_BUFF_LEN  1024
//==============================CNSS_Log BEGIN================================
// 日志对象.缓存日志，输出日志到日志文件，根据文件大小切换日志文件
class CRtsp_Log
{
public:
    CRtsp_Log();
    virtual ~CRtsp_Log();

    // 初始化日志
    void init(const char * strPrefix, const char * strFileName, NSS_LOG_TYPE enLogType);

    // 关闭日志模块
    void close();

    // 重新设置日志模块配置
    void resetConf(uint32_t ulBackupLogNum, uint32_t ulLogMaxSize,RTSP_LOG_LEVEL_E enLevel);

    // 刷新缓存的日志内容到日志文件
    void refreshLog();

    // 检查历史日志文件
    void checkHistoryLogCount();

    // 打印日志
    void printLog(const char* strTime, const char* strAddInfo,
                  const RTSP_LOG_LEVEL_E logLevel, const char * file, long line, 
                  const char* format, va_list args);

    // 获取当前日志文件信息，判断当前日志文件是否存在
    void statLogFile();

    void closeFd();
private:
    // 打开日志文件
    int openLogFile();

    // 输出日志到日志文件
    void outputLog2File();

    // 输出日志到日志文件，不缓存的方式，直接输出单条日志
    void outputLog2File(const char * strLog, uint32_t ulLogLen);

    // 备份历史日志文件
    void backupLogFile();

private:
    // 文件句柄
    ACE_HANDLE   m_nFd;

    // 当前日志文件大小
    uint64_t m_ulFileSize;

    // 日志文件大小最大值，大于该值时，需要切换日志文件
    uint32_t m_ulFileMaxSize;

    // 保留备份日志文件的数量
    uint32_t m_ulBackupFileMaxNum;

    // 日志级别
    RTSP_LOG_LEVEL_E m_enLevel;
    
    // 日志文件全路径
    char m_strFileAllPath[NSS_LOG_FILE_PATH_LEN + 1];

    // 日志文件名称
    char m_strFileName[NSS_LOG_FILE_PATH_LEN + 1];
    
	// 日志标示
	char m_strPrefix[NSS_LOG_FILE_PATH_LEN + 1];

    // 日志缓存区
    char m_strBuff[NSS_LOG_BUFF_SIZE];
    
    // 日志缓存区已经使用的长度
    uint32_t m_ulBuffLen;
    
    // 日志文件锁
    ACE_Thread_Mutex m_mLock;

    // 日志级别配置
    NSS_LOG_TYPE m_enLogType;

    // 最后写日志时间
    volatile time_t m_ulLastWriteLogTime;

    // 历史文件数
    uint32_t m_ulBackupLogFileCount;
    
    // 历史文件列表
    std::list<string> m_ltBackupLogFileList;
    
    // 历史文件锁
    ACE_Thread_Mutex m_mBackupLogLock;
};
//==============================CNSS_Log END==================================

//===========================CNSS_Log_Manager BEGIN===========================
class CRtsp_Log_Manager: public ACE_Task<ACE_MT_SYNCH>
{
public:
    virtual ~CRtsp_Log_Manager();

    // 单例
    static CRtsp_Log_Manager & instance()
    {
        static CRtsp_Log_Manager logManager;
        return logManager;
    };

    // 初始化日志模块
    int initLog(const char *strServerName,
                const char* strLogRootPath,
                unsigned int unLevel);

    int endLog();
    
    void printLog(const NSS_LOG_TYPE logType, const RTSP_LOG_LEVEL_E logLevel, 
                const char * usr, const char* ip, const char* ret,
                const char * flowID, const char* sessionID,
                const char * file, long line, const char* format, va_list args);

public:// 为了继承属性，外层不要调用
    // 启动线程处理
    int open(void* );

    // 日志管理线程
    int svc(void);

    int close(unsigned long);    


public:// 其他公共函数
    const char* logRootPath()const;

    // 读取配置文件中的日志配置信息
    int readLogConf(NSS_LOG_TYPE enLogType,
                          uint32_t & ulFileMaxSize,
                          uint32_t & ulBackupFileNum, 
                          RTSP_LOG_LEVEL_E &enLevel);
    // 获取debug的开关
    bool DebugOpenedFlag()const
    {
        return m_bDebugWitch;
    }

private:
    CRtsp_Log_Manager();
    // 刷新日志配置
    void refreshLogConf();

    // 检查备份文件数
    void checkHistoryLogFile();

    // 检查当前正在写的日志文件是否存在
    void statLogFile();

public:
    RTSP_LOG_LEVEL_E   m_RefreshLevel;
    bool            m_bDebugFlag;
    char m_strLogRootPath[NSS_LOG_FILE_PATH_LEN + 1];

private:
    // 调试日志对象
    CRtsp_Log m_debugLog;

    // 操作日志对象
    CRtsp_Log m_operationLog;

    // 运行日志对象
    CRtsp_Log m_runLog;

    // 安全日志对象
    CRtsp_Log m_securityLog;

    // 用户日志对象
    CRtsp_Log m_userLog; 

    // 检测日志周期
    uint32_t m_ulCheckInterval;

    // 初始化标识
    bool m_bRunFlag;

    char m_strConfigPath[NSS_LOG_FILE_PATH_LEN + 1];

    char m_strServerName[NSS_LOG_SERVER_NAME_PATH + 1];

    // 日志文件大小最大值，大于该值时，需要切换日志文件
    uint32_t m_ulWholeFileMaxSize;

    // 保留备份日志文件的数量
    uint32_t m_ulWholeBackupFileNum;

    // 日志级别
    RTSP_LOG_LEVEL_E m_enWholeLevel;

    // debug开关. 默认关闭:false
    bool m_bDebugWitch;
};// CNSS_Log_Manager
//===========================CNSS_Log_Manager END=============================

#endif // __RTSP_LOG_MANAGER_H__


