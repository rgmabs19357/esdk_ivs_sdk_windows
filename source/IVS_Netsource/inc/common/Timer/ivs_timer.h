/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  Timer.h
//  文件说明:   
//  版    本:   IVS V100R002C01
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/11/14
//  修改标识：
//  修改说明：
*******************************************************************************/

#ifndef __IVS_TIMER_H__
#define __IVS_TIMER_H__

//time_a after time_b
#define TIME_AFTER(time_a, time_b) ((long)(time_b) - (long)(time_a) < 0)

//time_a before time_b
#define TIME_BEFORE(time_a, time_b) TIME_AFTER(time_b, time_a)

int  IVS_StarTimer();
void IVS_StopTimer();

unsigned long  IVS_GetTimeTick();

extern unsigned long timeTick;

unsigned long long IVS_GetSysTickUs(void);

#endif
