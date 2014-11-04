/************************************************************
     Copyright, 2008 - 2010, Huawei Tech. co.,Ltd.
         All Rigths  Reserved
-------------------------------------------------------------
Project Code   :VRPV8
File name      :BaseJbo.h
Author         :邹清    z00003175
Description    :需要建立线程TASK时，加入该头文件
-------------------------------------------------------------
History
Date           Author         Modification
20100720       邹清           created file
*************************************************************/

#ifndef BASEJOB_H_HEADER_INCLUDED_B3F0ABAE
#define BASEJOB_H_HEADER_INCLUDED_B3F0ABAE

#include <WTypes.h>

//##ModelId=4C0E09D9011A
class BaseJob
{
  public:

      //工作状态定义
      //created --- 工作刚创建，但是工作并为真正开始，可以看作为线程挂起
      //running  --- 工作正常工作状态
      //finished --- 工作完成状态
      enum JOB_STATE
      {
          created,
          running,
          finished
      };
/************************************************************
Func Name    :MainLoop
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :工作主循环接口定义，里面完成工作循环逻辑
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
    //##ModelId=4C0E09F1038B
    virtual void MainLoop() = 0;
/************************************************************
Func Name    :kill
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :停止工作循环接口定义，将工作停下来
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
    //##ModelId=4C0E0A5003A9
    virtual void kill() = 0;

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
    BaseJob();
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
    virtual ~BaseJob();

protected:
    HANDLE m_hMutex;

};



#endif /* BASEJOB_H_HEADER_INCLUDED_B3F0ABAE */
