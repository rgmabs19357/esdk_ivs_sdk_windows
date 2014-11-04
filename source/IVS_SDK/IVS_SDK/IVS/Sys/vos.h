#ifndef VOS_H_INCLUDE
#define VOS_H_INCLUDE


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos_config.h"
#include "vos_basetype.h"
#include "vos_errno.h"

#ifndef  INFINITE
#define  INFINITE               0
#endif

#define  VOS_DEFAULT_STACK_SIZE (128*1024)

#define  VOS_MUTEX_MAXWAIT      INFINITE

#if VOS_APP_OS == VOS_OS_LINUX
#define CLOSESOCK(x) ::close(x)
#define SOCK_OPT_TYPE void
#define CONN_ERRNO errno
#elif VOS_APP_OS == VOS_OS_WIN32
#define CLOSESOCK(x) closesocket(x)
// modi by wKF71044 at [2012-8-13 12:45:06] socklen_t已被windows定义，不需要重新定义
//#define socklen_t int
#define SOCK_OPT_TYPE char
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define CONN_ERRNO WSAGetLastError()
//#define EWOULDBLOCK WSAEWOULDBLOCK
//#define EINPROGRESS WSAEINPROGRESS
#endif


#if VOS_APP_OS == VOS_OS_LINUX
typedef struct tagVOSSemaphore 
{
    sem_t sem;
}VOS_Sem;

typedef struct tagVOSThread 
{
    pthread_attr_t attr;
    pthread_t pthead;
}VOS_Thread;

typedef struct tagVOSMutex
{
    pthread_mutex_t  mutex;
}VOS_Mutex;

typedef struct tagVOSEvent 
{
    pthread_mutex_t EventMutex;
    pthread_cond_t  EventCond;
}VOS_Event;

#elif VOS_APP_OS == VOS_OS_WIN32
typedef struct tagVOSSemaphore 
{
    HANDLE sem;
}VOS_Sem;

typedef struct tagVOSThread 
{
    ULONG ptheadID;
    HANDLE pthead;
}VOS_Thread;

typedef struct tagVOSMutex
{
    HANDLE  mutex;
}VOS_Mutex;

typedef struct tagVOSEvent 
{
    HANDLE EventHandle;
}VOS_Event;
#endif

#if VOS_APP_OS == VOS_OS_WIN32
typedef  ULONG(__stdcall * VOS_THREAD_FUNC)(VOID *)  ;
#else
typedef  VOID* ( * VOS_THREAD_FUNC)(VOID *)  ;
#endif

ULONG VOS_CreateSemaphore( ULONG ulInitValue, VOS_Sem**pstVosSem );
ULONG VOS_DestroySemaphore( VOS_Sem *pstVosSem );
ULONG VOS_SemPost(VOS_Sem *pstVosSem);
ULONG VOS_SemWait( VOS_Sem *pstVosSem );
ULONG VOS_SemTryWait( VOS_Sem *pstVosSem );
ULONG VOS_SemWaitTimeout(VOS_Sem *pstVosSem, ULONG ulTimeout);
ULONG VOS_GetSemValue( VOS_Sem *pstVosSem );

VOS_Mutex *VOS_CreateMutex( VOID );
ULONG VOS_DestroyMutex( VOS_Mutex *pstMutex );
ULONG VOS_MutexLock( VOS_Mutex *pstMutex );
ULONG VOS_MutexUnlock( VOS_Mutex *pstMutex );
 
#if VOS_APP_OS == VOS_OS_LINUX 
ULONG VOS_CreateThread( VOS_THREAD_FUNC pfnThread, VOID *args, 
          VOS_Thread **pstVosThread,ULONG ulStackSize=VOS_DEFAULT_STACK_SIZE);
#elif VOS_APP_OS == VOS_OS_WIN32
ULONG VOS_CreateThread( VOS_THREAD_FUNC pfnThread, VOID *args, 
          VOS_Thread **pstVosThread,ULONG ulStackSize);
#endif
VOID  VOS_pthread_exit(void *retval);
LONG  VOS_ThreadJoin( VOS_Thread *pstVosThread );

#if VOS_APP_OS == VOS_OS_LINUX
VOID  VOS_StopThread( VOS_Thread *pstVosThread );
VOID  VOS_DeleteThread( VOS_Thread *pstVosThread );
VOID  VOS_ThreadCancel( VOS_Thread *pstVosThread );
pthread_t  VOS_pthread_self(void);
#elif VOS_APP_OS == VOS_OS_WIN32
HANDLE VOS_pthread_self(void);
#endif

VOID *VOS_malloc( ULONG size );
VOID  VOS_free( VOID *buff );

VOID  VOS_Sleep( ULONG ulMs );
VOID  VOS_StartTicks( VOID );
ULONG VOS_GetTicks ( VOID );
VOID  VOS_Delay (ULONG ulDelayTimeMs);

LONG VOS_CreateEvent(VOS_Event *pstVosEvent);
LONG VOS_WaitEvent(VOS_Event *pstVosEvent, LONG lTimeOut);
LONG VOS_SetEvent(VOS_Event *pstVosEvent);
LONG VOS_ReSetEvent(VOS_Event *pstVosEvent);
LONG VOS_DestroyEvent(VOS_Event *pstVosEvent );

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif // VOS_H_INCLUDE

