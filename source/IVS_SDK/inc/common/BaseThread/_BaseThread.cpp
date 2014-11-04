/***********************************************************
     Copyright, 2008 - 2010, Huawei Tech. co.,Ltd.
         All Rigths  Reserved
-------------------------------------------------------------
Project Code   :VRPV8
File name      :_BaseThread.cpp
Author         :邹清    z00003175
Description    :建立线程、退出线程
Function List  :
    _BaseThread
    ~_BaseThread
    ThreadFun
-------------------------------------------------------------
History
Date           Author         Modification
20100720       邹清           created file
*************************************************************/
#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */

#include "_BaseThread.h"
#include "BaseJob.h"
/************************************************************
Func Name    :ThreadFun
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :线程主循环
Input        :
    BaseJob* job  :Task对象指针
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
static unsigned int _stdcall ThreadFun(void *arg)
{
	BaseJob* job  = (BaseJob*)arg;
	if (NULL == job)
	{
		_endthreadex(0);
		//modify by limingjie lWX175483：2013-5-29  应该返回unsigned int
		return 1;
	}
    job->MainLoop();
	_endthreadex(0);
	return 0;
}


/************************************************************
Func Name    :_BaseThread
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :线程的创建
Input        :
    BaseJob* job  :Task对象指针
    int StackSize  :线程创建时的栈大小 
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
//##ModelId=4C0E087E015B
_BaseThread::_BaseThread(BaseJob* pBaseJob, int StackSize)
:m_pBaseJob (pBaseJob)
{
    m_hThread = (HANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)0,
                                (DWORD)StackSize,
                                 ThreadFun,
                                (LPVOID)pBaseJob,
                                0, NULL);
    if(m_hThread == NULL)
    {
        //记录日志

        return ;
    }
}

/************************************************************
Func Name    :~_BaseThread
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :线程对象的析构，将任务释放
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
//##ModelId=4C0E08980310
_BaseThread::~_BaseThread()
{
    try
    {
        m_pBaseJob->kill();

        if (m_hThread != NULL)
        {
            WaitForSingleObject(m_hThread, INFINITE);
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }
        // add by s00191067 成员变量未清空;
        m_pBaseJob = NULL;
        // end 
    }
    catch(...)
    {
         //IVS_LOG(IVS_LOG_ERR, "_BaseThread", "~_BaseThread() exception!\n");//lint !e1551
        //exit(1);
    }

}

