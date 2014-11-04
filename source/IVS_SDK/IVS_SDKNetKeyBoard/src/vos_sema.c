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

ULONG  VOS_CreateSemaphore( ULONG ulInitValue ,VOS_Sem **pstVosSem)
{
    VOS_Sem *pstVosSemTemp = VOS_NULL ;
    LONG lResult = VOS_OK ; 

    pstVosSemTemp = (VOS_Sem *) VOS_malloc(sizeof(VOS_Sem));
    if ( VOS_NULL == pstVosSemTemp )
    {
        return VOS_ERR_MEM ;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    lResult = sem_init(&pstVosSemTemp->sem, 0, ulInitValue );
    if ( VOS_OK != lResult ) 
    {
        VOS_free(pstVosSemTemp);
        return VOS_ERR_SYS ;
    }
#elif VOS_APP_OS == VOS_OS_WIN32
    pstVosSemTemp->sem = CreateSemaphore(NULL,(long)ulInitValue,(long)ulInitValue,NULL);
    if (NULL == pstVosSemTemp->sem)
    {
		//modify by limingjie lWX175483 :2013-5-24
		//创建信号量失败则将申请的内存释放
		VOS_free(pstVosSemTemp);
        return VOS_ERR_SYS ;
    }
    (void)lResult;
#endif
    *pstVosSem = pstVosSemTemp;

    return VOS_OK ;
}

ULONG VOS_DestroySemaphore( VOS_Sem *pstVosSem )
{
    if ( VOS_NULL == pstVosSem ) 
    {
        return VOS_ERR_PARAM ;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    sem_destroy( &pstVosSem->sem );
#elif VOS_APP_OS == VOS_OS_WIN32
    (void)CloseHandle(pstVosSem->sem);
#endif

    VOS_free( pstVosSem );	

    return VOS_OK ;
}

ULONG VOS_SemPost(VOS_Sem *pstVosSem)
{
    ULONG ulResult = VOS_OK ;

    if( VOS_NULL == pstVosSem )
    {
        return VOS_ERR_PARAM ;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    ulResult = ( ULONG )sem_post(&pstVosSem->sem );
    if( VOS_OK != ulResult )
    {
        return VOS_ERR_SYS ;
    }
#elif VOS_APP_OS == VOS_OS_WIN32
    if (TRUE != ReleaseSemaphore(pstVosSem->sem,1,NULL))
    {
        return VOS_ERR_SYS;
    }
#endif

    return ulResult ;
}/*lint !e818*///使用公共平台源代码，是否Const不做要求


ULONG VOS_SemWait( VOS_Sem *pstVosSem )
{
    ULONG ulResult = VOS_OK ;

    if ( VOS_NULL == pstVosSem ) 
    {
        return VOS_ERR_PARAM ;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    ulResult = (ULONG)sem_wait( &pstVosSem->sem );
    if( VOS_OK != ulResult )
    {
        return VOS_ERR_SYS ;
    }
#elif VOS_APP_OS == VOS_OS_WIN32
    ulResult = WaitForSingleObject(pstVosSem->sem,INFINITE);
    if (WAIT_OBJECT_0 != ulResult)
    {
        return VOS_ERR_SYS ;
    }
#endif

    return ulResult ;
}/*lint !e818*/ //无此问题

ULONG VOS_SemTryWait( VOS_Sem *pstVosSem )
{
    ULONG ulResult = VOS_OK ;

    if ( VOS_NULL == pstVosSem ) 
    {
        return VOS_ERR_PARAM ;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    ulResult = ( ULONG )sem_trywait( &pstVosSem->sem );
    if( VOS_OK != ulResult )
    {
        return VOS_ERR_SEMA_TIMEOUT ;
    }
#elif VOS_APP_OS == VOS_OS_WIN32
    ulResult = WaitForSingleObject(pstVosSem->sem,0);
    if (WAIT_OBJECT_0 != ulResult)
    {
        return VOS_ERR_SEMA_TIMEOUT ;
    }
#endif

    return ulResult ;
}/*lint !e818*/ //无此问题


ULONG VOS_SemWaitTimeout(VOS_Sem *pstVosSem, ULONG ulTimeout)
{
    ULONG ulResult = VOS_OK ;

    if ( VOS_NULL == pstVosSem ) 
    {
        return VOS_ERR_PARAM ;
    }
#if VOS_APP_OS == VOS_OS_LINUX
    if ( VOS_MUTEX_MAXWAIT == ulTimeout ) 
    {
        return VOS_SemWait( pstVosSem );
    }

    ulTimeout += VOS_GetTicks();
    do 
    {
        ulResult = VOS_SemTryWait( pstVosSem );
        if ( VOS_OK == ulResult ) 
        {
            break;
        }
        
        ulResult = VOS_ERR_SEMA_TIMEOUT ;

        VOS_Delay(1);
    } while ( VOS_GetTicks() < ulTimeout );
#elif VOS_APP_OS == VOS_OS_WIN32
    ulResult = WaitForSingleObject(pstVosSem->sem,ulTimeout);
    switch(ulResult)
    {
    case WAIT_OBJECT_0:
        return VOS_OK;
    case WAIT_TIMEOUT:
        return VOS_ERR_SEMA_TIMEOUT;
    default:
        break;
    }
#endif

    return ulResult ;
}/*lint !e818*/ //无此问题

#if VOS_APP_OS == VOS_OS_LINUX
ULONG VOS_GetSemValue( VOS_Sem *pstVosSem )
{
    long lValue = 0 ;

    if( VOS_NULL == pstVosSem )
    {
        return VOS_ERR_PARAM ;
    }

    sem_getvalue(&pstVosSem->sem, &lValue);
    if ( lValue < 0 ) 
    {
        lValue = 0;
    }

    return (ULONG)lValue;
}
#elif VOS_APP_OS == VOS_OS_WIN32
ULONG VOS_GetSemValue( VOS_Sem *pstVosSem )
{
    (void)pstVosSem;
    return 0;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



