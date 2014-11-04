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


LONG VOS_CreateEvent(VOS_Event *pstVosEvent)
{
    LONG result = VOS_OK;
    
#if VOS_APP_OS == VOS_OS_LINUX

    result = pthread_mutex_init(&pstVosEvent->EventMutex, 0);
    if( VOS_OK != result )
    {
        return VOS_ERR_SYS;
    }

    result = pthread_cond_init(&pstVosEvent->EventCond, 0);
    if( VOS_OK != result )
    {
        pthread_mutex_destroy(&pstVosEvent->EventMutex);
        return VOS_ERR_SYS;
    }
    
#elif VOS_APP_OS == VOS_OS_WIN32
    pstVosEvent->EventHandle = CreateEvent(0, FALSE, FALSE, 0);
    if( VOS_NULL == pstVosEvent->EventHandle )    
    {
        (void)CloseHandle(pstVosEvent->EventHandle);
        return VOS_ERR_SYS;
    }
#endif

    return result ;
}

LONG VOS_WaitEvent(VOS_Event *pstVosEvent, LONG lTimeOut)
{
    long lResult = VOS_OK;
    
#if VOS_APP_OS == VOS_OS_LINUX
    struct timespec ts;
    struct timeval  tv;

    gettimeofday(&tv, 0);
    ts.tv_sec  = tv.tv_sec  + lTimeOut/1000;
    ts.tv_nsec = (tv.tv_usec + (lTimeOut %1000)*1000) * 1000; 


    (void)pthread_mutex_lock(&pstVosEvent->EventMutex);
    if( 0 != lTimeOut )
    {
        lResult = pthread_cond_timedwait(&pstVosEvent->EventCond,
                                    &pstVosEvent->EventMutex,&ts);
    }
    else
    {
        lResult = pthread_cond_wait(&pstVosEvent->EventCond,
                                 &pstVosEvent->EventMutex);
    }
    (void)pthread_mutex_unlock(&pstVosEvent->EventMutex);
    
    if( VOS_OK != lResult )
    {
        switch(lResult)
        {
            case ETIMEDOUT:
            {
                lResult = VOS_ERR_QUE_TIMEOUT;
                break;
            }

            default:
            {
                lResult = VOS_ERR_SYS;
                break;
            }
        }
    }
    
#elif VOS_APP_OS == VOS_OS_WIN32

    lResult = (long)WaitForSingleObject(pstVosEvent->EventHandle, (unsigned long)lTimeOut);/*lint !e613*/ //无此问题
    switch(lResult)
    {
        case WAIT_TIMEOUT:
        {
            lResult = VOS_ERR_QUE_TIMEOUT;
            break;
        }

        case WAIT_ABANDONED:
        {
            lResult = VOS_ERR_SYS;
            break;
        }
		case WAIT_OBJECT_0:
		{
            lResult = VOS_OK;
            break;
		}
        default:
        {
            lResult = VOS_ERR_SYS;                
        }
        break;                        
    }
 
#endif

    return lResult;
}/*lint !e818*/ //使用公共平台源代码，是否Const不做要求

LONG VOS_SetEvent(VOS_Event *pstVosEvent)
{
    LONG lResult = VOS_OK;

#if VOS_APP_OS == VOS_OS_LINUX
    lResult = pthread_cond_signal(&pstVosEvent->EventCond);
    if(VOS_OK != lResult)
    {
        lResult = VOS_ERR_SYS;                
    }
    
#elif VOS_APP_OS == VOS_OS_WIN32

    lResult = SetEvent(pstVosEvent->EventHandle);/*lint !e613*/ //无此问题

    if(VOS_OK != lResult)
    {
        lResult = VOS_OK;                
    }
	else
	{
		lResult = VOS_ERR_SYS;
	}
    
#endif

    return lResult ;
}/*lint !e818*/ //无此问题
LONG VOS_ReSetEvent(VOS_Event *pstVosEvent)
{
    LONG lResult = VOS_OK;
    
#if VOS_APP_OS == VOS_OS_LINUX
    result = pthread_mutex_init(&pstVosEvent->EventMutex, 0);
    if( VOS_OK != result )
    {
        return VOS_ERR_SYS;
    }
    
    result = pthread_cond_init(&pstVosEvent->EventCond, 0);
    if( VOS_OK != result )
    {
        pthread_mutex_destroy(&pstVosEvent->EventMutex);
        return VOS_ERR_SYS;
    }
#elif VOS_APP_OS == VOS_OS_WIN32
    
    lResult = ResetEvent(pstVosEvent->EventHandle);
    
    if(VOS_OK != lResult)
    {
        lResult = VOS_OK;                
    }
    else
    {
        lResult = VOS_ERR_SYS;
    }
    
#endif

    return lResult ;
}/*lint !e818*/ //无此问题

LONG VOS_DestroyEvent(VOS_Event *pstVosEvent )
{
    if ( VOS_NULL == pstVosEvent ) 
    {
        return VOS_ERR_PARAM ;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    pthread_cond_destroy(&pstVosEvent->EventCond);
    pthread_mutex_destroy(&pstVosEvent->EventMutex);
#elif VOS_APP_OS == VOS_OS_WIN32
    (void)CloseHandle(pstVosEvent->EventHandle);
#endif

    return VOS_OK ;
}/*lint !e818*/ //无此问题


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

