/***********************************************************
     Copyright, 2008 - 2010, Huawei Tech. co.,Ltd.
         All Rigths  Reserved
-------------------------------------------------------------
Project Code   :VRPV8
File name      :BaseJob.cpp
Author         :邹清    z00003175
Description    :初始化与析构TASK
Function List  :
  BaseJob
  ~BaseJob
-------------------------------------------------------------
History
Date           Author         Modification
20100720       邹清           created file
*************************************************************/
// #include "stdafx.h"
#include "BaseJob.h"

/************************************************************
Func Name    :BaseJob
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :Task构造函数，主要完成对信号量的创建
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
//##ModelId=4C3C08450286
BaseJob::BaseJob()
:m_hMutex((HANDLE)0)
{
    m_hMutex = ::CreateMutex(NULL,FALSE,NULL);
}

/************************************************************
Func Name    :~BaseJob
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :Task析构函数，主要完成对信号量释放
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
//##ModelId=4C3C085A0063
BaseJob::~BaseJob()
{
    if (m_hMutex != NULL)
    {
        ::CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}

