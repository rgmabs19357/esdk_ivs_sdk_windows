
/******************************************************************************
   版权所有 (C), 2008-2011, 华为技术有限公司

 ******************************************************************************
  文件名          : NVSLog.h
  版本号          : 1.0
  作者            : xuxin 00114494
  生成日期        : 2008-8-17
  最近修改        : 
  功能描述        : NVS日志模块用户接口
  函数列表        : 
  修改历史        : 
  1 日期          : 
    作者          : 
    修改内容      : 
*******************************************************************************/


#ifndef IVS_LOG_H
#define IVS_LOG_H

#include <stdarg.h>

#define IVSLOG_API

//BEGIN R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志
namespace IVSLOG
{
    const unsigned long LOG_FILE_COUNT_MAX  = 10;   //日志文件最大个数
    const unsigned long MAXLEN_FILEPATH     = 256;  //文件路径最大长度

    typedef char PATH_ARRAY[MAXLEN_FILEPATH];       //路径数组类型
}
//END   R001C05LITS01 ADD 2010-09-17 ligengqiang 00124479 for 实时循环覆盖日志，最多保持10个日志

//日志等级
typedef enum _IvsLogLevel
{
    LOG_EMERGENCY   = 0,    //系统不可用
    LOG_ALERT       = 1,    //必须立刻采取行动，否则系统不可用
    LOG_CRITICAL    = 2,    //严重错误
    LOG_ERROR       = 3,    //一般错误
    LOG_WARNING     = 4,    //警告
    LOG_NOTICE      = 5,    //重要的正常信息
    LOG_INFO        = 6,    //一般的正常信息
    LOG_DEBUG       = 7,    //调试信息
    LOG_MAX                 //日志级别最大值，做合法性校验用
}IvsLogLevel;

//写日志线程函数的错误类型
typedef enum _IvsLogThreadErr
{
   SUCCESS = 0,       //成功 
   NEGLECTABLE_ERR,   //可以忽略的问题，线程可以继续执行
   CRITICAL_ERR       //严重问题，线程必须退出
}IvsLogThreadErr;

//设置日志级别，默认是:LOG_INFO
IVSLOG_API void IvsSetLogLevel(long lLevel);

//设置当前写的日志文件路径名(完整路径或相对路径)
//默认是当前路径下:exename.log
IVSLOG_API bool IvsSetLogFilePathName(const char* szPathName);

//设置日志文件长度限制，超过此长度时生成新文件，单位KB(100K-100M,默认是10M)
IVSLOG_API void IvsSetLogFileLengthLimit(unsigned long ulLimitLengthKB);

//上面三项设置完成后，可以启动NVS日志模块
IVSLOG_API void IvsStartLog(void);

//写一条日志(用下面定义的NVSWriteLog宏来写日志)
IVSLOG_API void __IvsWriteLog(const char* szFileName, long lLine,long lLevel, const char* format, va_list argp);
//停止NVS日志模块
IVSLOG_API void IvsStopLog(void);

//vc6以及vc7.1都不支持C99(但g++支持)
//所以这里不能使用可变参数宏定义，利用对()操作符的重载实现
class CWriter   
{
    public:
        CWriter(const char* file, long line)
        {
            m_file_ = file;
            m_line_ = line;
        }
        void operator()(long level, const char* format, ...)
        {
            va_list argp;
            va_start(argp, format);
            __IvsWriteLog(m_file_,m_line_,level,format,argp);
            va_end(argp);
        }
    private:
        CWriter()   //过PC-LINT
        {
            m_file_ = NULL;
            m_line_ = 0;
        }
        const char* m_file_;
        long m_line_;
};

//程序中使用如下宏写日志
#define IVS_LOG (CWriter(__FILE__, __LINE__))
//例如：IVS_LOG(LOG_INFO, "Recv=%d,Send=%d", nRecv,nSend);


#endif//IVS_LOG_H

