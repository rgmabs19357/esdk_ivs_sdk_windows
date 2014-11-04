/******************************************************************************
   版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文件名          : CNVSTimer.cpp
  版本号          : 1.0
  作者            : duijiying 24362
  生成日期        : 2007-4-02
  最近修改        : 
  功能描述        : 
  函数列表        : 
  修改历史        : 
  1 日期          : 2007-4-02
    作者          : duijiying
    修改内容      : 生成
*******************************************************************************/
#include <stdarg.h>

#include "vos_config.h"
#include "vos_basetype.h"
#include "vos_common.h"
#include "vos.h"
#include "SDKTimer.h"
#include "bp_log.h"

ITimerLog *g_pTimerLog = NULL;

#ifndef WIN32   //过PC-LINT
#define TIMER_SECOND_IN_MS 1000
#define TIMER_MS_IN_US 1000
#endif

#define _TIMER_FL_ "CNVSTimer.cpp", __LINE__
/*******************************************************************************
  Function:       TIMER_WRITE_LOG()
  Description:    日志打印函数
  Calls:            
  Called By:      
  Input:          和printf一致
  Output:         无 
  Return:         无
*******************************************************************************/

void TIMER_WRITE_LOG(long lLevel, const char *format, ...)
{
    if(NULL == g_pTimerLog)
    {
        return;
    };
    
    char buff[MAX_TIMER_LOG_LENTH + 1];
    buff[0] = '\0';
    
    va_list args = NULL;
    va_start (args, format);
    long lPrefix = snprintf (buff, MAX_TIMER_LOG_LENTH, "errno:%d.thread(%u):", 
        errno, (long)VOS_pthread_self());
    if(lPrefix < MAX_TIMER_LOG_LENTH)
    {
        (void)vsnprintf (buff + lPrefix,
            (unsigned long)(MAX_TIMER_LOG_LENTH - lPrefix), format, args);
    }
    buff[MAX_TIMER_LOG_LENTH] = '\0';
    g_pTimerLog->writeLog(TIMER_RUN_LOG, lLevel, buff, (long)strlen(buff));
    va_end (args);
    
};

/*******************************************************************************
  Function:       CNVSTimer::~CNVSTimer()
  Description:    析构函数
  Calls:            
  Called By:      
  Input:          无
  Output:         无 
  Return:         无
*******************************************************************************/
CNVSTimer::~CNVSTimer()
{
    try
    {
        if(NULL != m_plistTrigger)
        {
            ListOfTriggerIte itListOfTrigger = m_plistTrigger->begin();
            TIMER_WRITE_LOG(TIMER_DEBUG, "FILE(%s)LINE(%d): CNVSTimer::~CNVSTimer: thread = %u", 
                _TIMER_FL_, VOS_pthread_self());
            while(itListOfTrigger != m_plistTrigger->end())
            {
                VOS_DELETE((*itListOfTrigger).second);
                ++itListOfTrigger;
            };
            m_plistTrigger->clear();
            VOS_DELETE(m_plistTrigger);
            m_plistTrigger = NULL;
        }
    
        if(m_pVosThread != NULL)
        {
            VOS_free(m_pVosThread);
        }
        m_pVosThread = NULL;

        if(m_pMutexListOfTrigger != NULL)
        {
            (void)VOS_DestroyMutex(m_pMutexListOfTrigger);
        }
        m_pMutexListOfTrigger = NULL;
    }
    catch (...)
    {
    }

};

/*******************************************************************************
  Function:       CNVSTimer::init()
  Description:    初始化函数
  Calls:            
  Called By:      
  Input:          ulTimerScale: 定时器精度
  Output:         无 
  Return:         
  VOS_SUCCESS: init success
  VOS_FAIL: init fail 
*******************************************************************************/
long CNVSTimer::init(ULONG ulTimerScale)
{
    //初始化时需设置为非退出状态，否则重复启停会失败
    m_bExit = VOS_FALSE;

    if (ulTimerScale < MinTimerScale)
    {
        m_ulTimerScale = MinTimerScale;
    }
    else
    {
        m_ulTimerScale = ulTimerScale;
    }

    m_pMutexListOfTrigger = VOS_CreateMutex();
    if(NULL == m_pMutexListOfTrigger)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "init falied","create m_pMutexListOfTrigger fail");
        return VOS_FAIL;
    }
    
    m_ullRrsAbsTimeScales = VOS_GetTicks()/m_ulTimerScale;

    (void)VOS_NEW( m_plistTrigger );
    if( NULL == m_plistTrigger )
    {
        if(m_pMutexListOfTrigger != NULL)
        {
            (void)VOS_DestroyMutex(m_pMutexListOfTrigger);
        }
        m_pMutexListOfTrigger = NULL;

		BP_RUN_LOG_ERR(IVS_FAIL, "m_plistTrigger is NULL","NA");
        return VOS_FAIL;
    }

    return VOS_SUCCESS;
};

/*******************************************************************************
  Function:       CNVSTimer::run()
  Description:    启动定时检测线程
  Calls:            
  Called By:      
  Input:          无 
  Output:         无 
  Return:         
  VOS_SUCCESS: init success
  VOS_FAIL: init fail 
*******************************************************************************/
long CNVSTimer::run()
{
    errno = 0;
    if (VOS_OK != VOS_CreateThread((VOS_THREAD_FUNC)invoke, (void *)this, 
                                    &m_pVosThread, VOS_DEFAULT_STACK_SIZE))
    {
        TIMER_WRITE_LOG(TIMER_ERROR, 
            "FILE(%s)LINE(%d): Create timer thread failed. error(%d):%s", 
            _TIMER_FL_, errno, strerror(errno));
        return VOS_FAIL;
    };
    TIMER_WRITE_LOG(TIMER_DEBUG, 
        "FILE(%s)LINE(%d): VOS_CreateThread: create timer thread(%d) OK.", 
        _TIMER_FL_, m_pVosThread->pthead);

    return VOS_SUCCESS;
};

/*******************************************************************************
  Function:       CNVSTimer::exit()
  Description:    退出定时检测线程
  Calls:            
  Called By:      
  Input:          无 
  Output:         无 
  Return:         无 
*******************************************************************************/
void CNVSTimer::exit()
{
    if(NULL == m_pVosThread)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "m_pVosThread is null","NA");
        return;
    }

    //先停止线程
    this->m_bExit = VOS_TRUE;
    
    errno = 0;   
    long ret_val = VOS_ThreadJoin(m_pVosThread);
    if (ret_val != VOS_OK)
    {
		BP_RUN_LOG_INF("Wait timer thread exit failed","NA");
    }

    //释放内存
    VOS_free(m_pVosThread);
    m_pVosThread = VOS_NULL;

    //再释放资源
    clearTimer();

    //释放锁
	//modify by zwx211831-zhuohaofan 判断条件错误
    if (NULL != m_pMutexListOfTrigger)
    {
        (void)VOS_DestroyMutex(m_pMutexListOfTrigger);
    }
    m_pMutexListOfTrigger = VOS_NULL;
  
    return;
};

/*******************************************************************************
  Function:       CNVSTimer::registerTimer()
  Description:    注册定时器
  Calls:            
  Called By:      
  Input:          pTrigger: 超时处理对象实例, void *pArg: 超时处理参数, 
                  nScales: 超时事件(以init时指定的scale为单位)
                  enStyle: 检测类型    enOneShot: 触发一次, enRepeated: 循环触发
  Output:         无 
  Return:         
  VOS_SUCCESS: init success
  VOS_FAIL: init fail 
*******************************************************************************/
long CNVSTimer::registerTimer(ITrigger *pTrigger, void *pArg, ULONG nScales, 
    TriggerStyle enStyle)
{
    if (NULL == pTrigger )
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "pTrigger is NULL", "NA");
        return VOS_FAIL;
    }

    if (0 == nScales )
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "nScales is zero", "NA");
        return VOS_FAIL;
    }    

    if(  VOS_TRUE == m_bExit)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, " thread exit", "NA");
        return VOS_FAIL;
    }
    
    CTimerItem *pTimerItem = NULL;
    (void)VOS_NEW(pTimerItem);
    if (NULL == pTimerItem )
    {
		BP_RUN_LOG_ERR(IVS_FAIL, " new pTimerItem fail", "NA");
        return VOS_FAIL;
    }
    
    pTrigger->m_pTimerItem = pTimerItem;

    pTimerItem->m_pTrigger = pTrigger;
    pTimerItem->m_pArg = pArg;
    pTimerItem->m_ulInitialScales = nScales;
    pTimerItem->m_ullCurScales = m_ullRrsAbsTimeScales + nScales;
    pTimerItem->m_enStyle = enStyle;
    //BEGIN V100R001C05 ADD 2010-02-03 ligengqiang l00124479 for 合入朱玉军解决的黄石解码器下线问题 NP-231 
    pTimerItem->m_ulBaseTimeTicks = VOS_GetTicks();
    //END   V100R001C05 ADD 2010-02-03 ligengqiang l00124479 for 合入朱玉军解决的黄石解码器下线问题 NP-231 

    //加锁(如果和mainloop不是同一线程不需要加锁)
    VOS_BOOLEAN bNeedLock = VOS_FALSE;
    VOS_BOOLEAN bLocked = VOS_FALSE;

	////消除simian代码冗余
	//if (NULL != m_pVosThread)
	//{
	//	if(VOS_pthread_self() != m_pVosThread->pthead)
	//	{
	//		bNeedLock = VOS_TRUE;
	//	}
	//}
	//else
	//{
	//	bNeedLock = VOS_TRUE;
	//}
    if (NULL == m_pVosThread)
    {
        bNeedLock = VOS_TRUE;
    }
    else
    {
        if(VOS_pthread_self() != m_pVosThread->pthead)
        {
            bNeedLock = VOS_TRUE;
        }
    }
    
    if(VOS_TRUE == bNeedLock)
    {
        if (VOS_OK != VOS_MutexLock(m_pMutexListOfTrigger))
        {
            BP_RUN_LOG_ERR(IVS_FAIL, " get lock failed", "NA");
        }
        else
        {
            bLocked = VOS_TRUE;
        }
    }
    //注册寄存器插入，这里统计代码重复的时候搞错了吧
    (void)(m_plistTrigger->insert(ListOfTriggerPair(pTimerItem->m_ullCurScales, pTimerItem)));

    //解锁
    if(VOS_TRUE == bLocked)
    {
        if (VOS_OK != VOS_MutexUnlock(m_pMutexListOfTrigger))
        {
            BP_RUN_LOG_ERR(IVS_FAIL, " release lock failed", "NA");
        }
    } 
    
    return VOS_SUCCESS;
};

/*******************************************************************************
  Function:       CNVSTimer::clearTimer
  Description:    删除定时器里面所有的TimerItem
  Calls:            
  Called By:      
  Input:         无 
  Output:         无 
  Return:         
*******************************************************************************/
void CNVSTimer::clearTimer( )
{
    CTimerItem *pTimerItem = NULL;

    if(VOS_OK != VOS_MutexLock(m_pMutexListOfTrigger))
    {
        return;
    };
	if(NULL==m_plistTrigger)
	{
		(void)VOS_MutexUnlock(m_pMutexListOfTrigger);
		return;
	}
    ListOfTriggerIte itListOfTrigger = m_plistTrigger->begin();
    ListOfTriggerIte itCurrentTrigger = m_plistTrigger->begin();
    while(itListOfTrigger != m_plistTrigger->end())
    {
        pTimerItem = (*itListOfTrigger).second;
        itCurrentTrigger = itListOfTrigger;
        ++itListOfTrigger;
        (void)(m_plistTrigger->erase(itCurrentTrigger));

        TIMER_WRITE_LOG(TIMER_DEBUG, 
            "FILE(%s)LINE(%d): clearTimer erase pTimerItem(0x%x) .\n",
                        _TIMER_FL_, pTimerItem);

		(void)VOS_DELETE(pTimerItem);//修复内存泄露
            
        continue;
    }

    VOS_DELETE(m_plistTrigger);
	m_plistTrigger = NULL;
    (void)VOS_MutexUnlock(m_pMutexListOfTrigger);
    
}

/*******************************************************************************
  Function:       CNVSTimer::cancelTimer()
  Description:    注销定时器
  Calls:            
  Called By:      
  Input:          pTrigger: 超时处理对象实例
  Output:         无 
  Return:         
  VOS_SUCCESS: init success
  VOS_FAIL: init fail 
*******************************************************************************/
long CNVSTimer::cancelTimer(ITrigger *pTrigger)
{
    if(NULL == pTrigger)
    {
        TIMER_WRITE_LOG(TIMER_ERROR, 
            "FILE(%s)LINE(%d): CNVSTimer::cancelTimer: pTrigger is NULL",
            _TIMER_FL_);
        return VOS_FAIL;
    };

    if(pTrigger->m_pTimerItem != NULL)
    {
        pTrigger->m_pTimerItem->m_bRemoved = VOS_TRUE;
        pTrigger->m_pTimerItem->m_pTrigger = NULL;
        pTrigger->m_pTimerItem = NULL;
    }

    //加锁(如果和mainloop不是同一线程不需要加锁)
    VOS_BOOLEAN bNeedLock = VOS_FALSE;
    VOS_BOOLEAN bLocked = VOS_FALSE;
    if (NULL == m_pVosThread)
    {
        bNeedLock = VOS_TRUE;
    }
    else
    {
        if(VOS_pthread_self() != m_pVosThread->pthead)
        {
            bNeedLock = VOS_TRUE;
        }
    }
    
    if(VOS_TRUE == bNeedLock)
    {
        if (VOS_OK != VOS_MutexLock(m_pMutexListOfTrigger))
        {
			BP_RUN_LOG_INF("cancelTimer set m_bRemoved=VOS_TRUE", "NA");
        }
        else
        {
            bLocked = VOS_TRUE;
        }
    }
 


	//IVS_LOG(LOG_INFO, "cancelTimer set m_bRemoved=VOS_TRUE");

    
    //解锁(如果不是同一线程)
    if(VOS_TRUE == bLocked)
    {
        if (VOS_OK != VOS_MutexUnlock(m_pMutexListOfTrigger))
        {
			BP_RUN_LOG_ERR(IVS_FAIL, "cancelTimer set m_bRemoved=VOS_TRUE", "NA");
        }
    }        
    
    return VOS_SUCCESS;
};

/*******************************************************************************
  Function:       CNVSTimer::mainLoop()
  Description:    线程执行的定时检测主循环
  Calls:            
  Called By:      
  Input:          无
  Output:         无 
  Return:         无
*******************************************************************************/
void CNVSTimer::mainLoop()
{
    ULONGLONG ullCurrentScales = 0;
    while(VOS_FALSE == m_bExit)
    {
#ifdef WIN32
        Sleep(m_ulTimerScale);    
#else
        struct timeval tv;
        tv.tv_sec = (long)(m_ulTimerScale/TIMER_SECOND_IN_MS);
        tv.tv_usec = (m_ulTimerScale%TIMER_SECOND_IN_MS)*TIMER_MS_IN_US;
        (void)select(0, VOS_NULL, VOS_NULL, VOS_NULL, &tv);
#endif        
        
        //加锁       
        CTimerItem *pTimerItem = NULL;
        ITrigger *pTrigger = NULL;
        ++m_ullRrsAbsTimeScales ;//内部时间基准增加一个刻度
        ullCurrentScales = m_ullRrsAbsTimeScales;

        if(VOS_OK != VOS_MutexLock(m_pMutexListOfTrigger))
        {
            break;
        };
        // modi by wKF71044 at [2012-8-16 11:33:58]指针判断空
        if (NULL == m_plistTrigger)
        {
			(void)VOS_MutexUnlock(m_pMutexListOfTrigger);
            continue;
        }
        ListOfTriggerIte itListOfTrigger = m_plistTrigger->begin();
        ListOfTriggerIte itCurrentTrigger = m_plistTrigger->begin();
        while(itListOfTrigger != m_plistTrigger->end())
        {
            pTimerItem = (*itListOfTrigger).second;
            if(NULL == pTimerItem)
            {
				//IVS_LOG(LOG_INFO, " pTimerItem is NULL.");
                itCurrentTrigger = itListOfTrigger;
                ++itListOfTrigger;
                (void)(m_plistTrigger->erase(itCurrentTrigger));
                continue;
            }
            
            pTrigger = (ITrigger *)pTimerItem->m_pTrigger;
            
            //如果定时器已注销，删除定时器，继续查找下一个
            if((NULL == pTrigger) || (VOS_TRUE == pTimerItem->m_bRemoved))
            {
				//IVS_LOG(LOG_INFO, "Timer removed.");
                itCurrentTrigger = itListOfTrigger;
                ++itListOfTrigger;
                (void)(m_plistTrigger->erase(itCurrentTrigger));
                VOS_DELETE(pTimerItem);
                continue;
            };

            //BEGIN V100R001C05 ADD 2010-02-03 ligengqiang l00124479 for 合入朱玉军解决的黄石解码器下线问题 NP-231 
            ULONG ulTimeTickInterval = VOS_GetTicks() - pTimerItem->m_ulBaseTimeTicks;
            if (ulTimeTickInterval < pTimerItem->m_ulInitialScales * m_ulTimerScale)
            {
                ++itListOfTrigger;
                continue;
            }
            pTimerItem->m_ulBaseTimeTicks = VOS_GetTicks();
            //END   V100R001C05 ADD 2010-02-03 ligengqiang l00124479 for 合入朱玉军解决的黄石解码器下线问题 NP-231 
            
            itCurrentTrigger = itListOfTrigger;
            ++itListOfTrigger;
            (void)(m_plistTrigger->erase(itCurrentTrigger));

            //触发trigger的onTrigger操作
            pTrigger->onTrigger(pTimerItem->m_pArg, 
                ullCurrentScales, pTimerItem->m_enStyle);

            //如果只触发一次，删除定时器
            if(enOneShot == pTimerItem->m_enStyle)
            {
				//IVS_LOG(LOG_INFO, " Timer remove trigger once timer.");
                //防止pTrigger->onTrigger后删除定时器
                pTrigger->m_pTimerItem = NULL;
                VOS_DELETE(pTimerItem);
                continue;
            }

            //对于重复触发定时器，需要修改下次超时事件，重新加入multimap
            pTimerItem->m_ullCurScales = ullCurrentScales + pTimerItem->m_ulInitialScales;

			if (NULL != m_plistTrigger )
			{
				itListOfTrigger = m_plistTrigger->insert( ListOfTriggerPair(pTimerItem->m_ullCurScales, pTimerItem));
			}
			
        };
        (void)VOS_MutexUnlock(m_pMutexListOfTrigger);
    }

    return;
}


