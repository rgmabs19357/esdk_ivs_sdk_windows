//lint -e1551
/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NSS_Log_Manager.cpp
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
#include "ace_header.h"
#include "Rtsp_Log.h"
#include "Rtsp_Log_Manager.h"
#include "ace/Dirent.h"

const char* g_strRtspLogType[] = {"security", 
                                  "operation",
                                  "run", 
                                  "user",
                                  "debug", 
                                  NULL};

const char* g_strRtspLogLevel[] = {"EMR",
                                   "ALT",
                                   "CRI",
                                   "ERR",
                                   "WAR",
                                   "NTC",
                                   "INF",
                                   "DBG",
                                   NULL};

void nssPrintLog(const NSS_LOG_TYPE logType, const RTSP_LOG_LEVEL_E logLevel, 
                 const char * usr, const char* ip, const char* ret,
                 const char* flowID,const char* sessionID,
                 const char * file, const long line, const char * format, ...)
{
    if (NULL == format)
    {
        // Give a chance for the caller to address NULL input bug
        format = "NULL!!";
    }

    va_list args;
    va_start(args, format);
    CRtsp_Log_Manager::instance().printLog(logType, logLevel, 
                            usr, ip, ret, flowID, sessionID, 
                            file, line, format, args);
    va_end(args);
    return; //lint !e438
}

//==============================CNSS_Log BEGIN================================
CRtsp_Log::CRtsp_Log()
{
    // 文件句柄
    m_nFd = ACE_INVALID_HANDLE;

    // 日志文件大小
    m_ulFileSize = 0;
    
    // 日志文件大小最大值，大于该值时，需要切换日志文件
    m_ulFileMaxSize = NSS_LOG_SIZE_DEFAULT;

    // 保留备份日志文件的数量
    m_ulBackupFileMaxNum = NSS_LOG_NUM_DEFAULT;

    m_enLevel = RTSP_LOG_LEVEL_INFO;

    // 日志文件名称
    memset(m_strFileAllPath, 0, sizeof(m_strFileAllPath));
    memset(m_strFileName, 0, sizeof(m_strFileName));
    memset(m_strPrefix, 0, sizeof(m_strPrefix));

    // 日志缓存区
    memset(m_strBuff, 0, sizeof(m_strBuff));
    
    // 日志缓存区已经使用的长度
    m_ulBuffLen = 0;

    m_enLogType = LOG_TYPE_DBG;

    // 最后写日志时间
    m_ulLastWriteLogTime = 0;

    // 历史文件数
    m_ulBackupLogFileCount = 0;
    
    // 历史文件锁
    ACE_Guard<ACE_Thread_Mutex> locker(m_mBackupLogLock);
    // 历史文件列表
    m_ltBackupLogFileList.clear();
}

CRtsp_Log::~CRtsp_Log()
{
    ACE_Guard<ACE_Thread_Mutex> locker(m_mBackupLogLock);
    try
    {
        // 历史文件列表
        m_ltBackupLogFileList.clear();
    }catch(...)
    {
        
    }
}//lint !e1740

void CRtsp_Log::init(const char * strPrefix, const char * strFileName, NSS_LOG_TYPE enLogType)
{
    (void)ACE_OS::snprintf(m_strFileName, sizeof(m_strFileName), 
                           "%s",
                           strFileName);
    
    m_enLogType = enLogType;

	(void)ACE_OS::snprintf(m_strPrefix, sizeof(m_strPrefix), 
		"%s",
		strPrefix);

    // 日志文件全路径
    (void)ACE_OS::snprintf(m_strFileAllPath, 
                           sizeof(m_strFileAllPath), 
                           "%s/%s/%s_%s%s",
                           CRtsp_Log_Manager::instance().logRootPath(),
                           g_strRtspLogType[enLogType],
                           strFileName,
                           m_strPrefix,
                           NSS_LOG_NAME_SUFFIX);

    //// 备份旧的日志文件,格式:“日志名_YYYYMMDDHHMMSS.log"
    //ACE_stat stat;
    //if(0 == ACE_OS::stat(m_strFileAllPath, &stat))
    //{
    //    time_t currTime = time(NULL);
    //    struct tm tmv;
    //    (void)ACE_OS::localtime_r(&currTime, &tmv);
    //    char strNewFileName[NSS_LOG_FILE_PATH_LEN + 1] = {0,};
    //    (void)ACE_OS::snprintf(strNewFileName, 
    //                           sizeof(strNewFileName), 
    //                           "%s/%s/%s_%s_"
    //                           "%04d%02d%02d%02d%02d%02d"
    //                           "%s",
    //                           CRtsp_Log_Manager::instance().logRootPath(), g_strRtspLogType[enLogType],strFileName, m_strPrefix,
    //                           tmv.tm_year + 1900, tmv.tm_mon + 1,tmv.tm_mday, tmv.tm_hour,tmv.tm_min,tmv.tm_sec,
    //                           NSS_LOG_NAME_SUFFIX);

    //    // 重命名日志文件
    //    (void)ACE_OS::rename(m_strFileAllPath, strNewFileName);
    //}

    char strTempFileAllPath[NSS_LOG_FILE_PATH_LEN + 1];
    (void)ACE_OS::snprintf(strTempFileAllPath, 
        sizeof(strTempFileAllPath), 
        "%s/%s",
        CRtsp_Log_Manager::instance().logRootPath(),
        g_strRtspLogType[enLogType]);

    int iRet = ACE_OS::access(strTempFileAllPath,F_OK);
    if (0 == iRet)
    {
        ACE_Dirent indexDir(strTempFileAllPath);

        for (dirent * directory = indexDir.read(); NULL != directory;  directory = indexDir.read())
        {
            std::string strIndexFileName = directory->d_name;

            std::string::size_type nPosBegin = strIndexFileName.rfind(m_strFileName);
            if (std::string::npos == nPosBegin)
            {
                continue;
            }

            std::string::size_type nPosEnd = strIndexFileName.rfind(NSS_LOG_NAME_SUFFIX);
            if ((std::string::npos == nPosEnd)
                ||((nPosBegin + strlen (m_strFileName)) == nPosEnd))
            {
                continue;
            }
            std::string strLogPath = strTempFileAllPath;
            strIndexFileName = strLogPath + "/" + strIndexFileName;
            ACE_Guard<ACE_Thread_Mutex> locker(m_mBackupLogLock);
            m_ltBackupLogFileList.push_back(strIndexFileName);
            ++m_ulBackupLogFileCount;
        }
    }
    
    return;
}

void CRtsp_Log::close()
{
    refreshLog();
}

void CRtsp_Log::closeFd()
{
    if (ACE_INVALID_HANDLE != m_nFd)
    {
        (void)ACE_OS::close(m_nFd);
        m_nFd = ACE_INVALID_HANDLE;
    }
}

void CRtsp_Log::printLog(const char* strTime, const char* strAddInfo,
                        const RTSP_LOG_LEVEL_E logLevel, const char * file, long line, 
                        const char* format, va_list args)
{
    // 如果要打印的日志比当前的日志配置的级别低，则日志输出完成
    if(logLevel > m_enLevel)
    {
        return;
    }
   
    char strSingleLog[NSS_SINGLE_LOG_LEN_MAX + 1] = {0,};
    uint32_t ulLogLen = 0;
    if(LOG_TYPE_DBG == m_enLogType)
    {
        
        // Get file title of the caller
#ifndef WIN32
        char *strFileTitle = strrchr(const_cast<char *>(file), '/'); 
#else
        char *strFileTitle = strrchr(const_cast<char *>(file), '\\'); 
#endif
        if (NULL == strFileTitle)
        {
            strFileTitle = const_cast<char *>(file);
        }
        else
        {
            strFileTitle++;
        }
    
        // 错误码信息
        char strErrno[NSS_LOG_TEMP_BUFF_LEN] = {0,};
        if(0 != errno)
        {
            (void)ACE_OS::snprintf(strErrno, NSS_LOG_TEMP_BUFF_LEN, "[%d:%s]", errno, strerror(errno));
        }
        
        ulLogLen = (uint32_t)ACE_OS::snprintf(strSingleLog, 
                                NSS_SINGLE_LOG_LEN_MAX, 
                                "%s [%s] [0x%04x] | (%s:%d%s) %s ",
                                strTime, 
                                g_strRtspLogLevel[logLevel],
                                (uint32_t)ACE_OS::thr_self(),
                                strFileTitle, 
                                (int)line,
                                strErrno, 
                                strAddInfo);
    }
    else
    {
        ulLogLen = (uint32_t)ACE_OS::snprintf(strSingleLog, NSS_SINGLE_LOG_LEN_MAX, 
                                "%s [%s] [0x%04x] | %s ",
                                strTime, 
                                g_strRtspLogLevel[logLevel],
                                (uint32_t)ACE_OS::thr_self(),
                                strAddInfo);
    }
    
    ulLogLen += (uint32_t)ACE_OS::vsnprintf(strSingleLog + ulLogLen,
                         NSS_SINGLE_LOG_LEN_MAX - ulLogLen,
                         format,
                         args);
    if (ulLogLen >= NSS_SINGLE_LOG_LEN_MAX)
    {
        ulLogLen = NSS_SINGLE_LOG_LEN_MAX - 1;
        strSingleLog[ulLogLen++] = '\n';
        strSingleLog[ulLogLen] = '\0';
    }
    else
    {
        if (strSingleLog[ulLogLen - 1] != '\n')
        {
            strSingleLog[ulLogLen] = '\n';
            strSingleLog[ulLogLen + 1] = '\0';
            ulLogLen++;
        }
    }

    //1 TODO 日志不做缓存
    ACE_Guard<ACE_Thread_Mutex> locker(m_mLock);
    outputLog2File(strSingleLog, ulLogLen);

    return;
}

// 重新设置日志配置
void CRtsp_Log::resetConf(uint32_t ulBackupLogNum, uint32_t ulLogMaxSize,RTSP_LOG_LEVEL_E enLevel)
{
    m_ulBackupFileMaxNum = ulBackupLogNum;

    m_ulFileMaxSize = ulLogMaxSize;

    m_enLevel = enLevel;
}

int CRtsp_Log::openLogFile()
{
    // 创建日志文件路径,递归创建目录
    char strTempDir[NSS_LOG_FILE_PATH_LEN + 1] = {0, };
    char * strLastPos = m_strFileAllPath;
    char* strNewPos = NULL;
    if(('/' != m_strFileAllPath[0])
        && ('.' != m_strFileAllPath[0]))
    {
        strNewPos = strchr(strLastPos, '/');
        if (NULL != strNewPos)
        {
            strLastPos = strNewPos + 1;
        }
    }
    while(NULL != (strNewPos = strchr(strLastPos, '/'))) //lint !e838
    {
        strNewPos++;
        strncpy(strTempDir, m_strFileAllPath, (uint32_t)(strNewPos - m_strFileAllPath));
        // 如果创建目录失败时，忽略目录已经存在的错误
#ifndef WIN32
        if(0 != ACE_OS::mkdir(strTempDir, 0744))
#else
        if(0 != ACE_OS::mkdir(strTempDir))
#endif
        {
            if(EEXIST != errno )
            {
                return -1;
            }

            errno = 0;                
        }

        strLastPos = strNewPos;
    }

    // 创建日志文件
    int nFlags = O_CREAT | O_WRONLY | O_APPEND;
    m_nFd = ACE_OS::open(m_strFileAllPath, nFlags);
    if (ACE_INVALID_HANDLE == m_nFd)
    {
        return -1;
    }

    m_ulFileSize = 0;
    ACE_stat  stat;
    if(0 == ACE_OS::fstat(m_nFd, &stat))
    {
        m_ulFileSize = (uint64_t)stat.st_size;//lint !e67 !e571
    }

    return 0;
}
// 日志文件备份采用“日志类型nvs_模块名_PID_YYYYMMDDHH24MISS.log”的命名方式；
void CRtsp_Log::backupLogFile()
{
    char strTime[15] = {0,};
    struct tm tmv;
    time_t currTime = time(NULL);
    (void)ACE_OS::localtime_r( &currTime, &tmv );
    char strNewFilePath[NSS_LOG_FILE_PATH_LEN + 1] = {0, };

	(void)ACE_OS::snprintf(strNewFilePath, 
		sizeof(strNewFilePath), 
		"%s/%s/%s_%s"
		"%04d%02d%02d%02d%02d%02d"
		"%s",
		CRtsp_Log_Manager::instance().logRootPath(), g_strRtspLogType[m_enLogType],m_strFileName, m_strPrefix,
		tmv.tm_year + 1900, tmv.tm_mon + 1,tmv.tm_mday, tmv.tm_hour,tmv.tm_min,tmv.tm_sec,
		NSS_LOG_NAME_SUFFIX);

    // 关闭日志文件
    if(ACE_INVALID_HANDLE != m_nFd)
    {
        ACE_OS::close(m_nFd);
        m_nFd = ACE_INVALID_HANDLE;
    }
        
    (void)ACE_OS::rename(m_strFileAllPath, strNewFilePath);

    // 历史文件数增加一个
    ACE_Guard<ACE_Thread_Mutex> locker(m_mBackupLogLock);
    m_ltBackupLogFileList.push_back(strNewFilePath);
    ++m_ulBackupLogFileCount;

    return;
}

void CRtsp_Log::outputLog2File()
{
    if(0 == m_ulBuffLen)
    {
        return;
    }

	ACE_stat  stat;
	if(0 != ACE_OS::fstat(m_nFd, &stat))
	{
		m_ulFileSize = (uint64_t)stat.st_size;//lint !e67 !e571
	}
    // 如果文件大小已经达到最大值，则切换文件
    if(m_ulFileSize >= m_ulFileMaxSize)
    {
        backupLogFile();
    }

    // 如果文件句柄无效，则打开日志文件
    if(ACE_INVALID_HANDLE == m_nFd)
    {
        if(0 != openLogFile())
        {
            return;
        }
    }

    // 输出缓存区的日志到日志文件中
    int nWriteRet = ACE_OS::write(m_nFd, m_strBuff, m_ulBuffLen);

    // 如果写失败了，则关闭日志文件，下次再写
    if(nWriteRet != (int)m_ulBuffLen)
    {
        printf("\n ======error[%d]====write failed.nWriteRet=[%d] , m_ulBuffLen[%u]\n", errno,nWriteRet, m_ulBuffLen);
        (void)ACE_OS::close(m_nFd);
        m_nFd = ACE_INVALID_HANDLE;
        return;
    }


    // 写成功后，修改文件大小和缓存区大小
    (void)ACE_OS::fsync(m_nFd);
    m_ulFileSize += m_ulBuffLen;

    m_ulBuffLen = 0;
    return ;
}

void CRtsp_Log::outputLog2File(const char* strLog, uint32_t ulLogLen)
{
    // 如果文件大小已经达到最大值，则切换文件
    if(m_ulFileSize >= m_ulFileMaxSize)
    {
        backupLogFile();
    }

    // 如果文件句柄无效，则打开日志文件
    if(ACE_INVALID_HANDLE == m_nFd)
    {
        if(0 != openLogFile())
        {
            return;
        }
    }

    // 输出缓存区的日志到日志文件中
    int nWriteRet = ACE_OS::write(m_nFd, strLog, ulLogLen);

    // 如果写失败了，则关闭日志文件，下次再写
    if(nWriteRet != (int)ulLogLen)
    {
        printf(
            "\n ======error[%d]====write failed.nWriteRet=[%d] , ulLogLen[%u]\n", 
            errno,nWriteRet, ulLogLen);
        (void)ACE_OS::close(m_nFd);
        m_nFd = ACE_INVALID_HANDLE;
        
        return;
    }

    m_ulFileSize += ulLogLen;

    return ;
}


// 检查当前正在写的日志文件是否存在
void CRtsp_Log::statLogFile()
{
    ACE_Guard<ACE_Thread_Mutex> locker(m_mLock);
    // 如果当前写的日志文件句柄不可用，则不用检查文件是否存在
    if(ACE_INVALID_HANDLE == m_nFd)
    {
        return;
    }

    // 如果stat成功，则不用处理
    ACE_stat stat;
    if(0 == ACE_OS::stat(m_strFileAllPath, &stat))
    {
        return;
    }

    // 获取日志文件状态失败，则关闭日志文件，触发写日志流程创建日志文件
    (void)ACE_OS::close(m_nFd);
    m_nFd = ACE_INVALID_HANDLE;
    m_ulFileSize = 0;
}

// 刷新缓存的日志内容到日志文件
void CRtsp_Log::refreshLog()
{
    //1 TODO 日志不缓存,定期刷磁盘
    if(ACE_INVALID_HANDLE != m_nFd)
    {
        ACE_Guard<ACE_Thread_Mutex> locker(m_mLock);
        (void)ACE_OS::fsync(m_nFd);
    }

    return;
}

void CRtsp_Log::checkHistoryLogCount()
{
    ACE_Guard<ACE_Thread_Mutex> locker(m_mBackupLogLock);
    // 如果历史文件数超过了所设置的最大值，则删除最早的历史文件，即使历史文件数配置变化
    while(m_ulBackupFileMaxNum <= m_ulBackupLogFileCount)
    {
        m_ltBackupLogFileList.sort();
        string strFileName = m_ltBackupLogFileList.front();
        m_ltBackupLogFileList.pop_front();
        (void)ACE_OS::unlink(strFileName.c_str());
        --m_ulBackupLogFileCount;
    }
}

//==============================CNSS_Log END==================================

//===========================CNSS_Log_Manager BEGIN===========================
CRtsp_Log_Manager::CRtsp_Log_Manager()
{
    m_bRunFlag = false;
    memset(m_strLogRootPath, 0 , sizeof(m_strLogRootPath));
    memset(m_strConfigPath, 0 , sizeof(m_strConfigPath));
    memset(m_strServerName, 0 , sizeof(m_strServerName));
    m_ulCheckInterval = NSS_LOG_CHECK_TIME_DEFAULT;

    // 日志文件大小最大值，大于该值时，需要切换日志文件
    m_ulWholeFileMaxSize = NSS_LOG_SIZE_DEFAULT;

    // 保留备份日志文件的数量
    m_ulWholeBackupFileNum = NSS_LOG_NUM_DEFAULT;

    // 日志级别
    m_enWholeLevel = RTSP_LOG_LEVEL_INFO;
    m_RefreshLevel = RTSP_LOG_LEVEL_WARNING;
    m_bDebugFlag   = false;

    m_bDebugWitch = false;
}

CRtsp_Log_Manager::~CRtsp_Log_Manager()
{
    
}

int CRtsp_Log_Manager::initLog(const char * strServerName,
                               const char * strLogRootPath,
                               unsigned int unLevel)
{
    // 输入参数判断
    if ((NULL == strServerName) || (NULL == strLogRootPath))
    {
        printf("Init log failed. The parameter is invalid.\n");
        return -1;
    }
    
    (void)ACE_OS::snprintf(m_strServerName, sizeof(m_strServerName), "%s", strServerName);
    if ('\0' == m_strLogRootPath[0])
    {
		//modify by limingjie lWX175483 :2013-5-24
		//strcpy 改成 strncpy
        strncpy(m_strLogRootPath, strLogRootPath, strlen(strLogRootPath)+1);
    }

    uint32_t ulLen = strlen(m_strLogRootPath);
    if ((0 != strcmp("/", m_strLogRootPath))
        && (0 != strcmp("./", m_strLogRootPath))
        && (ulLen > 1)
        && ('/' == m_strLogRootPath[ulLen - 1]))
    {
        m_strLogRootPath[ulLen - 1] = '\0';
    }
    // 配置日志
    if (RTSP_LOG_LEVEL_DEBUG < unLevel)
    {
        unLevel = RTSP_LOG_LEVEL_WARNING;
    }
    CRtsp_Log_Manager::instance().m_RefreshLevel = (RTSP_LOG_LEVEL_E) unLevel;

    // 初始化各种日志模块
    m_securityLog.init("security", strServerName, LOG_TYPE_SEC);
    m_operationLog.init("operation", strServerName, LOG_TYPE_OPR);
    m_runLog.init("run", strServerName, LOG_TYPE_RUN);
    m_userLog.init("user", strServerName, LOG_TYPE_USR);
    m_debugLog.init("debug", strServerName, LOG_TYPE_DBG);
    m_bDebugWitch = m_bDebugFlag;

    // 初始化日志配置
    refreshLogConf();  

    // 日志检测周期设置
    m_ulCheckInterval = NSS_LOG_CHECK_TIME_DEFAULT;

    if (0 != this->open(0))
    {
        printf("\n\r=======[%s] failed to start.Create log thread failed.========\n",
               strServerName);
        
        return -1;
    }

    // 初始化完成后，打印一句启动的运行日志
    IVS_RUN_LOG_EMR("\n\r==========[%s]System start running=========,ProcessID[%d]", 
                    strServerName, 
                    (int)ACE_OS::getpid());

    return 0;
}

int CRtsp_Log_Manager::endLog()
{
    m_bRunFlag = false;
    //强制把最后的日志刷新到日志文件中
    m_securityLog.close();
    m_securityLog.closeFd();
    m_operationLog.close();
    m_operationLog.closeFd();
    m_runLog.close();
    m_runLog.closeFd();
    m_userLog.close();
    m_userLog.closeFd();
    m_debugLog.close();
    m_debugLog.closeFd();
    return this->close(0);
}

int CRtsp_Log_Manager::open(void *)
{
    m_bRunFlag = true;

    size_t stack_size[1] = {DEFAULT_LOG_STACK_SIZE};
    // 启动媒体处理线程
    int nRet = activate(THR_NEW_LWP | THR_JOINABLE,                      //lint !e835
                        1, // 线程数
                        0,
                        ACE_DEFAULT_THREAD_PRIORITY,
                        -1,
                        0,
                        0,
                        0,
                        stack_size, // 线程堆栈大小
                        0 );
    if (-1 == nRet)
    {
        return -1;
    }
    
    return 0;    
}

int CRtsp_Log_Manager::svc(void)
{  
    //time_t lastChangeTime = 0;
    
    // 先检测文件，再sleep，保证第一次配置
    while(m_bRunFlag)
    {
        // 每隔一个检测周期检测一次日志配置
        for (uint32_t i = 0; i < m_ulCheckInterval; i++)
        {
            // 为防止sleep时间过长，不能及时响应退出消息，这里只sleep 1秒
            ACE_OS::sleep(ACE_Time_Value((time_t)1, 0));

            // 刷新日志文件，防止太长时间没有写日志
            m_securityLog.refreshLog();

            m_operationLog.refreshLog();

            m_runLog.refreshLog();

            m_userLog.refreshLog();

            m_debugLog.refreshLog();

            if (!m_bRunFlag)
            {
                return 0;
            }
        }

        // 检查当前写的日志文件是否存在，如果不存在，则关闭日志文件句柄，触发写日志流程创建日志文件
        statLogFile();
        
        // 检测每种日志文件的数，是否超过最大数，如果超过最大数，则删除最早的
        checkHistoryLogFile();
        refreshLogConf();
        m_bDebugWitch = m_bDebugFlag;
    }
        
    
    return 0;
}

int CRtsp_Log_Manager::close(unsigned long)
{
    //(void)wait();
    this->thr_mgr_ = 0;
    
    return 0;
}

// 打印日志。如果debug开关打开了，则打印其他类别日志的同时，打印所有日志到debug文件中
void CRtsp_Log_Manager::printLog(const NSS_LOG_TYPE logType, const RTSP_LOG_LEVEL_E logLevel, 
                const char * usr, const char* ip, const char* ret,
                const char * flowID, const char* sessionID,
                const char * file, long line, const char* format, va_list args)
{
    // 未初始化时，打印前台日志
    if(!m_bRunFlag)
    {
        return;
    }
    // 11-25 11:42:13.786404
    ACE_Time_Value uSec = ACE_OS::gettimeofday();
    struct tm tmv;
    time_t currTime = time(NULL);
    (void)ACE_OS::localtime_r( &currTime, &tmv );
    char strTime[64];
    (void)ACE_OS::snprintf(strTime, sizeof(strTime), 
                "%04d-%02d-%02d %02d:%02d:%02d.%06d", 
                tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday, tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
                (int)uSec.usec());

    // 处理附加信息的格式拼接
    char strAddInfo[NSS_LOG_TEMP_BUFF_LEN+ 1] = {0,};
    // 日志分类处理
    switch(logType)
    {
        case LOG_TYPE_SEC:
            (void)ACE_OS::snprintf(strAddInfo, sizeof(strAddInfo), "%s %s |",usr, ret);
            m_securityLog.printLog(strTime, strAddInfo,logLevel,file,line, format,args);
            break;
        
        case LOG_TYPE_OPR:
            (void)ACE_OS::snprintf(strAddInfo, sizeof(strAddInfo), "%s %s %s |",usr, ip, ret);
            m_operationLog.printLog(strTime,strAddInfo,logLevel,file,line, format,args);
            break;

        case LOG_TYPE_RUN:
            m_runLog.printLog(strTime,strAddInfo,logLevel,file,line, format,args);
            break;
            
        case LOG_TYPE_USR:
            (void)ACE_OS::snprintf(strAddInfo, sizeof(strAddInfo), "%s |[%s] [%s]",usr, flowID, sessionID);
            m_userLog.printLog(strTime,strAddInfo,logLevel,file,line, format,args);
            break;

        case LOG_TYPE_DBG:
            break;
            
        default:
            return;
    }

    // 如果debug级别的日志被打开了，则打印debug级别的日志。
    if(m_bDebugWitch)
    {
        m_debugLog.printLog(strTime,strAddInfo, logLevel, file, line, format,args);
    }

    errno = 0;
    return;
}

// 读取日志配置，根据enLogType读取对应的配置项
int CRtsp_Log_Manager::readLogConf(NSS_LOG_TYPE enLogType, 
                                       uint32_t& ulFileMaxSize,
                                       uint32_t& ulBackupFileNum,
                                       RTSP_LOG_LEVEL_E& enLevel)
{
    ulFileMaxSize = m_ulWholeFileMaxSize;        
    ulBackupFileNum = m_ulWholeBackupFileNum;
 
    // 读取日志级别配置,调试日志只是开关，没有级别，默认是关闭的
    if(LOG_TYPE_DBG == enLogType)
    {
        // 默认关闭
        enLevel = RTSP_LOG_LEVEL_EMERGENCY;
        m_bDebugWitch = false;
    }
    else
    {
        enLevel = m_enWholeLevel;
    }

    return 0;
}

const char* CRtsp_Log_Manager::logRootPath()const
{
    return m_strLogRootPath;
}

// 刷新日志配置信息:日志级别、备份日志文件数、日志备份文件的大小
void CRtsp_Log_Manager::refreshLogConf()
{
    // 读取整体的配置
    // 日志文件大小最大值，大于该值时，需要切换日志文件
    uint32_t ulFileMaxSize = NSS_LOG_SIZE_DEFAULT;

    // 保留备份日志文件的数量
    uint32_t ulBackupFileNum = NSS_LOG_NUM_DEFAULT;

    // 日志级别
    RTSP_LOG_LEVEL_E enLevel = m_RefreshLevel;
    m_securityLog.resetConf(ulBackupFileNum, ulFileMaxSize, enLevel);
    m_operationLog.resetConf(ulBackupFileNum, ulFileMaxSize, enLevel);
    m_runLog.resetConf(ulBackupFileNum, ulFileMaxSize, enLevel);
    m_userLog.resetConf(ulBackupFileNum, ulFileMaxSize, enLevel);
    m_debugLog.resetConf(ulBackupFileNum, ulFileMaxSize, enLevel);    

    return;
}

// 检查当前正在写的日志文件是否存在
void CRtsp_Log_Manager::statLogFile()
{
    m_securityLog.statLogFile();

    m_operationLog.statLogFile();

    m_runLog.statLogFile();
    
    m_userLog.statLogFile();

    m_debugLog.statLogFile();

    return ;
}

void CRtsp_Log_Manager::checkHistoryLogFile()
{
    m_securityLog.checkHistoryLogCount();

    m_operationLog.checkHistoryLogCount();

    m_runLog.checkHistoryLogCount();
    
    m_userLog.checkHistoryLogCount();

    m_debugLog.checkHistoryLogCount();

    return ;
}

//===========================CNSS_Log_Manager END=============================
//lint +e1551


