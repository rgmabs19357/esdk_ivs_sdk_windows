#include "CumwCommon.h"
// #include "LockGuard.h"

CLockGuard::CLockGuard(VOS_Mutex *pMutex)
{
    m_pMutex = NULL;

    if(NULL == pMutex)
    {
		IVS_RUN_LOG_ERR("mutex is null");
        return;
    }

    m_pMutex = pMutex;

    (void)VOS_MutexLock(m_pMutex);
}

CLockGuard::~CLockGuard()
{
    if(NULL == m_pMutex)
    {
		IVS_RUN_LOG_ERR("mutex is null");
        return;
    }
    (void)VOS_MutexUnlock(m_pMutex);
    
    m_pMutex = NULL;
}

/*******************************************************************************
Function:       // CLockGuard::lock
Description:    // 加锁
Calls:          // 
Data Accessed:  // 
Data Updated:   // 
Input:          // VOS_Mutex *pMutex
Output:         // 无
Return:         // 无
Others:         // 无
*******************************************************************************/
void CLockGuard::lock(VOS_Mutex *pMutex)
{
    if(NULL == pMutex)
    {
		IVS_RUN_LOG_ERR("mutex is null");
        return;
    }
    (void)VOS_MutexLock(pMutex);
}

/*******************************************************************************
Function:       // CLockGuard::unlock
Description:    // 释放锁
Calls:          // 
Data Accessed:  // 
Data Updated:   // 
Input:          // VOS_Mutex *pMutex
Output:         // 无
Return:         // 无
Others:         // 无
*******************************************************************************/
void CLockGuard::unlock(VOS_Mutex *pMutex)
{
    if(NULL == pMutex)
    {
		IVS_RUN_LOG_ERR("mutex is null");
        return;
    }
    (void)VOS_MutexUnlock(pMutex);
}

