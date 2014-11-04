#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <list>
#include <fstream>

#ifdef _MSC_VER
# include <windows.h>
#else
# include <pthread.h>
# include <sys/time.h>
# include <syslog.h>   // for syslog
# include <dirent.h>
# include <string.h>
# include <limits.h>
# include <unistd.h>
#endif


#include "compat.h"
#include "ivs_io.h"
#include "ivs_log.h"

#ifdef LOG_HAS_CONFIG_FILE
#include "log_conf.h"
#endif

#ifdef  LOG_HAS_STACK_TRACE
#include "ivs_btrace.h"
#endif

#if defined _MSC_VER && (_MSC_VER >= 1500)
#pragma warning (disable : 4996)
#endif

#ifdef WIN32
	#include <Dbghelp.h>   // MakeSureDirectoryPathExists
	#pragma comment (lib, "Dbghelp.lib")
#else
	#include "unistd.h" 
	#include <stdarg.h>
	#include <sys/stat.h>  
	#define ACCESS access 
#endif

/************************************************************************/
/* OS平台相关函数定义                                                   */
/************************************************************************/

#ifdef _MSC_VER
#  define lock_t          CRITICAL_SECTION
#  define LOCK_INIT(p)    InitializeCriticalSection((p))
#  define LOCK_DESTROY(p) DeleteCriticalSection((p))
#  define LOCK(p)         EnterCriticalSection((p))
#  define UNLOCK(p)       LeaveCriticalSection((p))
#  define THREAD_ID       GetCurrentThreadId()
#  define SLEEP(ms)       Sleep(ms)
#  define THREAD_HANDLE   HANDLE
#else
#  define lock_t          pthread_spinlock_t
#  define LOCK_INIT(p)    pthread_spin_init((p), 0)
#  define LOCK_DESTROY(p) pthread_spin_destroy((p))
#  define LOCK(p)         pthread_spin_lock((p))
#  define UNLOCK(p)       pthread_spin_unlock((p))
#  define THREAD_ID       pthread_self()
#  define SLEEP(ms)       usleep(ms * 1000)
#  define THREAD_HANDLE   pthread_t
#endif // #ifdef _MSC_VER

/* 路径分隔符 */
#ifdef _WIN32 
# define DIR_SEP '\\'
#else
# define DIR_SEP '/'
#endif

/************************************************************************/
/* LOG_FILE 结构体定义                                                  */
/************************************************************************/
#define FILE_BUFFER_SIZE   (16 * 1024)     /* 写缓存大小 默认16k */
#define MAX_PATH_LEN       256             /* 最大文件路径深度  */
#define MAX_FNAME_LEN      512
#define LOG_LINE_LENGTH    2048            /* 日志行长度 */

#if (FILE_BUFFER_SIZE <= LOG_LINE_LENGTH)
# error "Wrong Definition of FILE_BUFFER_SIZE and LOG_LINE_LENGTH"
#endif

#ifdef LOG_HAS_WRITE_THREAD
# define BUFFER_RING_COUNT     256        /* 4M */
# define WRITE_THREAD_SLEEPMS  100        /* 写磁盘线程poll间隔 */

typedef struct 
{
    unsigned int size;
    char buf[FILE_BUFFER_SIZE];
} BUF_CHUNK;

#endif

typedef struct LogFile
{
    int           nFd;                      // posix 文件句柄
    int           nLogType;                 // 日志文件类型
    char          FileName[MAX_FNAME_LEN];  // 日志文件全路径名称

    char          FinishedFiles[FILE_PRESERVE_COUNT][MAX_FNAME_LEN];
    unsigned int  uFirstIdx;
    unsigned int  uFinished;

    lock_t        lockWrite;              // 文件写锁
    unsigned long ulSize;                 // 当前文件大小
    unsigned long ulDrop;                 // 因缓存不足或写磁盘错误丢失的字节数
    unsigned int  uPos;                   // 当前写缓存位置

#ifndef LOG_HAS_WRITE_THREAD
    char          Buffer[FILE_BUFFER_SIZE];
#else
    /* 环形队列 */
    unsigned int  uIdxInput;                   
    unsigned int  uIdxOutput;                  
    BUF_CHUNK     Ring[BUFFER_RING_COUNT]; 
#endif
} LOG_FILE;

typedef struct LogFileName
{
    unsigned int uiTime;                      // 文件修改时间
    char szFileName[MAX_FNAME_LEN];           // 文件名称

    LogFileName(unsigned int uiTime, char* pFileName)
    {
        this->uiTime = uiTime;
        memset(szFileName, 0, MAX_FNAME_LEN);
        memcpy(szFileName,pFileName,strlen(pFileName));
    };

} LOG_FILENAME;

// 按修改时间升序排列
bool compare_filename (const LOG_FILENAME& first, const LOG_FILENAME& second)
{
    if (first.uiTime < second.uiTime)
        return true;
    else 
        return false;
}

/************************************************************************/
/* 全局变量定义                                                         */
/************************************************************************/

/* 异常回调函数指针 */
void (*g_ExceptionCB)(int code) = NULL; 

/* 模块名称与ID */
static char g_FileName[32];
static char g_ModuleType[32];
static int  g_ModuleCode;

/* 时区偏移设置 默认 +8 */
static unsigned int g_TzHour = 8; 
static unsigned int g_TzMin  = 0;
static char         g_TzFlag = '+';

static int  g_bInitOk    = 0;
static int  g_bEnableDbg = 0;
static int  g_LogLevel   = IVS_LOG_INFO;
static char g_LogRootDir[MAX_FNAME_LEN];

static unsigned int g_MaxFileSize  = FILE_MAX_SIZE_DEF;
static unsigned int g_MaxFileCount = FILE_PRESERVE_COUNT;

#ifdef LOG_HAS_SFTP_UPLOAD
/* sftp 上传队列链表节点 */
typedef struct FileNode
{
    struct FileNode *pNext;
    int             bIsZip;
    int             nFails;
    char            Name[MAX_FNAME_LEN];
} FNAME_NODE;

/* sftp 上传队列链表 */
static lock_t        g_lockSftp;
static FNAME_NODE   *g_SftpFilesHead;
static unsigned int  g_SftpListSize;

/* sftp 服务器配置 */
static char           g_SftpUser[64];     /* sftp用户名     */
static char           g_SftpPass[64];     /* sftp密码       */
static char           g_SftpPath[512];    /* sftp服务器路径 */
static char           g_SftpSvrAddr[32];  /* sftp服务器地址 */
static unsigned short g_SftpPort;         /* sftp服务器端口 */

#endif  // end of LOG_HAS_SFTP_UPLOAD

#ifdef LOG_HAS_CONFIG_FILE
static char g_ConfFileName[MAX_FNAME_LEN];
#endif 

#ifdef LOG_HAS_WRITE_THREAD
static int     g_bWriteRun = 1;
THREAD_HANDLE  g_hWriteThread = NULL;
#endif 

#ifdef LOG_HAS_MONITOR_THREAD
static int     g_bMonitorRun = 1;
THREAD_HANDLE  g_hMonitorThread = NULL;
#define   MONITOR_THREAD_SLEEPMS   500    /* 监控线程轮询时间 500ms */
#endif 

/* 日志结构对象 */
LOG_FILE g_RunLog;
LOG_FILE g_DbgLog;

#ifdef LOG_HAS_USER
LOG_FILE g_UserLog;
#endif 

#ifdef LOG_HAS_ITF
LOG_FILE g_InterFaceLog;
#endif 

#ifdef LOG_HAS_SEC
LOG_FILE g_SecurityLog;
#endif 

#ifdef LOG_HAS_OPR
LOG_FILE g_OperationLog;
#endif 

const char* g_strLogType[] = {
    "security", 
    "operation",
    "run", 
    "interface",
    "user",
    "debug"
};

const char* g_strLogLevel[] = {
    "Emergency",
    "Alert    ",
    "Critical ",
    "Error    ",
    "Warning  ",
    "Notice   ",
    "Info     ",
    "Debug    "
};


/************************************************************************/
/*  系统日志接口                                                        */
/************************************************************************/
/*
 * SysLog
 * 日志模块自身异常时，写入系统日志
 */
#ifdef WIN32
#define LOG_MODULE_FAIL 0x01
static void SysLog(const char *fmt, ...)
{
    HANDLE h = RegisterEventSource(NULL, g_ModuleType);
    if (INVALID_HANDLE_VALUE == h)
    {
        printf("RegisterEventSource Fail\n");
        return;
    }
    
    char MsgBuf[1024];
    va_list args;
    va_start(args, fmt);
    int len = IVS_vsnprintf(MsgBuf, sizeof(MsgBuf) - 1, fmt, args);
    va_end(args);
    MsgBuf[len] = '\0';
    char *Msg = MsgBuf;
    ReportEvent(h, EVENTLOG_ERROR_TYPE, 0, LOG_MODULE_FAIL, 
                  NULL, 1, 0, (LPCSTR *)&Msg, NULL);//lint !e1773
    printf("%s\n", MsgBuf);
    DeregisterEventSource(h);
}
#else
static void SysLog(const char *fmt, ...)
{
    openlog(g_ModuleType, LOG_CONS | LOG_PID, 0);
    va_list ap;
    va_start(ap, fmt);
    vsyslog(LOG_ERR, fmt, ap);
    va_end(ap);
    closelog();
}
#endif 

/************************************************************************/
/* 配置文件读取接口                                                     */
/************************************************************************/
/*
 * ReadConfigure
 * 读取日志配置文件
 */
#ifdef LOG_HAS_CONFIG_FILE
static int ReadConfigure(const char* szConf)
{
    static time_t tmLastModify = 0;
    IVS_STATE  _st;
    if (IVS_stat(szConf, &_st))
    {
        printf("Log Error: Read Config %s Fail error: %d\n", szConf, errno);
        return 0;
    }

    if (_st.st_mtime != tmLastModify)
    {
        tmLastModify = _st.st_mtime;
        if (ReadConfig(szConf))
        {
            printf("Log Error: Some error in Configure file %s\n", szConf);
        }
    }
    return 0;
}
#endif

/************************************************************************/
/* SFTP 功能接口                                                        */
/************************************************************************/

#ifdef LOG_HAS_SFTP_UPLOAD
/*
 * SftpTransferInit
 * 初始化Sftp上传队列
 */
static int SftpTransferInit()
{
    LOCK_INIT(&g_lockSftp);
    g_SftpFilesHead = NULL;
    return 0;
}

/*
 * SftpTransferClean
 * 清除Sftp上传队列
 */
static void SftpTransferClean()
{
    FNAME_NODE *pNode, *pNext;
    LOCK(&g_lockSftp);
    pNode = g_SftpFilesHead;
    while (pNode)
    {
        pNext = pNode->pNext;
        free(pNode);   // 只删除节点，文件下次程序启动时重新上传
        pNode = pNext;
    }
    UNLOCK(&g_lockSftp);
    LOCK_DESTROY(&g_lockSftp);
}

/*
 * SftpInsertTransList
 * 文件szName插入Sftp上传链表中
 */
static int SftpInsertTransList(char *szName)
{
    FNAME_NODE *newNode = (FNAME_NODE *)malloc(sizeof(FNAME_NODE));
    if (NULL == newNode)
    {
        SysLog("Insert Sftp List Fail, Out of Memory Del File %s\n", szName);
        return -1;
    }
    else 
    {
        memset(newNode, 0, sizeof(FNAME_NODE));
        newNode->bIsZip = strstr(szName, ".zip")? 1 : 0;
        strcpy(newNode->Name, szName);
        LOCK(&g_lockSftp);
        FNAME_NODE **ppNode = &g_SftpFilesHead;
        while (*ppNode)
        {
            ppNode = &((*ppNode)->pNext);
        }
        *ppNode = newNode;
        g_SftpListSize++;
        UNLOCK(&g_lockSftp);
    }
    return 0;
}

/*
 * SftpTransfer
 * sftp上传文件
 */
static int SftpTransfer()
{
    FNAME_NODE *pNode = NULL;
    LOCK(&g_lockSftp);
    pNode = g_SftpFilesHead;
    if (NULL != pNode)
    {
        g_SftpFilesHead = pNode->pNext;
        pNode->pNext = NULL;
    }
    UNLOCK(&g_lockSftp);

    if (NULL == pNode)
    {
        return 0;
    }
  
    if (!pNode->bIsZip)
    {
        // TODO  压缩文件，删除旧文件


        pNode->bIsZip = 1;
    }

    /**********************************/
    int error = 0; // sftp upload 添加sftp上传函数 TODO
    /**********************************/
    if (error)
    {
        if (pNode->nFails++ > 3)
        {
            IVS_unlink(pNode->Name);
        }
        else
        {
            LOCK(&g_lockSftp);
            if (g_SftpFilesHead)
            {
                pNode->pNext = g_SftpFilesHead;
                g_SftpFilesHead = pNode;
            }
            else
            {
                g_SftpFilesHead = pNode;
            }
            UNLOCK(&g_lockSftp);
        }
        return -1;
    }
    printf("Unlink file: %s\n", pNode->Name);
    if (IVS_unlink(pNode->Name))
    {
        SysLog("Unlink File %s Fail,errno = %d", pNode->Name, errno);
    }
    free(pNode);
    return 0;
}
#endif // #ifdef LOG_HAS_SFTP_UPLOAD

/************************************************************************/
/* 日志文件操作                                                         */
/************************************************************************/
/*
 * ExtractFinishedFile
 * 移除超出数量上限的备份文件
 */
static void RemoveExtraFiles(LOG_FILE *pFile)
{
    if (pFile->uFinished + 1 < g_MaxFileCount)
    {
        return;
    }
    unsigned int toDel = static_cast<unsigned int>((pFile->uFinished + 1 - g_MaxFileCount) + 1);

    for (unsigned int i = 0; i < toDel; i++)
    {
        char *pName = pFile->FinishedFiles[pFile->uFirstIdx];
#ifdef LOG_HAS_SFTP_UPLOAD
        if (SftpInsertTransList(pName))
        {
            IVS_unlink(pName); // 内存分配失败，直接删除文件
        }
#else
        IVS_unlink(pName);
#endif 
        pFile->uFirstIdx++;
        if (pFile->uFirstIdx == FILE_PRESERVE_COUNT)
        {
            pFile->uFirstIdx = 0;
        }
    }
    pFile->uFinished -= toDel;
}

/*
 * HandleRenameFail
 * 文件重命名失败处理：
 * 1.创建新的文件
 * 2.将旧文件内容拷贝到新文件
 * 3.删除旧文件，删除失败则清理旧文件内容
 */
void HandleRenameFail(const char * pszOldFileName, const char * pszNewFileName)
{
    if (NULL == pszOldFileName || NULL == pszNewFileName)
    {
        return;
    }

    std::ifstream ifs(pszOldFileName, std::ios::binary);
    if (!ifs.is_open())
    {
        return;
    }

    std::ofstream ofs(pszNewFileName, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open())
    {
        return;
    }

    ofs << ifs.rdbuf();

    ofs.close();
    ifs.close();

    if (0 != IVS_unlink(pszOldFileName)) //lint !e534
    {
        std::ofstream trun(pszOldFileName, std::ios::binary | std::ios::trunc);
        trun.close();
    }
}

/*
 * BackupFinishedFile
 * 备份已完成的文件
 */
static void BackupFinishedFile(LOG_FILE *pFile, time_t _tt)
{
    struct tm _tm;
    (void)IVS_localtime(&_tm, &_tt);
    char NewFileName[MAX_FNAME_LEN] = {0};

    /* 备份格式 SMU1_operation_20121010101010 */
    (void)IVS_snprintf(NewFileName, MAX_FNAME_LEN - 1, "%s%c%s%c%s_%s_%04d%02d%02d%02d%02d%02d.log", 
                       g_LogRootDir, DIR_SEP, g_strLogType[pFile->nLogType], DIR_SEP, 
                       g_FileName, g_strLogType[pFile->nLogType],
                       _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);

    if (IVS_rename(pFile->FileName, NewFileName))
    {
        SysLog("Rename file Fail, error = %d,src filename = %s,dst filename = %s", errno, pFile->FileName, NewFileName);
        HandleRenameFail(pFile->FileName, NewFileName);
    }

    /* 备份旧文件 */ 
    RemoveExtraFiles(pFile);
    unsigned int uInsAt = (pFile->uFinished + pFile->uFirstIdx) % FILE_PRESERVE_COUNT;
	//modify by limingjie lWX175483: 2013-5-24
	//strcpy 改成 strncpy
    strncpy(pFile->FinishedFiles[uInsAt], NewFileName, MAX_FNAME_LEN-1);
    pFile->uFinished++;
}

/*
 * logFileOpen
 * 打开日志文件
 */
static int LogFileOpen(LOG_FILE *pFile)
{
	//////////////////////////////////////////////////////////////////////////
	//问题单 DTS2013022002725
	//根据规范只有文件大于10M时才备份
    ///* 若旧文件存在则将其备份 */
    if (!IVS_access(pFile->FileName, 0))
    {    
        IVS_STATE _st;
        IVS_stat(pFile->FileName, &_st);
		pFile->ulSize = static_cast<unsigned long>(_st.st_size);		//设置文件大小
		if (pFile->ulSize >= g_MaxFileSize)
		{
			BackupFinishedFile(pFile, _st.st_mtime); 
			pFile->ulSize = 0;
		}
    }
	//////////////////////////////////////////////////////////////////////////
    pFile->nFd = IVS_open(pFile->FileName, O_RDWR|O_CREAT|O_APPEND, S_IWRITE|S_IREAD);
    if (pFile->nFd < 0)
    {
		pFile->ulSize = 0;
        SysLog("Open File %s Fail, error = %d", pFile->FileName, errno);
        if (g_ExceptionCB)
        {
            g_ExceptionCB(LOG_ERR_OPENFAIL);
        }
        return -1;
    }
    return 0;
}

/*
 * LogFileClose
 * 关闭日志文件并备份
 */
static int LogFileClose(LOG_FILE *pFile)
{
    if (pFile->nFd < 0)
    {
        return 0;
    }

    IVS_close(pFile->nFd);
    pFile->nFd = -1;
    pFile->ulSize = 0;
    BackupFinishedFile(pFile, time(NULL));
    return 0;
}

/* 
 * FileWrite
 * 写文件底层IO封装
 */
static unsigned int FileWrite(int nFd, const char *buffer, unsigned int count)
{
    size_t toWrite = count;
    unsigned int hasWrote = 0;
    while (hasWrote < count)
    {
        int n = IVS_write(nFd, buffer + hasWrote, toWrite);
        if (n <= 0)
        {
            break;
        }
        toWrite -= n; //lint !e737 此时n值非负
        hasWrote += n; //lint !e737 此时n值非负
    }
	IVS_fsync(nFd);
	return hasWrote;
}

/*
 * LogFileWrite
 * 写入日志文件缓存
 */
#ifdef LOG_HAS_WRITE_THREAD
static size_t LogFileWrite(LOG_FILE *pFile, const char *data, size_t len)
{
    char  *pBuf = NULL;
    size_t uWrite = len;

    LOCK(&pFile->lockWrite);

    BUF_CHUNK *pChunk = &pFile->Ring[pFile->uIdxInput];
    pBuf = pChunk->buf;

    /*
     * 写入长度大于本节点剩余空间,
     * 则获取下一个buffer，保持本节点日志完整性 
     */
    if ((pFile->uPos + len) > FILE_BUFFER_SIZE)
    {
        unsigned int nextInput = pFile->uIdxInput + 1;
        if (nextInput == BUFFER_RING_COUNT)
        {
            nextInput = 0;
        }

        if (nextInput == pFile->uIdxOutput)
        {
            pFile->ulDrop += len;
            uWrite = 0;
            SysLog("Write File %s, Drop %u Bytes", pFile->FileName, len);
        }
        else
        {
            pFile->uIdxInput = nextInput;
            pChunk = &pFile->Ring[pFile->uIdxInput];
            pBuf = pChunk->buf;
            memcpy(pBuf, data, len);
            pChunk->size = pFile->uPos = len;
        }
    }
    else
    {
        memcpy(pBuf + pFile->uPos, data, len);
        pFile->uPos  += len;
        pChunk->size += len;
    }
    
    UNLOCK(&pFile->lockWrite);
    return uWrite;
}

/*
 * FlushRingBuffer
 * 刷新环形缓存数据到内核
 */
void FlushRingBuffer(LOG_FILE *pFile)
{
	unsigned int uIdxInput  = pFile->uIdxInput;
    unsigned int uIdxOutput = pFile->uIdxOutput;
    int bLocked = 0;
    
    if (pFile->nFd < 0 && LogFileOpen(pFile))
    {
        return;
    }

    /* 若环形队列为空, 则尝试刷新当前缓冲 */
    if (uIdxInput == uIdxOutput)
    {
        LOCK(&pFile->lockWrite);
        if (!pFile->uPos)
        {
            UNLOCK(&pFile->lockWrite);
            return;
        }
        /*
         * 在此过程中其他线程可能改变pFile->uIdxInput的值,
         * 重新刷新 uIdxInput
         */
        uIdxInput = pFile->uIdxInput;
        bLocked = 1;
    }

    while (uIdxInput != uIdxOutput)
    {
        BUF_CHUNK *pChunk = &pFile->Ring[uIdxOutput];
        unsigned int hasWrote = FileWrite(pFile->nFd, pChunk->buf, pChunk->size);
        if (hasWrote < pChunk->size)
        {
            int err = errno;
            SysLog("Write File %s error, errno = %d", pFile->FileName, err);
            if (ENOSPC == err && g_ExceptionCB)
            {
                g_ExceptionCB(LOG_ERR_DISKFULL);
            }
        }
    
        pFile->ulSize += hasWrote;
        pChunk->size = 0;

        uIdxOutput++;
        if (uIdxOutput == BUFFER_RING_COUNT)
        {
            uIdxOutput = 0;
        }

        if (pFile->ulSize >= g_MaxFileSize)
        {
            (void)LogFileClose(pFile);
            LogFileOpen(pFile);
        }
    }

    if (bLocked)
    {
        BUF_CHUNK *pChunk = &pFile->Ring[uIdxOutput];
        unsigned int hasWrote = FileWrite(pFile->nFd, pChunk->buf, pFile->uPos);
        if (hasWrote < pFile->uPos)
        {
            int err = errno;
            SysLog("Write File %s error, errno = %d", pFile->FileName, err);
            if (ENOSPC == err && g_ExceptionCB)
            {
                g_ExceptionCB(LOG_ERR_DISKFULL);
            }
            
        }
        pFile->ulSize += hasWrote;
        pChunk->size = 0;
        pFile->uPos = 0;

        UNLOCK(&pFile->lockWrite);
        if (pFile->ulSize >= g_MaxFileSize)
        {
            (void)LogFileClose(pFile);
            LogFileOpen(pFile);
        }
    }
    pFile->uIdxOutput = uIdxOutput;
}

/*
 * ClearRingBuffer
 * 刷新所有环形队列中的缓存
 */
static void ClearRingBuffer(LOG_FILE *pFile)
{
    FlushRingBuffer(pFile); /* 清理已满缓存 */
    FlushRingBuffer(pFile); /* 清理未满缓存 */
}

/*
 * WriteProc
 * 写磁盘轮询线程函数
 */
static long WriteProc(void *arg)
{
#if defined LOG_HAS_CONFIG_FILE
    unsigned int uConfTimer = 0 ;
#endif   
    
    while (g_bWriteRun)
    {
        SLEEP(WRITE_THREAD_SLEEPMS);
        FlushRingBuffer(&g_RunLog);
        FlushRingBuffer(&g_DbgLog);
#ifdef LOG_HAS_ITF
        FlushRingBuffer(&g_InterFaceLog);
#endif 
#ifdef LOG_HAS_SEC
        FlushRingBuffer(&g_SecurityLog);
#endif 

#ifdef LOG_HAS_USER
        FlushRingBuffer(&g_UserLog);
#endif 

#ifdef LOG_HAS_OPR
        FlushRingBuffer(&g_OperationLog);
#endif

#if defined LOG_HAS_CONFIG_FILE
        if ((uConfTimer += WRITE_THREAD_SLEEPMS) > 1000 /* 1s */)
        {
            (void)ReadConfigure(g_ConfFileName);
            uConfTimer = 0;
        }
#endif 
    }
    
    /*
     * 这里使用日志模块的其他线程均退出, 
     * 清除日志缓存中的残余数据 
     */
    ClearRingBuffer(&g_RunLog);
    ClearRingBuffer(&g_DbgLog);
#ifdef LOG_HAS_ITF
    ClearRingBuffer(&g_InterFaceLog);
#endif 
#ifdef LOG_HAS_SEC
    ClearRingBuffer(&g_SecurityLog);
#endif 

#ifdef LOG_HAS_USER
    ClearRingBuffer(&g_UserLog);
#endif 

#ifdef LOG_HAS_OPR
    ClearRingBuffer(&g_OperationLog);
#endif
    
    return 0;
}//lint !e818 arg 无须声明为const

/*
 *  LogManageProc
 */
# ifdef _MSC_VER
static DWORD WINAPI LogWriteProc(LPVOID lpParameter)
{
    return (DWORD)WriteProc(lpParameter);
}
# else
static void * LogWriteProc(void *lpParameter)
{
    return (void *)WriteProc(lpParameter);
}
# endif // _MSC_VER


#else // !LOG_HAS_WRITE_THREAD

static size_t LogFileWrite(LOG_FILE *pFile, char *data, size_t len)
{
    LOCK(&pFile->lockWrite);
    if (pFile->nFd < 0 && LogFileOpen(pFile))
    {
        UNLOCK(&pFile->lockWrite);
        return -1;
    }
    
    /*
     * 写入长度大于剩余空间,则刷新数据到内核
     */
    if ((pFile->uPos + len) > FILE_BUFFER_SIZE)
    {
        int hasWrote = FileWrite(pFile->nFd, pFile->Buffer, pFile->uPos);
        if ((unsigned int)hasWrote < pFile->uPos)
        {
            int err = errno;
            SysLog("Write File %s error, errno = %d", pFile->FileName, err);
            if (ENOSPC == err && g_ExceptionCB)
            {
                g_ExceptionCB(LOG_ERR_DISKFULL);
            }
        }
        pFile->ulSize += hasWrote;
        pFile->uPos = 0;
 
        if (pFile->ulSize >= g_MaxFileSize)
        {
            LogFileClose(pFile);
        }
    }

    memcpy(pFile->Buffer + pFile->uPos, data, len);
    pFile->uPos += len;
    UNLOCK(&pFile->lockWrite);
    return len;
}
#endif  // ifdef LOG_HAS_WRITE_THREAD


/*
 * LogFileFlush
 * 刷新缓存数据到内核统一接口
 */
static void LogFileFlush(LOG_FILE *pFile)
{
#ifdef LOG_HAS_WRITE_THREAD
    /* 写磁盘线程自动Flush 这里直接Sleep即可 */
    SLEEP(5);
#else
    LOCK(&pFile->lockWrite);
    if (pFile->uPos > 0 && pFile->nFd >= 0)
    {
        FileWrite(pFile->nFd, pFile->Buffer, pFile->uPos);
    }
    pFile->uPos = 0;
    pFile->ulSize += pFile->uPos;
    /* check size */
    if (pFile->ulSize >= g_MaxFileSize)
    {
        LogFileClose(pFile);
    }
    UNLOCK(&pFile->lockWrite);
#endif 
}//lint !e818 无须声明为const，否则配置为不用独立写线程时无效

#ifndef LOG_HAS_WRITE_THREAD
/*
 *  FlushLogAll
 *  刷新所有文件缓存
 */
static void FlushLogAll()
{
    LogFileFlush(&g_RunLog);
    LogFileFlush(&g_DbgLog);
#ifdef LOG_HAS_ITF
    LogFileFlush(&g_InterFaceLog);
#endif 
#ifdef LOG_HAS_SEC
    LogFileFlush(&g_SecurityLog);
#endif 

#ifdef LOG_HAS_USER
    LogFileFlush(&g_UserLog);
#endif 

#ifdef LOG_HAS_OPR
    LogFileFlush(&g_OperationLog);
#endif
}
#endif

#if defined LOG_HAS_MONITOR_THREAD
/*
 * MonitorProc 
 * 监控线程函数
 */
static long MonitorProc(void *arg)
{
#if defined LOG_HAS_CONFIG_FILE  && !defined LOG_HAS_WRITE_THREAD
    unsigned long uConfTimer = 0;
#endif 
    
    while (g_bMonitorRun)
    {
        SLEEP(MONITOR_THREAD_SLEEPMS);
#ifdef LOG_HAS_SFTP_UPLOAD
        SftpTransfer();
#endif 

/*
 * 若自有配置文件且未配置写磁盘线程, 
 * 则本线程需读取配置文件 
 */
#if defined LOG_HAS_CONFIG_FILE  && !defined LOG_HAS_WRITE_THREAD
        if (uConfTimer += MONITOR_THREAD_SLEEPMS > 1000 /* 1s */)
        {
            ReadConfigure(g_ConfFileName);
            uConfTimer = 0;
        }
#endif 

/*
 * 无写磁盘线程，本线程定时刷新日志文件缓存 
 */
#ifndef LOG_HAS_WRITE_THREAD
        FlushLogAll();
#endif 
    }
    return 0;
}

/*
 *  LogManageProc
 */
# ifdef _MSC_VER
static DWORD WINAPI LogMonitorProc(LPVOID lpParameter)
{
    return (DWORD)MonitorProc(lpParameter);
}
# else
static void * LogMonitorProc(void *lpParameter)
{
    return (void *)MonitorProc(lpParameter);
}
# endif // _MSC_VER

#endif  // defined LOG_HAS_MONITOR_THREAD

// 初始化环形队列
static void InitRingQueue( std::list<LOG_FILENAME> &aFileNameList, LOG_FILE *&pFile )
{
    int iBackupCount = 0;
    if (aFileNameList.size() > g_MaxFileCount -1 - pFile->uFinished)
    {
        iBackupCount = aFileNameList.size() - (g_MaxFileCount - 1 - pFile->uFinished);
    }
    // 删除超出最大文件数的文件
    std::list<LOG_FILENAME>::iterator iter = aFileNameList.begin();
    for (int iIndex = 0; iIndex < iBackupCount; ++iIndex, ++iter)
    {
#ifdef LOG_HAS_SFTP_UPLOAD
        if (SftpInsertTransList(iter->szFileName))
        {
            IVS_unlink(iter->szFileName);
        }
#else
        IVS_unlink(iter->szFileName); //lint !e534
#endif
    }
    // 剩余文件初始化环形队列
    while (aFileNameList.end() != iter)
    {
        unsigned int uiInsertIndex = 
            (pFile->uFinished + pFile->uFirstIdx) % FILE_PRESERVE_COUNT;
        strncpy(pFile->FinishedFiles[uiInsertIndex], iter->szFileName, MAX_FNAME_LEN);
        pFile->FinishedFiles[uiInsertIndex][MAX_FNAME_LEN - 1] = '\0';
        ++pFile->uFinished;
        ++iter;
    }
}

#ifdef _MSC_VER
/*
 * ScanDirectory
 * 初始化时检查当前目录下已存在日志
 */
static void ScanDirectory(char *szDir, LOG_FILE *&pFile)
{
    char szTempDir[MAX_PATH+1];
    (void)IVS_snprintf(szTempDir, MAX_PATH, "%s\\*", szDir);
    WIN32_FIND_DATA FileData;
    memset(&FileData, 0, sizeof(FileData));

    HANDLE hFindFile = FindFirstFile(szTempDir, &FileData);
    if (INVALID_HANDLE_VALUE == hFindFile)
    {
        return;
    }
    /************************************************************************
    * 初始化日志流程：
    * 1.遍历文件夹下已存在的日志文件，过滤倒数第五位不是数字的文件（日期+.log）；
    * 2.保存文件名和文件修改时间信息；
    * 3.对文件按修改时间升序排列；
    * 4.删除超出最大文件数的文件；
    * 5.用剩余文件初始化环形队列。
    ************************************************************************/
    std::list<LOG_FILENAME> aFileNameList;
    aFileNameList.clear();
    do
    {
        if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            continue;
        }
        else
        {
            char szName[MAX_PATH+1] = {0};
            (void)IVS_snprintf(szName, MAX_PATH, "%s\\%s", szDir, FileData.cFileName);
            IVS_STATE st;
            IVS_stat(szName, &st);

            // 找日志文件名中含有时间信息字段的文件, 考虑已完成的文件应为日期+“.log”
            if (szName[strlen(szName) - 5] <= '9' && szName[strlen(szName) - 5] >= '0')
            {
                LOG_FILENAME stLogFileName((unsigned int)st.st_mtime, szName);
                aFileNameList.push_back(stLogFileName);
            }
        }
    }
    while(FindNextFile(hFindFile,&FileData));
    FindClose(hFindFile);
    // 按修改时间降序排列
    aFileNameList.sort(compare_filename);
    // 初始化环形队列
    InitRingQueue(aFileNameList, pFile);
}

#endif // _MSC_VER

#if defined __GNUC__ || defined LINUX
//add by wangguangyan start
void  CreatLogDir(const char *pszDir)  
{ 
	printf("Enter CreatLogDir pszDir = %s\n",pszDir);
	char DirName[256] = {0};      
	strcpy(DirName,pszDir);      
	int len = strlen(DirName);      
	if(DirName[len-1]!='/')
	{
		strcat(DirName,"/");
	}
	len = strlen(DirName);      
	for(int i=1;i<len;i++)      
	{      
		if(DirName[i]=='/')      
		{      
			DirName[i] = 0;      
			if(access(DirName,NULL) != 0)      
			{      
				if(mkdir(DirName,0777) == -1)      
				{       
					perror("mkdir error");       
					return ;       
				}      
			}    
			DirName[i] = '/';      
		}      
	}        
	printf("Leave CreatLogDir \n");
}
//add by wangguangyan end
static void ScanDirectory(char *szDir, LOG_FILE *pFile)
{
	//add by wangguangyan start
	if(access(szDir,0) != 0)//判断是否存在日志目录
	{
		CreatLogDir(szDir);//新建日志目录
	}
	//add by wangguangyan end

    struct dirent *ent = NULL;   
    DIR  *pDir = opendir(szDir);
	if(NULL == pDir)
	{
		printf("opendir[%s]failed\n.",szDir);
		return;
	}
	printf("opendir[%s] success\n",szDir);


    std::list<LOG_FILENAME> aFileNameList;
    aFileNameList.clear();
    while ((ent = readdir(pDir))!= NULL) 
    {  
        if(ent->d_type & DT_DIR)
        {  
            continue;
        }  
        else
        {
            char szName[PATH_MAX+1] = {0};
            IVS_snprintf(szName, PATH_MAX, "%s/%s", szDir, ent->d_name);

            IVS_STATE st;
            IVS_stat(szName, &st);
            // 找日志文件名中含有时间信息字段的文件, 考虑已完成的文件应为日期+“.log”
            if (szName[strlen(szName) - 5] <= '9' && szName[strlen(szName) - 5] >= '0')
            {
                LOG_FILENAME stLogFileName((unsigned int)st.st_mtime, szName);
                aFileNameList.push_back(stLogFileName);
            }
        }  
    }
    closedir(pDir);
    // 按修改时间降序排列
    aFileNameList.sort(compare_filename);
    // 初始化环形队列
    InitRingQueue(aFileNameList, pFile);


}
#endif // defined __GNUC__ || defined LINUX

/*
 * LogFileInit
 * 初始化LOG_FILE结构
 */
static int LogFileInit(LOG_FILE *pFile, int nType)
{
    char subDir[MAX_FNAME_LEN] = {0};
    size_t pathLen = strlen(g_LogRootDir);
    memcpy(subDir, g_LogRootDir, pathLen);
    subDir[pathLen++] = DIR_SEP;

    /* 检查目录 */
	//modify by limingjie lWX175483 :2013-5-24
	//strcpy 改成 strncpy
    strncpy(subDir + pathLen, g_strLogType[nType], strlen(g_strLogType[nType])+1);
    pathLen = strlen(subDir);
    subDir[pathLen++] = DIR_SEP;
#ifdef WIN32
    if (0 > IVS_access(subDir, 0) && !MakeSureDirectoryPathExists(subDir)/*0 > IVS_mkdir(subDir)*/)
#else
	if (0 > IVS_access(subDir, 0) && access(subDir,R_OK|W_OK) == 0/*0 > IVS_mkdir(subDir)*/)
#endif
    { 
        printf("Log Error: Create Log Directory %s Fail\n", subDir);
        return -1;
    }
    memset(pFile, 0, sizeof(LOG_FILE));
    ScanDirectory(subDir, pFile); 
    pFile->nLogType = nType;
    pFile->nFd = -1;
    LOCK_INIT(&pFile->lockWrite);
    (void)IVS_snprintf(pFile->FileName, sizeof(pFile->FileName) - 1, 
             "%s%c%s_%s.log", subDir, DIR_SEP, g_FileName, g_strLogType[nType]);
    return 0;
}

/*
 * LogFileClean
 * 清理LOG_FILE结构
 */
static void LogFileClean(LOG_FILE *pFile)
{
    LogFileFlush(pFile);
    //LogFileClose(pFile);             //lint !e534 不考虑返回值
    // clean时正在写的文件不需要备份 modify by xiongfeng192614
    if (pFile->nFd >= 0)
    {
        IVS_close(pFile->nFd);
        pFile->nFd = -1;
        pFile->ulSize = 0;
    }
    // modify end
	LOCK_DESTROY(&pFile->lockWrite);
}

/************************************************************************/
/* 外部接口                                                             */
/************************************************************************/
/*
 * IVS_logInit
 * @szDir:        日志保存目录, 必须已存在
 * @szConf:       配置文件路径, 使能配置文件则必需
 * @szModuleType: 模块类型
 * @uModuleID:    模块ID
 * @callback:     异常回调函数
 */
int IVS_LogInit(const char *szDir, const char *szConf, const char* szFileName, const char *szModuleType, 
                int nModuleID, void (*callback)(int code))
{
#ifdef LOG_HAS_CONFIG_FILE 
    if (NULL == szDir || NULL == szFileName || NULL == szConf || NULL == szModuleType)
#else
    if (NULL == szDir || NULL == szFileName || NULL == szModuleType)
#endif 
    {
        printf("Log Error: Log Init Fail, Invalid parameter\n");
        return -1;
    }

#ifdef LOG_HAS_STACK_TRACE
    if (IVS_BackTraceInit())
    {
        printf("Log Error: Init BackTrace Fail\n");
        return -1;
    }
#endif

#ifdef LOG_HAS_SFTP_UPLOAD
    SftpTransferInit();
#endif 

#ifdef LOG_HAS_CONFIG_FILE
    memset(g_ConfFileName, 0, sizeof(g_ConfFileName));
    strncpy(g_ConfFileName, szConf, sizeof(g_ConfFileName) - 1);
    if (ReadConfigure(g_ConfFileName))
    {
        printf("Log Error: Read Configure file %s Fail Use Default Configure\n", g_ConfFileName);
    }
#endif
    memset(g_FileName, 0, sizeof(g_FileName));
    strncpy(g_FileName,  szFileName, sizeof(g_FileName) - 1);

    memset(g_ModuleType, 0, sizeof(g_ModuleType));
    strncpy(g_ModuleType,  szModuleType, sizeof(g_ModuleType) - 1);
    g_ModuleCode = nModuleID;

    /* 检查并创建日志根目录 */
    int pathLen = IVS_snprintf(g_LogRootDir, sizeof(g_LogRootDir) - 1,
             "%s%c%s", szDir, DIR_SEP, szModuleType);
    g_LogRootDir[pathLen++] = DIR_SEP;
    g_LogRootDir[pathLen] = 0;
    if (pathLen >= MAX_PATH_LEN)
    {
        printf("Log Error: Out of Mem, Log Path too deep\n");
        return -1;
    }
#ifdef WIN32
    if (0 > IVS_access(g_LogRootDir, 0) && !MakeSureDirectoryPathExists(g_LogRootDir)/*0 > IVS_mkdir(g_LogRootDir)*/)
#else
	if (0 > IVS_access(g_LogRootDir, 0) && access(g_LogRootDir,R_OK|W_OK) == 0/*0 > IVS_mkdir(subDir)*/)
#endif
    {
        printf("Log Error: Create Log Root Directory Fail\n");
        return -1;
    }

    int error = 0;
    error += LogFileInit(&g_RunLog, TYPE_RUN);
    error += LogFileInit(&g_DbgLog, TYPE_DBG);

#ifdef LOG_HAS_ITF
    error += LogFileInit(&g_InterFaceLog, TYPE_ITF);
#endif 

#ifdef LOG_HAS_SEC
    error += LogFileInit(&g_SecurityLog, TYPE_SEC);
#endif 

#ifdef LOG_HAS_USER
    error += LogFileInit(&g_UserLog, TYPE_USR);
#endif 

#ifdef LOG_HAS_OPR
    error += LogFileInit(&g_OperationLog, TYPE_OPR);
#endif

    if (error)
    {
        printf("Init Log File error\n");
        return -1;
    }

#ifdef LOG_HAS_WRITE_THREAD
# ifdef _MSC_VER
    g_hWriteThread = CreateThread(NULL, 0, LogWriteProc, NULL, 0, NULL);
    if (NULL == g_hWriteThread)
    {
        printf("Create Log Write Thread Fail\n");
        return -1;
    }
# else
    if (pthread_create(&g_hWriteThread, NULL, LogWriteProc, NULL))
    {
        printf("Create Log Write Thread Fail\n");
        return -1;
    }
# endif // _MSC_VER
#endif // ifdef  LOG_HAS_WRITE_THREAD

#ifdef LOG_HAS_MONITOR_THREAD
# ifdef _MSC_VER
    g_hMonitorThread = CreateThread(NULL, 0, LogMonitorProc, NULL, 0, NULL);
    if (NULL == g_hMonitorThread)
    {
        printf("Create Log Monitor Thread Fail\n");
        return -1;
    }
# else
    if (pthread_create(&g_hMonitorThread, NULL, LogMonitorProc, NULL))
    {
        printf("Create Log Write Thread Fail\n");
        return -1;
    }
# endif  // ifdef _MSC_VER 
#endif  // ifdef LOG_HAS_MONITOR_THREAD
    
    g_ExceptionCB = callback;
    g_bInitOk = 1;
    return 0;
}

/*
 * IVS_LogClean
 */
void IVS_LogClean()
{
#ifdef LOG_HAS_WRITE_THREAD
    g_bWriteRun = 0;
#ifdef _MSC_VER
    WaitForSingleObject(g_hWriteThread, INFINITE); 
#else
    pthread_join(g_hWriteThread, NULL);
#endif 
    g_hWriteThread = NULL;
    g_bWriteRun = 1;
#endif 

#ifdef LOG_HAS_MONITOR_THREAD
    g_bMonitorRun = 0;
# ifdef _MSC_VER
    WaitForSingleObject(g_hMonitorThread, INFINITE); 
# else
    pthread_join(g_hMonitorThread, NULL);
# endif 
    g_hMonitorThread = NULL;
    g_bMonitorRun = 1;
#endif 

#ifdef LOG_HAS_SFTP_UPLOAD
    SftpTransferClean();
#endif 

    LogFileClean(&g_RunLog);
    LogFileClean(&g_DbgLog);
#ifdef LOG_HAS_ITF
    LogFileClean(&g_InterFaceLog);
#endif 

#ifdef LOG_HAS_SEC
    LogFileClean(&g_SecurityLog);
#endif 

#ifdef LOG_HAS_USER
    LogFileClean(&g_UserLog);
#endif 

#ifdef LOG_HAS_OPR
    LogFileClean(&g_OperationLog);
#endif  

#ifdef LOG_HAS_STACK_TRACE
    IVS_BackTraceClean();
#endif
}

/*
 * IVS_LogSetSftp
 * 设置sftp参数
 */
#ifdef LOG_HAS_SFTP_UPLOAD
int IVS_LogSetSftp(char *szSvrDir, char* szUser, char *szPassWord,
                char *szSvrAddr, unsigned short usSvrPort)
{
    if (NULL == szSvrDir || NULL == szUser 
        || NULL == szPassWord || NULL == szSvrAddr)
    {
        return  -1;
    }

    strncpy(g_SftpPath, szSvrDir, sizeof(g_SftpPath) - 1);
    strncpy(g_SftpUser, szUser, sizeof(g_SftpUser) - 1);
    strncpy(g_SftpPass, szPassWord, sizeof(g_SftpPass) - 1);
    strncpy(g_SftpSvrAddr, szSvrAddr, sizeof(g_SftpSvrAddr) - 1);
    g_SftpPort = usSvrPort;
   
    return 0;
}
#endif


/*
 * IVS_LogSetLevel
 * 设置日志级别
 */
int IVS_LogSetLevel(int level)
{
    if (g_LogLevel < IVS_LOG_EMERG || g_LogLevel > IVS_LOG_DEBUG)
    {
        return -1;
    }
    g_LogLevel = level;
    return 0;
}

/*
 * IVS_LogSetDebug
 * 使能调试日志功能
 */
int IVS_LogSetDebug(int enable)
{
    g_bEnableDbg = enable;
    return 0;
}


/*
 * IVS_LogSetFileSize
 * 设置文件上限，最小10M
 */
int IVS_LogSetFileSize(unsigned long s)
{
    g_MaxFileSize = s < FILE_MAX_SIZE_DEF ? FILE_MAX_SIZE_DEF : s;
    return 0;
}

/*
 * IVS_LogFileCount
 * 设置保留文件数量
 */
int IVS_LogSetFileCount(unsigned int c)
{
    g_MaxFileCount = (c > FILE_PRESERVE_COUNT) ? FILE_PRESERVE_COUNT : c;
    return 0;
}

/*
 *  IVS_LogSetTimeZone
 *  设置时区
 */
void IVS_LogSetTimeZone(unsigned int hour, unsigned int min, int flag)
{
    g_TzFlag = (flag == UTC_FLG_PLUS)? '+' : '-';
    g_TzHour = hour;
    g_TzMin  = min;
}

/************************************************************************/
/* 写日志接口                                                           */
/************************************************************************/

/*
 *  FillTime
 *  填充日期时间
 */
static unsigned int FillTime(char *buf, unsigned int len)
{
    time_t _tt = time(NULL);
    struct tm _tm;
    IVS_localtime(&_tm, &_tt); //lint !e534 不考虑返回值

    struct timeval tv;
    IVS_gettimeofday(&tv);    //lint !e534 不考虑返回值
   
    /* 时间格式：2005-07-08 21:06:09.010+05:30 */
    return IVS_snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d.%03d%c%02d:%02d", 
        _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday,
        _tm.tm_hour, _tm.tm_min, _tm.tm_sec, tv.tv_usec/1000, g_TzFlag, g_TzHour, g_TzMin);//lint !e732
}

/**************************************************************************
* name       : PacketLog
* description: 组装日志
* input      : 
* output     : NA
* return     : void
* remark     :               
 **************************************************************************/
static unsigned int PacketLog(char * szLogTmp, const char * stcMsgText, const char* szFile, long lLine, long lLevel)
{

    unsigned int pos = 0;
    //时间时区
    pos += FillTime(szLogTmp, LOG_LINE_LENGTH);
    //日志级别
    const char* pszLevel = g_strLogLevel[lLevel];

    //文件名
    const char* pszFileName = NULL;
#ifdef _MSC_VER
    pszFileName = ::strrchr(szFile, '\\');
#else
    pszFileName = ::strrchr(szFile, '/');
#endif

    if(NULL != pszFileName)
    {
        //越过斜杠
        pszFileName += 1;
    }
    else
    {
        pszFileName = szFile;
    }

    //线程ID 
    unsigned long ulThreadID = THREAD_ID;

    //当前错误码
    int ulErr = 0;
#ifdef _MSC_VER
    ulErr = (int)::GetLastError();
#else
    ulErr = errno;
#endif
    //额外信息准备完成

    //格式化日志
    pos += IVS_snprintf(szLogTmp + pos, LOG_LINE_LENGTH - pos, "[%7s|TID:0x%04X|RtnCode:0x%04X]    %s:%05d %s ", 
        pszLevel, (unsigned int)ulThreadID, (unsigned int)ulErr,pszFileName, (int)lLine,stcMsgText);

    return pos;
}

/*
 * IVS_LogRun
 * 记录接口日志
 * @level:     日志级别
 * @stcMsg:    静态信息
 * @file:      源文件名
 * @line:      源文件行
 * @func:      函数名
 * @fmt:       格式串
 * 日期时间|级别|线程号|日志静态信息|日志附加信息
 */
void IVS_LogRun(int level, const char* stcMsgText, const char *file, int line,
               const char *func, const char *fmt, ...)
{
    char buf[LOG_LINE_LENGTH + 1]; // no need init
    if (!g_bInitOk || (level > g_LogLevel && !g_bEnableDbg))
    {
        return;  // 若日志级别低或Debug功能未打开，则直接返回
    }
    unsigned int pos = FillTime(buf, LOG_LINE_LENGTH);
    pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|%s|0x%08x|%s|",
            g_strLogLevel[level], (unsigned long)THREAD_ID, stcMsgText);   //lint !e737 调用者保证level非负

    va_list  ap;
    va_start(ap, fmt);
    pos += IVS_vsnprintf(buf + pos, LOG_LINE_LENGTH - pos, fmt, ap); //lint !e737 IVS_vsnprintf 返回值非负
    va_end(ap);

    buf[pos++] = '\n';
 
#ifdef LOG_HAS_STACK_TRACE
    if (level <= IVS_LOG_CRIT && pos < LOG_LINE_LENGTH)
    {
        pos += IVS_DumpBackTrace(buf + pos, LOG_LINE_LENGTH - pos);
    }
#endif

    if (level <= g_LogLevel)
    {
        LogFileWrite(&g_RunLog, buf, pos); //lint !e534 不测试返回值
    }
    
    if (g_bEnableDbg)
    {
        pos = PacketLog(buf, stcMsgText, file, line, level);
        va_list  apDbg;
        va_start(apDbg, fmt);
        pos += IVS_vsnprintf(buf + pos, LOG_LINE_LENGTH - pos, fmt, apDbg); //lint !e737 IVS_snprintf返回非负
        va_end(apDbg);
        buf[pos++] = '\n';
        //pos--;  // earse '\n'     
        //pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|0x%08x|%s:%u|%s\n",
        //     (unsigned long)THREAD_ID, file, line, func);  //lint !e737 IVS_snprintf返回非负
        LogFileWrite(&g_DbgLog, buf, pos); //lint !e534 不测试返回值
    }

    if (level <= IVS_LOG_CRIT)
    {
        /* 日志级别高，立刻刷新到内核 */    
        LogFileFlush(&g_RunLog);
    }
    return;
}

/*
 * IVS_LogDbg
 * 记录调试日志
 * @level:     日志级别
 * @file:      源文件名
 * @line:      源文件行
 * @func:      函数名
 * @fmt:       格式串
 */
void IVS_LogDbg(const char *file, int line, const char *func, const char *fmt, ...)
{
    if (!g_bInitOk || !g_bEnableDbg)
    {
        return ;
    }

    char buf[LOG_LINE_LENGTH + 1]; 
    unsigned int pos = 0;
    //pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|%s|", g_strLogLevel[IVS_LOG_DEBUG]); //lint !e737 IVS_snprintf返回非负

    pos = PacketLog(buf, "", file, line, IVS_LOG_DEBUG);
    va_list  ap;
    va_start(ap, fmt);
    pos += IVS_vsnprintf(buf + pos, LOG_LINE_LENGTH - pos, fmt, ap); //lint !e737 IVS_snprintf返回非负
    va_end(ap);
    buf[pos++] = '\n';

    //pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|0x%08x|%s:%u|%s\n", 
    //         (unsigned long)THREAD_ID, file, line, func); //lint !e737 IVS_snprintf返回非负
    LogFileWrite(&g_DbgLog, buf, pos); //lint !e534 不测试返回值
    return ;
}


/*
 * IVS_LogInterface
 * 记录接口日志
 * @level:       日志级别
 * @inftype:     接口类型
 * @infname:     接口名称
 * @sender:      发起方模块代码
 * @receiver:    接收方模块代码
 * @traceMark:   跟踪标识
 * @operation:   操作内容
 * @result:      操作结果
 * @file:        源文件名
 * @line:        源文件行
 * @func:        函数名
 * @fmt:         格式串
 * 日期时间|接口类型| 接口名称 |日志级别 | 发起方模块代码 | 接收方模块代码|跟踪标识|操作内容 | 操作结果 |日志附加信息
 */
#ifdef LOG_HAS_ITF
void IVS_LogInterface(int level, char *inftype, char *infname, 
              char *sender, char *receiver, char *traceMark, char *operation, char *result, 
              const char *file, int line, const char *func, const char *fmt, ...)
{
    char buf[LOG_LINE_LENGTH + 1]; 
    if (!g_bInitOk || (level > g_LogLevel && !g_bEnableDbg))
    {
        return;
    }

    int pos = FillTime(buf, LOG_LINE_LENGTH);
    pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|%s|%s|%s|%s|%s|%s|%s|%s|",
              inftype, infname, g_strLogLevel[level], sender, receiver, traceMark, operation, result);
    va_list ap;
    va_start(ap, fmt);
    pos += IVS_vsnprintf(buf + pos, LOG_LINE_LENGTH - pos, fmt, ap);
    va_end(ap);
    buf[pos++] = '\n';

    if (level <= g_LogLevel)
    {
        LogFileWrite(&g_InterFaceLog, buf, pos);
    }

    if (g_bEnableDbg)
    {
        pos--;
        pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|0x%08x|%s:%u|%s\n", 
            (unsigned long)THREAD_ID, file, line, func);
        LogFileWrite(&g_DbgLog, buf, pos);
    }
    return ;
}
#endif

/*
 * IVS_LogUser
 * 记录用户日志
 * @level:     日志级别
 * @user:      用户ID
 * @operation: 操作
 * @result:    操作结果
 * @file:      源文件名
 * @line:      源文件行
 * @func:      函数名
 * @fmt:       格式串
 */
#ifdef LOG_HAS_USER
void IVS_LogUser(int level, char *user, char *operation, char *result, 
           const char *file, int line, const char *func, const char *fmt, ...)
{
    return ;
}
#endif

/*
 * IVS_LogSecurity
 * 记录安全日志
 * @level:      日志级别(DB)
 * @user:       用户账号(DB)
 * @ip:         客户端IP(DB)
 * @stcMsgNum： 静态信息编码(DB)
 * @stcMsgText: 静态信息文本
 * @opcode:     操作类型(DB)
 * @result:     操作结果(DB)
 * @file:       源文件名
 * @line:       源文件行
 * @func:       函数名
 * @fmt:        格式串
 * 日期时间 | 日志级别 | 用户账号 |客户端IP |静态信息| 操作结果 |日志附加信息
 */
#ifdef LOG_HAS_SEC
void IVS_LogSecurity(int level, char *user, char *ip, char *stcMsgNum, char*stcMsgText, 
           char*opcode, char *result, const char *file, int line, const char *func, const char *fmt, ...)
{
    char buf[LOG_LINE_LENGTH + 1];  // no need init
    if (!g_bInitOk || (level > g_LogLevel && !g_bEnableDbg))
    {
        return;
    }

    int pos = FillTime(buf, LOG_LINE_LENGTH);
    pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|%s|%s|%s|%s|%s|%s|",
        g_strLogLevel[level], user, ip, stcMsgNum, stcMsgText, result);

    va_list ap;
    va_start(ap, fmt);
    pos += IVS_vsnprintf(buf + pos, LOG_LINE_LENGTH - pos, fmt, ap);
    va_end(ap);
    buf[pos++] = '\n';

    if (level <= g_LogLevel)
    {
        LogFileWrite(&g_UserLog, buf, pos);
    }

    if (g_bEnableDbg)
    {
        pos--;  
        pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|0x%08x|%s:%u|%s\n", 
            (unsigned long)THREAD_ID, file, line, func);
        LogFileWrite(&g_DbgLog, buf, pos);
    }

    /****/
     // TODO 数据库导入代码

    /****/


    return;
}
#endif 

/*
 * IVS_LogOperation
 * 记录操作日志
 * @level:      日志级别 (DB)
 * @user:       用户账号 (DB)
 * @ip:         客户端IP (DB)
 * @object      操作对象  
 * @stcMsgNum： 静态信息编码 (DB)
 * @stcMsgText: 静态信息文本
 * @opcode:     操作代码 (DB)
 * @result:     操作结果 (DB)  successfully/failed   
 * @file:       源文件名 
 * @line:       源文件行
 * @func:       函数名
 * @fmt:        格式串
 * 格式：日期时间 | 日志级别 | 用户账号 | 客户端IP | 操作对象 |静态信息 | 操作结果 |日志附加信息
 */
#ifdef LOG_HAS_OPR
void IVS_LogOperation(int level, char *user, char* ip, char *object, 
                     char *stcMsgNum, char *stcMsgText, char *opcode, char *result, 
                     const char *file, int line, const char *func, const char *fmt, ...)
{
    char buf[LOG_LINE_LENGTH + 1];
    if (!g_bInitOk || (level > g_LogLevel && !g_bEnableDbg))
    {
        return;
    }

    int pos = FillTime(buf, LOG_LINE_LENGTH);
    pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|%s|%s|%s|%s|%s|%s|%s|",
          g_strLogLevel[level], user, ip, object, stcMsgNum, stcMsgText, result);

    va_list ap;
    va_start(ap, fmt);
    pos += IVS_vsnprintf(buf + pos, LOG_LINE_LENGTH - pos, fmt, ap);
    va_end(ap);
    buf[pos++] = '\n';

    if (level <= g_LogLevel)
    {
        LogFileWrite(&g_OperationLog, buf, pos);
    }

    if (g_bEnableDbg)
    {
        pos--;
        pos += IVS_snprintf(buf + pos, LOG_LINE_LENGTH - pos, "|0x%08x|%s:%u|%s\n", 
            (unsigned long)THREAD_ID, file, line, func);
        LogFileWrite(&g_DbgLog, buf, pos);
    }

    /****/
    // TODO 数据库导入代码

    /****/
    return;
}
#endif 

/*
 * IVS_LogAssert
 * 记录断言信息，并中断程序
 */
void IVS_LogAssert(const char *expr, const char *file, int line, const char *func)
{
    IVS_LogDbg(file, line, func, "Assert %s fail ", expr);
#if defined _MSC_VER || defined _WIN32
    DebugBreak();
#else
    abort();
#endif
}

