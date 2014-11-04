#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#if defined _MSC_VER || defined WIN32
# include <sys/timeb.h>  // for _ftime
#endif 

#include "compat.h"

#if defined _MSC_VER && (_MSC_VER >= 1500)
#pragma warning (disable : 4996)
#endif 


/*
 * ivs_vsnprintf
 */
#if defined _MSC_VER || defined _WIN32
int IVS_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
    int len = _vsnprintf_s(buffer, count, count, format, argptr);
    return len == -1? (int)count : len;
}
#endif 

#if defined __GNUC__ || defined LINUX
int ivs_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
    size_t len = vsnprintf(buffer, count, format, argptr);
    return len >= count ?  count - 1 : len; 
}
#endif


/*
 * ivs_snprintf
 */
int IVS_snprintf(char *buffer, size_t count, const char *format, ...)
{
    int len = 0;
    va_list args;
    va_start(args, format);
    len = IVS_vsnprintf(buffer, count, format, args);
    va_end(args);
    return len;
}

/*
 * IVS_strcasecmp
 */
#if defined _MSC_VER || defined _WIN32
int IVS_strcasecmp(const char *s1, const char *s2)
{
    return _stricmp(s1, s2);
}

int IVS_strncasecmp(const char *s1, const char* s2, size_t count)
{
    return  _strnicmp(s1, s2, count);
}
#else
int IVS_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2);
}

int IVS_strncascmp(const char *s1, const char *s2, size_t count)
{
    return  strncasecmp(s1, s2, count);
}

#endif 


/*
 * IVS_localtime
 */
#ifdef _MSC_VER
int IVS_localtime(struct tm *_tm, const time_t *_tt)
{
    if (NULL == _tm || NULL == _tt)
    {
        return -1;
    }
    localtime_s(_tm, _tt);
    return 0;
}

int IVS_gettimeofday(struct timeval *tv)
{
    if (NULL == tv)
    {
        return (-1);
    }
    
    struct _timeb  localTime;
    _ftime(&localTime);
    tv->tv_sec  = (long)(localTime.time + localTime.timezone);
    tv->tv_usec = localTime.millitm * 1000;
    return (0);
}
#endif 


#if defined __GNUC__ || defined LINUX
int IVS_localtime(struct tm *_tm, const time_t *_tt)
{
    if (NULL == _tm || NULL == _tt)
    {
        return -1;
    }
    localtime_r(_tt, _tm);
    return 0;
}

int IVS_gettimeofday(struct timeval *tv)
{
    if (NULL == tv)
    {
        return -1;
    }
    gettimeofday(tv, NULL);
    return 0;
}
#endif 

