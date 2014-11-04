#include "ivs_timer.h"
#include "ivs_error.h"
#include "ivs_error.h"
#include <Windows.h>
#include "ivs_log.h"
#pragma comment(lib,"Winmm.lib")

#define TARGET_RESOLUTION 1         // 1-millisecond target resolution

#define TIMER_ACCURACY 2

int timerID;

//距离10分钟发生翻转
unsigned long timeTick = 0xFFFF159F;

static void CALLBACK  TimerCallBack(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    timeTick += TIMER_ACCURACY;
}

int  IVS_StarTimer()
{
    TIMECAPS tc;

    UINT wTimerRes;

    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
    {
        return IVS_FAIL;
    }

    wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);

    timeBeginPeriod(wTimerRes);

    timerID = timeSetEvent(TIMER_ACCURACY, wTimerRes, TimerCallBack, NULL, TIME_PERIODIC);
    if (!timerID)
    {
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

void  IVS_StopTimer()
{
    if (timerID)
    {
        // is timer event pending?
        timeKillEvent(timerID);  // cancel the event
        timerID = 0;
    }
}

unsigned long  IVS_GetTimeTick()
{
    return timeTick;
}

unsigned long long IVS_GetSysTickUs(void)
{
	LARGE_INTEGER counter, freq;
	QueryPerformanceCounter(&counter);
	QueryPerformanceFrequency(&freq);

	lldiv_t d = lldiv (counter.QuadPart, freq.QuadPart);
	return (d.quot * 1000000) + ((d.rem * 1000000) / freq.QuadPart);
}