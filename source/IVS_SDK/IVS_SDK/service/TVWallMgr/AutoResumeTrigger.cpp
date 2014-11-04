#include "AutoResumeTrigger.h"
#include "TVWallMgr.h"

CAutoResumeTrigger::CAutoResumeTrigger(void) : m_pTVWallMgr(NULL)
{
}


CAutoResumeTrigger::~CAutoResumeTrigger(void)
{
	m_pTVWallMgr = NULL;
}

void CAutoResumeTrigger::onTrigger(void * /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*enStyle*/)
{
	CHECK_POINTER_VOID(m_pTVWallMgr);
    m_pTVWallMgr->HandleAutoResume();

    m_pTVWallMgr->DelIdleCh();
}

void CAutoResumeTrigger::SetTVWallMgr(CTVWallMgr* pTVWallMgr)
{
	m_pTVWallMgr = pTVWallMgr;
}
