// #include "StdAfx.h"
#include "CriticalSectionEx.h"

CCRITICAL_SECTIONEX::CCRITICAL_SECTIONEX()
{
	m_pMutex = VOS_CreateMutex();
	// ::InitializeCriticalSection(&m_CriticalSectionEx);
}
CCRITICAL_SECTIONEX::~CCRITICAL_SECTIONEX()
{
	VOS_DestroyMutex(m_pMutex);
	// ::DeleteCriticalSection(&m_CriticalSectionEx);
}
void CCRITICAL_SECTIONEX::EnterCriticalSectionEx()
{
	(void)VOS_MutexLock(m_pMutex);
	// ::EnterCriticalSection(&m_CriticalSectionEx);
}
void CCRITICAL_SECTIONEX::LeaveCriticalSectionEx()
{
	(void)VOS_MutexUnlock(m_pMutex);
	// ::LeaveCriticalSection(&m_CriticalSectionEx);
}