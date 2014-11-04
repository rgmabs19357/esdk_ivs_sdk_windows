/********************************************************************
 filename    :    LockGuard.h
 author      :    g00209332
 created     :    2012/12/19
 description :    ÖÇÄÜËø
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 ´ÓC30¼Ì³Ð
*********************************************************************/

#ifndef CLOCKGUARD_H_INCLUDE
#define CLOCKGUARD_H_INCLUDE    

#include "vos.h"

class CLockGuard
{
  public:
    CLockGuard(VOS_Mutex *pMutex);
    ~CLockGuard();
    
  public:
    static void lock(VOS_Mutex *pMutex);
    static void unlock(VOS_Mutex *pMutex);
    
 private:
    CLockGuard();
    VOS_Mutex *m_pMutex;
};

#endif // CLOCKGUARD_H_INCLUDE


