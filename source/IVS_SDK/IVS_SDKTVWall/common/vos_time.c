

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef WIN32
#include "vos.h"
#else
#include "vos.h"
#endif


static ULONG g_ulSysStart = VOS_FALSE ;

static struct timeval g_startTime;

VOID VOS_StartTicks( VOID )
{
    if( VOS_TRUE == g_ulSysStart )
    {
        return ;
    }
    
    g_ulSysStart = VOS_TRUE ;

    memset(&g_startTime,0x00,sizeof(g_startTime));

#ifdef WIN32
    g_startTime.tv_sec = (long)timeGetTime();
#else
    gettimeofday(&g_startTime, NULL);
#endif

    return ;
}


ULONG VOS_GetTicks ( VOID )
{
    ULONG ticks = 0 ;

#ifdef WIN32
    ticks = timeGetTime();
#else
    struct timeval now;
    gettimeofday(&now, VOS_NULL);
    ticks=now.tv_sec;
#endif

    return( ticks );
}

VOID VOS_Delay (ULONG ulDelayTimeMs)
{
    LONG was_error;

    struct timeval tv;

    ULONG then, now, elapsed;

    then = VOS_GetTicks();

    do 
    {
        errno = 0;
        /* Calculate the time LONGerval left (in case of LONGerrupt) */
        now = VOS_GetTicks();
        elapsed = (now-then);
        then = now;
        if ( elapsed >= ulDelayTimeMs )
        {
            break;
        }

        ulDelayTimeMs -= elapsed;
        tv.tv_sec = (long)(ulDelayTimeMs/1000);
        tv.tv_usec = (ulDelayTimeMs%1000)*1000;

        was_error = select(0, VOS_NULL, VOS_NULL, VOS_NULL, &tv);

    } while ( was_error && (errno == EINTR) );
}

VOID VOS_Sleep( ULONG ulMs )
{
#if VOS_APP_OS == VOS_OS_LINUX
    VOS_Delay( ulMs );
#elif VOS_APP_OS == VOS_OS_WIN32
    Sleep(ulMs);
#endif

return ;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




