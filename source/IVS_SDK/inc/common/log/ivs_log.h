/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  ivs_log.h
//  文件说明:   日志接口头文件
//  版    本:   IVS V100R002C01
//  作    者:   w00210470
//  创建日期:   2012/11/12
//  修改标识：
//  修改说明：
*******************************************************************************/
#ifndef  __IVS_LOG_H__
#define  __IVS_LOG_H__

enum LOG_LEVEL 
{
    IVS_LOG_EMERG   = 0,   /* 紧急，系统不可用 */
    IVS_LOG_ALERT   = 1,   /* 必须立即采取措施 */
    IVS_LOG_CRIT    = 2,   /* 致命情况         */
    IVS_LOG_ERR     = 3,   /* 错误情况         */
    IVS_LOG_WARNING = 4,   /* 警告情况         */
    IVS_LOG_NOTICE  = 5,   /* 一般重要情况     */
    IVS_LOG_INFO    = 6,   /* 普通信息         */
    IVS_LOG_DEBUG   = 7    /* 调试信息         */
};

enum LOG_TYPE
{
    TYPE_SEC = 0,   /* 安全日志 */
    TYPE_OPR = 1,   /* 操作日志 */
    TYPE_RUN = 2,   /* 运行日志 */
    TYPE_ITF = 3,   /* 接口日志 */
    TYPE_USR = 4,   /* 用户日志 */
    TYPE_DBG = 5    /* 调试日志 */
};


/* 时区设置标志 */
enum  {
    UTC_FLG_PLUS = 0,   /* UTC + */
    UTC_FLG_NEG  = 1    /* UTC - */
};


/* 工程相关设置 */
//#define LOG_HAS_SFTP_UPLOAD       /* 日志sftp上传功能 注释取消该功能 */
#define LOG_HAS_CONFIG_FILE       /* 日志自有配置文件 注释取消该功能 */
#define LOG_HAS_WRITE_THREAD      /* 使用独立线程写入 注释取消该功能 */
//#define LOG_HAS_STACK_TRACE       /* 级别严重的日志同时打印调用栈，注释取消该功能 */

#if defined LOG_HAS_SFTP_UPLOAD || (!defined LOG_HAS_SFTP_UPLOAD && !defined LOG_HAS_WRITE_THREAD)
#define LOG_HAS_MONITOR_THREAD   /* 监控线程 */
#endif 

//#define LOG_HAS_SEC               /* 使能安全日志 注释去使能 */ 
//#define LOG_HAS_OPR               /* 使能操作日志 注释去使能 */
//#define LOG_HAS_ITF               /* 使能接口日志 注释去使能 */
//#define LOG_HAS_USER              /* 使能用户日志 注释去使能 */

#define FILE_PRESERVE_COUNT     10                /* 本地日志文件最大保存数量 */
#define FILE_MAX_SIZE_DEF      (10 * 1024 * 1024) /* 默认文件上限 10 M */


/* 异常回调定义 */
#define LOG_ERR_DISKFULL       500   /* 磁盘满         */   
#define LOG_ERR_DISKRESTORE    501   /* 磁盘恢复       */
#define LOG_ERR_OPENFAIL       502   /* 打开文件失败   */
#define LOG_ERR_DBIMPORT       503   /* 导入数据库错误 */
#define LOG_ERR_SFTPFAIL       504   /* sftp异常       */

/* --------------------------------------------------------------------------- */

/* 接口函数 */
int  IVS_LogInit(const char *szDir, const char *szConf, const char* szFileName, const char *szModuleType, 
	             int uModuleCode, void (*callback)(int code));
void IVS_LogClean();

int  IVS_LogSetFileSize(unsigned long s);
int  IVS_LogSetFileCount(unsigned int c);
int  IVS_LogSetLevel(int level);
int  IVS_LogSetDebug(int enable);

void IVS_LogSetTimeZone(unsigned int hour, unsigned int min, int flag);

#ifdef LOG_HAS_SFTP_UPLOAD
int  IVS_LogSetSftp(char *szSvrDir, char *szUser, char *szPassWord, 
                   char *szSvrAddr, unsigned short usSvrPort);
#endif


/* 运行日志 */
void IVS_LogRun(int level, const char *stcMsgText, const char *file, int line, const char *func, const char *fmt, ...);
#define IVS_LOG_RUN(level, stcMsgText, fmt, ...) \
    IVS_LogRun(level, stcMsgText, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define IVS_LOG(level, stcMsgText, fmt, ...)\
    IVS_LogRun(level, stcMsgText, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
    
/* 调试日志 */
void IVS_LogDbg(const char *file, int line, const char *func, const char *fmt, ...);
#define IVS_LOG_DBG(fmt, ...) \
    IVS_LogDbg(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)


/* 接口日志 */
#ifdef LOG_HAS_ITF 
void IVS_LogInterface(int level, char *inftype, char *infname, char *sender, char *receiver,
      char *traceMark, char *operation, char *result, const char *file,
      int line, const char *func, const char *fmt, ...);

# define IVS_LOG_ITF(level, inftype, infname, sender, receiver, traceMark, operation, result, fmt, ...)\
    IVS_LogInterface(level, inftype, infname, sender, receiver, traceMark, operation, result,\
            __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif 


/* 操作日志 */
#ifdef LOG_HAS_OPR
void IVS_LogOperation(int level, char *user,  char* ip, char *object, char *stcMsgNum, char*stcMsgText, char *opcode, 
                     char *result, const char *file, int line, const char *func, const char *fmt, ...);

# define IVS_LOG_OPR(level, user, ip, object, stcMsgNum, stcMsgText, opcode, result, fmt, ...)\
    IVS_LogOperation(level, user, ip, object, stcMsgNum, stcMsgText, opcode, result,\
         __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif 

/* 安全日志 */
#ifdef LOG_HAS_SEC
void IVS_LogSecurity(int level, char *user, char *ip, char *stcMsgNum, char*stcMsgText, 
           char*opcode, char *result, const char *file, int line, const char *func, const char *fmt, ...);

# define IVS_LOG_SEC(level, user, ip, stcMsgNum, stcMsgText, operation, result, fmt, ...)\
    IVS_LogSecurity(level, user, ip, stcMsgNum, stcMsgText, operation, result,\
        __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif 


/* 用户日志 */
#ifdef LOG_HAS_USER
void IVS_LogUser(int level, char *user, char *operation, char *result,
             char *file, int line, char *func, const char *fmt, ...);

# define IVS_LOG_USR(level, user, operation, result, fmt, ...)\
    IVS_LogUser(level, user, operation, result, \
        __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif 


/* 调试相关 */
void IVS_LogAssert(const char *expr, const char *file, int line, const char *func);
#if defined(_DEBUG) || defined (DEBUG) 
# define IVS_ASSERT(expr) ((expr) ? void(0): IVS_LogAssert(#expr, __FILE__, __LINE__, __FUNCTION__))
#else
# define IVS_ASSERT(expr)
#endif 

#ifdef __cplusplus
class CDbgTrace {
public:
    CDbgTrace(const char *file, int line, const char *func)
        :m_pFunc(func), m_pFile(file), m_nLine(line)
    {
        IVS_LogDbg(m_pFile, line, m_pFunc, "Enter Function %s", func);   
    }
    ~CDbgTrace()
    {
		try {IVS_LogDbg(m_pFile, m_nLine, m_pFunc, "Leave Function %s", m_pFunc);} catch(...){} ; //lint !e1740
    }
private:    
    const char *m_pFunc;
    const char *m_pFile;
    int         m_nLine;
};//lint !e1712 无须默认构造函数
# define IVS_TRACE()   CDbgTrace t(__FILE__, __LINE__, __FUNCTION__)
#else
# define IVS_TRACE()
#endif  // __cplusplus 

#endif // __IVS_LOG_H__

