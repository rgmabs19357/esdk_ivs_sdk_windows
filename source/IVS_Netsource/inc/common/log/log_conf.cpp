#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compat.h"
#include "ivs_log.h"

#if defined _MSC_VER && (_MSC_VER >= 1500)
#pragma warning (disable : 4996)
#endif

#ifdef _DEBUG 
# define DEBUG_CONF(fmt, ...)  printf(fmt, ##__VA_ARGS__);
#else
# define DEBUG_CONF(fmt, ...)
#endif 

#define MAX_COMMAND_LINE_LEN  1024   // 配置命令长度
#define MAX_COMMAND_ARGS      32     // 最多32个参数

int HandleLogLevel(int argc, char *argv[]);
int HandleDebugLog(int argc, char *argv[]);
int HandleLogSize (int argc, char *argv[]);
int HandleLogCount(int argc, char *argv[]);

typedef struct
{
    const char* szCmdName;
    int (*handler)(int argc, char* argv[]);
} CONF_CMD;

CONF_CMD Commands[] = {
    { "LogLevel",   HandleLogLevel }, 
    { "DebugLog",   HandleDebugLog },
    { "LogSize",    HandleLogSize  },
    { "LogCount",   HandleLogCount },
    /* 添加参数配置 */
    { NULL, NULL }
};

/***************************************************************************************/

#define NEXT_NON_SPACE(ptr)\
do {\
    while (*ptr && (*ptr == ' ' || *ptr == '\t') \
	       && *ptr != '\r' && *ptr != '\n')\
	{\
		ptr++;\
	}\
} while (0)

#define NEXT_SPACE(ptr)\
do {\
	while (*ptr && *ptr != ' ' && *ptr != '\t'\
		&& *ptr != '\r' && *ptr != '\n')\
	{\
		ptr++;\
	}\
} while (0)

#define NEXT_QUOT(ptr)\
do {\
	while (*ptr && *ptr != '"' \
			&& *ptr != '\r' && *ptr != '\n') \
	{\
		ptr++;\
	}\
} while (0)     

#define IS_LINE_END(ptr)\
	(!*ptr || *ptr == '\r' || *ptr == '\n')


/*
 * FindCmd
 * 查找命令
 */
static inline CONF_CMD *FindCmd(const char *name)
{
	CONF_CMD *pCmd = NULL;
	CONF_CMD *p = &Commands[0];

	while (p->szCmdName)
	{
		if (!strcmp(p->szCmdName, name))
		{
			pCmd = p;
			break;
		}
		p++;
	}
	return pCmd;
}

/*
 * 解析命令行
 */
static int ParseLine(const char *szLine)
{
    char Buf[MAX_COMMAND_LINE_LEN + 1] = {0};
    char *argv[MAX_COMMAND_ARGS] = {0};
    int  argc = 0 ;
    char* ptr = Buf;
    int ret = -1;
    int in_quot = 0;

    strncpy(Buf, szLine, MAX_COMMAND_LINE_LEN);
    while (argc < MAX_COMMAND_ARGS) 
    {
		NEXT_NON_SPACE(ptr);
		if (IS_LINE_END(ptr))
		{
			break;
		}

        /* 对双引号做特殊处理 */
        if (*ptr == '"') 
        {
            in_quot = 1;
            ptr++;
        }
        argv[argc++] = ptr;

        if (in_quot) 
        { 
            /* 查找下一个匹配的双引号 */
            NEXT_QUOT(ptr);
            if (*ptr != '"') 
            {
                printf("argument error, no complete quotationn");
                return -1;
            } 
            else
            {
                in_quot = 0;
            }
        } 
        else 
        {
           /* 查找空格 */
			NEXT_SPACE(ptr);
        }

        if (IS_LINE_END(ptr)) 
        {
            *ptr = '\0';  
            break;
        }
        else 
        {
            *ptr = '\0';    
            ptr++;
        }
    }

    if (argc == 0)
    {
        return 0;
    }

	CONF_CMD *pCmd = FindCmd(argv[0]);

	if (pCmd) 
	{  
		if (pCmd->handler)
		{
			ret = pCmd->handler(argc, argv);
		}
	}
	else
	{
		printf("can not find command'%s'\n", argv[0]);
	}
    return ret;
}

/*
 * process configure file
 * 读取配置文件
 */
int ReadConfig(const char* szFile)
{
    char Line[MAX_COMMAND_LINE_LEN + 1] = {0};
    int  LineNum = 0;
    int  retval = 0;
    char *buf = Line;
    int  left = MAX_COMMAND_LINE_LEN;

    FILE *fp = fopen(szFile, "r");
    if (NULL == fp) 
    {
        printf("Open %s Fail, error = %d\n", szFile, errno);
        return -1;
    }

    while (fgets(buf, left, fp)) 
    {
        char *cmd = buf;
        LineNum++;

        while (*cmd && *cmd != '#' && *cmd != '\\')
            cmd++;

        if (*cmd == '\\') 
        {
            left -= (cmd - buf);
            if (left == 0) 
            {
                printf("command too long in file %s%d\n", szFile, LineNum);
                retval = -1;
                break;
            }
            buf = cmd;
            continue;
        } 

		// #开始为注释
        if (*cmd == '#')
        {
            *cmd = '\0';
        }

        if (cmd > Line) 
        { 
            retval = ParseLine(Line);
            if (retval < 0) 
            {            
                printf("fail to parse command line in file %s%d\n", szFile, LineNum);
                break;
            }
        }
        memset(Line, 0, sizeof(Line));
        buf = Line;
        left = MAX_COMMAND_LINE_LEN;
    }

    fclose(fp);
    return retval;
}

/***************************************************************************************/


/*
 * HandleLogLevel
 * 处理日志级别配置
 */
int HandleLogLevel(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 0;   //没有设定参数，使用默认值
    }

    int level = IVS_LOG_INFO;

    if (!IVS_strcasecmp(argv[1], "Debug"))
    {
        level = IVS_LOG_DEBUG;
    } 
    else if (!IVS_strcasecmp(argv[1], "Info"))
    {
        level = IVS_LOG_INFO;
    }
    else if (!IVS_strcasecmp(argv[1], "Notice"))
    {
        level = IVS_LOG_NOTICE;
    }
    else if (!IVS_strcasecmp(argv[1], "Warning"))
    {
        level = IVS_LOG_WARNING;
    }
    else if (!IVS_strcasecmp(argv[1], "Error"))
    {
        level = IVS_LOG_ERR;
    }
    else if (!IVS_strcasecmp(argv[1], "Critical"))
    {
        level = IVS_LOG_CRIT;
    }
    else if (!IVS_strcasecmp(argv[1], "Alert"))
    {
        level = IVS_LOG_ALERT;
    }
    else if (!IVS_strcasecmp(argv[1], "Emergency"))
    {
        level = IVS_LOG_EMERG;
    }
    else
    {
        level = IVS_LOG_INFO;  // 默认设置为Info
    }

    (void)IVS_LogSetLevel(level);
    DEBUG_CONF("Set Loglevel to %s\n", argv[1]);
    return 0;
}//lint !e818 无需声明为const

/*
 * HandleLogDebug
 * 处理调试日志配置
 */
int HandleDebugLog(int argc, char *argv[])
{
    int enable = 0;
    if (argc < 2)
    {
        return 0;   // 不修改
    }

    if (!IVS_strcasecmp(argv[1], "enable"))
    {
        enable = 1;
    }
    (void)IVS_LogSetDebug(enable);
    DEBUG_CONF("Set DebugLog %s\n", enable? "enable" : "disable");
    return 0;
}//lint !e818 无需声明为const

/*
 * HandleLogSize
 * 处理日志文件大小配置
 */
int HandleLogSize (int argc, char *argv[])
{
    if (argc < 2)
    {
        return 0;
    }
    unsigned long s = (unsigned long)strtol(argv[1], NULL, 10);
    (void)IVS_LogSetFileSize(s);

    DEBUG_CONF("Set LogFileSize %lu\n", s);
    return 0;
}//lint !e818 无需声明为const


/*
 * HandleLogCount
 * 处理日志数量配置
 */
int HandleLogCount(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 0;
    }
    unsigned long c = (unsigned long)strtol(argv[1], NULL, 10);
    (void)IVS_LogSetFileCount(c);

    DEBUG_CONF("Set LogFileCount %lu\n", c);

    return 0;

}//lint !e818 无需声明为const

