/********************************************************************
 filename    :    AutoResumeTrigger.h
 author      :    g00209332
 created     :    2013/1/7
 description :    电视墙自动恢复触发器类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/1/7 初始版本
*********************************************************************/
#pragma once
#include "sdktimer.h"

class CTVWallMgr;//lint !e763
class CAutoResumeTrigger : public ITrigger
{
public:
    CAutoResumeTrigger(void);
    ~CAutoResumeTrigger(void);
    virtual void onTrigger(void * /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*enStyle*/);
	//设置电视墙管理对象
	void SetTVWallMgr(CTVWallMgr* pTVWallMgr);
private:
	CTVWallMgr* m_pTVWallMgr;		//电视墙管理对象
};
