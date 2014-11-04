#ifndef __CCRITICAL_SECTIONEX_H__
#define __CCRITICAL_SECTIONEX_H__

#include "vos.h"

//定义自己的临界区类，直接在init中使用CCRITICAL_SECTION将不可避免多次初始化;
class CCRITICAL_SECTIONEX
{
public:
	CCRITICAL_SECTIONEX();
	virtual ~CCRITICAL_SECTIONEX();
	void EnterCriticalSectionEx();
	void LeaveCriticalSectionEx();
protected:

	VOS_Mutex *m_pMutex;
};

#endif