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

#define THREAD_EXIT_WAITTIME 15000 //等待线程结束的时间为15秒


ULONG  VOS_CreateThread( VOS_THREAD_FUNC pfnThread, VOID *args, VOS_Thread **pstVosThread,ULONG ulStackSize)
{    
    VOS_Thread *pstThread = VOS_NULL ;
    
    pstThread = (VOS_Thread*)VOS_malloc(sizeof(VOS_Thread));
    if( VOS_NULL == pstThread )
    {
        return VOS_ERR_MEM; 
    }
    
#if VOS_APP_OS == VOS_OS_LINUX
    if ( pthread_attr_init(&pstThread->attr) != 0 )
    {
        VOS_free(pstThread);        
        return VOS_ERR ;
    }    

    pthread_attr_setdetachstate(&pstThread->attr, PTHREAD_CREATE_JOINABLE );

    if( 0 == ulStackSize )
    {
        ulStackSize = VOS_DEFAULT_STACK_SIZE;
    }    
    if (pthread_attr_setstacksize(&pstThread->attr, (size_t)ulStackSize))
    {
        VOS_free(pstThread);    
        return VOS_ERR ;
    }    

    if ( pthread_create(&pstThread->pthead, &pstThread->attr, pfnThread, args) != 0 ) 
    {
        VOS_free(pstThread);
        return VOS_ERR ;
    }
#elif VOS_APP_OS == VOS_OS_WIN32
    pstThread->pthead = CreateThread(NULL,ulStackSize,pfnThread,args,0,&pstThread->ptheadID);
    if (NULL == pstThread->pthead)
    {
        VOS_free(pstThread);
        return VOS_ERR ;
    }
#endif
    *pstVosThread = pstThread ;
    
    return VOS_OK;
}

//BEGIN V100R001C05 MODIFY 2010-01-07 zhaoyuzhen z00137994 for 线程指针为空时停止线程导致崩溃问题	
LONG VOS_ThreadJoin( VOS_Thread *pstVosThread )
{
    //如果线程指针为空，则直接返回OK
    if(NULL == pstVosThread)
    {
        return VOS_OK;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    return pthread_join(pstVosThread->pthead, 0);
#elif VOS_APP_OS == VOS_OS_WIN32
    pstVosThread = pstVosThread;
    (void)WaitForSingleObject(pstVosThread->pthead, THREAD_EXIT_WAITTIME);/*lint !e613*/ //无此问题
    (void)CloseHandle(pstVosThread->pthead);/*lint !e613*/ //无此问题
    return VOS_OK;
#endif 

}

#if VOS_APP_OS == VOS_OS_LINUX
VOID VOS_StopThread( VOS_Thread *pstVosThread )
{
    //如果线程指针为空，则直接返回
    if(NULL == pstVosThread)
    {
        return;
    }

    pthread_cancel(pstVosThread->pthead);
    pthread_join(pstVosThread->pthead, 0);
    
    return ;
}

VOID VOS_ThreadCancel( VOS_Thread *pstVosThread )
{
    //如果线程指针为空，则直接返回
    if(NULL == pstVosThread)
    {
        return;
    }

    pthread_cancel(pstVosThread->pthead);
    return ;
}

VOID VOS_DeleteThread( VOS_Thread *pstVosThread )
{
    //如果线程指针为空，则直接返回
    if(NULL == pstVosThread)
    {
        return;
    }

    VOS_StopThread( pstVosThread );

    VOS_free( pstVosThread );
    return ;
}

#endif

VOID  VOS_pthread_exit(void *retval)
{
    //如果线程指针为空，则直接返回OK
    if(NULL == retval)
    {
        return;
    }

#if VOS_APP_OS == VOS_OS_LINUX
    pthread_exit(retval);
#elif VOS_APP_OS == VOS_OS_WIN32
    (void)retval;
    ExitThread(0);
#endif
};//lint !e19
//END V100R001C05 ADD 2010-01-07 zhaoyuzhen z00137994 for 线程指针为空时停止线程导致崩溃问题	

#if VOS_APP_OS == VOS_OS_LINUX
pthread_t  VOS_pthread_self(void)
{
    return pthread_self();
};
#elif VOS_APP_OS == VOS_OS_WIN32
HANDLE VOS_pthread_self(void)
{
    return GetCurrentThread();
};//lint !e19
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


