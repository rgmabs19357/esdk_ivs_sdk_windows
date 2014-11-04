
/******************************************************************************
   版权所有 (C), 2008-2011, 华为技术有限公司

 ******************************************************************************
  文件名          : IvsLog.cpp
  版本号          : 1.0
  作者            : xuxin 00114494
  生成日期        : 2008-08-17
  最近修改        : 
  功能描述        : IVS日志模块
  函数列表        : 
  修改历史        : 
  1 日期          : 
    作者          : 
    修改内容      : 
*******************************************************************************/

//BEGIN R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志
//去掉map告警
#pragma warning(disable : 4786 4503)

#include <list>
#include <string>
using namespace std;
//END   R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志

#include <stdio.h>
#include "RingCache.h"
#include "IvsLog.h"

#include <tchar.h>

/************************** Begin 日志对象实现 ********************************/

#define IVSLOG_ERROR_OK             0       //成功
#define IVSLOG_ERROR_INIT_CACHE     (-1)    //初始化缓冲区出错
#define IVSLOG_ERROR_FILE_NAME      (-2)    //自动生成日志文件名出错
#define IVSLOG_ERROR_OPEN_FILE      (-3)    //打开文件出错
#define IVSLOG_ERROR_CREATE_EVENT   (-4)    //创建事件出错
#define IVSLOG_ERROR_CREATE_THREAD  (-5)    //创建线程出错

//默认缓冲区大小1M
#define DEFAULT_CACHE_SIZE          (1024*1024)
//默认文件切换长度，单位Byte
#define DEFAULT_CHANGE_FILE_LEN     (10*1024*1024)
//默认最大文件切换长度，单位Byte
#define MAX_CHANGE_FILE_LEN         (100*1024*1024)
//默认最小文件切换长度，单位Byte
#define MIN_CHANGE_FILE_LEN         (100*1024)
//日志文件路径名长度
#define MAX_LOG_FILE_PATH_NAME_LEN  1024
//单条日志最大长度
#define MAX_LEN_OF_SINGLE_LOG       2048


//等待退出事件的总时间
#define LOG_WAIT_FOR_EXIT_EVENT     5000
//等待结束间隔
#define LOG_WAIT_FOR_EXIT_EVENT_INTERVAL 50
//等待写间隔
#define LOG_WAIT_FOR_WRITE_OVER_INTERVAL 10


class CIvsLog
{
    private:    //单实例
        CIvsLog();
    public:
        virtual ~CIvsLog();

    public:
        static CIvsLog* GetIvsLogInstance();    //获取日志对象
        static void DeleteIvsLogInstance();        //删除日志对象

    public:
        //启动日志
        long Start();
        //设置日志级别 
        void SetLevel(long lLevel);
        //设置当前写的日志文件路径名
        bool SetLogFilePathName(const char* szPathName);
        //设置日志文件长度限制，超过此长度时生成新文件，参数单位KB
        void SetFileLengthLimit(unsigned long ulLimitLengthKB);
        //写日志
        long Write(const char* szFile, long lLine,
            long lLevel, const char* format, va_list argp);
        //停止日志
        long Stop();

    private:
        //读缓冲写文件线程
        static DWORD CALLBACK ThreadEntry(LPVOID lpvoid);
        void WriteLogFileThread();
        bool LogFileLenDeal();
        IvsLogThreadErr WriteFile(char* pLogInfo, unsigned long ulLogDataLen);
        
        const char* GetLevelString(long lLevel) const;
        
        //BEGIN R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志
        void DeleteBackLogFiles() const;
        //END   R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志

    private:
        //日志模块是否被启动
        bool    m_bRun;        
        
        //是否允许写日志
        bool    m_bAllowWrite;  

        //是否磁盘满导致日志停止
        bool    m_bDiskFull;
        //当检测到磁盘空间大于1M时，恢复日志打印
        #define MIN_DISK_SPACE_FOR_LOG      (1024*1024) //磁盘空间大于1M时恢复日志
        #define DISK_SPACE_CHECK_INTERVAL   10000       //磁盘空间检测间隔10秒钟
        //上次检测磁盘满的时间(仅在磁盘满导致日志停止时起作用)
        DWORD   m_dwLastCheckDiskSpaceTime;
        
        //日志级别，默认为INFO级别
        long    m_lLevel;        
        
        //写文件线程的句柄
        HANDLE    m_hWriteThread;    
        
        //写日志事件的句柄
        HANDLE  m_hWriteEvent;  
        
        //写线程退出事件的句柄
        HANDLE    m_hThreadExitEvent;    

        //日志缓冲
        CRingCache    m_Cache;    

        //日志文件
        FILE*    m_pLogFile;        
        
        //日志文件长度限制，单位Byte
        unsigned long    m_ulFileLenLimit;

        //日志文件路径和名称
        char    m_szLogFilePathName[MAX_LOG_FILE_PATH_NAME_LEN];

    private:
        //单实例日志对象
        static CIvsLog*    g_pIvsLog;    
};

CIvsLog* CIvsLog::g_pIvsLog = NULL;

CIvsLog::CIvsLog()
{
    m_bRun = false;
    m_bAllowWrite = false;
    m_bDiskFull = false;
    m_dwLastCheckDiskSpaceTime = 0;
    m_lLevel = LOG_INFO;
    m_hWriteThread = NULL;
    m_hWriteEvent = NULL;
    m_hThreadExitEvent = NULL;

    m_pLogFile = NULL;
    m_ulFileLenLimit = DEFAULT_CHANGE_FILE_LEN;
    ::memset(m_szLogFilePathName,0,MAX_LOG_FILE_PATH_NAME_LEN);
}

CIvsLog::~CIvsLog()
{
    try
    {
        //停止日志模块
        (void)Stop();

        //以下是为过PC-LINT
        if(NULL != m_hWriteThread)
        {
            (void)::CloseHandle(m_hWriteThread);
            m_hWriteThread = NULL;
        }

        if(NULL != m_hThreadExitEvent)
        {
            (void)::CloseHandle(m_hThreadExitEvent);
            m_hThreadExitEvent = NULL;
        }
		if(NULL != m_hWriteEvent)
		{
			(void)::CloseHandle(m_hWriteEvent);
			m_hWriteEvent = NULL;
		}
        if(NULL != m_pLogFile)
        {
            (void)::fclose(m_pLogFile);
            m_pLogFile = NULL;
        }

    }
    catch(...)
    {
    }
}

//获取日志模块的唯一实例
CIvsLog* CIvsLog::GetIvsLogInstance()
{
    //若日志实例未生成则申请对象
    if(NULL == g_pIvsLog)
    {
        g_pIvsLog = new CIvsLog;
    }

    return g_pIvsLog;
}

//删除日志模块的实例
void CIvsLog::DeleteIvsLogInstance()
{
    //BEGIN R001C05LYYS01 MOD 2010-03-19 ligengqiang 00124479 for 朱玉军发现的逻辑错误
    //若已经申请对象则释放
    if(NULL != g_pIvsLog)
    //END   R001C05LYYS01 MOD 2010-03-19 ligengqiang 00124479 for 朱玉军发现的逻辑错误
    {
        //必须要先停止对象
        (void)(g_pIvsLog->Stop());

        //删除对象
        delete g_pIvsLog;
        g_pIvsLog = NULL;
    }
}

//启动日志模块
long CIvsLog::Start()
{
    //如果已经启动则直接返回
    if(m_bRun)
    {
        m_bAllowWrite = true;
        return IVSLOG_ERROR_OK;
    }

    //初始化缓冲区为2M
    unsigned long ulCacheSize = m_Cache.SetCacheSize(DEFAULT_CACHE_SIZE);
    if(DEFAULT_CACHE_SIZE != ulCacheSize)
    {
        //缓冲区申请失败
        return IVSLOG_ERROR_INIT_CACHE;
    }

    //检查文件名
    if(::strlen(m_szLogFilePathName) == 0)
    {
        //根据中间件模块句柄得到路径
        (void)::GetModuleFileName(NULL, m_szLogFilePathName, MAX_LOG_FILE_PATH_NAME_LEN-1);
        char* pszFind = ::strrchr(m_szLogFilePathName, '.');
        if(NULL == pszFind)
        {
            //异常
            return IVSLOG_ERROR_FILE_NAME;
        }
        //添加后缀
        (void)::sprintf(pszFind, ".log");
    }
    //打开文件
    m_pLogFile = ::fopen(m_szLogFilePathName, "a+");
    if(NULL == m_pLogFile)
    {
        //文件无法打开
        return IVSLOG_ERROR_OPEN_FILE;
    }

    //初始化写事件
    m_hWriteEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    if(NULL == m_hWriteEvent)
    {
        //写事件创建失败
        
        //关闭文件
        (void)::fclose(m_pLogFile);
        m_pLogFile = NULL;
        return IVSLOG_ERROR_CREATE_EVENT;
    }

    //初始化写线程退出事件
    m_hThreadExitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    if(NULL == m_hThreadExitEvent)
    {
        //写线程退出事件创建失败
        
        //关闭文件
        (void)::fclose(m_pLogFile);
        m_pLogFile = NULL;

        //关闭写事件句柄
        (void)::CloseHandle(m_hWriteEvent);
        m_hWriteEvent = NULL;
        return IVSLOG_ERROR_CREATE_EVENT;
    }

    //设置日志启动标志
    m_bRun = true;

    //创建读缓冲写文件线程
    unsigned long ulThreadID = 0;
    m_hWriteThread = ::CreateThread(NULL, NULL, ThreadEntry, this, 0, &ulThreadID);
    if(NULL == m_hWriteThread)
    {
        //写线程创建失败，清理资源
        m_bRun = false;

        //关闭文件
        (void)::fclose(m_pLogFile);
        m_pLogFile = NULL;

        //关闭写事件句柄
        (void)::CloseHandle(m_hWriteEvent);
        m_hWriteEvent = NULL;

        //关闭写线程退出事件句柄
        (void)::CloseHandle(m_hThreadExitEvent);
        m_hThreadExitEvent = NULL;
        return IVSLOG_ERROR_CREATE_THREAD;
    }

    //开始接收日志
    m_bAllowWrite = true;

    return IVSLOG_ERROR_OK;
}
//设置日志级别
void CIvsLog::SetLevel(long lLevel)
{
    switch(lLevel)
    {
        case LOG_EMERGENCY:
        case LOG_ALERT:
        case LOG_CRITICAL:
        case LOG_ERROR:
        case LOG_WARNING:
        case LOG_NOTICE:
        case LOG_INFO:
        case LOG_DEBUG:
            m_lLevel = lLevel;
            break;
        default:
            break;
    }
}

//设置当前写的日志文件路径名
bool CIvsLog::SetLogFilePathName(const char* szPathName)
{
    bool bSetOk = false;
    //参数检查
    if((NULL!=szPathName) && (::strlen(szPathName)<MAX_LOG_FILE_PATH_NAME_LEN))
    {
        //复制保存文件名，检查第一个字符是否为斜杠
        if('\\' == szPathName[0])
        {
            //去掉斜杠
            (void)::sprintf(m_szLogFilePathName, "%s", szPathName+1);
        }
        else
        {
            (void)::sprintf(m_szLogFilePathName, "%s", szPathName);
        }

        //循环创建各级文件目录
        char* pszFind = ::strchr(m_szLogFilePathName, '\\');
        while(NULL != pszFind)
        {
            pszFind[0] = '\0';
            (void)::CreateDirectory(m_szLogFilePathName, NULL);
            pszFind[0] = '\\';
            pszFind = ::strchr(pszFind+1, '\\');
        }

        //文件目录创建完成
        bSetOk = true;
    }

    return bSetOk;
}

//设置日志文件长度限制，超过此长度时生成新文件，参数单位KB
void CIvsLog::SetFileLengthLimit(unsigned long ulLimitLengthKB)
{
    //KB转为Byte
    unsigned long ulNewLimitLength = ulLimitLengthKB * 1024;

    //范围确定
    if(ulNewLimitLength < MIN_CHANGE_FILE_LEN)
    {
        //小于最小值使用最小值
        m_ulFileLenLimit = MIN_CHANGE_FILE_LEN;
    }
    else if(ulNewLimitLength < MAX_CHANGE_FILE_LEN)
    {
        //正常范围
        m_ulFileLenLimit = ulNewLimitLength;
    }
    else
    {
        //大于最大值使用最大值
        m_ulFileLenLimit = MAX_CHANGE_FILE_LEN;
    }
}

//写一条日志到缓冲区，如下:
//2008-08-17 10:45:45.939[DEBUG|Log.cpp:152|PID:772|TID:2342|Err:0]程序启动...
long CIvsLog::Write(const char* szFile, long lLine,
                long lLevel, const char* format, va_list argp)
{
    if(!m_bRun)
    {   
        //未启动
        return 0;
    }

    //未允许写
    if(!m_bAllowWrite)
    {
        //不是由于磁盘满导致停止
        if(!m_bDiskFull)
        {
            return 0;
        }

        //磁盘满导致停止
        DWORD dwTime = GetTickCount();
        if(dwTime-m_dwLastCheckDiskSpaceTime < DISK_SPACE_CHECK_INTERVAL)
        {
            //检测时间未到
            return 0;
        }

        //检测磁盘是否达到恢复日志打印的要求
        char szDirectory[MAX_LOG_FILE_PATH_NAME_LEN] = {0};
        ::strncpy(szDirectory, m_szLogFilePathName, MAX_LOG_FILE_PATH_NAME_LEN-1);
        char* pFind = ::strchr(szDirectory, '\\');
        if(NULL == pFind)
        {
            //异常，不再检测
            m_bDiskFull = false;
            return 0;
        }
        pFind[1] = '\0';
        ULARGE_INTEGER uliFreeBytesAvailableToCaller;
        ULARGE_INTEGER uliTotalNumberOfBytes;
        ULARGE_INTEGER uliTotalNumberOfFreeBytes;
        uliFreeBytesAvailableToCaller.QuadPart = 0;
        uliTotalNumberOfBytes.QuadPart = 0;
        uliTotalNumberOfFreeBytes.QuadPart = 0;
        BOOL bRes = GetDiskFreeSpaceEx(szDirectory, &uliFreeBytesAvailableToCaller,
            &uliTotalNumberOfBytes, &uliTotalNumberOfFreeBytes);
        if(!bRes)
        {
            //检测失败，不再检测
            m_bDiskFull = false;
            return 0;
        }

        if(uliFreeBytesAvailableToCaller.QuadPart < MIN_DISK_SPACE_FOR_LOG)
        {
            //磁盘空间还未达到恢复标准，记录本次检测时间，等待再次检测
            m_dwLastCheckDiskSpaceTime = dwTime;
            return 0;
        }

        //磁盘空间达到恢复标准，恢复日志输入，清除磁盘满标志
        m_dwLastCheckDiskSpaceTime = 0;
        m_bDiskFull = false;
        m_bAllowWrite = true;
        
        //如果文件未打开，则要重新打开日志文件，写入新日志信息
        if(NULL == m_pLogFile)
        {
            m_pLogFile = ::fopen(m_szLogFilePathName, "a+");
            if(NULL == m_pLogFile)
            {
                //仍然有问题，恢复错误标记，下次再尝试打开
                m_bDiskFull = true;
                m_bAllowWrite = false;
                return 0;
            }
            //通知写线程写日志
            (void)::SetEvent(m_hWriteEvent);
        }
    }

    //日志级别限制
    if(lLevel > m_lLevel)
    {
        return 0;
    }

//额外信息准备
    //日志时间
    SYSTEMTIME st;
    ::GetLocalTime(&st);        
    //日志级别
    const char* pszLevel = GetLevelString(lLevel);
    //文件名
    const char* pszFileName = ::strrchr(szFile, '\\');
    if(NULL != pszFileName)
    {
        //越过斜杠
        pszFileName += 1;
    }
    else
    {
        pszFileName = szFile;
    }

    //行号 即lLine
    //进程ID
    unsigned long ulProcessID = ::GetCurrentProcessId();
    //线程ID
    unsigned long ulThreadID = ::GetCurrentThreadId();
    //当前错误码
    unsigned long ulErr = GetLastError();
//额外信息准备完成

    //不能使用成员变量，多线程会出现问题
    char szLogTmp [MAX_LEN_OF_SINGLE_LOG] = {0};
    //首先向最终日志信息中加入额外信息
    (void)::sprintf(szLogTmp,"%04d-%02d-%02d %02d:%02d:%02d.%03d[%8s|%20s:%05d|PID:0x%04X|TID:0x%04X|Err:0x%04X]    ",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
        pszLevel, pszFileName, lLine, ulProcessID, ulThreadID, ulErr);//lint !e705

    //将用户信息接在额外信息后写进日志中
    unsigned long ulLen = ::strlen(szLogTmp);
    (void)::_vsnprintf(szLogTmp+ulLen, (MAX_LEN_OF_SINGLE_LOG-ulLen)-1, format, argp);
    szLogTmp[MAX_LEN_OF_SINGLE_LOG-1] = '\0';
    ulLen = ::strlen(szLogTmp);
    if((ulLen+2) < MAX_LEN_OF_SINGLE_LOG)
    {//自动增加一个换行
        szLogTmp[ulLen] = '\n';
        szLogTmp[ulLen+1] = '\0';
    }

    //将日志写进缓冲区
    unsigned long ulWriteLen = m_Cache.Write(szLogTmp, ::strlen(szLogTmp));
    while(0 == ulWriteLen)
    {
        //通知写线程写日志
        (void)::SetEvent(m_hWriteEvent);
        //等待数据写入缓冲
        Sleep(LOG_WAIT_FOR_WRITE_OVER_INTERVAL);
        ulWriteLen = m_Cache.Write(szLogTmp, ::strlen(szLogTmp));
    }

    //通知写线程写日志
    (void)::SetEvent(m_hWriteEvent);
    
    return 0;
}

//停止日志，终止写线程
long CIvsLog::Stop()
{
    //不接受写日志
    m_bAllowWrite = false;
    m_bDiskFull = false;
    m_dwLastCheckDiskSpaceTime = 0;

    //若已经停止或者未启动，直接退出
    if(!m_bRun)
    {
        return IVSLOG_ERROR_OK;
    }

    //等缓冲中数据都写到文件中(等待5秒)
    long lWaitTime = LOG_WAIT_FOR_EXIT_EVENT;
    while(lWaitTime >= 0)
    {
        if(0 == m_Cache.GetDataSize())
        {
            //缓冲中日志已经都写到文件中了
            break;
        }
        
        //触发写事件
        (void)::SetEvent(m_hWriteEvent);

        Sleep(LOG_WAIT_FOR_WRITE_OVER_INTERVAL);
        lWaitTime -= LOG_WAIT_FOR_WRITE_OVER_INTERVAL;
    }
    
    //设置日志标志为停止，触发写事件，让线程自己退出
    //等待5秒，若仍未退出，则强制中止
    m_bRun = false;    
    lWaitTime = LOG_WAIT_FOR_EXIT_EVENT;
    while(lWaitTime >= 0)
    {
        //触发写事件，让线程自己退出
        (void)::SetEvent(m_hWriteEvent);

        if(WAIT_TIMEOUT != ::WaitForSingleObject(m_hThreadExitEvent, LOG_WAIT_FOR_EXIT_EVENT_INTERVAL))
        {
            //线程结束
            //begin delete for AQ1D01618 by xuxin
            //m_hThreadExitEvent = NULL; //下面有CloseHandle清理，这里不能置空
            //end delete for AQ1D01618 by xuxin
            break;
        }
        
        lWaitTime -= LOG_WAIT_FOR_EXIT_EVENT_INTERVAL;
    }

    if(NULL != m_hWriteThread)
    {
        //强行中止写线程
        (void)::TerminateThread(m_hWriteThread, 0);
        (void)::CloseHandle(m_hWriteThread);
        m_hWriteThread = NULL;
    }

    //清理写事件
    if(NULL != m_hWriteEvent)
    {
        (void)::CloseHandle(m_hWriteEvent);
        m_hWriteEvent = NULL;
    }

    //清理写线程退出事件
    if(NULL != m_hThreadExitEvent)
    {
        (void)::CloseHandle(m_hThreadExitEvent);
        m_hThreadExitEvent = NULL;
    }
    
    //关闭文件
    if(NULL != m_pLogFile)
    {
        (void)::fclose(m_pLogFile);
        m_pLogFile = NULL;
    }

    //清空缓冲
    m_Cache.Clear();

    return IVSLOG_ERROR_OK;
}

//日志写线程入口
DWORD CALLBACK CIvsLog::ThreadEntry(LPVOID lpvoid)
{
    if(NULL != lpvoid)
    {
        //调用写线程函数体
        CIvsLog* pIvsLog = (CIvsLog *)lpvoid;
        pIvsLog->WriteLogFileThread();
    }

    return 0;
}
/*lint -e438 */
/*lint -e838*/
//写日志线程
void CIvsLog::WriteLogFileThread()
{
    //线程中变量准备
    unsigned long ulLogDataLen = 0;
    IvsLogThreadErr enRet;
    char* pLogInfo = NULL;
    __try
    {
        //申请读数据空间
        pLogInfo = new char[m_Cache.GetCacheSize()];
    }
    __except(1)
    {
    }
    if(NULL == pLogInfo)
    {
        //释放并赋空值
        (void)::CloseHandle(m_hWriteThread);
        m_hWriteThread = NULL;

        //线程退出事件通知
        (void)::SetEvent(m_hThreadExitEvent);

        return ;
    }

    //启动读缓冲循环
    while(m_bRun)
    {
        //等待写文件事件
        (void)::WaitForSingleObject(m_hWriteEvent, INFINITE);

        //磁盘满文件未打开
        if(m_bDiskFull || NULL == m_pLogFile)
        {
            //可能是磁盘满，等待
            if(m_bDiskFull)//lint !e774
            {
                continue;
            }
            else
            {   //不是因为磁盘满造成的，退出
                break;
            }
        }

        //读缓冲
        ulLogDataLen = m_Cache.GetCacheSize();
        ulLogDataLen= m_Cache.Read(pLogInfo, ulLogDataLen);
        if(0 == ulLogDataLen)
        {
            //缓冲为空未读到数据
            continue;
        }

        enRet = WriteFile(pLogInfo, ulLogDataLen);
        if (NEGLECTABLE_ERR == enRet)
        {
            continue;
        }
        else if (CRITICAL_ERR == enRet)
        {
            break;
        }

        if (!LogFileLenDeal())
        {
            break;
        }
    }

    //恢复状态
    m_bRun = false;
    m_bAllowWrite = false;
    m_bDiskFull = false;
    m_dwLastCheckDiskSpaceTime = 0;

    //释放并赋空值
    (void)::CloseHandle(m_hWriteThread);
    m_hWriteThread = NULL;

    //关闭当前日志文件
    if(NULL != m_pLogFile)
    {
        (void)::fclose(m_pLogFile);
        m_pLogFile = NULL;
    }

    //释放临时空间
    __try
    {
        delete[] pLogInfo;
    }
    __except(1)
    {
    }
    pLogInfo = NULL;

    //线程退出事件通知
    (void)::SetEvent(m_hThreadExitEvent);
}
/*lint +e838*/

//检查和处理日志文件长度超过限制
bool CIvsLog::LogFileLenDeal()
{
    SYSTEMTIME sysTime;
    char szNewFileName[MAX_LOG_FILE_PATH_NAME_LEN] = {0};

    if(NULL == m_pLogFile)//PCLint
    {
        return true;
    }

    //检查文件长度是否超过限制
    unsigned long ulCurFileLen = (unsigned long)::ftell(m_pLogFile);
    if(ulCurFileLen < m_ulFileLenLimit)
    {
        return true;
    }

    if(::strlen(m_szLogFilePathName) >= (MAX_LOG_FILE_PATH_NAME_LEN-20))
    {
        //文件名太长，无法修改文件名为备份文件名
        return true;
    }

    //关闭当前日志文件
    (void)::fclose(m_pLogFile);
    m_pLogFile = NULL;

    //生成备份文件名
    ::GetLocalTime(&sysTime);

    char szTemp[MAX_LOG_FILE_PATH_NAME_LEN] = {0};
    memcpy(szTemp, m_szLogFilePathName, strlen(m_szLogFilePathName) - 4);
    (void)::sprintf(szNewFileName, "%s_%4d%02d%02d%02d%02d%02d%s",
        szTemp, sysTime.wYear, sysTime.wMonth,
        sysTime.wDay, sysTime.wHour, sysTime.wMinute,
        sysTime.wSecond, ".log");

    //修改当前日志文件为备份文件，修改文件名失败也没有关系，可以再打开它继续往该文件中写
    (void)::rename(m_szLogFilePathName, szNewFileName);

    //BEGIN R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志
    //删除过期日志
    DeleteBackLogFiles();
    //END   R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志

    //重新打开日志文件
    m_pLogFile = ::fopen(m_szLogFilePathName, "a+");
    if(NULL != m_pLogFile)
    {
        //文件打开正常
        return true;
    }

    //新日志文件打开失败
    if(ERROR_DISK_FULL != ::GetLastError())
    {
        //异常，停止日志
        return false;
    }

    //若是磁盘满则暂停日志，等待空间
    m_bAllowWrite = false;
    m_bDiskFull = true;

    return true;
}
/*lint -e429 */
//检查和处理日志文件长度超过限制
IvsLogThreadErr CIvsLog::WriteFile(char* pLogInfo, unsigned long ulLogDataLen)
{

    if(NULL == m_pLogFile)//PCLint
    {
        return NEGLECTABLE_ERR;
    }

    //写数据到文件
    if(1 != fwrite(pLogInfo, ulLogDataLen, 1, m_pLogFile))
    {
        //写数据到文件中出现异常，若是磁盘满则暂停日志，等待空间
        if(ERROR_DISK_FULL == ::GetLastError())
        {
            m_bAllowWrite = false;
            m_bDiskFull = true;
            return NEGLECTABLE_ERR;
        }
        else
        {//其它错误则退出写日志
            return CRITICAL_ERR;
        }
    }

    //将文件缓冲中数据写到硬盘
    if(fflush(m_pLogFile) != 0)
    {
        //写数据到硬盘中出现异常，若是磁盘满则暂停日志，等待空间
        if(ERROR_DISK_FULL == ::GetLastError())
        {
            m_bAllowWrite = false;
            m_bDiskFull = true;
            return NEGLECTABLE_ERR;
        }
        else
        {//其它错误则退出写日志
            return CRITICAL_ERR;
        }
    }

    return SUCCESS;
}//lint !e818

//获取日志级别字符串
const char* CIvsLog::GetLevelString(long lLevel) const
{
    const char* pstrLevel = NULL;
    switch(lLevel)
    {
        case LOG_EMERGENCY:
            pstrLevel = "EMERGENCY";
            break;
        case LOG_ALERT:
            pstrLevel = "ALERT";
            break;
        case LOG_CRITICAL:
            pstrLevel = "CRITICAL";
            break;
        case LOG_ERROR:
            pstrLevel = "ERROR";
            break;
        case LOG_WARNING:
            pstrLevel = "WARNING";
            break;
        case LOG_NOTICE:
            pstrLevel = "NOTICE";
            break;
        case LOG_INFO:
            pstrLevel = "INFO";
            break;
        case LOG_DEBUG:
            pstrLevel = "DEBUG";
            break;
        default:
            pstrLevel = "NOLEVEL";
            break;
    }

    return pstrLevel;
}
/*************************** End 日志对象实现 *********************************/


/********************** Begin 日志模块用户接口实现 ****************************/
//启动NVS日志模块
IVSLOG_API void IvsStartLog(void)
{
    //获取日志实例
    CIvsLog* pIvsLog = CIvsLog::GetIvsLogInstance();
    //获取日志实例
    (void)(pIvsLog->Start());
    //写启动信息
    (void)(pIvsLog->Write(__FILE__, __LINE__,
                LOG_INFO, "IVS Log Module Start!", NULL));
}

//写一条日志
IVSLOG_API void __IvsWriteLog(const char* szFileName, long lLine,
                      long lLevel, const char* format, va_list argp)
{
    //获取日志实例
    CIvsLog* pIvsLog = CIvsLog::GetIvsLogInstance();
    //写一条日志
    (void)(pIvsLog->Write(szFileName, lLine, lLevel, format, argp));
}

//停止IVS日志模块
IVSLOG_API void IvsStopLog(void)
{
    //获取日志实例
    CIvsLog* pIvsLog = CIvsLog::GetIvsLogInstance();
    //写停止信息
    (void)(pIvsLog->Write(__FILE__, __LINE__,
                 LOG_INFO, "IVS SDK Log Module Stop!\n\n\n\n", NULL));
    //停止日志
    (void)(pIvsLog->Stop());
    //删除日志对象
    CIvsLog::DeleteIvsLogInstance();
}

//设置日志级别
IVSLOG_API void IvsSetLogLevel(long lLevel)
{
    //获取日志实例
    CIvsLog* pIvsLog = CIvsLog::GetIvsLogInstance();
    //设置日志级别
    pIvsLog->SetLevel(lLevel);
}

//设置当前写的日志文件路径名(完整路径或相对路径)
IVSLOG_API bool IvsSetLogFilePathName(const char* szPathName)
{
    //获取日志实例
    CIvsLog* pIvsLog = CIvsLog::GetIvsLogInstance();
    //设置文件名
    bool bSetOk = pIvsLog->SetLogFilePathName(szPathName);
    return bSetOk;
}

//设置日志文件长度限制，超过此长度时生成新文件，单位KB(100K到100M之间,默认是10M)
IVSLOG_API void IvsSetLogFileLengthLimit(unsigned long ulLimitLengthKB)
{
    //获取日志实例
    CIvsLog* pIvsLog = CIvsLog::GetIvsLogInstance();
    //设置文件长度限制
    pIvsLog->SetFileLengthLimit(ulLimitLengthKB);
}
/************************ End 日志模块用户接口实现 ****************************/

//BEGIN R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志
//删除过期的日志文件
void CIvsLog::DeleteBackLogFiles() const
{
    if (strlen(m_szLogFilePathName) > 256)
    {
        return;
    }

    //获取日志所在目录
    IVSLOG::PATH_ARRAY szAbsolutePath = {0};

    memcpy(szAbsolutePath, m_szLogFilePathName, strlen(m_szLogFilePathName) - 4);
        
    //替换最后一个反斜杠
    char* pszFind = strrchr(szAbsolutePath, '\\');
    if (NULL == pszFind)
    {
        return;
    }
    //保留'\\'，去掉'\\'后面的字符串
    *(pszFind + 1) = '\0';    //替换为结束符


    //日志文件过滤器
    IVSLOG::PATH_ARRAY szLogFileNameFilter = {0};
    IVSLOG::PATH_ARRAY szTemp = {0};
    memcpy(szTemp, m_szLogFilePathName, strlen(m_szLogFilePathName) - 4);
    (void)_snprintf(szLogFileNameFilter, sizeof(szLogFileNameFilter), "%s*", szTemp);

    //日志文件列表
    list<string> LogFileList;
    
    WIN32_FIND_DATA stData = {0};
    BOOL bFind = TRUE;

    HANDLE hFind = FindFirstFile(szLogFileNameFilter, &stData);
    if (INVALID_HANDLE_VALUE != hFind)
    {
        bFind = TRUE;
        while (bFind)
        {
            //保存到日志文件列表
            LogFileList.push_back(stData.cFileName);

            bFind = FindNextFile(hFind, &stData);
        }
    }
    (void)FindClose(hFind);

    //日志文件个数
    unsigned long ulFileCount = LogFileList.size();

    //没超过指定数目，不需删除
    if (IVSLOG::LOG_FILE_COUNT_MAX > ulFileCount)
    {
        return;
    }

    //排序
    LogFileList.sort();

    //删除过期日志
    list<string>::iterator Iter = LogFileList.begin();
    for (; IVSLOG::LOG_FILE_COUNT_MAX <= ulFileCount && Iter != LogFileList.end(); ++Iter)
    {
        //日志文件路径
        IVSLOG::PATH_ARRAY szDelLogFileName = {0};
        (void)_snprintf(szDelLogFileName, sizeof(szDelLogFileName), "%s\\%s", 
            szAbsolutePath, Iter->c_str());

        //删除该过期日志
        (void)remove(szDelLogFileName);

        ulFileCount--;
    }
}
//END   R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志
