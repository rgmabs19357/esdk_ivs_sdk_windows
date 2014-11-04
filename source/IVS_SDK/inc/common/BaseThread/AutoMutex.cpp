/***********************************************************
     Copyright, 2008 - 2010, Huawei Tech. co.,Ltd.
         All Rigths  Reserved
-------------------------------------------------------------
Project Code   :VRPV8
File name      :Decoder.cpp
Author         :邹清    z00003175
Description    :定义个自动互斥量
Function List  :
   AutoMutex
   ~AutoMutex
-------------------------------------------------------------
History
Date           Author         Modification
20100720       邹清           created file
*************************************************************/
// #include "StdAfx.h"
#include "AutoMutex.h"

/************************************************************
Func Name    :AutoMutex
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :构造函数,将输入的互斥量句柄保存,并进入互斥量
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
AutoMutex::AutoMutex(HANDLE hMutex)
:m_hMutex(hMutex)
{
    if (NULL == m_hMutex)
    {
        return;
    }

    ::WaitForSingleObject(m_hMutex,INFINITE);
}

/************************************************************
Func Name    :~AutoMutex
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :析构函数,释放互斥量
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
AutoMutex::~AutoMutex(void)
{
    if (m_hMutex)
    {
        ::ReleaseMutex(m_hMutex);
        m_hMutex = NULL;
    }
}
